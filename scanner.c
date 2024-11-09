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

int freeAndReturn(DynamicString *string, int errorCode) {
    dynamicStringFree(string);
    return errorCode;
}

int checkTypeValid(DynamicString *string, Token *token) {
    if (dynamicStringCompare(string, "?i32")) {
        token->type = TOKEN_TYPE_KEYWORD;
        token->attribute.keyword = KEYWORD_I_32_NULL;
    } else if (dynamicStringCompare(string, "?f64")) {
        token->type = TOKEN_TYPE_KEYWORD;
        token->attribute.keyword = KEYWORD_F_64_NULL;
    } else if (dynamicStringCompare(string, "[]u8")) {
        token->type = TOKEN_TYPE_KEYWORD;
        token->attribute.keyword = KEYWORD_U_8_ARRAY;
    } else if (dynamicStringCompare(string, "?[]u8")) {
        token->type = TOKEN_TYPE_KEYWORD;
        token->attribute.keyword = KEYWORD_U_8_ARRAY_NULL;
    } else {
        return LEXICAL_ERROR;
    }
    return TOKEN_OK;
}

int handleIdentifierOrKeyword(DynamicString *string, Token *token) {

    // TODO: check if all keywords are here or if there are some extra or missing
    if (dynamicStringCompare(string, "i32"))
        token->attribute.keyword = KEYWORD_I_32;
    else if (dynamicStringCompare(string, "f64"))
        token->attribute.keyword = KEYWORD_F_64;
    else if (dynamicStringCompare(string, "u8_array"))
        token->attribute.keyword = KEYWORD_U_8_ARRAY;
    else if (dynamicStringCompare(string, "void"))
        token->attribute.keyword = KEYWORD_VOID;
    else if (dynamicStringCompare(string, "null"))
        token->attribute.keyword = KEYWORD_NULL;
    else if (dynamicStringCompare(string, "if"))
        token->attribute.keyword = KEYWORD_IF;
    else if (dynamicStringCompare(string, "else"))
        token->attribute.keyword = KEYWORD_ELSE;
    else if (dynamicStringCompare(string, "while"))
        token->attribute.keyword = KEYWORD_WHILE;
    else if (dynamicStringCompare(string, "pub"))
        token->attribute.keyword = KEYWORD_PUB;
    else if (dynamicStringCompare(string, "fn"))
        token->attribute.keyword = KEYWORD_FN;
    else if (dynamicStringCompare(string, "return"))
        token->attribute.keyword = KEYWORD_RETURN;
    else if (dynamicStringCompare(string, "var"))
        token->attribute.keyword = KEYWORD_VAR;
    else if (dynamicStringCompare(string, "const"))
        token->attribute.keyword = KEYWORD_CONST;
    else if (dynamicStringCompare(string, "_"))
        token->attribute.keyword = KEYWORD_UNDERSCORE;
    else if (dynamicStringCompare(string, "main"))
        token->attribute.keyword = KEYWORD_MAIN;
    else if (dynamicStringCompare(string, "ifj"))
        token->attribute.keyword = KEYWORD_IFJ;
    else if (dynamicStringCompare(string, "string"))
        token->attribute.keyword = KEYWORD_STRING;
    else if (dynamicStringCompare(string, "length"))
        token->attribute.keyword = KEYWORD_LENGTH;
    else if (dynamicStringCompare(string, "concat"))
        token->attribute.keyword = KEYWORD_CONCAT;
    else if (dynamicStringCompare(string, "substring"))
        token->attribute.keyword = KEYWORD_SUBSTRING;
    else if (dynamicStringCompare(string, "strcmp"))
        token->attribute.keyword = KEYWORD_STRCMP;
    else if (dynamicStringCompare(string, "ord"))
        token->attribute.keyword = KEYWORD_ORD;
    else if (dynamicStringCompare(string, "chr"))
        token->attribute.keyword = KEYWORD_CHR;
    else if (dynamicStringCompare(string, "write"))
        token->attribute.keyword = KEYWORD_WRITE;
    else if (dynamicStringCompare(string, "readstr"))
        token->attribute.keyword = KEYWORD_READSTR;
    else if (dynamicStringCompare(string, "readi32"))
        token->attribute.keyword = KEYWORD_READI32;
    else if (dynamicStringCompare(string, "readf64"))
        token->attribute.keyword = KEYWORD_READF64;
    else if (dynamicStringCompare(string, "i2f"))
        token->attribute.keyword = KEYWORD_I2F;
    else if (dynamicStringCompare(string, "f2i"))
        token->attribute.keyword = KEYWORD_F2I;
    else
        token->type = TOKEN_TYPE_IDENTIFIER;

    if (token->type == TOKEN_TYPE_IDENTIFIER) {
        // TODO: dynamicStringCopy(&string, &token->attribute.string) ?
        token->attribute.string = dynamicStringToCString(string);
    } else {
        token->type = TOKEN_TYPE_KEYWORD;
    }

    return freeAndReturn(string, TOKEN_OK);
}

int getNextToken(Token *token) {

    DynamicString buffer;
    dynamicStringInit(&buffer);

    if (sourceFile == NULL) {
        return INTERNAL_ERROR;
    }

    int state = STATE_START;
    token->type = TOKEN_TYPE_EMPTY;

    while (true) {

        int current = getc(sourceFile);

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
                return freeAndReturn(&buffer, TOKEN_OK);
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
                token->type = TOKEN_TYPE_STRING_VALUE;
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
                return freeAndReturn(&buffer, LEXICAL_ERROR);
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
                    return freeAndReturn(&buffer, TOKEN_OK);
                } else {
                    return freeAndReturn(&buffer, LEXICAL_ERROR);
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
                token->attribute.integer = (int)strtod((dynamicStringToCString(&buffer)), NULL);
                return freeAndReturn(&buffer, TOKEN_OK);
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
                return freeAndReturn(&buffer, TOKEN_OK);
            }
            break;

        case STATE_NUMBER_DOT:
            if (isdigit(current)) {
                dynamicStringAddChar(&buffer, current);
                state = STATE_FLOAT;
            } else {
                ungetc(current, sourceFile);
                return freeAndReturn(&buffer, LEXICAL_ERROR);
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
                token->attribute.decimal = atof(dynamicStringToCString(&buffer));
                return freeAndReturn(&buffer, LEXICAL_ERROR);
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
                return freeAndReturn(&buffer, LEXICAL_ERROR);
            }
            break;

        case STATE_EXP_SIGN:
            if (isdigit(current)) {
                dynamicStringAddChar(&buffer, current);
                state = STATE_EXP_NUMBER;
            } else {
                ungetc(current, sourceFile);
                return freeAndReturn(&buffer, LEXICAL_ERROR);
            }
            break;

        case STATE_EXP_NUMBER:
            if (isdigit(current)) {
                dynamicStringAddChar(&buffer, current);
            } else {
                ungetc(current, sourceFile);
                if (token->type == TOKEN_TYPE_INTEGER_VALUE) {
                    token->attribute.integer = (int)strtod((dynamicStringToCString(&buffer)), NULL);
                    return freeAndReturn(&buffer, TOKEN_OK);
                } else if (token->type == TOKEN_TYPE_DOUBLE_VALUE) {
                    token->attribute.decimal = strtof((dynamicStringToCString(&buffer)), NULL);
                    return freeAndReturn(&buffer, TOKEN_OK);
                } else {
                    return freeAndReturn(&buffer, INTERNAL_ERROR);
                }
            }
            break;

        // IDENTIFIER / KEYWORD
        case STATE_IDENTIFIER_OR_KEYWORD:
            if (isalpha(current) || isdigit(current) || current == '_') {
                state = STATE_IDENTIFIER_OR_KEYWORD;
                dynamicStringAddChar(&buffer, current);
            } else {
                ungetc(current, sourceFile);
                return handleIdentifierOrKeyword(&buffer, token);
            }

            break;

        // STRING
        case STATE_READ_STRING:
            if (current <= 31) {
                return freeAndReturn(&buffer, LEXICAL_ERROR);
            } else if (current == '\\') {
                state = STATE_BACKSLASH;
            } else if (current == '"') {
                state = STATE_STRING;
            } else {
                dynamicStringAddChar(&buffer, current);
            }
            break;

        case STATE_BACKSLASH:
            if (current == 'n') {
                dynamicStringAddChar(&buffer, '\n');
                state = STATE_READ_STRING;
            } else if (current == 't') {
                dynamicStringAddChar(&buffer, '\t');
                state = STATE_READ_STRING;
            } else if (current == '"') {
                dynamicStringAddChar(&buffer, '\"');
                state = STATE_READ_STRING;
            } else if (current == '\\') {
                dynamicStringAddChar(&buffer, '\\');
                state = STATE_READ_STRING;
            } else if (current == 'r') {
                dynamicStringAddChar(&buffer, '\r');
                state = STATE_READ_STRING;
            } else if (current == 'x') {
                state = STATE_HEXA0;
            } else {
                return freeAndReturn(&buffer, LEXICAL_ERROR);
            }
            break;

        case STATE_HEXA0:
            if (isxdigit(current)) {
                dynamicStringAddChar(&buffer, current);
                state = STATE_HEXA1;
            } else {
                return freeAndReturn(&buffer, LEXICAL_ERROR);
            }
            break;

        case STATE_HEXA1:
            if (isxdigit(current)) {
                dynamicStringAddChar(&buffer, current);
                state = STATE_READ_STRING;
            } else {
                return freeAndReturn(&buffer, LEXICAL_ERROR);
            }
            break;

        case STATE_STRING:
            ungetc(current, sourceFile);
            token->attribute.string = dynamicStringToCString(&buffer);
            return freeAndReturn(&buffer, TOKEN_OK);

        // COMMENT
        case STATE_COMMENT:
            if (current == '\n' || current == EOF) {
                ungetc(current, sourceFile);
                state = STATE_START;
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
                ungetc(current, sourceFile);
                return LEXICAL_ERROR;
            }

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
            if (current == '[') {
                state = STATE_OPENING_SQUARE_BRAC;
            } else if (islower(current) || isdigit(current)) {
                dynamicStringAddChar(&buffer, current);
            } else {
                ungetc(current, sourceFile);
                if (checkTypeValid(&buffer, token) == TOKEN_OK)
                    return freeAndReturn(&buffer, TOKEN_OK);
                else
                    return freeAndReturn(&buffer, LEXICAL_ERROR);
            }
            break;

        case STATE_OPENING_SQUARE_BRAC:
            if (current == ']') {
                state = STATE_CLOSING_SQUARE_BRAC;
            } else {
                ungetc(current, sourceFile);
                return freeAndReturn(&buffer, LEXICAL_ERROR);
            }
            break;

        case STATE_CLOSING_SQUARE_BRAC:
            if (islower(current) || isdigit(current)) {
                dynamicStringAddChar(&buffer, current);
                state = STATE_TYPE;
            } else {
                ungetc(current, sourceFile);
                return freeAndReturn(&buffer, LEXICAL_ERROR);
            }
            break;

        default:
            break;
        }
    }
}