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

int freeAndReturn(DynamicString *string, int errorCode);

int handleIdentifierOrKeyword(DynamicString *string, Token *token);

int checkTypeValid(DynamicString *string, Token *token);

#endif