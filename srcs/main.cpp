#include "Server.hpp"

Server* serverInstance = nullptr;

void signalHandler(int signum) {
    (void)signum;
    if (serverInstance) {
        serverInstance->shutdown();
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);
    if (port < 1024 || port > 65535) {
        std::cout << "Invalid port number. Use a port between 1024 and 65535." << std::endl;
        return 1;
    }

    // Set up signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    try {
        Server server(port, argv[2]);
        serverInstance = &server;
        server.run();
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
