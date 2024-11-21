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

    ASTNode *discard = mockASTProgramStructure(1);
    currentNode->left->left->right = discard;
    currentNode = discard;

    ASTNode *variableDefinition = mockASTProgramStructure(8);
    currentNode->right = variableDefinition;
    currentNode = variableDefinition;

    ASTNode *ifStatement = mockASTProgramStructure(4);
    currentNode->right = ifStatement;
    currentNode = ifStatement;

    ASTNode *nullIfStatement = mockASTProgramStructure(5);
    currentNode->left->left = nullIfStatement;
    currentNode = nullIfStatement;

    currentNode = mockASTProgramStructure(3);
    currentNode->parent = previousNode->right;
    previousNode->right->right = currentNode;
    displayAST(ast);

    semanticAnalysis();
    return 0;
}