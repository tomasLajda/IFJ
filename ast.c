/*
 * IFJ Project
 * @brief Implementation file of AST
 *
 * @author Martin Valapka - xvalapm00
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    node->data.nodeType.keyword = NULL;
    node->parent = NULL;
    node->absParent = NULL;
    node->exprTree = NULL;
    node->token = NULL;
    return node;
}

int main(int argc, char const *argv[]) {
    return 0;
}
