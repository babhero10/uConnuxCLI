#ifndef EXIT_HPP
#define EXIT_HPP

#include "../../include/icommand.hpp"
#include <vector>
#include <string>

class ExitCommand : public ICommand {
public:
  ExitCommand() = default;
  virtual ~ExitCommand() = default;

  std::string getName() const override;
  std::string getDescription() const override;
  int execute(const std::vector<std::string> &arguments) override;
};

#endif
