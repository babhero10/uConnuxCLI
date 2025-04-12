#ifndef CLEAR_HPP
#define CLEAR_HPP

#include "../../include/icommand.hpp"
#include <vector>
#include <string>

class ClearCommand : public ICommand {
public:
  ClearCommand() = default;
  virtual ~ClearCommand() = default;

  std::string getName() const override;
  std::string getDescription() const override;
  int execute(const std::vector<std::string> &arguments) override;
};

#endif
