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

// Scanner states
#define STATE_START 0 // Start state - default

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