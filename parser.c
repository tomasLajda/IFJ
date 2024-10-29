/*
 * IFJ Project
 * @brief Implementation file for parser
 *
 * @author Martin Valapka - xvalapm00
 */

#include "parser.h"
#include <string.h>

Token* currentToken = NULL;
SymbolTable* symbolTable = NULL;
AST* ast = NULL;