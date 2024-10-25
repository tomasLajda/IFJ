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
    Keyword keyword;
    enum {
        OP_ADD,
        OP_SUB,
        OP_MUL,
        OP_DIV,
        OP_EQ,
        OP_NEQ,
        OP_LT,
        OP_LET,
        OP_GT,
        OP_GET
    } binaryOperation;
} NodeType;

// Structure of Expression node
typedef struct Expression {
    NodeType type;
    struct ASTNode* left;
    struct ASTNode* right;
} Expression;

// Structure of AST node
typedef struct ASTNode {
    bool isExpression;
    union {
        NodeType nodeType;
        struct Expression expr;
    } data;
    
    struct ASTNode* parent;
    struct ASTNode* absParent;
    struct AST* exprTree;
    Token* token;
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
 */
void freeAST(ASTNode* ast);

/**
 * @brief Adds node to AST
 */
void addNode(AST* ast, ASTNode* node);

/**
 * @brief Frees AST node from memory
 */
void freeNode(ASTNode* node);

#endif //_AST_H