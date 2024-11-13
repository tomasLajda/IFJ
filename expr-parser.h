/*
IFJ Project

@brief Header file for the expression parser

@author Matúš Csirik - xcsirim00

*/

#ifndef EXPR_PARSER_H
#define EXPR_PARSER_H

#include "ast.h"
#include "enums.h"
#include "scanner.h"
#include "stack.h"

/**
 * @brief Begins parsing the expression token by token until the end of the expression.
 * Returns a pointer to the AST of the expression through the 'exprAST' parameter.
 * Returns a pointer to the token that ended the expression through the 'token' parameter.
 *  *
 * @param exprAST Pointer to the AST for the expression
 * @param token Pointer to the token that ended the expression (either ';' or ')')
 *
 * @return Returns 0 if the expression was parsed successfully, 1 if a syntax error was encountered,
 * 2 if an internal parsing error was encountered
 */
int parseExpression(AST *exprAST, Token *token);

#endif // EXPR_PARSER_H
