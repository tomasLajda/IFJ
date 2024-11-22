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

    ast = initAST();
    sourceFile = stdin;

    parse();

    semanticAnalysis(ast);

    generateCode(stdout, ast);

    displayEntireAST(ast);

    return 0;
}