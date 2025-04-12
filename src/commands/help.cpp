#include "../../include/commands/help.hpp" // Adjust path if needed
#include "../../include/command_registry.hpp"
#include "../../include/logger.hpp" // For printing output
#include <string>
#include <vector>

// Make sure logger is accessible (likely declared extern elsewhere)
extern Logger logger;

HelpCommand::HelpCommand(const CommandRegistry &registry)
    : registry_(registry) {}

std::string HelpCommand::getName() const { return "help"; }

std::string HelpCommand::getDescription() const {
  return "Displays available commands or help for a specific command.";
}

int HelpCommand::execute(const std::vector<std::string> &arguments) {
  const auto &commands = registry_.getCommands(); // Get map [name -> ICommand*]

  if (arguments.size() == 1) {
    // General help: List all commands
    logger.info("Available commands:");
    // Determine max command name length for alignment (optional)
    size_t max_len = 0;
    for (const auto &pair : commands) {
      if (pair.first.length() > max_len) {
        max_len = pair.first.length();
      }
    }

    for (const auto &pair : commands) {
      std::string name = pair.first;
      std::string description =
          pair.second->getDescription(); // Get desc from command
      // Basic formatting: Pad name and add description
      logger.info("  ", name, std::string(max_len - name.length() + 2, ' '),
                  (description.empty() ? "(No description)" : description));
      // If you don't have info use:
      // logger.info("  ", name, std::string(max_len - name.length() + 2, ' '),
      // (description.empty() ? "(No description)" : description));
    }
    logger.info(
        "Type 'help <command_name>' for more details on a specific command.");

  } else if (arguments.size() == 2) {
    // Specific help: Show help for one command
    const std::string &command_to_help = arguments[1];
    ICommand *cmd = registry_.findCommand(command_to_help);

    if (cmd) {
      logger.info("Help for '", cmd->getName(), "':");
      logger.info("  Description: ", cmd->getDescription());
      // TODO: Add more detailed usage info here if ICommand supports it
      // e.g., logger.info("  Usage: ", cmd->getUsage());
    } else {
      logger.fatal("Unknown command '", command_to_help,
                   "'. Cannot display help.");
      return COMMAND_NOT_FOUND; // Or COMMAND_ERROR
    }
  } else {
    // Incorrect usage of the help command itself
    logger.fatal("Usage: help [command_name]");
    return COMMAND_ERROR; // Indicate incorrect arguments for help command
  }

  return COMMAND_SUCCESS;
}
