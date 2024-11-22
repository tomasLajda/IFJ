#include <stdio.h>

#include "ast.h"
#include "helpers.h"
#include "semantic_analysis.h"

AST *ast;

int main() {

    semanticAnalysis(ast);
    return 0;
}