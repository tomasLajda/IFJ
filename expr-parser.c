/*
IFJ Project

@brief Implementation file for the expression parser

@author Matúš Csirik - xcsirim00

*/

#include "expr-parser.h"
#include <stdio.h>
#include <stdlib.h>

int is_operator(Token *token);
int is_operand(Token *token);
int is_delimiter(Token *token);
int precedence(Token *token);

int is_operator(Token *token) {
    return token->type == TOKEN_TYPE_PLUS ||  // +
           token->type == TOKEN_TYPE_MINUS || // -
           token->type == TOKEN_TYPE_MUL ||   // /
           token->type == TOKEN_TYPE_DIV;     // *
}

int is_operand(Token *token) {
    return token->type == TOKEN_TYPE_IDENTIFIER       // id
           || token->type == TOKEN_TYPE_INTEGER_VALUE // i32
           || token->type == TOKEN_TYPE_DOUBLE_VALUE; // f64
}

int is_delimiter(Token *token) {
    return token->type == TOKEN_TYPE_EOL ||       // EOL
           token->type == TOKEN_TYPE_EOF ||       // EOF
           token->type == TOKEN_TYPE_SEMICOLON || // ;
           token->type == TOKEN_TYPE_RIGHT_BR;    // )
}

int precedence(Token *token) {
    switch (token->type) {
    case TOKEN_TYPE_EQ:  // ==
    case TOKEN_TYPE_NEQ: // !=
    case TOKEN_TYPE_LTH: // >
    case TOKEN_TYPE_LEQ: // >=
    case TOKEN_TYPE_GTH: // <
    case TOKEN_TYPE_GEQ: // <=
        return 1;
    case TOKEN_TYPE_PLUS:  // +
    case TOKEN_TYPE_MINUS: // -
        return 2;
    case TOKEN_TYPE_MUL: // *
    case TOKEN_TYPE_DIV: // /
        return 3;
    default:
        return 0;
    }
}

int parseExpression(AST *exprAST, Token *token) {}