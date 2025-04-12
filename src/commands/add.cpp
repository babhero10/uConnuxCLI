#include "../../include/commands/add.hpp"
#include "../../include/args_opt.hpp"
#include "../../include/icommand.hpp"
#include "../../include/logger.hpp"

AddCommand::AddCommand() {
  parser.addOption('a', "add", opt_parser::ArgumentOptions::REQ_ARG);
}

std::string AddCommand::getName() const { return "add"; }
std::string AddCommand::getDescription() const {
  return "Adds a new item to the list.";
}

int AddCommand::execute(const std::vector<std::string> &arguments) {
  extern Logger logger;

  for (opt_parser::Option &opt :
       const_cast<std::vector<opt_parser::Option> &>(parser.getOptions())) {
    opt.set_found(false);
    opt.set_arg("");
  }

  int parse_result = parser.parseOptionsString(arguments);

  if (parse_result < 0) {
    logger.fatal("Error parsing arguments for '", getName(), "' command.");
    return COMMAND_ERROR;
  }

  const opt_parser::Option *itemOpt = parser.findOption('a');
  if (!itemOpt || !itemOpt->get_found() || itemOpt->get_arg().empty()) {
    logger.fatal("Error: Missing required argument -a/--add for command '",
                 getName(), "'.");
    return COMMAND_ERROR;
  }
  std::string itemToAdd = itemOpt->get_arg();


  logger.success("Item '", itemToAdd, "' added.");
  return COMMAND_SUCCESS;
}
