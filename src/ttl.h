#pragma once
// TTL logic is handled directly inside store.cpp via expires_at timestamp.
// This header exposes helper utilities for TTL parsing.
#include <string>
#include <ctime>

// Parse "EX 3600" from command args → returns seconds (0 if not present)
int parse_ttl(const std::string& ex_flag, const std::string& seconds_str);