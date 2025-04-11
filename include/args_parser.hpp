#ifndef ARGS_PARSER_H
#define ARGS_PARSER_H

#include <vector>
#include <string>

/**
 * @brief Parses a command-line string into arguments, handling quotes and expanding wildcards.
 *
 * Splits the input string into tokens, respecting single (') and double (") quotes.
 * Handles backslash (\) escape characters both inside and outside quotes.
 * Expands arguments containing shell wildcard patterns (*, ?, []) using glob().
 * Arguments with wildcards that match no files are omitted.
 * Arguments without wildcards, or patterns that cause glob errors, are passed through literally.
 *
 * @param commandLine The input command line string.
 * @return A std::vector<std::string> containing the parsed and expanded arguments.
 * @throws std::bad_alloc If memory allocation fails during parsing or expansion.
 * @throws std::runtime_error For specific parsing errors (e.g., unclosed quotes, though
 *                            this implementation might handle them leniently).
 */
std::vector<std::string> parseCommandLine(const std::string& commandLine);

#endif // ARGS_PARSER_H
