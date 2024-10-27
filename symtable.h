/*
 * IFJ Project
 * @brief Header file for binary search tree
 *
 * @author Tomáš Lajda - xlajdat00
 *
 */

#ifndef _BINARY_SEARCH_TREE_H
#define _BINARY_SEARCH_TREE_H

#include <stdbool.h>

#include "error_codes.h"
#include "linked_list.h"
#include "scanner.h"
#include "stack.h"

typedef struct Symbol {
    char *key;
    DataType type;
    bool function;
    bool defined;
    List *params;
} Symbol;

typedef struct BinaryTreeNode {
    Symbol data;
    struct BinaryTreeNode *left;
    struct BinaryTreeNode *right;
    unsigned height;
} BinaryTreeNode;

typedef BinaryTreeNode *BinaryTreeNodePtr;

typedef struct SymbolTable {
    BinaryTreeNodePtr root;
    struct SymbolTable *previousTable;
} SymbolTable;

/**
 * @brief Initializes the symbol table.
 *
 * @param table Pointer to the symbol table to be initialized.
 * @param previousTable Pointer to the previous symbol table.
 */
void symbolTableInit(SymbolTable *table, SymbolTable *previousTable);

/**
 * @brief Frees the symbol table and all its nodes.
 *
 * @param table Pointer to the symbol table to be freed.
 */
void symbolTableDispose(SymbolTable *table);

/**
 * @brief Frees a node of the symbol table.
 *
 * @param node Pointer to the node to be freed.
 */
void treeDispose(BinaryTreeNodePtr node);

/**
 * @brief Inserts a new symbol into the symbol table.
 *
 * @param table Pointer to the symbol table.
 * @param data Symbol to be inserted.
 */
void symbolTableInsert(SymbolTable *table, Symbol data);

/**
 * @brief Searches for a symbol in the symbol table by its key.
 *
 * @param table Pointer to the symbol table.
 * @param key The key of the symbol to search for.
 * @return true if the symbol is found, false otherwise.
 */
bool symbolTableSearch(SymbolTable *table, const char *key);

/**
 * @brief Creates a new node with the given data.
 *
 * @param node Pointer to the node to be created.
 * @param data Data to be stored in the node.
 */
void treeCreate(BinaryTreeNodePtr *node, Symbol data);

/**
 * @brief Performs a left rotation on the given node.
 *
 * @param node Pointer to the node to be rotated.
 */
void treeRotateLeft(BinaryTreeNodePtr *node);

/**
 * @brief Performs a right rotation on the given node.
 *
 * @param node Pointer to the node to be rotated.
 */
void treeRotateRight(BinaryTreeNodePtr *node);

/**
 * @brief Calculates the height of a node.
 *
 * @param node Pointer to the node.
 * @return The height of the node.
 */
unsigned treeHeight(BinaryTreeNodePtr node);

/**
 * @brief Calculates the balance factor of a node.
 *
 * @param node Pointer to the node.
 * @return The balance factor of the node.
 */
int treeBalanceFactor(BinaryTreeNodePtr node);

/**
 * @brief Updates the height of a node.
 *
 * @param node Pointer to the node.
 */
void treeUpdateHeight(BinaryTreeNodePtr node);

/**
 * @brief Rebalances the tree at the given node.
 *
 * @param node Pointer to the node to be rebalanced.
 */
void treeRebalance(BinaryTreeNodePtr *node);

/**
 * @brief Inserts a new symbol into the binary tree.
 *
 * This function takes a binary tree node and a symbol, and inserts the symbol
 * into the binary tree. If the tree is empty, a new node is created. If the
 * tree is not empty, the function recursively finds the correct position for
 * the new symbol and inserts it.
 *
 * @param node A pointer to the root node of the binary tree.
 * @param data The symbol to be inserted into the binary tree.
 * @return A pointer to the root node of the binary tree after insertion.
 */
BinaryTreeNodePtr treeInsert(BinaryTreeNodePtr node, Symbol data);

#endif