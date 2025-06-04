#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <memory>
#include "ValueObject.hpp"

namespace kvcpp {
    namespace core {

        /**
        * Thread-safe in-memory key-value store.
        * Keys are strings, values are ValueObjects containing typed attributes.
        */
        class KeyValueStore {
        private:
            // The main storage: key -> ValueObject
            std::unordered_map<std::string, std::unique_ptr<ValueObject>> store_;

            // Mutex for thread safety
            mutable std::mutex mtx_;

        public:
            // Constructor Destructor
            KeyValueStore() = default;
            ~KeyValueStore() = default;

            // delete copy constructor and assignment operator for thread safety
            KeyValueStore(const KeyValueStore&) = delete;
            KeyValueStore& operator=(const KeyValueStore&) = delete;

            // move constructor and assignment
            KeyValueStore(KeyValueStore&&) = default;
            KeyValueStore& operator=(KeyValueStore&&) = default;

            /**
            * Get the value object for a given key
            * @param key The key to search for
            * @return Pointer to ValueObject if found, nullptr if not found
            */
            const ValueObject* get(const std::string& key) const;

            /**
            * Search for keys that have a specific attribute with a specific value
            * @param attributeKey The attribute name to search for
            * @param attributeValue The attribute value to match (as string)
            * @return Vector of keys that match the criteria
            */
            std::vector<std::string> search(const std::string& attributeKey,
                const std::string& attributeValue) const;            /**
            * Add or update a key-value pair in the store
            * @param key The key to add/update
            * @param attributePairs List of attribute key-value pairs (both as strings)
            * @throws TypeMismatchException if attribute types don't match existing registrations
            * @throws InvalidValueException if values cannot be parsed
            */
            void put(const std::string& key,
                const std::vector<std::pair<std::string, std::string>>& attributePairs);

            /**
            * Add or update a key-value pair in the store with a ValueObject
            * @param key The key to add/update
            * @param valueObject The ValueObject to store
            */
            void put(const std::string& key, const ValueObject& valueObject);

            /**
            * Delete a key-value pair from the store
            * @param key The key to delete
            * @return true if key was found and deleted, false if key not found
            */
            bool deleteKey(const std::string& key);

            /**
            * Get all keys in the store
            * @return Vector of all keys
            */
            std::vector<std::string> keys() const;

            /**
            * Get the number of entries in the store
            * @return Number of key-value pairs
            */
            size_t size() const;

            /**
            * Check if the store is empty
            * @return true if empty, false otherwise
            */
            bool empty() const;            /**
            * Clear all entries from the store
            */
            void clear();

            /**
            * Save the store to a file using PersistenceManager
            * @param filePath Path to the file where data should be saved
            * @throws KVStoreException if saving fails
            */
            void save(const std::string& filePath) const;

            /**
            * Load store data from a file using PersistenceManager
            * @param filePath Path to the file from which data should be loaded
            * @throws KVStoreException if loading fails
            */
            void load(const std::string& filePath);

        private:
            /**
            * Helper function to convert AttributeValue to string for search comparison
            * @param value The AttributeValue to convert
            * @return String representation of the value
            */
            std::string attributeValueToString(const AttributeValue& value) const;
        };

    }
}