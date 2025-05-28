#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

// Handle QUIT command - disconnect client
void Server::handleQuit(int clientFd, const std::vector<std::string>& args) {
    auto clientIt = _clients.find(clientFd);
    if (clientIt == _clients.end()) return;

    std::string nick = clientIt->second.getNickname();
    std::string user = clientIt->second.getUsername();
    
    // Build quit reason
    std::string reason;
    if (!args.empty()) {
        reason = args[0];
        if (reason[0] == ':') {
            reason = reason.substr(1);
            for (size_t i = 1; i < args.size(); ++i) {
                reason += " " + args[i];
            }
        } else {
            for (size_t i = 1; i < args.size(); ++i) {
                reason += " " + args[i];
            }
        }
    } else {
        reason = "Client quit";
    }
    
    // Build the quit message
    std::string quitMsg = ":" + nick + "!" + user + "@localhost QUIT :" + reason + "\r\n";
    std::string errorMsg = "ERROR :Closing link: " + nick + " (Quit: " + reason + ")";
    // Send final error message to quitting client
    sendToClient(clientFd, errorMsg);

    // Disconnect with the quit message
    disconnectClient(clientFd, quitMsg);

    std::cout << "Client " << nick << " quit with reason: " << reason << std::endl;
}
