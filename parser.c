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

// PROLOG ::= token_const token_ifj token_equals token_@import("ifj24.zig");
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

// FUNC_DEFS ::= FUNC_DEF FUNC_DEFS | ε
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

// FUNC_DEF ::= token_pub token_fn token_func_id token_Orb PARAMS token_Crb FUNC_TYPE
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

// FUNC ::= TYPE token_Ocb STATEMENTS RETURN token_Ccb
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

// TYPE ::= token_i32 | token_?i32 | token_f64 | token_?f64 | token_[]u8 | token_?[]u8 | token_void
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

// RETURN ::= token_return EXPR token_semicolon
// V_RETURN ::= token_return token_semicolon | ε 
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
// PARAMS ::= token_id token_colon TYPE NEXT_PARAM | ε
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

// STATEMENTS ::= STATEMENT STATEMENTS | ε
void parseStatements() {
    while (true) {
        parseStatement();

        if (currentToken->type == TOKEN_TYPE_RIGHT_CURLY_BR) {
            return;
        }
    }
}

// STATEMENT ::= VAR_DEF | IF | WHILE | FUNC_CALL | DISCARD_CALL
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
                HANDLE_ERROR("Expected '(' or '=' after identifier", SYNTAX_ERROR, currentToken);
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

// VAR_DEF ::= VAR_TYPE token_id TYPE_SPEC token_equals EXPR token_semicolon
void parseVarDef() {
    if (currentToken->type == KEYWORD_VAR || currentToken->type == KEYWORD_CONST) {
        getNextToken(currentToken);
    } 
    else {
        HANDLE_ERROR("Expected 'var' or 'const' in variable definition", SYNTAX_ERROR, currentToken);
    }

    if (currentToken->type != TOKEN_TYPE_IDENTIFIER) {
        HANDLE_ERROR("Expected variable identifier", SYNTAX_ERROR, currentToken);
    }
    getNextToken(currentToken);

    parseTypeSpec();

    if (currentToken->type != TOKEN_TYPE_ASSIGN) {
        HANDLE_ERROR("Expected '=' in variable definition", SYNTAX_ERROR, currentToken);
    }
    getNextToken(currentToken);

    // Expression();

    if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
        HANDLE_ERROR("Expected ';' at the end of variable definition", SYNTAX_ERROR, currentToken);
    }
    getNextToken(currentToken);
}