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
 * @brief Parses an expression and constructs its Abstract Syntax Tree (AST).
 *
 * This function implements a shift-reduce parsing algorithm to analyze the given
 * tokens and build the corresponding AST. It utilizes a parsing and input stacks,
 * reduction rules, and checks the correctness of the syntax of the expression.
 *
 * @param exprAST Pointer to the AST structure to be populated.
 * @param token Pointer to the initial token to begin parsing.
 * @return
 *   - 0 on successful parsing.
 *   - SYNTAX_ERROR (2) if a syntax error is encountered.
 *   - INTERNAL_ERROR (99) for internal failures (e.g., memory allocation issues).
 */
int parseExpression(AST *exprAST, Token *token);

#endif // EXPR_PARSER_H
