/*
 * IFJ Project
 * @brief Header file for dynamic string
 *
 * @author Tomáš Lajda - xlajdat00
 * @author Matúš Csirik - xcsirim00
 * @author Vojtěch Gajdušek - xgajduv00
 *
 */

#include "helpers.h"
#include "enums.h"
#include "error_codes.h"
#include "scanner.h"
#include <stdio.h>

char *TokenTypeToString(TokenType type) {
    switch (type) {
    case TOKEN_TYPE_EMPTY:
        return "EMPTY";
    case TOKEN_TYPE_EOF:
        return "EOF";
    case TOKEN_TYPE_EOL:
        return "EOL";
    case TOKEN_TYPE_COMMENT:
        return "COMMENT";
    case TOKEN_TYPE_IDENTIFIER:
        return "ID";
    case TOKEN_TYPE_KEYWORD:
        return "KEYWORD";
    case TOKEN_TYPE_INTEGER_VALUE:
        return "INTEGER_VALUE";
    case TOKEN_TYPE_DOUBLE_VALUE:
        return "DOUBLE_VALUE";
    case TOKEN_TYPE_STRING_VALUE:
        return "STRING_VALUE";
    case TOKEN_TYPE_EQ:
        return "==";
    case TOKEN_TYPE_NEQ:
        return "!=";
    case TOKEN_TYPE_LTH:
        return "<";
    case TOKEN_TYPE_LEQ:
        return "<=";
    case TOKEN_TYPE_GTH:
        return ">";
    case TOKEN_TYPE_GEQ:
        return ">=";
    case TOKEN_TYPE_ASSIGN:
        return "=";
    case TOKEN_TYPE_PLUS:
        return "+";
    case TOKEN_TYPE_MINUS:
        return "-";
    case TOKEN_TYPE_MUL:
        return "*";
    case TOKEN_TYPE_DIV:
        return "/";
    case TOKEN_TYPE_VB:
        return "|";
    case TOKEN_TYPE_LEFT_BR:
        return "(";
    case TOKEN_TYPE_RIGHT_BR:
        return ")";
    case TOKEN_TYPE_LEFT_CURLY_BR:
        return "{";
    case TOKEN_TYPE_RIGHT_CURLY_BR:
        return "}";
    case TOKEN_TYPE_COMMA:
        return ",";
    case TOKEN_TYPE_COLON:
        return ":";
    case TOKEN_TYPE_SEMICOLON:
        return ";";
    case TOKEN_TYPE_DOT:
        return ".";
    case TOKEN_TYPE_EXPR:
        return "EXPR";
    case TOKEN_TYPE_DOLLA:
        return "$";
    case TOKEN_TYPE_NULL_COND:
        return "NULL_COND";
    default:
        return "UNKNOWN_TOKEN_TYPE";
    }
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
    if (token == NULL) {
        return NULL;
    }
    Token *newToken = (Token *)malloc(sizeof(Token));
    if (newToken == NULL) {
        HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, NULL);
    }
    newToken->type = token->type;
    newToken->line = token->line;

    // Deep copy attribute based on its type
    switch (token->type) {
    case TOKEN_TYPE_IDENTIFIER:
    case TOKEN_TYPE_STRING_VALUE:
        if (token->attribute.string != NULL) {
            newToken->attribute.string = (char *)malloc(strlen(token->attribute.string) + 1);
            if (newToken->attribute.string == NULL) {
                HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, NULL);
            }
            strcpy(newToken->attribute.string, token->attribute.string);
        } else {
            newToken->attribute.string = NULL;
        }
        break;
    case TOKEN_TYPE_INTEGER_VALUE:
        newToken->attribute.integer = token->attribute.integer;
        break;
    case TOKEN_TYPE_DOUBLE_VALUE:
        newToken->attribute.decimal = token->attribute.decimal;
        break;
    case TOKEN_TYPE_KEYWORD:
        newToken->attribute.keyword = token->attribute.keyword;
        break;
    default:
        // For token types without attributes
        break;
    }

    return newToken;
}

ASTNode *copyASTNode(ASTNode *node) {
    if (node == NULL) {
        return NULL;
    }
    ASTNode *newNode = initASTNode();
    newNode->token = copyToken(node->token);
    newNode->parent = node->parent;

    // Copy left child and set parent
    if (node->left != NULL) {
        newNode->left = copyASTNode(node->left);
    }

    // Copy right child and set parent
    if (node->right != NULL) {
        newNode->right = copyASTNode(node->right);
    }

    return newNode;
}

StackElement *createStackElement(Token *token, ASTNode *astNodePtr) {
    StackElement *element = (StackElement *)malloc(sizeof(StackElement));
    if (element == NULL) {
        HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, NULL);
    }
    element->ASTNodePtr = astNodePtr;
    element->tokenPtr = token;
    element->next = NULL;
    return element;
}

void freeToken(Token *token) {
    if (token == NULL) {
        return;
    }
    if (token->type == TOKEN_TYPE_IDENTIFIER || token->type == TOKEN_TYPE_STRING_VALUE) {
        free(token->attribute.string);
        token->attribute.string = NULL;
    }
    free(token);
}

bool isTokenTypeOperator(TokenType type) {
    switch (type) {
    case TOKEN_TYPE_EQ:
    case TOKEN_TYPE_NEQ:
    case TOKEN_TYPE_LTH:
    case TOKEN_TYPE_LEQ:
    case TOKEN_TYPE_GTH:
    case TOKEN_TYPE_GEQ:
    case TOKEN_TYPE_PLUS:
    case TOKEN_TYPE_MINUS:
    case TOKEN_TYPE_MUL:
    case TOKEN_TYPE_DIV:
        return true;
    default:
        return false;
    }
}
