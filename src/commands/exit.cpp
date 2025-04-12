#include "../../include/commands/exit.hpp"
#include "../../include/icommand.hpp"
#include "../../include/logger.hpp"

    std::string ExitCommand::getName() const { return "exit"; }
    std::string ExitCommand::getDescription() const  { return "Exits the application."; }

    int ExitCommand::execute(const std::vector<std::string>& arguments)  {
        extern Logger logger;
        if (arguments.size() > 1) {
            logger.fatal("Usage: ", getName());
            return COMMAND_ERROR;
        }

        return COMMAND_EXIT_REQUESTED;
    }
