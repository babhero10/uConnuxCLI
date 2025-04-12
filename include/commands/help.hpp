#ifndef HELP_COMMAND_HPP
#define HELP_COMMAND_HPP

#include "../icommand.hpp" // Use relative path as in your main.cpp
#include "../command_registry.hpp"
#include <string>
#include <vector>

// Forward declaration is sufficient here as we only use a reference
class CommandRegistry;

class HelpCommand : public ICommand {
private:
    // Store a *reference* to the registry to access command list
    // Use const& because HelpCommand shouldn't modify the registry
    const CommandRegistry& registry_;

public:
    explicit HelpCommand(const CommandRegistry& registry);
    virtual ~HelpCommand() = default; // Virtual destructor

    // --- ICommand Interface ---
    virtual std::string getName() const override;
    virtual std::string getDescription() const override; // Added for better help
    virtual int execute(const std::vector<std::string>& arguments) override;
};

#endif // HELP_COMMAND_HPP
