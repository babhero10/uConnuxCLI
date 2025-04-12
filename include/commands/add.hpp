#ifndef ADD_HPP
#define ADD_HPP

#include "../../include/args_opt.hpp"
#include "../../include/icommand.hpp"

class AddCommand : public ICommand {
private:
  opt_parser::OptionsParser parser;

public:
  AddCommand();
  virtual ~AddCommand() = default;

  std::string getName() const override;
  std::string getDescription() const override;
  int execute(const std::vector<std::string> &arguments) override;
};

#endif
