#include <iostream>
#include <vector>
#include <string>
#include "kvstore/cli/CLI.hpp"

/**
 * Interactive CLI main entry point
 * Provides a REPL-style interface for the key-value store
 */
int main(int argc, char* argv[]) {
    try {        // Parse command line options
        bool verbose = false;
        bool jsonMode = false;
        std::string storeFile = "store/store.json";
        bool autoSave = true;

        // Simple argument parsing
        for(int i = 1; i < argc; ++i) {
            std::string arg = argv[i];

            if(arg == "--verbose" || arg == "-v") {
                verbose = true;
            }
            else if(arg == "--json" || arg == "-j") {
                jsonMode = true;
            }
            else if(arg == "--no-autosave") {
                autoSave = false;
            }
            else if(arg == "--file" || arg == "-f") {
                if(i + 1 < argc) {
                    std::string userPath = argv[++i];
                    // Normalize the path - if it's just a filename, put it in store/
                    if(userPath.find('/') == std::string::npos && userPath.find('\\') == std::string::npos) {
                        storeFile = "store/" + userPath;
                    }
                    else {
                        storeFile = userPath;
                    }
                }
                else {
                    std::cerr << "Error: --file requires a filename argument" << std::endl;
                    return 1;
                }
            }
            else if(arg == "--help" || arg == "-h") {
                std::cout << "KVC++ Interactive CLI" << std::endl;
                std::cout << "Usage: " << argv[0] << " [OPTIONS]" << std::endl;
                std::cout << std::endl;
                std::cout << "Options:" << std::endl;
                std::cout << "  -h, --help        Show this help message" << std::endl;
                std::cout << "  -v, --verbose     Enable verbose output" << std::endl;
                std::cout << "  -j, --json        Enable JSON output mode" << std::endl;
                std::cout << "  -f, --file FILE   Use FILE as the store file (default: store/store.json)" << std::endl;
                std::cout << "  --no-autosave     Disable automatic saving on exit" << std::endl;
                std::cout << std::endl;
                std::cout << "Interactive Commands:" << std::endl;
                std::cout << "  Type 'help' in the interactive session for available commands" << std::endl;
                return 0;
            }
            else {
                std::cerr << "Error: Unknown option: " << arg << std::endl;
                std::cerr << "Use --help for usage information" << std::endl;
                return 1;
            }
        }

// Create CLI with the correct store file after parsing all arguments
        kvspp::cli::CLI cli(storeFile);

        // Configure CLI
        cli.setVerboseMode(verbose);
        cli.setJsonMode(jsonMode);
        cli.setAutoSave(autoSave);

        // Run interactive mode
        return cli.runInteractive();

    }
    catch(const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
