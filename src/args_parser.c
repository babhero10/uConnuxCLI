#include "../include/args_parser.h"
#include <ctype.h>
#include <glob.h>
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
    return ERR_NULL_POINTER;
  }
  if (args == NULL || args->argv == NULL)
    return ERR_NULL_POINTER;
  for (int i = 0; i < args->argc; ++i) {
    free(args->argv[i]);
  }
  free(args->argv);
  args->argc = 0;
  args->argv = NULL;
  return ERR_SUCCESS;
}

/**
 * @brief Splits a command-line string into individual arguments, handling
 * quotes.
 */
static ArgList split_args(const char *input) {

  ArgList result = {0, NULL};

  if (input == NULL) {
    return result;
  }

  result.argv = malloc(MAX_TOKENS * sizeof(char *));
  if (!result.argv) {
    return result; // Return empty list on failure
  }

  const char *p = input;
  while (*p) {
    while (isspace((unsigned char)*p))
      p++; // Skip whitespace
    if (*p == '\0')
      break;

    char token[MAX_TOKEN_LEN] = {0};
    int pos = 0;
    int quoted = 0;

    if (*p == '\"' || *p == '\'') {
      quoted = *p++;
    }

    while (*p && (quoted ? (*p != quoted) : !isspace((unsigned char)*p))) {
      if (*p == '\\' && *(p + 1)) {
        p++; // Skip escape character
      }
      if (pos < MAX_TOKEN_LEN - 1) {
        token[pos++] = *p++;
      } else {
        p++;
      }
    }

    if (quoted && *p == quoted)
      p++; // Skip closing quote
    token[pos] = '\0';

    result.argv[result.argc++] = strdup(token);
    if (!result.argv[result.argc - 1]) {
      free_args(&result);
      result.argc = 0;
      return result;
    }

    if (result.argc >= MAX_TOKENS)
      break;
  }
  return result;
}

/**
 * @brief Expands wildcard patterns in arguments to matching file names.
 */
static void expand_wildcards(ArgList *args) {
  ArgList expanded = {0, malloc(MAX_TOKENS * sizeof(char *))};
  if (!expanded.argv) {
    free_args(args);
    args->argc = 0;
    args->argv = NULL;
    return;
  }

  for (int i = 0; i < args->argc; ++i) {
    glob_t glob_result;
    int ret = glob(args->argv[i], 0, NULL, &glob_result);

    if (ret == 0) {
      for (size_t j = 0; j < glob_result.gl_pathc; ++j) {
        expanded.argv[expanded.argc++] = strdup(glob_result.gl_pathv[j]);
        if (!expanded.argv[expanded.argc - 1]) {
          free_args(&expanded);
          globfree(&glob_result);
          free_args(args);
          args->argc = 0;
          args->argv = NULL;
          return;
        }
      }
      globfree(&glob_result);
    } else {
      expanded.argv[expanded.argc++] = strdup(args->argv[i]);
      if (!expanded.argv[expanded.argc - 1]) {
        free_args(&expanded);
        free_args(args);
        args->argc = 0;
        args->argv = NULL;
        return;
      }
    }
  }

  free_args(args);
  *args = expanded;
}

/**
 * @brief Parses the command-line string and returns the arguments.
 */
ErrorCode get_args(int *argc, char ***argv, const char *cmdline) {
  if (!argc || !argv || !cmdline) {
    return ERR_INVALID_ARGUMENT;
  }

  ArgList args = split_args(cmdline);
  if (!args.argv) {
    return ERR_MEMORY_ALLOCATION;
  }

  expand_wildcards(&args);
  if (!args.argv) {
    return ERR_MEMORY_ALLOCATION; // expand_wildcards sets args to empty on
  }

  *argc = args.argc;
  *argv = args.argv;
  return ERR_SUCCESS;
}
