#include "ast.h"
#include "code_generator.h"
#include "helpers.h"
#include <stdio.h>

int main() {

    AST *newAST = initAST();
    newAST->root = mockASTProgramStructure(3);
    // newAST->root->left->left->right = mockASTProgramStructure(4);
    // newAST->root->left->left->right->left->left = mockASTProgramStructure(2);
    // newAST->root->left->left->right->left->right = mockASTProgramStructure(9);
    displayAST(newAST);
    // displayAST(newAST->);

    FILE *outputFile = fopen("generated_code.ifjcode", "w");
    generateCode(outputFile, newAST);

    // freeAST(newAST);
    return 0;
}