#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <utility>

namespace kvcpp {
    namespace core {

        using AttributeValue = std::variant<std::string, int, double, bool>;
        using AttributePair = std::pair<std::string, std::string>;

        /**
         * ValueObject represents the value part of a key-value pair in the store.
         * It contains a map of attributes where each attribute has a name (string)
         * and a typed value (string, int, double, or bool).
         */
        class ValueObject {
        private:
            std::unordered_map<std::string, AttributeValue> attributes_;

        public:
            // converts list of attribute pairs (string, string) to appropriate types
            explicit ValueObject(const std::vector<AttributePair>& attributePairs);

            ValueObject() = default;
            // copy constructor and assignment operator
            ValueObject(const ValueObject& other) = default;
            ValueObject& operator=(const ValueObject& other) = default;
            // Move constructor and assignment operator
            ValueObject(ValueObject&& other) noexcept = default;
            ValueObject& operator=(ValueObject&& other) noexcept = default;
            // Destructor
            ~ValueObject() = default;

            // Get an attribute value by name (returns nullptr if not found)
            const AttributeValue* getAttribute(const std::string& attributeName) const;

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