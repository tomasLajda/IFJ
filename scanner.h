/*
IFJ project

@brief Scanner header file

@author Vojtěch Gajdušek - xgajduv00
*/

#ifndef _SCANNER_H
#define _SCANNER_H

#include <stdio.h>

typedef enum { KEYWORD_IF, KEYWORD_RETURN } Keyword;

typedef enum {
    TOKEN_TYPE_EMPTY, // Empty token
    TOKEN_TYPE_EOF,   // End of file
    TOKEN_TYPE_EOL,   // End of line

    TOKEN_TYPE_IDENTIFIER, // Identifier
    TOKEN_TYPE_KEYWORD,    // Keyword

    TOKEN_TYPE_INTEGER, // Number integer
    TOKEN_TYPE_DOUBLE,  // Number double
    TOKEN_TYPE_STRING,  // String

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

    TOKE_TYPE_OR,              // Or |
    TOKEN_TYPE_LEFT_BR,        // Left bracket (
    TOKEN_TYPE_RIGHT_BR,       // Right bracket )
    TOKEN_TYPE_LEFT_CURLY_BR,  // Left curly bracket {
    TOKEN_TYPE_RIGHT_CURLY_BR, // Right curly bracket }
    TOKEN_TYPE_COMMA,          // Comma ,
    TOKEN_TYPE_COLON,          // Colon :
    TOKEN_TYPE_SEMICOLON,      // Semicolon ;

} TokenType;

typedef union {
    int integer;
    double decimal;
} TokenAttribute;

typedef struct {
    TokenType type;
    TokenAttribute attribute;
} Token;

#endif