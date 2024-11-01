/*
 * IFJ Project
 * @brief Implementation file for parser
 *
 * @author Martin Valapka - xvalapm00
 */

#include "parser.h"
#include <string.h>

Token* currentToken = NULL;
SymbolTable* symbolTable = NULL;
AST* ast = NULL;

void parseProlog() {
    if (currentToken->type != KEYWORD_CONST) {
        HANDLE_ERROR("Expected 'const' in prolog", SYNTAX_ERROR, currentToken);
        return;
    }
    getNextToken(currentToken);

    if (currentToken->type != KEYWORD_IFJ) {
        HANDLE_ERROR("Expected 'ifj' in prolog", SYNTAX_ERROR, currentToken);
        return;
    }
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_ASSIGN) {
        HANDLE_ERROR("Expected '=' in prolog", SYNTAX_ERROR, currentToken);
        return;
    }
    getNextToken(currentToken);

    if (currentToken->type != KEYWORD_IMPORT) {
        HANDLE_ERROR("Expected '@import' in prolog", SYNTAX_ERROR, currentToken);
        return;
    }
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
        HANDLE_ERROR("Expected ';' in prolog", SYNTAX_ERROR, currentToken);
        return;
    }
    getNextToken(currentToken);
}

void parseFuncDefs() {
    while (true) {
        if (currentToken->type == KEYWORD_PUB) {
            parseFuncDef();

            if (currentToken->type != KEYWORD_PUB) {
                break;
            }
        } 
        else {
            break;
        }
    }
}

void parseFuncDef() {
    if (currentToken->type != KEYWORD_PUB) {
        HANDLE_ERROR("Expected 'pub' in function definition", SYNTAX_ERROR, currentToken);
        return;
    }
    getNextToken(currentToken);

    if (currentToken->type != KEYWORD_FN) {
        HANDLE_ERROR("Expected 'fn' in function definition", SYNTAX_ERROR, currentToken);
        return;
    }
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_IDENTIFIER) {
        HANDLE_ERROR("Expected function identifier in function definition", SYNTAX_ERROR, currentToken);
        return;
    }
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
        HANDLE_ERROR("Expected '(' in function definition", SYNTAX_ERROR, currentToken);
        return;
    }
    getNextToken(currentToken);
    parseParams();

    if (currentToken->type != TOKEN_TYPE_RIGHT_BR) {
        HANDLE_ERROR("Expected ')' in function definition", SYNTAX_ERROR, currentToken);
        return;
    }
    getNextToken(currentToken);
    parseFunc();
}

void parseFunc() {
    parseType();

    if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
        HANDLE_ERROR("Expected '{' in function definition", SYNTAX_ERROR, currentToken);
        return;
    }
    getNextToken(currentToken);

    parseStatements();
    parseReturn();

    if (currentToken->type != TOKEN_TYPE_RIGHT_BR) {
        HANDLE_ERROR("Expected '}' in function definition", SYNTAX_ERROR, currentToken);
        return;
    }
    getNextToken(currentToken);
}
