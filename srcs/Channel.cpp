#include "Channel.hpp"

// Constructor with channel name
Channel::Channel(const std::string& name) : _name(name), _inviteOnly(false), 
    _topicRestricted(false), _userLimit(-1) {}

// Add client to channel
void Channel::addClient(int clientFd) {
    if (std::find(_clients.begin(), _clients.end(), clientFd) == _clients.end()) {
        _clients.push_back(clientFd);
    }
}

// Remove client from channel
void Channel::removeClient(int clientFd) {
    _clients.erase(std::remove(_clients.begin(), _clients.end(), clientFd), _clients.end());
    removeOperator(clientFd);
}

// Set channel topic
void Channel::setTopic(const std::string& topic) { _topic = topic; }

// Get channel topic
const std::string& Channel::getTopic() const { return _topic; }

// Add operator to channel
void Channel::addOperator(int clientFd) {
    if (std::find(_operators.begin(), _operators.end(), clientFd) == _operators.end()) {
        _operators.push_back(clientFd);
    }
}

// Remove operator from channel
void Channel::removeOperator(int clientFd) {
    _operators.erase(std::remove(_operators.begin(), _operators.end(), clientFd), _operators.end());
}

// Check if client is operator
bool Channel::isOperator(int clientFd) const {
    return std::find(_operators.begin(), _operators.end(), clientFd) != _operators.end();
}

// Set invite-only mode
void Channel::setInviteOnly(bool inviteOnly) { _inviteOnly = inviteOnly; }

// Check if channel is invite-only
bool Channel::isInviteOnly() const { return _inviteOnly; }

// Set topic restriction mode
void Channel::setTopicRestricted(bool restricted) { _topicRestricted = restricted; }

// Check if topic is restricted
bool Channel::isTopicRestricted() const { return _topicRestricted; }

// Set channel key/password
void Channel::setKey(const std::string& key) { _key = key; }

// Check if provided key matches channel key
bool Channel::checkKey(const std::string& key) const { return _key.empty() || _key == key; }

// Set user limit
void Channel::setUserLimit(int limit) { _userLimit = limit; }

// Get user limit (-1 for no limit)
int Channel::getUserLimit() const { return _userLimit; }

// Get list of clients in channel
const std::vector<int>& Channel::getClients() const { return _clients; }

// Get channel name
const std::string& Channel::getName() const { return _name; }

// Get channel key
const std::string& Channel::getKey() const { return _key; }

void Channel::addInvited(int clientFd) {
    if (std::find(_invitedClients.begin(), _invitedClients.end(), clientFd) == _invitedClients.end()) {
        _invitedClients.push_back(clientFd);
    }
}

void Channel::removeInvited(int clientFd) {
    _invitedClients.erase(std::remove(_invitedClients.begin(), _invitedClients.end(), clientFd), _invitedClients.end());
}

bool Channel::isInvited(int clientFd) const {
    return std::find(_invitedClients.begin(), _invitedClients.end(), clientFd) != _invitedClients.end();
}

bool Channel::hasClient(int clientFd) const {
    return std::find(_clients.begin(), _clients.end(), clientFd) != _clients.end();
}

const std::vector<int>& Channel::getOperators() const {
    return _operators;
}

// Set topic and record who set it and when
void Channel::setTopic(const std::string& topic, const std::string& setter) {
    _topic = topic;
    _topicSetter = setter;
    _topicTimestamp = std::time(NULL); // current time
}

// Return the nickname of the user who set the topic
const std::string& Channel::getTopicSetter() const {
    return _topicSetter;
}

// Return the timestamp when the topic was set
time_t Channel::getTopicTimestamp() const {
    return _topicTimestamp;
}

// Set the creation time of the channel (usually when the channel is created)
void Channel::setCreationTime(time_t timestamp) {
    _creationTime = timestamp;
}

// Get the creation time of the channel
time_t Channel::getCreationTime() const {
    return _creationTime;
}

const std::vector<BanEntry>& Channel::getBanList() const {
    return _banList;
}

bool Channel::removeBan(const std::string& mask) {
    for (std::vector<BanEntry>::iterator it = _banList.begin(); it != _banList.end(); ++it) {
        if (it->mask == mask) {
            _banList.erase(it);
            return true;
        }
    }
    return false;
}

void Channel::addBan(const std::string& mask, const std::string& setBy) {
    BanEntry entry(mask, setBy);
    _banList.push_back(entry);
}