
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
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = stores_.find(token);
        if(it == stores_.end()) throw std::runtime_error("Store not found");
        std::string fname = filename;
        // Ensure .json extension
        if(fname.size() < 5 || fname.substr(fname.size() - 5) != ".json") {
            fname += ".json";
        }
        // Always store in ./store/ relative to executable
        if(fname.rfind("store/", 0) != 0 && fname.rfind("./store/", 0) != 0) {
            fname = std::string("store/") + fname;
        }
        it->second.save(fname);
    }


    void StoreManager::loadStore(const storeToken& token, const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string fname = filename;
        if(fname.size() < 5 || fname.substr(fname.size() - 5) != ".json") {
            fname += ".json";
        }
        if(fname.rfind("store/", 0) != 0 && fname.rfind("./store/", 0) != 0) {
            fname = std::string("store/") + fname;
        }
        stores_[token].load(fname);
    }

} // namespace kvstore
