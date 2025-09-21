#include "kvstore/TCPServer.hpp"
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

        TCPServer::TCPServer(kvspp::core::KeyValueStore& store, int port)
            : store_(store), port_(port), running_(false), serverSock_(-1) {
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

        void TCPServer::handleClient(int clientSock) {
            char buffer[BUFFER_SIZE];
            std::string partial;
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
                    // TODO: Parse and handle the command, send response
                    std::string response = "ERROR Not implemented\n";
                    send(clientSock, response.c_str(), response.size(), 0);
                }
            }
#ifdef _WIN32
            closesocket(clientSock);
#else
            close(clientSock);
#endif
        }

    } // namespace net
} // namespace kvspp
