/*
 * IFJ Project
 * @brief Implementation file for parser
 *
 * @author Martin Valapka - xvalapm00
 */

// TODO: BUILT-IN FUNCTIONS, AST, GOBACK FUNCTION

#include "parser.h"
#include "testing_utils.h"
#include <string.h>
#include "helpers.h"

// Token* currentToken = NULL;
AST *ast = NULL;
ASTNode *mainParent = NULL;
ASTNode *currenParent = NULL;
bool voidFuncType = false;
bool parsingParameters = false;
bool onlyZeroArgs = false;
bool onlyOneArg = false;
bool onlyTwoArgs = false;
bool onlyThreeArgs = false;
unsigned int argCounter = 0;
unsigned int paramCounter = 0;

bool isTokenKeyword(Token *token, Keyword keyword) {
    return (token->type == TOKEN_TYPE_KEYWORD && token->attribute.keyword == keyword);
}

bool isTokenBuiltInFunction(Token *token) {
    switch (token->attribute.keyword) {
    case KEYWORD_READSTR:
    case KEYWORD_READI32:
    case KEYWORD_READF64:
        onlyZeroArgs = true;
        return true;

    case KEYWORD_STRING:
    case KEYWORD_LENGTH:
    case KEYWORD_CHR:
    case KEYWORD_I2F:
    case KEYWORD_F2I:
        onlyOneArg = true;
        return true;

    case KEYWORD_CONCAT:
    case KEYWORD_STRCMP:
    case KEYWORD_ORD:
        onlyTwoArgs = true;
        return true;

    case KEYWORD_SUBSTRING:
        onlyThreeArgs = true;
        return true;

    default:
        return false;
    }
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
    paramCounter = 0;

    printTokenInfo(currentToken);
    if (!isTokenKeyword(currentToken, KEYWORD_PUB)) {
        HANDLE_ERROR("Expected 'pub' in function definition", SYNTAX_ERROR, currentToken);
    }

    ASTNode *pubNode = initASTNode();
    pubNode->token = copyToken(currentToken);
    addRightNode(ast, currenParent, pubNode);
    currenParent = pubNode;

    printf("\n");
    displayAST(ast);
    printf("\n");

    getNextToken(currentToken);

    if (!isTokenKeyword(currentToken, KEYWORD_FN)) {
        HANDLE_ERROR("Expected 'fn' in function definition", SYNTAX_ERROR, currentToken);
    }

    ASTNode *fnNode = initASTNode();
    fnNode->token = copyToken(currentToken);
    addLeftNode(ast, currenParent, fnNode);
    currenParent = fnNode;
    mainParent = fnNode;

    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_IDENTIFIER &&
        !isTokenKeyword(currentToken, KEYWORD_MAIN)) {
        printTokenInfo(currentToken);
        HANDLE_ERROR("Expected function identifier in function definition", SYNTAX_ERROR,
                     currentToken);
    }

    ASTNode *funcIdNode = initASTNode();
    funcIdNode->token = copyToken(currentToken);
    addLeftNode(ast, currenParent, funcIdNode);
    currenParent = funcIdNode;

    printf("\n");
    displayAST(ast);
    printf("\n");

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

// FUNC ::= TYPE token_Ocb STATEMENTS token_Ccb
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
        if (parsingParameters) {
            HANDLE_ERROR("Parameter cannot be of type void", PARAMETER_ERROR, currentToken);
        }
        voidFuncType = true;
        printTokenInfo(currentToken);
        getNextToken(currentToken);
    } else {
        if (isTokenKeyword(currentToken, KEYWORD_I_32) ||
            isTokenKeyword(currentToken, KEYWORD_F_64) ||
            isTokenKeyword(currentToken, KEYWORD_U_8_ARRAY) ||
            isTokenKeyword(currentToken, KEYWORD_I_32_NULL) ||
            isTokenKeyword(currentToken, KEYWORD_F_64_NULL) ||
            isTokenKeyword(currentToken, KEYWORD_U_8_ARRAY_NULL)) {
            voidFuncType = false;

            ASTNode *typeNode = initASTNode();
            typeNode->token = copyToken(currentToken);
            addLeftNode(ast, currenParent, typeNode);

            printf("\n");
            displayAST(ast);
            printf("\n");

            printTokenInfo(currentToken);
            getNextToken(currentToken);
        } else {
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
            HANDLE_ERROR("Expected ';' after 'return' in void function", SYNTAX_ERROR,
                         currentToken);
        }
        printTokenInfo(currentToken);
        getNextToken(currentToken);
    } else {
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
    parsingParameters = true;

    if (currentToken->type == TOKEN_TYPE_RIGHT_BR) {
        return;
    }
    if (currentToken->type != TOKEN_TYPE_IDENTIFIER) {
        HANDLE_ERROR("Expected parameter identifier", SYNTAX_ERROR, currentToken);
    }
    paramCounter++;

    ASTNode *paramIdNode = initASTNode();
    paramIdNode->token = copyToken(currentToken);

    if (paramCounter == 1) {
        addLeftNode(ast, currenParent, paramIdNode);
    } else {
        addRightNode(ast, currenParent, paramIdNode);
    }
    currenParent = paramIdNode;

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
    parsingParameters = false;
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
    onlyZeroArgs = false;
    onlyOneArg = false;
    onlyTwoArgs = false;
    onlyThreeArgs = false;
    argCounter = 0;

    switch (currentToken->type) {
    case TOKEN_TYPE_KEYWORD:
        if (isTokenKeyword(currentToken, KEYWORD_VAR) ||
            isTokenKeyword(currentToken, KEYWORD_CONST)) {
            parseVarDef();
        } else if (isTokenKeyword(currentToken, KEYWORD_IF)) {
            parseIf();
        } else if (isTokenKeyword(currentToken, KEYWORD_WHILE)) {
            parseWhile();
        } else if (isTokenKeyword(currentToken, KEYWORD_UNDERSCORE)) {
            parseDiscardCall();
        } else if (isTokenKeyword(currentToken, KEYWORD_RETURN)) {
            parseReturn();
        } else if (isTokenKeyword(currentToken, KEYWORD_IFJ)) {
            printTokenInfo(currentToken);
            getNextToken(currentToken);

            if (currentToken->type != TOKEN_TYPE_DOT) {
                HANDLE_ERROR("Unexpected token in built-in function", SYNTAX_ERROR, currentToken);
            }
            printTokenInfo(currentToken);
            getNextToken(currentToken);

            if (!isTokenKeyword(currentToken, KEYWORD_WRITE)) {
                HANDLE_ERROR("Unexpected built-in function in function call", SYNTAX_ERROR,
                             currentToken);
            }
            printTokenInfo(currentToken);
            getNextToken(currentToken);

            if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
                HANDLE_ERROR("Expected '(' after built-in function", SYNTAX_ERROR, currentToken);
            }
            onlyOneArg = true;
            parseFuncCall();
        } else {
            printTokenInfo(currentToken);
            HANDLE_ERROR("Unexpected keyword in statement", SYNTAX_ERROR, currentToken);
        }
        break;

    case TOKEN_TYPE_IDENTIFIER:
        printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (currentToken->type == TOKEN_TYPE_LEFT_BR) {
            parseFuncCall();
        } else if (currentToken->type == TOKEN_TYPE_ASSIGN) {
            parseVarAss();
        } else {
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
    if (!isTokenKeyword(currentToken, KEYWORD_VAR) &&
        !isTokenKeyword(currentToken, KEYWORD_CONST)) {
        HANDLE_ERROR("Expected 'var' or 'const' in variable definition", SYNTAX_ERROR,
                     currentToken);
    }

    printTokenInfo(currentToken);
    getNextToken(currentToken);

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

    if (isTokenKeyword(currentToken, KEYWORD_IFJ)) {
        printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (currentToken->type != TOKEN_TYPE_DOT) {
            HANDLE_ERROR("Expected '.' after ifj", SYNTAX_ERROR, currentToken);
        }
        printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (!isTokenBuiltInFunction(currentToken)) {
            HANDLE_ERROR("Expected built-in function after '.'", SYNTAX_ERROR, currentToken);
        }

        printTokenInfo(currentToken);
        getNextToken(currentToken);
        parseFuncCall();
    } else {
        parseExpression(ast, currentToken);

        if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
            HANDLE_ERROR("Expected ';' at the end of variable definition", SYNTAX_ERROR,
                         currentToken);
        }
        printTokenInfo(currentToken);
        getNextToken(currentToken);
    }
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
        HANDLE_ERROR("Expected '=' after identifier in variable assignment", SYNTAX_ERROR,
                     currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (isTokenKeyword(currentToken, KEYWORD_IFJ)) {
        printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (currentToken->type != TOKEN_TYPE_DOT) {
            HANDLE_ERROR("Expected '.' after ifj", SYNTAX_ERROR, currentToken);
        }
        printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (!isTokenBuiltInFunction(currentToken)) {
            HANDLE_ERROR("Expected built-in function after '.'", SYNTAX_ERROR, currentToken);
        }

        printTokenInfo(currentToken);
        getNextToken(currentToken);
        parseFuncCall();
        return;
    } else {
        parseExpression(ast, currentToken);
    }

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
    if (currentToken->type != TOKEN_TYPE_VB) {
        return;
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_IDENTIFIER) {
        HANDLE_ERROR("Expected identifier in null condition", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_VB) {
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
        HANDLE_ERROR("Expected '{' to start the body of else statement", SYNTAX_ERROR,
                     currentToken);
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
        if (onlyZeroArgs && argCounter > 0) {
            HANDLE_ERROR("This built-in function takes no arguments", PARAMETER_ERROR,
                         currentToken);
        }
        if (onlyTwoArgs && argCounter != 2) {
            HANDLE_ERROR("Built-in functions require exactly 2 arguments", PARAMETER_ERROR,
                         currentToken);
        }
        if (onlyOneArg && argCounter != 1) {
            HANDLE_ERROR("Built-in function requires exactly 1 argument", PARAMETER_ERROR,
                         currentToken);
        }
        argCounter = 0;
        onlyZeroArgs = false;
        onlyOneArg = false;
        onlyTwoArgs = false;
        return;
    }

    if (onlyZeroArgs) {
        HANDLE_ERROR("This built-in function takes no arguments", PARAMETER_ERROR, currentToken);
    }

    if (currentToken->type == TOKEN_TYPE_IDENTIFIER) {
        printTokenInfo(currentToken);
        getNextToken(currentToken);
    } else {
        parseExpression(ast, currentToken);
    }

    argCounter++;

    if (onlyOneArg && argCounter > 1) {
        HANDLE_ERROR("Built-in function requires exactly 1 argument", PARAMETER_ERROR,
                     currentToken);
    }

    if (currentToken->type == TOKEN_TYPE_COMMA) {
        printTokenInfo(currentToken);
        getNextToken(currentToken);
        parseArgs();
    }
}

int parse() {
    printf("Parsing started\n");

    ast = initAST();
    ASTNode *root = initASTNode();
    ast->root = root;
    currenParent = root;
    mainParent = root;

    parseProg();

    if (currentToken->type != TOKEN_TYPE_EOF) {
        fprintf(stderr, "Error: Unexpected token after parsing the program\n");
        return SYNTAX_ERROR;
    }

    displayAST(ast);

    return 0;
}
