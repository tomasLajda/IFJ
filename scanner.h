/**
 * IFJ project
 *
 * @brief Scanner header file
 *
 * @author Vojtěch Gajdušek - xgajduv00
 */

#ifndef _SCANNER_H
#define _SCANNER_H

#include <stdio.h>

#include "dynamic_string.h"
#include "enums.h"

typedef union {
    long long int integer;
    double decimal;
    char *string;
    Keyword keyword;
    void *noAttribute;
} TokenAttribute;

typedef struct {
    TokenType type;
    TokenAttribute attribute;
    unsigned line;
} Token;

/**
 * @brief Retrieves the next token from the input stream.
 *
 * Implements the scanner's main logic as a state machine to analyze the
 * input and generate a token. Handles various token types such as keywords,
 * identifiers, literals, operators, and special characters.
 *
 * @param token Pointer to a Token structure where the next token will be stored.
 * @return int Returns TOKEN_OK on success, or an error code on failure.
 */
int getNextToken(Token *token);

/**
 * @brief Frees a dynamic string and returns the specified error code.
 *
 * Cleans up the memory allocated for the dynamic string and ensures
 * proper error handling by propagating the provided error code.
 *
 * @param string Pointer to the dynamic string to free.
 * @param errorCode Error code to return after freeing the string.
 * @return The provided error code.
 */
int freeAndReturn(DynamicString *string, int errorCode);

/**
 * @brief Determines if a string represents a keyword or identifier.
 *
 * Checks the provided string against a list of keywords. If a match is
 * found, assigns the appropriate token attributes. Otherwise, treats the
 * string as an identifier.
 *
 * @param string Pointer to the dynamic string containing the token text.
 * @param token Pointer to the token to classify.
 * @return TOKEN_OK on success.
 */
int handleIdentifierOrKeyword(DynamicString *string, Token *token);

/**
 * @brief Validates and assigns a type keyword to a token.
 *
 * Checks if the provided string matches a valid type keyword and assigns
 * the corresponding token attributes. If the type is invalid, returns a
 * lexical error.
 *
 * @param string Pointer to the dynamic string containing the type.
 * @param token Pointer to the token to assign the type attributes.
 * @return TOKEN_OK on success, or LEXICAL_ERROR on invalid type.
 */
int checkTypeValid(DynamicString *string, Token *token);

/**
 * @brief Formats a string for safe storage in IFJcode24.
 *
 * Replaces specific characters in the dynamic string with their escape
 * sequence equivalents to ensure compatibility with IFJcode24.
 *
 * @param string Pointer to the dynamic string to format.
 */
void stringFormatting(DynamicString *string);

#endif