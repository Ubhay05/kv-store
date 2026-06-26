#pragma once
#include <string>
#include <vector>

enum class CommandType {
    SET,
    GET,
    DEL,
    EXISTS,
    KEYS,
    FLUSH,
    PING,
    UNKNOWN
};

struct Command {
    CommandType          type;
    std::vector<std::string> args;   // e.g. ["name", "Ubhay", "EX", "3600"]
};

// Parse raw string like "SET name Ubhay EX 3600" into Command struct
Command parse_command(const std::string& raw);