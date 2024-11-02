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

    if (currentToken->type != TOKEN_TYPE_LEFT_CURLY_BR) {
        HANDLE_ERROR("Expected '{' in function definition", SYNTAX_ERROR, currentToken);
        return;
    }
    getNextToken(currentToken);

    parseStatements();
    parseReturn();

    if (currentToken->type != TOKEN_TYPE_RIGHT_CURLY_BR) {
        HANDLE_ERROR("Expected '}' in function definition", SYNTAX_ERROR, currentToken);
        return;
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
            return;
        }
    }
}

void parseReturn() {
    if (voidFuncType) {
         if (currentToken->type == KEYWORD_RETURN) {
            getNextToken(currentToken);

            if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
                HANDLE_ERROR("Expected ';' after return in void function", SYNTAX_ERROR, currentToken);
                return;
            }
            
            getNextToken(currentToken);
        }
        return;
    } 
    else {
        if (currentToken->type != KEYWORD_RETURN) {
            HANDLE_ERROR("Expected 'return' keyword in return statement", SYNTAX_ERROR, currentToken);
            return;
        }
        getNextToken(currentToken);
        
        // TODO: parseExpr();
        
        if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
            HANDLE_ERROR("Expected ';' after return expression", SYNTAX_ERROR, currentToken);
            return;
        }
        getNextToken(currentToken);
    }
}
