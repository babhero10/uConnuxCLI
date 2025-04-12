#ifndef COMMAND_REGISTRY_HPP
#define COMMAND_REGISTRY_HPP

#include "icommand.hpp" // Defines ICommand, COMMAND_SUCCESS, etc.
#include "logger.hpp"   // Needed for logger usage within the template function

#include <vector>
#include <map>
#include <string>
#include <memory>       // For std::unique_ptr
#include <type_traits>  // For static_assert and std::is_base_of
#include <utility>      // For std::forward

// Forward declaration if Logger is only used by pointer/reference in header
// class Logger; // Not strictly needed here as logger.hpp is included for the template

class CommandRegistry {
private:
    // Owns the command objects
    std::vector<std::unique_ptr<ICommand>> command_objects;
    // Non-owning pointers for quick lookup by name
    std::map<std::string, ICommand*> command_map;

public:
    CommandRegistry();  // Constructor declaration
    ~CommandRegistry(); // Destructor declaration

    // Prevent copying and assignment
    CommandRegistry(const CommandRegistry&) = delete;
    CommandRegistry& operator=(const CommandRegistry&) = delete;

    // Prevent moving (optional, but unique_ptr members make it tricky anyway)
    CommandRegistry(CommandRegistry&&) = delete;
    CommandRegistry& operator=(CommandRegistry&&) = delete;

    /**
     * @brief Registers a command type.
     *
     * Creates an instance of the command T using the provided arguments,
     * stores it, and maps its name for lookup.
     * Overwrites existing command with the same name if found.
     *
     * @tparam T The command class type (must inherit from ICommand).
     * @tparam Args The types of arguments for T's constructor.
     * @param args Arguments to forward to T's constructor.
     */
    template<typename T, typename... Args>
    void registerCommand(Args&&... args) {
        // Ensure T derives from ICommand at compile time
        static_assert(std::is_base_of<ICommand, T>::value, "Command type T must inherit from ICommand");

        // Create the command instance using provided constructor args
        auto command_ptr = std::make_unique<T>(std::forward<Args>(args)...);
        std::string name = command_ptr->getName();

        // Check for duplicate registration
        if (command_map.count(name)) {
            // Access the global logger instance (declared elsewhere, e.g., logger.cpp or main.cpp)
            extern Logger logger;
            logger.warn("Command '", name, "' is already registered. Overwriting previous entry.");
            // Note: Overwriting might orphan the previous unique_ptr briefly before
            // the new one is added. Consider explicitly removing the old one if necessary,
            // though the current map/vector approach handles ownership correctly.
        }

        // Store the raw pointer in the map *before* moving ownership.
        // Using operator[] handles both insertion and overwrite.
        command_map[name] = command_ptr.get();

        // Transfer ownership of the command object to the vector.
        command_objects.push_back(std::move(command_ptr));
    }

    /**
     * @brief Finds a command by its registered name.
     * @param name The name of the command to find.
     * @return A non-owning pointer to the command if found, nullptr otherwise.
     */
    ICommand* findCommand(const std::string& name) const;

    /**
     * @brief Executes a command based on parsed arguments.
     *
     * The first element of 'arguments' is expected to be the command name.
     *
     * @param arguments A vector of strings representing the command and its arguments.
     * @return COMMAND_SUCCESS, COMMAND_ERROR, COMMAND_NOT_FOUND, or a command-specific code.
     */
    int executeCommand(const std::vector<std::string>& arguments);

    /**
     * @brief Provides read-only access to the map of registered commands.
     * Useful for help commands or autocompletion features.
     * @return A constant reference to the internal command map.
     */
    const std::map<std::string, ICommand*>& getCommands() const;
};

#endif // COMMAND_REGISTRY_HPP
