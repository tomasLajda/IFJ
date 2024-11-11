/*
 * IFJ Project
 * @brief Header file for AST
 *
 * @author Martin Valapka - xvalapm00
 *
 */

#ifndef _AST_H
#define _AST_H

#include "enums.h"
#include "scanner.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure of AST node
typedef struct ASTNode {
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *parent;
    struct AST *exprTree;
    Token *token;
} ASTNode;

// Structure of AST
typedef struct AST {
    struct ASTNode *root;
    bool isExpression;
} AST;

/**
 * @brief Initializes new AST
 *
 * @return Returns initialized AST
 */
AST *initAST();

/**
 * @brief Frees AST from memory
 *
 * @param ast AST to be freed
 */
void freeAST(AST *ast);

/**
 * @brief Initializes new AST node
 *
 * @return Returns initialized AST node
 */
ASTNode *initASTNode();

/**
 * @brief Frees node and its children from memory
 *
 * @param node Node to be freed
 */
void disposeSubtree(ASTNode *node);

/**
 * @brief Adds left node to AST
 *
 * @param ast AST to add node to
 * @param parent Parent node
 * @param node Node to be added
 */
void addLeftNode(AST *ast, ASTNode *parent, ASTNode *node);

/**
 * @brief Adds right child node to AST
 *
 * @param ast AST to add node to
 * @param parent Parent node
 * @param node Node to be added
 */
void addRightNode(AST *ast, ASTNode *parent, ASTNode *node);

/**
 * @brief Prints contents of the AST
 *
 * @param ast AST to be pdisplayed
 */
void displayAST(AST *ast);

/**
 * @brief Helper function to print contents of an AST node
 *
 * @param node AST node to be displayed
 * @param level Current level in the tree (for indentation)
 * @param isLeft Set true if the node is a left child, false otherwise
 */
void displayASTNode(ASTNode *node, int level, bool isLeft);

#endif //_AST_H