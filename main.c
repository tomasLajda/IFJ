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
    sourceFile = fopen("test1.txt", "r");
    int i = 0;
    int result = 0;

    AST *exprAST = initAST();
    ASTNode *root = initASTNode();
    exprAST->root = root;
    Token *token = createToken(TOKEN_TYPE_EMPTY);
    root->token = token;
    printf("root token type: %d\n", root->token->type);

    // while (token->type != TOKEN_TYPE_EOF) {
    result = parseExpression(exprAST, token);
    if (result == SYNTAX_ERROR) {
        printf("Syntax error\n");
        printf("%d. delimiter token type: %s\n", ++i, TokenTypeToString(token->type));
    } else if (result == INTERNAL_ERROR) {
        printf("Internal error\n");
        printf("%d. delimiter token type: %s\n", ++i, TokenTypeToString(token->type));
    } else {
        printf("Successful parsing\n");
        displayAST(exprAST);
        printf("%d. delimiter token type: %s\n", ++i, TokenTypeToString(token->type));
    }
    // }

    free(token);
    free(root);
    fclose(sourceFile);
    freeAST(exprAST);
    return 0;
}