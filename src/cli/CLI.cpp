#include "kvstore/cli/CLI.hpp"
#include "kvstore/exceptions/Exceptions.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <iomanip>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#endif

namespace kvspp {
    namespace cli {
        CLI::CLI(const std::string& defaultStoreFile)
            : store_(std::make_unique<core::KeyValueStore>())
            , defaultStoreFile_(defaultStoreFile)
            , autoSave_(true)
            , verboseMode_(false)
            , jsonMode_(false) {            // Only try to load existing store file if it's the default store/store.json
            // For custom files specified with -f, start with an empty store unless the file exists
            if(defaultStoreFile_ == "store/store.json") {
                // Always try to load the default store, create it if it doesn't exist
                try {
                    store_->load(defaultStoreFile_);
                    if(verboseMode_) {
                        printInfo("Loaded existing store from: " + defaultStoreFile_);
                    }
                }
                catch(const exceptions::KVStoreException&) {
                    // File doesn't exist or is invalid, start with empty store
                    if(verboseMode_) {
                        printInfo("Starting with empty store");
                    }
                }
            }
            else if(std::filesystem::exists(defaultStoreFile_)) {
             // For custom files, only load if they already exist
                try {
                    store_->load(defaultStoreFile_);
                    if(verboseMode_) {
                        printInfo("Loaded existing store from: " + defaultStoreFile_);
                    }
                }
                catch(const exceptions::KVStoreException&) {
                    // File exists but is invalid, start with empty store
                    if(verboseMode_) {
                        printInfo("Starting with empty store (existing file was invalid)");
                    }
                }
            }
            else {
             // Custom file doesn't exist, start with empty store
                if(verboseMode_) {
                    printInfo("Starting with empty store for new file: " + defaultStoreFile_);
                }
            }
        }

        CLI::~CLI() {
            autoSaveIfEnabled();
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
                    autoSaveIfEnabled();
                    printInfo("Goodbye!");
                    break;
                }

                int result = processCommand(tokens);
                if(result < 0) {
                    // Error occurred, but continue in interactive mode
                    continue;
                }
            }

            return 0;
        }

        int CLI::runSingleCommand(const std::vector<std::string>& args) {
            if(args.empty()) {
                printError("No command specified");
                return 1;
            }

            int result = processCommand(args);
            autoSaveIfEnabled();
            return result;
        }

        int CLI::processCommand(const std::vector<std::string>& tokens) {
            if(tokens.empty()) {
                return 0;
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
                else if(command == "search" || command == "find") {
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
                else if(command == "types") {
                    return cmdTypes(tokens);
                }
                else if(command == "inspect") {
                    return cmdInspect(tokens);
                }
                else if(command == "help" || command == "h" || command == "?") {
                    return cmdHelp(tokens);
                }
                else {
                    printError("Unknown command: " + command + ". Type 'help' for available commands.");
                    return -1;
                }
            }
            catch(const exceptions::KVStoreException& e) {
                printError("KVStore Error: " + std::string(e.what()));
                return -1;
            }
            catch(const std::exception& e) {
                printError("Error: " + std::string(e.what()));
                return -1;
            }
        }

        std::vector<std::string> CLI::parseCommandLine(const std::string& input) {
            std::vector<std::string> tokens;
            std::istringstream iss(input);
            std::string token;

            while(iss >> token) {
                tokens.push_back(token);
            }

            return tokens;
        }

        int CLI::cmdGet(const std::vector<std::string>& args) {
            if(args.size() != 2) {
                printError("Usage: get <key>");
                return -1;
            }

            const std::string& key = args[1];
            const auto* value = store_->get(key);

            if(!value) {
                if(jsonMode_) {
                    std::cout << "null" << std::endl;
                }
                else {
                    printError("Key '" + key + "' not found");
                }
                return 1;
            }

            if(jsonMode_) {
                std::cout << valueToJson(value) << std::endl;
            }
            else {
                printValue(key, value);
            }

            return 0;
        }

        int CLI::cmdPut(const std::vector<std::string>& args) {
            if(args.size() < 3) {
                printError("Usage: put <key> <attr1:value1> [attr2:value2] ...");
                printInfo("Example: put user1 name:John age:25 active:true");
                return -1;
            }

            const std::string& key = args[1];
            auto attributePairs = parseAttributePairs(args, 2);

            if(attributePairs.empty()) {
                printError("No valid attribute pairs provided");
                return -1;
            }

            store_->put(key, attributePairs);

            if(jsonMode_) {
                std::cout << "{\"status\":\"success\",\"key\":\"" << key << "\"}" << std::endl;
            }
            else {
                printSuccess("Successfully stored key '" + key + "' with " +
                    std::to_string(attributePairs.size()) + " attributes");
            }

            return 0;
        }

        int CLI::cmdDelete(const std::vector<std::string>& args) {
            if(args.size() != 2) {
                printError("Usage: delete <key>");
                return -1;
            }

            const std::string& key = args[1];
            bool deleted = store_->deleteKey(key);

            if(jsonMode_) {
                std::cout << "{\"deleted\":" << (deleted ? "true" : "false") << "}" << std::endl;
            }
            else {
                if(deleted) {
                    printSuccess("Successfully deleted key '" + key + "'");
                }
                else {
                    printError("Key '" + key + "' not found");
                }
            }

            return deleted ? 0 : 1;
        }

        int CLI::cmdSearch(const std::vector<std::string>& args) {
            if(args.size() != 3) {
                printError("Usage: search <attribute_key> <attribute_value>");
                printInfo("Example: search age 25");
                return -1;
            }

            const std::string& attrKey = args[1];
            const std::string& attrValue = args[2];

            auto results = store_->search(attrKey, attrValue);

            if(jsonMode_) {
                std::cout << keysToJson(results) << std::endl;
            }
            else {
                if(results.empty()) {
                    printInfo("No keys found matching " + attrKey + "=" + attrValue);
                }
                else {
                    printInfo("Found " + std::to_string(results.size()) + " keys:");
                    printKeyList(results);
                }
            }

            return 0;
        }

        int CLI::cmdKeys(const std::vector<std::string>& args) {
            auto keys = store_->keys();

            if(jsonMode_) {
                std::cout << keysToJson(keys) << std::endl;
            }
            else {
                if(keys.empty()) {
                    printInfo("No keys in store");
                }
                else {
                    printInfo("Store contains " + std::to_string(keys.size()) + " keys:");
                    printKeyList(keys);
                }
            }

            return 0;
        }

        int CLI::cmdClear(const std::vector<std::string>& args) {
            size_t oldSize = store_->size();
            store_->clear();

            if(jsonMode_) {
                std::cout << "{\"cleared\":" << oldSize << "}" << std::endl;
            }
            else {
                printSuccess("Cleared " + std::to_string(oldSize) + " entries from store");
            }

            return 0;
        }

        int CLI::cmdSave(const std::vector<std::string>& args) {
            std::string filename = defaultStoreFile_;
            if(args.size() > 1) {
                filename = args[1];
            }

            store_->save(filename);

            if(jsonMode_) {
                std::cout << "{\"saved\":\"" << filename << "\"}" << std::endl;
            }
            else {
                printSuccess("Store saved to: " + filename);
            }

            return 0;
        }

        int CLI::cmdLoad(const std::vector<std::string>& args) {
            std::string filename = defaultStoreFile_;
            if(args.size() > 1) {
                filename = args[1];
            }

            size_t oldSize = store_->size();
            store_->load(filename);
            size_t newSize = store_->size();

            if(jsonMode_) {
                std::cout << "{\"loaded\":\"" << filename << "\",\"entries\":" << newSize << "}" << std::endl;
            }
            else {
                printSuccess("Loaded " + std::to_string(newSize) + " entries from: " + filename);
                if(oldSize > 0) {
                    printInfo("Previous store had " + std::to_string(oldSize) + " entries");
                }
            }

            return 0;
        }

        int CLI::cmdStats(const std::vector<std::string>& args) {
            size_t size = store_->size();
            auto keys = store_->keys();

            if(jsonMode_) {
                std::cout << "{\"size\":" << size << ",\"empty\":" << (size == 0 ? "true" : "false") << "}" << std::endl;
            }
            else {
                std::cout << ColorOutput::bold("=== Store Statistics ===") << std::endl;
                std::cout << "Total entries: " << ColorOutput::cyan(std::to_string(size)) << std::endl;
                std::cout << "Empty: " << (size == 0 ? ColorOutput::red("Yes") : ColorOutput::green("No")) << std::endl;
                std::cout << "Default file: " << ColorOutput::yellow(defaultStoreFile_) << std::endl;
                std::cout << "Auto-save: " << (autoSave_ ? ColorOutput::green("Enabled") : ColorOutput::red("Disabled")) << std::endl;
            }

            return 0;
        }

        int CLI::cmdTypes(const std::vector<std::string>& args) {
            // This would require exposing TypeRegistry information
            // For now, just show a placeholder
            if(jsonMode_) {
                std::cout << "{\"message\":\"Type information not yet implemented\"}" << std::endl;
            }
            else {
                printInfo("Type registry information not yet implemented");
                printInfo("This feature will show registered attribute types");
            }

            return 0;
        }

        int CLI::cmdInspect(const std::vector<std::string>& args) {
            if(args.size() != 2) {
                printError("Usage: inspect <key>");
                return -1;
            }

            const std::string& key = args[1];
            const auto* value = store_->get(key);

            if(!value) {
                printError("Key '" + key + "' not found");
                return 1;
            }

            if(jsonMode_) {
                std::cout << valueToJson(value) << std::endl;
            }
            else {
                std::cout << ColorOutput::bold("=== Inspecting Key: " + key + " ===") << std::endl;
                const auto& attributes = value->getAttributes();
                std::cout << "Attribute count: " << ColorOutput::cyan(std::to_string(attributes.size())) << std::endl;
                std::cout << "String representation: " << ColorOutput::yellow(value->toString()) << std::endl;
                std::cout << "Raw attributes:" << std::endl;

                for(const auto& [attrName, attrValue] : attributes) {
                    std::cout << "  " << ColorOutput::green(attrName) << " = ";

                    std::visit([](const auto& val) {
                        using T = std::decay_t<decltype(val)>;
                        if constexpr(std::is_same_v<T, std::string>) {
                            std::cout << ColorOutput::yellow("\"" + val + "\"") << " (string)";
                        }
                        else if constexpr(std::is_same_v<T, int>) {
                            std::cout << ColorOutput::cyan(std::to_string(val)) << " (int)";
                        }
                        else if constexpr(std::is_same_v<T, double>) {
                            std::cout << ColorOutput::cyan(std::to_string(val)) << " (double)";
                        }
                        else if constexpr(std::is_same_v<T, bool>) {
                            std::cout << (val ? ColorOutput::green("true") : ColorOutput::red("false")) << " (bool)";
                        }
                    }, attrValue);

                    std::cout << std::endl;
                }
            }

            return 0;
        }

        int CLI::cmdHelp(const std::vector<std::string>& args) {
            if(jsonMode_) {
                std::cout << "{\"help\":\"Available commands: get, put, delete, search, keys, clear, save, load, stats, inspect, help\"}" << std::endl;
                return 0;
            }

            std::cout << ColorOutput::bold("=== KVS++ Store CLI Help ===") << std::endl << std::endl;

            std::cout << ColorOutput::green("Data Operations:") << std::endl;
            std::cout << "  " << ColorOutput::cyan("get <key>") << "                     - Get value for a key" << std::endl;
            std::cout << "  " << ColorOutput::cyan("put <key> <attr:val> ...") << "      - Store key with attributes" << std::endl;
            std::cout << "  " << ColorOutput::cyan("delete <key>") << "                  - Delete a key" << std::endl;
            std::cout << "  " << ColorOutput::cyan("search <attr> <value>") << "         - Find keys by attribute" << std::endl;
            std::cout << std::endl;

            std::cout << ColorOutput::green("Store Operations:") << std::endl;
            std::cout << "  " << ColorOutput::cyan("keys") << "                         - List all keys" << std::endl;
            std::cout << "  " << ColorOutput::cyan("clear") << "                        - Clear all data" << std::endl;
            std::cout << "  " << ColorOutput::cyan("stats") << "                        - Show store statistics" << std::endl;
            std::cout << "  " << ColorOutput::cyan("inspect <key>") << "                - Detailed view of a key" << std::endl;
            std::cout << std::endl;

            std::cout << ColorOutput::green("File Operations:") << std::endl;
            std::cout << "  " << ColorOutput::cyan("save [filename]") << "              - Save store to file" << std::endl;
            std::cout << "  " << ColorOutput::cyan("load [filename]") << "              - Load store from file" << std::endl;
            std::cout << std::endl;

            std::cout << ColorOutput::green("Utility:") << std::endl;
            std::cout << "  " << ColorOutput::cyan("help") << "                         - Show this help" << std::endl;
            std::cout << "  " << ColorOutput::cyan("exit/quit/q") << "                  - Exit (interactive mode only)" << std::endl;
            std::cout << std::endl;

            std::cout << ColorOutput::yellow("Examples:") << std::endl;
            std::cout << "  put user1 name:John age:25 active:true" << std::endl;
            std::cout << "  get user1" << std::endl;
            std::cout << "  search age 25" << std::endl;
            std::cout << "  search active true" << std::endl;

            return 0;
        }

        void CLI::printWelcome() {
            if(jsonMode_) return;

            std::cout << ColorOutput::bold("=== Welcome to KVS++ Store CLI ===") << std::endl;
            std::cout << "Type " << ColorOutput::cyan("help") << " for available commands or "
                << ColorOutput::cyan("exit") << " to quit." << std::endl;
            std::cout << "Store file: " << ColorOutput::yellow(defaultStoreFile_) << std::endl;
            std::cout << "Auto-save: " << (autoSave_ ? ColorOutput::green("Enabled") : ColorOutput::red("Disabled")) << std::endl;
            std::cout << std::endl;
        }        void CLI::printPrompt() {
            if(jsonMode_) return;
            std::cout << ColorOutput::green("kvs++ > ");
        }

        void CLI::printError(const std::string& message) {
            if(jsonMode_) {
                std::cout << "{\"error\":\"" << message << "\"}" << std::endl;
            }
            else {
                std::cout << ColorOutput::red("[ERROR] ") << message << std::endl;
            }
        }

        void CLI::printSuccess(const std::string& message) {
            if(jsonMode_) return;  // Success messages are usually not needed in JSON mode
            std::cout << ColorOutput::green("[SUCCESS] ") << message << std::endl;
        }

        void CLI::printInfo(const std::string& message) {
            if(jsonMode_) return;
            std::cout << ColorOutput::blue("[INFO] ") << message << std::endl;
        }

        void CLI::printValue(const std::string& key, const core::ValueObject* value) {
            std::cout << ColorOutput::cyan(key) << " -> " << ColorOutput::yellow(value->toString()) << std::endl;
        }

        void CLI::printKeyList(const std::vector<std::string>& keys) {
            for(const auto& key : keys) {
                std::cout << "  " << ColorOutput::cyan(key) << std::endl;
            }
        }

        std::string CLI::valueToJson(const core::ValueObject* value) {
            if(!value) return "null";

            std::ostringstream json;
            json << "{";

            const auto& attributes = value->getAttributes();
            bool first = true;

            for(const auto& [name, attrValue] : attributes) {
                if(!first) json << ",";
                first = false;

                json << "\"" << name << "\":";

                std::visit([&json](const auto& val) {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr(std::is_same_v<T, std::string>) {
                        json << "\"" << val << "\"";
                    }
                    else if constexpr(std::is_same_v<T, bool>) {
                        json << (val ? "true" : "false");
                    }
                    else {
                        json << val;
                    }
                }, attrValue);
            }

            json << "}";
            return json.str();
        }

        std::string CLI::keysToJson(const std::vector<std::string>& keys) {
            std::ostringstream json;
            json << "[";

            for(size_t i = 0; i < keys.size(); ++i) {
                if(i > 0) json << ",";
                json << "\"" << keys[i] << "\"";
            }

            json << "]";
            return json.str();
        }

        std::vector<std::pair<std::string, std::string>> CLI::parseAttributePairs(
            const std::vector<std::string>& args, size_t startIndex) {

            std::vector<std::pair<std::string, std::string>> pairs;

            for(size_t i = startIndex; i < args.size(); ++i) {
                const std::string& arg = args[i];
                size_t colonPos = arg.find(':');

                if(colonPos == std::string::npos || colonPos == 0 || colonPos == arg.length() - 1) {
                    printError("Invalid attribute format: '" + arg + "'. Expected format: key:value");
                    continue;
                }

                std::string key = arg.substr(0, colonPos);
                std::string value = arg.substr(colonPos + 1);

                pairs.emplace_back(key, value);
            }

            return pairs;
        }

        void CLI::autoSaveIfEnabled() {
            if(autoSave_ && !store_->empty()) {
                try {
                    store_->save(defaultStoreFile_);
                    if(verboseMode_) {
                        printInfo("Auto-saved to: " + defaultStoreFile_);
                    }
                }
                catch(const exceptions::KVStoreException& e) {
                    printError("Auto-save failed: " + std::string(e.what()));
                }
            }
        }

        std::string CLI::normalizeStorePath(const std::string& filePath) {
            // If the file path already contains a directory separator, use it as-is
            if(filePath.find('/') != std::string::npos || filePath.find('\\') != std::string::npos) {
                return filePath;
            }

            // If it's just a filename, place it in the store/ directory
            return "store/" + filePath;
        }

        // ColorOutput implementation
        std::string ColorOutput::red(const std::string& text) {
            return "\033[31m" + text + "\033[0m";
        }

        std::string ColorOutput::green(const std::string& text) {
            return "\033[32m" + text + "\033[0m";
        }

        std::string ColorOutput::yellow(const std::string& text) {
            return "\033[33m" + text + "\033[0m";
        }

        std::string ColorOutput::blue(const std::string& text) {
            return "\033[34m" + text + "\033[0m";
        }

        std::string ColorOutput::cyan(const std::string& text) {
            return "\033[36m" + text + "\033[0m";
        }

        std::string ColorOutput::bold(const std::string& text) {
            return "\033[1m" + text + "\033[0m";
        }

        std::string ColorOutput::reset() {
            return "\033[0m";
        }

    }
}