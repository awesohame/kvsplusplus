#include "kvstore/core/KeyValueStore.hpp"
#include "kvstore/exceptions/Exceptions.hpp"
#include "kvstore/core/TypeRegistry.hpp"
#include "kvstore/persistence/PersistenceManager.hpp"
#include <algorithm>
#include <variant>

namespace kvspp {
    namespace core {
        void KeyValueStore::setAutosave(bool enabled) {
            std::lock_guard<std::mutex> lock(mtx_);
            autosave_ = enabled;
        }

        bool KeyValueStore::getAutosave() const {
            std::lock_guard<std::mutex> lock(mtx_);
            return autosave_;
        }

        const ValueObject* KeyValueStore::get(const std::string& key) const {
            std::lock_guard<std::mutex> lock(mtx_);

            auto it = store_.find(key);
            if(it != store_.end()) {
                return it->second.get();
            }
            return nullptr;
        }

        std::vector<std::string> KeyValueStore::search(const std::string& attributeKey,
            const std::string& attributeValue) const {
            std::lock_guard<std::mutex> lock(mtx_);
            std::vector<std::string> result;

            for(const auto& pair : store_) {
                const std::string& key = pair.first;
                const auto& valueObject = pair.second;

                if(valueObject->hasAttribute(attributeKey)) {
                    const auto* attr = valueObject->getAttribute(attributeKey);
                    if(attr && attributeValueToString(*attr) == attributeValue) {
                        result.push_back(key);
                    }
                }
            }

            return result;
        }        void KeyValueStore::put(const std::string& key,
            const std::vector<std::pair<std::string, std::string>>& attributePairs) {
            std::lock_guard<std::mutex> lock(mtx_);

            // Create new ValueObject with this store's TypeRegistry
            auto valueObject = std::make_unique<ValueObject>(attributePairs, typeRegistry_);

            // Store the object
            store_[key] = std::move(valueObject);
        }

        void KeyValueStore::put(const std::string& key, const ValueObject& valueObject) {
            std::lock_guard<std::mutex> lock(mtx_);
            auto newValueObject = std::make_unique<ValueObject>(valueObject);
            newValueObject->setTypeRegistry(typeRegistry_);
            store_[key] = std::move(newValueObject);
        }

        bool KeyValueStore::deleteKey(const std::string& key) {
            std::lock_guard<std::mutex> lock(mtx_);

            auto it = store_.find(key);
            if(it != store_.end()) {
                store_.erase(it);
                return true;
            }
            return false;
        }

        std::vector<std::string> KeyValueStore::keys() const {
            std::lock_guard<std::mutex> lock(mtx_);
            std::vector<std::string> result;
            result.reserve(store_.size());

            for(const auto& pair : store_) {
                result.push_back(pair.first);
            }

            return result;
        }

        size_t KeyValueStore::size() const {
            std::lock_guard<std::mutex> lock(mtx_);
            return store_.size();
        }

        bool KeyValueStore::empty() const {
            std::lock_guard<std::mutex> lock(mtx_);
            return store_.empty();
        }

        void KeyValueStore::clear() {
            std::lock_guard<std::mutex> lock(mtx_);
            store_.clear();
        }        std::string KeyValueStore::attributeValueToString(const AttributeValue& value) const {
            return std::visit([](const auto& v) -> std::string {
                if constexpr(std::is_same_v<std::decay_t<decltype(v)>, std::string>) {
                    return v;
                }
                else if constexpr(std::is_same_v<std::decay_t<decltype(v)>, bool>) {
                    return v ? "true" : "false";
                }
                else {
                    return std::to_string(v);
                }
            }, value);
        }

        void KeyValueStore::save(const std::string& filePath) const {
            persistence::PersistenceManager manager(filePath);
            manager.save(*this);
        }

        void KeyValueStore::load(const std::string& filePath) {
            persistence::PersistenceManager manager(filePath);
            manager.load(*this);
        }

        TypeRegistry& KeyValueStore::getTypeRegistry() {
            return typeRegistry_;
        }

    }
}