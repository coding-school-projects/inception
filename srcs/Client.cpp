#include "Client.hpp"

// Validation constants
const size_t Client::MAX_NICK_LENGTH = 20;
const size_t Client::MAX_USER_LENGTH = 20;
const std::string Client::FORBIDDEN_CHARS = " ,*?!@.";

// Default constructor
Client::Client() : 
    _fd(-1), 
    _authenticated(false), 
    _regState(UNREGISTERED),
    _lastActivityTime(time(nullptr)),
    _lastPingTime(0),
    _waitingForPong(false),
    _invisible(false),
    _isOperator(false) {}

// Constructor with file descriptor
Client::Client(int fd) : 
    _fd(fd), 
    _authenticated(false), 
    _regState(UNREGISTERED),
    _lastActivityTime(time(nullptr)),
    _lastPingTime(0),
    _waitingForPong(false),
    _invisible(false),
    _isOperator(false) {
        // Get and store client IP at construction time
        sockaddr_in addr;
        socklen_t len = sizeof(addr);
        if (getpeername(fd, (sockaddr*)&addr, &len) == 0) {
            _clientIP = inet_ntoa(addr.sin_addr);
        } else {
            _clientIP = "0.0.0.0"; // Default if error occurs
        }
    }

// Destructor
Client::~Client() {}

// GETTERS

int Client::getFd() const { return _fd; }
bool Client::isAuthenticated() const { return _authenticated; }
bool Client::isRegistered() const { return (_regState & REGISTERED) == REGISTERED; }
uint8_t Client::getRegState() const { return _regState; }
const std::string& Client::getNickname() const { return _nickname; }
const std::string& Client::getUsername() const { return _username; }
const std::string& Client::getRealName() const { return _realname; }
const std::string& Client::getClientIP() const { return _clientIP; }
std::queue<std::string>& Client::getMessages() { return _messages; }

// SETTERS

void Client::setRegState(RegistrationState state) { _regState |= state; }
void Client::authenticate() { _authenticated = true; }
void Client::setNickname(const std::string& nickname) { _nickname = nickname; }
void Client::setUsername(const std::string& username) { _username = username; }
void Client::setRealName(const std::string& realname) { _realname = realname; }

// Buffer handling
void Client::appendBuffer(const char* buffer) { _buffer += buffer; }

// Extract complete messages from buffer (delimited by \r\n)
bool Client::extractMessage(std::string& message) {
    // Look for either \r\n or \n as line endings
    size_t pos = _buffer.find("\r\n");
    if (pos == std::string::npos) {
        pos = _buffer.find('\n');
        if (pos == std::string::npos) {
            return false;
        }
        // Found just \n
        message = _buffer.substr(0, pos);
        _buffer.erase(0, pos + 1);
    } else {
        // Found \r\n
        message = _buffer.substr(0, pos);
        _buffer.erase(0, pos + 2);
    }
    _messages.push(message);
    return true;
}

bool Client::isReceivingCommand() const { return !_buffer.empty(); }

// Validation
bool Client::isValidNickname(const std::string& nick) {
    if (nick.empty() || nick.length() > MAX_NICK_LENGTH) return false;
    if (nick[0] == ':' || nick[0] == '#') return false;
    if (nick.find_first_of(FORBIDDEN_CHARS) != std::string::npos) return false;
    return true;
}

bool Client::isValidUsername(const std::string& user) {
    if (user.empty() || user.length() > MAX_USER_LENGTH) return false;
    if (user.find_first_of(FORBIDDEN_CHARS) != std::string::npos) return false;
    return true;
}

bool Client::isValidChannelName(const std::string& channel) {
    if (channel.empty() || channel.length() > 50) return false;
	// First character must be '#' or '&'
	if (channel[0] != '#' && channel[0] != '&') return false;
	// Subsequent characters must not contain spaces, commas, or control chars
	for (size_t i = 1; i < channel.length(); i++) {
        // Reject control characters (ASCII < 32) and bell character (7)
        if (static_cast<unsigned char>(channel[i]) < 32 || channel[i] == '\a')
            return false;
        
        // Reject spaces and commas
        if (isspace(channel[i]) || channel[i] == ',')
            return false;
    }
    return true;
}

// Activity
void Client::updateActivityTime() { _lastActivityTime = std::time(nullptr); }
time_t Client::getLastActivity() const { return _lastActivityTime; }

// Ping/Pong
void Client::markPingSent() {
	_lastPingTime = std::time(nullptr);
	_waitingForPong = true;
}

void Client::markPongReceived() { _waitingForPong = false; }
bool Client::isWaitingForPong() const { return _waitingForPong; }
time_t Client::getLastPingTime() const { return _lastPingTime; }

// Modes
void Client::setInvisible(bool state) { _invisible = state; }
bool Client::isInvisible() const { return _invisible; }
void Client::setOperator(bool state) { _isOperator = state; }
bool Client::isOperator() const { return _isOperator; }
