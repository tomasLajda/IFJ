/*
IFJ Project

@brief Header file to define error codes

@author Vojtěch Gajdušek - xgajduv00
@author Matúš Csirik - xcsirim00

*/

// Macro for handling errors
#ifdef TEST_MODE
#define HANDLE_ERROR(msg, code)                                                                    \
    do {                                                                                           \
        fprintf(stderr, "%s\n", msg);                                                              \
        return;                                                                                    \
    } while (0)
#else
#define HANDLE_ERROR(msg, code)                                                                    \
    do {                                                                                           \
        fprintf(stderr, "%s\n", msg);                                                              \
        exit(code);                                                                                \
    } while (0)
#endif

#ifndef _ERROR_H
#define _ERROR_H

#define TOKEN_OK 0 // Token is OK
#define LEXICAL_ERROR                                                                              \
    1 // Error in the program during lexical analysis (incorrect structure of the current lexeme)

#define SYNTAX_ERROR                                                                               \
    2 // Error in the program during syntactic analysis (incorrect program syntax, missing header,
      // etc.)

#define UNDEFINED_ERROR 3 // Semantic error in the program – undefined function or variable.

#define PARAMETER_ERROR                                                                            \
    4 // Semantic error in the program – incorrect number/type of parameters in a function call;
      // incorrect type or disallowed discard of function return value.

#define REDEFINITION_ERROR                                                                         \
    5 // Semantic error in the program – redefinition of a variable or function; assignment to an
      // unmodifiable variable.

#define RETURN_EXPRESSION_ERROR                                                                    \
    6 // Semantic error in the program – missing or extra expression in the return statement of a
      // function.

#define TYPE_COMPATIBILITY_ERROR                                                                   \
    7 // Semantic error of type compatibility in arithmetic, string, and relational expressions;
      // incompatible expression type (e.g., during assignment).

#define TYPE_INFERENCE_ERROR                                                                       \
    8 // Semantic error in type inference – variable type is not specified and cannot be inferred
      // from the expression used.

#define UNUSED_VARIABLE_ERROR                                                                      \
    9 // Semantic error – unused variable within its scope; modifiable variable with no possibility
      // of change after its initialization.

#define OTHER_SEMANTIC_ERROR 10 // Other semantic errors.

#define INTERNAL_ERROR                                                                             \
    99 // Internal compiler error, i.e., not influenced by the input program (e.g., memory
       // allocation error, etc.).

#endif