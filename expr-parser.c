/*
IFJ Project

@brief Implementation file for the expression parser

@author Matúš Csirik - xcsirim00

*/

#include "expr-parser.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    EXPR_ADD, // Expr -> Expr + Expr
    EXPR_SUB, // Expr -> Expr - Expr
    EXPR_EQ,  // Expr -> Expr == Expr
    EXPR_NEQ, // Expr -> Expr != Expr
    EXPR_LTH, // Expr -> Expr < Expr
    EXPR_GTH, // Expr -> Expr > Expr
    EXPR_LEQ, // Expr -> Expr <= Expr
    EXPR_GEQ, // Expr -> Expr >= Expr
    EXPR_MUL, // Expr -> Expr * Expr
    EXPR_DIV, // Expr -> Expr / Expr
    EXPR_ID,  // id   -> Expr
} ExpressionRule;

int parseExpression(AST *exprAST, Token *token);
int is_operator(Token *token);
int is_operand(Token *token);
int is_delimiter(Token *token); // is it a token that marks the end of an expression?
int precedence(Token *token);
int isLeftAssociative(Token *token);

int is_operator(Token *token) {
    return token->type == TOKEN_TYPE_PLUS ||  // +
           token->type == TOKEN_TYPE_MINUS || // -
           token->type == TOKEN_TYPE_MUL ||   // /
           token->type == TOKEN_TYPE_DIV ||   // *
           token->type == TOKEN_TYPE_EQ ||    // ==
           token->type == TOKEN_TYPE_NEQ ||   // !=
           token->type == TOKEN_TYPE_LTH ||   // <
           token->type == TOKEN_TYPE_LEQ ||   // <=
           token->type == TOKEN_TYPE_GTH ||   // >
           token->type == TOKEN_TYPE_GEQ;     // >=
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

int isLeftAssociative(Token *token) {
    switch (token->type) {
    case TOKEN_TYPE_PLUS:  // +
    case TOKEN_TYPE_MINUS: // -
    case TOKEN_TYPE_MUL:   // *
    case TOKEN_TYPE_DIV:   // /
        return 1;
    default:
        return 0;
    }
}

Stack *fillInputStack(Stack *stack, Token *delimiterToken) {
    Token *token = getNextToken(token);
    while (is_operand(token) || is_operator(token)) {
        push(stack, token);
        token = getNextToken(token);
    }
    delimiterToken = token;
    if (!is_delimiter(token)) {
        return NULL; // syntax error
    }
    return stack;
}

int isReducible(Stack *stack) {
    if (stack->top == NULL) {
        return 0;
    }
    if (stack->top->tokenPtr->type == TOKEN_TYPE_IDENTIFIER ||
        stack->top->tokenPtr->type == TOKEN_TYPE_INTEGER_VALUE ||
        stack->top->tokenPtr->type == TOKEN_TYPE_DOUBLE_VALUE) {
        return EXPR_ID; // id -> Expr
    }

    if ()

        return 0;
}

int parseExpression(AST *exprAST, Token *token) {

    Stack *stack = (Stack *)malloc(sizeof(Stack));
    if (stack == NULL) {
        return 2;
    }
    initStack(stack);

    Stack *input = (Stack *)malloc(sizeof(Stack));
    if (input == NULL) {
        return 2;
    }
    initStack(input);

    Token *delimiterToken = NULL;
    if (fillInputStack(input, delimiterToken) == NULL) {
        return 1; // Token doesn't belong in the expression
    }

    return 0;
}