/*
IFJ Project

@brief Implementation file for the expression parser

@author Matúš Csirik - xcsirim00
TODO: delimiter testy, return int ak zloba

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
    /* +  */ {'>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'},
    /* -  */ {'>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'},
    /* *  */ {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'},
    /* /  */ {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'},
    /* <  */ {'<', '<', '<', '<', 'x', 'x', 'x', 'x', 'x', 'x', '<', '>', '<', '>'},
    /* >  */ {'<', '<', '<', '<', 'x', 'x', 'x', 'x', 'x', 'x', '<', '>', '<', '>'},
    /* <= */ {'<', '<', '<', '<', 'x', 'x', 'x', 'x', 'x', 'x', '<', '>', '<', '>'},
    /* >= */ {'<', '<', '<', '<', 'x', 'x', 'x', 'x', 'x', 'x', '<', '>', '<', '>'},
    /* != */ {'<', '<', '<', '<', 'x', 'x', 'x', 'x', 'x', 'x', '<', '>', '<', '>'},
    /* == */ {'<', '<', '<', '<', 'x', 'x', 'x', 'x', 'x', 'x', '<', '>', '<', '>'},
    /* (  */ {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', 'x', '<', 'x'},
    /* )  */ {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'x', '>', 'x', '>'},
    /* i  */ {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'x', '>', 'x', '>'},
    /* $  */ {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', 'x', '<', 'x'},
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

/**
 * @brief Checks if the given stack is reducible.
 *
 * @param stack A pointer to the stack to be checked.
 * @param nextInputEleemnt A pointer to the next element to be processed.
 * @return An integer indicating whether the stack is reducible (1), not (0), SYNTAX_ERROR (2), or
 * INTERNAL_ERROR (99).
 */
int isReducible(Stack *stack, StackElement *nextInputElement) {
    if (stack == NULL || stack->top == NULL || nextInputElement == NULL ||
        nextInputElement->tokenPtr == NULL) {
        return 0; // Stack is empty, cannot reduce
    }

    Token *nextInputToken = nextInputElement->tokenPtr;
    StackElement *first = stack->top;
    // Check for operand reduction (e.g., EXPR → id)
    if (isOperand(first->tokenPtr) || (first->tokenPtr->type == TOKEN_TYPE_KEYWORD &&
                                       first->tokenPtr->attribute.keyword == KEYWORD_NULL)) {
        return 1; // Stack is reducible by operand rule
    }

    // Check for operator reduction (e.g., EXPR → Expr + Expr)
    if (getStackLength(stack) >= 3) {
        StackElement *second = first->next;
        StackElement *third = second->next;

        // Verify the pattern: Expr Operator Expr
        if (first->tokenPtr->type == EXPR && isOperator(second->tokenPtr) &&
            third->tokenPtr->type == EXPR) {

            // Get precedence from the precedence table
            char precedence = precedenceTable[getTableIndex(second->tokenPtr->type)]
                                             [getTableIndex(nextInputToken->type)];

            switch (precedence) {
            case '>':
                return 1; // Reduce
            case '<':
                return 0; // Shift
            case 'x':
                return SYNTAX_ERROR; // Syntax error
            default:
                return INTERNAL_ERROR; // Internal error
            }
        }
    }
    // Check for parentheses reduction (Expr → ( Expr ))
    if (getStackLength(stack) >= 3) {
        StackElement *second = first->next;
        StackElement *third = second->next;
        // Verify the pattern: ) Expr (
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
 * @param stack Pointer to the current parsing stack.
 * @return
 *   - Returns a specific reduction rule identifier (e.g., EXPR_ADD) if a rule applies.
 *   - Returns EXPR_PAR for parentheses reduction.
 *   - Returns EXPR_ID for operand reduction.
 *   - Returns -1 if no applicable reduction rule is found.
 */
int chooseReduceRule(Stack *stack) {
    if (stack == NULL || stack->top == NULL) {
        return -1; // Invalid stack state
    }

    StackElement *first = stack->top;

    // Check for operand reduction (EXPR → id)
    if (isOperand(first->tokenPtr) || (first->tokenPtr->type == TOKEN_TYPE_KEYWORD &&
                                       first->tokenPtr->attribute.keyword == KEYWORD_NULL)) {
        return EXPR_ID; // Expr -> id
    }

    // Ensure there are at least three elements on the stack for further checks
    if (getStackLength(stack) < 3) {
        return -1; // Not enough elements to apply any reduction rule
    }

    StackElement *second = first->next;
    StackElement *third = second->next;

    // Check for parentheses reduction (Expr → ( Expr ))
    if (first->tokenPtr->type == TOKEN_TYPE_RIGHT_BR && second->tokenPtr->type == EXPR &&
        third->tokenPtr->type == TOKEN_TYPE_LEFT_BR) {
        return EXPR_PAR; // Expr -> ( Expr )
    }

    // 3. Check for Binary Operator Reduction (Expr → Expr operator Expr)
    if (first->tokenPtr->type == EXPR && isOperator(second->tokenPtr) &&
        third->tokenPtr->type == EXPR) {
        switch (second->tokenPtr->type) {
        case TOKEN_TYPE_PLUS:
            return EXPR_ADD; // Expr -> Expr + Expr
        case TOKEN_TYPE_MINUS:
            return EXPR_SUB; // Expr -> Expr - Expr
        case TOKEN_TYPE_EQ:
            return EXPR_EQ; //  Expr -> Expr == Expr
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
            return -1; // Undefined operator type
        }
    }
    return -1; // No rule to apply
}

/**
 * @brief Fills the input stack with tokens up to a delimiter token.
 *
 * @param stack Pointer to the stack to be filled with tokens.
 * @param delimiterToken Pointer to a token that will store the encountered delimiter token.
 *
 * @return
 *   - Pointer to the filled destination stack if successful.
 *   - NULL if a syntax error occurs or memory allocation fails.
 */
Stack *fillInputStack(Stack *stack, Token *firstToken, Token *secondToken, Token *delimiterToken) {

    // Initialize a temporary stack and a token pointer
    Stack tempStack;
    initStack(&tempStack);

    Token *token = (Token *)malloc(sizeof(Token));
    if (token == NULL) {
        HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, NULL);
    }

    int openingParentheses = 0;
    int closingParentheses = 0;
    int relationOperators = 0;
    int providedTokens = 0;

    if (firstToken != NULL) {
        providedTokens++;
    }
    if (secondToken != NULL) {
        providedTokens++;
    }

    // Begin filling
    if (providedTokens == 2) {
        if ((isOperand(firstToken) || (firstToken->type == TOKEN_TYPE_KEYWORD &&
                                       firstToken->attribute.keyword == KEYWORD_NULL)) &&
            isDelimiter(secondToken)) {
            ASTNode *astNode = initASTNode();
            astNode->token = copyToken(firstToken);
            push(stack, createStackElement(firstToken, astNode));
            Token *delim = copyToken(secondToken);
            *delimiterToken = *delim;
            return stack;
        }
        token = copyToken(firstToken);
        providedTokens--;
    } else if (providedTokens == 1) {
        if (firstToken != NULL) {
            token = copyToken(firstToken);
            providedTokens--;
        } else if (secondToken != NULL) {
            providedTokens--;
            token = copyToken(secondToken);
        } else {
            HANDLE_ERROR("Unexpected NULL token", INTERNAL_ERROR, NULL);
        }
    } else {
        getNextToken(token);
    }

    while (isOperand(token) || isOperator(token) || isParentheses(token) ||
           (token->type == TOKEN_TYPE_KEYWORD && token->attribute.keyword == KEYWORD_NULL)) {

        // Track parentheses balance
        if (token->type == TOKEN_TYPE_LEFT_BR) {
            openingParentheses++;
        } else if (token->type == TOKEN_TYPE_RIGHT_BR) {
            closingParentheses++;
        }
        if (closingParentheses > openingParentheses) {
            break;
        }
        if (isRelOperator(token)) {
            relationOperators++;
        }
        if (relationOperators > 1) {
            break;
        }
        // Copy the token for the stack element
        Token *tempToken = copyToken(token);
        if (tempToken == NULL) {
            freeToken(token);
            HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, NULL);
        }

        ASTNode *astNode = initASTNode();
        astNode->token = copyToken(token);
        // Create a stack element and push it onto the temporary stack
        StackElement *newElement = createStackElement(tempToken, astNode);
        if (newElement == NULL) {
            freeToken(tempToken);
            freeToken(token);
            HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, NULL);
        }
        push(&tempStack, newElement);
        if (providedTokens == 1) {
            token = copyToken(secondToken);
            providedTokens--;
        } else {
            getNextToken(token);
        }
    }

    // Assign the delimiter token and check if its valid
    *delimiterToken = *token;
    if (!isDelimiter(token)) {
        cleanupStack(&tempStack);
        HANDLE_ERROR(
            "Token doesn't belong in expression",
            SYNTAX_ERROR); // Token doesn't belong in the expression - a syntax error occured
    }

    if (isEmpty(&tempStack)) {
        cleanupStack(&tempStack);
        HANDLE_ERROR("Empty expression", SYNTAX_ERROR); // Empty expression - a syntax error occured
    }

    // Reverse the temporary stack and push it onto the input stack
    while (!isEmpty(&tempStack)) {
        StackElement *topElement = top(&tempStack);
        if (topElement == NULL) {
            cleanupStack(&tempStack);
            freeToken(token);
            HANDLE_ERROR("Unexpected NULL stack element", INTERNAL_ERROR, INTERNAL_ERROR);
        }

        // Duplicate the token from the top element
        Token *tempToken = copyToken(topElement->tokenPtr);
        if (tempToken == NULL) {
            cleanupStack(&tempStack);
            freeToken(token);
            HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, NULL);
        }

        // Duplicate the ASTNode from the top element
        ASTNode *astNode = copyASTNode(topElement->ASTNodePtr);
        if (astNode == NULL) {
            cleanupStack(&tempStack);
            freeToken(token);
            freeToken(tempToken);
            HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, NULL);
        }

        // Create a new stack element with the duplicated token
        StackElement *newElement = createStackElement(tempToken, astNode);
        if (newElement == NULL) {
            freeToken(token);
            cleanupStack(&tempStack);
            freeToken(tempToken);
            HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, NULL);
        }

        // Push the new element onto the input stack
        push(stack, newElement);
        // Pop the top element from the temporary stack
        pop(&tempStack);
    }
    cleanupStack(&tempStack);
    freeToken(token);
    return stack;
}

int parseExpression(AST *exprAST, Token *firstToken, Token *secondToken, Token *delimiterToken) {

    // Initialize the parsing stack
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    if (stack == NULL) {
        HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, INTERNAL_ERROR);
    }
    initStack(stack);

    // Add a dollar element which serves as a bottom of the stack
    Token *dollarToken = createToken(DOLLAR);
    StackElement *dollar = createStackElement(dollarToken, NULL);
    if (dollar == NULL || dollar->tokenPtr == NULL) {
        cleanupStack(stack);
        free(stack);
        HANDLE_ERROR("Memory allocation failure.\n", INTERNAL_ERROR, INTERNAL_ERROR);
    }
    push(stack, dollar);

    // Initialize the input stack
    Stack *input = (Stack *)malloc(sizeof(Stack));
    if (input == NULL) {
        cleanupStack(stack);
        free(stack);
        HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, INTERNAL_ERROR);
    }
    initStack(input);

    // Fill the input stack with tokens up to the delimiter token
    if (fillInputStack(input, firstToken, secondToken, delimiterToken) == NULL) { // syntax error
        cleanupStack(input);
        free(input);
        cleanupStack(stack);
        free(stack);
        HANDLE_ERROR("Syntax error. fillinupstack() returned null\n", SYNTAX_ERROR,
                     SYNTAX_ERROR); // Indicate syntax error
    }

    // Initialize the current input element
    StackElement *currentInputElement = top(input);
    if (isEmpty(input)) {
        dollarToken = createToken(DOLLAR);
        currentInputElement = createStackElement(dollarToken, NULL);
    }

    // Shift-reduce parsing loop
    // Continue parsing as long as there are input tokens or the stack can be reduced further
    while (!isEmpty(input) || isReducible(stack, currentInputElement)) {

        int reducible = isReducible(stack, currentInputElement);

        if (reducible == 1) { // Reduce
            int rule = chooseReduceRule(stack);
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
                ASTNode *rightNode = copyASTNode(top(stack)->ASTNodePtr);
                pop(stack);
                // Pop operator
                ASTNode *operatorNode = initASTNode();
                operatorNode->token = copyToken(top(stack)->tokenPtr);
                pop(stack);
                // Pop left operand
                ASTNode *leftNode = copyASTNode(top(stack)->ASTNodePtr);
                pop(stack);

                addLeftNode(exprAST, operatorNode, leftNode);
                addRightNode(exprAST, operatorNode, rightNode);

                // Push Expr token to replace the reduced expression
                push(stack, createStackElement(createToken(EXPR), operatorNode));
                break;

            case EXPR_ID: // id -> Expr
                // Pop id
                ASTNode *idNode = copyASTNode(top(stack)->ASTNodePtr);
                pop(stack);

                // Push Expr token to replace the id
                push(stack, createStackElement(createToken(EXPR), idNode));
                break;

            case EXPR_PAR: // ( Expr ) -> Expr
                // Pop the right parenthesis
                pop(stack);
                // Pop the expression
                ASTNode *parenthesesNode = copyASTNode(top(stack)->ASTNodePtr);
                pop(stack);
                // Pop the left parenthesis
                pop(stack);

                // Push Expr token to replace the reduced expression
                push(stack, createStackElement(createToken(EXPR), parenthesesNode));
                break;

            default:
                cleanupStack(input);
                free(input);
                cleanupStack(stack);
                free(stack);
                HANDLE_ERROR("Invalid reduction rule.\n", INTERNAL_ERROR, INTERNAL_ERROR);
            }
        } else if (reducible == 2) { // Syntax error detected
            cleanupStack(input);
            free(input);
            cleanupStack(stack);
            free(stack);
            HANDLE_ERROR("Syntax error. Expression precedence not found.\n", SYNTAX_ERROR,
                         SYNTAX_ERROR); // Syntax error
        } else if (!isEmpty(input)) {   // Shift
            Token *currentToken = copyToken(currentInputElement->tokenPtr);
            ASTNode *currentASTNode = copyASTNode(currentInputElement->ASTNodePtr);
            pop(input);
            StackElement *newElement = createStackElement(currentToken, currentASTNode);
            push(stack, newElement);

            currentInputElement = top(input);
            if (isEmpty(input)) {
                dollarToken = createToken(DOLLAR);
                currentInputElement = createStackElement(dollarToken, NULL);
            }

        } else {
            // Cannot reduce further
            freeToken(dollarToken);
            free(currentInputElement);
            cleanupStack(input);
            free(input);
            cleanupStack(stack);
            free(stack);
            HANDLE_ERROR("Syntax error.\n", SYNTAX_ERROR, SYNTAX_ERROR); // Syntax error
        }
    }

    // Verify successful parsing by checking the final stack state
    if (getStackLength(stack) == 2 && stack->top->tokenPtr->type == EXPR &&
        stack->top->next->tokenPtr->type == DOLLAR) {
        // Successful parsing
        exprAST->root = copyASTNode(stack->top->ASTNodePtr);
        freeToken(dollarToken);
        free(currentInputElement);
        cleanupStack(input);
        free(input);
        cleanupStack(stack);
        free(stack);
        return 0; // Indicate successful parsing
    } else {
        // Parsing incomplete or incorrect
        freeToken(dollarToken);
        free(currentInputElement);
        cleanupStack(input);
        free(input);
        cleanupStack(stack);
        free(stack);
        HANDLE_ERROR("Syntax error.", SYNTAX_ERROR, SYNTAX_ERROR); // Syntax error
    }
}