#include <iostream>
#include <string>
#include "kvstore/core/KeyValueStore.hpp"
#include "kvstore/net/TCPServer.hpp"

/**
 * TCP server main entry point
 * Runs the key-value store as a persistent TCP server
 */
int main(int argc, char* argv[]) {
    int port = 5555;
    if(argc > 1) {
        port = std::stoi(argv[1]);
    }
    kvspp::net::TCPServer server(port);
    std::cout << "KVS++ TCP server listening on port " << port << std::endl;
    server.start();
    // Wait for user to terminate
    std::cout << "Press Enter to stop the server..." << std::endl;
    std::cin.get();
    server.stop();
    return 0;
}
