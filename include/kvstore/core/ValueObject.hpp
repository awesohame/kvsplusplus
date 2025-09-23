#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <utility>

namespace kvspp {
    namespace core {

        // Forward declaration
        class TypeRegistry;

        using AttributeValue = std::variant<std::string, int, double, bool>;
        using AttributePair = std::pair<std::string, std::string>;

        /**
         * ValueObject represents the value part of a key-value pair in the store.
         * It contains a map of attributes where each attribute has a name (string)
         * and a typed value (string, int, double, or bool).
         */
        class ValueObject {
        public:
            // Returns the value as a string (for flat value use)
            std::string getValueString() const;
        private:
            std::unordered_map<std::string, AttributeValue> attributes_;
            TypeRegistry* typeRegistry_;  // Reference to the store's TypeRegistry

        public:
            // Constructor that accepts TypeRegistry reference
            explicit ValueObject(TypeRegistry& typeRegistry);

            // Constructor with attribute pairs and TypeRegistry reference
            ValueObject(const std::vector<AttributePair>& attributePairs, TypeRegistry& typeRegistry);

            // Default constructor (requires setTypeRegistry call before use)
            ValueObject() = default;
            // copy constructor and assignment operator
            ValueObject(const ValueObject& other) = default;
            ValueObject& operator=(const ValueObject& other) = default;
            // Move constructor and assignment operator
            ValueObject(ValueObject&& other) noexcept = default;
            ValueObject& operator=(ValueObject&& other) noexcept = default;
            // Destructor
            ~ValueObject() = default;

            // Set TypeRegistry reference (for default constructed objects)
            void setTypeRegistry(TypeRegistry& typeRegistry);

            // Get an attribute value by name (returns nullptr if not found)
            const AttributeValue* getAttribute(const std::string& attributeName) const;

            // Set an attribute with type validation
            void setAttribute(const std::string& attributeName, const std::string& value);
            void setAttribute(const std::string& attributeName, int value);
            void setAttribute(const std::string& attributeName, double value);
            void setAttribute(const std::string& attributeName, bool value);

            // Check if an attribute exists
            bool hasAttribute(const std::string& attributeName) const;

            // Get all attributes
            const std::unordered_map<std::string, AttributeValue>& getAttributes() const;

            // Override toString method to print as comma-separated key-value pairs
            std::string toString() const;

        private:
            // Helper method to parse string values to appropriate AttributeValue types
            static AttributeValue parseStringToAttributeValue(const std::string& valueStr);
        };

    }
}