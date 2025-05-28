#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

// Handle PART command - leave channel
void Server::handlePart(int clientFd, const std::vector<std::string>& args) {
    if (args.empty()) {
        sendToClient(clientFd, ":ft_irc 461 " + _clients[clientFd].getNickname() + 
                          " PART :Not enough parameters");
        return;
    }

    Client& client = _clients[clientFd];
    const std::string& nick = client.getNickname();
    std::string channelName = args[0];
    
    // Handle reason (combine all arguments after channel name)
    std::string reason;
    if (args.size() > 1) {
        reason = args[1];
        // If reason starts with :, include all remaining text
        if (reason[0] == ':') {
            reason = reason.substr(1);
            for (size_t i = 2; i < args.size(); ++i) {
                reason += " " + args[i];
            }
        }
    } else {
        reason = "Leaving";
    }

    // Check channel exists
    if (_channels.find(channelName) == _channels.end()) {
        sendToClient(clientFd, ":ft_irc 403 " + nick + " " + channelName + 
                          " :No such channel");
        return;
    }

    Channel& channel = _channels[channelName];
    
    // Check if user is in channel using your existing getClients()
    const std::vector<int>& members = channel.getClients();
    if (std::find(members.begin(), members.end(), clientFd) == members.end()) {
        sendToClient(clientFd, ":ft_irc 442 " + nick + " " + channelName + 
                          " :You're not on that channel");
        return;
    }

    // Notify channel (except the parting client)
    std::string partMsg = ":" + nick + "!" + client.getUsername() + "@127.0.0.1 PART " + 
                         channelName;
    sendToChannel(channelName, partMsg);

    // Remove from channel structures
    channel.removeClient(clientFd);
    channel.removeOperator(clientFd);
    channel.removeInvited(clientFd);

    // Clean up empty channels
    if (channel.getClients().empty()) {
        _channels.erase(channelName);
        std::cout << YELLOW << "Channel " << channelName << " removed (last user left)" << RESET << std::endl;
    }

    // Send confirmation to client
    // sendToClient(clientFd, partMsg);
    std::cout << "Sending:" << partMsg << std::endl;
}
