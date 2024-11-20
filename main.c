#include "ast.h"
#include "helpers.h"
#include <stdio.h>

int main() {

    AST *newAST = initAST();
    newAST->root = mockASTProgramStructure(1);
    newAST->root->right = mockASTProgramStructure(2);
    newAST->root->right->right = mockASTProgramStructure(3);
    newAST->root->right->right->right = mockASTProgramStructure(4);
    newAST->root->right->right->right->right = mockASTProgramStructure(5);
    newAST->root->right->right->right->right->right = mockASTProgramStructure(6);
    newAST->root->right->right->right->right->right->right = mockASTProgramStructure(7);
    newAST->root->right->right->right->right->right->right->right = mockASTProgramStructure(8);
    newAST->root->right->right->right->right->right->right->right->right = mockASTProgramStructure(9);
    displayAST(newAST);

    freeAST(newAST);
    return 0;
}