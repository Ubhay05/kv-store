#include "wal.h"
#include <iostream>
#include <fstream>

WAL::WAL(const std::string& filepath) : filepath_(filepath) {
    // Open in append mode so we don't overwrite on restart
    file_.open(filepath_, std::ios::app);
    if (!file_.is_open()) {
        std::cerr << "[WAL] Failed to open log file: " << filepath_ << "\n";
    }
}

WAL::~WAL() {
    if (file_.is_open()) file_.close();
}

void WAL::log_set(const std::string& key, const std::string& value, int ttl) {
    if (!file_.is_open()) return;
    if (ttl > 0) {
        file_ << "SET " << key << " " << value << " EX " << ttl << "\n";
    } else {
        file_ << "SET " << key << " " << value << "\n";
    }
    file_.flush();   // immediately write to disk // if not it is saved to buffer and when crashed  gets erased, so write imm to disk
}

void WAL::log_del(const std::string& key) {
    if (!file_.is_open()) return;
    file_ << "DEL " << key << "\n";
    file_.flush();
}

void WAL::log_flush() {
    if (!file_.is_open()) return;
    file_ << "FLUSH\n";
    file_.flush();
}

void WAL::replay(std::function<void(const std::string&)> callback) { // when server again started run all the before commands
    std::ifstream infile(filepath_); //input file stream
    if (!infile.is_open()) {
        std::cout << "[WAL] No existing log file found. Starting fresh.\n";
        return;
    }

    std::string line;
    int count = 0;
    while (std::getline(infile, line)) {
        if (!line.empty()) {
            callback(line);
            count++;
        }
    }

    if (count > 0) {
        std::cout << "[WAL] Replayed " << count << " entries from log.\n";
    }
}

void WAL::clear() {
    if (file_.is_open()) file_.close();
    // Truncate the file
    std::ofstream ofs(filepath_, std::ios::trunc); // if closed then only we can clear
    ofs.close();
    // Reopen in append mode
    file_.open(filepath_, std::ios::app);
}