#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

// Handle INVITE command - invite user to channel
void Server::handleInvite(int clientFd, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        sendToClient(clientFd, "461 INVITE :Not enough parameters");
        return;
    }

    Client& inviter = _clients[clientFd];
    const std::string& inviterNick = inviter.getNickname();
    const std::string& targetNick = args[0];
    const std::string& channelName = args[1];

    // Check if channel exists
    if (_channels.find(channelName) == _channels.end()) {
        sendToClient(clientFd, "403 " + inviterNick + " " + channelName + " :No such channel");
        return;
    }

    Channel& channel = _channels[channelName];

    // Check if inviter is in channel
    if (!channel.hasClient(clientFd)) {
        sendToClient(clientFd, "442 " + inviterNick + " " + channelName + " :You're not on that channel");
        return;
    }

    // Check if inviter is operator (if channel is invite-only)
    if (channel.isInviteOnly() && !channel.isOperator(clientFd)) {
        sendToClient(clientFd, "482 " + inviterNick + " " + channelName + " :You're not channel operator");
        return;
    }

    // Find target client
    int targetFd = -1;
    for (const auto& [fd, client] : _clients) {
        if (client.getNickname() == targetNick) {
            targetFd = fd;
            break;
        }
    }

    if (targetFd == -1) {
        sendToClient(clientFd, "401 " + inviterNick + " " + targetNick + " :No such nick");
        return;
    }

    // Check if target is already in channel
    if (channel.hasClient(targetFd)) {
        sendToClient(clientFd, "443 " + inviterNick + " " + targetNick + " " + channelName + 
                       " :is already on channel");
        return;
    }

    // Add to invited list
    channel.addInvited(targetFd);

    // Send invite notification to target
    std::string inviteMsg = ":" + inviterNick + "!" + inviter.getUsername() + "@localhost INVITE " + 
                           targetNick + " " + channelName;
    sendToClient(targetFd, inviteMsg);

    // Send success response to inviter
    sendToClient(clientFd, "341 " + inviterNick + " " + targetNick + " " + channelName);

    std::cout << inviterNick << " invited " << targetNick << " to " << channelName << std::endl;
}