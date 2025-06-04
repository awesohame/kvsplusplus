#include <iostream>
#include <vector>
#include <string>
#include "kvstore/cli/CLI.hpp"

/**
 * Single command CLI main entry point
 * Executes a single command and exits (good for scripting)
 */
int main(int argc, char* argv[]) {
    try {
        if(argc < 2) {
            std::cerr << "Usage: " << argv[0] << " [OPTIONS] <command> [args...]" << std::endl;
            std::cerr << "Try: " << argv[0] << " help" << std::endl;
            return 1;
        }

        kvspp::cli::CLI cli;

        // Parse command line options and build command vector
        std::vector<std::string> command;
        bool verbose = false;
        bool jsonMode = false;
        std::string storeFile = "store/store.json";
        bool autoSave = true;

        // Parse arguments
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
                    storeFile = argv[++i];
                }
                else {
                    std::cerr << "Error: --file requires a filename argument" << std::endl;
                    return 1;
                }
            }
            else if(arg == "--help" || arg == "-h") {
                std::cout << "KVC++ Single Command CLI" << std::endl;
                std::cout << "Usage: " << argv[0] << " [OPTIONS] <command> [args...]" << std::endl;
                std::cout << std::endl;
                std::cout << "Options:" << std::endl;
                std::cout << "  -h, --help        Show this help message" << std::endl;
                std::cout << "  -v, --verbose     Enable verbose output" << std::endl;
                std::cout << "  -j, --json        Enable JSON output mode" << std::endl;
                std::cout << "  -f, --file FILE   Use FILE as the store file (default: store/store.json)" << std::endl;
                std::cout << "  --no-autosave     Disable automatic saving after command" << std::endl;
                std::cout << std::endl;
                std::cout << "Commands:" << std::endl;
                std::cout << "  get <key>                     Get value for a key" << std::endl;
                std::cout << "  put <key> <attr:val> ...      Store key with attributes" << std::endl;
                std::cout << "  delete <key>                  Delete a key" << std::endl;
                std::cout << "  search <attr> <value>         Find keys by attribute" << std::endl;
                std::cout << "  keys                          List all keys" << std::endl;
                std::cout << "  clear                         Clear all data" << std::endl;
                std::cout << "  save [filename]               Save store to file" << std::endl;
                std::cout << "  load [filename]               Load store from file" << std::endl;
                std::cout << "  stats                         Show store statistics" << std::endl;
                std::cout << "  inspect <key>                 Detailed view of a key" << std::endl;
                std::cout << "  help                          Show command help" << std::endl;
                std::cout << std::endl;
                std::cout << "Examples:" << std::endl;
                std::cout << "  " << argv[0] << " put user1 name:John age:25 active:true" << std::endl;
                std::cout << "  " << argv[0] << " get user1" << std::endl;
                std::cout << "  " << argv[0] << " search age 25" << std::endl;
                std::cout << "  " << argv[0] << " --json keys" << std::endl;
                return 0;
            }
            else {
                // This is part of the command
                command.push_back(arg);
            }
        }

        if(command.empty()) {
            std::cerr << "Error: No command specified" << std::endl;
            std::cerr << "Use --help for usage information" << std::endl;
            return 1;
        }

        // Configure CLI
        cli.setVerboseMode(verbose);
        cli.setJsonMode(jsonMode);
        cli.setDefaultStoreFile(storeFile);
        cli.setAutoSave(autoSave);

        // Execute single command
        return cli.runSingleCommand(command);

    }
    catch(const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
