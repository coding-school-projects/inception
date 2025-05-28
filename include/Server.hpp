/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olmohame <olmohame@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 07:04:13 by olmohame          #+#    #+#             */
/*   Updated: 2025/05/28 07:05:24 by olmohame         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Headers.hpp"

class Channel;
class Client;

class Server
{
public:
    Server(int port, const std::string& password, const std::string& serverName = "ft_irc");
    ~Server();
    void run();
    void shutdown();

private:
    int _serverSocket;                        // Server socket file descriptor
    int _port;                                // Server port
    std::string _password;                    // Server password
    std::map<int, Client> _clients;           // Map of connected clients (key: fd)
    std::map<std::string, Channel> _channels; // Map of channels (key: channel name)
    std::vector<struct pollfd> _pollFds;      // Poll file descriptors
    std::string _serverName;
    volatile bool _running;
	bool _isShuttingDown;

    // Socket and connection management
    void initSocket();
    void acceptNewClient();
    void disconnectClient(int clientFd, const std::string& quitMsg = "");
    void handleClientMessage(int clientFd);
    void processCommand(int clientFd, const std::string& command);
    
    // Command handlers
    void handlePass(int clientFd, const std::vector<std::string>& args);
    void handleNick(int clientFd, const std::vector<std::string>& args);
    void handleUser(int clientFd, const std::vector<std::string>& args);
    void handleJoin(int clientFd, const std::vector<std::string>& args);
    void handlePrivMsg(int clientFd, const std::vector<std::string>& args);
    void handleKick(int clientFd, const std::vector<std::string>& args);
    void handleInvite(int clientFd, const std::vector<std::string>& args);
    void handleTopic(int clientFd, const std::vector<std::string>& args);
    void handleMode(int clientFd, const std::vector<std::string>& args);
    void handleNames(int clientFd, const std::vector<std::string>& args, bool listAll);
    void handleQuit(int clientFd, const std::vector<std::string>& args);
    void handlePart(int clientFd, const std::vector<std::string>& args);
    void handleList(int clientFd, const std::vector<std::string>& args);
    void handlePing(int clientFd, const std::vector<std::string>& args);
    void handlePong(int clientFd, const std::vector<std::string>& args);
    void handleWho(int clientFd, const std::vector<std::string> &args);
    void handleWhois(int clientFd, const std::vector<std::string>& args);
    void handleCapLs(int clientFd, const std::vector<std::string> &args);
    void handleLusers(int clientFd);
    
    // Message sending utilities
    bool sendToClient(int clientFd, const std::string& msg, bool isError = false);
    void sendToChannel(const std::string& channelName, const std::string& message, int excludeFd = -1);
    void sendChannelInfo(int clientFd, const std::string& channelName);
    void sendWelcomeMessages(int clientFd);
    std::string getCreationDate();
	void checkConnectionHealth();

    const int MAX_CHANNEL_USERS = 100;
};