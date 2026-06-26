#include "server.h"
#include "parser.h"
#include "ttl.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

Server::Server(int port, const std::string& wal_path, size_t num_threads)
    : port_(port), store_(1000), wal_(wal_path), pool_(num_threads) {
    restore_from_wal();
}

// Replay WAL on startup to rebuild store state
void Server::restore_from_wal() {
    wal_.replay([this](const std::string& line) {
        process_command(line);
    });
}

// Process a raw command string and return response
std::string Server::process_command(const std::string& raw) {
    Command cmd = parse_command(raw);

    switch (cmd.type) {

        case CommandType::PING:
            return "+PONG\n";

        case CommandType::SET: {
            if (cmd.args.size() < 2) return "-ERR SET requires key and value\n";
            std::string key   = cmd.args[0];
            std::string value = cmd.args[1];
            int ttl = 0;
            // Check for EX flag: SET key value EX 3600
            if (cmd.args.size() >= 4) {
                ttl = parse_ttl(cmd.args[2], cmd.args[3]);
            }
            store_.set(key, value, ttl);
            wal_.log_set(key, value, ttl);
            return "+OK\n";
        }

        case CommandType::GET: {
            if (cmd.args.empty()) return "-ERR GET requires key\n";
            auto val = store_.get(cmd.args[0]);
            if (!val.has_value()) return "-1\n";
            return val.value() + "\n";
        }

        case CommandType::DEL: {
            if (cmd.args.empty()) return "-ERR DEL requires key\n";
            bool deleted = store_.del(cmd.args[0]);
            wal_.log_del(cmd.args[0]);
            return deleted ? ":1\n" : ":0\n";
        }

        case CommandType::EXISTS: {
            if (cmd.args.empty()) return "-ERR EXISTS requires key\n";
            return store_.exists(cmd.args[0]) ? ":1\n" : ":0\n";
        }

        case CommandType::KEYS: {
            auto all_keys = store_.keys();
            std::ostringstream oss;
            oss << "*" << all_keys.size() << "\n";
            for (auto& k : all_keys) oss << k << "\n";
            return oss.str();
        }

        case CommandType::FLUSH: {
            store_.flush();
            wal_.log_flush();
            wal_.clear();
            return "+OK\n";
        }

        case CommandType::UNKNOWN:
        default:
            return "-ERR unknown command\n";
    }
}

// Handle one client connection
void Server::handle_client(int client_fd) {
    char buffer[4096];
    std::cout << "[server] client connected: fd=" << client_fd << "\n";

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes <= 0) {
            std::cout << "[server] client disconnected: fd=" << client_fd << "\n";
            break;
        }

        std::string raw(buffer, bytes);
        // Trim trailing newline/whitespace
        while (!raw.empty() && (raw.back() == '\n' || raw.back() == '\r' || raw.back() == ' ')) {
            raw.pop_back();
        }

        std::string response = process_command(raw);
        send(client_fd, response.c_str(), response.size(), 0);
    }

    close(client_fd);
}

// Start the TCP server
void Server::start() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "[server] Failed to create socket\n";
        return;
    }

    // Allow reuse of port immediately after restart
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port_);

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "[server] Bind failed on port " << port_ << "\n";
        close(server_fd);
        return;
    }

    if (listen(server_fd, 10) < 0) {
        std::cerr << "[server] Listen failed\n";
        close(server_fd);
        return;
    }

    std::cout << "[server] Listening on port " << port_ << " with 4 threads\n";
    std::cout << "[server] Commands: SET GET DEL EXISTS KEYS FLUSH PING\n";

    while (true) {
        sockaddr_in client_addr{};
        socklen_t   client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);

        if (client_fd < 0) {
            std::cerr << "[server] Accept failed\n";
            continue;
        }

        // Hand off to thread pool
        pool_.enqueue([this, client_fd] {
            handle_client(client_fd);
        });
    }

    close(server_fd);
}