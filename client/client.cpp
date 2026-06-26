#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, char* argv[]) {
    std::string host = "127.0.0.1";
    int         port = 6379;

    if (argc >= 3) {
        host = argv[1];
        port = std::stoi(argv[2]);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(port);
    inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr);

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed. Is the server running?\n";
        return 1;
    }

    std::cout << "Connected to KV Store at " << host << ":" << port << "\n";
    std::cout << "Commands: SET GET DEL EXISTS KEYS FLUSH PING\n";
    std::cout << "Type 'exit' to quit\n\n";

    char buffer[4096];

    while (true) {
        std::cout << "> ";
        std::string input;
        if (!std::getline(std::cin, input)) break;
        if (input == "exit" || input == "quit") break;
        if (input.empty()) continue;

        // Send command
        input += "\n";
        send(sock, input.c_str(), input.size(), 0);

        // Receive response
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            std::cout << "Server disconnected\n";
            break;
        }

        std::cout << buffer;
    }

    close(sock);
    return 0;
}