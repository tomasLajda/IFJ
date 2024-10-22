/*
IFJ project

@brief Scanner header file

@author Vojtěch Gajdušek - xgajduv00
*/

#ifndef _SCANNER_H
#define _SCANNER_H

#include <stdio.h>

#include "dynamic_string.h"
#include "enums.h"

typedef enum {
    TOKEN_TYPE_EMPTY,   // Empty token
    TOKEN_TYPE_EOF,     // End of file
    TOKEN_TYPE_EOL,     // End of line
    TOKEN_TYPE_COMMENT, // Comment

    TOKEN_TYPE_IDENTIFIER, // Identifier
    TOKEN_TYPE_KEYWORD,    // Keyword

    // VALUES
    TOKEN_TYPE_INTEGER_VALUE, // Number integer
    TOKEN_TYPE_DOUBLE_VALUE,  // Number double
    TOKEN_TYPE_STRING_VALUE,  // String

    // COMPARE
    TOKEN_TYPE_EQ,  // Equal ==
    TOKEN_TYPE_NEQ, // Not equal !=
    TOKEN_TYPE_LTH, // Less than <
    TOKEN_TYPE_LEQ, // Less or equal <=
    TOKEN_TYPE_GTH, // Greater than >
    TOKEN_TYPE_GEQ, // Greater or equal >=

    // OPERATORS
    TOKEN_TYPE_ASSIGN, // Assign =
    TOKEN_TYPE_PLUS,   // Plus +
    TOKEN_TYPE_MINUS,  // Minus -
    TOKEN_TYPE_MUL,    // Multiplication *
    TOKEN_TYPE_DIV,    // Division /

    TOKEN_TYPE_OR,             // Or |
    TOKEN_TYPE_LEFT_BR,        // Left bracket (
    TOKEN_TYPE_RIGHT_BR,       // Right bracket )
    TOKEN_TYPE_LEFT_CURLY_BR,  // Left curly bracket {
    TOKEN_TYPE_RIGHT_CURLY_BR, // Right curly bracket }
    TOKEN_TYPE_COMMA,          // Comma ,
    TOKEN_TYPE_COLON,          // Colon :
    TOKEN_TYPE_SEMICOLON,      // Semicolon ;
    TOKEN_TYPE_DOT,            // Dot .
    TOKEN_TYPE_AT,             // At @
} TokenType;

typedef union {
    int integer;
    double decimal;
    char *string;
    Keyword keyword;
} TokenAttribute;

typedef struct {
    TokenType type;
    TokenAttribute attribute;
    unsigned line;
} Token;

int getNextToken(Token *token);

#endif