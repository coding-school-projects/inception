#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

// Handle TOPIC command - view or change channel topic
void Server::handleTopic(int clientFd, const std::vector<std::string>& args) {
    if (args.empty()) {
        sendToClient(clientFd, "461 TOPIC :Not enough parameters");
        return;
    }

    Client& requester = _clients[clientFd];
    const std::string& nick = requester.getNickname();
    const std::string& channelName = args[0];

    // Check if channel exists
    if (_channels.find(channelName) == _channels.end()) {
        sendToClient(clientFd, "403 " + nick + " " + channelName + " :No such channel");
        return;
    }

    Channel& channel = _channels[channelName];

    // Check if client is in channel
    if (!channel.hasClient(clientFd)) {
        sendToClient(clientFd, "442 " + nick + " " + channelName + " :You're not on that channel");
        return;
    }

    // If no topic provided, return current topic
    if (args.size() == 1) {
        if (channel.getTopic().empty()) {
            sendToClient(clientFd, "331 " + nick + " " + channelName + " :No topic is set");
        } else {
            sendToClient(clientFd, "332 " + nick + " " + channelName + " :" + channel.getTopic());
        }
        return;
    }

    // Changing topic
    // Check if topic is restricted to operators
    if (channel.isTopicRestricted() && !channel.isOperator(clientFd)) {
        sendToClient(clientFd, "482 " + nick + " " + channelName + " :You're not channel operator");
        return;
    }

    // Combine all arguments after channel name as the new topic
    std::string newTopic = args[1];
    if (args.size() > 2) {
        for (size_t i = 2; i < args.size(); ++i) {
            newTopic += " " + args[i];
        }
    }

    // Remove leading : if present
    if (!newTopic.empty() && newTopic[0] == ':') {
        newTopic = newTopic.substr(1);
    }

    // Set new topic
    channel.setTopic(newTopic);

    // Notify channel members
    std::string topicMsg = ":" + nick + "!" + requester.getUsername() + "@localhost TOPIC " + 
                          channelName + " :" + newTopic;
    sendToChannel(channelName, topicMsg);

    std::cout << nick << " changed topic of " << channelName << " to: " << newTopic << std::endl;
}