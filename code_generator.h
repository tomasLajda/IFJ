/*
IFJ project

@brief Header file for code generator

@author Vojtěch Gajdušek - xgajduv00
*/

#ifndef _CODE_GENERATOR_H
#define _CODE_GENERATOR_H

#include "ast.h"
#include <stdio.h>

int generateCode(FILE *outputFile, AST *ast);

#endif