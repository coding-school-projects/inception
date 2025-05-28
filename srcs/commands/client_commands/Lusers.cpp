#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

// Handle LUSERS command - show server user statistics (RFC 1459, RFC 2812)
void Server::handleLusers(int clientFd) {
    // Check if client exists (defensive programming)
    if (_clients.find(clientFd) == _clients.end()) {
        return;
    }

    Client& requester = _clients[clientFd];
    const std::string& nick = requester.getNickname();

    // Count statistics
    int total_users = 0;
    int invisible = 0;
    int operators = 0;
    int unknown = 0;
    int channels = _channels.size();
    int servers = 1;  // Since we're a single server

    // Calculate user statistics
    for (const auto& [fd, client] : _clients) {
        if (client.isRegistered()) {
            total_users++;
            if (client.isInvisible()) invisible++;
            if (client.isOperator()) operators++;
        } else {
            unknown++;
        }
    }

    int visible = total_users - invisible;

    // Build response according to RFC 1459 and RFC 2812
    std::string response;

    // RPL_LUSERCLIENT (251)
    response += ":ft_irc 251 " + nick + " :There are " + std::to_string(visible) + 
               " users and " + std::to_string(invisible) + " invisible on " + 
               std::to_string(servers) + " server(s)\n";
    
    // RPL_LUSEROP (252) - operators online
    response += ":ft_irc 252 " + nick + " " + std::to_string(operators) + 
               " :operator(s) online\n";
    
    // RPL_LUSERUNKNOWN (253) - unknown connections
    response += ":ft_irc 253 " + nick + " " + std::to_string(unknown) + 
               " :unknown connection(s)\n";
    
    // RPL_LUSERCHANNELS (254) - channels formed
    response += ":ft_irc 254 " + nick + " " + std::to_string(channels) + 
               " :channel(s) formed\n";
    
    // RPL_LUSERME (255) - client count
    response += ":ft_irc 255 " + nick + " :I have " + std::to_string(total_users) + 
               " client(s) and " + std::to_string(servers) + " server(s)";

    // Send all responses at once
    sendToClient(clientFd, response);
}
