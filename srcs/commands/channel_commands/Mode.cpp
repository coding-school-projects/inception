#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

void Server::handleMode(int clientFd, const std::vector<std::string>& args) {
    if (args.empty()) {
        sendToClient(clientFd, "461 MODE :Not enough parameters");
        return;
    }

    Client& requester = _clients[clientFd];
    const std::string& nick = requester.getNickname();
    const std::string& target = args[0];
    
    // Channel mode handling
    if (target[0] == '#' || target[0] == '&') {
        // Validate channel name
        if (!Client::isValidChannelName(target)) {
            sendToClient(clientFd, "403 " + nick + " " + target + " :Invalid channel name");
            return;
        }

        // Check if channel exists
        auto chanIt = _channels.find(target);
        if (chanIt == _channels.end()) {
            sendToClient(clientFd, "403 " + nick + " " + target + " :No such channel");
            return;
        }

        Channel& channel = chanIt->second;
        
        // Check if client is in channel
        if (!channel.hasClient(clientFd)) {
            sendToClient(clientFd, "442 " + nick + " " + target + " :You're not on that channel");
            return;
        }

        // If no mode change requested, list current modes
        if (args.size() == 1) {
            std::string modes = "+";
            std::string modeParams;
            
            if (channel.isInviteOnly()) modes += "i";
            if (channel.isTopicRestricted()) modes += "t";
            if (!channel.getKey().empty()) {
                modes += "k";
                modeParams += " " + channel.getKey();
            }
            if (channel.getUserLimit() > 0) {
                modes += "l";
                modeParams += " " + std::to_string(channel.getUserLimit());
            }
            
            sendToClient(clientFd, "324 " + nick + " " + target + " " + modes + modeParams);
            sendToClient(clientFd, "329 " + nick + " " + target + " " + std::to_string(channel.getCreationTime()));
            return;
        }

        if (args.size() == 2 && args[1] == "b") {
            const std::vector<BanEntry>& bans = channel.getBanList();
            for (const BanEntry& ban : bans) {
                sendToClient(clientFd, "367 " + nick + " " + target + " " +
                ban.mask + " " + ban.setBy + " " + std::to_string(ban.timestamp));
            }
            sendToClient(clientFd, "368 " + nick + " " + target + " :End of Channel Ban List");
            return;
        }

        
        // Process each mode character in the mode string
        const std::string& modeStr = args[1];
        if (modeStr.empty() || modeStr.size() < 2 || (modeStr[0] != '+' && modeStr[0] != '-')) {
            sendToClient(clientFd, "472 " + nick + " " + modeStr + " :Invalid mode string");
            return;
        }

        bool set = modeStr[0] == '+';
        size_t argIndex = 2; // Start of mode arguments
        
        // Process each mode character
        for (size_t i = 1; i < modeStr.size(); ++i) {
            char modeChar = modeStr[i];
            std::string modeChange;
            
            switch (modeChar) {
                case 'i': // Invite-only mode
                    if (!channel.isOperator(clientFd)) {
                        sendToClient(clientFd, "482 " + nick + " " + target + " :You're not channel operator");
                        continue;
                    }
                    channel.setInviteOnly(set);
                    modeChange = std::string(1, modeStr[0]) + "i";
                    break;
                    
                case 't': // Topic restriction mode
                    if (!channel.isOperator(clientFd)) {
                        sendToClient(clientFd, "482 " + nick + " " + target + " :You're not channel operator");
                        continue;
                    }
                    channel.setTopicRestricted(set);
                    modeChange = std::string(1, modeStr[0]) + "t";
                    break;
                    
                case 'k': { // Channel key/password mode
                    if (!channel.isOperator(clientFd)) {
                        sendToClient(clientFd, "482 " + nick + " " + target + " :You're not channel operator");
                        continue;
                    }
                    if (set) {
                        if (args.size() <= argIndex) {
                            sendToClient(clientFd, "461 " + nick + " MODE :Not enough parameters");
                            continue;
                        }
                        channel.setKey(args[argIndex]);
                        modeChange = "+k " + args[argIndex];
                        argIndex++;
                    } else {
                        channel.setKey("");
                        modeChange = "-k";
                    }
                    break;
                }
                    
                case 'o': { // Operator privilege mode
                    if (!channel.isOperator(clientFd)) {
                        sendToClient(clientFd, "482 " + nick + " " + target + " :You're not channel operator");
                        continue;
                    }
                    if (args.size() <= argIndex) {
                        sendToClient(clientFd, "461 " + nick + " MODE :Not enough parameters");
                        continue;
                    }
                    
                    const std::string& targetNick = args[argIndex];
                    int targetFd = -1;
                    for (const auto& [fd, client] : _clients) {
                        if (client.getNickname() == targetNick) {
                            targetFd = fd;
                            break;
                        }
                    }
                    
                    if (targetFd == -1) {
                        sendToClient(clientFd, "401 " + nick + " " + targetNick + " :No such nick");
                        continue;
                    }
                    
                    if (!channel.hasClient(targetFd)) {
                        sendToClient(clientFd, "441 " + nick + " " + targetNick + " " + target + 
                                   " :They aren't on that channel");
                        continue;
                    }
                    
                    if (set) {
                        channel.addOperator(targetFd);
                        modeChange = "+o " + targetNick;
                    } else {
                        if (targetFd == clientFd && channel.getOperators().size() == 1) {
                            sendToClient(clientFd, "484 " + nick + " " + target + 
                                       " :Cannot remove your own operator status (last operator)");
                            continue;
                        }
                        channel.removeOperator(targetFd);
                        modeChange = "-o " + targetNick;
                    }
                    argIndex++;
                    break;
                }
                    
                case 'l': { // User limit mode
                    if (!channel.isOperator(clientFd)) {
                        sendToClient(clientFd, "482 " + nick + " " + target + " :You're not channel operator");
                        continue;
                    }
                    if (set) {
                        if (args.size() <= argIndex) {
                            sendToClient(clientFd, "461 " + nick + " MODE :Not enough parameters");
                            continue;
                        }
                        try {
                            int limit = std::stoi(args[argIndex]);
                            if (limit <= 0) {
                                sendToClient(clientFd, "461 " + nick + " MODE :Limit must be positive");
                                continue;
                            }
                            channel.setUserLimit(limit);
                            modeChange = "+l " + args[argIndex];
                            argIndex++;
                        } catch (...) {
                            sendToClient(clientFd, "461 " + nick + " MODE :Invalid limit value");
                            continue;
                        }
                    } else {
                        channel.setUserLimit(-1);
                        modeChange = "-l";
                    }
                    break;
                }

                case 'b': {
                    if (!channel.isOperator(clientFd)) {
                        sendToClient(clientFd, "482 " + nick + " " + target + " :You're not channel operator");
                        continue;
                    }

                    if (args.size() <= argIndex) {
                        // If no mask is provided, treat as a ban list query
                        const std::vector<BanEntry>& bans = channel.getBanList();
                        for (const BanEntry& ban : bans) {
                            sendToClient(clientFd, "367 " + nick + " " + target + " " +
                            ban.mask + " " + ban.setBy + " " + std::to_string(ban.timestamp));
                        }
                        sendToClient(clientFd, "368 " + nick + " " + target + " :End of Channel Ban List");
                        continue;
                    }

                    const std::string& mask = args[argIndex];
                    argIndex++;

                    if (set) {
                        channel.addBan(mask, nick);
                        modeChange = "+b " + mask;
                    } else {
                        if (channel.removeBan(mask))
                            modeChange = "-b " + mask;
                        else {
                            sendToClient(clientFd, "441 " + nick + " " + mask + " " + target + " :Ban mask not found");
                            continue;
                        }
                    }
                    break;
                }
   
                default:
                    sendToClient(clientFd, "472 " + nick + " " + std::string(1, modeChar) + 
                               " :is unknown mode char to me");
                    continue;
            }

            // Send mode change notification to channel
            if (!modeChange.empty()) {
                std::string modeMsg = ":" + nick + " MODE " + target + " " + modeChange;
                sendToChannel(target, modeMsg);
            }
        }
    } 
    // User mode handling
    else {
        // Can only change your own modes
        if (target != nick) {
            sendToClient(clientFd, "502 " + nick + " :Can only change your own modes");
            return;
        }
        
        if (args.size() < 2) {
            sendToClient(clientFd, "461 " + nick + " MODE :Not enough parameters");
            return;
        }
        
        const std::string& modeStr = args[1];
        if (modeStr.empty() || modeStr.size() < 2 || (modeStr[0] != '+' && modeStr[0] != '-')) {
            sendToClient(clientFd, "472 " + nick + " " + modeStr + " :Invalid mode string");
            return;
        }
        
        bool set = modeStr[0] == '+';
        char modeChar = modeStr[1];
        
        switch (modeChar) {
            case 'i': // Invisible mode
                requester.setInvisible(set);
                sendToClient(clientFd, ":" + nick + " MODE " + nick + " " + modeStr.substr(0, 2));
                break;
                
            default:
                sendToClient(clientFd, "472 " + nick + " " + std::string(1, modeChar) + 
                           " :is unknown user mode char to me");
        }
    }
}