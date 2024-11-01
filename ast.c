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
    tree->isExpression = false;
    return tree;
}

ASTNode* initASTNode() {
    ASTNode* node = (ASTNode*) malloc(sizeof(ASTNode));

    if (node == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR, NULL);
    }

    node->nodeType.tokenType = TOKEN_TYPE_EMPTY;
    node->parent = NULL;
    node->exprTree = NULL;
    node->token = NULL;
    return node;
}

void disposeSubtree(ASTNode* node) {
    if (node == NULL) {
        return;
    }
    
    disposeSubtree(node->left);
    disposeSubtree(node->right);
    
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

void addLeftNode(AST* ast, ASTNode* parent, ASTNode* node) {
    if (ast == NULL || parent == NULL || node == NULL) {
        HANDLE_ERROR("NULL pointer passed to addLeftNode", INTERNAL_ERROR, NULL);
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
    else {
        HANDLE_ERROR("Left child node is already occupied", INTERNAL_ERROR, NULL);
    }
}

void addRightNode(AST* ast, ASTNode* parent, ASTNode* node) {
    if (ast == NULL || parent == NULL || node == NULL) {
        HANDLE_ERROR("NULL pointer passed to addRightNode", INTERNAL_ERROR, NULL);
    }

    if (ast->root == NULL) {
        ast->root = node;
        node->parent = NULL;
        return;
    }

    if (parent->right == NULL) {
        parent->right = node;
        node->parent = parent;
    } 
    else {
        HANDLE_ERROR("Right child node is already occupied", INTERNAL_ERROR, NULL);
    }
}