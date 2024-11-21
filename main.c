#include <stdio.h>

#include "ast.h"
#include "helpers.h"
#include "semantic_analysis.h"

AST *ast = NULL;

int main() {
    ast = initAST();
    ast->root = initASTNode();
    ASTNode *previousNode = ast->root;
    ASTNode *currentNode = ast->root;
    currentNode = mockASTProgramStructure(30);
    currentNode->parent = previousNode;
    previousNode->right = currentNode;
    currentNode = mockASTProgramStructure(3);
    currentNode->parent = previousNode->right;
    previousNode->right->right = currentNode;
    displayAST(ast);

    semanticAnalysis();
    return 0;
}