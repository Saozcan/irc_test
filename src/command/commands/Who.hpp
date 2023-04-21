#pragma once

#include "../ACommand.hpp"

class Who : public ACommand {
public:
    Who();
    ~Who();
    void execute(const std::vector<std::string>& splitArgs,  std::pair<const int, NormalUser*>& user, Server& server);
private:
};
