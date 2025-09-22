#ifndef KVSPP_TCP_SERVER_HPP
#define KVSPP_TCP_SERVER_HPP

#include <thread>
#include <atomic>
#include <string>
#include <memory>
#include "kvstore/core/StoreManager.hpp"

namespace kvspp {
    namespace net {

        class TCPServer {
        public:
            TCPServer(int port = 5555);
            ~TCPServer();

            void start();
            void stop();
            bool isRunning() const;

        private:
            void run();
            void handleClient(int clientSock);
            std::string handleCommand(const std::string& line, std::string& selectedToken, bool& autosave);
            static std::vector<std::string> splitCommand(const std::string& line);

            int port_;
            std::thread serverThread_;
            std::atomic<bool> running_;
            int serverSock_;
        };

    } // namespace net
} // namespace kvspp

#endif // KVSPP_TCP_SERVER_HPP
