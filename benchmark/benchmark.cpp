#include <iostream>
#include <string>
#include <chrono>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int send_command(int sock, const std::string& cmd) {
    std::string msg = cmd + "\n";
    send(sock, msg.c_str(), msg.size(), 0);
    char buf[256];
    memset(buf, 0, sizeof(buf));
    recv(sock, buf, sizeof(buf) - 1, 0);
    return 0;
}

int connect_to_server(const std::string& host, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);
    connect(sock, (sockaddr*)&addr, sizeof(addr));
    return sock;
}

void benchmark(const std::string& host, int port, int num_ops) {
    int sock = connect_to_server(host, port);
    if (sock < 0) {
        std::cerr << "Failed to connect\n";
        return;
    }

    std::cout << "Running benchmark: " << num_ops << " ops each\n\n";

    // --- SET benchmark ---
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_ops; i++) {
        std::string cmd = "SET key" + std::to_string(i) + " value" + std::to_string(i);
        send_command(sock, cmd);
    }
    auto end = std::chrono::high_resolution_clock::now();
    double set_ms  = std::chrono::duration<double, std::milli>(end - start).count();
    double set_ops = (num_ops / set_ms) * 1000;
    std::cout << "SET: " << set_ms << " ms → " << (int)set_ops << " ops/sec\n";

    // --- GET benchmark ---
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_ops; i++) {
        std::string cmd = "GET key" + std::to_string(i);
        send_command(sock, cmd);
    }
    end = std::chrono::high_resolution_clock::now();
    double get_ms  = std::chrono::duration<double, std::milli>(end - start).count();
    double get_ops = (num_ops / get_ms) * 1000;
    std::cout << "GET: " << get_ms << " ms → " << (int)get_ops << " ops/sec\n";

    // --- DEL benchmark ---
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_ops; i++) {
        std::string cmd = "DEL key" + std::to_string(i);
        send_command(sock, cmd);
    }
    end = std::chrono::high_resolution_clock::now();
    double del_ms  = std::chrono::duration<double, std::milli>(end - start).count();
    double del_ops = (num_ops / del_ms) * 1000;
    std::cout << "DEL: " << del_ms << " ms → " << (int)del_ops << " ops/sec\n";

    close(sock);
}

int main(int argc, char* argv[]) {
    std::string host    = "127.0.0.1";
    int         port    = 6379;
    int         num_ops = 1000;

    if (argc >= 2) num_ops = std::stoi(argv[1]);

    std::cout << "╔══════════════════════════════╗\n";
    std::cout << "║     KV Store Benchmark       ║\n";
    std::cout << "╚══════════════════════════════╝\n\n";

    benchmark(host, port, num_ops);
    return 0;
}