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

#define EXPR TOKEN_TYPE_AT    // Need an expression token type (replaces '@')
#define DOLLAR TOKEN_TYPE_DOT // Need a dollar token type (replaces '.')

// Returns 1 if the token is an operator, 0 otherwise
int isOperator(Token *token) {
    // printf("is token type: %d an operator?\n", token->type);
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

// Returns 1 if the token is an operand, 0 otherwise
int isOperand(Token *token) { // Returns 1 if the token is an operand, 0 otherwise
    // printf("is token type: %d an operand?\n", token->type);
    return token->type == TOKEN_TYPE_IDENTIFIER       // id
           || token->type == TOKEN_TYPE_INTEGER_VALUE // i32
           || token->type == TOKEN_TYPE_DOUBLE_VALUE; // f64
}

// Returns 1 if the token is a delimiter, 0 otherwise
int isDelimiter(Token *token) { // Returns 1 if the token is a delimiter, 0 otherwise
    // printf("is token type: %d a delimiter?\n", token->type);
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

/**
 * @brief Fills the input stack with tokens up to a delimiter token.
 *
 * @param stack A pointer to the stack to be filled.
 * @param delimiterToken A pointer that stores the token that acts as a delimiter.
 * @return A pointer to the filled stack.
 */
Stack *fillInputStack(Stack *stack, Token *delimiterToken) {
    Stack tempStack;
    initStack(&tempStack);

    Token *token = (Token *)malloc(sizeof(Token));
    if (token == NULL) {
        fprintf(stderr, "Memory allocation failure.\n");
        exit(1);
    }
    getNextToken(token);

    while (isOperand(token) || isOperator(token)) {
        StackElement *newElement = malloc(sizeof(StackElement));
        if (newElement == NULL) {
            fprintf(stderr, "Memory allocation failure.\n");
            exit(1);
        }
        Token *tempToken = malloc(sizeof(Token));
        if (tempToken == NULL) {
            free(newElement);
            fprintf(stderr, "Memory allocation failure.\n");
            exit(1);
        }
        *tempToken = *token; // Copy the token
        initStackElement(newElement, tempToken);
        push(&tempStack, newElement);
        // display(&tempStack);
        getNextToken(token);
    }
    delimiterToken = token;
    if (!isDelimiter(token)) {
        return NULL; // syntax error
    }

    while (!isEmpty(&tempStack)) {
        StackElement *topElement = top(&tempStack);
        StackElement *newElement = malloc(sizeof(StackElement));
        if (newElement == NULL) {
            fprintf(stderr, "Memory allocation failure.\n");
            exit(1);
            return NULL;
        }
        Token *tempToken = malloc(sizeof(Token));
        if (tempToken == NULL) {
            free(newElement);
            fprintf(stderr, "Memory allocation failure.\n");
            exit(1);
            return NULL;
        }
        *tempToken = *(topElement->tokenPtr); // Copy the token
        initStackElement(newElement, tempToken);
        push(stack, newElement);
        pop(&tempStack);
    }
    cleanupStack(&tempStack);
    return stack;
}

/**
 * @brief Checks if the given stack is reducible.
 *
 * @param stack A pointer to the stack to be checked.
 * @return An integer indicating whether the stack is reducible (0) or not (1).
 */
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

    if (isOperator(leftToken) && middleToken->type == EXPR && rightToken->type == EXPR) {
        return 0;
    }
    return 1;
}

/**
 * @brief Chooses the appropriate reduction rule based on the current state of the stack. Use only
 * if isReducible() returns 0.
 *
 * @param stack A pointer to the stack used in the expression parser.
 * @return An integer representing the chosen reduction rule or -1 if no rule is applicable.
 */
int chooseReduceRule(Stack *stack) {

    StackElement *e1 = stack->top;
    StackElement *e2 = e1->next;
    StackElement *e3 = e2->next;

    if (e1 && e2 && e3 && e1->tokenPtr->type == EXPR && isOperator(e2->tokenPtr) &&
        e3->tokenPtr->type == EXPR) {
        switch (e2->tokenPtr->type) {
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
    } else if (e1 && isOperand(e1->tokenPtr)) {
        return EXPR_ID;
    }
    return -1; // No rule to apply
}
Token *createToken(TokenType type) {
    Token *token = (Token *)malloc(sizeof(Token));
    if (token == NULL) {
        fprintf(stderr, "Memory allocation failure.\n");
        exit(1);
    }
    token->type = type;
    return token;
}

int parseExpression(AST *exprAST, Token *token) {

    // Initialize the stack
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    if (stack == NULL) {
        fprintf(stderr, "Memory allocation failure.\n");
        exit(1);
    }
    initStack(stack);
    StackElement *dollarElement = (StackElement *)malloc(sizeof(StackElement));
    if (dollarElement == NULL) {
        fprintf(stderr, "Memory allocation failure.\n");
        exit(1);
    }
    initStackElement(dollarElement,
                     createToken(DOLLAR)); // Push the expression token onto the stack
    push(stack, dollarElement);            // Push the dollar token onto the stack
    // printf("stack: ");
    // display(stack);
    // Initialize the input stack
    Stack *input = (Stack *)malloc(sizeof(Stack));
    if (input == NULL) {
        fprintf(stderr, "Memory allocation failure.\n");
        exit(1);
    }
    initStack(input);
    if (fillInputStack(input, token) == NULL) {
        return 1; // Token doesn't belong in the expression
    }
    // printf("input stack: ");
    // display(input);

    // Initialize the AST
    exprAST = initAST();

    // Main loop
    Token *currentToken = (Token *)malloc(sizeof(Token));
    if (currentToken == NULL) {
        fprintf(stderr, "Memory allocation failure.\n");
        exit(1);
    }
    while (!isEmpty(input)) {
        *currentToken = *topToken(input);
        pop(input);
        // printf("current token type: %d\n", currentToken->type);
        // Shift
        StackElement *newElement = (StackElement *)malloc(sizeof(StackElement));
        if (newElement == NULL) {
            fprintf(stderr, "Memory allocation failure.\n");
            exit(1);
        }
        Token *tempToken = malloc(sizeof(Token));
        if (tempToken == NULL) {
            free(newElement);
            fprintf(stderr, "Memory allocation failure.\n");
            exit(1);
        }
        *tempToken = *currentToken; // Copy the token
        initStackElement(newElement, tempToken);
        push(stack, newElement);

        // printf("\nstack: ");
        // display(stack);

        // printf("input: ");
        // display(input);
        if (isReducible(stack)) { // Reduce
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
        } else {
            // shift
        }
    }

    return 0;
}