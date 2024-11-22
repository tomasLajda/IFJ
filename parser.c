/*
 * IFJ Project
 * @brief Implementation file for parser
 *
 * @author Martin Valapka - xvalapm00
 */

// TODO: BUILT-IN FUNCTIONS, AST, _ = ifj.write("string"), FUNCCALL to AST, TODO nemozem, FUNCCALL
// OKEN_TYPE_IDENTIFIER a !TOKEN_TYPE_IDENTIFIER pri expr
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
bool onlyZeroArgs = false;
bool onlyOneArg = false;
bool onlyTwoArgs = false;
bool onlyThreeArgs = false;
bool parsingReturnType = false;
bool firstInTrue = false;
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

        // printf("I am in node: ");
        // displayASTNode(currentNode, 0, true);
        // printf("\n");

        if ((isTokenKeyword(currentNode->token, KEYWORD_IF) ||
             isTokenKeyword(currentNode->token, KEYWORD_WHILE) ||
             isTokenKeyword(currentNode->token, KEYWORD_PUB))) {
            currentParent = currentNode;
            mainParent = currentNode;
            break;
        }
        ASTNode *parentNode = currentNode->parent;
        currentNode = parentNode;
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
    //printTokenInfo(currentToken);

    if (!isTokenKeyword(currentToken, KEYWORD_CONST)) {
        HANDLE_ERROR("Expected 'const' in prolog", SYNTAX_ERROR, currentToken);
    }
    getNextToken(currentToken);

    if (!isTokenKeyword(currentToken, KEYWORD_IFJ)) {
        HANDLE_ERROR("Expected 'ifj' in prolog", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_ASSIGN) {
        HANDLE_ERROR("Expected '=' in prolog", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (!isTokenKeyword(currentToken, KEYWORD_IMPORT)) {
        HANDLE_ERROR("Expected '@import' in prolog", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
        HANDLE_ERROR("Expected ';' in prolog", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
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

    //printTokenInfo(currentToken);
    if (!isTokenKeyword(currentToken, KEYWORD_PUB)) {
        HANDLE_ERROR("Expected 'pub' in function definition", SYNTAX_ERROR, currentToken);
    }

    ASTNode *pubNode = initASTNode();
    pubNode->token = copyToken(currentToken);
    addRightNode(ast, currentParent, pubNode);
    currentParent = pubNode;

    // printf("\n");
    // displayAST(ast);
    // printf("\n");

    getNextToken(currentToken);

    if (!isTokenKeyword(currentToken, KEYWORD_FN)) {
        HANDLE_ERROR("Expected 'fn' in function definition", SYNTAX_ERROR, currentToken);
    }

    ASTNode *fnNode = initASTNode();
    fnNode->token = copyToken(currentToken);
    addLeftNode(ast, currentParent, fnNode);
    currentParent = fnNode;
    mainParent = fnNode;

    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_IDENTIFIER &&
        !isTokenKeyword(currentToken, KEYWORD_MAIN)) {
        //printTokenInfo(currentToken);
        HANDLE_ERROR("Expected function identifier in function definition", SYNTAX_ERROR,
                     currentToken);
    }

    ASTNode *funcIdNode = initASTNode();
    funcIdNode->token = copyToken(currentToken);
    addLeftNode(ast, currentParent, funcIdNode);
    currentParent = funcIdNode;

    // printf("\n");
    // displayAST(ast);
    // printf("\n");

    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
        HANDLE_ERROR("Expected '(' in function definition", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);
    parseParams();

    if (currentToken->type != TOKEN_TYPE_RIGHT_BR) {
        HANDLE_ERROR("Expected ')' in function definition", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);
    parseFuncType();
}

// FUNC_TYPE ::= V_FUNC | FUNC
void parseFuncType() {
    if (isTokenKeyword(currentToken, KEYWORD_VOID)) {
        voidFuncType = true;
        parseVoidFunc();
    } else {
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
    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseStatements();

    if (currentToken->type != TOKEN_TYPE_RIGHT_CURLY_BR) {
        HANDLE_ERROR("Expected '}' in function definition", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);
}

// V_FUNC ::= token_void token_Ocb ` token_Ccb
void parseVoidFunc() {
    if (!isTokenKeyword(currentToken, KEYWORD_VOID)) {
        HANDLE_ERROR("Expected 'void' in function definition", SYNTAX_ERROR, currentToken);
    }

    ASTNode *voidReturnTypeNode = initASTNode();
    voidReturnTypeNode->token = copyToken(currentToken);
    addRightNode(ast, mainParent, voidReturnTypeNode);
    mainParent = mainParent->left;
    currentParent = mainParent;

    // printf("\n");
    // displayAST(ast);
    // printf("\n");

    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_LEFT_CURLY_BR) {
        HANDLE_ERROR("Expected '{' in function definition", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseStatements();

    if (currentToken->type != TOKEN_TYPE_RIGHT_CURLY_BR) {
        HANDLE_ERROR("Expected '}' in function definition", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
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
        } else {
            ASTNode *returnTypeNode = initASTNode();
            returnTypeNode->token = copyToken(currentToken);
            addRightNode(ast, mainParent, returnTypeNode);
            mainParent = mainParent->left;
            currentParent = mainParent;
        }

        //printTokenInfo(currentToken);
        getNextToken(currentToken);
    } else {
        //printTokenInfo(currentToken);
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

    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (voidFuncType) {
        if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
            HANDLE_ERROR("Expected ';' after 'return' in void function", SYNTAX_ERROR,
                         currentToken);
        }
        //printTokenInfo(currentToken);
        getNextToken(currentToken);
    } else {
        initTokenBuffer();
        //printTokenInfo(currentToken);
        tokenBuffer.first = copyToken(currentToken);

        if (currentToken->type == TOKEN_TYPE_IDENTIFIER) {
            //printTokenInfo(currentToken);
            getNextToken(currentToken);
            tokenBuffer.second = copyToken(currentToken);
            // printf("Token 1: ");
            //  printTokenInfo(currentToken);

            if (currentToken->type == TOKEN_TYPE_LEFT_BR) {
                parseFuncCall();
            } else {
                AST *exprTree = initAST();
                ASTNode *exprNode = initASTNode();
                exprTree->root = exprNode;
                // printf("BUFFER 1 %s\n", TokenTypeToString(tokenBuffer.first->type));
                // printf("BUFFER 2 %s\n", TokenTypeToString(tokenBuffer.second->type));
                parseExpression(exprTree, tokenBuffer.first, tokenBuffer.second, currentToken);
                // printf("Expression tree23:\n");
                // displayAST(exprTree);

                currentParent->exprTree = exprTree;

                // printf("Expression tree:\n");
                // displayAST(exprTree);
                // printf("CAU: ");
                //printTokenInfo(currentToken);
            }
        } else {
            AST *exprTree = initAST();
            ASTNode *exprNode = initASTNode();
            exprTree->root = exprNode;
            parseExpression(exprTree, tokenBuffer.first, NULL, currentToken);
            exprNode->exprTree = exprTree;
            exprNode->exprTree->isExpression = true;
            currentParent->exprTree = exprTree;

            // printf("Expression tree:\n");
            // displayAST(exprTree);

            goBack(currentParent);
        }

        if (currentToken->type == TOKEN_TYPE_RIGHT_BR) {
            //printTokenInfo(currentToken);
            getNextToken(currentToken);

            if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
                //printTokenInfo(currentToken);
                HANDLE_ERROR("Expected ';' after return expression", SYNTAX_ERROR, currentToken);
            }
        }

        else if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
            //printTokenInfo(currentToken);
            HANDLE_ERROR("Expected ';' after return expression", SYNTAX_ERROR, currentToken);
        }
        //printTokenInfo(currentToken);
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
    } else {
        addRightNode(ast, currentParent, paramIdNode);
    }
    currentParent = paramIdNode;

    // printf("\n");
    // displayAST(ast);
    // printf("\n");

    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_COLON) {
        HANDLE_ERROR("Expected ':' after parameter identifier", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseType();

    if (currentToken->type == TOKEN_TYPE_COMMA) {
        //printTokenInfo(currentToken);
        getNextToken(currentToken);
        parseParams();
    }
}

// STATEMENTS ::= STATEMENT STATEMENTS | ε
void parseStatements() {
    if (currentToken->type == TOKEN_TYPE_RIGHT_CURLY_BR) {
        return;
    }

    //printf("\n");
    //displayAST(ast);
    //printf("\n");

    if (isTokenKeyword(currentToken, KEYWORD_RETURN)) {
        //printf("I am in return\n");
        ASTNode *returnNode = initASTNode();
        //printTokenInfo(currentToken);
        returnNode->token = copyToken(currentToken);

        //printf("Curren parent: ");
        //printTokenInfo(currentParent->token);
        // printf("Main parent: ");
        // printTokenInfo(mainParent->token);

        if (currentParent && currentParent->token->type == TOKEN_TYPE_EXPR &&
            currentParent->left == NULL && firstInTrue == true) {
            addLeftNode(ast, currentParent, returnNode);
            firstInTrue = false;
        } else {
            // printf("Curren parent: ");
            // displayASTNode(currentParent, 0, true);
            addRightNode(ast, currentParent, returnNode);
        }
        currentParent = returnNode;

        //printf("\n");
        //displayAST(ast);
        //printf("\n");

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
    parsingReturnType = false;
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
            //printTokenInfo(currentToken);
            getNextToken(currentToken);

            if (currentToken->type != TOKEN_TYPE_DOT) {
                HANDLE_ERROR("Unexpected token in built-in function", SYNTAX_ERROR, currentToken);
            }
            //printTokenInfo(currentToken);
            getNextToken(currentToken);

            if (!isTokenKeyword(currentToken, KEYWORD_WRITE)) {
                HANDLE_ERROR("Unexpected built-in function in function call", SYNTAX_ERROR,
                             currentToken);
            }
            decider = copyToken(currentToken);

            //printTokenInfo(currentToken);
            getNextToken(currentToken);

            if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
                HANDLE_ERROR("Expected '(' after built-in function", SYNTAX_ERROR, currentToken);
            }
            onlyOneArg = true;
            parseFuncCall();
        } else {
            //printTokenInfo(currentToken);
            HANDLE_ERROR("Unexpected keyword in statement", SYNTAX_ERROR, currentToken);
        }
        break;

    case TOKEN_TYPE_IDENTIFIER:
        decider = copyToken(currentToken);

        //printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (currentToken->type == TOKEN_TYPE_LEFT_BR) {
            parseFuncCall();
        } else if (currentToken->type == TOKEN_TYPE_ASSIGN) {
            parseVarAss();
        } else {
            // printf("CAU ");
            //printTokenInfo(currentToken);
            HANDLE_ERROR("Expected '(' or '=' after identifier", SYNTAX_ERROR, currentToken);
        }
        break;
    default:
        //printTokenInfo(currentToken);
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

    //printf("Curren parent vo vardef: ");
    //printTokenInfo(currentParent->token);

    ASTNode *varTypeNode = initASTNode();
    varTypeNode->token = copyToken(currentToken);

    if (firstInTrue == true) {
        addLeftNode(ast, currentParent, varTypeNode);
        firstInTrue = false;
    } else {
        addRightNode(ast, currentParent, varTypeNode);
    }
    currentParent = varTypeNode;
    mainParent = varTypeNode;

    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_IDENTIFIER) {
        HANDLE_ERROR("Expected variable identifier", SYNTAX_ERROR, currentToken);
    }

    ASTNode *varIdNode = initASTNode();
    varIdNode->token = copyToken(currentToken);
    addLeftNode(ast, currentParent, varIdNode);
    currentParent = varIdNode;

    //printf("\n");
    //displayAST(ast);
    //printf("\n");

    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseTypeSpec();

    if (currentToken->type != TOKEN_TYPE_ASSIGN) {
        HANDLE_ERROR("Expected '=' in variable definition", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);
    initTokenBuffer();
    tokenBuffer.first = copyToken(currentToken);

    if (isTokenKeyword(currentToken, KEYWORD_IFJ)) {
        //printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (currentToken->type != TOKEN_TYPE_DOT) {
            HANDLE_ERROR("Expected '.' after ifj", SYNTAX_ERROR, currentToken);
        }
        //printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (!isTokenBuiltInFunction(currentToken)) {
            HANDLE_ERROR("Expected built-in function after '.'", UNDEFINED_ERROR, currentToken);
        }

        decider = copyToken(currentToken);
        //printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
            HANDLE_ERROR("Expected '(' after built-in function", SYNTAX_ERROR, currentToken);
        }
        parseFuncCall();
    } else if (currentToken->type == TOKEN_TYPE_IDENTIFIER) {
        ASTNode *funcID = initASTNode();
        funcID->token = copyToken(currentToken);
        funcID->isAssignment = false;
        currentParent = funcID;

        //printTokenInfo(currentToken);
        getNextToken(currentToken);
        tokenBuffer.second = copyToken(currentToken);

        if (currentToken->type == TOKEN_TYPE_LEFT_BR) {
            AST *exprTree = initAST();
            exprTree->root = funcID;
            ASTNode *valueNode = initASTNode();
            valueNode->token = createToken(TOKEN_TYPE_EXPR);
            mainParent->left->right = valueNode;
            mainParent->left->right->exprTree = exprTree;
            exprTree->isExpression = false;
            // funcID->exprTree->root =
            currentParent = funcID;
            parseFuncCall();
            currentParent = mainParent;
        } else {
            AST *exprTree = initAST();
            ASTNode *exprNode = initASTNode();
            exprTree->root = exprNode;
            // printf("PRED EX: \n");
            // printTokenInfo(currentToken);
            // printTokenInfo(tokenBuffer.first);
            // printTokenInfo(tokenBuffer.second);
            parseExpression(exprTree, tokenBuffer.first, tokenBuffer.second, currentToken);
            // printf("%d\n", result);
            // printf("PO EX ");
            //printTokenInfo(currentToken);
            currentParent->exprTree = exprTree;
            currentParent->exprTree->isExpression = true;
            exprNode->token = createToken(TOKEN_TYPE_EXPR);
            addLeftNode(ast, currentParent, exprNode);
            getNextToken(currentToken);
            // displayAST(exprTree);
        }
    } else if (currentToken->type != TOKEN_TYPE_IDENTIFIER) {
        AST *exprTree = initAST();
        ASTNode *exprNode = initASTNode();
        exprTree->root = exprNode;
        // printf("ahojki%s\n", TokenTypeToString(currentToken->type));
        parseExpression(exprTree, tokenBuffer.first, NULL, currentToken);
        // displayAST(exprTree);
        currentParent->exprTree = exprTree;
        currentParent->exprTree->isExpression = true;

        // printf("Expression tree for vardef:\n");
        // displayAST(currentParent->exprTree);

        // printf("\n");
        // displayAST(ast);
        // printf("\n");

        if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
            //printTokenInfo(currentToken);
            HANDLE_ERROR("Expected ';' at the end of variable definition", SYNTAX_ERROR, currentToken);
        }
        //printTokenInfo(currentToken);
        getNextToken(currentToken);
    } else {
        HANDLE_ERROR("Unexpected token in variable definition", SYNTAX_ERROR, currentToken);
    }
}

// TYPE_SPEC ::= token_colon TYPE | ε
void parseTypeSpec() {
    if (currentToken->type != TOKEN_TYPE_COLON) {
        return;
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseType();
}

// VAR_ASS ::= token_id token_equals EXPR token_semicolon
void parseVarAss() {
    ASTNode *varID = initASTNode();
    varID->token = copyToken(decider);

    if (firstInTrue == true) {
        addLeftNode(ast, currentParent, varID);
        firstInTrue = false;
    } else {
        addRightNode(ast, currentParent, varID);
    }

    currentParent = varID;

    if (currentToken->type != TOKEN_TYPE_ASSIGN) {
        HANDLE_ERROR("Expected '=' after identifier in variable assignment", SYNTAX_ERROR,
                     currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    initTokenBuffer();
    tokenBuffer.first = copyToken(currentToken);

    if (isTokenKeyword(currentToken, KEYWORD_IFJ)) {
        //printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (currentToken->type != TOKEN_TYPE_DOT) {
            HANDLE_ERROR("Expected '.' after ifj", SYNTAX_ERROR, currentToken);
        }
        //printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (!isTokenBuiltInFunction(currentToken)) {
            HANDLE_ERROR("Expected built-in function after '.'", SYNTAX_ERROR, currentToken);
        }
        decider = copyToken(currentToken);
        //printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
            HANDLE_ERROR("Expected '(' after built-in function", SYNTAX_ERROR, currentToken);
        }
        parseFuncCall();
    } else if (currentToken->type == TOKEN_TYPE_IDENTIFIER) {
        ASTNode *funcID = initASTNode();
        funcID->token = copyToken(currentToken);
        //printTokenInfo(currentToken);
        getNextToken(currentToken);
        tokenBuffer.second = copyToken(currentToken);

        if (currentToken->type == TOKEN_TYPE_LEFT_BR) {
            AST *exprTree = initAST();
            exprTree->root = funcID;
            ASTNode *valueNode = initASTNode();
            valueNode->token = createToken(TOKEN_TYPE_EXPR);
            mainParent->left = valueNode;
            mainParent->left->exprTree = exprTree;
            exprTree->isExpression = false;
            currentParent = funcID;
            parseFuncCall();
            currentParent = mainParent;
        } else {
            AST *exprTree = initAST();
            ASTNode *exprNode = initASTNode();
            exprTree->root = exprNode;
            parseExpression(exprTree, tokenBuffer.first, tokenBuffer.second, currentToken);
            currentParent->exprTree = exprTree;
            currentParent->exprTree->isExpression = true;
            exprNode->token = createToken(TOKEN_TYPE_EXPR);
            addLeftNode(ast, currentParent, exprNode);
        }
    } else if (currentToken->type != TOKEN_TYPE_IDENTIFIER) {
        AST *exprTree = initAST();
        ASTNode *exprNode = initASTNode();
        exprTree->root = exprNode;
        parseExpression(exprTree, tokenBuffer.first, NULL, currentToken);

        currentParent->exprTree = exprTree;
        currentParent->exprTree->isExpression = true;
        exprNode->token = createToken(TOKEN_TYPE_EXPR);
        addLeftNode(ast, currentParent, exprNode);

        //printTokenInfo(currentToken);
        // displayAST(exprTree);

        // printf("\n");
        // displayAST(ast);
        // printf("\n");

        if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
            //printTokenInfo(currentToken);
            HANDLE_ERROR("Expected ';' at the end of variable assignment", SYNTAX_ERROR,
                         currentToken);
        }
    } else {
        HANDLE_ERROR("Unexpected token in variable assignment", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);
}

// While ::= token_while token_Orb EXPR token_Crb NULL_COND token_Ocb STATEMENTS token_Ccb
void parseWhile() {
    if (!isTokenKeyword(currentToken, KEYWORD_WHILE)) {
        HANDLE_ERROR("Expected 'while' at the beginning of while loop", SYNTAX_ERROR, currentToken);
    }

    ASTNode *whileNode = initASTNode();
    whileNode->token = copyToken(currentToken);

    if (firstInTrue == true) {
        addLeftNode(ast, currentParent, whileNode);
        firstInTrue = false;
    } else {
        addRightNode(ast, currentParent, whileNode);
    }
    currentParent = whileNode;
    mainParent = whileNode;

    //printTokenInfo(currentToken);
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

    // rintf("Expression tree for while:\n");
    // isplayAST(exprNode->exprTree);

    if (currentToken->type != TOKEN_TYPE_RIGHT_BR) {
        HANDLE_ERROR("Expected ')' after expression in while loop", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type == TOKEN_TYPE_VB) {
        parseNullCond();
    }

    if (currentToken->type != TOKEN_TYPE_LEFT_CURLY_BR) {
        HANDLE_ERROR("Expected '{' to start the body of while loop", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseStatements();
    goBack(currentParent);

    if (currentToken->type != TOKEN_TYPE_RIGHT_CURLY_BR) {
        HANDLE_ERROR("Expected '}' to end the body of while loop", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);
}

// NULL_CONDITION ::= token_vb token_id token_vb | ε
void parseNullCond() {
    // bool isNullCond = true;
    //printf("SOM TU\n");
    //printTokenInfo(currentToken);
    getNextToken(currentToken);
    //printf("SOM TUgvgvv\n");
    //printTokenInfo(currentToken);

    if (currentToken->type != TOKEN_TYPE_IDENTIFIER) {
        //printf("SOM TUfdfffdfd\n");
        HANDLE_ERROR("Expected identifier in null condition", SYNTAX_ERROR, currentToken);
    }
    //printf("fjididffjfdfdfdf");
    //printf("%s", TokenTypeToString(currentToken->type));
    // displayAST(ast);
    ASTNode *nullNode = initASTNode();
    nullNode->token = createToken(TOKEN_TYPE_VB);
    nullNode->right = initASTNode();
    nullNode->right->token = copyToken(currentToken);

    // goBack(currentParent);

    currentParent->parent = nullNode;
    mainParent->left = nullNode;
    nullNode->left = currentParent;

    // displayAST(ast);
    // printf("Curren parent 1: ");
    // displayASTNode(currentParent, 0, true);
    // printf("Curren parent: ");
    // displayASTNode(currentParent, 0, true);
    // displayAST(ast);

    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_VB) {
        HANDLE_ERROR("Expected '|' after identifier in null condition", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);
}

// IF ::= token_if token_Orb EXPR token_Crb NULL_COND token_Ocb STATEMENTS token_Ccb ELSE
void parseIf() {
    if (!isTokenKeyword(currentToken, KEYWORD_IF)) {
        HANDLE_ERROR("Expected 'if' at the beginning of if statement", SYNTAX_ERROR, currentToken);
    }

    ASTNode *ifNode = initASTNode();
    ifNode->token = copyToken(currentToken);

    if (firstInTrue == true) {
        addLeftNode(ast, currentParent, ifNode);
        firstInTrue = false;
    } else {
        addRightNode(ast, currentParent, ifNode);
    }
    currentParent = ifNode;
    mainParent = ifNode;

    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
        HANDLE_ERROR("Expected '(' after 'if'", SYNTAX_ERROR, currentToken);
    }

    AST *exprTree = initAST();
    ASTNode *exprNode = initASTNode();
    exprTree->root = exprNode;
    parseExpression(exprTree, NULL, NULL, currentToken);
    //printf("TENTO: ");
    //printTokenInfo(currentToken);
    exprNode->exprTree = exprTree;
    exprNode->exprTree->isExpression = true;
    exprNode->token = createToken(TOKEN_TYPE_EXPR);

    //printf("TENTO: ");
    //printTokenInfo(currentToken);

    //printf("Expression tree before:\n");
    //printTokenInfo(currentParent->token);

    //printf("TENTO: ");
    //printTokenInfo(currentToken);
    addLeftNode(ast, currentParent, exprNode);
    //printf("TENTO: ");
    //printTokenInfo(currentToken);

    //printf("ODTIALTO: \n");
    //printf("%s", TokenTypeToString(currentParent->token->type));
    currentParent = exprNode;
    //printf("%s", TokenTypeToString(currentParent->token->type));
    //printf("%s", TokenTypeToString(currentToken->type));

    //printf("Expression tree currentParent for if:\n");
    //printTokenInfo(currentParent->token);

    if (currentToken->type != TOKEN_TYPE_RIGHT_BR) {
        HANDLE_ERROR("Expected ')' after expression in if statement", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
    //printf("Current token: ");
    getNextToken(currentToken);

    if (currentToken->type == TOKEN_TYPE_VB) {
        parseNullCond();
    }

    if (currentToken->type != TOKEN_TYPE_LEFT_CURLY_BR) {
        HANDLE_ERROR("Expected '{' to start the body of if statement", SYNTAX_ERROR, currentToken);
    }
    firstInTrue = true;
    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseStatements();
    firstInTrue = false;

    if (currentToken->type != TOKEN_TYPE_RIGHT_CURLY_BR) {
        HANDLE_ERROR("Expected '}' to end the body of if statement", SYNTAX_ERROR, currentToken);
    }
    currentParent = exprNode;
    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseElse();
}

// ELSE ::= token_else token_Ocb STATEMENTS token_Ccb | ε
void parseElse() {
    //printf("Current token: ");
    //printTokenInfo(currentToken);

    if (!isTokenKeyword(currentToken, KEYWORD_ELSE)) {
        HANDLE_ERROR("Expected 'else' to start the else statement", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_LEFT_CURLY_BR) {
        HANDLE_ERROR("Expected '{' to start the body of else statement", SYNTAX_ERROR,
                     currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseStatements();

    if (currentToken->type != TOKEN_TYPE_RIGHT_CURLY_BR) {
        HANDLE_ERROR("Expected '}' to end the body of else statement", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);
}

// FUNC_CALL ::= token_Orb ARGS token_Crb token_semicolon
void parseFuncCall() {
    if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
        HANDLE_ERROR("Expected '(' after function identifier", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    parseArgs();

    if (currentToken->type != TOKEN_TYPE_RIGHT_BR) {
        //printTokenInfo(currentToken);
        HANDLE_ERROR("Expected ')' after arguments in function call", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
        HANDLE_ERROR("Expected ';' at the end of function call", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);
}

// DISCARD_CALL ::= token_underscore token_equals EXPR token_semicolon
void parseDiscardCall() {
    if (!isTokenKeyword(currentToken, KEYWORD_UNDERSCORE)) {
        HANDLE_ERROR("Expected '_' at the beginning of discard call", SYNTAX_ERROR, currentToken);
    }

    ASTNode *discardNode = initASTNode();
    discardNode->token = copyToken(currentToken);

    if (firstInTrue == true) {
        addLeftNode(ast, currentParent, discardNode);
        firstInTrue = false;
    } else {
        addRightNode(ast, currentParent, discardNode);
    }
    currentParent = discardNode;

    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    if (currentToken->type != TOKEN_TYPE_ASSIGN) {
        HANDLE_ERROR("Expected '=' after '_' in discard call", SYNTAX_ERROR, currentToken);
    }

    //printTokenInfo(currentToken);
    getNextToken(currentToken);

    initTokenBuffer();
    tokenBuffer.first = copyToken(currentToken);

    if (isTokenKeyword(currentToken, KEYWORD_IFJ)) {
        //printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (currentToken->type != TOKEN_TYPE_DOT) {
            HANDLE_ERROR("Expected '.' after ifj", SYNTAX_ERROR, currentToken);
        }
        //printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (!isTokenBuiltInFunction(currentToken)) {
            HANDLE_ERROR("Expected built-in function after '.'", SYNTAX_ERROR, currentToken);
        }
        decider = copyToken(currentToken);
        //printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (currentToken->type != TOKEN_TYPE_LEFT_BR) {
            HANDLE_ERROR("Expected '(' after built-in function", SYNTAX_ERROR, currentToken);
        }
        parseFuncCall();
    } else if (currentToken->type == TOKEN_TYPE_IDENTIFIER) {
        //printTokenInfo(currentToken);
        getNextToken(currentToken);

        tokenBuffer.second = copyToken(currentToken);
        // printf("2. Token in discardCall ");
        //printTokenInfo(currentToken);

        if (currentToken->type == TOKEN_TYPE_LEFT_BR) {
            parseFuncCall();
        } else {
            AST *exprTree = initAST();
            ASTNode *exprNode = initASTNode();
            exprTree->root = exprNode;
            parseExpression(exprTree, tokenBuffer.first, tokenBuffer.second, currentToken);
            currentParent->exprTree = exprTree;
            currentParent->exprTree->isExpression = true;

            // printf("Expression tree in discardCall:\n");
            // displayAST(exprTree);

            // printf("TOKEN IN DISCARD_CALL: ");
            //printTokenInfo(currentToken);
        }
    } else {
        AST *exprTree = initAST();
        ASTNode *exprNode = initASTNode();
        exprTree->root = exprNode;
        parseExpression(exprTree, tokenBuffer.first, NULL, currentToken);
        currentParent->exprTree = exprTree;
        currentParent->exprTree->isExpression = true;

        // printf("Expression tree in discardCall:\n");
        // displayAST(exprTree);

        // printf("\n");
        // displayAST(ast);
        // printf("\n");
    }

    if (currentToken->type == TOKEN_TYPE_RIGHT_BR) {
        //printTokenInfo(currentToken);
        getNextToken(currentToken);

        if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
            //printTokenInfo(currentToken);
            HANDLE_ERROR("Expected ';' after discard expression", SYNTAX_ERROR, currentToken);
        }
    }

    else if (currentToken->type != TOKEN_TYPE_SEMICOLON) {
        HANDLE_ERROR("Expected ';' at the end of discard call", SYNTAX_ERROR, currentToken);
    }
    //printTokenInfo(currentToken);
    getNextToken(currentToken);
}

// ARGS ::= (EXPR | token_id) NEXT_ARG | ε
void parseArgs() {
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
    } else {
        addRightNode(ast, currentParent, argNode);
    }
    currentParent = argNode;

    argCounter++;

    if (currentToken->type == TOKEN_TYPE_COMMA) {
        //printTokenInfo(currentToken);
        getNextToken(currentToken);
        parseArgs();
    }
}

int parse() {
    //printf("Parsing started\n");

    initTokenBuffer();
    ast = initAST();
    ASTNode *root = initASTNode();
    ast->root = root;
    currentParent = root;
    mainParent = root;

    parseProg();

    if (currentToken->type != TOKEN_TYPE_EOF) {
        HANDLE_ERROR("Expected EOF at the end of the program", SYNTAX_ERROR, currentToken);
    }

    //printf("FINAL PRINT: \n");
    //displayEntireAST(ast);
    free(decider);
    // freeTokenBuffer();

    return 0;
}
