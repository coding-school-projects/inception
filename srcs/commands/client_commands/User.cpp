#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

void Server::handleUser(int clientFd, const std::vector<std::string>& args) {
    Client& client = _clients[clientFd];
    const std::string currentNick = client.getNickname().empty() ? "*" : client.getNickname();

    // 1. Prevent re-registration
    if (client.isRegistered()) {
        sendToClient(clientFd, "462 " + currentNick + " :You may not reregister", true);
        return;
    }

    // 2. Require PASS first
    if (client.getRegState() < Client::PASS_RECEIVED) {
        sendToClient(clientFd, "464 * :Password required", true);
        return;
    }

    // 3. Check arguments count
    if (args.size() < 4) {
        sendToClient(clientFd, "461 " + currentNick + " USER :Not enough parameters", true);
        return;
    }

    // 4. Require NICK before USER
    if (client.getRegState() < Client::NICK_RECEIVED) {
        sendToClient(clientFd, "451 " + currentNick + " :You have not registered", true);
        return;
    }

    // 5. Validate username
    if (!Client::isValidUsername(args[0])) {
        sendToClient(clientFd, "461 " + currentNick + " USER :Invalid username", true);
        return;
    }

    // 6. Set username and realname
    const std::string& username = args[0];
    std::string realname;
    if (args[3][0] == ':') {
        realname = args[3].substr(1);
        for (size_t i = 4; i < args.size(); ++i)
            realname += " " + args[i];
    } else {
        realname = args[3];
    }

    client.setUsername(username);
    client.setRealName(realname);
    client.setRegState(Client::USER_RECEIVED);

    // 7. Authenticate and send welcome if ready
    if (client.isRegistered()) {
        client.authenticate();
        sendWelcomeMessages(clientFd);

        std::cout << "Client " << clientFd << " (" << currentNick << ") registered with username '" << username 
                  << "' and realname '" << realname << "'\n";
        std::cout << GREEN_B << "Client " << clientFd << " successfully registered" << RESET << std::endl;
    }
}
