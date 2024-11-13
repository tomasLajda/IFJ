/*
 * IFJ Project
 * @brief Implementation file of AST
 *
 * @author Martin Valapka - xvalapm00
 *
 */

#include "ast.h"
#include "error_codes.h"
#include "helpers.h"
#include <stdbool.h>

AST *initAST() {
    AST *tree = (AST *)malloc(sizeof(AST));

    if (tree == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR, NULL);
    }

    tree->root = NULL;
    tree->isExpression = false;
    return tree;
}

ASTNode *initASTNode() {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));

    if (node == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR, NULL);
    }

    node->parent = NULL;
    node->exprTree = NULL;
    node->token = NULL;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void disposeSubtree(ASTNode *node) {
    if (node == NULL) {
        return;
    }

    disposeSubtree(node->left);
    disposeSubtree(node->right);

    if (node->exprTree != NULL) {
        freeAST(node->exprTree);
    }

    if (node->token != NULL) {
        free(node->token);
    }
    free(node);
}

void freeAST(AST *ast) {
    if (ast == NULL) {
        return;
    }

    if (ast->root != NULL) {
        disposeSubtree(ast->root);
    }
    free(ast);
}

void addLeftNode(AST *ast, ASTNode *parent, ASTNode *node) {
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
    } else {
        HANDLE_ERROR("Left child node is already occupied", INTERNAL_ERROR, NULL);
    }
}

void addRightNode(AST *ast, ASTNode *parent, ASTNode *node) {
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
    } else {
        HANDLE_ERROR("Right child node is already occupied", INTERNAL_ERROR, NULL);
    }
}

void displayAST(AST *ast) {
    if (ast == NULL) {
        printf("AST is NULL\n");
        return;
    }
    if (ast->root == NULL) {
        printf("AST is empty\n");
        return;
    }
    printf("AST:\n");
    displayASTNode(ast->root, 0, true);
}

void displayASTNode(ASTNode *node, int level, bool isLeft) {
    if (node == NULL) {
        return;
    }
    for (int i = 0; i < level; i++) {
        printf("  ");
    }
    printf("|-- ");
    if (isLeft) {
        printf("L: ");
    } else {
        printf("R: ");
    }
    if (node->token != NULL) {
        printf("Token Type: %s", TokenTypeToString(node->token->type));
    } else {
        printf("NULL Token");
    }
    printf("\n");
    displayASTNode(node->left, level + 1, true);
    displayASTNode(node->right, level + 1, false);
}