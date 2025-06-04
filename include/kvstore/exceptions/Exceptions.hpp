#pragma once

#include <stdexcept>
#include <string>

namespace kvspp {
    namespace exceptions {

        /**
         * Base exception class for all KV store related exceptions
         */
        class KVStoreException : public std::runtime_error {
        public:
            explicit KVStoreException(const std::string& message) 
                : std::runtime_error(message) {}
        };

        /**
         * Exception thrown when there's a type mismatch for an attribute
         * This occurs when trying to set an attribute with a different type
         * than what was previously registered
         */
        class TypeMismatchException : public KVStoreException {
        public:
            TypeMismatchException(const std::string& attributeName, 
                                const std::string& expectedType, 
                                const std::string& actualType)
                : KVStoreException("Type mismatch for attribute '" + attributeName + 
                                "': expected " + expectedType + ", got " + actualType) {}
        };

        /**
         * Exception thrown when trying to parse an invalid value
         * For example, trying to parse "abc" as an integer
         */
        class InvalidValueException : public KVStoreException {
        public:
            InvalidValueException(const std::string& value, const std::string& expectedType)
                : KVStoreException("Cannot parse '" + value + "' as " + expectedType) {}
        };

        /**
         * Exception thrown when a key is not found in the store
         */
        class KeyNotFoundException : public KVStoreException {
        public:
            explicit KeyNotFoundException(const std::string& key)
                : KVStoreException("Key '" + key + "' not found in store") {}
        };

        /**
         * Exception thrown when an attribute is not found in a value object
         */
        class AttributeNotFoundException : public KVStoreException {
        public:
            AttributeNotFoundException(const std::string& attributeName, const std::string& key)
                : KVStoreException("Attribute '" + attributeName + "' not found in key '" + key + "'") {}
        };

        /**
         * Exception thrown for persistence related errors
         */
        class PersistenceException : public KVStoreException {
        public:
            explicit PersistenceException(const std::string& message)
                : KVStoreException("Persistence error: " + message) {}
        };

    }
} 