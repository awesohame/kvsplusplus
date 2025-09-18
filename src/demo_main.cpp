
#include <iostream>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif
#include "kvstore/core/StoreManager.hpp"

int main() {
    auto& manager = kvstore::StoreManager::instance();


    // Use two different tokens
    std::string token1 = "store1";
    std::string token2 = "store2";

    // Put values in both stores
    manager.put(token1, "keyA", "valueA1");
    manager.put(token2, "keyA", "valueA2");
    manager.put(token1, "keyB", "valueB1");
    manager.put(token2, "keyB", "valueB2");

    // Ensure /store directory exists
#ifdef _WIN32
    _mkdir("store");
#else
    mkdir("store", 0777);
#endif

    // Save both stores in /store
    manager.saveStore(token1, "store/store1.json");
    manager.saveStore(token2, "store/store2.json");

    // Clear all stores and reload from files
    manager.clearAllStores();
    manager.loadStore(token1, "store/store1.json");
    manager.loadStore(token2, "store/store2.json");

    // Retrieve and print values after reload
    std::cout << "After reload:" << std::endl;
    std::cout << "store1:keyA = " << manager.get(token1, "keyA") << std::endl;
    std::cout << "store2:keyA = " << manager.get(token2, "keyA") << std::endl;
    std::cout << "store1:keyB = " << manager.get(token1, "keyB") << std::endl;
    std::cout << "store2:keyB = " << manager.get(token2, "keyB") << std::endl;

    // Demonstrate isolation
    manager.remove(token1, "keyA");
    std::cout << "After removing keyA from store1:" << std::endl;
    try {
        std::cout << "store1:keyA = " << manager.get(token1, "keyA") << std::endl;
    }
    catch(const std::exception& e) {
        std::cout << "store1:keyA not found: " << e.what() << std::endl;
    }
    std::cout << "store2:keyA = " << manager.get(token2, "keyA") << std::endl;

    return 0;
}
