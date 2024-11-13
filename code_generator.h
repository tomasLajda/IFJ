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

#endif