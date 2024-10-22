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
#define STATE_DIVISION 24 // Starts with '/', can be comment '//'
#define STATE_COMMENT 25  // Comment '//'

// IDENTIFIER
#define STATE_IDENTIFIER_OR_KEYWORD                                                                \
    7 // Starts with letter or '_', returns string as IDENTIFIER or KEYWORD

// TYPE
#define STATE_OPENING_SQUARE_BRAC 9  // '[' was loaded after '?', ']' has to come next
#define STATE_CLOSING_SQUARE_BRAC 10 // ']' was loaded
#define STATE_TYPE 11                // State to clarify type after either '[]', '?' or '?[]'

// NUMBER
#define STATE_NUMBER                                                                               \
    12 // Loads a number (1-9) and either return INT or if dot comes next continues to
       // STATE_NUMBER_DOT, or if (e/E) is loaded, goes to STATE_EXPONENT
#define STATE_ZERO 23       // Loads '0' (zero can't be int, only 0.smth)
#define STATE_NUMBER_DOT 13 // '.' was loaded after a number, loads next number, goes to STATE_FLOAT
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

    DynamicString buffer;
    dynamicStringInit(&buffer);

    if (sourceFile == NULL) {
        return INTERNAL_ERROR;
    }

    int state = STATE_START;
    token->type = TOKEN_TYPE_EMPTY;

    while (true) {

        char current = (char)getc(sourceFile);

        switch (state) {

        // START
        case STATE_START:

            // WHITESPACE CHARACTER
            if (isspace(current)) {
                state = STATE_START;
            }
            // END OF FILE
            else if (current == EOF) {
                token->type = TOKEN_TYPE_EOF;
                // TODO: RETURN, DYNAMIC STRING + JEHO FREE
                return TOKEN_OK;
            } else if (current == '+') {
                token->type = TOKEN_TYPE_PLUS;
                return TOKEN_OK;
            } else if (current == '-') {
                token->type = TOKEN_TYPE_MINUS;
                return TOKEN_OK;
            } else if (current == '*') {
                token->type = TOKEN_TYPE_MUL;
                return TOKEN_OK;
            } else if (current == ';') {
                token->type = TOKEN_TYPE_SEMICOLON;
                return TOKEN_OK;
            }
            // > or >=
            else if (current == '>') {
                state = STATE_MORE;
            }
            // < or <=
            else if (current == '<') {
                state = STATE_LESS;
            }
            // / - DIVISION or COMMENT
            else if (current == '/') {
                state = STATE_DIVISION;
            }
            // = can be OPERATOR OR ==
            else if (current == '=') {
                state = STATE_EQUAL;
            } else if (current == '!') {
                state = STATE_EXCL_MARK;
            } else if (current == '.') {
                token->type = TOKEN_TYPE_DOT;
                return TOKEN_OK;
            } else if (current == '{') {
                token->type = TOKEN_TYPE_LEFT_CURLY_BR;
                return TOKEN_OK;
            } else if (current == '}') {
                token->type = TOKEN_TYPE_RIGHT_CURLY_BR;
                return TOKEN_OK;
            } else if (current == '(') {
                token->type = TOKEN_TYPE_LEFT_BR;
                return TOKEN_OK;
            } else if (current == ')') {
                token->type = TOKEN_TYPE_RIGHT_BR;
                return TOKEN_OK;
            } else if (current == ':') {
                token->type = TOKEN_TYPE_COLON;
                return TOKEN_OK;
            }
            // STRING
            else if (current == '"') {
                state = STATE_READ_STRING;
            }
            // IMPORT
            else if (current == '@') {
                dynamicStringAddChar(&buffer, current);
                state = STATE_IMPORT;
            }
            // TYPE - starting with ?
            else if (current == '?') {
                dynamicStringAddChar(&buffer, current);
                state = STATE_TYPE;
            }
            // TYPE - starting with [
            else if (current == '[') {
                dynamicStringAddChar(&buffer, current);
                state = STATE_OPENING_SQUARE_BRAC;
            } else if (current == '0') {
                dynamicStringAddChar(&buffer, current);
                state = STATE_ZERO;
            }
            // NUMBER
            else if (isdigit(current) && current != '0') {
                dynamicStringAddChar(&buffer, current);
                state = STATE_NUMBER;
            }
            // IDENTIFIER OR KEYWORD
            else if (isalpha(current) || current == '_') {
                dynamicStringAddChar(&buffer, current);
                state = STATE_IDENTIFIER_OR_KEYWORD;
            }
            // else
            else {
                // TODO: LEXICAL ERROR / INTERNAL ERROR
                dynamicStringFree(&buffer);
                return LEXICAL_ERROR;
            }

            break;

        // IMPORT - @import("ifj24.zig")
        case STATE_IMPORT:
            if (isalnum(current) || current == '"' || current == '.' || current == '(' ||
                current == ')') {
                dynamicStringAddChar(&buffer, current);
            } else {
                ungetc(current, sourceFile);
                if (dynamicStringCompare(&buffer, "@import(\"ifj24.zig\")")) {
                    token->type = TOKEN_TYPE_KEYWORD;
                    token->attribute.keyword = KEYWORD_IMPORT;
                    dynamicStringFree(&buffer);
                    return TOKEN_OK;
                } else {
                    dynamicStringFree(&buffer);
                    return LEXICAL_ERROR;
                }
            }

            break;

        // NUMBER
        case STATE_NUMBER:
            if (isdigit(current)) {
                dynamicStringAddChar(&buffer, current);
            } else if (current == '.') {
                dynamicStringAddChar(&buffer, current);
                token->type = TOKEN_TYPE_DOUBLE_VALUE;
                state = STATE_NUMBER_DOT;
            } else if (current == 'e' || current == 'E') {
                dynamicStringAddChar(&buffer, current);
                token->type = TOKEN_TYPE_INTEGER_VALUE;
                state = STATE_EXPONENT;
            } else {
                ungetc(current, sourceFile);
                token->type = TOKEN_TYPE_INTEGER_VALUE;
                token->attribute.integer = atoi(dynamicStringToCString(&buffer));
                dynamicStringFree(&buffer);
                return TOKEN_OK;
            }

            break;

        case STATE_ZERO:
            if (current == '.') {
                dynamicStringAddChar(&buffer, current);
                token->type = TOKEN_TYPE_DOUBLE_VALUE;
                state = STATE_NUMBER_DOT;
            } else {
                ungetc(current, sourceFile);
                token->type = TOKEN_TYPE_INTEGER_VALUE;
                token->attribute.integer = 0;
                dynamicStringFree(&buffer);
                return TOKEN_OK;
            }
            break;

        case STATE_NUMBER_DOT:
            if (isdigit(current)) {
                dynamicStringAddChar(&buffer, current);
                state = STATE_FLOAT;
            } else {
                ungetc(current, sourceFile);
                dynamicStringFree(&buffer);
                return LEXICAL_ERROR;
            }
            break;

        case STATE_FLOAT:
            if (isdigit(current)) {
                dynamicStringAddChar(&buffer, current);
            } else if (current == 'e' || current == 'E') {
                dynamicStringAddChar(&buffer, current);
                state = STATE_EXPONENT;
            } else {
                ungetc(current, sourceFile);
                dynamicStringFree(&buffer);
                return LEXICAL_ERROR;
            }
            break;

        case STATE_EXPONENT:
            if (isdigit(current)) {
                dynamicStringAddChar(&buffer, current);
                state = STATE_EXP_NUMBER;
            } else if (current == '+' || current == '-') {
                dynamicStringAddChar(&buffer, current);
                state = STATE_EXP_SIGN;
            } else {
                ungetc(current, sourceFile);
                dynamicStringFree(&buffer);
                return LEXICAL_ERROR;
            }
            break;

        case STATE_EXP_SIGN:
            if (isdigit(current)) {
                dynamicStringAddChar(&buffer, current);
                state = STATE_EXP_NUMBER;
            } else {
                ungetc(current, sourceFile);
                dynamicStringFree(&buffer);
                return LEXICAL_ERROR;
            }
            break;

        case STATE_EXP_NUMBER:
            if (isdigit(current)) {
                dynamicStringAddChar(&buffer, current);
            } else {
                ungetc(current, sourceFile);
                dynamicStringFree(&buffer);
                return LEXICAL_ERROR;
            }
            break;

        // IDENTIFIER / KEYWORD
        case STATE_IDENTIFIER_OR_KEYWORD:
            if (isalpha(current) || isdigit(current) || current == '_') {
                state = STATE_IDENTIFIER_OR_KEYWORD;
                DynamicStringAddChar(&token->attribute.string, current);
            } else {

                // TODO: token_ok? function for token type - identifier/keyword
                ungetc(current, sourceFile);
                return LEXICAL_ERROR;
            }

            break;

        // STRING
        case STATE_READ_STRING:
            break;

        case STATE_BACKSLASH:
            break;

        case STATE_HEXA0:
            break;

        case STATE_HEXA1:
            break;

        case STATE_STRING:
            break;

        // COMMENT
        case STATE_COMMENT:
            if (current == '\n' || current == EOF) {
                state = STATE_START;
                ungetc(current, sourceFile);
            }
            break;

        // LESS
        case STATE_LESS:
            if (current == '=') {
                token->type = TOKEN_TYPE_LEQ;
            } else {
                ungetc(current, sourceFile);
                token->type = TOKEN_TYPE_LTH;
            }
            return TOKEN_OK;

        // MORE
        case STATE_MORE:
            if (current == '=') {
                token->type = TOKEN_TYPE_GEQ;
            } else {
                ungetc(current, sourceFile);
                token->type = TOKEN_TYPE_GTH;
            }
            return TOKEN_OK;

        // EQUAL
        case STATE_EQUAL:
            if (current == '=') {
                token->type = TOKEN_TYPE_EQ;
            } else {
                token->type = TOKEN_TYPE_ASSIGN;
            }
            return TOKEN_OK;

        // EXCL MARK !
        case STATE_EXCL_MARK:
            if (current == '=') {
                token->type = TOKEN_TYPE_NEQ;
                return TOKEN_OK;
            } else {
                // TODO: ungetc ?
                return LEXICAL_ERROR;
            }

            break;

        // DIVISION
        case STATE_DIVISION:
            if (current == '/') {
                state = STATE_COMMENT;
            } else {
                token->type = TOKEN_TYPE_DIV;
                ungetc(current, sourceFile);
                return TOKEN_OK;
            }

            break;

        // TYPE
        case STATE_TYPE:
            break;

        case STATE_OPENING_SQUARE_BRAC:
            break;

        case STATE_CLOSING_SQUARE_BRAC:
            break;

        default:
            break;
        }
    }
}