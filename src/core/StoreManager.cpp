
#include "kvstore/core/StoreManager.hpp"
#include <stdexcept>
#include <mutex>

namespace kvstore {

    StoreManager& StoreManager::instance() {
        static StoreManager instance;
        return instance;
    }

    void StoreManager::clearAllStores() {
        std::lock_guard<std::mutex> lock(mutex_);
        stores_.clear();
    }

    kvspp::core::KeyValueStore& StoreManager::getStore(const storeToken& token) {
        std::lock_guard<std::mutex> lock(mutex_);
        return stores_[token]; // Creates if not exists
    }


    void StoreManager::put(const storeToken& token, const std::string& key, const std::string& value) {
        std::vector<std::pair<std::string, std::string>> attrs = { {"value", value} };
        getStore(token).put(key, attrs);
    }


    std::string StoreManager::get(const storeToken& token, const std::string& key) {
        const auto* vo = getStore(token).get(key);
        if(!vo) throw std::runtime_error("Key not found");
        // Assuming ValueObject has a toString() method
        return vo->toString();
    }


    void StoreManager::remove(const storeToken& token, const std::string& key) {
        getStore(token).deleteKey(key);
    }


    void StoreManager::saveStore(const storeToken& token, const std::string& filename) const {
        // Lock for thread safety
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = stores_.find(token);
        if(it == stores_.end()) throw std::runtime_error("Store not found");
        it->second.save(filename);
    }

    void StoreManager::loadStore(const storeToken& token, const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex_);
        stores_[token].load(filename);
    }

} // namespace kvstore
