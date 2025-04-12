#include "../../include/commands/clear.hpp"
#include "../../include/icommand.hpp"
#include "../../include/logger.hpp"
#include "../../include/theme.hpp"

  std::string ClearCommand::getName() const  { return "clear"; }
  std::string ClearCommand::getDescription() const  {
    return "Clears the terminal screen.";
  }

  int ClearCommand::execute(const std::vector<std::string> &arguments)  {
    extern Logger logger;
    if (arguments.size() > 1) {
      logger.fatal("Usage: ", getName());
      return COMMAND_ERROR;
    }
    clearScreen();
    return COMMAND_SUCCESS;
  }
