#include "ast.h"
#include "helpers.h"
#include <stdio.h>

int main() {

    AST *newAST = initAST();
    newAST->root = mockASTProgramStructure(3);
    displayAST(newAST);
    // displayAST(newAST->);

    // freeAST(newAST);
    return 0;
}