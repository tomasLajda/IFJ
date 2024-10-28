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

AST* initAST() {
    AST* tree = (AST*) malloc(sizeof(AST));
    if (tree == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR, NULL);
    }
    tree->root = NULL;
    return tree;
}

ASTNode* initASTNode() {
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
    node->childCount = 0;
    return node;
}

void disposeSubtree(ASTNode* node) {
    if (node == NULL) {
        return;
    }
    if (node->nodeType.tokenType >= TOKEN_TYPE_IDENTIFIER && node->nodeType.tokenType <= TOKEN_TYPE_DIV) {
        disposeSubtree(node->left);
        disposeSubtree(node->right);
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
        disposeSubtree(ast->root);
    }

    free(ast);
}

AST* initExpressionTree(ASTNode* node) {
    if (node == NULL) {
        HANDLE_ERROR("NULL pointer passed to createExpressionTree", INTERNAL_ERROR, NULL);
    }

    node->isExpression = true;

    AST* exprTree = initAST();
    if (exprTree == NULL) {
        HANDLE_ERROR("Failed to create expression tree", INTERNAL_ERROR, NULL);
    }
    exprTree->root = node;
    node->exprTree = exprTree;
    exprTree->root->left = NULL;
    exprTree->root->right = NULL;
    
    return exprTree;
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