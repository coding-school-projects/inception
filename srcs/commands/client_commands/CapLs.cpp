#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

void Server::handleCapLs(int clientFd, const std::vector<std::string>& args) {
    if (args.empty()) return;

    const std::string& subcommand = args[0];

    if (subcommand == "LS") {
        sendToClient(clientFd, ":ft_irc CAP * LS :cap-notify");
    } else if (subcommand == "REQ") {  // Acknowledge requested capabilities
        if (args.size() >= 2) {        // Example: /CAP REQ :multi-prefix
            sendToClient(clientFd, "ft_irc CAP * ACK :" + args[1]);
        }
    } else if (subcommand == "END") { // Client can continue with registration
    }
}

