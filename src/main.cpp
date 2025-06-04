#include <iostream>
#include <vector>
#include "kvstore/core/KeyValueStore.hpp"
#include "kvstore/exceptions/Exceptions.hpp"

int main() {
    try {
        std::cout << "=== KV Store Test ===" << std::endl;

        // Create a key-value store instance
        kvcpp::core::KeyValueStore store;

        // Test data
        std::vector<std::pair<std::string, std::string>> attributes = {
            {"title", "SDE-Bootcamp"},
            {"price", "30000.00"},
            {"enrolled", "false"},
            {"estimated_time", "30"}
        };

        // Test put operation
        std::cout << "Adding key 'sde_bootcamp'..." << std::endl;
        store.put("sde_bootcamp", attributes);

        // Test get operation
        std::cout << "Getting key 'sde_bootcamp'..." << std::endl;
        const auto* valueObj = store.get("sde_bootcamp");
        if(valueObj) {
            std::cout << "Found: " << valueObj->toString() << std::endl;
        }
        else {
            std::cout << "Key not found!" << std::endl;
        }

        // Test keys operation
        std::cout << "All keys: ";
        auto allKeys = store.keys();
        for(const auto& key : allKeys) {
            std::cout << key << " ";
        }
        std::cout << std::endl;        // Test search operation
        std::cout << "Searching for enrolled=false..." << std::endl;
        auto searchResults = store.search("enrolled", "false");
        std::cout << "Found " << searchResults.size() << " matches" << std::endl;        // Test custom exception handling - TypeMismatchException
        std::cout << "\n=== Testing Custom Exception Handling ===" << std::endl;
        try {
            // First, create another object with the same attribute but different type
            std::vector<std::pair<std::string, std::string>> badAttributes = {
                {"title", "Advanced Course"},
                {"price", "not_a_number"},  // This will be parsed as string, but price was established as double
                {"enrolled", "true"}
            };

            std::cout << "Attempting to store conflicting type for 'price' attribute..." << std::endl;
            store.put("bad_course", badAttributes);
            std::cout << "ERROR: Should have thrown TypeMismatchException!" << std::endl;
        }
        catch(const kvcpp::exceptions::TypeMismatchException& e) {
            std::cout << "[PASS] Successfully caught TypeMismatchException: " << e.what() << std::endl;
        }

        // Test setAttribute methods with type validation
        std::cout << "\n=== Testing setAttribute with Type Validation ===" << std::endl;
        try {
            kvcpp::core::ValueObject directObj;
            directObj.setAttribute("name", "Alice");  // string
            directObj.setAttribute("age", 25);        // int  
            directObj.setAttribute("salary", 75000.50); // double
            directObj.setAttribute("active", true);    // bool

            std::cout << "[PASS] Created object with mixed types: " << directObj.toString() << std::endl;

            // Now try to violate the type that was just established
            std::cout << "Attempting to set 'age' as string (should fail)..." << std::endl;
            directObj.setAttribute("age", "twenty-five");  // This should throw TypeMismatchException

            std::cout << "ERROR: Should have thrown TypeMismatchException!" << std::endl;

        }
        catch(const kvcpp::exceptions::TypeMismatchException& e) {
            std::cout << "[PASS] Successfully caught TypeMismatchException: " << e.what() << std::endl;
        }

        std::cout << "=== Test Completed Successfully ===" << std::endl;

    }
    catch(const kvcpp::exceptions::KVStoreException& e) {
        std::cerr << "KV Store Error: " << e.what() << std::endl;
        return 1;
    }
    catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}