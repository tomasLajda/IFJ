/*
IFJ project

@brief Header file for code generator

@author Vojtěch Gajdušek - xgajduv00
*/

#ifndef _CODE_GENERATOR_H
#define _CODE_GENERATOR_H

#include "ast.h"
#include <stdio.h>

/**
 * @brief Generates code from the given abstract syntax tree (AST) and writes it to the specified
 * output file.
 *
 * @param outputFile A pointer to the file where the generated code will be written.
 * @param ast A pointer to the abstract syntax tree representing the source code.
 * @return int Returns 0 on success, or a non-zero error code on failure.
 */
int generateCode(FILE *outputFile, AST *ast);

/**
 * @brief Generates the header for the output file.
 *
 * This function writes the necessary header information to the provided output file.
 *
 * @param outputFile A pointer to the FILE object where the header will be written.
 * @return int Returns 0 on success, or a non-zero error code on failure.
 */
int generateFileHeader(FILE *outputFile);

#endif