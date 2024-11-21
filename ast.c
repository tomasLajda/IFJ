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
    node->isAssignment = false;
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
        freeToken(node->token);
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
        // Display additional token information
        switch (node->token->type) {
        case TOKEN_TYPE_IDENTIFIER:
        case TOKEN_TYPE_STRING_VALUE:
            if (node->token->attribute.string != NULL) {
                printf(", Value: \"%s\"", node->token->attribute.string);
            }
            break;
        case TOKEN_TYPE_INTEGER_VALUE:
            printf(", Integer: %d", node->token->attribute.integer);
            break;
        case TOKEN_TYPE_DOUBLE_VALUE:
            printf(", Double: %f", node->token->attribute.decimal);
            break;
        case TOKEN_TYPE_KEYWORD:
            printf(", Keyword: %s", TokenKeywordToString(node->token->attribute.keyword));
            break;
        default:
            break;
        }
    } else {
        printf("NULL Token");
    }
    printf("\n");
    displayASTNode(node->left, level + 1, true);
    displayASTNode(node->right, level + 1, false);
}

#include "ast.h"
#include <stdio.h>

// Function to display a single AST node and its children
void displayEntireASTNode(ASTNode *node, int level, bool isLeft) {
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
        // Display additional token information
        switch (node->token->type) {
        case TOKEN_TYPE_IDENTIFIER:
        case TOKEN_TYPE_STRING_VALUE:
            if (node->token->attribute.string != NULL) {
                printf(", Value: \"%s\"", node->token->attribute.string);
            }
            break;
        case TOKEN_TYPE_INTEGER_VALUE:
            printf(", Integer: %d", node->token->attribute.integer);
            break;
        case TOKEN_TYPE_DOUBLE_VALUE:
            printf(", Double: %f", node->token->attribute.decimal);
            break;
        case TOKEN_TYPE_KEYWORD:
            printf(", Keyword: %s", TokenKeywordToString(node->token->attribute.keyword));
            break;
        default:
            break;
        }
    } else {
        printf("NULL Token");
    }
    printf("\n");

    // Recursively display the left and right children
    displayEntireASTNode(node->left, level + 1, true);
    displayEntireASTNode(node->right, level + 1, false);

    // Display the nested AST if it exists
    if (node->exprTree != NULL) {
        printf("  ");
        for (int i = 0; i < level; i++) {
            printf("  ");
        }
        printf("|-- Nested AST:\n");
        displayEntireASTNode(node->exprTree->root, level + 1, true);
    }
}

// Function to display the entire AST, including nested ASTs
void displayEntireAST(AST *ast) {
    if (ast == NULL) {
        printf("AST is NULL\n");
        return;
    }
    if (ast->root == NULL) {
        printf("AST is empty\n");
        return;
    }

    printf("AST:\n");
    displayEntireASTNode(ast->root, 0, true);
}