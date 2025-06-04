#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <memory>
#include "ValueObject.hpp"

namespace kvcpp {
    namespace core {

        class KeyValueStore {
        private:
            // The main storage: key -> ValueObject
            std::unordered_map<std::string, std::unique_ptr<ValueObject>> store_;

            // Mutex for thread safety
            mutable std::mutex mtx_;

        public:
            KeyValueStore() = default;
            ~KeyValueStore() = default;

            // delete copy constructor and assignment operator for thread safety
            KeyValueStore(const KeyValueStore&) = delete;
            KeyValueStore& operator=(const KeyValueStore&) = delete;

            // move constructor and assignment
            KeyValueStore(KeyValueStore&&) = default;
            KeyValueStore& operator=(KeyValueStore&&) = default;

            const ValueObject* get(const std::string& key) const;

            std::vector<std::string> search(const std::string& attributeKey,
                const std::string& attributeValue) const;

            void put(const std::string& key,
                const std::vector<std::pair<std::string, std::string>>& attributePairs);

            bool deleteKey(const std::string& key);

            std::vector<std::string> keys() const;

            size_t size() const;

            bool empty() const;

            void clear();

        private:
            std::string attributeValueToString(const AttributeValue& value) const;
        };

    }
}
