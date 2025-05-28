#pragma once

#include "Headers.hpp"

class Client {
public:
    // Registration states as bit flags
    static const uint8_t UNREGISTERED   = 0;
    static const uint8_t PASS_RECEIVED  = 1; // 0b00000001
    static const uint8_t NICK_RECEIVED  = 2; // 0b00000010
    static const uint8_t USER_RECEIVED  = 4; // 0b00000100
    static const uint8_t REGISTERED     = 7; // 0b00000111 (1 | 2 | 4)

    typedef uint8_t RegistrationState;

    Client();
    Client(int fd);
    ~Client();
    
    // Getters
    int getFd() const;
    bool isAuthenticated() const;
    bool isRegistered() const;
    uint8_t getRegState() const;
    const std::string& getNickname() const;
    const std::string& getUsername() const;
    const std::string& getRealName() const;
    const std::string& getClientIP() const;
    std::queue<std::string>& getMessages();
    
    // Setters
    void setRegState(RegistrationState state);
    void authenticate();
    void setNickname(const std::string& nickname);
    void setUsername(const std::string& username);
    void setRealName(const std::string& realname);
    
    // Buffer management
    void appendBuffer(const char* buffer);
    bool extractMessage(std::string& message);
    bool isReceivingCommand() const;
    
    // Validation methods
    static bool isValidNickname(const std::string& nick);
    static bool isValidUsername(const std::string& user);
    static bool isValidChannelName(const std::string& channel);

	// Activity tracking
    void updateActivityTime();
    time_t getLastActivity() const;


	// Ping/Pong
    void markPingSent();
    void markPongReceived();
    bool isWaitingForPong() const;
    time_t getLastPingTime() const;

    // Modes
	void setInvisible(bool state);
    bool isInvisible() const;
	void setOperator(bool state);
    bool isOperator() const;

private:
    int _fd;                        // Client socket file descriptor
    bool _authenticated;            // Authentication status
    uint8_t _regState;              // Registration progress
    std::string _nickname;          // Client nickname
    std::string _username;          // Client username
    std::string _realname;          // Client real name
    std::string _clientIP;
    std::string _buffer;            // Accumulated received data
    std::queue<std::string> _messages; // Complete messages ready for processing
    
	std::time_t _lastActivityTime;  // Last general activity
	std::time_t _lastPingTime;      // When we last PINGed this client
    bool _waitingForPong;           // Whether we're expecting a PONG

	bool _invisible = false;
	bool _isOperator = false;

    // Validation constants
    static const size_t MAX_NICK_LENGTH;
    static const size_t MAX_USER_LENGTH;
    static const std::string FORBIDDEN_CHARS;
};
