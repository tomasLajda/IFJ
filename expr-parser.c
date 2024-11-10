/*
IFJ Project

@brief Implementation file for the expression parser

@author Matúš Csirik - xcsirim00

*/

#include "expr-parser.h"
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

    while (isOperand(token) || isOperator(token) || token->type == TOKEN_TYPE_LEFT_BR ||
           token->type == TOKEN_TYPE_RIGHT_BR) {
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
 * @param nextInputToken A pointer to the next token to be processed.
 * @return An integer indicating whether the stack is reducible (0) or not (1).
 */
int isReducible(Stack *stack, Token *nextInputToken) {
    printf("is reducible?\n");
    // printf("%d ? %d ", stack->top->tokenPtr->type, nextInputToken->type);

    if (stack == NULL || stack->top == NULL) {
        // printf("stack is empty\n");
        return 0; // Stack is empty, cannot reduce
    }

    StackElement *first = stack->top;

    // Check for operand reduction (e.g., EXPR → id)
    if (isOperand(first->tokenPtr)) {
        // printf("is operand\n");
        return 1; // Stack is reducible by operand rule
    }

    // Ensure there are at least three elements on the stack for operator reduction
    if (getStackLength(stack) >= 3) {
        StackElement *second = first->next;
        StackElement *third = second->next;

        if (first->tokenPtr->type == EXPR && isOperator(second->tokenPtr) &&
            third->tokenPtr->type == EXPR) {

            // Get precedence and associativity of operators from the precedence table
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

    // printf("choose reduce rule\n");
    StackElement *first = stack->top;

    if (isOperand(first->tokenPtr)) {
        // printf("first: %d\n", first->tokenPtr->type);
        return EXPR_ID;
    }

    StackElement *second = first->next;
    StackElement *third = second->next;

    if (first->tokenPtr->type == TOKEN_TYPE_RIGHT_BR && second->tokenPtr->type == EXPR &&
        third->tokenPtr->type == TOKEN_TYPE_LEFT_BR) {
        return EXPR_PAR;
    }

    if (first->tokenPtr->type == EXPR && isOperator(second->tokenPtr) &&
        third->tokenPtr->type == EXPR) {
        // printf("first: %d, second: %d, third: %d\n", first->tokenPtr->type,
        // second->tokenPtr->type,
        //    third->tokenPtr->type);
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
    printf("no rule\n");
    return -1; // No rule to apply
}

StackElement *createStackElement(Token *token, AST *ast) {
    StackElement *element = (StackElement *)malloc(sizeof(StackElement));
    if (element == NULL) {
        fprintf(stderr, "Memory allocation failure.\n");
        exit(1);
    }
    element->ASTPtr = ast;
    element->tokenPtr = token;
    element->next = NULL;
    return element;
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

    // Initialize the stack on which the expression will be shifted and reduced
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    if (stack == NULL) {
        fprintf(stderr, "Memory allocation failure.\n");
        exit(1);
    }
    initStack(stack);
    StackElement *dollarElement = (StackElement *)malloc(sizeof(StackElement));
    if (dollarElement == NULL) {
        fprintf(stderr, "Memory allocation failure.\n");
        cleanupStack(stack);
        free(stack);
        exit(1);
    }
    StackElement *dollar = createStackElement(createToken(DOLLAR), NULL);
    printf("dollar: %d\n", dollar->tokenPtr->type);
    display(stack);
    push(stack, dollar); // Push the dollar element onto the stack
    printf("stack: ");
    display(stack);

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

    // Initialize the AST
    exprAST = initAST(); // TODO
    if (exprAST == NULL) {
        fprintf(stderr, "Memory allocation failure.\n");
        return 1;
    }

    // Initialize the current and next input element
    StackElement *currentInputElement = (StackElement *)malloc(sizeof(StackElement));
    if (currentInputElement == NULL) {
        fprintf(stderr, "Memory allocation failure.\n");
        exit(1);
    }
    initStackElement(currentInputElement, NULL);
    currentInputElement = top(input);
    StackElement *nextInputElement = (StackElement *)malloc(sizeof(StackElement));
    if (nextInputElement == NULL) {
        fprintf(stderr, "Memory allocation failure.\n");
        free(currentInputElement);
        exit(1);
    }
    initStackElement(nextInputElement, NULL);
    if (!isEmpty(input)) {
        nextInputElement = top(input)->next;
    } else {
        nextInputElement = createStackElement(createToken(DOLLAR), NULL);
    }

    printf("current token ptr type: %d\n", currentInputElement->tokenPtr->type);
    printf("next token ptr type: %d\n", nextInputElement->tokenPtr->type);
    printf("\nbefore_stack: ");
    display(stack);
    printf("before_input: ");
    display(input);
    printf("\nSTART\n");
    // Shift-reduce loop
    while (!isEmpty(input) || isReducible(stack, nextInputElement->tokenPtr)) {
        if (isReducible(stack, nextInputElement->tokenPtr)) { // Reduce
            printf("yes\n");
            int rule = chooseReduceRule(stack);
            printf("rule: %d\n", rule);
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
                // Reduce
                // Pop right operand
                StackElement *rightElement =
                    createStackElement(topToken(stack), top(stack)->ASTPtr);
                pop(stack);
                // ASTNode *rightNode = (ASTNode *)rightElement->ASTPtr->root;

                // Pop operator
                StackElement *operatorElement = createStackElement(topToken(stack), NULL);
                pop(stack);

                // Pop left operand
                StackElement *leftElement = createStackElement(topToken(stack), top(stack)->ASTPtr);
                pop(stack);
                // ASTNode *leftNode = (ASTNode *)leftElement->ASTPtr->root;

                // // Create new AST node for the operator
                // ASTNode *parent = initASTNode();
                // if (parent == NULL) {
                //     fprintf(stderr, "Memory allocation failure.\n");
                //     // Clean up and exit
                //     disposeSubtree(leftNode);
                //     disposeSubtree(rightNode);
                //     exit(1);
                // }
                // parent->token = operatorToken;
                // parent->nodeType.tokenType = operatorToken->type;
                // parent->isExpression = true;
                // parent->left = leftNode;
                // parent->right = rightNode;
                // parent->parent = NULL;
                // parent->absParent = NULL;

                // // Update parent pointers of children
                // leftNode->parent = parent;
                // rightNode->parent = parent;

                // Create a new stack element for Expr
                StackElement *exprElement =
                    createStackElement(createToken(EXPR), NULL); // TODO: AST
                // Push Expr back onto stack
                push(stack, exprElement);
                printf("\n[Reduce] Applied rule: %d\n", (rule));
                printf("Parser Stack after reduction:\n");
                display(stack);
                break;
            case EXPR_ID: // id -> Expr
                printf("REDUCING BY ID RULE: ");
                display(stack);
                // Pop the operand from the stack
                AST *eAST = initAST();
                ASTNode *root = initASTNode();
                if (eAST == NULL || root == NULL) {
                    fprintf(stderr, "Memory allocation failure.\n");
                    exit(1);
                }
                eAST->root = root;
                eAST->root->token = createToken(stack->top->tokenPtr->type);
                StackElement *newIdExpressionElement = createStackElement(createToken(EXPR), eAST);
                pop(stack);
                push(stack, newIdExpressionElement);
                printf("\n[Reduce] Applied rule: %d\n", (rule));
                printf("Parser Stack after reduction:\n");
                display(stack);
                break;
            case EXPR_PAR: // ( Expr ) -> Expr
                printf("REDUCING BY PARENTHESES RULE: ");
                display(stack);
                // Pop the right parenthesis
                pop(stack);

                // Pop the expression
                StackElement *Expression = createStackElement(topToken(stack), top(stack)->ASTPtr);
                pop(stack);

                // Pop the left parenthesis
                pop(stack);

                // Push the expression
                StackElement *newParExpressionElement =
                    createStackElement(createToken(EXPR), Expression->ASTPtr);
                push(stack, newParExpressionElement);
                printf("\n[Reduce] Applied rule: %d\n", (rule));
                printf("Parser Stack after reduction:\n");
                display(stack);
                break;
            default:
                fprintf(stderr, "Syntax error.\n");
                return 1; // Syntax error
            }
        } else if (!isEmpty(input)) { // Shift
            printf("No, so shifting...\n");

            Token *currentToken = createToken(currentInputElement->tokenPtr->type);
            pop(input);
            StackElement *newElement = createStackElement(currentToken, NULL);
            push(stack, newElement);

            printf("STACK AFTER SHIFTING: ");
            display(stack);
            printf("INPUT AFTER SHIFTING: ");
            display(input);

            currentInputElement = top(input);
            if (isEmpty(input)) {
                nextInputElement = createStackElement(createToken(DOLLAR), NULL);
            } else {
                nextInputElement = currentInputElement;
            }
        } else {
            fprintf(stderr, "Syntax error: Cannot reduce further.\n");
            return 1; // Indicate syntax error
        }
    }

    if (getStackLength(stack) == 2 && stack->top->tokenPtr->type == EXPR &&
        stack->top->next->tokenPtr->type == DOLLAR) {
        printf("Parsing successful.\n");
        exprAST->root = stack->top->ASTPtr; // Assign the AST root
        cleanupStack(input);
        free(input);
        cleanupStack(stack);
        free(stack);
        return 0; // Success
    } else {
        printf("fail\n");
        cleanupStack(input);
        free(input);
        cleanupStack(stack);
        free(stack);
        return 1; // Syntax error
    }
}