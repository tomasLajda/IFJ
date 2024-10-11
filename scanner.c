/*
IFJ Project

@brief Implementation of scanner

@author Vojtěch Gajdušek - xgajduv00
*/

#include "scanner.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

// Scanner states
#define STATE_START 0 // Start state - default

FILE *sourceFile; // Source file to be used as input for scanner

int getNextToken(Token *token) {

    int state = STATE_START;
    token->type = TOKEN_TYPE_EMPTY;

    while (true) {
        switch (state) {
        case /* constant-expression */:
            /* code */
            break;

        default:
            break;
        }
    }
}