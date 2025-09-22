#include "kvstore/cli/CLI.hpp"
#include "kvstore/utils/Helpers.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <iomanip>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

using namespace kvspp::utils;

namespace kvspp {
    namespace cli {

        CLI::CLI()
            : manager_(kvstore::StoreManager::instance())
            , autoSave_(true)
            , verboseMode_(false)
            , jsonMode_(false) {
        }

        int CLI::runInteractive() {
            printWelcome();

            std::string input;
            while(true) {
                printPrompt();

                if(!std::getline(std::cin, input)) {
                    // EOF or error
                    std::cout << std::endl;
                    break;
                }

                // Trim whitespace
                input.erase(0, input.find_first_not_of(" \t"));
                input.erase(input.find_last_not_of(" \t") + 1);

                if(input.empty()) {
                    continue;
                }

                auto tokens = parseCommandLine(input);
                if(tokens.empty()) {
                    continue;
                }

                // Check for exit commands
                if(tokens[0] == "exit" || tokens[0] == "quit" || tokens[0] == "q") {
                    printInfo("Goodbye!");
                    break;
                }

                int result = processCommand(tokens);
                if(result == -2) {
                    // Special return code for exit
                    break;
                }
            }

            return 0;
        }

        int CLI::runSingleCommand(const std::vector<std::string>& args) {
            if(args.empty()) {
                printError("No command provided");
                return -1;
            }

            return processCommand(args);
        }

        int CLI::processCommand(const std::vector<std::string>& tokens) {
            if(tokens.empty()) {
                return -1;
            }

            const std::string& command = tokens[0];

            try {
                if(command == "get") {
                    return cmdGet(tokens);
                }
                else if(command == "put") {
                    return cmdPut(tokens);
                }
                else if(command == "delete" || command == "del") {
                    return cmdDelete(tokens);
                }
                else if(command == "search") {
                    return cmdSearch(tokens);
                }
                else if(command == "keys") {
                    return cmdKeys(tokens);
                }
                else if(command == "clear") {
                    return cmdClear(tokens);
                }
                else if(command == "save") {
                    return cmdSave(tokens);
                }
                else if(command == "load") {
                    return cmdLoad(tokens);
                }
                else if(command == "stats") {
                    return cmdStats(tokens);
                }
                else if(command == "inspect") {
                    return cmdInspect(tokens);
                }
                else if(command == "help" || command == "h") {
                    return cmdHelp(tokens);
                }
                else {
                    printError("Unknown command: " + command + ". Type 'help' for available commands.");
                    return -1;
                }
            }
            catch(const std::exception& e) {
                printError(std::string(e.what()));
                return -1;
            }
        }

        std::vector<std::string> CLI::parseCommandLine(const std::string& input) {
            std::vector<std::string> tokens;
            std::istringstream iss(input);
            std::string token;

            while(iss >> std::quoted(token)) {
                tokens.push_back(token);
            }

            return tokens;
        }

        int CLI::cmdGet(const std::vector<std::string>& args) {
            if(args.size() != 3) {
                printError("Usage: get <storeToken> <key>");
                return -1;
            }

            const std::string& storeToken = args[1];
            const std::string& key = args[2];

            try {
                std::string value = manager_.get(storeToken, key);

                if(jsonMode_) {
                    std::cout << valueToJson(value) << std::endl;
                }
                else {
                    printValue(key, value);
                }
                return 0;
            }
            catch(const std::exception& e) {
                if(jsonMode_) {
                    std::cout << "null" << std::endl;
                }
                else {
                    printError("Key '" + key + "' not found in store '" + storeToken + "'");
                }
                return 1;
            }
        }

        int CLI::cmdPut(const std::vector<std::string>& args) {
            if(args.size() != 4) {
                printError("Usage: put <storeToken> <key> <value>");
                return -1;
            }

            const std::string& storeToken = args[1];
            const std::string& key = args[2];
            const std::string& value = args[3];

            try {
                manager_.put(storeToken, key, value);

                // Auto-save if enabled
                if(autoSave_) {
                    try {
                        std::string fname = storeToken + ".json";
                        manager_.saveStore(storeToken, fname);
                        if(verboseMode_) {
                            printInfo("Auto-saved store '" + storeToken + "' to: " + fname);
                        }
                    }
                    catch(const std::exception& e) {
                        printError("Auto-save failed: " + std::string(e.what()));
                    }
                }

                if(jsonMode_) {
                    std::cout << "{\"success\": true}" << std::endl;
                }
                else {
                    printSuccess("Successfully stored key '" + key + "' in store '" + storeToken + "'");
                }
                return 0;
            }
            catch(const std::exception& e) {
                printError("Failed to store key: " + std::string(e.what()));
                return -1;
            }
        }

        int CLI::cmdDelete(const std::vector<std::string>& args) {
            if(args.size() != 3) {
                printError("Usage: delete <storeToken> <key>");
                return -1;
            }

            const std::string& storeToken = args[1];
            const std::string& key = args[2];

            try {
                manager_.remove(storeToken, key);

                // Auto-save if enabled
                if(autoSave_) {
                    try {
                        std::string fname = storeToken + ".json";
                        manager_.saveStore(storeToken, fname);
                        if(verboseMode_) {
                            printInfo("Auto-saved store '" + storeToken + "' to: " + fname);
                        }
                    }
                    catch(const std::exception& e) {
                        printError("Auto-save failed: " + std::string(e.what()));
                    }
                }

                if(jsonMode_) {
                    std::cout << "{\"success\": true}" << std::endl;
                }
                else {
                    printSuccess("Successfully deleted key '" + key + "' from store '" + storeToken + "'");
                }
                return 0;
            }
            catch(const std::exception& e) {
                printError("Key '" + key + "' not found in store '" + storeToken + "'");
                return 1;
            }
        }

        int CLI::cmdSearch(const std::vector<std::string>& args) {
            printError("Search command not yet implemented with StoreManager");
            return -1;
        }

        int CLI::cmdKeys(const std::vector<std::string>& args) {
            if(args.size() != 2) {
                printError("Usage: keys <storeToken>");
                return -1;
            }

            printError("Keys command not yet implemented with StoreManager");
            return -1;
        }

        int CLI::cmdClear(const std::vector<std::string>& args) {
            if(args.size() != 2) {
                printError("Usage: clear <storeToken>");
                return -1;
            }

            printError("Clear command not yet implemented with StoreManager");
            return -1;
        }

        int CLI::cmdSave(const std::vector<std::string>& args) {
            if(args.size() < 2 || args.size() > 3) {
                printError("Usage: save <storeToken> [filename]");
                return -1;
            }

            const std::string& storeToken = args[1];
            std::string filename = args.size() == 3 ? args[2] : (storeToken + ".json");

            try {
                // Ensure store directory exists
#ifdef _WIN32
                _mkdir("store");
#else
                mkdir("store", 0777);
#endif
                manager_.saveStore(storeToken, filename);

                if(jsonMode_) {
                    std::cout << "{\"success\": true}" << std::endl;
                }
                else {
                    printSuccess("Store '" + storeToken + "' saved to: " + filename);
                }
                return 0;
            }
            catch(const std::exception& e) {
                printError("Failed to save store: " + std::string(e.what()));
                return -1;
            }
        }

        int CLI::cmdLoad(const std::vector<std::string>& args) {
            if(args.size() < 2 || args.size() > 3) {
                printError("Usage: load <storeToken> [filename]");
                return -1;
            }

            const std::string& storeToken = args[1];
            std::string filename = args.size() == 3 ? args[2] : (storeToken + ".json");

            try {
                manager_.loadStore(storeToken, filename);

                if(jsonMode_) {
                    std::cout << "{\"success\": true}" << std::endl;
                }
                else {
                    printSuccess("Store '" + storeToken + "' loaded from: " + filename);
                }
                return 0;
            }
            catch(const std::exception& e) {
                printError("Failed to load store: " + std::string(e.what()));
                return -1;
            }
        }

        int CLI::cmdStats(const std::vector<std::string>& args) {
            if(args.size() != 2) {
                printError("Usage: stats <storeToken>");
                return -1;
            }

            printError("Stats command not yet implemented with StoreManager");
            return -1;
        }

        int CLI::cmdInspect(const std::vector<std::string>& args) {
            if(args.size() != 3) {
                printError("Usage: inspect <storeToken> <key>");
                return -1;
            }

            printError("Inspect command not yet implemented with StoreManager");
            return -1;
        }

        int CLI::cmdHelp(const std::vector<std::string>& args) {
            if(jsonMode_) {
                std::cout << "{\"commands\": [\"get\", \"put\", \"delete\", \"save\", \"load\", \"help\"]}" << std::endl;
            }
            else {
                std::cout << std::endl;
                std::cout << "=== KVS++ Store CLI Help ===" << std::endl;
                std::cout << std::endl;
                std::cout << "Data Operations:" << std::endl;
                std::cout << "  get <storeToken> <key>              - Get value for a key" << std::endl;
                std::cout << "  put <storeToken> <key> <value>       - Store key with value" << std::endl;
                std::cout << "  delete <storeToken> <key>            - Delete a key" << std::endl;
                std::cout << std::endl;
                std::cout << "File Operations:" << std::endl;
                std::cout << "  save <storeToken> [filename]         - Save store to file" << std::endl;
                std::cout << "  load <storeToken> [filename]         - Load store from file" << std::endl;
                std::cout << std::endl;
                std::cout << "Utility:" << std::endl;
                std::cout << "  help                                 - Show this help" << std::endl;
                std::cout << "  exit/quit/q                          - Exit (interactive mode)" << std::endl;
                std::cout << std::endl;
                std::cout << "Note: All commands now require a storeToken as the first argument." << std::endl;
                std::cout << "Example: get mystore user1" << std::endl;
                std::cout << std::endl;
            }
            return 0;
        }

        void CLI::printWelcome() {
            std::cout << std::endl;
            std::cout << "=== Welcome to KVS++ Store CLI ===" << std::endl;
            std::cout << "Multi-store support enabled. All commands require a storeToken." << std::endl;
            std::cout << "Type 'help' for available commands or 'exit' to quit." << std::endl;
            std::cout << std::endl;
        }

        void CLI::printPrompt() {
            std::cout << "kvs++ > ";
        }

        void CLI::printSuccess(const std::string& message) {
            if(!jsonMode_) {
                std::cout << ColorOutput::passMsg(message) << std::endl;
            }
        }

        void CLI::printError(const std::string& message) {
            if(!jsonMode_) {
                std::cerr << ColorOutput::failMsg(message) << std::endl;
            }
        }

        void CLI::printInfo(const std::string& message) {
            if(!jsonMode_) {
                std::cout << message << std::endl;
            }
        }

        void CLI::printValue(const std::string& key, const std::string& value) {
            std::cout << key << " -> " << value << std::endl;
        }

        std::string CLI::valueToJson(const std::string& value) {
            return "{\"value\":\"" + value + "\"}";
        }

        void CLI::autoSaveIfEnabled() {
            // In multi-store mode, auto-save happens after each put/delete operation
            // rather than at exit, since we save each store individually.
            if(autoSave_ && verboseMode_) {
                printInfo("Auto-save was enabled - stores were saved after each modification");
            }
        }

    }
}
