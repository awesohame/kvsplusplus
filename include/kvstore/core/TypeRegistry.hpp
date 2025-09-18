#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <mutex>

namespace kvspp {
    namespace core {

        using AttributeValue = std::variant<std::string, int, double, bool>;

        // enum for type of attribute
        enum class AttributeType {
            STRING,
            INTEGER,
            DOUBLE,
            BOOLEAN
        };

        /**
         * TypeRegistry ensures that once an attribute type is defined,
         * all subsequent uses of that attribute must be of the same type.
         * This class is thread-safe.
         * Each KeyValueStore has its own TypeRegistry instance.
         */
        class TypeRegistry {
        private:
            std::unordered_map<std::string, AttributeType> attributeTypes_;

            // for thread safety
            mutable std::mutex mtx;

        public:
            // Default constructor
            TypeRegistry() = default;

            // Delete copy constructor and assignment for safety
            TypeRegistry(const TypeRegistry&) = delete;
            TypeRegistry& operator=(const TypeRegistry&) = delete;

            // Allow move operations
            TypeRegistry(TypeRegistry&&) = default;
            TypeRegistry& operator=(TypeRegistry&&) = default;

            // Register new attribute type or validate existing one
            // throws exception if type mismatch
            void validateAndRegisterType(const std::string& attributeName, AttributeType type);

            // get registered type for an attribute (returns nullptr if not registered)
            const AttributeType* getRegisteredType(const std::string& attributeName) const;

            // check if an attribute is registered
            bool isRegistered(const std::string& attributeName) const;

            // helper function to AttributeValue's type
            static AttributeType getTypeFromValue(const AttributeValue& value);

            // helper function to get type name as string
            static std::string getTypeName(AttributeType type);

            // clear all registered types
            void clear();
        };

    }
}