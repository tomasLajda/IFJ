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
    if (currentToken->type == KEYWORD_CONST) {
        getNextToken(currentToken);

        if (currentToken->type == KEYWORD_IFJ) {
            getNextToken(currentToken);

            if (currentToken->type == TOKEN_TYPE_ASSIGN) {
                getNextToken(currentToken);

                if (currentToken->type == KEYWORD_IMPORT) {
                    getNextToken(currentToken);

                    if (currentToken->type == TOKEN_TYPE_SEMICOLON) {
                        getNextToken(currentToken);
                    }
                    else {
                        HANDLE_ERROR("Expected ';' in prolog", SYNTAX_ERROR, currentToken);
                    } 
                }
                else {
                    HANDLE_ERROR("Expected '@import' in prolog", SYNTAX_ERROR, currentToken);
                }
            }
            else {
                HANDLE_ERROR("Expected '=' in prolog", SYNTAX_ERROR, currentToken);
            }
        }
        else {
            HANDLE_ERROR("Expected 'ifj' in prolog", SYNTAX_ERROR, currentToken);
        }
    }
    else {
        HANDLE_ERROR("Expected 'const' in prolog", SYNTAX_ERROR, currentToken);
    }                        
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
    if (currentToken->type == KEYWORD_PUB) {
        getNextToken(currentToken);

        if (currentToken->type == KEYWORD_FN) {
            getNextToken(currentToken);

            if (currentToken->type == TOKEN_TYPE_IDENTIFIER) {
                getNextToken(currentToken);

                if (currentToken->type == TOKEN_TYPE_LEFT_BR) {
                    getNextToken(currentToken);
                    parseParams();

                    if (currentToken->type == TOKEN_TYPE_RIGHT_BR) {
                        getNextToken(currentToken);
                        parseFunc();
                    }
                    else {
                        HANDLE_ERROR("Expected ')' in function definition", SYNTAX_ERROR, currentToken);
                    }
                }
                else {
                    HANDLE_ERROR("Expected '(' in function definition", SYNTAX_ERROR, currentToken);
                }
            }
            else {
                HANDLE_ERROR("Expected function identifier in function definition", SYNTAX_ERROR, currentToken);
            }
        }
        else {
            HANDLE_ERROR("Expected 'fn' in function definition", SYNTAX_ERROR, currentToken);
        }
    }
    else {
        HANDLE_ERROR("Expected 'pub' in function definition", SYNTAX_ERROR, currentToken);
    }
}
