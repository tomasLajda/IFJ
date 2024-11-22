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

    ASTNode *whileStatement = mockASTProgramStructure(6);
    currentNode->right = whileStatement;
    currentNode = whileStatement;

    // ASTNode *functionCall = mockASTProgramStructure(2);
    // currentNode->right = functionCall;
    // currentNode = functionCall;

    ASTNode *variableAssignment = mockASTProgramStructure(9);
    variableAssignment->left->exprTree = initAST();
    variableAssignment->left->exprTree->root = mockASTProgramStructure(2);
    variableAssignment->left->exprTree->isExpression = false;
    currentNode->right = variableAssignment;
    currentNode = variableAssignment;

    currentNode = mockASTProgramStructure(3);
    currentNode->parent = previousNode->right;
    previousNode->right->right = currentNode;

    // displayEntireAST(ast);

    semanticAnalysis();
    // displayEntireAST(ast);
    // displayAST(ast->root->right->exprTree);
    return 0;
}