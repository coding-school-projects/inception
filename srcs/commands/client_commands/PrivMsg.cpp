#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

// Handle PRIVMSG command - send private message to user or channel
void Server::handlePrivMsg(int clientFd, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        sendToClient(clientFd, "461 PRIVMSG :Not enough parameters");
        return;
    }

    Client& sender = _clients[clientFd];
    const std::string& senderNick = sender.getNickname();
    const std::string& target = args[0];
    std::string message = args[1];

    // Combine all arguments after the first one as the message
    if (args.size() > 2) {
        for (size_t i = 2; i < args.size(); ++i) {
            message += " " + args[i];
        }
    }

    // Check if message starts with : (IRC protocol allows this)
    if (!message.empty() && message[0] == ':') {
        message = message.substr(1);
    }

    // Check if target is a channel
    if (target[0] == '#' || target[0] == '&') {
        // Channel message
        if (_channels.find(target) == _channels.end()) {
            sendToClient(clientFd, "403 " + senderNick + " " + target + " :No such channel");
            return;
        }

        Channel& channel = _channels[target];
        if (!channel.hasClient(clientFd)) {
            sendToClient(clientFd, "404 " + senderNick + " " + target + " :Cannot send to channel");
            return;
        }

        // Format message according to IRC protocol
        std::string formattedMsg = ":" + senderNick + "!" + sender.getUsername() + "@localhost PRIVMSG " + 
                                  target + " :" + message;
        sendToChannel(target, formattedMsg, clientFd);
    } else {
        // User message
        int targetFd = -1;
        for (const auto& [fd, client] : _clients) {
            if (client.getNickname() == target) {
                targetFd = fd;
                break;
            }
        }

        if (targetFd == -1) {
            sendToClient(clientFd, "401 " + senderNick + " " + target + " :No such nick");
            return;
        }

        std::string formattedMsg = ":" + senderNick + "!" + sender.getUsername() + "@localhost PRIVMSG " + 
                                 target + " :" + message;
        sendToClient(targetFd, formattedMsg);
    }
}