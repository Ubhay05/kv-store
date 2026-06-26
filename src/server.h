#pragma once
#include "store.h"
#include "wal.h"
#include "thread_pool.h"
#include <string>

class Server {
public:
    Server(int port, const std::string& wal_path, size_t num_threads = 4);
    void start();

private:
    int         port_;
    Store       store_;
    WAL         wal_;
    ThreadPool  pool_;

    void handle_client(int client_fd);
    std::string process_command(const std::string& raw);
    void        restore_from_wal();
};