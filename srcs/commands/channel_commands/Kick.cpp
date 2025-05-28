#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

// Handle KICK command - eject user from channel
void Server::handleKick(int clientFd, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        sendToClient(clientFd, "461 KICK :Not enough parameters");
        return;
    }

    Client& kicker = _clients[clientFd];
    const std::string& kickerNick = kicker.getNickname();
    const std::string& channelName = args[0];
    const std::string& targetNick = args[1];
    std::string reason = "No reason given";
    if (args.size() > 2) {
        reason = args[2];
        for (size_t i = 3; i < args.size(); ++i) {
            reason += " " + args[i];
        }
        // Remove leading colon if present
        if (!reason.empty() && reason[0] == ':')
            reason = reason.substr(1);
    }

    // Check if channel exists
    if (_channels.find(channelName) == _channels.end()) {
        sendToClient(clientFd, "403 " + kickerNick + " " + channelName + " :No such channel");
        return;
    }

    Channel& channel = _channels[channelName];

    // Check if kicker is in channel
    if (!channel.hasClient(clientFd)) {
        sendToClient(clientFd, "442 " + kickerNick + " " + channelName + " :You're not on that channel");
        return;
    }

    // Check if kicker is operator
    if (!channel.isOperator(clientFd)) {
        sendToClient(clientFd, "482 " + channelName + " :You're not channel operator");
        // sendToClient(clientFd, "482 " + kickerNick + " " + channelName + " :You're not channel operator\r\n");
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
        sendToClient(clientFd, "401 " + kickerNick + " " + targetNick + " :No such nick");
        return;
    }

    // Check if target is in channel
    if (!channel.hasClient(targetFd)) {
        sendToClient(clientFd, "441 " + kickerNick + " " + targetNick + " " + channelName + 
                       " :They aren't on that channel");
        return;
    }

    // Format kick message
    std::string kickMsg = ":" + kickerNick + "!" + kicker.getUsername() + "@localhost KICK " + 
                         channelName + " " + targetNick + " :" + reason;

    // Send to channel (including target)
    sendToChannel(channelName, kickMsg);

    // Remove target from channel
    channel.removeClient(targetFd);
    channel.removeOperator(targetFd);
    channel.removeInvited(targetFd);

    std::cout << kickerNick << " kicked " << targetNick << " from " << channelName << std::endl;
}