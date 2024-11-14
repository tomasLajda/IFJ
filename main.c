#include "scanner.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOLERANCE 1e-5

void printToken(Token *token) {
    printf("Token type: %d\n", token->type);
    switch (token->type) {
    case TOKEN_TYPE_INTEGER_VALUE:
        printf("Token attribute (integer): %d\n", token->attribute.integer);
        break;
    case TOKEN_TYPE_DOUBLE_VALUE:
        printf("Token attribute (double): %f\n", token->attribute.decimal);
        break;
    case TOKEN_TYPE_STRING_VALUE:
        printf("Token attribute (string): %s\n", token->attribute.string);
        break;
    case TOKEN_TYPE_KEYWORD:
        printf("Token attribute (keyword): %d\n", token->attribute.keyword);
        break;
    case TOKEN_TYPE_IDENTIFIER:
        printf("Token attribute (identifier): %s\n", token->attribute.string);
        break;
    case TOKEN_TYPE_PLUS:
        printf("Token is a PLUS operator.\n");
        break;
    case TOKEN_TYPE_MINUS:
        printf("Token is a MINUS operator.\n");
        break;
    case TOKEN_TYPE_MUL:
        printf("Token is a MULTIPLICATION operator.\n");
        break;
    case TOKEN_TYPE_DIV:
        printf("Token is a DIVISION operator.\n");
        break;
    case TOKEN_TYPE_SEMICOLON:
        printf("Token is a SEMICOLON.\n");
        break;
    case TOKEN_TYPE_LEFT_BR:
        printf("Token is a LEFT BRACKET.\n");
        break;
    case TOKEN_TYPE_RIGHT_BR:
        printf("Token is a RIGHT BRACKET.\n");
        break;
    // Add cases for other token types as needed
    default:
        printf("Token has no attribute or unrecognized type.\n");
        break;
    }
}

void test_getNextToken() {
    Token token;
    FILE *testFile;

    // Test 1: Simple operators
    testFile = fopen("test1.txt", "w+");
    fprintf(testFile, "+ - * / ;");
    extern FILE *sourceFile;
    sourceFile = testFile;
    rewind(testFile);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_PLUS);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_MINUS);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_MUL);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_DIV);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_SEMICOLON);

    fclose(testFile);

    // Test 2: Keywords and identifiers
    testFile = fopen("test2.txt", "w+");
    fprintf(testFile, "if else while var const");
    rewind(testFile);
    sourceFile = testFile;

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_KEYWORD && token.attribute.keyword == KEYWORD_IF);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_KEYWORD && token.attribute.keyword == KEYWORD_ELSE);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_KEYWORD && token.attribute.keyword == KEYWORD_WHILE);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_KEYWORD && token.attribute.keyword == KEYWORD_VAR);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_KEYWORD && token.attribute.keyword == KEYWORD_CONST);

    fclose(testFile);

    // Test 3: Strings
    testFile = fopen("test3.txt", "w+");
    fprintf(testFile, "\"hello\" \"world\"");
    rewind(testFile);
    sourceFile = testFile;

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_STRING_VALUE && strcmp(token.attribute.string, "hello") == 0);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_STRING_VALUE && strcmp(token.attribute.string, "world") == 0);

    fclose(testFile);

    // Test 4: Numbers
    testFile = fopen("test4.txt", "w+");
    fprintf(testFile, "123 0 45.67 8e2 3.14e-2");
    rewind(testFile);
    sourceFile = testFile;

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_INTEGER_VALUE && token.attribute.integer == 123);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_INTEGER_VALUE && token.attribute.integer == 0);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_DOUBLE_VALUE && token.attribute.decimal == 45.67);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_INTEGER_VALUE &&
           fabs(token.attribute.integer - 8e2) < TOLERANCE);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_DOUBLE_VALUE &&
           fabs(token.attribute.decimal - 3.14e-2) < TOLERANCE);

    fclose(testFile);

    // Test 5: Comments
    testFile = fopen("test5.txt", "w+");
    fprintf(testFile, "// this is a comment\n+");
    rewind(testFile);
    sourceFile = testFile;

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_PLUS);

    fclose(testFile);

    // Test 6: Identifiers
    testFile = fopen("test6.txt", "w+");
    fprintf(testFile, "variable1 variable2");
    rewind(testFile);
    sourceFile = testFile;

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_IDENTIFIER && strcmp(token.attribute.string, "variable1") == 0);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_IDENTIFIER && strcmp(token.attribute.string, "variable2") == 0);

    fclose(testFile);

    // Test 7: Mixed content
    testFile = fopen("test7.txt", "w+");
    fprintf(testFile, "123 + var1 - 45.67 * \"string\" / if ;");
    rewind(testFile);
    sourceFile = testFile;

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_INTEGER_VALUE && token.attribute.integer == 123);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_PLUS);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_IDENTIFIER && strcmp(token.attribute.string, "var1") == 0);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_MINUS);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_DOUBLE_VALUE &&
           fabs(token.attribute.decimal - 45.67) < TOLERANCE);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_MUL);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_STRING_VALUE && strcmp(token.attribute.string, "string") == 0);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_DIV);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_KEYWORD && token.attribute.keyword == KEYWORD_IF);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_SEMICOLON);

    fclose(testFile);

    // Test 8: Parentheses and mixed content
    testFile = fopen("test8.txt", "w+");
    fprintf(testFile, "( 1 + 2.5 ) + x / 6");
    rewind(testFile);
    sourceFile = testFile;

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_LEFT_BR);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_INTEGER_VALUE && token.attribute.integer == 1);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_PLUS);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_DOUBLE_VALUE &&
           fabs(token.attribute.decimal - 2.5) < TOLERANCE);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_RIGHT_BR);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_PLUS);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_IDENTIFIER && strcmp(token.attribute.string, "x") == 0);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_DIV);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_INTEGER_VALUE && token.attribute.integer == 6);

    fclose(testFile);

    // Test 9: Complex expressions
    testFile = fopen("test9.txt", "w+");
    fprintf(testFile, "3.14 * (2 + 5) / vvr - 10");
    rewind(testFile);
    sourceFile = testFile;

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_DOUBLE_VALUE &&
           fabs(token.attribute.decimal - 3.14) < TOLERANCE);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_MUL);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_LEFT_BR);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_INTEGER_VALUE && token.attribute.integer == 2);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_PLUS);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_INTEGER_VALUE && token.attribute.integer == 5);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_RIGHT_BR);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_DIV);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_IDENTIFIER && strcmp(token.attribute.string, "vvr") == 0);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_MINUS);

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_INTEGER_VALUE && token.attribute.integer == 10);

    fclose(testFile);

    // Test 10: Edge case - Empty file
    testFile = fopen("test10.txt", "w+");
    rewind(testFile);
    sourceFile = testFile;

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_EOF);

    fclose(testFile);

    // Test 11: Edge case - Only whitespace
    testFile = fopen("test11.txt", "w+");
    fprintf(testFile, "    \n\t  ");
    rewind(testFile);
    sourceFile = testFile;

    getNextToken(&token);
    printToken(&token);
    assert(token.type == TOKEN_TYPE_EOF);

    fclose(testFile);

    // Test 12: Edge case - Invalid token
    testFile = fopen("test12.txt", "w+");
    fprintf(testFile, "@");
    rewind(testFile);
    sourceFile = testFile;

    int result = getNextToken(&token);
    if (result != 0) {
        printf("Error: Invalid token encountered.\n");
    } else {
        printToken(&token);
    }
    assert(result != 0);

    fclose(testFile);

    printf("All tests passed!\n");
}

int main() {
    test_getNextToken();
    return 0;
}