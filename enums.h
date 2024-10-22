/*
 * IFJ Project
 * @brief This file contains all the enums used in the project
 *
 *@author Vojtěch Gajdušek - xgajduv00
 * @author Tomáš Lajda - xlajdat00
 *
 */

#ifndef ENUMS_H
#define ENUMS_H

// For the parts where we dont need all keywords
typedef enum {
    TYPE_I_32 = 0,
    TYPE_I_32_NULL,
    TYPE_F_64,
    TYPE_F_64_NULL,
    TYPE_U_8_ARRAY,
    TYPE_U_8_ARRAY_NULL,
    TYPE_VOID,
} DataType;

typedef enum {
    KEYWORD_I_32 = TYPE_I_32,
    KEYWORD_I_32_NULL = TYPE_I_32_NULL,
    KEYWORD_F_64 = TYPE_F_64,
    KEYWORD_F_64_NULL = TYPE_F_64_NULL,
    KEYWORD_U_8_ARRAY = TYPE_U_8_ARRAY,
    KEYWORD_U_8_ARRAY_NULL = TYPE_U_8_ARRAY_NULL,
    KEYWORD_VOID = TYPE_VOID,
    KEYWORD_NULL,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,
    KEYWORD_PUB,
    KEYWORD_FN,
    KEYWORD_RETURN,
    KEYWORD_VAR,
    KEYWORD_CONST,
    KEYWORD_UNDERSCORE,
    KEYWORD_MAIN,
    KEYWORD_IMPORT, // @import("ifj24.zig")
    KEYWORD_IFJ,
    KEYWORD_STRING, // These might be different -> ifj lib
    KEYWORD_LENGTH,
    KEYWORD_CONCAT,
    KEYWORD_SUBSTRING,
    KEYWORD_STRCMP,
    KEYWORD_ORD,
    KEYWORD_CHR,
    KEYWORD_WRITE,
    KEYWORD_READSTR,
    KEYWORD_READI32,
    KEYWORD_READF64,
    KEYWORD_I2F,
    KEYWORD_F2I,
} Keyword;

typedef enum {
    TOKEN_TYPE_EMPTY,   // Empty token
    TOKEN_TYPE_EOF,     // End of file
    TOKEN_TYPE_EOL,     // End of line
    TOKEN_TYPE_COMMENT, // Comment

    TOKEN_TYPE_IDENTIFIER, // Identifier
    TOKEN_TYPE_KEYWORD,    // Keyword
    TOKEN_TYPE_IMPORT,     // Import

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

#endif