#include "../include/args_parser.h"
#include <ctype.h>
#include <glob.h> // Make sure glob.h is included
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS 64
#define MAX_TOKEN_LEN 256

/**
 * @brief Frees the memory allocated for an ArgList structure.
 */
ErrorCode free_args(ArgList *args) {
  if (args == NULL) {
    return ERR_NULL_POINTER; // Assuming this error code exists
  }

  if (args->argv == NULL) {
    args->argc = 0; // Ensure consistency
    return ERR_SUCCESS; // Or ERR_INVALID_STATE if argv is NULL unexpectedly
  }
  for (int i = 0; i < args->argc; ++i) {
    free(args->argv[i]); // Free each argument string
    args->argv[i] = NULL; // Good practice: nullify pointer after free
  }
  free(args->argv);      // Free the array of pointers
  args->argv = NULL;     // Good practice
  args->argc = 0;
  return ERR_SUCCESS;
}

/**
 * @brief Splits a command-line string into individual arguments, handling
 * quotes.
 */
static ArgList split_args(const char *input) {

  ArgList result = {0, NULL};

  if (input == NULL) {
    // Return an empty, but validly initialized list
    result.argv = malloc(MAX_TOKENS * sizeof(char *));
     if (!result.argv) {
         return result;
     }
     // Ensure the initial state is clean even if input is NULL
     result.argc = 0;
     return result; // Return list capable of holding args, but empty
  }

  result.argv = malloc(MAX_TOKENS * sizeof(char *));
  if (!result.argv) {
    fprintf(stderr, "Error: Failed to allocate memory for argv array.\n");
    // Return empty list on failure, indicating an error state
    result.argc = 0; // Explicitly set argc to 0
    return result;
  }

  const char *p = input;
  result.argc = 0; // Initialize argc correctly

  while (*p) {
    while (isspace((unsigned char)*p))
      p++; // Skip leading whitespace
    if (*p == '\0')
      break; // End of input string

    // Ensure we don't exceed MAX_TOKENS
    if (result.argc >= MAX_TOKENS) {
        fprintf(stderr, "Warning: Maximum token count (%d) reached. Input truncated.\n", MAX_TOKENS);
        break;
    }


    char token[MAX_TOKEN_LEN] = {0};
    int pos = 0;
    char quote_char = 0; // Use char to store quote type (' or ") or 0 if none

    if (*p == '\"' || *p == '\'') {
      quote_char = *p++; // Store the quote char and advance pointer
    }

    while (*p != '\0') {
        // Break conditions:
        if (quote_char) {
            if (*p == quote_char) {
                p++; // Consume the closing quote
                break; // End of quoted token
            }
            // Handle escape characters *within* quotes
            if (*p == '\\' && (p[1] == quote_char || p[1] == '\\')) {
                p++; // Skip backslash, take the next character literally
            }
        } else { // Not inside quotes
            if (isspace((unsigned char)*p)) {
                break; // End of non-quoted token
            }
            // Handle escape characters *outside* quotes (e.g., escaping space)
            if (*p == '\\' && p[1] != '\0') {
                 p++; // Skip backslash, take the next character literally
            }
        }

        // Add character to token if space allows
        if (pos < MAX_TOKEN_LEN - 1) {
            token[pos++] = *p++;
        } else {
            // Token buffer full, discard remaining chars of this token
            fprintf(stderr, "Warning: Token length exceeds limit (%d). Truncating.\n", MAX_TOKEN_LEN -1);
            // Consume the rest of the token until its natural end
             while (*p != '\0' && (quote_char ? (*p != quote_char) : !isspace((unsigned char)*p))) {
                 // Need to handle escapes even while discarding
                 if (*p == '\\' && p[1] != '\0') {
                    p++;
                 }
                 p++;
             }
             if (quote_char && *p == quote_char) p++; // Consume closing quote if present
             break; // Exit inner loop after handling oversized token
        }
    }
    // Null-terminate the token (already done by initialization if pos=0)
    token[pos] = '\0';


    // Add the token to the result list
    result.argv[result.argc] = strdup(token);
    if (!result.argv[result.argc]) {
      perror("strdup failed in split_args");
      free_args(&result); // result.argc is currently the index, so it correctly frees 0 to argc-1
      result.argc = 0;    // Reset count
      result.argv = NULL; // Signal complete failure
      return result;      // Return the empty/error state list
    }
    result.argc++; // Increment count *after* successful allocation and duplication

    // Check again if we hit MAX_TOKENS right after incrementing
     if (result.argc >= MAX_TOKENS && *p != '\0') {
         // Skip remaining whitespace before potentially breaking outer loop
         while (isspace((unsigned char)*p)) p++;
         if (*p != '\0') { // Check if there's more non-whitespace input
            fprintf(stderr, "Warning: Maximum token count (%d) reached. Input truncated.\n", MAX_TOKENS);
         }
         break; // Exit outer loop
     }

  }
  return result;
}


/**
 * @brief Expands wildcard patterns in arguments to matching file names.
 *        If a pattern contains wildcards but doesn't match any files
 *        (GLOB_NOMATCH), it's omitted from the final list.
 *        Arguments without wildcards are passed through unchanged.
 *        Other glob errors result in the pattern being treated literally.
 */
static void expand_wildcards(ArgList *args) {
  // Basic input check
  if (!args || !args->argv) {
    return; // Nothing to do
  }

  // Allocate space for potentially expanded arguments.
  // Using MAX_TOKENS might be insufficient if expansion is large.
  // A dynamic array (realloc) would be more robust.
  ArgList expanded = {0, NULL};
  expanded.argv = malloc(MAX_TOKENS * sizeof(char *));
  if (!expanded.argv) {
    perror("malloc failed for expanded argv");
    free_args(args); // Free the original args as we cannot proceed
    args->argc = 0;
    args->argv = NULL;
    return;
  }
  expanded.argc = 0; // Initialize count for the expanded list

  for (int i = 0; i < args->argc; ++i) {
    const char *current_arg = args->argv[i];

    // Optimization: Check if the argument actually contains glob characters
    // If not, no need to call glob()
    if (strpbrk(current_arg, "*?[]") == NULL) {
        // No wildcards, just copy the argument if space permits
        if (expanded.argc >= MAX_TOKENS) {
            fprintf(stderr, "Warning: Maximum token count (%d) reached during expansion (literal copy).\n", MAX_TOKENS);
            break; // Stop processing further arguments
        }
        expanded.argv[expanded.argc] = strdup(current_arg);
        if (!expanded.argv[expanded.argc]) {
            perror("strdup failed for literal argument");
            free_args(&expanded); // Free partially built list
            free_args(args);      // Free original list
            args->argc = 0;
            args->argv = NULL;
            return;
        }
        expanded.argc++;
        continue; // Move to the next original argument
    }

    // Argument contains potential wildcards, attempt expansion
    glob_t glob_result;
    memset(&glob_result, 0, sizeof(glob_result)); // Initialize glob_t

    // Flags:
    // GLOB_ERR: Causes glob to return on errors like read errors.
    // GLOB_NOCHECK: If pattern doesn't match, return the pattern itself. WE DON'T WANT THIS.
    // We want the default behavior where GLOB_NOMATCH is returned if no match.
    // Consider adding GLOB_TILDE for ~ expansion if needed.
    // Consider adding GLOB_BRACE for {a,b} expansion if needed.
    int ret = glob(current_arg, GLOB_ERR /*| GLOB_TILDE | GLOB_BRACE*/, NULL, &glob_result);

    if (ret == 0) {
      // Glob succeeded, found one or more matches
      for (size_t j = 0; j < glob_result.gl_pathc; ++j) {
        if (expanded.argc >= MAX_TOKENS) {
          fprintf(stderr, "Warning: Maximum token count (%d) reached during wildcard expansion.\n", MAX_TOKENS);
          // Need to break out cleanly
          goto max_tokens_reached;
        }
        expanded.argv[expanded.argc] = strdup(glob_result.gl_pathv[j]);
        if (!expanded.argv[expanded.argc]) {
          perror("strdup failed for glob result");
          globfree(&glob_result); // Free glob resources
          free_args(&expanded);   // Free partially built list
          free_args(args);        // Free original list
          args->argc = 0;
          args->argv = NULL;
          return;
        }
        expanded.argc++;
      }
    } else if (ret == GLOB_NOMATCH) {
      // Pattern contained wildcards, but matched no files.
      // Per requirement, DO NOTHING - skip this argument.
      // globfree() still needs to be called below.
    } else {
      // Other glob error (e.g., GLOB_NOSPACE, GLOB_ABORTED due to GLOB_ERR)
      // Print a warning and treat the pattern literally.
      fprintf(stderr, "Warning: glob() failed for pattern '%s' (error %d). Treating pattern as literal argument.\n", current_arg, ret);
      if (expanded.argc >= MAX_TOKENS) {
         fprintf(stderr, "Warning: Maximum token count (%d) reached during expansion (glob error fallback).\n", MAX_TOKENS);
         goto max_tokens_reached; // Use goto for cleanup
      }
      expanded.argv[expanded.argc] = strdup(current_arg); // Add the original pattern
      if (!expanded.argv[expanded.argc]) {
        perror("strdup failed for fallback literal argument");
        globfree(&glob_result); // Free glob resources
        free_args(&expanded);   // Free partially built list
        free_args(args);        // Free original list
        args->argc = 0;
        args->argv = NULL;
        return;
      }
      expanded.argc++;
    }

  max_tokens_reached: // Label for breaking out if MAX_TOKENS hit
    globfree(&glob_result); // Free resources allocated by glob() in all cases where it was called

    // If we broke early due to token limit, stop processing outer loop
    if (expanded.argc >= MAX_TOKENS && i < args->argc - 1) {
        break;
    }

  } // End of loop through original args

  // We are done processing original arguments. Free the original list.
  free_args(args);

  // Replace the original ArgList structure's content with the expanded one.
  *args = expanded;
}

// get_args remains the same as in the original code
/**
 * @brief Parses the command-line string and returns the arguments.
 */
ErrorCode get_args(int *argc, char ***argv, const char *cmdline) {
  if (!argc || !argv || !cmdline) {
    return ERR_INVALID_ARGUMENT; // Assuming this error code exists
  }

  // Initialize output parameters in case of early failure
  *argc = 0;
  *argv = NULL;

  ArgList args = split_args(cmdline);
  if (!args.argv) {
      return ERR_MEMORY_ALLOCATION; // Assuming this error code exists
  }

  // If split_args returned successfully but with 0 args, no need to expand
  if (args.argc > 0) {
      expand_wildcards(&args);
      // Check if expand_wildcards failed (indicated by args.argv being NULL)
      if (!args.argv && args.argc == 0) {
           return ERR_MEMORY_ALLOCATION;
      }
  }


  // If everything succeeded, assign the results
  *argc = args.argc;
  *argv = args.argv;

  return ERR_SUCCESS;
}
