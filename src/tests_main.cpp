#include <iostream>
#include <vector>
#include "kvstore/core/KeyValueStore.hpp"
#include "kvstore/exceptions/Exceptions.hpp"
#include "kvstore/utils/Helpers.hpp"

int main() {
    try {
        std::cout << "=== KVS++ Notes App Search Cache Test ===" << std::endl;

        // create a key-value store instance
        kvspp::core::KeyValueStore store;// dummy data -> search query cache for a notes app
        std::vector<std::pair<std::string, std::string>> query1 = {
            {"query", "machine learning algorithms"},
            {"results_count", "12"},
            {"last_accessed", "true"},
            {"execution_time", "85"}  // milliseconds
        };        std::cout << "Caching search query 'ml_search_001'..." << std::endl;
        store.put("ml_search_001", query1); // test put
        std::cout << kvspp::utils::ColorOutput::passMsg("Successfully cached search query") << std::endl;
        std::cout << "Getting cached query 'ml_search_001'..." << std::endl;
        const auto* valueObj = store.get("ml_search_001"); // test get
        if(valueObj) {
            std::cout << kvspp::utils::ColorOutput::passMsg("Found: " + valueObj->toString()) << std::endl;
        }
        else {
            std::cout << kvspp::utils::ColorOutput::failMsg("Query cache not found!") << std::endl;
        }

        std::cout << "All cached queries: ";
        auto allKeys = store.keys(); // test keys
        for(const auto& key : allKeys) {
            std::cout << key << " ";
        }
        std::cout << std::endl;        std::cout << "Searching for recently accessed queries (last_accessed=true)..." << std::endl;
        auto searchResults = store.search("last_accessed", "true"); // test search
        std::cout << kvspp::utils::ColorOutput::passMsg("Found " + std::to_string(searchResults.size()) + " matches") << std::endl;

        // test custom exception - TypeMismatchException
        std::cout << "\n=== Testing Custom Exception Handling ===" << std::endl;
        try {
            // first, create another search query with conflicting type
            std::vector<std::pair<std::string, std::string>> badQuery = {
                {"query", "database design"},
                {"results_count", "not_a_number"},  // this will be parsed as string, but results_count was established as int
                {"last_accessed", "false"}
            };            std::cout << "Attempting to store conflicting type for 'results_count' attribute..." << std::endl;            store.put("bad_query", badQuery);
            std::cout << kvspp::utils::ColorOutput::failMsg("ERROR: Should have thrown TypeMismatchException!") << std::endl;
        }
        catch(const kvspp::exceptions::TypeMismatchException& e) {
            std::cout << kvspp::utils::ColorOutput::passMsg("Successfully caught TypeMismatchException: " + std::string(e.what())) << std::endl;
        }
        // test setAttribute methods with type validation
        std::cout << "\n=== Testing setAttribute with Type Validation ===" << std::endl;        try {
            kvspp::core::ValueObject directObj;
            directObj.setAttribute("search_term", "deep learning");  // string
            directObj.setAttribute("result_count", 8);        // int  
            directObj.setAttribute("avg_relevance", 87.5); // double
            directObj.setAttribute("cached", true);    // bool

            std::cout << kvspp::utils::ColorOutput::passMsg("Created search cache object with mixed types: " + directObj.toString()) << std::endl;

            // now try to violate the type that was just established            std::cout << "Attempting to set 'result_count' as string (should fail)..." << std::endl;            directObj.setAttribute("result_count", "eight");  // this should throw TypeMismatchException

            std::cout << kvspp::utils::ColorOutput::failMsg("ERROR: Should have thrown TypeMismatchException!") << std::endl;
        }
        catch(const kvspp::exceptions::TypeMismatchException& e) {
            std::cout << kvspp::utils::ColorOutput::passMsg("Successfully caught TypeMismatchException: " + std::string(e.what())) << std::endl;
        }

        std::cout << "=== Test Completed Successfully ===" << std::endl;// test persistence functionality
        std::cout << "\n=== Testing Persistence Manager ===" << std::endl; // add more test data
        std::vector<std::pair<std::string, std::string>> query2 = {
            {"query", "neural networks fundamentals"},
            {"results_count", "7"},
            {"last_accessed", "true"},
            {"execution_time", "65"}
        };
        store.put("nn_search_002", query2);

        std::vector<std::pair<std::string, std::string>> query3 = {
            {"query", "cloud computing basics"},
            {"results_count", "3"},
            {"last_accessed", "false"},
            {"execution_time", "45"}
        };        store.put("cloud_search_003", query3);        std::cout << kvspp::utils::ColorOutput::passMsg("Added more test data. Current store has " + std::to_string(store.size()) + " entries") << std::endl;

        // save current store data
        std::string testFile = "store/store.json";
        std::cout << "Saving store to: " << testFile << std::endl;
        store.save(testFile);
        std::cout << kvspp::utils::ColorOutput::passMsg("Store saved successfully") << std::endl;

        // create a new store and load the data
        kvspp::core::KeyValueStore newStore;
        std::cout << "Loading data into new store..." << std::endl;
        newStore.load(testFile);

        // verify the loaded data
        auto loadedKeys = newStore.keys();
        std::cout << kvspp::utils::ColorOutput::passMsg("Loaded " + std::to_string(loadedKeys.size()) + " keys from file") << std::endl;        for(const auto& key : loadedKeys) {
            const auto* loadedValueObj = newStore.get(key);
            if(loadedValueObj) {
                std::cout << "  Loaded: " << key << " -> " << loadedValueObj->toString() << std::endl;
            }
        }

        // test that TypeRegistry state is maintained
        std::cout << "\nTesting that type consistency persists across sessions..." << std::endl;
        try {
            kvspp::core::ValueObject testObj;
            testObj.setAttribute("results_count", "invalid_count"); // should fail - results_count was int
            newStore.put("test_consistency", testObj);
            std::cout << kvspp::utils::ColorOutput::failMsg("ERROR: Should have thrown TypeMismatchException!") << std::endl;
        }
        catch(const kvspp::exceptions::TypeMismatchException& e) {
            std::cout << kvspp::utils::ColorOutput::passMsg("Type consistency maintained after load: " + std::string(e.what())) << std::endl;
        }

        // test search functionality on loaded data
        std::cout << "\nTesting search on loaded data..." << std::endl;
        auto loadedSearchResults = newStore.search("last_accessed", "true");
        std::cout << kvspp::utils::ColorOutput::passMsg("Found " + std::to_string(loadedSearchResults.size()) + " search queries with last_accessed=true") << std::endl;

        std::cout << "=== All Tests Completed Successfully ===" << std::endl;
    }
    catch(const kvspp::exceptions::KVStoreException& e) {
        std::cerr << "KV Store Error: " << e.what() << std::endl;
        return 1;
    }
    catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}