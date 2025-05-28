/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pchennia <pchennia@student.42.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 05:52:03 by olmohame          #+#    #+#             */
/*   Updated: 2025/05/28 11:53:04 by pchennia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

Server::Server(int port, const std::string& password, const std::string& servername)
    : _serverSocket(-1),
	  _port(port),
      _password(password),
      _serverName(servername),
	  _running(true),
      _isShuttingDown(false)
{
    initSocket();
}

Server::~Server()
{
    close(_serverSocket);
    for (auto& p : _clients)
        close(p.first);
}

// Initialize server socket and start listening
void Server::initSocket()
{
    // Creating the socket
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket < 0)
        throw std::runtime_error("Failed to create socket");

    // Set socket options to reuse address
    int opt = 1;
    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        close(_serverSocket);
        _serverSocket = -1;
        throw std::runtime_error("Failed to set socket options");
    }
    if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) < 0)
    {
        close(_serverSocket);
        _serverSocket = -1;
        throw std::runtime_error("Failed to set socket to non-blocking mode");
    }
    
    // Accept traffic from any address, on a specific port
    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(_port);
    if (bind(_serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
        throw std::runtime_error("Failed to bind socket");

    // Listening for possible connections
    if (listen(_serverSocket, SOMAXCONN) < 0)
    {
        close(_serverSocket);
        _serverSocket = -1;
        throw std::runtime_error("Failed to listen on socket");
    }

    // Add server socket to poll set
    _pollFds.push_back({ _serverSocket, POLLIN, 0 });
    std::cout << "Server started on port " << _port << std::endl;
}

void Server::run()
{
    _running = true;
    time_t lastHealthCheck = time(nullptr);
    while (_running)
    {
        time_t now = time(nullptr);
        if (now - lastHealthCheck >= 5)
        {
            checkConnectionHealth();
            lastHealthCheck = now;
        }
        // Check for I/O activity, timeout set to 100ms
        if (poll(_pollFds.data(), _pollFds.size(), 100) < 0)
        {
            if (errno == EINTR) continue;
            perror("poll error");
            break;
        }
        // Process I/O events
        for (size_t i = 0; i < _pollFds.size(); )
        {
            // Case where there is some data to read. POLLIN = something happened
            if (_pollFds[i].revents & POLLIN)
            {
                if (_pollFds[i].fd == _serverSocket)
                {
                    acceptNewClient();
                    i++;
                }
                else
                {
                    handleClientMessage(_pollFds[i].fd);
                    // Check whether the client is still active
                    if (_clients.find(_pollFds[i].fd) == _clients.end())
                        continue;
                    i++;
                }
            } 
            else if (_pollFds[i].revents & (POLLHUP | POLLERR))
            {
                std::cout << "Client " << _pollFds[i].fd << " disconnected (hangup/error)" << std::endl;
                disconnectClient(_pollFds[i].fd);
            } 
            else
            {
                i++;
            }
        }
    }
}


// Accept new client connection
void Server::acceptNewClient()
{
    sockaddr_in clientAddr;
    socklen_t addrlen = sizeof(clientAddr);
    int clientFd = accept(_serverSocket, (struct sockaddr*)&clientAddr, &addrlen);
    if (clientFd < 0)
    {
        perror("accept error");
        return;
    }
    // Set non-blocking mode
    if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
    {
        close(clientFd);
        clientFd = -1;
        std::cout << "Failed to set client socket to non-blocking mode for fd=" << clientFd << std::endl;
        return ;
    }
    _pollFds.push_back({ clientFd, POLLIN, 0 });
    _clients[clientFd] = Client(clientFd);
    std::cout << "New connection: fd=" << clientFd << std::endl;
}

std::string Server::getCreationDate()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    std::stringstream ss;
    ss << std::put_time(ltm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void briefDelay(void)
{
    int microseconds = 50000;
    struct timeval tv = {0, microseconds};
    select(0, NULL, NULL, NULL, &tv);
}

bool isSocketValid(int fd)
{
    int error = 0;
    socklen_t len = sizeof(error);
    return getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) == 0;
}

void Server::shutdown()
{
    std::cout << "Server shutting down ..." << std::endl;
    _running = false;
	_isShuttingDown = true;

	// Firt, notify clients about the shutting down
    for (const auto& client : _clients)
    {
        try
        {
            if (isSocketValid(client.first))
            {
                sendToClient(client.first, "ERROR :Server is shutting down");
                briefDelay();
            }
        }
        catch (const std::exception& e)
        {
            std::cout << "Shutdown notification error: " << e.what() << std::endl;
        }
    }

    // Prepare a static snapShot to allow for a safe client disconnection
    std::vector<int> clientsSnapshot;
    for (const auto& client : _clients)
        clientsSnapshot.push_back(client.first);

    // Disconnect clients
    for (int fd : clientsSnapshot)
    {
        try
        {
            if (_clients.count(fd))
                disconnectClient(fd);
        }
        catch (const std::exception& e)
        {
            std::cout << "Error while disconnecting client: " << e.what() << std::endl;
            close(fd);
        }
    }
    //Deleting the 
    _clients.clear();
    _channels.clear();
    _pollFds.clear();

    if (_serverSocket != -1)
    {
        close(_serverSocket);
        _serverSocket = -1;
    }

    std::cout << RED << "Server shutdown complete" << RESET << std::endl;
	_isShuttingDown = false;
}

void Server::checkConnectionHealth()
{
    const time_t PING_TIMEOUT = 300;
    const time_t PING_INTERVAL = 900;
    time_t now = time(nullptr);
    bool showHeader = false;
    
    for (auto& [fd, client] : _clients)
    {
        if ((now - client.getLastActivity() > PING_INTERVAL && !client.isWaitingForPong()) ||
            (client.isWaitingForPong() && now - client.getLastPingTime() > PING_TIMEOUT))
        {
            showHeader = true;
            break;
        }
    }
    
    if (showHeader)
        std::cout << "\n[Health Check] " << std::ctime(&now);

    for (auto it = _clients.begin(); it != _clients.end(); )
    {
        Client& client = it->second;
        time_t idle = now - client.getLastActivity();
        
        if (idle > PING_INTERVAL && !client.isWaitingForPong())
        {
            std::string pingMsg = "PING :" + std::to_string(now);
            if (showHeader)
            {
                std::cout << "  Client " << it->first 
                          << " (" << client.getNickname() << "): Sending PING\n";
            }
            sendToClient(it->first, pingMsg);
            client.markPingSent();
            ++it;
        }
        else if (client.isWaitingForPong() && now - client.getLastPingTime() > PING_TIMEOUT)
        {
            if (showHeader)
            {
                std::cout << "  Client " << it->first 
                          << " (" << client.getNickname() << "): Disconnecting (PING timeout)\n";
            }
      
            std::vector<std::string> quitArgs;
            quitArgs.push_back("Ping timeout");
            handleQuit(it->first, quitArgs);
            it = _clients.begin();
        }
        else
        {
            ++it;
        }
    }
}