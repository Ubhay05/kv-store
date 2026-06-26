#include "parser.h"
#include <sstream>
#include <algorithm>
#include <cctype>

Command parse_command(const std::string& raw) {
    Command cmd;
    cmd.type = CommandType::UNKNOWN; //intialise to unknown

    if (raw.empty()) return cmd;

    // Split by whitespace
    std::istringstream iss(raw);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }

    if (tokens.empty()) return cmd;

    // First token is the command name — uppercase it
    std::string name = tokens[0];
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);

    // Map to CommandType
    if      (name == "SET")    cmd.type = CommandType::SET;
    else if (name == "GET")    cmd.type = CommandType::GET;
    else if (name == "DEL")    cmd.type = CommandType::DEL;
    else if (name == "EXISTS") cmd.type = CommandType::EXISTS;
    else if (name == "KEYS")   cmd.type = CommandType::KEYS;
    else if (name == "FLUSH")  cmd.type = CommandType::FLUSH;
    else if (name == "PING")   cmd.type = CommandType::PING;
    else                       cmd.type = CommandType::UNKNOWN;

    // Rest are args
    cmd.args = std::vector<std::string>(tokens.begin() + 1, tokens.end());

    return cmd;
}