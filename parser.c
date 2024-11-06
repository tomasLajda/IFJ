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
bool voidFuncType = false; 

void parseProlog() {
    if (currentToken->type != KEYWORD_CONST) {
        HANDLE_ERROR("Expected 'const' in prolog", SYNTAX_ERROR, currentToken);
    }
    getNextToken(currentToken);

    if (currentToken->type != KEYWORD_IFJ) {
        HANDLE_ERROR("Expected 'ifj' in prolog", SYNTAX_ERROR, currentToken);
    }
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_ASSIGN) {
        HANDLE_ERROR("Expected '=' in prolog", SYNTAX_ERROR, currentToken);
    }
    getNextToken(currentToken);

    if (currentToken->type != KEYWORD_IMPORT) {
        HANDLE_ERROR("Expected '@import' in prolog", SYNTAX_ERROR, currentToken);
    }
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
        HANDLE_ERROR("Expected ';' in prolog", SYNTAX_ERROR, currentToken);
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
    }
    getNextToken(currentToken);

    if (currentToken->type != KEYWORD_FN) {
        HANDLE_ERROR("Expected 'fn' in function definition", SYNTAX_ERROR, currentToken);
    }
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_IDENTIFIER) {
        HANDLE_ERROR("Expected function identifier in function definition", SYNTAX_ERROR, currentToken);
    }
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
        HANDLE_ERROR("Expected '(' in function definition", SYNTAX_ERROR, currentToken);
    }
    getNextToken(currentToken);
    parseParams();

    if (currentToken->type != TOKEN_TYPE_RIGHT_BR) {
        HANDLE_ERROR("Expected ')' in function definition", SYNTAX_ERROR, currentToken);
    }
    getNextToken(currentToken);
    parseFunc();
}

void parseFunc() {
    parseType();

    if (currentToken->type != TOKEN_TYPE_LEFT_CURLY_BR) {
        HANDLE_ERROR("Expected '{' in function definition", SYNTAX_ERROR, currentToken);
    }
    getNextToken(currentToken);

    parseStatements();
    parseReturn();

    if (currentToken->type != TOKEN_TYPE_RIGHT_CURLY_BR) {
        HANDLE_ERROR("Expected '}' in function definition", SYNTAX_ERROR, currentToken);
    }
    getNextToken(currentToken);
}

void parseType() {
    if (currentToken->type == KEYWORD_VOID) {
        voidFuncType = true;
        getNextToken(currentToken);
    } 
    else {
        if (currentToken->type == KEYWORD_I_32 || currentToken->type == KEYWORD_F_64 || currentToken->type == KEYWORD_U_8_ARRAY ||
            currentToken->type == KEYWORD_I_32_NULL || currentToken->type == KEYWORD_F_64_NULL || currentToken->type == KEYWORD_U_8_ARRAY_NULL) {
            voidFuncType = false;
            getNextToken(currentToken);
        } 
        else {
            HANDLE_ERROR("Expected type in function definition", SYNTAX_ERROR, currentToken);
        }
    }
}

void parseReturn() {
    if (voidFuncType) {
         if (currentToken->type == KEYWORD_RETURN) {
            getNextToken(currentToken);

            if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
                HANDLE_ERROR("Expected ';' after return in void function", SYNTAX_ERROR, currentToken);
            }

            getNextToken(currentToken);
        }
        return;
    } 
    else {
        if (currentToken->type != KEYWORD_RETURN) {
            HANDLE_ERROR("Expected 'return' keyword in return statement", SYNTAX_ERROR, currentToken);
        }
        getNextToken(currentToken);
        
        // TODO: Expression();
        
        if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
            HANDLE_ERROR("Expected ';' after return expression", SYNTAX_ERROR, currentToken);
        }
        getNextToken(currentToken);
    }
}

// TODO: Reimplement the part with next comma with peek function
void parseParams() {
    if (currentToken->type != TOKEN_TYPE_IDENTIFIER) {
        return;
    }

    while (true) {
        if (currentToken->type != TOKEN_TYPE_IDENTIFIER) {
            HANDLE_ERROR("Expected parameter identifier", SYNTAX_ERROR, currentToken);
        }
        getNextToken(currentToken);

        if (currentToken->type != TOKEN_TYPE_COLON) {
            HANDLE_ERROR("Expected ':' after parameter identifier", SYNTAX_ERROR, currentToken);
        }
        getNextToken(currentToken);

        parseType();

        // peek();
        if (currentToken->type != TOKEN_TYPE_COMMA) {
            break;
        }
        getNextToken(currentToken);
    }
}

void parseStatements() {
    while (true) {
        parseStatement();

        if (currentToken->type == TOKEN_TYPE_RIGHT_CURLY_BR) {
            return;
        }
    }
}

void parseStatement() {
    switch (currentToken->type) {
        case KEYWORD_VAR:
        case KEYWORD_CONST:
            parseVarDef();
            break;
        case KEYWORD_IF:
            parseIf();
            break;
        case KEYWORD_WHILE:
            parseWhile();
            break;
        case TOKEN_TYPE_IDENTIFIER:
            if (getNextToken(currentToken) == TOKEN_TYPE_LEFT_BR) {
                parseFuncCall();
            }
            else if (getNextToken(currentToken) == TOKEN_TYPE_ASSIGN) {
                getNextToken(currentToken);
                parseVarAss();
            }
            else {
                if (getNextToken(currentToken) != TOKEN_TYPE_LEFT_BR) {
                    HANDLE_ERROR("Expected '(' after identifier for function call", SYNTAX_ERROR, currentToken);
                }
                if (getNextToken(currentToken) != TOKEN_TYPE_ASSIGN) {
                    HANDLE_ERROR("Expected '=' after identifier for variable assignment", SYNTAX_ERROR, currentToken);
                }
            }
            break;
        case KEYWORD_UNDERSCORE:
            parseDiscardCall();
            break;
        default:
            HANDLE_ERROR("Unexpected token in statement", SYNTAX_ERROR, currentToken);
            break;
    }
}
