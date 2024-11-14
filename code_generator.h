/*
IFJ project

@brief Header file for code generator

@author Vojtěch Gajdušek - xgajduv00
*/

#ifndef _CODE_GENERATOR_H
#define _CODE_GENERATOR_H

#include "ast.h"
#include "dynamic_string.h"
#include <stdio.h>

#define BUILT_IN_FUNCTION_READ                                                                     \
    "# Built-in function read\n"                                                                   \
    "LABEL $IFJ24_read\n"                                                                          \
    "PUSHFRAME\n"                                                                                  \
    "DEFVAR LF@%%retval\n"                                                                         \
    "JUMPIFEQ $read_int LF@type string@int\n"                                                      \
    "JUMPIFEQ $read_float LF@type string@float\n"                                                  \
    "JUMPIFEQ $read_string LF@type string@string\n"                                                \
    "JUMPIFEQ $read_bool LF@type string@bool\n"                                                    \
    "LABEL $read_int\n"                                                                            \
    "READ LF@%%retval int\n"                                                                       \
    "JUMP $read_end\n"                                                                             \
    "LABEL $read_float\n"                                                                          \
    "READ LF@%%retval float\n"                                                                     \
    "JUMP $read_end\n"                                                                             \
    "LABEL $read_string\n"                                                                         \
    "READ LF@%%retval string\n"                                                                    \
    "JUMP $read_end\n"                                                                             \
    "LABEL $read_bool\n"                                                                           \
    "READ LF@%%retval bool\n"                                                                      \
    "JUMP $read_end\n"                                                                             \
    "LABEL $read_end\n"                                                                            \
    "POPFRAME\n"                                                                                   \
    "RETURN\n"

#define BUILT_IN_FUNCTION_WRITE                                                                    \
    "# Built-in function write\n"                                                                  \
    "LABEL $IFJ24_write\n"                                                                         \
    "PUSHFRAME\n"                                                                                  \
    "DEFVAR LF@type\n"                                                                             \
    "DEFVAR LF@value\n"                                                                            \
    "MOVE LF@value TF@0\n"                                                                         \
    "TYPE LF@type LF@value\n"                                                                      \
    "JUMPIFEQ $write_int LF@type string@int\n"                                                     \
    "JUMPIFEQ $write_float LF@type string@float\n"                                                 \
    "JUMPIFEQ $write_string LF@type string@string\n"                                               \
    "JUMPIFEQ $write_bool LF@type string@bool\n"                                                   \
    "JUMPIFEQ $write_null LF@type string@nil\n"                                                    \
    "LABEL $write_int\n"                                                                           \
    "WRITE LF@value\n"                                                                             \
    "JUMP $write_end\n"                                                                            \
    "LABEL $write_float\n"                                                                         \
    "WRITE LF@value\n"                                                                             \
    "JUMP $write_end\n"                                                                            \
    "LABEL $write_string\n"                                                                        \
    "WRITE LF@value\n"                                                                             \
    "JUMP $write_end\n"                                                                            \
    "LABEL $write_bool\n"                                                                          \
    "JUMPIFEQ $write_bool_true LF@value bool@true\n"                                               \
    "WRITE string@false\n"                                                                         \
    "JUMP $write_end\n"                                                                            \
    "LABEL $write_bool_true\n"                                                                     \
    "WRITE string@true\n"                                                                          \
    "JUMP $write_end\n"                                                                            \
    "LABEL $write_null\n"                                                                          \
    "WRITE string@null\n"                                                                          \
    "JUMP $write_end\n"                                                                            \
    "LABEL $write_end\n"                                                                           \
    "POPFRAME\n"                                                                                   \
    "RETURN\n"

#define BUILT_IN_FUNCTION_WRITE                                                                    \
    "# Built-in function write\n"                                                                  \
    "LABEL $IFJ24_write\n"                                                                         \
    "PUSHFRAME\n"                                                                                  \
    "DEFVAR LF@type\n"                                                                             \
    "TYPE LF@type LF@value\n"                                                                      \
    "JUMPIFEQ $write_int LF@type string@int\n"                                                     \
    "JUMPIFEQ $write_float LF@type string@float\n"                                                 \
    "JUMPIFEQ $write_string LF@type string@string\n"                                               \
    "JUMPIFEQ $write_bool LF@type string@bool\n"                                                   \
    "JUMPIFEQ $write_null LF@type string@nil\n"                                                    \
    "LABEL $write_int\n"                                                                           \
    "WRITE LF@value\n"                                                                             \
    "JUMP $write_end\n"                                                                            \
    "LABEL $write_float\n"                                                                         \
    "WRITE LF@value\n"                                                                             \
    "JUMP $write_end\n"                                                                            \
    "LABEL $write_string\n"                                                                        \
    "WRITE LF@value\n"                                                                             \
    "JUMP $write_end\n"                                                                            \
    "LABEL $write_bool\n"                                                                          \
    "JUMPIFEQ $write_bool_true LF@value bool@true\n"                                               \
    "WRITE string@false\n"                                                                         \
    "JUMP $write_end\n"                                                                            \
    "LABEL $write_bool_true\n"                                                                     \
    "WRITE string@true\n"                                                                          \
    "JUMP $write_end\n"                                                                            \
    "LABEL $write_null\n"                                                                          \
    "WRITE string@null\n"                                                                          \
    "JUMP $write_end\n"                                                                            \
    "LABEL $write_end\n"                                                                           \
    "POPFRAME\n"                                                                                   \
    "RETURN\n"

#define BUILT_IN_FUNCTION_I2F                                                                      \
    "# Built-in function i2f\n"                                                                    \
    "LABEL $IFJ24_i2f\n"                                                                           \
    "PUSHFRAME\n"                                                                                  \
    "DEFVAR LF@%%retval\n"                                                                         \
    "INT2FLOAT LF@%%retval LF@input\n"                                                             \
    "POPFRAME\n"                                                                                   \
    "RETURN\n"

#define BUILT_IN_FUNCTION_F2I                                                                      \
    "# Built-in function f2i\n"                                                                    \
    "LABEL $IFJ24_f2i\n"                                                                           \
    "PUSHFRAME\n"                                                                                  \
    "DEFVAR LF@%%retval\n"                                                                         \
    "FLOAT2INT LF@%%retval LF@input\n"                                                             \
    "POPFRAME\n"                                                                                   \
    "RETURN\n"

#define BUILT_IN_FUNCTION_LENGTH                                                                   \
    "# Built-in function length\n"                                                                 \
    "LABEL $IFJ24_length\n"                                                                        \
    "PUSHFRAME\n"                                                                                  \
    "DEFVAR LF@%%retval\n"                                                                         \
    "STRLEN LF@%%retval LF@s\n"                                                                    \
    "POPFRAME\n"                                                                                   \
    "RETURN\n"

#define BUILT_IN_FUNCTION_CONCAT                                                                   \
    "# Built-in function concat\n"                                                                 \
    "LABEL $IFJ24_concat\n"                                                                        \
    "PUSHFRAME\n"                                                                                  \
    "DEFVAR LF@%%retval\n"                                                                         \
    "CONCAT LF@%%retval LF@s1 LF@s2\n"                                                             \
    "POPFRAME\n"                                                                                   \
    "RETURN\n"

#define BUILT_IN_FUNCTION_SUBSTRING                                                                \
    "# Built-in function substring\n"                                                              \
    "LABEL $IFJ24_substring\n"                                                                     \
    "PUSHFRAME\n"                                                                                  \
    "DEFVAR LF@%%retval\n"                                                                         \
    "DEFVAR LF@len\n"                                                                              \
    "DEFVAR LF@current_char\n"                                                                     \
    "DEFVAR LF@substring\n"                                                                        \
    "DEFVAR LF@counter\n"                                                                          \
    "DEFVAR LF@start_index\n"                                                                      \
    "DEFVAR LF@end_index\n"                                                                        \
    "DEFVAR LF@condition\n"                                                                        \
    "MOVE LF@%%retval nil@nil\n"                                                                   \
    "MOVE LF@substring string@\"\"\n"                                                              \
    "MOVE LF@start_index LF@i\n"                                                                   \
    "MOVE LF@end_index LF@j\n"                                                                     \
    "STRLEN LF@len LF@s\n"                                                                         \
    "LT LF@condition LF@start_index int@0\n"                                                       \
    "JUMPIFEQ $substring_error LF@condition bool@true\n"                                           \
    "LT LF@condition LF@end_index int@0\n"                                                         \
    "JUMPIFEQ $substring_error LF@condition bool@true\n"                                           \
    "GE LF@condition LF@start_index LF@len\n"                                                      \
    "JUMPIFEQ $substring_error LF@condition bool@true\n"                                           \
    "GT LF@condition LF@end_index LF@len\n"                                                        \
    "JUMPIFEQ $substring_error LF@condition bool@true\n"                                           \
    "LT LF@condition LF@end_index LF@start_index\n"                                                \
    "JUMPIFEQ $substring_error LF@condition bool@true\n"                                           \
    "MOVE LF@counter LF@start_index\n"                                                             \
    "LABEL $substring_loop\n"                                                                      \
    "GT LF@condition LF@counter LF@end_index\n"                                                    \
    "JUMPIFEQ $substring_end LF@condition bool@true\n"                                             \
    "GETCHAR LF@current_char LF@s LF@counter\n"                                                    \
    "CONCAT LF@substring LF@substring LF@current_char\n"                                           \
    "ADD LF@counter LF@counter int@1\n"                                                            \
    "JUMP $substring_loop\n"                                                                       \
    "LABEL $substring_end\n"                                                                       \
    "MOVE LF@%%retval LF@substring\n"                                                              \
    "POPFRAME\n"                                                                                   \
    "RETURN\n"                                                                                     \
    "LABEL $substring_error\n"                                                                     \
    "MOVE LF@%%retval nil@nil\n"                                                                   \
    "JUMP $substring_end\n"

#define BUILT_IN_FUNCTION_STRCMP                                                                   \
    "# Built-in function strcmp\n"                                                                 \
    "LABEL $IFJ24_strcmp\n"                                                                        \
    "PUSHFRAME\n"                                                                                  \
    "DEFVAR LF@%%retval\n"                                                                         \
    "DEFVAR LF@less\n"                                                                             \
    "DEFVAR LF@greater\n"                                                                          \
    "LT LF@less LF@s1 LF@s2\n"                                                                     \
    "JUMPIFEQ $strcmp_less LF@less bool@true\n"                                                    \
    "GT LF@greater LF@s1 LF@s2\n"                                                                  \
    "JUMPIFEQ $strcmp_greater LF@greater bool@true\n"                                              \
    "MOVE LF@%%retval int@0\n"                                                                     \
    "JUMP $strcmp_end\n"                                                                           \
    "LABEL $strcmp_less\n"                                                                         \
    "MOVE LF@%%retval int@-1\n"                                                                    \
    "JUMP $strcmp_end\n"                                                                           \
    "LABEL $strcmp_greater\n"                                                                      \
    "MOVE LF@%%retval int@1\n"                                                                     \
    "LABEL $strcmp_end\n"                                                                          \
    "POPFRAME\n"                                                                                   \
    "RETURN\n"

#define BUILT_IN_FUNCTION_ORD                                                                      \
    "# Built-in function ord\n"                                                                    \
    "LABEL $IFJ24_ord\n"                                                                           \
    "PUSHFRAME\n"                                                                                  \
    "DEFVAR LF@%%retval\n"                                                                         \
    "DEFVAR LF@len\n"                                                                              \
    "STRLEN LF@len LF@s\n"                                                                         \
    "LT LF@%%retval LF@i LF@len\n"                                                                 \
    "JUMPIFNEQ $ord_error LF@%%retval bool@true\n"                                                 \
    "GETCHAR LF@%%retval LF@s LF@i\n"                                                              \
    "CHAR2INT LF@%%retval LF@%%retval\n"                                                           \
    "JUMP $ord_end\n"                                                                              \
    "LABEL $ord_error\n"                                                                           \
    "MOVE LF@%%retval int@0\n"                                                                     \
    "LABEL $ord_end\n"                                                                             \
    "POPFRAME\n"                                                                                   \
    "RETURN\n"

#define BUILT_IN_FUNCTION_CHR                                                                      \
    "# Built-in function chr\n"                                                                    \
    "LABEL $IFJ24_chr\n"                                                                           \
    "PUSHFRAME\n"                                                                                  \
    "DEFVAR LF@%%retval\n"                                                                         \
    "DEFVAR LF@num\n"                                                                              \
    "MOVE LF@num LF@i\n"                                                                           \
    "JUMPIFLT $chr_error LF@num int@0\n"                                                           \
    "JUMPIFGT $chr_error LF@num int@255\n"                                                         \
    "INT2CHAR LF@%%retval LF@num\n"                                                                \
    "JUMP $chr_end\n"                                                                              \
    "LABEL $chr_error\n"                                                                           \
    "MOVE LF@%%retval nil@nil\n"                                                                   \
    "LABEL $chr_end\n"                                                                             \
    "POPFRAME\n"                                                                                   \
    "RETURN\n"

/**
 * @brief Generates code from the given abstract syntax tree (AST)
 *
 * @param ast A pointer to the abstract syntax tree representing the source code.
 * @return int Returns 0 on success, or a non-zero error code on failure.
 */
int generateCode(FILE *outputFile, AST *ast);

/**
 * @brief Generates the header for the code.
 *
 * This function writes the necessary header information.
 *
 * @return int Returns 0 on success, or a non-zero error code on failure.
 */
int generateCodeHeader();

/**
 * @brief Flushes the contents of the code generator to the specified output file.
 *
 * This function writes any buffered data to the given output file, ensuring that
 * all generated code is properly saved.
 *
 * @param outputFile A pointer to the FILE object where the generated code will be written.
 */
void codeGeneratorFlush(FILE *outputFile);

/**
 * @brief Generates the start of the main function.
 *
 * This function is responsible for generating the start of the main function,
 * setting up the initial frame and performing any necessary initialization steps.
 *
 * @return int Returns 0 on success, or a non-zero error code on failure.
 */
int mainStart();

/**
 * @brief Generates the end of the main function.
 *
 * This function is responsible for generating the end of the main function,
 * performing any necessary cleanup and finalization steps.
 *
 * @return int Returns 0 on success, or a non-zero error code on failure.
 */
int mainEnd();

/**
 * @brief Generates code for a given expression represented by an AST node.
 *
 * This function takes an abstract syntax tree (AST) node representing an expression
 * and generates the corresponding code for it.
 *
 * @param node A pointer to the AST node representing the expression.
 * @return int Returns 0 on success, or a non-zero error code on failure.
 */
int generateExpression(ASTNode *node);

/**
 * @brief Generates code for a given function call, creating a new frame and pushing it onto the
 * stack.
 *
 * @param node A pointer to the AST node representing the function call.
 * @return int Returns 0 on success, or a non-zero error code on failure.
 */
int functionStart(char *functionName);

/**
 * @brief Generates code for the end of a function, popping the frame from the stack.
 *
 * @param functionName The name of the function to end.
 * @return int Returns 0 on success, or a non-zero error code on failure.
 */
int functionEnd(char *functionName);

/**
 * @brief Generates the built-in IFJ functions.
 *
 * @return int Returns 0 on success, or a non-zero error code on failure.
 */
int generateBuiltInFunctions();

#endif