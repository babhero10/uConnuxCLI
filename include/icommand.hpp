#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <vector>

// Define constants for command execution results
#define COMMAND_SUCCESS 0;
#define COMMAND_ERROR 1;
#define COMMAND_EXIT_REQUESTED -99 // Special code for exit
#define COMMAND_NOT_FOUND -1;     // Special code if dispatcher fails

class ICommand {
public:
  virtual std::string getName() const = 0;
  virtual std::string getDescription() const = 0;
  virtual int execute(const std::vector<std::string>& arguments) = 0;
  virtual ~ICommand() = default;
};

#endif
