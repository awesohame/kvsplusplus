#include "kvstore/core/ValueObject.hpp"
#include "kvstore/core/TypeRegistry.hpp"
#include "kvstore/exceptions/Exceptions.hpp"
#include <sstream>
#include <stdexcept>
#include <regex>

namespace kvcpp {
    namespace core {
        ValueObject::ValueObject(const std::vector<AttributePair>& attributePairs) {
            for(const auto& pair : attributePairs) {
                const std::string& key = pair.first;
                const std::string& valueStr = pair.second;

                // determine type and convert the string
                AttributeValue value = parseStringToAttributeValue(valueStr);

                // Validate type against TypeRegistry
                AttributeType valueType = TypeRegistry::getInstance().getTypeFromValue(value);
                TypeRegistry::getInstance().validateAndRegisterType(key, valueType);

                attributes_[key] = value;
            }
        }        const AttributeValue* ValueObject::getAttribute(const std::string& attributeName) const {
            auto it = attributes_.find(attributeName);
            if(it != attributes_.end()) {
                return &(it->second);
            }
            return nullptr;
        }

        void ValueObject::setAttribute(const std::string& attributeName, const std::string& value) {
            AttributeType valueType = TypeRegistry::getInstance().getTypeFromValue(AttributeValue(value));
            TypeRegistry::getInstance().validateAndRegisterType(attributeName, valueType);
            attributes_[attributeName] = value;
        }

        void ValueObject::setAttribute(const std::string& attributeName, int value) {
            AttributeType valueType = TypeRegistry::getInstance().getTypeFromValue(AttributeValue(value));
            TypeRegistry::getInstance().validateAndRegisterType(attributeName, valueType);
            attributes_[attributeName] = value;
        }

        void ValueObject::setAttribute(const std::string& attributeName, double value) {
            AttributeType valueType = TypeRegistry::getInstance().getTypeFromValue(AttributeValue(value));
            TypeRegistry::getInstance().validateAndRegisterType(attributeName, valueType);
            attributes_[attributeName] = value;
        }

        void ValueObject::setAttribute(const std::string& attributeName, bool value) {
            AttributeType valueType = TypeRegistry::getInstance().getTypeFromValue(AttributeValue(value));
            TypeRegistry::getInstance().validateAndRegisterType(attributeName, valueType);
            attributes_[attributeName] = value;
        }

        bool ValueObject::hasAttribute(const std::string& attributeName) const {
            return attributes_.find(attributeName) != attributes_.end();
        }

        const std::unordered_map<std::string, AttributeValue>& ValueObject::getAttributes() const {
            return attributes_;
        }

        std::string ValueObject::toString() const {
            std::ostringstream oss;
            bool first = true;

            for(const auto& pair : attributes_) {
                if(!first) {
                    oss << ", ";
                }
                oss << pair.first << ": ";

                // visit the variant to convert value to string
                std::visit([&oss](const auto& value) {
                    if constexpr(std::is_same_v<std::decay_t<decltype(value)>, bool>) {
                        oss << (value ? "true" : "false");
                    }
                    else {
                        oss << value;
                    }
                }, pair.second);

                first = false;
            }

            return oss.str();
        }

        AttributeValue ValueObject::parseStringToAttributeValue(const std::string& valueStr) {
            // try to parse as boolean first
            if(valueStr == "true" || valueStr == "false") {
                return valueStr == "true";
            }

            // try to parse as integer
            try {
                std::size_t pos;
                int intValue = std::stoi(valueStr, &pos);
                if(pos == valueStr.length()) {
                    return intValue;
                }
            }
            catch(const std::exception&) {
                // not an integer
            }

            // try to parse as double
            try {
                std::size_t pos;
                double doubleValue = std::stod(valueStr, &pos);
                if(pos == valueStr.length()) {
                    return doubleValue;
                }
            }
            catch(const std::exception&) {
                // not a double
            }

            // default is string
            return valueStr;
        }

    }
}