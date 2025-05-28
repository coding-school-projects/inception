#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

// Handle NAMES command - list users/users in channel
void Server::handleNames(int clientFd, const std::vector<std::string>& args, bool listAll) {
    // Check if client exists
    if (_clients.find(clientFd) == _clients.end()) {
        return;
    }

    Client& client = _clients[clientFd];
    const std::string& nick = client.getNickname();

    // If no arguments or listAll is true, show all visible users on server
    if (listAll) {
        // Server-wide user listing
        std::string reply = ":ft_irc 353 " + nick + " * * :";
        bool first = true;

        // // List all registered, visible users
        // for (const auto& [fd, client] : _clients) {
        //     if (client.isRegistered() && !client.isInvisible()) {
        //         if (!first) reply += " ";
        //         reply += client.getNickname();
        //         first = false;
        //     }
        // }

        // List all registered, visible users
        for (const auto& [fd, client] : _clients) {
            if (client.isRegistered()) {
                if (!first) reply += " ";
                reply += client.getNickname();
                first = false;
            }
        }

        // reply += "\r\n";
        sendToClient(clientFd, reply);
        sendToClient(clientFd, ":ft_irc 366 " + nick + " * :End of /NAMES list");
        return;
    }

    // Show users in specific channel
    std::string channelName = args[0];

     // Ensure channel name starts with #
    if (channelName[0] != '#' && channelName[0] != '&') {
        channelName = "#" + channelName;
    }

    // Check if channel exists
    if (_channels.find(channelName) == _channels.end()) {
        sendToClient(clientFd, ":ft_irc 366 " + nick + " " + channelName + " :End of /NAMES list");
        return;
    }

    Channel& channel = _channels[channelName];
    const std::vector<int>& members = channel.getClients();

    // Build NAMES list response
    std::string namesList = ":ft_irc 353 " + nick + " = " + channelName + " :";
    bool first = true;

    for (int memberFd : members) {
        if (_clients.find(memberFd) == _clients.end()) continue;
        
        const Client& member = _clients[memberFd];
        
        if (!first) namesList += " ";
        if (channel.isOperator(memberFd)) {
            namesList += "@";
        }
        namesList += member.getNickname();
        first = false;
    }

    // namesList += "\r\n";
    sendToClient(clientFd, namesList);
    sendToClient(clientFd, ":ft_irc 366 " + nick + " " + channelName + " :End of /NAMES list");
}