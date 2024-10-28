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
    node->nodeType.tokenType = TOKEN_TYPE_EMPTY;
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
    if (node->nodeType.tokenType >= TOKEN_TYPE_IDENTIFIER && node->nodeType.tokenType <= TOKEN_TYPE_DIV) {
        freeNode(node->left);
        freeNode(node->right);
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

    node->isExpression = true;

    AST* exprTree = createAST();
    if (exprTree == NULL) {
        HANDLE_ERROR("Failed to create expression tree", INTERNAL_ERROR, NULL);
    }
    exprTree->root = node;
    node->exprTree = exprTree;

    node->left = NULL;
    node->right = NULL;
}

void addNode(AST* ast, ASTNode* parent, ASTNode* node) {
    if (ast == NULL || node == NULL) {
        HANDLE_ERROR("NULL pointer passed to addNode", INTERNAL_ERROR, NULL);
    }

    if (ast->root == NULL) {
        ast->root = node;
        node->parent = NULL;
        return;
    }

    if (parent->left == NULL) {
        parent->left = node;
        node->parent = parent;
    } 
    else if (parent->right == NULL) {
        parent->right = node;
        node->parent = parent;
    } 
    else {
        HANDLE_ERROR("Both child nodes are occupied", INTERNAL_ERROR, NULL);
    }
}