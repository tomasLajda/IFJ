/*
 * IFJ Project
 * @brief Header file for dynamic string
 *
 * @author Tomáš Lajda - xlajdat00
 * @author Matúš Csirik - xcsirim00
 *
 */

#ifndef HELPERS_H

#include "ast.h"
#include "dynamic_string.h"
#include "enums.h"
#include "scanner.h"
#include "stack.h"

/**
 * @brief Function that transforms the token type integer to the corresponding character (string)
 */
char *TokenTypeToString(TokenType type);

/**
 * @brief Function that creates a new token with the given type
 */
Token *createToken(TokenType type);

/**
 * @brief Function that creates a deep copy of the given token
 */
Token *copyToken(Token *token);

/**
 * @brief Function that creates a deep copy of the given ASTNode
 */
ASTNode *copyASTNode(ASTNode *node);

/**
 * @brief Function that creates a new stack element with the given token and ASTNode
 */
StackElement *createStackElement(Token *token, ASTNode *astNodePtr);

#endif // HELPERS_H