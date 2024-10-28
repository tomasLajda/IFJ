/*
 * IFJ Project
 * @brief Header file for AST
 *
 * @author Martin Valapka - xvalapm00
 *
 */

#ifndef _AST_H
#define _AST_H

#include "scanner.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "enums.h"

// Type of AST nodes
typedef union {
    TokenType tokenType;
} NodeType;

// Structure of AST node
typedef struct ASTNode {
    bool isExpression;
    NodeType nodeType;
    struct ASTNode* left;
    struct ASTNode* right;
    struct ASTNode* parent;
    struct ASTNode* absParent;
    struct AST* exprTree;
    Token* token;
    struct ASTNode** children;
    int childCount;
} ASTNode;

// Structure of AST
typedef struct AST {
    struct ASTNode* root;
} AST;

/**
 * @brief Creates AST
 */
AST* createAST();

/**
 * @brief Frees AST from memory
 * 
 * @param ast AST to be freed
 */
void freeAST(AST* ast);

/**
 * @brief Creates AST node
 */
ASTNode* createASTNode();

/**
 * @brief Adds node to AST
 * 
 * @param ast AST to add node to
 * @param parent Parent node
 * @param node Node to be added
 */
void addNode(AST* ast, ASTNode* parent, ASTNode* node);

/**
 * @brief Frees AST node from memory
 * 
 * @param node Node to be freed
 */
void freeNode(ASTNode* node);

/** 
 * @brief Creates expression tree from AST node
 *
 * @param node AST node
*/
void createExpressionTree(ASTNode* node);

#endif //_AST_H