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

#define EXPR TOKEN_TYPE_EXPR
#define DOLLAR TOKEN_TYPE_DOLLA

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
    if (stack == NULL || stack->top == NULL) {
        return 0; // Stack is empty, cannot reduce
    }

    StackElement *first = stack->top;

    // Case 1: Reduce operand to EXPR (EXPR → id)
    if (isOperand(first->tokenPtr)) {
        return 1; // Stack is reducible
    }

    // Case 2: Check for 'EXPR Operator EXPR' pattern
    if (getStackLength(stack) >= 3) {
        StackElement *second = first->next;
        StackElement *third = second->next;

        if (first->tokenPtr->type == EXPR && isOperator(second->tokenPtr) &&
            third->tokenPtr->type == EXPR) {
            return 1; // Stack is reducible
        }
    }

    return 0; // No reducible pattern found
}

/**
 * @brief Chooses the appropriate reduction rule based on the current state of the stack. Use only
 * if isReducible() returns 0.
 *
 * @param stack A pointer to the stack used in the expression parser.
 * @return An integer representing the chosen reduction rule or -1 if no rule is applicable.
 */
int chooseReduceRule(Stack *stack) {

    printf("choose reduce rule\n");
    StackElement *e1 = stack->top;
    StackElement *e2 = e1->next;
    StackElement *e3 = e2->next;

    if (e1 && e2 && e3 && e1->tokenPtr->type == EXPR && isOperator(e2->tokenPtr) &&
        e3->tokenPtr->type == EXPR) {
        // printf("e1: %d, e2: %d, e3: %d\n", e1->tokenPtr->type, e2->tokenPtr->type,
        //    e3->tokenPtr->type);
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
        // printf("e1: %d\n", e1->tokenPtr->type);
        return EXPR_ID;
    }
    printf("no rule\n");
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
        // cleanupStack(stack);
        free(stack);
        exit(1);
    }
    Token *dollarToken = createToken(DOLLAR);
    if (dollarToken == NULL) {
        fprintf(stderr, "Memory allocation failure.\n");
        free(dollarElement);
        free(stack);
        return 1;
    }
    initStackElement(dollarElement, dollarToken);
    push(stack, dollarElement); // Push the dollar token onto the stack

    // printf("stack: ");
    // display(stack);

    // Initialize the input stack
    Stack *input = (Stack *)malloc(sizeof(Stack));
    if (input == NULL) {
        fprintf(stderr, "Memory allocation failure.\n");
        cleanupStack(stack);
        free(stack);
        exit(1);
    }
    initStack(input);
    if (fillInputStack(input, token) == NULL) {
        fprintf(stderr, "Token doesn't belong in the expression.\n");
        // Free allocated memory before returning
        cleanupStack(input);
        free(input);
        cleanupStack(stack);
        free(stack);
        return 1;
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

    printf("\nbefore_stack: ");
    display(stack);

    printf("before_input: ");
    display(input);

    while (!isEmpty(input) || isReducible(stack)) {
        if (isReducible(stack)) { // Reduce
            printf("reducible\n");
            switch (chooseReduceRule(stack)) {
            case EXPR_ADD: // Expr -> Expr + Expr
                printf("REDUCING BY ADD RULE: ");
                // Pop right operand
                display(stack);
                StackElement *rightElement = top(stack);
                ASTNode *rightNode = initASTNode();
                rightNode->token = rightElement->tokenPtr;
                pop(stack);

                // Pop operator
                StackElement *operatorElement = top(stack);
                Token *operatorToken = operatorElement->tokenPtr;
                pop(stack);

                // Pop left operand
                StackElement *leftElement = top(stack);
                ASTNode *leftNode = initASTNode();
                leftNode->token = leftElement->tokenPtr;
                pop(stack);

                // Create new AST node
                ASTNode *parent = initASTNode();
                parent->nodeType.tokenType = operatorToken->type;
                parent->token = operatorToken;
                parent->left = leftNode;
                parent->right = rightNode;
                parent->isExpression = true;

                // Push Expr back onto stack
                Token *eToken = createToken(EXPR);
                StackElement *eElement = malloc(sizeof(StackElement));
                if (eElement == NULL) {
                    fprintf(stderr, "Memory allocation failure.\n");
                    disposeSubtree(parent);
                    exit(1);
                }
                initStackElement(eElement, eToken);
                push(stack, eElement);
                display(stack);
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
                printf("REDUCING: ");
                display(stack);
                printf("BY ID RULE\n");
                // Pop the operand from the stack
                StackElement *operandElement = top(stack);
                ASTNode *operandNode = initASTNode();
                operandNode->token = operandElement->tokenPtr;
                pop(stack);

                // Push the operand back onto the stack
                Token *exprToken = createToken(EXPR);
                StackElement *exprElement = malloc(sizeof(StackElement));
                if (exprElement == NULL) {
                    fprintf(stderr, "Memory allocation failure.\n");
                    // Cleanup and exit
                    disposeSubtree(operandNode);
                    exit(1);
                }
                initStackElement(exprElement, exprToken);
                push(stack, exprElement);
                printf("AFTER REDUCING: ");
                display(stack);
                break;
            default:
                return 1; // Syntax error
            }
        } else if (!isEmpty(input)) { // Shift
            printf("Shifting\n");
            *currentToken = *topToken(input);
            pop(input);
            printf("current token type: %d\n", currentToken->type);
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

            printf("\nstack: ");
            display(stack);

            printf("input: ");
            display(input);
            printf("\n");
        }
    }

    // success
    printf("success\n");
    cleanupStack(input);
    free(input);
    cleanupStack(stack);
    free(stack);
    return 0;
}