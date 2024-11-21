#include "ast.h"
#include "code_generator.h"
#include "helpers.h"
#include <stdio.h>

int main() {

    float a = 3.14;
    printf("%a\n", a);
    // 0x1.d1eb851eb852p+2

    AST *newAST = initAST();
    newAST->root = mockASTProgramStructure(3);
    newAST->root->right = mockASTProgramStructure(30);
    newAST->root->right->left->left->right = mockASTProgramStructure(9);
    newAST->root->right->left->left->right->right = mockASTProgramStructure(20);
    // fun call ifj.write argument variableX

    // newAST->root->right->left->left->right->isAssignment = true;
    // newAST->root->right->left->left->right = mockASTProgramStructure(2);

    // newAST->root->right->left->left->right = mockASTProgramStructure(4);
    // newAST->root->right->left->left->right->left->left = mockASTProgramStructure(4);
    // newAST->root->right->left->left->right->left->right = mockASTProgramStructure(2);
    // newAST->root->right->left->left->right->left->left->left->left = mockASTProgramStructure(6);

    // newAST->root->left->left->right->left->left = mockASTProgramStructure(2);
    // newAST->root->left->left->right->left->right = mockASTProgramStructure(9);
    displayAST(newAST);
    printf("\n");
    printf("\n");
    printf("\n");
    displayEntireAST(newAST);
    // displayAST(newAST->);

    FILE *outputFile = fopen("generated_code.ifjcode", "w");
    generateCode(outputFile, newAST);

    // freeAST(newAST);
    return 0;
}