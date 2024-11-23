/*
 * IFJ Project
 * @brief Implementation file for parser
 *
 * @author Martin Valapka - xvalapm00
 */

#include "parser.h"
#include "helpers.h"
#include "testing_utils.h"
#include <string.h>

// Token* currentToken = NULL;
AST *ast = NULL;
ASTNode *mainParent = NULL;
ASTNode *currentParent = NULL;
TokenBuffer tokenBuffer = {NULL, NULL};
Token *decider = NULL;
bool voidFuncType = false;
bool parsingReturnType = false;
bool isFirstStatement = false; // In IF -> 1st statement left child, Otherwise right child
unsigned int argCounter = 0;
unsigned int paramCounter = 0;

void initTokenBuffer() {
    tokenBuffer.first = malloc(sizeof(Token));
    tokenBuffer.second = malloc(sizeof(Token));
}

void freeTokenBuffer() {
    if (tokenBuffer.first != NULL) {
        free(tokenBuffer.first);
        tokenBuffer.first = NULL;
    }
    if (tokenBuffer.second != NULL) {
        free(tokenBuffer.second);
        tokenBuffer.second = NULL;
    }
}

void goBack(ASTNode *startNode) {
    ASTNode *currentNode = startNode;

    while (currentNode != NULL) {
        if ((currentNode != startNode) &&
            (isTokenKeyword(currentNode->token, KEYWORD_IF) ||
             isTokenKeyword(currentNode->token, KEYWORD_WHILE) ||
             isTokenKeyword(currentNode->token, KEYWORD_PUB))) {
            currentParent = currentNode;
            mainParent = currentNode;
            break;
        }
        currentNode = currentNode->parent;
    }
}

bool isTokenKeyword(Token *token, Keyword keyword) {
    return (token->type == TOKEN_TYPE_KEYWORD && token->attribute.keyword == keyword);
}

bool isTokenBuiltInFunction(Token *token) {
    switch (token->attribute.keyword) {
    case KEYWORD_READSTR:
    case KEYWORD_READI32:
    case KEYWORD_READF64:
    case KEYWORD_STRING:
    case KEYWORD_LENGTH:
    case KEYWORD_CHR:
    case KEYWORD_I2F:
    case KEYWORD_F2I:
    case KEYWORD_CONCAT:
    case KEYWORD_STRCMP:
    case KEYWORD_ORD:
    case KEYWORD_SUBSTRING:
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
    if (!isTokenKeyword(currentToken, KEYWORD_CONST)) {
        HANDLE_ERROR("Expected 'const' in prolog", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
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

    if (!isTokenKeyword(currentToken, KEYWORD_PUB)) {
        HANDLE_ERROR("Expected 'pub' in function definition", SYNTAX_ERROR, currentToken);
    }

    ASTNode *pubNode = initASTNode();
    pubNode->token = copyToken(currentToken);
    addRightNode(ast, currentParent, pubNode);
    currentParent = pubNode;

    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (!isTokenKeyword(currentToken, KEYWORD_FN)) {
        HANDLE_ERROR("Expected 'fn' in function definition", SYNTAX_ERROR, currentToken);
    }

    ASTNode *fnNode = initASTNode();
    fnNode->token = copyToken(currentToken);
    addLeftNode(ast, currentParent, fnNode);
    currentParent = fnNode;
    mainParent = fnNode;

    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_IDENTIFIER &&
        !isTokenKeyword(currentToken, KEYWORD_MAIN)) {
        printTokenInfo(currentToken);
        HANDLE_ERROR("Expected function identifier in function definition", SYNTAX_ERROR, currentToken);
    }

    ASTNode *funcIdNode = initASTNode();
    funcIdNode->token = copyToken(currentToken);
    addLeftNode(ast, currentParent, funcIdNode);
    currentParent = funcIdNode;

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

    parseFuncType();
}

// FUNC_TYPE ::= V_FUNC | FUNC
void parseFuncType() {
    if (isTokenKeyword(currentToken, KEYWORD_VOID)) {
        voidFuncType = true;
        parseVoidFunc();
    } 
    else {
        voidFuncType = false;
        parseFunc();
    }
}

// FUNC ::= TYPE token_Ocb STATEMENTS token_Ccb
void parseFunc() {
    parsingReturnType = true;
    parseType();

    if (currentToken->type != TOKEN_TYPE_LEFT_CURLY_BR) {
        HANDLE_ERROR("Expected '{' in function definition", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseStatements();

    if (currentToken->type != TOKEN_TYPE_RIGHT_CURLY_BR) {
        HANDLE_ERROR("Expected '}' in function definition", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);
}

// V_FUNC ::= token_void token_Ocb ` token_Ccb
void parseVoidFunc() {
    if (!isTokenKeyword(currentToken, KEYWORD_VOID)) {
        HANDLE_ERROR("Expected 'void' in function definition", SYNTAX_ERROR, currentToken);
    }

    ASTNode *returnTypeNode = initASTNode();
    returnTypeNode->token = copyToken(currentToken);
    addRightNode(ast, mainParent, returnTypeNode);
    mainParent = mainParent->left;
    currentParent = mainParent;

    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_LEFT_CURLY_BR) {
        HANDLE_ERROR("Expected '{' in function definition", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseStatements();

    if (currentToken->type != TOKEN_TYPE_RIGHT_CURLY_BR) {
        HANDLE_ERROR("Expected '}' in function definition", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);
}

// TYPE ::= token_i32 | token_?i32 | token_f64 | token_?f64 | token_[]u8 | token_?[]u8
void parseType() {
    if (isTokenKeyword(currentToken, KEYWORD_I_32) || isTokenKeyword(currentToken, KEYWORD_F_64) ||
        isTokenKeyword(currentToken, KEYWORD_U_8_ARRAY) ||
        isTokenKeyword(currentToken, KEYWORD_I_32_NULL) ||
        isTokenKeyword(currentToken, KEYWORD_F_64_NULL) ||
        isTokenKeyword(currentToken, KEYWORD_U_8_ARRAY_NULL)) {

        if (parsingReturnType == false) {
            ASTNode *typeNode = initASTNode();
            typeNode->token = copyToken(currentToken);
            addLeftNode(ast, currentParent, typeNode);
        } 
        else {
            ASTNode *returnTypeNode = initASTNode();
            returnTypeNode->token = copyToken(currentToken);
            addRightNode(ast, mainParent, returnTypeNode);
            mainParent = mainParent->left;
            currentParent = mainParent;
        }

        printTokenInfo(currentToken);
        getNextToken(currentToken);
    } 
    else {
        printTokenInfo(currentToken);
        HANDLE_ERROR("Expected type in function definition", SYNTAX_ERROR, currentToken);
    }
}

// RETURN ::= token_return EXPR token_semicolon
// V_RETURN ::= token_return token_semicolon | ε
void parseReturn() {
    if (!isTokenKeyword(currentToken, KEYWORD_RETURN)) {
        if (!voidFuncType) {
            HANDLE_ERROR("Expected 'return' keyword", SYNTAX_ERROR, currentToken);
        }
        return;
    }

    ASTNode *returnNode = initASTNode();
    printTokenInfo(currentToken);
    returnNode->token = copyToken(currentToken);

    if (currentParent && currentParent->token->type == TOKEN_TYPE_EXPR &&
        currentParent->left == NULL && isFirstStatement == true) {
        addLeftNode(ast, currentParent, returnNode);
        isFirstStatement = false;
    } 
    else {
        addRightNode(ast, currentParent, returnNode);
    }
    currentParent = returnNode;

    printTokenInfo(currentToken);
    getNextToken(currentToken);

    // FUNCTION RETURN TYPE IS VOID
    if (voidFuncType) {
        if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
            HANDLE_ERROR("Expected ';' after 'return' in void function", SYNTAX_ERROR, currentToken);
        }
        printTokenInfo(currentToken);
        getNextToken(currentToken);
    }
    // FUNCTION RETURN TYPE IS NOT VOID 
    else {
        initTokenBuffer();
        tokenBuffer.first = copyToken(currentToken);

        // VARIABLE
        if (currentToken->type == TOKEN_TYPE_IDENTIFIER) {
            printTokenInfo(currentToken);
            getNextToken(currentToken);
            tokenBuffer.second = copyToken(currentToken);

            // FUNC_CALL - MAYBE IN THE FUTURE
            if (currentToken->type == TOKEN_TYPE_LEFT_BR) {
                HANDLE_ERROR("Unexpected '(' after identifier in return expression", SYNTAX_ERROR, currentToken);
            }

            // VARIABLE
            AST *exprTree = initAST();
            ASTNode *exprNode = initASTNode();
            exprTree->root = exprNode;
            parseExpression(exprTree, tokenBuffer.first, tokenBuffer.second, currentToken);
            exprNode->exprTree->isExpression = true;
            currentParent->exprTree = exprTree;

            goBack(currentParent);
            
        }
        // NUMERICAL VALUE 
        else {
            AST *exprTree = initAST();
            ASTNode *exprNode = initASTNode();
            exprTree->root = exprNode;
            parseExpression(exprTree, tokenBuffer.first, NULL, currentToken);
            exprNode->exprTree = exprTree;
            exprNode->exprTree->isExpression = true;
            currentParent->exprTree = exprTree;

            goBack(currentParent);
        }

        if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
            printTokenInfo(currentToken);
            HANDLE_ERROR("Expected ';' after return expression", SYNTAX_ERROR, currentToken);
        }
        printTokenInfo(currentToken);
        getNextToken(currentToken);
    }
}

// PARAMS ::= token_id token_colon TYPE NEXT_PARAM | ε
void parseParams() {
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
        addLeftNode(ast, currentParent, paramIdNode);
    } 
    else {
        addRightNode(ast, currentParent, paramIdNode);
    }
    currentParent = paramIdNode;

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

    if (isTokenKeyword(currentToken, KEYWORD_RETURN)) {
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
    parsingReturnType = false;
    argCounter = 0;

    switch (currentToken->type) {
    case TOKEN_TYPE_KEYWORD:
        if (isTokenKeyword(currentToken, KEYWORD_VAR) ||
            isTokenKeyword(currentToken, KEYWORD_CONST)) {
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
        // ifj.write(); 
        else if (isTokenKeyword(currentToken, KEYWORD_IFJ)) {
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
            decider = copyToken(currentToken);

            printTokenInfo(currentToken);
            getNextToken(currentToken);

            if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
                HANDLE_ERROR("Expected '(' after built-in function", SYNTAX_ERROR, currentToken);
            }
            parseFuncCall();
        } 
        else {
            printTokenInfo(currentToken);
            HANDLE_ERROR("Unexpected keyword in statement", SYNTAX_ERROR, currentToken);
        }
        break;

    case TOKEN_TYPE_IDENTIFIER:
        decider = copyToken(currentToken);

        printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (currentToken->type == TOKEN_TYPE_LEFT_BR) {
            parseFuncCall();
        } 
        else if (currentToken->type == TOKEN_TYPE_ASSIGN) {
            parseVarAss();
        } 
        else {
            printTokenInfo(currentToken);
            HANDLE_ERROR("Expected '(' or '=' after identifier", SYNTAX_ERROR, currentToken);
        }
        break;
    default:
        printTokenInfo(currentToken);
        HANDLE_ERROR("Unexpected token in statement", SYNTAX_ERROR, currentToken);
        break;
    }
}

// VAR_DEF ::= VAR_TYPE token_id TYPE_SPEC token_equals EXPR token_semicolon
void parseVarDef() {
    if (!isTokenKeyword(currentToken, KEYWORD_VAR) &&
        !isTokenKeyword(currentToken, KEYWORD_CONST)) {
        HANDLE_ERROR("Expected 'var' or 'const' in variable definition", SYNTAX_ERROR, currentToken);
    }

    printTokenInfo(currentParent->token);

    ASTNode *varTypeNode = initASTNode();
    varTypeNode->token = copyToken(currentToken);

    // FIRST STATEMENT IN IF
    if (isFirstStatement == true) {
        addLeftNode(ast, currentParent, varTypeNode);
        isFirstStatement = false;
    }
    // DEFAULT CASE
    else {
        addRightNode(ast, currentParent, varTypeNode);
    }
    currentParent = varTypeNode;
    mainParent = varTypeNode;

    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_IDENTIFIER) {
        HANDLE_ERROR("Expected variable identifier", SYNTAX_ERROR, currentToken);
    }

    ASTNode *varIdNode = initASTNode();
    varIdNode->token = copyToken(currentToken);
    addLeftNode(ast, currentParent, varIdNode);
    currentParent = varIdNode;

    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseTypeSpec();

    if (currentToken->type != TOKEN_TYPE_ASSIGN) {
        HANDLE_ERROR("Expected '=' in variable definition", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);
    initTokenBuffer();
    tokenBuffer.first = copyToken(currentToken);

    // BUILT-IN FUNCTION
    if (isTokenKeyword(currentToken, KEYWORD_IFJ)) {
        printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (currentToken->type != TOKEN_TYPE_DOT) {
            HANDLE_ERROR("Expected '.' after ifj", SYNTAX_ERROR, currentToken);
        }
        printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (!isTokenBuiltInFunction(currentToken)) {
            HANDLE_ERROR("Expected built-in function after '.'", UNDEFINED_ERROR, currentToken);
        }

        decider = copyToken(currentToken);
        printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
            HANDLE_ERROR("Expected '(' after built-in function", SYNTAX_ERROR, currentToken);
        }
        parseFuncCall();
    } 
    // VARIABLE || FUNC_CALL
    else if (currentToken->type == TOKEN_TYPE_IDENTIFIER) {
        ASTNode *funcIdNode = initASTNode();
        funcIdNode->token = copyToken(currentToken);
        funcIdNode->isAssignment = false;
        currentParent = funcIdNode;

        printTokenInfo(currentToken);
        getNextToken(currentToken);

        tokenBuffer.second = copyToken(currentToken);

        // FUNC_CALL
        if (currentToken->type == TOKEN_TYPE_LEFT_BR) {
            AST *exprTree = initAST();
            exprTree->root = funcIdNode;
            ASTNode *valueNode = initASTNode();
            valueNode->token = createToken(TOKEN_TYPE_EXPR);

            mainParent->left->right = valueNode;
            mainParent->left->right->exprTree = exprTree;
            exprTree->isExpression = false;
            currentParent = funcIdNode;

            parseFuncCall();
            currentParent = mainParent;
        } 
        // VARIABLE
        else {
            AST *exprTree = initAST();
            ASTNode *exprNode = initASTNode();
            exprTree->root = exprNode;
            parseExpression(exprTree, tokenBuffer.first, tokenBuffer.second, currentToken);
            exprNode->token = createToken(TOKEN_TYPE_EXPR);

            currentParent->exprTree = exprTree;
            currentParent->exprTree->isExpression = true;

            addLeftNode(ast, currentParent, exprNode);
            getNextToken(currentToken);
        }
    }
    // NUMERICAL VALUE 
    else if (currentToken->type != TOKEN_TYPE_IDENTIFIER) {
        AST *exprTree = initAST();
        ASTNode *exprNode = initASTNode();
        exprTree->root = exprNode;
        parseExpression(exprTree, tokenBuffer.first, NULL, currentToken);

        currentParent->exprTree = exprTree;
        currentParent->exprTree->isExpression = true;

        if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
            HANDLE_ERROR("Expected ';' at the end of variable definition", SYNTAX_ERROR, currentToken);
        }
        printTokenInfo(currentToken);
        getNextToken(currentToken);
    } 
    else {
        HANDLE_ERROR("Unexpected token in variable definition", SYNTAX_ERROR, currentToken);
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
    ASTNode *varIdNode = initASTNode();
    varIdNode->token = copyToken(decider);

    // FIRST STATEMENT IN IF
    if (isFirstStatement == true) {
        addLeftNode(ast, currentParent, varIdNode);
        isFirstStatement = false;
    } 
    // DEFAULT CASE 
    else {
        addRightNode(ast, currentParent, varIdNode);
    }
    currentParent = varIdNode;

    if (currentToken->type != TOKEN_TYPE_ASSIGN) {
        HANDLE_ERROR("Expected '=' after identifier in variable assignment", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    initTokenBuffer();
    tokenBuffer.first = copyToken(currentToken);

    // BUILT-IN FUNCTION
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
        decider = copyToken(currentToken);
        printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
            HANDLE_ERROR("Expected '(' after built-in function", SYNTAX_ERROR, currentToken);
        }
        parseFuncCall();
    } 
    // VARIABLE || FUNC_CALL
    else if (currentToken->type == TOKEN_TYPE_IDENTIFIER) {
        ASTNode *funcIdNode = initASTNode();
        funcIdNode->token = copyToken(currentToken);
        printTokenInfo(currentToken);
        getNextToken(currentToken);
        tokenBuffer.second = copyToken(currentToken);

        // FUNC_CALL - NEEDS FIX
        if (currentToken->type == TOKEN_TYPE_LEFT_BR) {
            AST *exprTree = initAST();
            exprTree->root = funcIdNode;
            ASTNode *valueNode = initASTNode();
            valueNode->token = createToken(TOKEN_TYPE_EXPR);
            mainParent->left = valueNode;
            mainParent->left->exprTree = exprTree;
            exprTree->isExpression = false;
            currentParent = funcIdNode;
            parseFuncCall();
            currentParent = mainParent;
        } 
        // VARIABLE
        else {
            AST *exprTree = initAST();
            ASTNode *exprNode = initASTNode();
            exprTree->root = exprNode;
            parseExpression(exprTree, tokenBuffer.first, tokenBuffer.second, currentToken);
            currentParent->exprTree = exprTree;
            currentParent->exprTree->isExpression = true;
            exprNode->token = createToken(TOKEN_TYPE_EXPR);
            addLeftNode(ast, currentParent, exprNode);
        }
    } 
    // NUMERICAL VALUE
    else if (currentToken->type != TOKEN_TYPE_IDENTIFIER) {
        AST *exprTree = initAST();
        ASTNode *exprNode = initASTNode();
        exprTree->root = exprNode;
        parseExpression(exprTree, tokenBuffer.first, NULL, currentToken);

        currentParent->exprTree = exprTree;
        currentParent->exprTree->isExpression = true;
        exprNode->token = createToken(TOKEN_TYPE_EXPR);
        addLeftNode(ast, currentParent, exprNode);

        if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
            printTokenInfo(currentToken);
            HANDLE_ERROR("Expected ';' at the end of variable assignment", SYNTAX_ERROR, currentToken);
        }
        printTokenInfo(currentToken);
        getNextToken(currentToken);
    } 
    else {
        HANDLE_ERROR("Unexpected token in variable assignment", SYNTAX_ERROR, currentToken);
    }
}

// While ::= token_while token_Orb EXPR token_Crb NULL_COND token_Ocb STATEMENTS token_Ccb
void parseWhile() {
    if (!isTokenKeyword(currentToken, KEYWORD_WHILE)) {
        HANDLE_ERROR("Expected 'while' at the beginning of while loop", SYNTAX_ERROR, currentToken);
    }

    ASTNode *whileNode = initASTNode();
    whileNode->token = copyToken(currentToken);

    // FIRST STATEMENT IN IF
    if (isFirstStatement == true) {
        addLeftNode(ast, currentParent, whileNode);
        isFirstStatement = false;
    } 
    // DEFAULT CASE
    else {
        addRightNode(ast, currentParent, whileNode);
    }
    currentParent = whileNode;
    mainParent = whileNode;

    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
        HANDLE_ERROR("Expected '(' after 'while'", SYNTAX_ERROR, currentToken);
    }

    AST *exprTree = initAST();
    ASTNode *exprNode = initASTNode();
    exprTree->root = exprNode;
    parseExpression(exprTree, NULL, NULL, currentToken);
    exprNode->exprTree = exprTree;
    exprNode->exprTree->isExpression = true;
    exprNode->token = createToken(TOKEN_TYPE_EXPR);

    addLeftNode(ast, currentParent, exprNode);
    currentParent = exprNode;

    if (currentToken->type != TOKEN_TYPE_RIGHT_BR) {
        HANDLE_ERROR("Expected ')' after expression in while loop", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type == TOKEN_TYPE_VB) {
        parseNullCond();
    }

    if (currentToken->type != TOKEN_TYPE_LEFT_CURLY_BR) {
        HANDLE_ERROR("Expected '{' to start the body of while loop", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseStatements();
    goBack(currentParent);

    if (currentToken->type != TOKEN_TYPE_RIGHT_CURLY_BR) {
        HANDLE_ERROR("Expected '}' to end the body of while loop", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);
}

// NULL_CONDITION ::= token_vb token_id token_vb | ε
void parseNullCond() {
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_IDENTIFIER) {
        HANDLE_ERROR("Expected identifier in null condition", SYNTAX_ERROR, currentToken);
    }

    ASTNode *nullNode = initASTNode();
    nullNode->token = createToken(TOKEN_TYPE_VB);
    nullNode->right = initASTNode();
    nullNode->right->token = copyToken(currentToken);

    currentParent->parent = nullNode;
    mainParent->left = nullNode;
    nullNode->left = currentParent;

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

    ASTNode *ifNode = initASTNode();
    ifNode->token = copyToken(currentToken);

    // FIRST STATEMENT IN IF
    if (isFirstStatement == true) {
        addLeftNode(ast, currentParent, ifNode);
        isFirstStatement = false;
    } 
    // DEFAULT CASE
    else {
        addRightNode(ast, currentParent, ifNode);
    }
    currentParent = ifNode;
    mainParent = ifNode;

    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
        HANDLE_ERROR("Expected '(' after 'if'", SYNTAX_ERROR, currentToken);
    }

    AST *exprTree = initAST();
    ASTNode *exprNode = initASTNode();
    exprTree->root = exprNode;
    parseExpression(exprTree, NULL, NULL, currentToken);
    exprNode->exprTree = exprTree;
    exprNode->exprTree->isExpression = true;
    exprNode->token = createToken(TOKEN_TYPE_EXPR);

    addLeftNode(ast, currentParent, exprNode);
    currentParent = exprNode;

    if (currentToken->type != TOKEN_TYPE_RIGHT_BR) {
        HANDLE_ERROR("Expected ')' after expression in if statement", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type == TOKEN_TYPE_VB) {
        parseNullCond();
    }

    if (currentToken->type != TOKEN_TYPE_LEFT_CURLY_BR) {
        HANDLE_ERROR("Expected '{' to start the body of if statement", SYNTAX_ERROR, currentToken);
    }
    isFirstStatement = true;
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseStatements();
    isFirstStatement = false;

    if (currentToken->type != TOKEN_TYPE_RIGHT_CURLY_BR) {
        HANDLE_ERROR("Expected '}' to end the body of if statement", SYNTAX_ERROR, currentToken);
    }
    currentParent = exprNode;
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseElse();
}

// ELSE ::= token_else token_Ocb STATEMENTS token_Ccb | ε
void parseElse() {
    if (!isTokenKeyword(currentToken, KEYWORD_ELSE)) {
        HANDLE_ERROR("Expected 'else' to start the else statement", SYNTAX_ERROR, currentToken);
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

// FUNC_CALL ::= token_Orb ARGS token_Crb token_semicolon
void parseFuncCall() {
    if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
        HANDLE_ERROR("Expected '(' after function identifier", SYNTAX_ERROR, currentToken);
    }
    printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseArgs();

    if (currentToken->type != TOKEN_TYPE_RIGHT_BR) {
        printTokenInfo(currentToken);
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
    // TODO: IMPLEMENT: EXPR = FUNC_CALL
    if (!isTokenKeyword(currentToken, KEYWORD_UNDERSCORE)) {
        HANDLE_ERROR("Expected '_' at the beginning of discard call", SYNTAX_ERROR, currentToken);
    }

    ASTNode *discardNode = initASTNode();
    discardNode->token = copyToken(currentToken);

    // FIRST STATEMENT IN IF
    if (isFirstStatement == true) {
        addLeftNode(ast, currentParent, discardNode);
        isFirstStatement = false;
    } 
    // DEFAULT CASE
    else {
        addRightNode(ast, currentParent, discardNode);
    }
    currentParent = discardNode;

    printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_ASSIGN) {
        HANDLE_ERROR("Expected '=' after '_' in discard call", SYNTAX_ERROR, currentToken);
    }

    printTokenInfo(currentToken);
    getNextToken(currentToken);

    initTokenBuffer();
    tokenBuffer.first = copyToken(currentToken);

    // BUILT-IN FUNCTION
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
        decider = copyToken(currentToken);
        printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
            HANDLE_ERROR("Expected '(' after built-in function", SYNTAX_ERROR, currentToken);
        }
        parseFuncCall();
    } 
    // VARIABLE || FUNC_CALL
    else if (currentToken->type == TOKEN_TYPE_IDENTIFIER) {
        printTokenInfo(currentToken);
        getNextToken(currentToken);

        tokenBuffer.second = copyToken(currentToken);

        // FUNC_CALL
        if (currentToken->type == TOKEN_TYPE_LEFT_BR) {
            parseFuncCall();
        } 
        // VARIABLE
        else {
            AST *exprTree = initAST();
            ASTNode *exprNode = initASTNode();
            exprTree->root = exprNode;
            parseExpression(exprTree, tokenBuffer.first, tokenBuffer.second, currentToken);
            currentParent->exprTree = exprTree;
            currentParent->exprTree->isExpression = true;
        }
    } 
    // NUMERICAL VALUE
    else {
        AST *exprTree = initAST();
        ASTNode *exprNode = initASTNode();
        exprTree->root = exprNode;
        parseExpression(exprTree, tokenBuffer.first, NULL, currentToken);
        currentParent->exprTree = exprTree;
        currentParent->exprTree->isExpression = true;
    }

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

    tokenBuffer.first = copyToken(currentToken);

    ASTNode *argNode = initASTNode();
    AST *exprTree = initAST();
    ASTNode *root = initASTNode();
    exprTree->root = root;
    exprTree->isExpression = true;
    argNode->exprTree = exprTree;
    argNode->token = createToken(TOKEN_TYPE_EXPR);

    parseExpression(exprTree, tokenBuffer.first, NULL, currentToken);

    if (argCounter == 0) {
        addLeftNode(ast, currentParent, argNode);
    } 
    else {
        addRightNode(ast, currentParent, argNode);
    }
    currentParent = argNode;

    argCounter++;

    if (currentToken->type == TOKEN_TYPE_COMMA) {
        printTokenInfo(currentToken);
        getNextToken(currentToken);
        parseArgs();
    }
}

int parse() {
    initTokenBuffer();
    ast = initAST();
    ASTNode *root = initASTNode();
    ast->root = root;
    currentParent = root;
    mainParent = root;

    parseProg();

    displayEntireAST(ast);
    free(decider);

    return 0;
}
