#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

// Handle LIST command - list all channels
void Server::handleList(int clientFd, const std::vector<std::string>& args) {
    Client& client = _clients[clientFd];
    const std::string& nick = client.getNickname();

    // Send list header (RFC 2812)
    sendToClient(clientFd, ":ft_irc 321 " + nick + " Channel :Users Name");

    // Optional channel filter
    std::string filter = args.empty() ? "" : args[0];

    for (const auto& channelPair : _channels) {
        const Channel& channel = channelPair.second;
        const std::string& channelName = channel.getName();

        // Apply filter if specified
        if (!filter.empty() && channelName.find(filter) == std::string::npos) {
            continue;
        }

        // Count visible users
        size_t userCount = channel.getClients().size();

        // Send channel info (RFC 2812 RPL_LIST)
        std::string topic = channel.getTopic().empty() ? "No topic set" : channel.getTopic();
        sendToClient(clientFd, ":ft_irc 322 " + nick + " " + channelName + " " +
                      std::to_string(userCount) + " :" + topic);
    }

    // End of list
    sendToClient(clientFd, ":ft_irc 323 " + nick + " :End of /LIST");
}
