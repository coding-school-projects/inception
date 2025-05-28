#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

// Handle NICK command - set or change nickname
void Server::handleNick(int clientFd, const std::vector<std::string>& args) {
    if (_clients[clientFd].getRegState() < Client::PASS_RECEIVED) {
        sendToClient(clientFd, "464 * :Password required", true); 
        return;
    }

    if (args.empty()) {
        if (!_clients[clientFd].getNickname().empty()) {
            return;
        }  
        sendToClient(clientFd, "431 * :No nickname given", true);
        return;
    }

    const std::string& newNick = args[0];

    if (!Client::isValidNickname(newNick)) {
        sendToClient(clientFd, "432 * " + newNick + " :Erroneous nickname", true);
        return;
    }

    for (const auto& client : _clients) {
        if (client.second.getNickname() == newNick && client.first != clientFd) {
            sendToClient(clientFd, "433 * " + newNick + " :Nickname is already in use", true); 
            return;
        }
    }

    if (_clients[clientFd].getNickname() == newNick) {
        return;
    }

    if (!_clients[clientFd].getNickname().empty()) {
        std::string nickChange = ":" + _clients[clientFd].getNickname() + " NICK :" + newNick;
        sendToClient(clientFd, nickChange);

        for (auto& channelPair : _channels) {
            Channel& channel = channelPair.second;
            if (std::find(channel.getClients().begin(), channel.getClients().end(), clientFd) != channel.getClients().end()) {
                sendToChannel(channelPair.first, nickChange);
            }
        }
    }

    _clients[clientFd].setNickname(newNick);
    _clients[clientFd].setRegState(Client::NICK_RECEIVED);
}
