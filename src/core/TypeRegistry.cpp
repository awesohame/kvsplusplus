#include "kvstore/core/TypeRegistry.hpp"
#include "kvstore/exceptions/Exceptions.hpp"

namespace kvcpp {
    namespace core {

        TypeRegistry& TypeRegistry::getInstance() {
            static TypeRegistry instance;
            return instance;
        }

        void TypeRegistry::validateAndRegisterType(const std::string& attributeName, AttributeType type) {
            std::lock_guard<std::mutex> lock(mtx);            auto it = attributeTypes_.find(attributeName);
            if(it != attributeTypes_.end()) {
                // Attribute already exists, check if types match
                if(it->second != type) {
                    throw kvcpp::exceptions::TypeMismatchException(
                        attributeName,
                        getTypeName(it->second),
                        getTypeName(type)
                    );
                }
            }
            else {
             // New attribute, register its type
                attributeTypes_[attributeName] = type;
            }
        }

        const AttributeType* TypeRegistry::getRegisteredType(const std::string& attributeName) const {
            std::lock_guard<std::mutex> lock(mtx);

            auto it = attributeTypes_.find(attributeName);
            if(it != attributeTypes_.end()) {
                return &(it->second);
            }
            return nullptr;
        }

        bool TypeRegistry::isRegistered(const std::string& attributeName) const {
            std::lock_guard<std::mutex> lock(mtx);
            return attributeTypes_.find(attributeName) != attributeTypes_.end();
        }

        AttributeType TypeRegistry::getTypeFromValue(const AttributeValue& value) {
            if(std::holds_alternative<std::string>(value)) {
                return AttributeType::STRING;
            }
            else if(std::holds_alternative<int>(value)) {
                return AttributeType::INTEGER;
            }
            else if(std::holds_alternative<double>(value)) {
                return AttributeType::DOUBLE;
            }
            else if(std::holds_alternative<bool>(value)) {
                return AttributeType::BOOLEAN;
            }

// this should never happen if AttributeValue is properly defined
            throw kvcpp::exceptions::InvalidValueException("unknown", "valid AttributeValue variant");
        }

        std::string TypeRegistry::getTypeName(AttributeType type) {
            switch(type) {
            case AttributeType::STRING:
                return "string";
            case AttributeType::INTEGER:
                return "integer";
            case AttributeType::DOUBLE:
                return "double";
            case AttributeType::BOOLEAN:
                return "boolean";
            default:
                return "unknown";
            }
        }

        void TypeRegistry::clear() {
            std::lock_guard<std::mutex> lock(mtx);
            attributeTypes_.clear();
        }

    }
}