
#pragma once
#include <unordered_map>
#include <string>
#include <mutex>
#include "KeyValueStore.hpp"

namespace kvspp { namespace core { class KeyValueStore; } }

namespace kvstore {

    class StoreManager {
    public:
        using storeToken = std::string;

        // Singleton accessor
        static StoreManager& instance();

        // Get or create a store for the given token
        kvspp::core::KeyValueStore& getStore(const storeToken& token);

        // Thread-safe put
        void put(const storeToken& token, const std::string& key, const std::string& value);

        // Thread-safe get
        std::string get(const storeToken& token, const std::string& key);

        // Thread-safe remove
        void remove(const storeToken& token, const std::string& key);

        // Save a specific store to a file
        void saveStore(const storeToken& token, const std::string& filename) const;

        // Load a specific store from a file
        void loadStore(const storeToken& token, const std::string& filename);

        // Clear all stores (for testing/demo)
        void clearAllStores();

        // Normalize filename: append .json if missing, add store/ if not absolute
        static std::string normalizeFilename(const std::string& filename);

    private:
        StoreManager() = default;
        StoreManager(const StoreManager&) = delete;
        StoreManager& operator=(const StoreManager&) = delete;

        std::unordered_map<storeToken, kvspp::core::KeyValueStore> stores_;
        mutable std::mutex mutex_;
    };

} // namespace kvstore
