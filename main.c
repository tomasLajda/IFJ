#include "ast.h"
#include "enums.h"
#include "error_codes.h"
#include "expr-parser.h"
#include "helpers.h"
#include "scanner.h"
#include "stack.h"
#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>

FILE *sourceFile;

int main() {
    sourceFile = fopen("testCORRECT.txt", "r");
    int i = 0;

    AST *exprAST = initAST();
    ASTNode *root = initASTNode();
    exprAST->root = root;
    Token *token = createToken(TOKEN_TYPE_EMPTY);

    while (token->type != TOKEN_TYPE_EOF) {
        parseExpression(exprAST, token);
        displayAST(exprAST);
        printf("%d. delimiter token type: %s\n", ++i, TokenTypeToString(token->type));
    }

    fclose(sourceFile);
    freeAST(exprAST);
    return 0;
}