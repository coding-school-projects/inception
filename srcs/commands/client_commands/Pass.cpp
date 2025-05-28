#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

void Server::handlePass(int clientFd, const std::vector<std::string>& args) {
    Client& client = _clients[clientFd];

    if (client.isRegistered()) {
        sendToClient(clientFd, "462 " + client.getNickname() + " :You may not reregister", true);
        return;
    }

    if (client.getRegState() >= Client::PASS_RECEIVED) {
        return;
    }

    if (args.empty()) {
        sendToClient(clientFd, "461 PASS :Not enough parameters", true);
        return;
    }

    if (args[0] != _password) {
        sendToClient(clientFd, "464 :Password incorrect - try again", true);
        return;
    }

    client.setRegState(Client::PASS_RECEIVED);
    std::cout << "Client " << clientFd << " passed authentication\n";
}
