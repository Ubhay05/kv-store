#include "ttl.h"
#include <stdexcept>
#include <algorithm>
#include <cctype>

int parse_ttl(const std::string& ex_flag, const std::string& seconds_str) {
    // Convert flag to uppercase for case-insensitive comparison
    std::string flag = ex_flag;
    std::transform(flag.begin(), flag.end(), flag.begin(), ::toupper);

    if (flag != "EX") return 0;

    try {
        int secs = std::stoi(seconds_str);
        return (secs > 0) ? secs : 0;
    } catch (...) {
        return 0;
    }
}