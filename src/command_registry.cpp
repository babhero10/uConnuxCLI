#include "../include/command_registry.hpp"
#include "../include/logger.hpp" // Include logger definitions (needed for extern declaration and usage)

#include <vector>
#include <string>

// Assume COMMAND_SUCCESS, COMMAND_ERROR, COMMAND_NOT_FOUND are defined in icommand.hpp
// (which is included via command_registry.hpp)

// Provide the definition for the global logger instance used in registerCommand/executeCommand
// This MUST be defined in exactly one .cpp file in your project (e.g., logger.cpp or main.cpp)
// We declare it extern here to link against that definition.
extern Logger logger;

// Constructor - Can be defaulted in header, but explicit definition is fine too.
CommandRegistry::CommandRegistry() = default;
// Destructor - Needs to be defined, even if default, because unique_ptr needs
//              the full definition of ICommand to destroy it. If ICommand's
//              destructor is non-trivial or virtual, this definition should be
//              in the .cpp file where ICommand is fully defined (or include icommand.hpp here).
//              Since icommand.hpp is included, default is okay here.
CommandRegistry::~CommandRegistry() = default;


ICommand* CommandRegistry::findCommand(const std::string& name) const {
    auto it = command_map.find(name);
    if (it != command_map.end()) {
        return it->second; // Return the non-owning pointer
    }
    return nullptr; // Command not found
}

int CommandRegistry::executeCommand(const std::vector<std::string>& arguments) {
    if (arguments.empty()) {
        logger.warn("executeCommand called with empty arguments.");
        return COMMAND_SUCCESS; // Or perhaps an error code indicating no command?
    }

    const std::string& commandName = arguments[0];
    ICommand* command = findCommand(commandName);

    if (command) {
        try {
            // Delegate execution to the found command object
            return command->execute(arguments);
        }
        catch (const std::exception& e) {
             logger.fatal("Exception during execution of '", commandName, "': ", e.what());
             return COMMAND_ERROR; // Indicate general error
        }
        catch (...) {
             logger.fatal("Unknown exception during execution of '", commandName, "'.");
             return COMMAND_ERROR; // Indicate general error
        }
    } else {
        logger.fatal("Unknown command: '", commandName, "'. Type 'help' for available commands.");
        return COMMAND_NOT_FOUND; // Indicate command not found
    }
}

const std::map<std::string, ICommand*>& CommandRegistry::getCommands() const {
    return command_map;
}
