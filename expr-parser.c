/*
IFJ Project

@brief Implementation file for the expression parser

@author Matúš Csirik - xcsirim00

*/

#include "expr-parser.h"
#include "enums.h"
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

#define EXPR TOKEN_TYPE_AT // Need an expression token type (replaces '@')

int isOperator(Token *token);
int isOperand(Token *token);
int isDelimiter(Token *token); // is it a token that marks the end of an expression?
int precedence(Token *token);
int isLeftAssociative(Token *token);

/**
 * @brief Fills the input stack with tokens up to a delimiter token.
 *
 * @param stack A pointer to the stack to be filled.
 * @param delimiterToken A pointer that stores the token that acts as a delimiter.
 * @return A pointer to the filled stack.
 */
Stack *fillInputStack(Stack *stack, Token *delimiterToken);

/**
 * @brief Checks if the given stack is reducible.
 *
 * @param stack A pointer to the stack to be checked.
 * @return An integer indicating whether the stack is reducible (1 or 10 if reducible, 10 means its
 * the EXPR_ID rule, 0 otherwise).
 */
int isReducible(Stack *stack);

int isOperator(Token *token) {
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

int isOperand(Token *token) {
    return token->type == TOKEN_TYPE_IDENTIFIER       // id
           || token->type == TOKEN_TYPE_INTEGER_VALUE // i32
           || token->type == TOKEN_TYPE_DOUBLE_VALUE; // f64
}

int isDelimiter(Token *token) {
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

    if (getStackLength(stack) < 3) {
        return 0;
    }

    Token *topToken = top(stack);
    Token *secondToken = stack->top->next->tokenPtr;
    Token *thirdToken = stack->top->next->next->tokenPtr;

    if (topToken->type == EXPR && is_operator(secondToken) == EXPR && thirdToken->type == EXPR) {
        return 1;
    }
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