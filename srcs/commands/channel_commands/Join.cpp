#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

void Server::handleJoin(int clientFd, const std::vector<std::string>& args) {
    Client& client = _clients[clientFd];
    
    // Check registration
    if (!client.isRegistered()) {
        sendToClient(clientFd, ":" + _serverName + " 451 :You have not registered");
        return;
    }

    // Check parameters
    if (args.empty()) {
        sendToClient(clientFd, ":" + _serverName + " 461 " + client.getNickname() + " JOIN :Not enough parameters");
        return;
    }

    std::string channelName = args[0];
    std::string key = args.size() > 1 ? args[1] : "";

    // Validate channel name
    if (channelName.empty() || channelName[0] != '#') {
        sendToClient(clientFd, ":" + _serverName + " 403 " + client.getNickname() + " " + channelName + " :Invalid channel name");
        return;
    }

    // Check channel name length
    if (channelName.length() > 50) {
        sendToClient(clientFd, ":" + _serverName + " 403 " + client.getNickname() + " " + channelName + " :Channel name too long");
        return;
    }

    // Create channel if it doesn't exist
    if (_channels.find(channelName) == _channels.end()) {
        _channels[channelName] = Channel(channelName);
        _channels[channelName].setCreationTime(std::time(NULL));
        _channels[channelName].addOperator(clientFd);
        std::cout << "Client " << client.getNickname() 
                  << " created and became operator of " << channelName << std::endl;
    }

    Channel& channel = _channels[channelName];

    // Check if already in channel
    if (channel.hasClient(clientFd)) {
        return;
    }

    // Check channel key
    if (!channel.getKey().empty() && channel.getKey() != key) {
        sendToClient(clientFd, ":" + _serverName + " 475 " + client.getNickname() + " " + channelName + " :Cannot join channel (+k)");
        return;
    }

    // Check invite-only
    if (channel.isInviteOnly() && !channel.isInvited(clientFd) && !channel.isOperator(clientFd)) {
        sendToClient(clientFd, ":" + _serverName + " 473 " + client.getNickname() + " " + channelName + " :Cannot join channel (+i)");
        return;
    }

    // Check user limit
    if (channel.getUserLimit() > 0 && channel.getClients().size() >= static_cast<size_t>(channel.getUserLimit())) {
        sendToClient(clientFd, ":" + _serverName + " 471 " + client.getNickname() + " " + channelName + " :Cannot join channel (+l)");
        return;
    }

    // Add client to channel
    channel.addClient(clientFd);
    channel.removeInvited(clientFd);

    // Send JOIN message to all channel members
    std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getClientIP() + " JOIN :" + channelName;
    sendToChannel(channelName, joinMsg);

    // Send channel information to the new member
    sendChannelInfo(clientFd, channelName);

    std::cout << client.getNickname() << " joined " << channelName << std::endl;
}

void Server::sendChannelInfo(int clientFd, const std::string& channelName) {
    Channel& channel = _channels[channelName];
    Client& client = _clients[clientFd];
    const std::string& nick = client.getNickname();

    // Send RPL_TOPIC or RPL_NOTOPIC
    if (channel.getTopic().empty()) {
        sendToClient(clientFd, ":" + _serverName + " 331 " + nick + " " + channelName + " :No topic is set");
    } else {
        sendToClient(clientFd, ":" + _serverName + " 332 " + nick + " " + channelName + " :" + channel.getTopic());
        sendToClient(clientFd, ":" + _serverName + " 333 " + nick + " " + channelName + " " +
            channel.getTopicSetter() + " " + std::to_string(channel.getTopicTimestamp()));
    }

    // Send RPL_NAMREPLY
    std::string namesList = ":" + _serverName + " 353 " + nick + " = " + channelName + " :";
    const std::vector<int>& members = channel.getClients();
    for (size_t i = 0; i < members.size(); ++i) {
        if (channel.isOperator(members[i])) {
            namesList += "@";
        }
        namesList += _clients[members[i]].getNickname();
        if (i != members.size() - 1) {
            namesList += " ";
        }
    }
    sendToClient(clientFd, namesList);

    // Send RPL_ENDOFNAMES
    sendToClient(clientFd, ":" + _serverName + " 366 " + nick + " " + channelName + " :End of /NAMES list");

    // Send RPL_CHANNELMODEIS
    std::string modes = "+";
    if (channel.isInviteOnly()) modes += "i";
    if (channel.isTopicRestricted()) modes += "t";
    if (!channel.getKey().empty()) modes += "k";
    if (channel.getUserLimit() > 0) modes += "l";
    
    if (modes != "+") {
        sendToClient(clientFd, ":" + _serverName + " 324 " + nick + " " + channelName + " " + modes);
    }

    // Send RPL_CREATIONTIME
    sendToClient(clientFd, ":" + _serverName + " 329 " + nick + " " + channelName + " " + 
               std::to_string(channel.getCreationTime()));
}