#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

void Server::handleWho(int clientFd, const std::vector<std::string>& args) {
    if (args.empty()) {
        sendToClient(clientFd, "461 WHO :Not enough parameters");
        return;
    }

    const std::string& target = args[0];
    Client& requester = _clients[clientFd];
    std::string response;

    // WHO for channels
    if (target[0] == '#' || target[0] == '&') {
        if (_channels.find(target) == _channels.end()) {
            sendToClient(clientFd, "315 " + requester.getNickname() + " " + target + " :End of WHO list");
            return;
        }

        const Channel& channel = _channels[target];
        for (int memberFd : channel.getClients()) {
            const Client& member = _clients[memberFd];
            response += "352 " + requester.getNickname() + " " + target + " " +
                       member.getUsername() + " " + member.getClientIP() + " ft_irc " +
                       member.getNickname() + " H :0 " + member.getRealName()+ "\r\n";
        }
    } 
    // WHO for users
    else {
        for (const auto& [fd, client] : _clients) {
            if (client.getNickname() == target) {
                response += "352 " + requester.getNickname() + " * " +
                           client.getUsername() + " " + client.getClientIP() + " ft_irc " +
                           client.getNickname() + " H :0 " + client.getRealName() + "\r\n";
                break;
            }
        }
    }

    response += "315 " + requester.getNickname() + " " + target + " :End of WHO list";
    sendToClient(clientFd, response);
}