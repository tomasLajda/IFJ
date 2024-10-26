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

void addNode(AST* ast, ASTNode* parent, ASTNode* node) {
    if (ast == NULL || node == NULL) {
        HANDLE_ERROR("NULL pointer passed to addNode", INTERNAL_ERROR, NULL);
    }

    if (parent == NULL) {
        if (ast->root != NULL) {
            HANDLE_ERROR("Attempting to add root node when root already exists", INTERNAL_ERROR, NULL);
        }
        ast->root = node;
        node->parent = NULL;
        node->absParent = NULL;
        return;
    }

    node->parent = parent;

    if (parent->data.nodeType.binaryOperation >= OP_ADD && parent->data.nodeType.binaryOperation <= OP_GET) {
        if (parent->childCount == 0) {
            createExpressionTree(parent);
        } 
        else if (parent->childCount == 1) {
            if (parent->exprTree == NULL || parent->exprTree->root == NULL) {
                HANDLE_ERROR("Expression tree not properly initialized", INTERNAL_ERROR, NULL);
            }
            ASTNode* exprRoot = parent->exprTree->root;
            exprRoot->data.expr.left = parent->children[0];
            exprRoot->data.expr.right = node;
        } 
        else {
            ASTNode* newOpNode = createASTNode();
            newOpNode->isExpression = true;
            newOpNode->data.nodeType = parent->data.nodeType;
            
            newOpNode->data.expr.left = parent;
            newOpNode->data.expr.right = node;

            if (parent->parent != NULL) {
                ASTNode** siblings = parent->parent->children;
                for (int i = 0; i < parent->parent->childCount; i++) {
                    if (siblings[i] == parent) {
                        siblings[i] = newOpNode;
                        break;
                    }
                }
            } 
            else {
                ast->root = newOpNode;
            }

            node->parent = newOpNode;
            newOpNode->parent = parent->parent;
            parent->parent = newOpNode;
            parent->absParent = newOpNode->absParent;
            newOpNode->childCount = 2;
            parent = newOpNode;
        }

        ASTNode** newChildren = (ASTNode**)realloc(parent->children, (parent->childCount + 1) * sizeof(ASTNode*));
        if (newChildren == NULL) {
            HANDLE_ERROR("Memory reallocation failed", INTERNAL_ERROR, NULL);
        }
        parent->children = newChildren;
        parent->children[parent->childCount] = node;
        parent->childCount++;
    } 
    else if (parent->isExpression == false) {
        ASTNode** newChildren = (ASTNode**)realloc(parent->children, (parent->childCount + 1) * sizeof(ASTNode*));
        if (newChildren == NULL) {
            HANDLE_ERROR("Memory reallocation failed", INTERNAL_ERROR, NULL);
        }
        parent->children = newChildren;
        parent->children[parent->childCount] = node;
        parent->childCount++;
    } 
    else {
        if (parent->data.expr.left == NULL) {
            parent->data.expr.left = node;
        } 
        else if (parent->data.expr.right == NULL) {
            parent->data.expr.right = node;
        } 
        else {
            HANDLE_ERROR("Expression node already has both children", INTERNAL_ERROR, NULL);
        }
    }

    ASTNode* currentParent = parent;
    while (currentParent != NULL && currentParent->isExpression) {
        currentParent = currentParent->parent;
    }
    node->absParent = currentParent;
}
