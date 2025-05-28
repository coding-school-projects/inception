#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

void Server::handleWhois(int clientFd, const std::vector<std::string>& args) {
    if (args.empty()) {
        sendToClient(clientFd, "431 :No nickname given");
        return;
    }

    const std::string& targetNick = args[0];
    Client& requester = _clients[clientFd];
    std::string response;

    for (const auto& [fd, client] : _clients) {
        if (client.getNickname() == targetNick) {
            response = "311 " + requester.getNickname() + " " + client.getNickname() + " " +
                      client.getUsername() + " " + client.getClientIP() + " * :" +
                      client.getRealName() + "\r\n";

            // Add channel list
            std::string channels;
            for (const auto& [chanName, channel] : _channels) {
                if (channel.hasClient(fd)) {
                    channels += (channel.isOperator(fd) ? "@" : "") + chanName + " ";
                }
            }
            if (!channels.empty()) {
                response += "319 " + requester.getNickname() + " " + client.getNickname() +
                           " :" + channels + "\r\n";
            }

            response += "318 " + requester.getNickname() + " " + client.getNickname() +
                       " :End of WHOIS list";
            sendToClient(clientFd, response);
            return;
        }
    }

    sendToClient(clientFd, "401 " + requester.getNickname() + " " + targetNick + " :No such nick");
}