#include "Server.hpp"
#include "Client.hpp"

/**
 * Handles PONG response from client
 * @param clientFd The client file descriptor
 * @param args Command arguments containing the PONG token
 */

 void Server::handlePong(int clientFd, const std::vector<std::string>& args) {
    auto it = _clients.find(clientFd);
    if (it == _clients.end()) {
        std::cerr << "PONG from unknown client: " << clientFd << std::endl;
        return;
    }

    Client& client = it->second;
	client.updateActivityTime();  // Always update on any client activity
    
       // Extract token (handling IRC message formatting)
	   std::string receivedToken;
	   if (!args.empty()) {
		   receivedToken = args[0];
		   if (receivedToken[0] == ':') {
			   receivedToken = receivedToken.substr(1);
		   }
		   // Handle multi-word tokens
		   for (size_t i = 1; i < args.size(); ++i) {
			   receivedToken += " " + args[i];
		   }
	   }
   
	   std::cout << "Client " << clientFd << " (" << client.getNickname() 
				 << ") PONG response: " << receivedToken << std::endl;

    // Only validate if we're expecting a PONG
    if (client.isWaitingForPong()) {
		try {
            // Convert token to number and compare with last ping time
            long pingTime = std::stol(receivedToken);
            if (pingTime == static_cast<long>(client.getLastPingTime())) {
                client.markPongReceived();
                std::cout << "  Valid PONG response received for PING " 
                          << pingTime << std::endl;
            } else {
                std::cout << "  Warning: PONG token mismatch (received: " << pingTime
                          << ", expected: " << client.getLastPingTime() << ")\n";
            }
        } catch (const std::exception& e) {
            std::cout << "  Warning: Invalid PONG token format from " 
                      << client.getNickname() << ": " << receivedToken << "\n";
        }
		// For debugging connection issues:
		std::cerr << "Potential desync for client " << clientFd 
		<< " - last PING was at " << client.getLastPingTime() 
		<< " (" << time(nullptr) - client.getLastPingTime() 
		<< " seconds ago)\n";
    }
}