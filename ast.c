/*
 * IFJ Project
 * @brief Implementation file of AST
 *
 * @author Martin Valapka - xvalapm00
 *
 */

#include <stdbool.h>
#include "ast.h"
#include "error_codes.h"

AST* createAST() {
    AST* tree = (AST*) malloc(sizeof(AST));
    if (tree == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR, NULL);
    }
    tree->root = NULL;
    return tree;
}

ASTNode* createASTNode() {
    ASTNode* node = (ASTNode*) malloc(sizeof(ASTNode));
    if (node == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR, NULL);
    }
    node->isExpression = false;
    node->data.nodeType.keyword = KEYWORD_NULL; // ?
    node->parent = NULL;
    node->absParent = NULL;
    node->exprTree = NULL;
    node->token = NULL;
    node->children = NULL;
    node->childCount = 0;
    return node;
}

void freeNode(ASTNode* node) {
    if (node == NULL) {
        return;
    }
    if (node->isExpression) {
        freeNode(node->data.expr.left);
        freeNode(node->data.expr.right);
    }
    if (node->exprTree != NULL) {
        freeAST(node->exprTree);
    }
    if (node->token != NULL) {
        free(node->token);
    }

    free(node);
}

void freeAST(AST* ast) {
    if (ast == NULL) {
        return;
    }
    if (ast->root != NULL) {
        freeNode(ast->root);
    }

    free(ast);
}

void createExpressionTree(ASTNode* node) {
    if (node == NULL) {
        HANDLE_ERROR("NULL pointer passed to createExpressionTree", INTERNAL_ERROR, NULL);
    }

    AST* exprTree = createAST();
    if (exprTree == NULL) {
        HANDLE_ERROR("Failed to create expression tree", INTERNAL_ERROR, NULL);
    }

    ASTNode* opNode = createASTNode();
    if (opNode == NULL) {
        freeAST(exprTree);
        HANDLE_ERROR("Failed to create operation node", INTERNAL_ERROR, NULL);
    }

    opNode->isExpression = true;
    opNode->data.nodeType = node->data.nodeType;
    opNode->parent = NULL;
    opNode->absParent = node;

    exprTree->root = opNode;
    node->exprTree = exprTree;
}
