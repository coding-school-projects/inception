#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

void Server::sendWelcomeMessages(int clientFd) {
    const std::string& nick = _clients[clientFd].getNickname();
std::string msg =
    "001 " + nick + " :Welcome to the IRC server " + nick + "\n" +
    ":ft_irc 002 " + nick + " :Your host is ft_irc, running version 1.0\n" +
    ":ft_irc 003 " + nick + " :This server was created " + getCreationDate() + "\n" +
    ":ft_irc 004 " + nick + " :ft_irc 1.0 o o\n" +
    ":ft_irc 375 " + nick + " :- Message of the Day -\n" +
    ":ft_irc 372 " + nick + " :---- _  _------------------------\n" +
    ":ft_irc 372 " + nick + " :-|_|/ \\|_)  Welcome To Our IRC \n" +
    ":ft_irc 372 " + nick + " :-| |\\_/|        " + nick + "\n" +
    ":ft_irc 372 " + nick + " :---------------------------------\n" +
    ":ft_irc 376 " + nick + " :End of /MOTD command.";
    sendToClient(clientFd, msg);
}

void Server::disconnectClient(int clientFd, const std::string& quitMsg) {
    auto clientIt = _clients.find(clientFd);
    if (clientIt == _clients.end()) return;

    // Save nickname for logging
    std::string nickname = clientIt->second.getNickname();

    // Notify all channels the client is in
    for (auto it = _channels.begin(); it != _channels.end(); ) {
        Channel& channel = it->second;
        if (channel.hasClient(clientFd)) {
            // Send quit message to channel members (except the disconnecting client)
            if (!quitMsg.empty()) {
                sendToChannel(it->first, quitMsg, clientFd);
            }
            
            // Remove client from channel
            channel.removeClient(clientFd);
            channel.removeOperator(clientFd);
            channel.removeInvited(clientFd);

            // Remove empty channels
            if (channel.getClients().empty()) {
                std::cout << YELLOW << "Channel " << channel.getName() 
                          << " removed (last user left)" << RESET << std::endl;
                it = _channels.erase(it);
                continue;
            }
        }
        ++it;
    }

    // Force immediate connection termination (RST instead of FIN-ACK)
    struct linger linger_opt = {1, 0};  // { active=1, timeout=0 }
    setsockopt(clientFd, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(linger_opt));
    
    // Close the socket
    ::shutdown(clientFd, SHUT_RDWR);
    if (close(clientFd) == -1) {
        if (errno != EBADF)
            perror("close() failed");
    }

    // Remove from clients map and poll set
    _clients.erase(clientIt);
    _pollFds.erase(std::remove_if(_pollFds.begin(), _pollFds.end(), 
                  [clientFd](const pollfd& pfd) { return pfd.fd == clientFd; }), 
                  _pollFds.end());
    
    if (!_isShuttingDown) {
        std::cout << YELLOW_B << "Client " << nickname << " disconnected: fd=" << clientFd << RESET << std::endl;
    }
}

// Handle incoming message from client
void Server::handleClientMessage(int clientFd) {
    char buffer[512];
    ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

    if (bytesRead == 0) {
        // Client disconnected gracefully
        std::cout << "Client " << clientFd << " disconnected gracefully" << std::endl;
        std::vector<std::string> quitArgs;
        quitArgs.push_back("Connection closed");
        handleQuit(clientFd, quitArgs);  // Use handleQuit to ensure proper cleanup
        return;
    } else if (bytesRead < 0) {
        return;
    }

	// Update activity time for successful receives
    if (_clients.find(clientFd) != _clients.end()) {
        _clients[clientFd].updateActivityTime();
    }

    buffer[bytesRead] = '\0';
	std::cout << std::endl;
    // std::cout << "Raw received data: " << buffer << std::endl;
    
    _clients[clientFd].appendBuffer(buffer);

    std::string message;
    while (_clients[clientFd].extractMessage(message)) {
        std::cout << CYAN_BU << "Processing command: " << message << RESET << std::endl;
        processCommand(clientFd, message);
    }
}

// Process a complete command from client
void Server::processCommand(int clientFd, const std::string& command) {
	// Skip empty commands
    if (command.empty() || command == "\r\n" || command == "\n") {
        return;
    }

	// Update activity time for ANY valid command received
    if (_clients.find(clientFd) != _clients.end()) {
        _clients[clientFd].updateActivityTime();
    }

    std::cout << "Received command: " << command << std::endl;

    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    
    // Convert to uppercase for case-insensitive comparison
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

    std::vector<std::string> args;
    std::string arg;
    while (iss >> arg) {
        args.push_back(arg);
    }

       if (cmd == "CAP" && !args.empty() && args[0] == "LS") {
        handleCapLs(clientFd, args);
        return;
    }

    // Handle registration sequence
    if (!_clients[clientFd].isRegistered()) {
        if (cmd == "PASS") {
            handlePass(clientFd, args);
            return;
        }
        else if (cmd == "NICK") {
            handleNick(clientFd, args);
            return;
        }
        else if (cmd == "USER") {
            handleUser(clientFd, args);
            return;
        }
        else {
            sendToClient(clientFd, "451 :You have not registered");
            return;
        }
    }

    // Handle registered commands
    if (cmd == "JOIN") {
        handleJoin(clientFd, args);
    } else if (cmd == "PRIVMSG") {
        handlePrivMsg(clientFd, args);
    } else if (cmd == "CAP" && !args.empty() && args[0] == "LS") {
        handleCapLs(clientFd, args);
    } else if (cmd == "NICK") {
        handleNick(clientFd, args);
    } else if (cmd == "USER") {
        handleUser(clientFd, args);
    } else if (cmd == "PASS") {
        handlePass(clientFd, args);
    } else if (cmd == "KICK") {
        handleKick(clientFd, args);
    } else if (cmd == "INVITE") {
        handleInvite(clientFd, args);
    } else if (cmd == "TOPIC") {
        handleTopic(clientFd, args);
    } else if (cmd == "MODE") {
        handleMode(clientFd, args);
    } else if (cmd == "NAMES") {
        handleNames(clientFd, args, args.empty());
    } else if (cmd == "LUSERS") {
        handleLusers(clientFd);
    } else if (cmd == "LIST") {
        handleList(clientFd, args);
    } else if (cmd == "PART") {
        handlePart(clientFd, args);
    } else if (cmd == "QUIT") {
        handleQuit(clientFd, args);
    } else if (cmd == "WHO") {
        handleWho(clientFd, args);
    } else if (cmd == "WHOIS") {
        handleWhois(clientFd, args);
	} else if (cmd == "PING") {
        handlePing(clientFd, args);
    } else if (cmd == "PONG") {
        handlePong(clientFd, args);
    } else if (cmd == "USERS") {
        std::string response = "Connected users:";
        for (const auto& pair : _clients) {
            response += " " + pair.second.getNickname();
        }
        sendToClient(clientFd, ":" + _serverName + " NOTICE " + _clients[clientFd].getNickname() + " :" + response);
    } else {
        sendToClient(clientFd, "421 " + cmd + " :Unknown command");
        std::cout << "Unknown command from client " << clientFd << ": " << cmd << std::endl;
    }
}

// Send message to a specific client
bool Server::sendToClient(int clientFd, const std::string& msg, bool isError) {
    // std::string response = (isError ? ":" + _serverName + " " : "") + msg + "\r\n";
    std::string response = isError ? ":" + _serverName + " " + msg : msg;
    response += "\r\n";  // Ensure CRLF termination
    if (send(clientFd, response.c_str(), response.length(), 0) == -1) {
        std::cerr << "Error sending message: " << strerror(errno) << std::endl;
        perror("send error");
        disconnectClient(clientFd);
        return false;
    }
    return true;
}

// Send message to all clients in a channel (optionally excluding one)
void Server::sendToChannel(const std::string& channelName, const std::string& message, int excludeFd) {
    if (_channels.find(channelName) == _channels.end()) {
        return;
    }

    const Channel& channel = _channels[channelName];
    for (int memberFd : channel.getClients()) {
        if (memberFd != excludeFd) {
            sendToClient(memberFd, message);
        }
    }
}
