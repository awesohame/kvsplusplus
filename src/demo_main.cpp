#include <iostream>
#include <vector>
#include <string>
#include "kvstore/core/KeyValueStore.hpp"
#include "kvstore/exceptions/Exceptions.hpp"
#include "kvstore/utils/Helpers.hpp"

/**
 * Demo application showcasing the KVS++ store functionality
 * This creates sample data and demonstrates various operations
 */
int main() {
    try {
        std::cout << "=== KVS++ Store Demo Application ===" << std::endl;
        std::cout << "This demo showcases the key-value store functionality with sample data." << std::endl;
        std::cout << std::endl;        // Create a new store
        kvspp::core::KeyValueStore store;

        std::cout << "1. Setting up sample data..." << std::endl;

        // Sample 1: E-commerce course data
        std::vector<std::pair<std::string, std::string>> course1 = {
            {"title", "SDE-Bootcamp"},
            {"price", "30000.00"},
            {"enrolled", "false"},
            {"estimated_time", "30"}
        };
        store.put("sde_bootcamp", course1);

        std::vector<std::pair<std::string, std::string>> course2 = {
            {"title", "Data Science Masterclass"},
            {"price", "45000.00"},
            {"enrolled", "true"},
            {"estimated_time", "40"}
        };
        store.put("data_science_course", course2);

        std::vector<std::pair<std::string, std::string>> course3 = {
            {"title", "Machine Learning Fundamentals"},
            {"price", "25000.00"},
            {"enrolled", "true"},
            {"estimated_time", "25"}
        };
        store.put("ml_fundamentals", course3);

        // Sample 2: User profiles
        std::vector<std::pair<std::string, std::string>> user1 = {
            {"name", "Alice Johnson"},
            {"age", "28"},
            {"premium", "true"},
            {"score", "95.5"}
        };
        store.put("user_001", user1);

        std::vector<std::pair<std::string, std::string>> user2 = {
            {"name", "Bob Smith"},
            {"age", "35"},
            {"premium", "false"},
            {"score", "87.2"}
        };
        store.put("user_002", user2);

        std::vector<std::pair<std::string, std::string>> user3 = {
            {"name", "Carol Davis"},
            {"age", "22"},
            {"premium", "true"},
            {"score", "92.8"}
        };
        store.put("user_003", user3);

        std::cout << kvspp::utils::ColorOutput::passMsg("Created " + std::to_string(store.size()) + " sample entries") << std::endl;
        std::cout << std::endl;

        // Demo 2: Basic operations
        std::cout << "2. Demonstrating basic operations..." << std::endl;        // Get operation
        std::cout << "Getting 'sde_bootcamp':" << std::endl;
        const auto* course = store.get("sde_bootcamp");        if(course) {
            std::cout << "  " << course->toString() << std::endl;
            std::cout << kvspp::utils::ColorOutput::passMsg("Successfully retrieved course data") << std::endl;
        }

        // Keys operation
        std::cout << "\nAll keys in store:" << std::endl;
        auto allKeys = store.keys();
        for(const auto& key : allKeys) {
            std::cout << "  - " << key << std::endl;
        }
        std::cout << std::endl;

        // Demo 3: Search operations
        std::cout << "3. Demonstrating search functionality..." << std::endl;        // Search for enrolled courses
        std::cout << "Courses where enrolled=true:" << std::endl;
        auto enrolledCourses = store.search("enrolled", "true");
        for(const auto& key : enrolledCourses) {
            const auto* value = store.get(key);
            if(value) {
                std::cout << "  " << key << ": " << value->toString() << std::endl;
            }
        }
        std::cout << kvspp::utils::ColorOutput::passMsg("Found " + std::to_string(enrolledCourses.size()) + " enrolled courses") << std::endl;        // Search for premium users
        std::cout << "\nPremium users (premium=true):" << std::endl;
        auto premiumUsers = store.search("premium", "true");
        for(const auto& key : premiumUsers) {
            const auto* value = store.get(key);
            if(value) {
                std::cout << "  " << key << ": " << value->toString() << std::endl;
            }
        }
        std::cout << kvspp::utils::ColorOutput::passMsg("Found " + std::to_string(premiumUsers.size()) + " premium users") << std::endl;        // Search by age
        std::cout << "\nUsers aged 28:" << std::endl;
        auto age28Users = store.search("age", "28");
        for(const auto& key : age28Users) {
            const auto* value = store.get(key);
            if(value) {
                std::cout << "  " << key << ": " << value->toString() << std::endl;
            }
        }
        std::cout << kvspp::utils::ColorOutput::passMsg("Found " + std::to_string(age28Users.size()) + " users aged 28") << std::endl;
        std::cout << std::endl;

        // Demo 4: Type consistency
        std::cout << "4. Demonstrating type consistency..." << std::endl;

        try {
            // This should work - age is already established as int
            std::vector<std::pair<std::string, std::string>> user4 = {
                {"name", "Dave Wilson"},
                {"age", "45"},  // This is fine, will be parsed as int
                {"premium", "false"},
                {"score", "78.9"}
            };
            store.put("user_004", user4);
            std::cout << kvspp::utils::ColorOutput::passMsg("Successfully added user_004 with consistent types") << std::endl;

            // This should fail - age was established as int, trying to use string
            std::vector<std::pair<std::string, std::string>> badUser = {
                {"name", "Eve Brown"},
                {"age", "twenty-five"},  // This should fail
                {"premium", "true"}
            };            store.put("bad_user", badUser);
            std::cout << kvspp::utils::ColorOutput::failMsg("ERROR: Should have failed due to type mismatch!") << std::endl;

        }
        catch(const kvspp::exceptions::TypeMismatchException& e) {
            std::cout << kvspp::utils::ColorOutput::passMsg("Correctly caught type mismatch: " + std::string(e.what())) << std::endl;
        }
        std::cout << std::endl;

        // Demo 5: Persistence
        std::cout << "5. Demonstrating persistence..." << std::endl;

        std::string demoFile = "store/demo_store.json";        std::cout << "Saving store to: " << demoFile << std::endl;        store.save(demoFile);
        std::cout << kvspp::utils::ColorOutput::passMsg("Store saved successfully") << std::endl;

        // Create new store and load
        kvspp::core::KeyValueStore newStore;        std::cout << "Loading into new store..." << std::endl;        newStore.load(demoFile);
        std::cout << kvspp::utils::ColorOutput::passMsg("Loaded " + std::to_string(newStore.size()) + " entries into new store") << std::endl;

        // Verify the data
        const auto* loadedCourse = newStore.get("sde_bootcamp");        if(loadedCourse) {
            std::cout << kvspp::utils::ColorOutput::passMsg("Verified loaded data: " + loadedCourse->toString()) << std::endl;
        }
        std::cout << std::endl;

        // Demo 6: Advanced operations
        std::cout << "6. Demonstrating advanced operations..." << std::endl;

        // Update an existing key
        std::vector<std::pair<std::string, std::string>> updatedCourse = {
            {"title", "SDE-Bootcamp Pro"},  // Updated title
            {"price", "35000.00"},          // Updated price
            {"enrolled", "true"},           // Changed enrollment status
            {"estimated_time", "35"}        // Updated time
        };        store.put("sde_bootcamp", updatedCourse);  // This replaces the existing entry
        std::cout << kvspp::utils::ColorOutput::passMsg("Updated 'sde_bootcamp' entry") << std::endl;

        const auto* updated = store.get("sde_bootcamp");
        if(updated) {
            std::cout << "  New value: " << updated->toString() << std::endl;
        }        // Delete operation
        std::cout << "\nDeleting 'user_002'..." << std::endl;
        bool deleted = store.deleteKey("user_002");
        std::cout << (deleted ? kvspp::utils::ColorOutput::passMsg("Delete operation successful") : kvspp::utils::ColorOutput::failMsg("Delete operation failed")) << std::endl;
        std::cout << "Store now has " << store.size() << " entries" << std::endl;
        std::cout << std::endl;        std::cout << "=== Demo Completed Successfully ===" << std::endl;        std::cout << "You can now experiment with the CLI tools:" << std::endl;
        std::cout << "  - kvspp-cli (interactive mode)" << std::endl;
        std::cout << "  - kvspp-single-cmd (single command mode)" << std::endl;
        std::cout << "  - Use 'load " << demoFile << "' to load this demo data" << std::endl;

        return 0;

    }
    catch(const kvspp::exceptions::KVStoreException& e) {
        std::cerr << "KVStore Error: " << e.what() << std::endl;
        return 1;
    }
    catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
