#ifndef KVSPP_TCP_SERVER_HPP
#define KVSPP_TCP_SERVER_HPP

#include <thread>
#include <atomic>
#include <string>
#include <memory>
#include "kvstore/core/KeyValueStore.hpp"

namespace kvspp {
    namespace net {

        class TCPServer {
        public:
            TCPServer(kvspp::core::KeyValueStore& store, int port = 5555);
            ~TCPServer();

            void start();
            void stop();
            bool isRunning() const;

        private:
            void run();
            void handleClient(int clientSock);

            kvspp::core::KeyValueStore& store_;
            int port_;
            std::thread serverThread_;
            std::atomic<bool> running_;
            int serverSock_;
        };

    } // namespace net
} // namespace kvspp

#endif // KVSPP_TCP_SERVER_HPP
