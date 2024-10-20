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

#include "linked_list.h"
#include "scanner.h"

struct symbol_t {
    char *key;
    DataType type;
    bool function;
    union {
        bool null;
        int i32;
        double f64;
        char *u8;
    } value;
    List *params;
} typedef Symbol;

struct binary_tree_node_t {
    Symbol data;
    struct binary_tree_node_t *left;
    struct binary_tree_node_t *right;
    unsigned height;
} typedef *BinaryTreeNodePtr;

struct binary_search_tree_t {
    BinaryTreeNodePtr *root;
} typedef BinarySearchTree;

/**
 * @brief Initializes the binary search tree.
 *
 * @param bst Pointer to the binary search tree to be initialized.
 */
void binaryTreeInit(BinarySearchTree *bst);

/**
 * @brief Frees the binary search tree and all its nodes.
 *
 * @param bst Pointer to the binary search tree to be freed.
 */
void binaryTreeFree(BinarySearchTree *bst);

/**
 * @brief Inserts a new symbol into the binary search tree.
 *
 * @param bst Pointer to the binary search tree.
 * @param data Symbol to be inserted.
 */
void binaryTreeInsert(BinarySearchTree *bst, Symbol data);

/**
 * @brief Searches for a symbol in the binary search tree by its key.
 *
 * @param bst Pointer to the binary search tree.
 * @param key The key of the symbol to search for.
 * @param data Pointer to a symbol where the found data will be stored.
 * @return true if the symbol is found, false otherwise.
 */
bool binaryTreeSearch(BinarySearchTree *bst, const char *key);

/**
 * @brief Retrieves the data of a symbol in the binary search tree by its key.
 *
 * @param bst Pointer to the binary search tree.
 * @param key The key of the symbol to search for.
 * @param data Pointer to a symbol where the found data will be stored.
 */
void binaryTreeGetData(BinarySearchTree *bst, const char *key, Symbol *data);

/**
 * @brief Deletes a symbol from the binary search tree by its key.
 *
 * @param bst Pointer to the binary search tree.
 * @param key The key of the symbol to delete.
 */
void binaryTreeDelete(BinarySearchTree *bst, const char *key);

/**
 * @brief Checks if the binary search tree is balanced.
 *
 * @param bst Pointer to the binary search tree.
 * @return true if the binary search tree is balanced, false otherwise.
 */
bool binaryTreeGetBalance(BinarySearchTree *bst);

#endif