#pragma once

#include "kvstore/core/KeyValueStore.hpp"
#include "kvstore/core/ValueObject.hpp"
#include "kvstore/exceptions/Exceptions.hpp"
#include <string>
#include <memory>
#include <mutex>

namespace kvcpp {
    namespace persistence {

        /**
         * PersistenceManager handles saving and loading key-value store data
         * to/from JSON files. It maintains type consistency across sessions
         * by integrating with the TypeRegistry.
         */
        class PersistenceManager {
        private:
            std::string filePath_;
            mutable std::mutex mtx_;

        public:
            explicit PersistenceManager(const std::string& filePath);

            // Save the entire store to JSON file
            void save(const core::KeyValueStore& store);

            // Load store data from JSON file
            void load(core::KeyValueStore& store);

            // Check if the persistence file exists
            bool fileExists() const;

            // Get the current file path
            const std::string& getFilePath() const;

            // Set a new file path
            void setFilePath(const std::string& newFilePath);

        private:
            // JSON serialization helpers
            std::string valueObjectToJson(const core::ValueObject& obj) const;
            std::string attributeValueToJson(const core::AttributeValue& value) const;
            std::string escapeJsonString(const std::string& str) const;

            // JSON deserialization helpers
            core::ValueObject jsonToValueObject(const std::string& jsonStr) const;
            core::AttributeValue parseJsonValue(const std::string& jsonValue) const;
            std::string unescapeJsonString(const std::string& str) const;

            // File I/O helpers
            std::string readFile(const std::string& path) const;
            void writeFile(const std::string& path, const std::string& content) const;
              // JSON parsing utilities
            std::string extractJsonString(const std::string& json, const std::string& key) const;
            std::string findJsonValue(const std::string& json, const std::string& key) const;
            void parseStoreSection(const std::string& storeJson, core::KeyValueStore& store) const;
        };

    }
}