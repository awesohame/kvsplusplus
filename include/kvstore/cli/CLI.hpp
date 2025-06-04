#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "kvstore/core/KeyValueStore.hpp"

namespace kvspp {
    namespace cli {

        /**
         * Command line interface for the key-value store
         * Supports both interactive and single-command modes
         */
        class CLI {
        private:
            std::unique_ptr<core::KeyValueStore> store_;
            std::string defaultStoreFile_;
            bool autoSave_;
            bool verboseMode_;
            bool jsonMode_;

        public:
            // Constructor
            explicit CLI(const std::string& defaultStoreFile = "store/store.json");

            // Destructor
            ~CLI();

            // Main entry points
            int runInteractive();
            int runSingleCommand(const std::vector<std::string>& args);

            // Configuration
            void setAutoSave(bool enable) { autoSave_ = enable; }
            void setVerboseMode(bool enable) { verboseMode_ = enable; }
            void setJsonMode(bool enable) { jsonMode_ = enable; }
            void setDefaultStoreFile(const std::string& filename) { defaultStoreFile_ = filename; }

        private:
            // Command processing
            int processCommand(const std::vector<std::string>& tokens);
            std::vector<std::string> parseCommandLine(const std::string& input);

            // Individual commands
            int cmdGet(const std::vector<std::string>& args);
            int cmdPut(const std::vector<std::string>& args);
            int cmdDelete(const std::vector<std::string>& args);
            int cmdSearch(const std::vector<std::string>& args);
            int cmdKeys(const std::vector<std::string>& args);
            int cmdClear(const std::vector<std::string>& args);
            int cmdSave(const std::vector<std::string>& args);
            int cmdLoad(const std::vector<std::string>& args);
            int cmdStats(const std::vector<std::string>& args);
            int cmdTypes(const std::vector<std::string>& args);
            int cmdInspect(const std::vector<std::string>& args);
            int cmdHelp(const std::vector<std::string>& args);

            // Utility functions
            void printWelcome();
            void printPrompt();
            void printError(const std::string& message);
            void printSuccess(const std::string& message);
            void printInfo(const std::string& message);
            void printValue(const std::string& key, const core::ValueObject* value);
            void printKeyList(const std::vector<std::string>& keys);

            // JSON output helpers
            std::string valueToJson(const core::ValueObject* value);
            std::string keysToJson(const std::vector<std::string>& keys);

            // Helper for parsing attribute pairs
            std::vector<std::pair<std::string, std::string>> parseAttributePairs(
                const std::vector<std::string>& args, size_t startIndex);

            // Auto-save functionality
            void autoSaveIfEnabled();
        };

        /**
         * Utility class for colored console output
         */
        class ColorOutput {
        public:
            static std::string red(const std::string& text);
            static std::string green(const std::string& text);
            static std::string yellow(const std::string& text);
            static std::string blue(const std::string& text);
            static std::string cyan(const std::string& text);
            static std::string bold(const std::string& text);
            static std::string reset();
        };

    }
}