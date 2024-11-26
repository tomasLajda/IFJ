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
 * @param token Pointer to a Token structure where the next token will be stored.
 * @return int Returns TOKEN_OK on success, or an error code on failure.
 */
int getNextToken(Token *token);

/**
 * @brief Frees the memory allocated for a DynamicString and returns an error code.
 *
 * @param string Pointer to the DynamicString to be freed.
 * @param errorCode The error code to be returned.
 * @return int Returns the provided error code.
 */
int freeAndReturn(DynamicString *string, int errorCode);

/**
 * @brief Handles the processing of an identifier or keyword.
 *
 * @param string Pointer to the DynamicString containing the identifier or keyword.
 * @param token Pointer to a Token structure where the processed token will be stored.
 * @return Returns TOKEN_OK and calls freeAndReturn()
 */
int handleIdentifierOrKeyword(DynamicString *string, Token *token);

/**
 * @brief Checks if the type specified in the DynamicString is valid.
 *
 * @param string Pointer to the DynamicString containing the type to be checked.
 * @param token Pointer to a Token structure where the result will be stored.
 * @return int Returns TOKEN_OK  if the type is valid, or an LEXICAL_ERROR if invalid.
 */
int checkTypeValid(DynamicString *string, Token *token);

/**
 * @brief Formats a dynamic string for IFJcode printing.
 *
 * This function takes a DynamicString and converts it to a format suitable
 * for printing in IFJcode. It ensures that the string adheres to the
 * required formatting rules for IFJcode output.
 *
 * @param string Pointer to the DynamicString to be formatted.
 */
void stringFormatting(DynamicString *string);

#endif