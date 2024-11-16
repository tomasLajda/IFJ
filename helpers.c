/*
 * IFJ Project
 * @brief Header file for dynamic string
 *
 * @author Tomáš Lajda - xlajdat00
 * @author Matúš Csirik - xcsirim00
 *
 */

#include "helpers.h"
#include "enums.h"
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
    case TOKEN_TYPE_OR:
        return "OR";
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
    default:
        return "UNKNOWN_TOKEN_TYPE";
    }
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
