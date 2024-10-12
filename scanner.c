/*
IFJ Project

@brief Implementation of scanner

@author Vojtěch Gajdušek - xgajduv00
*/

#include "scanner.h"
#include "error_codes.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

// DEFINE SCANNER STATES
#define STATE_START 0 // Start state - default

// IMPORT
#define STATE_IMPORT 1 // State for @import keyword, that has to be in every file

// OPERATORS
#define STATE_LESS 2      // Starts with '<', can be '<='
#define STATE_MORE 3      // Starts with '>', can be '>='
#define STATE_EQUAL 4     // Starts with '=', can be '=='
#define STATE_EXCL_MARK 5 // Starts with '!', has to continue with '=' for NOT_EQUAL, else error

// IDENTIFIER
#define STATE_IDENTIFIER_OR_KEYWORD                                                                \
    7 // Starts with letter or '_', returns string as IDENTIFIER or KEYWORD

// TYPE
#define STATE_OPENING_SQUARE_BRAC 9  // '[' was loaded after '?', ']' has to come next
#define STATE_CLOSING_SQUARE_BRAC 10 // ']' was loaded
#define STATE_TYPE 11                // State to clarify type after either '[]', '?' or '?[]'

// NUMBER
#define STATE_NUMBER                                                                               \
    12 // Loads a number (1-9) and either return INT or if dot comes next continues to STATE_DOT, or
       // if (e/E) is loaded, goes to STATE_EXPONENT
#define STATE_ZERO 23 // Loads '0' (zero can't be int, only 0.smth)
#define STATE_DOT 13  // '.' was loaded after a number, loads next number, goes to STATE_FLOAT
#define STATE_FLOAT                                                                                \
    14 // Loads numbers and returns FLOAT or IF (e/E) is loaded goes to STATE_EXPONENT
#define STATE_EXPONENT                                                                             \
    15 // Loads number or sign (+/-) and goes to STATE_EXP_NUMBER or STATE_EXP_SIGN
#define STATE_EXP_SIGN 16 // Sign for exponent was loaded, loads next number
#define STATE_EXP_NUMBER                                                                           \
    17 // Loads next number(s) and returns number with exp (can be float with exp)

// STRING
#define STATE_READ_STRING 18 // '"' was loaded, reads ASCII > 31
#define STATE_BACKSLASH                                                                            \
    19                  // State for special characters after '\' ('\n', '\t', etc.) -> goes back to
                        // STATE_READ_STRING, or '\x' to STATE HEXA0
#define STATE_HEXA0 20  // '\x' was read, reads first part of the hexa number, goes to STATE_HEXA1
#define STATE_HEXA1 21  // reads second part of the hexa number and goes back to STATE_READ_STRING
#define STATE_STRING 22 // Second '"' was loaded, string ends

FILE *sourceFile; // Source file to be used as input for scanner

int getNextToken(Token *token) {

    if (sourceFile == NULL) {
        return INTERNAL_ERROR;
    }

    int state = STATE_START;
    token->type = TOKEN_TYPE_EMPTY;

    while (true) {

        char current = (char)getc(sourceFile);

        switch (state) {
        case STATE_START:
            // WHITESPACE CHARACTER
            if (isspace(current)) {
                state = STATE_START;
            } else if (current == EOF) {
                token->type = TOKEN_TYPE_EOF;
                // TODO: RETURN, FREE
                return 1;
            } else {
                // TODO: RETURN, FREE
                return 1;
            }

            break;

        default:
            break;
        }
    }
}