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
 * @param ast A pointer to the abstract syntax tree representing the source code.
 * @return int Returns 0 on success, or a non-zero error code on failure.
 */
int generateCode(AST *ast);

/**
 * @brief Generates the header for the output file.
 *
 * This function writes the necessary header information to the provided output file.
 *
 * @return int Returns 0 on success, or a non-zero error code on failure.
 */
int generateFileHeader();

#endif