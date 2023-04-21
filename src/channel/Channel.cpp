#include "Channel.hpp"
#include "../utility/Utility.hpp"

Channel::Channel() {}

Channel::Channel(const std::string &name) : _name(name){}

Channel::~Channel() {}

void Channel::setChannelName(const std::string &name) {
    _name = name;
}

void Channel::addUser(NormalUser *user) {
    if(_users.empty()) {
        _operators.insert(std::pair<std::string, NormalUser *>(user->getNick(), user));
        _users.insert(std::pair<std::string, NormalUser* >(user->getNick(), user));
        return ;
    }
    std::map<std::string, NormalUser*>::iterator it = _users.find(user->getNick());
    if(it == _users.end())
        _users.insert(std::pair<std::string, NormalUser* >(user->getNick(), user));
}

void Channel::leaveUser(const std::string& name) {
    _users.erase(name);
}

void Channel::addMode(const std::string& user) {
    std::map<std::string, NormalUser*>::iterator it = _users.find(user);
    if(it != _users.end())
        _operators.insert(std::pair<std::string, NormalUser*>((*it).second->getNick(), (*it).second));
    else{
        std::cout << "User cannot be found!" << std::endl;
    }
}

void Channel::removeMode(const std::string& user) {
    std::map<std::string, NormalUser*>::iterator opIt = _operators.find(user);
    if(opIt != _operators.end()) {
        _operators.erase(opIt);
        std::cout << "User " << user << " has been removed from operators." << std::endl;
    }
    else
        std::cout << "User " << user << " is not an operator." << std::endl;
}

bool Channel::checkOperators(const std::string& user) {
    std::map<std::string, NormalUser*>::iterator it = _operators.find(user);
    if(it != _operators.end())
        return true;
    return false;
}

void Channel::sendMessage(const std::string &from, std::string &message, bool isMessage) {
    std::map<std::string, NormalUser*>::iterator it = _users.begin();
    for (; it != _users.end(); it++) {
        if (it->first == from && isMessage)
            continue;
        Utility::sendToClient((*it).second->getPoll().fd, message);
    }
}

void Channel::irc366(int fd) {
    std::string message = ":ircserv 366 #" + _name + " :";
    message += this->getUsers();
    std::cout << message << std::endl;
    message += ":End of /NAMES list\r\n";
    Utility::sendToClient(fd, message);
}

bool Channel::isEmpty() {
    if (this->_users.empty())
        return true;
    return false;
}

NormalUser *Channel::getUser(const std::string &nick) const {
    std::map<std::string, NormalUser*>::const_iterator it = _users.find(nick);
    if (it != _users.end())
        return it->second;
    return nullptr;
}

void Channel::writeUsers() const {
    std::map<std::string, NormalUser*>::const_iterator it = _users.begin();
    for (; it != _users.end(); it++) {
        std::cout << "User: " << it->first << std::endl;
    }
}

void Channel::whoReply(const std::string &nick) const {
    std::map<std::string, NormalUser*>::const_iterator it = _users.find(nick);
    if (it != _users.end()) {
        std::string message = ":ircserv 352 " + nick + " #" + _name + " " + it->second->getNick() + " " + it->second->getHostname() + " ircServ" + " " + it->second->getName() + " H :0 " + it->second->getRealName() + "\r\n";
        Utility::sendToClient(it->second->getPoll().fd, message);
    }
}

std::string Channel::getUsers() const {
    std::map<std::string, NormalUser*>::const_iterator it = _users.begin();
    std::string returnString;
    for (; it != _users.end(); it++) {
        returnString += it->first + " ";
    }
    return returnString;
}

void Channel::irc353(int fd) {
    std::string message = ":ircserv 353 " + _name + " = #" + _name + " :";
    std::map<std::string, NormalUser*>::iterator it = _users.begin();
    message += this->getUsers();
    std::cout << message << std::endl;
    message += "\r\n";
    Utility::sendToClient(fd, message);
}

void Channel::sendMode(int fd, const std::string& mode) const {
    std::string message = ":ircserv MODE #" + _name + " " + mode + " ";
    message += this->getUsers();
    message += "\r\n";
    for (std::map<std::string, NormalUser*>::const_iterator it = _users.begin(); it != _users.end(); it++) {
        Utility::sendToClient(it->second->getPoll().fd, message);
    }
}
