/*
 * IFJ Project
 * @brief Header file for binary search tree and symbol table operations.
 *
 * This file contains the declarations for functions and structures used
 * for managing a binary search tree and a symbol table.
 *
 * Author: Tomáš Lajda - xlajdat00
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
    bool constant;
    bool used;
    bool changed;
    bool compileTime;
    double value;
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
    char *functionKey;
    struct SymbolTable *previousTable;
} SymbolTable;

/**
 * @brief Creates a new tree node with the given data.
 *
 * @param node Pointer to the node to be created.
 * @param data Data to be stored in the new node.
 */
void treeCreate(BinaryTreeNodePtr *node, Symbol data);

/**
 * @brief Returns the height of the tree.
 *
 * @param node The root node of the tree.
 * @return The height of the tree.
 */
unsigned treeHeight(BinaryTreeNodePtr node);

/**
 * @brief Calculates the balance factor of the node.
 *
 * @param node The node to calculate the balance factor for.
 * @return The balance factor of the node.
 */
int treeBalanceFactor(BinaryTreeNodePtr node);

/**
 * @brief Updates the height of the node.
 *
 * @param node The node to update the height for.
 */
void treeUpdateHeight(BinaryTreeNodePtr node);

/**
 * @brief Performs a left rotation on the node.
 *
 * @param node The node to perform the left rotation on.
 */
void treeRotateLeft(BinaryTreeNodePtr *node);

/**
 * @brief Performs a right rotation on the node.
 *
 * @param node The node to perform the right rotation on.
 */
void treeRotateRight(BinaryTreeNodePtr *node);

/**
 * @brief Rebalances the tree.
 *
 * @param node The root node of the tree to rebalance.
 */
void treeRebalance(BinaryTreeNodePtr *node);

/**
 * @brief Disposes of the tree.
 *
 * @param node The root node of the tree to dispose of.
 */
void treeDispose(BinaryTreeNodePtr node);

/**
 * @brief Finds the node with the minimum value in the tree.
 *
 * @param node The root node of the tree.
 * @return The node with the minimum value.
 */
BinaryTreeNodePtr treeMinValueNode(BinaryTreeNodePtr node);

/**
 * @brief Searches for a key in the tree.
 *
 * @param node The root node of the tree.
 * @param key The key to search for.
 * @return True if the key is found, false otherwise.
 */
bool treeSearch(BinaryTreeNodePtr node, const char *key);

/**
 * @brief Inserts a new node with the given data into the tree.
 *
 * @param node The root node of the tree.
 * @param data The data to insert.
 * @return The new root node of the tree.
 */
BinaryTreeNodePtr treeInsert(BinaryTreeNodePtr node, Symbol data);

/**
 * @brief Deletes a node with the given key from the tree.
 *
 * @param node The root node of the tree.
 * @param key The key of the node to delete.
 * @return The new root node of the tree.
 */
BinaryTreeNodePtr treeDelete(BinaryTreeNodePtr node, const char *key);

/**
 * @brief Reassigns the data of a node with the given key.
 *
 * @param node The root node of the tree.
 * @param key The key of the node to reassign.
 * @param data The new data to assign.
 */
void treeReassign(BinaryTreeNodePtr node, const char *key, Symbol data);

/**
 * @brief Retrieves the data of a node with the given key.
 *
 * @param node The root node of the tree.
 * @param key The key of the node to retrieve.
 * @return Pointer to the data of the node, or NULL if not found.
 */
Symbol *treeGet(BinaryTreeNodePtr node, const char *key);

/**
 * @brief Prints a specified number of spaces.
 *
 * @param count The number of spaces to print.
 */
void printSpaces(int count);

/**
 * @brief Recursively prints a binary tree in a structured format.
 *
 * @param node A pointer to the current node in the binary tree.
 * @param level The current level of the node in the binary tree.
 */
void treePrint(BinaryTreeNodePtr node, int level);

/**
 * @brief Checks if all symbols in the tree have been used.
 *
 * @param node The root node of the tree.
 */
void treeCheckUsed(BinaryTreeNodePtr node);

/**
 * @brief Checks if all symbols in the tree have been changed.
 *
 * @param node The root node of the tree.
 */
void treeCheckChanged(BinaryTreeNodePtr node);

/**
 * @brief Initializes the symbol table.
 *
 * @param table Pointer to the symbol table to be initialized.
 * @param previousTable Pointer to the previous symbol table.
 */
void symbolTableInit(SymbolTable *table, SymbolTable *previousTable);

/**
 * @brief Sets the function key for the given symbol table.
 *
 * @param table Pointer to the symbol table whose function key is to be set.
 * @param functionKey The key of the function to set in the symbol table.
 */
void symbolTableSetFunctionKey(SymbolTable *table, char *functionKey);

/**
 * @brief Frees the symbol table and all its nodes.
 *
 * @param table Pointer to the symbol table to be freed.
 */
void symbolTableDispose(SymbolTable *table);

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
 * @brief Deletes a symbol from the symbol table by its key.
 *
 * @param table Pointer to the symbol table.
 * @param key The key of the symbol to delete.
 */
void symbolTableDelete(SymbolTable *table, const char *key);

/**
 * @brief Reassigns a symbol in the symbol table by its key.
 *
 * @param table Pointer to the symbol table.
 * @param key The key of the symbol to reassign.
 * @param data The new symbol data.
 */
void symbolTableReassign(SymbolTable *table, const char *key, Symbol data);

/**
 * @brief Sets the used flag of a symbol in the symbol table by its key.
 *
 * @param table Pointer to the symbol table.
 * @param key The key of the symbol to set the used flag for.
 */
void symbolTableSetUsed(SymbolTable *table, const char *key);

/**
 * @brief Pushes the symbol table onto the stack.
 *
 * @param stack Pointer to the stack.
 * @param table Pointer to the symbol table.
 */
void symbolTablePush(Stack *stack, SymbolTable *table);

/**
 * @brief Gets the top symbol table from the stack.
 *
 * @param stack Pointer to the stack.
 * @return Pointer to the top symbol table.
 */
SymbolTable *symbolTableTop(Stack *stack);

/**
 * @brief Pops the top symbol table from the stack.
 *
 * @param stack Pointer to the stack.
 */
void symbolTablePop(Stack *stack);

/**
 * @brief Gets the previous symbol table.
 *
 * @param table Pointer to the current symbol table.
 * @return Pointer to the previous symbol table.
 */
SymbolTable *symbolTableGetPrevious(SymbolTable *table);

/**
 * @brief Gets a symbol from the symbol table by its key.
 *
 * @param table Pointer to the symbol table.
 * @param key The key of the symbol to get.
 * @return Pointer to the symbol if found, NULL otherwise.
 */
Symbol *symbolTableGetSymbol(SymbolTable *table, const char *key);

/**
 * @brief Prints the symbol table.
 *
 * @param table Pointer to the symbol table.
 */
void symbolTablePrint(SymbolTable *table);

/**
 * @brief Copies function parameters into the symbol table.
 *
 * @param table Pointer to the symbol table.
 * @param params List of parameters to be copied.
 */
void symbolTableCopyFunctionParams(SymbolTable *table, List *params);

/**
 * @brief Sets the values of a symbol.
 *
 * @param symbol Pointer to the symbol to be initialized.
 * @param key The key associated with the symbol.
 * @param type The data type of the symbol.
 * @param function Boolean flag indicating if the symbol represents a function.
 * @param constant Boolean flag indicating if the symbol is a constant.
 * @param used Boolean flag indicating if the symbol has been used.
 */
void symbolSetValues(Symbol *symbol, char *key, DataType type, bool function, bool constant,
                     bool used);

/**
 * @brief Resets the values of a symbol.
 *
 * @param symbol Pointer to the symbol to be reset.
 */
void symbolResetValues(Symbol *symbol);

/**
 * @brief Checks if all symbols in the symbol table have been used.
 *
 * @param table Pointer to the symbol table to be checked.
 */
void symbolTableCheckUsed(SymbolTable *table);

/**
 * @brief Marks a symbol table entry as changed.
 *
 * @param table A pointer to the symbol table.
 * @param key The key of the entry to mark as changed.
 */
void symbolTableSetChanged(SymbolTable *table, const char *key);

/**
 * @brief Checks if the symbol table has changed.
 *
 * @param table A pointer to the SymbolTable to be checked.
 */
void symbolTableCheckChanged(SymbolTable *table);

#endif