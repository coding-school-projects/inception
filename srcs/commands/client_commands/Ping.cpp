#include "Server.hpp"
#include "Client.hpp"

/**
 * Handles PING command from client
 * @param clientFd The client file descriptor
 * @param args Command arguments containing the PING token
 */

void Server::handlePing(int clientFd, const std::vector<std::string>& args) {
    auto it = _clients.find(clientFd);
    if (it == _clients.end()) return;

    // Use the correct method name: updateActivityTime()
    it->second.updateActivityTime();Client& client = it->second;
    client.updateActivityTime();

    if (args.empty()) {
        sendToClient(clientFd, "461 PING :Not enough parameters");
        return;
    }

    // Handle token with proper IRC message formatting
    std::string token;
    if (args[0][0] == ':') {
        // Combined token with spaces
        token = args[0].substr(1);
        for (size_t i = 1; i < args.size(); ++i) {
            token += " " + args[i];
        }
    } else {
        // Simple single-word token
        token = args[0];
    }

    // Always respond with PONG (RFC 1459 requirement)
    std::string pongMsg = "PONG :" + token;
    if (!sendToClient(clientFd, pongMsg)) {
        std::cerr << "Failed to send PONG to client " << clientFd << std::endl;
        return;
    }

    std::cout << "Responded to PING from client " << clientFd 
              << " (" << client.getNickname() << ") with: " << pongMsg << std::endl;
    
    // If this was a response to our PING, mark it
    if (it->second.isWaitingForPong()) {
        it->second.markPongReceived();
    }
}