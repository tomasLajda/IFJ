/**
 * IFJ Project
 *
 * @brief Header file for the expression parser. Utilizing a shift-reduce parsing algorithm
 * with a static precedence table. Handles the parsing of expressions and constructs their AST
 * (Abstract Syntax Tree). Expressions can not include string values or function calls.
 *
 * @author Matúš Csirik - xcsirim00
 *
 **/

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
 * The expression can not include string values or function calls.
 *
 * Use firstToken and secondToken to pass the first and second token of the expression if they were
 * already "gotten" by getNextToken() before.
 *
 * @param exprAST Pointer to the AST structure to be filled with the expression. Must be
 * initialized. The root of the AST will be set to the root of the expression. If the expression is
 * empty, the root will be NULL.
 * @param firstToken Pointer to the optional first token of the expression.
 * @param secondToken Pointer to the optional second token of the expression.
 * @param delimiterToken Pointer to the token in which to store the delimiter of the expression.
 *
 * @note firstToken and secondToken are optional and can be NULL. They can be used independently.
 *
 * @return
 *   - 0 on successful parsing.
 *   - SYNTAX_ERROR (2) if a syntax error is encountered.
 *   - INTERNAL_ERROR (99) for internal failures (e.g., memory allocation issues).
 */
int parseExpression(AST *exprAST, Token *firstToken, Token *secondToken, Token *delimiterToken);

/**
 * @brief Precedence table for the shift-reduce parsing algorithm.
 *
 */
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

// Helper functions for the expression parser implementation below //

/**
 * @brief Retrieves the index of a given token type in the precedence table.
 *
 * @param type The token type for which the index is to be retrieved.
 * @return The index of the given token type in the table. -1 if the type is not found.
 */
int getTableIndex(TokenType type);

/**
 * @brief Checks if the given stack is reducible based on the next input element.
 *
 * @param stack A pointer to the stack to be checked for reduction.
 * @param nextInputEleemnt A pointer to the next element to be processed.
 * @return An integer indicating whether the stack is reducible (1), not (0), SYNTAX_ERROR (2),
 * or INTERNAL_ERROR (99).
 */
int isReducible(Stack *stack, StackElement *nextInputElement);

/**
 * @brief Chooses the appropriate reduction rule based on the current state of the stack. Use only
 * if isReducible() returns 0.
 *
 * @param stack Pointer to the current parsing stack.
 * @return Specific reduction rule identifier (e.g., EXPR_ADD) if a rule applies.  -1 if no
 * applicable reduction rule is found.
 */
int chooseReduceRule(Stack *stack);

/**
 * @brief Fills the input stack with tokens up to a delimiter. Uses getNextToken() to get tokens.
 *
 * @param stack Pointer to the stack to be filled with tokens.
 * @param firstToken Pointer to the first token of the expression if it was already "gotten" before.
 * @param secondToken Pointer to the second token of the expression if it was already "gotten"
 * before along with the first one.
 * @param delimiterToken Pointer to a token that will store the encountered delimiter token.
 *
 * @return
 *   - Pointer to the filled destination stack if successful.
 *   - NULL if a syntax error occurs or memory allocation fails.
 */
Stack *fillInputStack(Stack *stack, Token *firstToken, Token *secondToken, Token *delimiterToken);

#endif // EXPR_PARSER_H
