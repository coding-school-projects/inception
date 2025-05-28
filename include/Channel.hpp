#pragma once

#include "Headers.hpp"

struct BanEntry {
    std::string mask;
    std::string setBy;
    std::time_t timestamp;

    BanEntry() {}
    BanEntry(const std::string& m, const std::string& s)
        : mask(m), setBy(s), timestamp(std::time(nullptr)) {}
};

class Channel {
private:
    std::string _name;                // Channel name (starts with #)
    std::string _topic;               // Channel topic
    std::vector<int> _clients;        // List of client FDs in channel
    std::vector<int> _operators;      // List of operator client FDs
    bool _inviteOnly;                 // Invite-only flag
    bool _topicRestricted;            // Topic restriction flag
    std::string _key;                 // Channel password/key
    int _userLimit;                   // Maximum users (-1 for no limit)
    std::vector<int> _invitedClients; // Track invited clients

    std::string _topicSetter;
    time_t _topicTimestamp;
    time_t _creationTime;

    std::vector<BanEntry> _banList;

public:
    Channel() {} 
    Channel(const std::string& name);
    
    // Client management
    void addClient(int clientFd);
    void removeClient(int clientFd);
    
    // Operator management
    void addOperator(int clientFd);
    void removeOperator(int clientFd);
    bool isOperator(int clientFd) const;
    
    // Topic management
    void setTopic(const std::string& topic);
    const std::string& getTopic() const;
    
    // Mode management
    void setInviteOnly(bool inviteOnly);
    bool isInviteOnly() const;
    void setTopicRestricted(bool restricted);
    bool isTopicRestricted() const;
    void setKey(const std::string& key);
    bool checkKey(const std::string& key) const;
    void setUserLimit(int limit);
    int getUserLimit() const;
    
    // Getters
    const std::vector<int>& getClients() const;
    const std::string& getName() const;
    const std::string& getKey() const;

    void addInvited(int clientFd);
    void removeInvited(int clientFd);
    bool isInvited(int clientFd) const;
	bool hasClient(int clientFd) const;
	const std::vector<int>& getOperators() const;

    void setTopic(const std::string& topic, const std::string& setter);
    const std::string& getTopicSetter() const;
    time_t getTopicTimestamp() const;
    void setCreationTime(time_t timestamp);
    time_t getCreationTime() const;

    const std::vector<BanEntry>& getBanList() const;
    void addBan(const std::string& mask, const std::string& setBy);
    bool removeBan(const std::string& mask);
};