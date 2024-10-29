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

#define EXPR TOKEN_TYPE_AT    // Need an expression token type (replaces '@')
#define DOLLAR TOKEN_TYPE_DOT // Need a dollar token type (replaces '.')

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
 * @return An integer indicating whether the stack is reducible (0) or not (1).
 */
int isReducible(Stack *stack);

/**
 * @brief Chooses the appropriate reduction rule based on the current state of the stack. Use only
 * if isReducible() returns 0.
 *
 * @param stack A pointer to the stack used in the expression parser.
 * @return An integer representing the chosen reduction rule or -1 if no rule is applicable.
 */
int chooseReduceRule(Stack *stack);

int isOperator(Token *token);
int isOperand(Token *token);
int isDelimiter(Token *token); // is it a token that marks the end of an expression?
int precedence(Token *token);
int isLeftAssociative(Token *token);

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
    Stack tempStack;
    initStack(&tempStack);

    Token *token = getNextToken(token);
    while (is_operand(token) || is_operator(token)) {
        push(stack, token);
        token = getNextToken(token);
    }
    delimiterToken = token;
    if (!is_delimiter(token)) {
        return NULL; // syntax error
    }

    while (!isEmpty(&tempStack)) {
        push(stack, topToken(&tempStack));
        pop(&tempStack);
    }

    return stack;
}

int isReducible(Stack *stack) {
    if (stack->top == NULL) {
        return 1;
    }
    if (isOperand(topToken(stack))) {
        return 0;
    }

    if (getStackLength(stack) < 3) {
        return 1;
    }

    Token *leftToken = stack->top->tokenPtr;
    Token *middleToken = stack->top->next->tokenPtr;
    Token *rightToken = stack->top->next->next->tokenPtr;

    if (isOperator(leftToken->type) && middleToken->type == EXPR && rightToken->type == EXPR) {
        return 0;
    }
}

int chooseReduceRule(Stack *stack) {
    if (isOperand(topToken(stack))) {
        return EXPR_ID;
    }
    switch (topToken(stack)->type) {
    case TOKEN_TYPE_PLUS:
        return EXPR_ADD; // Expr -> Expr + Expr
    case TOKEN_TYPE_MINUS:
        return EXPR_SUB; // Expr -> Expr - Expr
    case TOKEN_TYPE_EQ:
        return EXPR_EQ; // Expr -> Expr == Expr
    case TOKEN_TYPE_NEQ:
        return EXPR_NEQ; // Expr -> Expr != Expr
    case TOKEN_TYPE_LTH:
        return EXPR_LTH; // Expr -> Expr < Expr
    case TOKEN_TYPE_GTH:
        return EXPR_GTH; // Expr -> Expr > Expr
    case TOKEN_TYPE_LEQ:
        return EXPR_LEQ; // Expr -> Expr <= Expr
    case TOKEN_TYPE_GEQ:
        return EXPR_GEQ; // Expr -> Expr >= Expr
    case TOKEN_TYPE_MUL:
        return EXPR_MUL; // Expr -> Expr * Expr
    case TOKEN_TYPE_DIV:
        return EXPR_DIV; // Expr -> Expr / Expr
    default:
        return -1; // No rule to apply
    }
}

Token *createToken(TokenType type) {
    Token *token = (Token *)malloc(sizeof(Token));
    token->type = type;
}

int parseExpression(AST *exprAST, Token *token) {

    // Initialize the stack
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    if (stack == NULL) {
        return 2;
    }
    initStack(stack);
    push(stack, createToken(DOLLAR)); // Push the dollar token onto the stack

    // Initialize the input stack
    Stack *input = (Stack *)malloc(sizeof(Stack));
    if (input == NULL) {
        return 2;
    }
    initStack(input);
    if (fillInputStack(input, token) == NULL) {
        return 1; // Token doesn't belong in the expression
    }

    // Initialize the AST
    exprAST = initAST();

    // Main loop
    Token *currentToken;
    while (!isEmpty(input)) {
        currentToken = topToken(input);
        pop(input);

        // Shift
        push(stack, currentToken);
        if (isReducible(stack)) {
            switch (chooseReduceRule(stack)) {
            case EXPR_ADD: // Expr -> Expr + Expr
                // Reduce
                break;
            case EXPR_SUB: // Expr -> Expr - Expr
                // Reduce
                break;
            case EXPR_EQ: // Expr -> Expr == Expr
                // Reduce
                break;
            case EXPR_NEQ: // Expr -> Expr != Expr
                // Reduce
                break;
            case EXPR_LTH: // Expr -> Expr < Expr
                // Reduce
                break;
            case EXPR_GTH: // Expr -> Expr > Expr
                // Reduce
                break;
            case EXPR_LEQ: // Expr -> Expr <= Expr
                // Reduce
                break;
            case EXPR_GEQ: // Expr -> Expr >= Expr
                // Reduce
                break;
            case EXPR_MUL: // Expr -> Expr * Expr
                // Reduce
                break;
            case EXPR_DIV: // Expr -> Expr / Expr
                // Reduce
                break;
            case EXPR_ID: // id -> Expr
                // Reduce
                break;
            default:
                return 1; // Syntax error
            }
        }
    }

    return 0;
}