#pragma once
#include <string>
#include <fstream>
#include <functional>

// Write-Ahead Log — appends every write to disk
// On restart, replays log to rebuild store state
class WAL {
public:
    WAL(const std::string& filepath);
    ~WAL();

    // Log a SET or DEL command
    void log_set(const std::string& key, const std::string& value, int ttl = 0);
    void log_del(const std::string& key);
    void log_flush();

    // Replay log file — calls callback for each line
    // callback receives raw command string like "SET name Ubhay"
    void replay(std::function<void(const std::string&)> callback);

    // Clear the log file (e.g. after a snapshot)
    void clear();

private:
    std::string   filepath_;
    std::ofstream file_;
};