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
    int childCount;
} ASTNode;

// Structure of AST
typedef struct AST {
    struct ASTNode* root;
} AST;

/**
 * @brief Initializes new AST
 * 
 * @return Returns initialized AST
 */
AST* initAST();

/**
 * @brief Frees AST from memory
 * 
 * @param ast AST to be freed
 */
void freeAST(AST* ast);

/**
 * @brief Initializes new AST node
 * 
 * @return Returns initialized AST node
 */
ASTNode* initASTNode();

/**
 * @brief Frees node and its children from memory
 * 
 * @param node Node to be freed
 */
void disposeSubtree(ASTNode* node);

/** 
 * @brief Initializes expression tree from AST node
 *
 * @param node Node to initialize expression tree from
 * @return Returns initialized expression tree
*/
AST* initExpressionTree(ASTNode* node);

/**
 * @brief Adds left node to AST
 * 
 * @param ast AST to add node to
 * @param parent Parent node
 * @param node Node to be added
 */
void addLeftNode(AST* ast, ASTNode* parent, ASTNode* node);

/**
 * @brief Adds right child node to AST
 * 
 * @param ast AST to add node to
 * @param parent Parent node
 * @param node Node to be added
 */
void addRightNode(AST* ast, ASTNode* parent, ASTNode* node);

#endif //_AST_H