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