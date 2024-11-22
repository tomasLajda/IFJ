#include <stdio.h>

#include "ast.h"
#include "code_generator.h"
#include "helpers.h"
#include "parser.h"
#include "scanner.h"
#include "semantic_analysis.h"

AST *ast;
Token *currentToken;
FILE *sourceFile;

int main() {
    currentToken = malloc(sizeof(Token));
    ast = initAST();
    ASTNode *root = initASTNode();
    ast->root = root;
    sourceFile = stdin;
    getNextToken(currentToken);

    parse();

    // displayEntireAST(ast);

    semanticAnalysis(ast);

    generateCode(stdout, ast);

    displayEntireAST(ast);

    return 0;
}