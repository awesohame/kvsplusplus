#include "kvstore/persistence/PersistenceManager.hpp"
#include "kvstore/core/TypeRegistry.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include <cctype>

namespace kvspp {
    namespace persistence {

        PersistenceManager::PersistenceManager(const std::string& filePath)
            : filePath_(filePath) {
        }

        void PersistenceManager::save(const core::KeyValueStore& store) {
            std::lock_guard<std::mutex> lock(mtx_);

            try {
                std::ostringstream json;
                json << "{\n";
                json << "  \"store\": {\n";

                auto keys = store.keys();
                for(size_t i = 0; i < keys.size(); ++i) {
                    const auto& key = keys[i];
                    const auto* valueObj = store.get(key);

                    if(valueObj) {
                        json << "    \"" << escapeJsonString(key) << "\": "
                            << valueObjectToJson(*valueObj);
                        json << ",\n";
                    }
                }
                // Write autosave field (assume store.hasAutosave() and store.getAutosave())
                json << "    \"autosave\": " << (store.hasAutosave() ? (store.getAutosave() ? "true" : "false") : "false") << "\n";
                json << "  }\n";
                json << "}";

                writeFile(filePath_, json.str());

            }
            catch(const std::exception& e) {
                throw exceptions::KVStoreException("Failed to save store: " + std::string(e.what()));
            }
        }        void PersistenceManager::load(core::KeyValueStore& store) {
            std::lock_guard<std::mutex> lock(mtx_);

            if(!fileExists()) {
                return; // No file to load from
            }

            try {
                std::string content = readFile(filePath_);

                // Clear existing store
                auto keys = store.keys();
                for(const auto& key : keys) {
                    store.deleteKey(key);
                }

                // Parse JSON and load data
                std::string storeSection = findJsonValue(content, "store");
                if(storeSection.empty()) {
                    return; // Empty or invalid JSON
                }

                // Parse the store section which contains key-value pairs
                parseStoreSection(storeSection, store);

            }
            catch(const std::exception& e) {
                throw exceptions::KVStoreException("Failed to load store: " + std::string(e.what()));
            }
        }

        bool PersistenceManager::fileExists() const {
            return std::filesystem::exists(filePath_);
        }

        const std::string& PersistenceManager::getFilePath() const {
            return filePath_;
        }

        void PersistenceManager::setFilePath(const std::string& newFilePath) {
            std::lock_guard<std::mutex> lock(mtx_);
            filePath_ = newFilePath;
        }

        // JSON serialization helpers
        std::string PersistenceManager::valueObjectToJson(const core::ValueObject& obj) const {
            std::ostringstream json;
            json << "{\n";

            const auto& attributes = obj.getAttributes();
            size_t count = 0;

            for(const auto& [key, value] : attributes) {
                json << "      \"" << escapeJsonString(key) << "\": "
                    << attributeValueToJson(value);

                if(++count < attributes.size()) {
                    json << ",";
                }
                json << "\n";
            }

            json << "    }";
            return json.str();
        }

        std::string PersistenceManager::attributeValueToJson(const core::AttributeValue& value) const {
            if(std::holds_alternative<std::string>(value)) {
                return "\"" + escapeJsonString(std::get<std::string>(value)) + "\"";
            }
            else if(std::holds_alternative<int>(value)) {
                return std::to_string(std::get<int>(value));
            }
            else if(std::holds_alternative<double>(value)) {
                return std::to_string(std::get<double>(value));
            }
            else if(std::holds_alternative<bool>(value)) {
                return std::get<bool>(value) ? "true" : "false";
            }
            return "null";
        }

        std::string PersistenceManager::escapeJsonString(const std::string& str) const {
            std::string result;
            for(char c : str) {
                switch(c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
                }
            }
            return result;
        }

        // JSON deserialization helpers
        core::ValueObject PersistenceManager::jsonToValueObject(const std::string& jsonStr, core::TypeRegistry& typeRegistry) const {
            core::ValueObject obj(typeRegistry);

            std::string content = jsonStr;
            // Remove outer braces
            if(content.front() == '{') content.erase(0, 1);
            if(content.back() == '}') content.pop_back();

            std::istringstream iss(content);
            std::string line;

            while(std::getline(iss, line)) {
                // Skip empty lines and whitespace
                line.erase(0, line.find_first_not_of(" \t\n\r"));
                line.erase(line.find_last_not_of(" \t\n\r") + 1);

                if(line.empty()) continue;

                // Find the key and value
                size_t colonPos = line.find(':');
                if(colonPos == std::string::npos) continue;

                std::string keyPart = line.substr(0, colonPos);
                std::string valuePart = line.substr(colonPos + 1);

                // Remove quotes and whitespace from key
                keyPart.erase(0, keyPart.find_first_not_of(" \t\""));
                keyPart.erase(keyPart.find_last_not_of(" \t\"") + 1);

                // Remove whitespace from value part
                valuePart.erase(0, valuePart.find_first_not_of(" \t"));
                if(valuePart.back() == ',') valuePart.pop_back();
                valuePart.erase(valuePart.find_last_not_of(" \t") + 1);

                if(!keyPart.empty() && !valuePart.empty()) {
                    core::AttributeValue value = parseJsonValue(valuePart);

                    // Use the appropriate setAttribute method based on type
                    if(std::holds_alternative<std::string>(value)) {
                        obj.setAttribute(unescapeJsonString(keyPart), std::get<std::string>(value));
                    }
                    else if(std::holds_alternative<int>(value)) {
                        obj.setAttribute(unescapeJsonString(keyPart), std::get<int>(value));
                    }
                    else if(std::holds_alternative<double>(value)) {
                        obj.setAttribute(unescapeJsonString(keyPart), std::get<double>(value));
                    }
                    else if(std::holds_alternative<bool>(value)) {
                        obj.setAttribute(unescapeJsonString(keyPart), std::get<bool>(value));
                    }
                }
            }

            return obj;
        }

        core::AttributeValue PersistenceManager::parseJsonValue(const std::string& jsonValue) const {
            std::string value = jsonValue;

            // Remove whitespace
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            // Check for boolean values
            if(value == "true") return true;
            if(value == "false") return false;

            // Check for quoted string
            if(value.front() == '"' && value.back() == '"') {
                std::string str = value.substr(1, value.length() - 2);
                return unescapeJsonString(str);
            }

            // Try to parse as number
            try {
                if(value.find('.') != std::string::npos) {
                    return std::stod(value);
                }
                else {
                    return std::stoi(value);
                }
            }
            catch(const std::exception&) {
             // If parsing fails, treat as string
                return value;
            }
        }

        std::string PersistenceManager::unescapeJsonString(const std::string& str) const {
            std::string result;
            for(size_t i = 0; i < str.length(); ++i) {
                if(str[i] == '\\' && i + 1 < str.length()) {
                    switch(str[i + 1]) {
                    case '"': result += '"'; ++i; break;
                    case '\\': result += '\\'; ++i; break;
                    case 'b': result += '\b'; ++i; break;
                    case 'f': result += '\f'; ++i; break;
                    case 'n': result += '\n'; ++i; break;
                    case 'r': result += '\r'; ++i; break;
                    case 't': result += '\t'; ++i; break;
                    default: result += str[i]; break;
                    }
                }
                else {
                    result += str[i];
                }
            }
            return result;
        }

        // File I/O helpers
        std::string PersistenceManager::readFile(const std::string& path) const {
            std::ifstream file(path);
            if(!file) {
                throw std::runtime_error("Cannot open file for reading: " + path);
            }

            std::ostringstream content;
            content << file.rdbuf();
            return content.str();
        }

        void PersistenceManager::writeFile(const std::string& path, const std::string& content) const {
            // Ensure directory exists
            std::filesystem::path filePath(path);
            if(filePath.has_parent_path()) {
                std::filesystem::create_directories(filePath.parent_path());
            }

            std::ofstream file(path);
            if(!file) {
                throw std::runtime_error("Cannot open file for writing: " + path);
            }

            file << content;
        }

        // JSON parsing utilities
        std::string PersistenceManager::findJsonValue(const std::string& json, const std::string& key) const {
            std::string searchKey = "\"" + key + "\"";
            size_t keyPos = json.find(searchKey);
            if(keyPos == std::string::npos) return "";

            size_t colonPos = json.find(':', keyPos);
            if(colonPos == std::string::npos) return "";

            // Find the start of the value
            size_t valueStart = colonPos + 1;
            while(valueStart < json.length() && std::isspace(json[valueStart])) {
                ++valueStart;
            }

            if(valueStart >= json.length()) return "";

            // Find the end of the value (handle nested objects)
            size_t valueEnd = valueStart;
            int braceCount = 0;
            bool inString = false;
            bool escaped = false;

            while(valueEnd < json.length()) {
                char c = json[valueEnd];

                if(escaped) {
                    escaped = false;
                }
                else if(c == '\\') {
                    escaped = true;
                }
                else if(c == '"') {
                    inString = !inString;
                }
                else if(!inString) {
                    if(c == '{') {
                        ++braceCount;
                    }
                    else if(c == '}') {
                        if(braceCount == 0) break;
                        --braceCount;
                    }
                    else if(c == ',' && braceCount == 0) {
                        break;
                    }
                }
                ++valueEnd;
            }
            return json.substr(valueStart, valueEnd - valueStart);
        }

        void PersistenceManager::parseStoreSection(const std::string& storeJson, core::KeyValueStore& store) const {
            std::string content = storeJson;

            // Remove outer braces
            if(content.front() == '{') content.erase(0, 1);
            if(content.back() == '}') content.pop_back();

            // Parse each key-value pair and autosave field
            size_t pos = 0;
            while(pos < content.length()) {
                // Find the start of a key
                size_t keyStart = content.find('"', pos);
                if(keyStart == std::string::npos) break;

                // Find the end of the key
                size_t keyEnd = content.find('"', keyStart + 1);
                if(keyEnd == std::string::npos) break;

                std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);

                // Find the colon after the key
                size_t colonPos = content.find(':', keyEnd);
                if(colonPos == std::string::npos) break;

                // Check for autosave field
                if(key == "autosave") {
                    // Find value (true/false)
                    size_t valStart = content.find_first_not_of(" ", colonPos + 1);
                    if(valStart != std::string::npos) {
                        bool autosave = false;
                        if(content.compare(valStart, 4, "true") == 0) autosave = true;
                        else if(content.compare(valStart, 5, "false") == 0) autosave = false;
                        store.setAutosave(autosave);
                    }
                    // Move to next field
                    pos = content.find(',', colonPos);
                    if(pos == std::string::npos) break;
                    ++pos;
                    continue;
                }

                // Find the start of the value (should be an object starting with '{')
                size_t valueStart = content.find('{', colonPos);
                if(valueStart == std::string::npos) break;

                // Find the matching closing brace for this object
                size_t valueEnd = valueStart + 1;
                int braceCount = 1;
                bool inString = false;
                bool escaped = false;

                while(valueEnd < content.length() && braceCount > 0) {
                    char c = content[valueEnd];

                    if(escaped) {
                        escaped = false;
                    }
                    else if(c == '\\') {
                        escaped = true;
                    }
                    else if(c == '"') {
                        inString = !inString;
                    }
                    else if(!inString) {
                        if(c == '{') {
                            ++braceCount;
                        }
                        else if(c == '}') {
                            --braceCount;
                        }
                    }
                    ++valueEnd;
                }

                if(braceCount == 0) {
                    // Extract the complete object JSON
                    std::string objectJson = content.substr(valueStart, valueEnd - valueStart);

                    // Parse the object and add to store
                    core::ValueObject obj = jsonToValueObject(objectJson, store.getTypeRegistry());
                    store.put(unescapeJsonString(key), obj);
                }

                // Move to next key-value pair
                pos = valueEnd;
            }
        }

    }
}