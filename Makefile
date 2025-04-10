# Compiler and flags
CC = gcc
# Add -MMD -MP to generate dependency files (.d)
# Add -I$(SRC_DIR) if headers might be alongside source files in subdirs
CFLAGS = -Wall -I./include -I$(SRC_DIR) -MMD -MP
LDFLAGS = -lreadline

# Directories
SRC_DIR = ./src
INC_DIR = ./include # Included via CFLAGS
BUILD_DIR = ./build
# Define the desired log directory using the HOME environment variable
# Ensure HOME is set in your environment for this to work
LOG_DIR = $(HOME)/uconnux

# Target executable name
TARGET = uconnux-cli

# Find all source files recursively within SRC_DIR
SOURCES = $(shell find $(SRC_DIR) -name '*.c')

# Define corresponding object files in BUILD_DIR, preserving subdirectory structure
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

# Define corresponding dependency files in BUILD_DIR, preserving subdirectory structure
DEPS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.d, $(SOURCES))

# Default target: Build the executable AND ensure the log directory exists
# Make 'all' depend on both the target executable and the log directory target
all: $(BUILD_DIR)/$(TARGET) $(LOG_DIR)

# Rule to link object files into the final executable
# Added BUILD_DIR as an order-only prerequisite
$(BUILD_DIR)/$(TARGET): $(OBJECTS) | $(BUILD_DIR)
	@echo "Linking..."
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build finished: $@"

# Include the dependency files (.d) generated by the compiler
-include $(DEPS)

# Rule to compile source files into object files
# Creates necessary subdirectories in BUILD_DIR before compiling
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR) # Ensure top-level build dir exists first
	@echo "Compiling $< -> $@"
	@mkdir -p $(dir $@) # Ensure the target object directory exists in BUILD_DIR
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to create the top-level build directory
$(BUILD_DIR):
	@echo "Creating build directory: $@"
	@mkdir -p $@

# Rule to create the log directory (if it doesn't exist)
# This is NOT phony because it represents a real directory on the filesystem
# Make will check if the directory $(LOG_DIR) exists. If not, it runs the command.
$(LOG_DIR):
	@echo "Ensuring log directory exists: $@"
	@mkdir -p $@

# Clean up build artifacts (removes ONLY the build dir)
# NOTE: We intentionally DO NOT clean the LOG_DIR here, as it might contain valuable logs.
clean:
	@echo "Cleaning build directory: $(BUILD_DIR)"
	@rm -rf $(BUILD_DIR)

# Declare non-file targets as phony
.PHONY: all clean list

# Optional: Add a rule to list found sources/objects for debugging
list:
	@echo "Sources:"
	@echo "$(SOURCES)"
	@echo "Objects:"
	@echo "$(OBJECTS)"
	@echo "Deps:"
	@echo "$(DEPS)"
	@echo "Log Directory Variable:"
	@echo "$(LOG_DIR)"
