/*
 * IFJ Project
 * @brief Implementation file for parser
 *
 * @author Martin Valapka - xvalapm00
 */

// TODO: BUILT-IN FUNCTIONS, AST, OTHERS, RETURN TO STATEMENTS

#include "parser.h"
#include "testing_utils.h"
#include <string.h>

// Token* currentToken = NULL;
SymbolTable* symbolTable = NULL;
AST* ast = NULL;
ASTNode* currentParent = NULL;
bool voidFuncType = false; 

bool isTokenKeyword(Token* token, Keyword keyword) {
    return (token->type == TOKEN_TYPE_KEYWORD && 
            token->attribute.keyword == keyword);
}

// PROG ::= PROLOG FUNC_DEFS
void parseProg() {
    parseProlog();
    parseFuncDefs();

    if (currentToken->type != TOKEN_TYPE_EOF) {
        HANDLE_ERROR("Unexpected token after function definitions", SYNTAX_ERROR, currentToken);
    }
}

// PROLOG ::= token_const token_ifj token_equals token_@import("ifj24.zig");
void parseProlog() {
    printTokenInfo(currentToken);

    if (!isTokenKeyword(currentToken, KEYWORD_CONST)) {
        HANDLE_ERROR("Expected 'const' in prolog", SYNTAX_ERROR, currentToken);
    }
    getNextToken(currentToken);

    if (!isTokenKeyword(currentToken, KEYWORD_IFJ)) {
        HANDLE_ERROR("Expected 'ifj' in prolog", SYNTAX_ERROR, currentToken);
    }

    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_ASSIGN) {
        HANDLE_ERROR("Expected '=' in prolog", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (!isTokenKeyword(currentToken, KEYWORD_IMPORT)) {
        HANDLE_ERROR("Expected '@import' in prolog", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
        HANDLE_ERROR("Expected ';' in prolog", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);
}

// FUNC_DEFS ::= FUNC_DEF FUNC_DEFS | ε
void parseFuncDefs() {
    if (isTokenKeyword(currentToken, KEYWORD_PUB)) {
        parseFuncDef();
        parseFuncDefs();
    }
}

// FUNC_DEF ::= token_pub token_fn token_func_id token_Orb PARAMS token_Crb FUNC_TYPE
void parseFuncDef() {
    printTokenInfo(currentToken);
    if (!isTokenKeyword(currentToken, KEYWORD_PUB)) {
        HANDLE_ERROR("Expected 'pub' in function definition", SYNTAX_ERROR, currentToken);
    }

    ast = initAST();
    ASTNode* funcDefNode = initASTNode();
    ast->root = funcDefNode;
    funcDefNode->token = currentToken;
    currentParent = funcDefNode;

    getNextToken(currentToken);
    
    if (!isTokenKeyword(currentToken, KEYWORD_FN)) {
        HANDLE_ERROR("Expected 'fn' in function definition", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_IDENTIFIER && 
        !isTokenKeyword(currentToken, KEYWORD_MAIN)) {
        printTokenInfo(currentToken);
        HANDLE_ERROR("Expected function identifier in function definition", SYNTAX_ERROR, currentToken);
    }

    ASTNode* funcIdNode = initASTNode();
    funcIdNode->token = currentToken;
    addLeftNode(ast, currentParent, funcIdNode);

    printTokenInfo(currentToken);
    getNextToken(currentToken);
    
    if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
        HANDLE_ERROR("Expected '(' in function definition", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);
    parseParams();

    if (currentToken->type != TOKEN_TYPE_RIGHT_BR) {
        HANDLE_ERROR("Expected ')' in function definition", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);
    parseFunc();
}

// FUNC ::= TYPE token_Ocb STATEMENTS RETURN token_Ccb
void parseFunc() {
    parseType();

    if (currentToken->type != TOKEN_TYPE_LEFT_CURLY_BR) {
        HANDLE_ERROR("Expected '{' in function definition", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseStatements();
    // parseReturn();

    if (currentToken->type != TOKEN_TYPE_RIGHT_CURLY_BR) {
        HANDLE_ERROR("Expected '}' in function definition", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);
}

// TYPE ::= token_i32 | token_?i32 | token_f64 | token_?f64 | token_[]u8 | token_?[]u8 | token_void
void parseType() {
    if (isTokenKeyword(currentToken, KEYWORD_VOID)) {
        voidFuncType = true;
        printTokenInfo(currentToken);
        getNextToken(currentToken);
    } 
    else {
        if (isTokenKeyword(currentToken, KEYWORD_I_32) || isTokenKeyword(currentToken, KEYWORD_F_64) || isTokenKeyword(currentToken, KEYWORD_U_8_ARRAY) || 
            isTokenKeyword(currentToken, KEYWORD_I_32_NULL) ||  isTokenKeyword(currentToken, KEYWORD_F_64_NULL) || isTokenKeyword(currentToken, KEYWORD_U_8_ARRAY_NULL)) {
            voidFuncType = false;
            printTokenInfo(currentToken);
            getNextToken(currentToken);
        } 
        else {
            printTokenInfo(currentToken);
            HANDLE_ERROR("Expected type in function definition", SYNTAX_ERROR, currentToken);
        }
    }
}

// RETURN ::= token_return EXPR token_semicolon
// V_RETURN ::= token_return token_semicolon | ε 
void parseReturn() {
    if (!isTokenKeyword(currentToken, KEYWORD_RETURN)) {
        HANDLE_ERROR("Expected 'return' keyword", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (voidFuncType) {
        if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
            HANDLE_ERROR("Expected ';' after 'return' in void function", SYNTAX_ERROR, currentToken);
        }
        printTokenInfo(currentToken);
        getNextToken(currentToken);
    } 
    else {
        parseExpression(ast, currentToken);

        if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
            HANDLE_ERROR("Expected ';' after return expression", SYNTAX_ERROR, currentToken);
        }
        printTokenInfo(currentToken);
        getNextToken(currentToken);
    }
}

// PARAMS ::= token_id token_colon TYPE NEXT_PARAM | ε
void parseParams() {
    if (currentToken->type != TOKEN_TYPE_IDENTIFIER) {
        return;
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_COLON) {
        HANDLE_ERROR("Expected ':' after parameter identifier", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseType();

    if (currentToken->type == TOKEN_TYPE_COMMA) {
        printTokenInfo(currentToken);
        getNextToken(currentToken);
        parseParams();
    }
}

// STATEMENTS ::= STATEMENT STATEMENTS | ε
void parseStatements() {
    if (currentToken->type == TOKEN_TYPE_RIGHT_CURLY_BR) {
        return;
    }

    if (currentToken->type == TOKEN_TYPE_KEYWORD && isTokenKeyword(currentToken, KEYWORD_RETURN)) {
        parseReturn();
        if (currentToken->type != TOKEN_TYPE_RIGHT_CURLY_BR) {
            HANDLE_ERROR("Unreachable code after return statement", SYNTAX_ERROR, currentToken);
        }
        return;
    }
    parseStatement();
    parseStatements();
}

// STATEMENT ::= VAR_DEF | IF | WHILE | FUNC_CALL | DISCARD_CALL | VAR_ASS
void parseStatement() {
    switch (currentToken->type) {
        case TOKEN_TYPE_KEYWORD:
            if (isTokenKeyword(currentToken, KEYWORD_VAR) || isTokenKeyword(currentToken, KEYWORD_CONST)) {
                parseVarDef();
            } 
            else if (isTokenKeyword(currentToken, KEYWORD_IF)) {
                parseIf();
            } 
            else if (isTokenKeyword(currentToken, KEYWORD_WHILE)) {
                parseWhile();
            }
            else if (isTokenKeyword(currentToken, KEYWORD_UNDERSCORE)) {
                parseDiscardCall();
            }
            else if (isTokenKeyword(currentToken, KEYWORD_RETURN)) {
                parseReturn();
            } 
            else {
                printTokenInfo(currentToken);
                HANDLE_ERROR("Unexpected keyword in statement", SYNTAX_ERROR, currentToken);
            }
            break;
        
        case TOKEN_TYPE_IDENTIFIER:
            getNextToken(currentToken);

            if (currentToken->type == TOKEN_TYPE_LEFT_BR) {
                parseFuncCall();
            }
            else if (currentToken->type == TOKEN_TYPE_ASSIGN) {
                parseVarAss();
            }
            else {
                HANDLE_ERROR("Expected '(' or '=' after identifier", SYNTAX_ERROR, currentToken);
            }
            break;

        default:
            HANDLE_ERROR("Unexpected token in statement", SYNTAX_ERROR, currentToken);
            break;
    }
}

// VAR_DEF ::= VAR_TYPE token_id TYPE_SPEC token_equals EXPR token_semicolon
void parseVarDef() {
    if ((isTokenKeyword(currentToken, KEYWORD_VAR) || isTokenKeyword(currentToken, KEYWORD_CONST))) {
        printTokenInfo(currentToken);
        getNextToken(currentToken);
    } 
    else {
        HANDLE_ERROR("Expected 'var' or 'const' in variable definition", SYNTAX_ERROR, currentToken);
    }

    if (currentToken->type != TOKEN_TYPE_IDENTIFIER) {
        HANDLE_ERROR("Expected variable identifier", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseTypeSpec();

    if (currentToken->type != TOKEN_TYPE_ASSIGN) {
        HANDLE_ERROR("Expected '=' in variable definition", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseExpression(ast, currentToken);

    if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
        printTokenInfo(currentToken);
        HANDLE_ERROR("Expected ';' at the end of variable definition", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);
}

// TYPE_SPEC ::= token_colon TYPE | ε
void parseTypeSpec() {
    if (currentToken->type != TOKEN_TYPE_COLON) {
        return;
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseType();
}

// VAR_ASS ::= token_id token_equals EXPR token_semicolon
void parseVarAss() {
    if (currentToken->type != TOKEN_TYPE_ASSIGN) {
        HANDLE_ERROR("Expected '=' after identifier in variable assignment", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseExpression(ast, currentToken);

    if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
        HANDLE_ERROR("Expected ';' at the end of variable assignment", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);
}

// While ::= token_while token_Orb EXPR token_Crb NULL_COND token_Ocb STATEMENTS token_Ccb
void parseWhile() {
    if (!isTokenKeyword(currentToken, KEYWORD_WHILE)) {
        HANDLE_ERROR("Expected 'while' at the beginning of while loop", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
        HANDLE_ERROR("Expected '(' after 'while'", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseExpression(ast, currentToken);

    if (currentToken->type != TOKEN_TYPE_RIGHT_BR) {
        HANDLE_ERROR("Expected ')' after expression in while loop", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseNullCond();

    if (currentToken->type != TOKEN_TYPE_LEFT_CURLY_BR) {
        HANDLE_ERROR("Expected '{' to start the body of while loop", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseStatements();

    if (currentToken->type != TOKEN_TYPE_RIGHT_CURLY_BR) {
        HANDLE_ERROR("Expected '}' to end the body of while loop", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);
}

// NULL_CONDITION ::= token_vb token_id token_vb | ε
void parseNullCond() {
    if (currentToken->type != TOKEN_TYPE_OR) {
        return;
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_IDENTIFIER) {
        HANDLE_ERROR("Expected identifier in null condition", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_OR) {
        HANDLE_ERROR("Expected '|' after identifier in null condition", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);
}

// IF ::= token_if token_Orb EXPR token_Crb NULL_COND token_Ocb STATEMENTS token_Ccb ELSE
void parseIf() {
    if (!isTokenKeyword(currentToken, KEYWORD_IF)) {
        HANDLE_ERROR("Expected 'if' at the beginning of if statement", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
        HANDLE_ERROR("Expected '(' after 'if'", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseExpression(ast, currentToken);

    if (currentToken->type != TOKEN_TYPE_RIGHT_BR) {
        HANDLE_ERROR("Expected ')' after expression in if statement", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseNullCond();

    if (currentToken->type != TOKEN_TYPE_LEFT_CURLY_BR) {
        HANDLE_ERROR("Expected '{' to start the body of if statement", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseStatements();

    if (currentToken->type != TOKEN_TYPE_RIGHT_CURLY_BR) {
        HANDLE_ERROR("Expected '}' to end the body of if statement", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseElse();
}

// ELSE ::= token_else token_Ocb STATEMENTS token_Ccb | ε
void parseElse() {
    if (!isTokenKeyword(currentToken, KEYWORD_ELSE)) {
        return;
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_LEFT_CURLY_BR) {
        HANDLE_ERROR("Expected '{' to start the body of else statement", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseStatements();

    if (currentToken->type != TOKEN_TYPE_RIGHT_CURLY_BR) {
        HANDLE_ERROR("Expected '}' to end the body of else statement", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);
}

// FUNC_CALL ::= token_id token_Orb ARGS token_Crb token_semicolon
void parseFuncCall() {
    if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
        HANDLE_ERROR("Expected '(' after function identifier", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseArgs();

    if (currentToken->type != TOKEN_TYPE_RIGHT_BR) {
        HANDLE_ERROR("Expected ')' after arguments in function call", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
        HANDLE_ERROR("Expected ';' at the end of function call", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);
}

// DISCARD_CALL ::= token_underscore token_equals EXPR token_semicolon
void parseDiscardCall() {
    if (!isTokenKeyword(currentToken, KEYWORD_UNDERSCORE)) {
        HANDLE_ERROR("Expected '_' at the beginning of discard call", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_ASSIGN) {
        HANDLE_ERROR("Expected '=' after '_' in discard call", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseExpression(ast, currentToken);

    if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
        HANDLE_ERROR("Expected ';' at the end of discard call", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);
}

// ARGS ::= (EXPR | token_id) NEXT_ARG | ε      
void parseArgs() {
    if (currentToken->type == TOKEN_TYPE_RIGHT_BR) {
        return;
    }

    if (currentToken->type == TOKEN_TYPE_IDENTIFIER) {
        printTokenInfo(currentToken);
        getNextToken(currentToken);
    } 
    else {
        parseExpression(ast, currentToken);
    }

    if (currentToken->type == TOKEN_TYPE_COMMA) {
        printTokenInfo(currentToken);
        getNextToken(currentToken);
        parseArgs();
    }
}

int parse() {
    printf("Parsing started\n");
    parseProg();

    if (currentToken->type != TOKEN_TYPE_EOF) {
        fprintf(stderr, "Error: Unexpected token after parsing the program\n");
        return SYNTAX_ERROR;
    }
    return 0;
}