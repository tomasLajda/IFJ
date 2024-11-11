/*
IFJ Project

@brief Implementation file for the expression parser

@author Matúš Csirik - xcsirim00

*/

#include "expr-parser.h"
#include "error_codes.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    EXPR_ADD, // Expr     -> Expr + Expr
    EXPR_SUB, // Expr     -> Expr - Expr
    EXPR_EQ,  // Expr     -> Expr == Expr
    EXPR_NEQ, // Expr     -> Expr != Expr
    EXPR_LTH, // Expr     -> Expr < Expr
    EXPR_GTH, // Expr     -> Expr > Expr
    EXPR_LEQ, // Expr     -> Expr <= Expr
    EXPR_GEQ, // Expr     -> Expr >= Expr
    EXPR_MUL, // Expr     -> Expr * Expr
    EXPR_DIV, // Expr     -> Expr / Expr
    EXPR_ID,  // id       -> Expr
    EXPR_PAR  // ( Expr ) -> Expr
} ExpressionRule;

char precedenceTable[14][14] = {
    /*   */ /* +    -    *    /    <    >    <=   >=   !=   ==   (    )    i    $   */
    /* +  */ {'>', '>', '<', '<', '>', '>', '>', '>', '<', '>', '<', '>', '<', '>'},
    /* -  */ {'>', '>', '<', '<', '>', '>', '>', '>', '<', '>', '<', '>', '<', '>'},
    /* *  */ {'>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>', '<', '>'},
    /* /  */ {'>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>', '<', '>'},
    /* <  */ {'<', '<', '<', '<', 'x', 'x', 'x', 'x', '<', '>', '<', '>', '<', '>'},
    /* >  */ {'<', '<', '<', '<', 'x', 'x', 'x', 'x', '<', '>', '<', '>', '<', '>'},
    /* <= */ {'<', '<', '<', '<', 'x', 'x', 'x', 'x', '<', '>', '<', '>', '<', '>'},
    /* >= */ {'<', '<', '<', '<', 'x', 'x', 'x', 'x', '<', '>', '<', '>', '<', '>'},
    /* != */ {'<', '<', '<', '<', 'x', 'x', 'x', 'x', '<', '>', '<', '>', '<', '>'},
    /* == */ {'<', '<', '<', '<', 'x', 'x', 'x', 'x', '<', '>', '<', '>', '<', '>'},
    /* (  */ {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<', 'x'},
    /* )  */ {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'x', '>', 'x', '>'},
    /* i  */ {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'x', '>', 'x', '>'},
    /* $  */ {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', 'x', '<', 'x'}

};

#define EXPR TOKEN_TYPE_EXPR
#define DOLLAR TOKEN_TYPE_DOLLA

int getTableIndex(TokenType type) {
    switch (type) {
    case TOKEN_TYPE_PLUS:
        return 0;
    case TOKEN_TYPE_MINUS:
        return 1;
    case TOKEN_TYPE_MUL:
        return 2;
    case TOKEN_TYPE_DIV:
        return 3;
    case TOKEN_TYPE_LTH:
        return 4;
    case TOKEN_TYPE_GTH:
        return 5;
    case TOKEN_TYPE_LEQ:
        return 6;
    case TOKEN_TYPE_GEQ:
        return 7;
    case TOKEN_TYPE_NEQ:
        return 8;
    case TOKEN_TYPE_EQ:
        return 9;
    case TOKEN_TYPE_LEFT_BR:
        return 10;
    case TOKEN_TYPE_RIGHT_BR:
        return 11;
    case TOKEN_TYPE_IDENTIFIER:
    case EXPR:
        return 12;
    case TOKEN_TYPE_DOLLA:
        return 13;
    default:
        return -1;
    }
}

// Returns 1 if the token is an operator, 0 otherwise
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

// Returns 1 if the token is an operand, 0 otherwise
int isOperand(Token *token) {
    return token->type == TOKEN_TYPE_IDENTIFIER       // id
           || token->type == TOKEN_TYPE_INTEGER_VALUE // i32
           || token->type == TOKEN_TYPE_DOUBLE_VALUE; // f64
}

int isParentheses(Token *token) {
    return token->type == TOKEN_TYPE_LEFT_BR || token->type == TOKEN_TYPE_RIGHT_BR;
}

// Returns 1 if the token is a delimiter, 0 otherwise
int isDelimiter(Token *token) {
    return token->type == TOKEN_TYPE_SEMICOLON || // ;    a = 2 + 3;
           token->type == TOKEN_TYPE_COMMA ||     // ,    f(2+3, 4){}
           token->type == TOKEN_TYPE_RIGHT_BR;    // )    f(2+3){}
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
 * @brief Checks if the given stack is reducible.
 *
 * @param stack A pointer to the stack to be checked.
 * @param nextInputToken A pointer to the next token to be processed.
 * @return An integer indicating whether the stack is reducible (0) or not (1).
 */
int isReducible(Stack *stack, Token *nextInputToken) {
    if (stack == NULL || stack->top == NULL) {
        return 0; // Stack is empty, cannot reduce
    }

    printf("Checking reducibility of the stack...\n");
    display(stack);
    printf("Next input token: %s\n", TokenTypeToString(nextInputToken->type));

    StackElement *first = stack->top;

    // Check for operand reduction (e.g., EXPR → id)
    if (isOperand(first->tokenPtr)) {
        return 1; // Stack is reducible by operand rule
    }

    // Check for operator reduction (e.g., EXPR → Expr + Expr)
    if (getStackLength(stack) >= 3) {
        StackElement *second = first->next;
        StackElement *third = second->next;

        if (first->tokenPtr->type == EXPR && isOperator(second->tokenPtr) &&
            third->tokenPtr->type == EXPR) {

            // Get precedence from the precedence table
            char precedence = precedenceTable[getTableIndex(second->tokenPtr->type)]
                                             [getTableIndex(nextInputToken->type)];

            if (precedence == '>') {
                return 1; // Reduce
            } else if (precedence == '<') {
                return 0; // Shift
            } else {
                // Handle equal precedence based on associativity
                if (isLeftAssociative(second->tokenPtr)) {
                    return 1; // Reduce due to left associativity
                } else {
                    return 0; // Shift due to right associativity
                }
            }
        }
    }
    // Check for parentheses reduction (Expr → ( Expr ))
    if (getStackLength(stack) >= 3) {
        StackElement *second = first->next;
        StackElement *third = second->next;

        if (first->tokenPtr->type == TOKEN_TYPE_RIGHT_BR && second->tokenPtr->type == EXPR &&
            third->tokenPtr->type == TOKEN_TYPE_LEFT_BR) {
            return 1; // Stack is reducible by parentheses rule
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
    StackElement *first = stack->top;
    if (isOperand(first->tokenPtr)) {
        return EXPR_ID; // Expr -> id
    }

    StackElement *second = first->next;
    StackElement *third = second->next;

    if (first->tokenPtr->type == TOKEN_TYPE_RIGHT_BR && second->tokenPtr->type == EXPR &&
        third->tokenPtr->type == TOKEN_TYPE_LEFT_BR) {
        return EXPR_PAR; // Expr -> ( Expr )
    }

    if (first->tokenPtr->type == EXPR && isOperator(second->tokenPtr) &&
        third->tokenPtr->type == EXPR) {
        switch (second->tokenPtr->type) {
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
    return -1; // No rule to apply
}

StackElement *createStackElement(Token *token, AST *ast) {
    StackElement *element = (StackElement *)malloc(sizeof(StackElement));
    if (element == NULL) {
        HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, NULL);
    }
    element->ASTPtr = ast;
    element->tokenPtr = token;
    element->next = NULL;
    return element;
}

Token *createToken(TokenType type) {
    Token *token = (Token *)malloc(sizeof(Token));
    if (token == NULL) {
        HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, NULL);
    }
    token->type = type;
    return token;
}

Token *copyToken(Token *token) {
    Token *newToken = (Token *)malloc(sizeof(Token));
    if (newToken == NULL) {
        HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, NULL);
    }
    newToken->type = token->type;
    newToken->attribute = token->attribute;
    newToken->line = token->line;
    return newToken;
}

/**
 * @brief Fills the input stack with tokens up to a delimiter token.
 *
 * @param stack A pointer to the stack to be filled.
 * @param delimiterToken A pointer that stores the token that acts as a delimiter.
 * @return A pointer to the filled stack if successful, NULL otherwise.
 */
Stack *fillInputStack(Stack *stack, Token *delimiterToken) {
    // Initialize a temporary stack and a token pointer
    Stack tempStack;
    initStack(&tempStack);
    Token *token = (Token *)malloc(sizeof(Token));
    if (token == NULL) {
        HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, NULL);
    }

    // Begin filling
    getNextToken(token);
    printf("token type: %s\n", TokenTypeToString(token->type));
    while (isOperand(token) || isOperator(token) || isParentheses(token)) {
        // Copy the token for the stack element
        Token *tempToken = copyToken(token);
        if (tempToken == NULL) {
            free(token);
            HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, NULL);
        }

        // Create a stack element and push it onto the temporary stack
        StackElement *newElement = createStackElement(tempToken, NULL);
        if (newElement == NULL) {
            free(tempToken);
            free(token);
            HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, NULL);
        }
        push(&tempStack, newElement);
        getNextToken(token);
    }
    printf("token type: %s\n", TokenTypeToString(token->type));
    // Check if the last token is a delimiter
    *delimiterToken = *token;
    printf("delimiter token type: %s\n", TokenTypeToString(delimiterToken->type));
    if (!isDelimiter(token)) {
        free(token);
        cleanupStack(&tempStack);
        return NULL; // Token doesn't belong in the expression - a syntax error occured
    }

    // Reverse the temporary stack and push it onto the input stack
    while (!isEmpty(&tempStack)) {
        StackElement *topElement = top(&tempStack);

        // Duplicate the token from the top element
        Token *tempToken = copyToken(topElement->tokenPtr);
        if (tempToken == NULL) {
            cleanupStack(&tempStack);
            free(token);
            HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, NULL);
        }

        // Create a new stack element with the duplicated token
        StackElement *newElement = createStackElement(tempToken, NULL);
        if (newElement == NULL) {
            free(token);
            cleanupStack(&tempStack);
            free(tempToken);
            HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, NULL);
        }

        // Push the new element onto the input stack
        push(stack, newElement);
        // Pop the top element from the temporary stack
        pop(&tempStack);
    }
    cleanupStack(&tempStack);
    free(token);
    return stack;
}

int parseExpression(AST *exprAST, Token *token) {

    // Initialize the stack onto which the expression will be shifted and reduced
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    if (stack == NULL) {
        HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, 2);
    }
    initStack(stack);
    // Add a dollar element which serves as a bottom of the stack
    StackElement *dollar = createStackElement(createToken(DOLLAR), NULL);
    if (dollar == NULL || dollar->tokenPtr == NULL) {
        cleanupStack(stack);
        free(stack);
        HANDLE_ERROR("Memory allocation failure.\n", INTERNAL_ERROR, 2);
    }
    push(stack, dollar);

    // Initialize the input stack
    Stack *input = (Stack *)malloc(sizeof(Stack));
    if (input == NULL) {
        cleanupStack(stack);
        free(stack);
        free(dollar);
        HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, 2);
    }
    initStack(input);

    // Fill the input stack with tokens up to the delimiter token
    if (fillInputStack(input, token) == NULL) {
        fprintf(stderr, "Token doesn't belong in the expression.\n");
        cleanupStack(input);
        free(input);
        cleanupStack(stack);
        free(stack);
        return 1; // Indicate syntax error
    }

    // Initialize the AST
    // exprAST = initAST(); // TODO: implement AST filling
    // if (exprAST == NULL) {
    //     fprintf(stderr, "Memory allocation failure.\n");
    //     return 1;
    // }

    // // LOGGING PRINTS
    printf("      starting stack: ");
    display(stack);
    printf("starting input stack:");
    display(input);
    printf(
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");

    // Initialize the current and next input element
    StackElement *currentInputElement = top(input);

    // // LOGGING PRINTS
    // printf("current token ptr type: %d\n", currentInputElement->tokenPtr->type);
    // printf("next token ptr type: %d\n", nextInputElement->tokenPtr->type);
    // printf("\nbefore_stack: ");
    // display(stack);
    // printf("before_input: ");
    // display(input);
    // printf("\nSTART\n");

    // Shift-reduce loop
    while (!isEmpty(input) || isReducible(stack, currentInputElement->tokenPtr)) {
        if (isReducible(stack, currentInputElement->tokenPtr)) { // Reduce
            int rule = chooseReduceRule(stack);
            printf("reducing ");
            display(stack);

            switch (rule) {
            case EXPR_ADD: // Expr -> Expr + Expr
            case EXPR_SUB: // Expr -> Expr - Expr
            case EXPR_EQ:  // Expr -> Expr == Expr
            case EXPR_NEQ: // Expr -> Expr != Expr
            case EXPR_LTH: // Expr -> Expr < Expr
            case EXPR_GTH: // Expr -> Expr > Expr
            case EXPR_LEQ: // Expr -> Expr <= Expr
            case EXPR_GEQ: // Expr -> Expr >= Expr
            case EXPR_MUL: // Expr -> Expr * Expr
            case EXPR_DIV: // Expr -> Expr / Expr

                // Pop right operand
                // StackElement *rightElement = createStackElement(topToken(stack), NULL);
                pop(stack);
                // Pop operator
                // StackElement *operatorElement = createStackElement(topToken(stack), NULL);
                pop(stack);
                // Pop left operand
                // StackElement *leftElement = createStackElement(topToken(stack), NULL);
                pop(stack);

                // Push Expr token to replace the reduced expression
                push(stack, createStackElement(createToken(EXPR), NULL));
                // printf("\n[Reduce] Applied rule: %d\n", (rule));
                // printf("Parser Stack after reduction:\n");
                // display(stack);
                // printf("---------------------------------------------------------------------------"
                //        "----"
                //        "------------------------------------\n");
                break;
            case EXPR_ID: // id -> Expr
                // printf("REDUCING BY ID RULE: ");
                // display(stack);

                // Pop id
                pop(stack);
                // Push Expr token to replace the id
                push(stack, createStackElement(createToken(EXPR), NULL));

                // printf("\n[Reduce] Applied rule: %d\n", (rule));
                // printf("Parser Stack after reduction:\n");
                // display(stack);
                // printf("---------------------------------------------------------------------------"
                //        "----"
                //        "------------------------------------\n");
                break;
            case EXPR_PAR: // ( Expr ) -> Expr
                // printf("REDUCING BY PARENTHESES RULE: ");
                // display(stack);
                // Pop the right parenthesis
                pop(stack);
                // Pop the expression
                pop(stack);
                // Pop the left parenthesis
                pop(stack);

                // Push Expr token to replace the reduced expression
                push(stack, createStackElement(createToken(EXPR), NULL));
                // printf("\n[Reduce] Applied rule: %d\n", (rule));
                // printf("Parser Stack after reduction:\n");
                // display(stack);
                // printf("---------------------------------------------------------------------------"
                //        "----"
                //        "------------------------------------\n");
                break;
            default:
                fprintf(stderr, "Syntax error.\n");
                cleanupStack(input);
                free(input);
                cleanupStack(stack);
                free(stack);
                return 1; // Syntax error
            }
        } else if (!isEmpty(input)) { // Shift
            // printf("No, so shifting...\n");

            Token *currentToken = createToken(currentInputElement->tokenPtr->type);
            pop(input);
            StackElement *newElement = createStackElement(currentToken, NULL);
            push(stack, newElement);

            // printf("STACK AFTER SHIFTING: ");
            // display(stack);
            // printf("INPUT AFTER SHIFTING: ");
            // display(input);
            // printf("-------------------------------------------------------------------------------"
            //        "------------------------------------\n");

            currentInputElement = top(input);
            if (isEmpty(input)) {
                currentInputElement = createStackElement(createToken(DOLLAR), NULL);
            }
        } else {
            fprintf(stderr, "Error: Cannot reduce further.\n");
            cleanupStack(input);
            free(input);
            cleanupStack(stack);
            free(stack);
            return 1; // Indicate syntax error
        }
    }

    if (getStackLength(stack) == 2 && stack->top->tokenPtr->type == EXPR &&
        stack->top->next->tokenPtr->type == DOLLAR) {
        printf("Parsing successful.\n");
        cleanupStack(input);
        free(input);
        cleanupStack(stack);
        free(stack);
        return 0;
    } else {
        printf("Incorrect syntax detected.\n");
        cleanupStack(input);
        free(input);
        cleanupStack(stack);
        free(stack);
        return 1; // Indicate syntax error
    }
}