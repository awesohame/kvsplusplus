#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "kvstore/core/StoreManager.hpp"

namespace kvspp {
    namespace cli {

        /**
         * Command line interface for the key-value store
         * Supports both interactive and single-command modes with multiple stores
         * All commands now require a storeToken as the first argument
         */
        class CLI {
        private:
            kvstore::StoreManager& manager_;
            bool autoSave_;
            bool verboseMode_;
            bool jsonMode_;

        public:
            // Constructor
            explicit CLI();

            // Destructor
            ~CLI() = default;

            // Main entry points
            int runInteractive();
            int runSingleCommand(const std::vector<std::string>& args);

            // Configuration
            void setAutoSave(bool enable) { autoSave_ = enable; }
            void setVerboseMode(bool enable) { verboseMode_ = enable; }
            void setJsonMode(bool enable) { jsonMode_ = enable; }

        private:
            // Command processing
            int processCommand(const std::vector<std::string>& tokens);
            std::vector<std::string> parseCommandLine(const std::string& input);

            // Command implementations (all require storeToken as first arg)
            int cmdGet(const std::vector<std::string>& args);
            int cmdPut(const std::vector<std::string>& args);
            int cmdDelete(const std::vector<std::string>& args);
            int cmdSearch(const std::vector<std::string>& args);
            int cmdKeys(const std::vector<std::string>& args);
            int cmdClear(const std::vector<std::string>& args);
            int cmdSave(const std::vector<std::string>& args);
            int cmdLoad(const std::vector<std::string>& args);
            int cmdStats(const std::vector<std::string>& args);
            int cmdInspect(const std::vector<std::string>& args);
            int cmdHelp(const std::vector<std::string>& args);

            // Output helpers
            void printWelcome();
            void printPrompt();
            void printSuccess(const std::string& message);
            void printError(const std::string& message);
            void printInfo(const std::string& message);
            void printValue(const std::string& key, const std::string& value);
            std::string valueToJson(const std::string& value);

            // Auto-save functionality
            void autoSaveIfEnabled();
        };

    }
}
