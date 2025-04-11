#include "../include/args_parser.hpp" // Include the header first

#include <vector>
#include <string>
#include <iostream>  // For std::cerr
#include <cctype>    // For std::isspace

// --- C headers needed for glob ---
#include <glob.h>    // For glob(), globfree(), glob_t
#include <cstring>   // For strpbrk, std::memset

namespace { // Use an anonymous namespace for internal linkage

// RAII wrapper for glob_t (No changes needed here, works with C++11)
class GlobResult {
private:
    glob_t glob_data;

public:
    GlobResult(const GlobResult&) = delete;
    GlobResult& operator=(const GlobResult&) = delete;

    GlobResult() {
        std::memset(&glob_data, 0, sizeof(glob_data));
    }

    int call_glob(const char* pattern, int flags) {
        if (glob_data.gl_pathc > 0) {
             globfree(&glob_data);
             std::memset(&glob_data, 0, sizeof(glob_data));
        }
        return glob(pattern, flags, nullptr, &glob_data);
    }

    size_t count() const { return glob_data.gl_pathc; }
    const char* const* paths() const { return glob_data.gl_pathv; }
    const char* operator[](size_t i) const { return glob_data.gl_pathv[i]; }

    ~GlobResult() {
        // Check gl_pathc directly, implies glob was called and might have allocated
        if (glob_data.gl_pathc > 0) {
            globfree(&glob_data);
        }
    }
};


// --- Argument Splitting Logic (Modified Escape Handling) ---
// Takes const std::string& now, as string_view is removed
std::vector<std::string> splitArguments(const std::string& input) {
    std::vector<std::string> tokens;
    std::string current_token;
    current_token.reserve(64);

    // State machine remains useful for quotes
    enum class State { DEFAULT, IN_SINGLE_QUOTES, IN_DOUBLE_QUOTES };
    State state = State::DEFAULT;

    for (std::size_t i = 0; i < input.length(); ++i) {
        char c = input[i];
        char next_c = (i + 1 < input.length()) ? input[i+1] : '\0'; // Lookahead

        switch (state) {
            case State::DEFAULT:
                if (std::isspace(static_cast<unsigned char>(c))) {
                    // End of token if we have accumulated one
                    if (!current_token.empty()) {
                        // Use emplace_back(std::move(...)) in C++11 for efficiency
                        tokens.emplace_back(std::move(current_token));
                        current_token.clear();
                        current_token.reserve(64);
                    }
                } else if (c == '\'') {
                    state = State::IN_SINGLE_QUOTES;
                } else if (c == '"') {
                    state = State::IN_DOUBLE_QUOTES;
                } else if (c == '\\') {
                    current_token += c;
                } else {
                    current_token += c;
                }
                break;

            case State::IN_SINGLE_QUOTES:
                if (c == '\'') {
                    state = State::DEFAULT;
                } else {
                    current_token += c;
                }
                break;

            case State::IN_DOUBLE_QUOTES:
                if (c == '"') {
                    state = State::DEFAULT; // End quote state
                    // Don't add the quote itself
                } else if (c == '\\') {
                    // --- Modified Escape Logic ---
                    // Check if the next character is special (\, ", ')
                    if (next_c == '\\' || next_c == '"' || next_c == '\'') {
                        // Escape the special character: add it and skip it in the next iteration
                        current_token += next_c;
                        i++; // Consume the escaped character
                    } else {
                        // Not escaping a special char, treat backslash literally
                        current_token += c;
                    }
                } else {
                    // Regular character inside double quotes
                    current_token += c;
                }
                break;
        }
    }

    // Handle unclosed quotes (same as before)
    if (state == State::IN_SINGLE_QUOTES || state == State::IN_DOUBLE_QUOTES) {
        std::cerr << "Warning: Unclosed quote encountered in input string." << std::endl;
    }

    // Add the last token if any
    if (!current_token.empty()) {
        tokens.emplace_back(std::move(current_token));
    }

    return tokens;
}


// --- Wildcard Expansion Logic (No changes needed, works with C++11) ---
std::vector<std::string> expandWildcards(const std::vector<std::string>& args) {
    std::vector<std::string> expanded_args;
    expanded_args.reserve(args.size());
    GlobResult glob_result;

    for (const std::string& arg : args) {
        // std::string::npos exists since C++98
        // arg.find_first_of is C++11
        if (arg.find_first_of("*?[]") == std::string::npos) {
            expanded_args.push_back(arg); // push_back is fine
        } else {
            // std::string::c_str() exists since C++98
            // GLOB_TILDE might be a GNU extension, but often available. GLOB_ERR is POSIX.
            int ret = glob_result.call_glob(arg.c_str(), GLOB_ERR | GLOB_TILDE);

            if (ret == 0) {
                size_t match_count = glob_result.count();
                expanded_args.reserve(expanded_args.size() + match_count);
                for (size_t j = 0; j < match_count; ++j) {
                    expanded_args.emplace_back(glob_result[j]);
                }
            } else if (ret == GLOB_NOMATCH) {
                // Do nothing.
            } else {
                std::cerr << "Warning: glob() failed for pattern '" << arg
                          << "' (error code " << ret << "). Treating pattern as literal argument.\n";
                expanded_args.push_back(arg);
            }
        }
    }
    return expanded_args;
}

} // end anonymous namespace


// --- Public Interface Function ---
// Only the std::string version remains
std::vector<std::string> parseCommandLine(const std::string& commandLine) {
    // 1. Split arguments respecting quotes (now takes std::string)
    std::vector<std::string> initial_args = splitArguments(commandLine);

    // 2. Expand wildcards
    std::vector<std::string> final_args = expandWildcards(initial_args);

    return final_args;
}
