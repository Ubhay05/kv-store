#include "server.h"
#include <iostream>

int main(int argc, char* argv[]) {
    int         port     = 6379;          // same default port as Redis
    std::string wal_path = "data/wal.log";

    // Allow custom port: ./kv-server 7000
    if (argc >= 2) {
        port = std::stoi(argv[1]);
    }

    std::cout << "╔══════════════════════════════╗\n";
    std::cout << "║      KV Store Server         ║\n";
    std::cout << "║   (Mini Redis in C++)        ║\n";
    std::cout << "╚══════════════════════════════╝\n\n";

    Server server(port, wal_path, 4);
    server.start();

    return 0;
}