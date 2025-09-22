#include "kvstore/net/TCPServer.hpp"
#include <iostream>
#include <cstring>
#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

#define BUFFER_SIZE 4096

namespace kvspp {
    namespace net {


        TCPServer::TCPServer(int port)
            : port_(port), running_(false), serverSock_(-1) {
        }

        TCPServer::~TCPServer() {
            stop();
        }

        void TCPServer::start() {
            if(running_) return;
            running_ = true;
            serverThread_ = std::thread(&TCPServer::run, this);
        }

        void TCPServer::stop() {
            running_ = false;
#ifdef _WIN32
            if(serverSock_ != INVALID_SOCKET) closesocket(serverSock_);
#else
            if(serverSock_ != -1) close(serverSock_);
#endif
            if(serverThread_.joinable()) serverThread_.join();
        }

        bool TCPServer::isRunning() const {
            return running_;
        }

        void TCPServer::run() {
#ifdef _WIN32
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
            serverSock_ = socket(AF_INET, SOCK_STREAM, 0);
            if(serverSock_ < 0) {
                std::cerr << "Failed to create socket\n";
                return;
            }
            sockaddr_in serverAddr{};
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_addr.s_addr = INADDR_ANY;
            serverAddr.sin_port = htons(port_);
            if(bind(serverSock_, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
                std::cerr << "Bind failed\n";
                return;
            }
            if(listen(serverSock_, 5) < 0) {
                std::cerr << "Listen failed\n";
                return;
            }
            while(running_) {
                sockaddr_in clientAddr;
#ifdef _WIN32
                int clientLen = sizeof(clientAddr);
                int clientSock = accept(serverSock_, (struct sockaddr*)&clientAddr, &clientLen);
#else
                socklen_t clientLen = sizeof(clientAddr);
                int clientSock = accept(serverSock_, (struct sockaddr*)&clientAddr, &clientLen);
#endif
                if(clientSock < 0) continue;
                std::thread(&TCPServer::handleClient, this, clientSock).detach();
            }
#ifdef _WIN32
            closesocket(serverSock_);
            WSACleanup();
#else
            close(serverSock_);
#endif
        }

#include <stdexcept>
        void TCPServer::handleClient(int clientSock) {
            char buffer[BUFFER_SIZE];
            std::string partial;
            std::string selectedToken;
            bool autosave = true;
            while(true) {
#ifdef _WIN32
                int bytes = recv(clientSock, buffer, BUFFER_SIZE - 1, 0);
#else
                ssize_t bytes = recv(clientSock, buffer, BUFFER_SIZE - 1, 0);
#endif
                if(bytes <= 0) break;
                buffer[bytes] = '\0';
                partial += buffer;
                size_t pos;
                while((pos = partial.find('\n')) != std::string::npos) {
                    std::string line = partial.substr(0, pos);
                    partial.erase(0, pos + 1);
                    // Trim trailing \r if present
                    if(!line.empty() && line.back() == '\r') line.pop_back();
                    std::string response = handleCommand(line, selectedToken, autosave);
                    send(clientSock, response.c_str(), response.size(), 0);
                    if(line == "QUIT") return;
                }
            }
#ifdef _WIN32
            closesocket(clientSock);
#else
            close(clientSock);
#endif
        }

        // Helper: split string by space, preserving quoted values for SET

    } // namespace net
} // namespace kvspp

// Definitions must be outside the namespace block
std::vector<std::string> kvspp::net::TCPServer::splitCommand(const std::string& line) {
    std::vector<std::string> tokens;
    std::string token;
    bool inQuotes = false;
    for(size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if(c == '"') inQuotes = !inQuotes;
        else if(c == ' ' && !inQuotes) {
            if(!token.empty()) { tokens.push_back(token); token.clear(); }
        }
        else token += c;
    }
    if(!token.empty()) tokens.push_back(token);
    return tokens;
}

std::string kvspp::net::TCPServer::handleCommand(const std::string& line, std::string& selectedToken, bool& autosave) {
    auto tokens = splitCommand(line);
    if(tokens.empty()) return "ERROR Empty command\n";
    std::string cmd = tokens[0];
    for(auto& c : cmd) c = toupper(c);
    if(cmd == "SELECT") {
        if(tokens.size() != 2) return "ERROR Usage: SELECT <storetoken>\n";
        selectedToken = tokens[1];
        return "OK\n";
    }
    if(cmd == "AUTOSAVE") {
        if(tokens.size() != 2) return "ERROR Usage: AUTOSAVE ON|OFF\n";
        std::string val = tokens[1];
        for(auto& c : val) c = toupper(c);
        if(val == "ON") { autosave = true; return "OK\n"; }
        else if(val == "OFF") { autosave = false; return "OK\n"; }
        else return "ERROR Usage: AUTOSAVE ON|OFF\n";
    }
    if(selectedToken.empty()) {
        return "ERROR No store selected. Use SELECT <storetoken> first.\n";
    }
    auto& store = kvstore::StoreManager::instance().getStore(selectedToken);
    if(cmd == "GET") {
        if(tokens.size() != 2) return "ERROR Usage: GET <key>\n";
        const kvspp::core::ValueObject* val = store.get(tokens[1]);
        if(!val) return "NOT_FOUND\n";
        return "VALUE " + val->toString() + "\n";
    }
    else if(cmd == "SET") {
        if(tokens.size() < 3) return "ERROR Usage: SET <key> <value>\n";
        store.put(tokens[1], { {"value", tokens[2]} });
        if(autosave) {
            try {
                kvstore::StoreManager::instance().saveStore(selectedToken, selectedToken + ".json");
            }
            catch(const std::exception& e) {
                return std::string("ERROR Autosave failed: ") + e.what() + "\n";
            }
        }
        return "OK\n";
    }
    else if(cmd == "DELETE") {
        if(tokens.size() != 2) return "ERROR Usage: DELETE <key>\n";
        bool removed = store.deleteKey(tokens[1]);
        if(autosave) {
            try {
                kvstore::StoreManager::instance().saveStore(selectedToken, selectedToken + ".json");
            }
            catch(const std::exception& e) {
                return std::string("ERROR Autosave failed: ") + e.what() + "\n";
            }
        }
        return removed ? "OK\n" : "NOT_FOUND\n";
    }
    else if(cmd == "SAVE") {
        if(tokens.size() != 2) return "ERROR Usage: SAVE <filename>\n";
        std::string filename = tokens[1];
        // Always use project root's store/ directory for relative paths
        if(!(filename.size() > 1 && (filename[1] == ':' || filename[0] == '/' || filename[0] == '\\'))) {
            filename = "../../store/" + filename;
        }
        try {
            kvstore::StoreManager::instance().saveStore(selectedToken, filename);
            return "OK\n";
        }
        catch(const std::exception& e) {
            return std::string("ERROR Save failed: ") + e.what() + "\n";
        }
    }
    else if(cmd == "LOAD") {
        if(tokens.size() != 2) return "ERROR Usage: LOAD <filename>\n";
        std::string filename = tokens[1];
        if(!(filename.size() > 1 && (filename[1] == ':' || filename[0] == '/' || filename[0] == '\\'))) {
            filename = "../../store/" + filename;
        }
        try {
            kvstore::StoreManager::instance().loadStore(selectedToken, filename);
            return "OK\n";
        }
        catch(const std::exception& e) {
            return std::string("ERROR Load failed: ") + e.what() + "\n";
        }
    }
    else if(cmd == "QUIT") {
        return "OK\n";
    }
    else {
        return "ERROR Unknown command\n";
    }
}
