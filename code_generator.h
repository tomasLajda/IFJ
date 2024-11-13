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
 * @brief Generates the main function for the code.
 *
 * This function is responsible for generating the main function
 * in the code. It sets up the necessary structure and logic
 * for the main entry point of the program.
 *
 * @return int Returns 0 on success, or a non-zero error code on failure.
 */
int generateMain();

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

#endif