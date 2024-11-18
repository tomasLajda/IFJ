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
    bool constant;
    bool used;
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
 * @brief Initializes the symbol table.
 *
 * @param table Pointer to the symbol table to be initialized.
 * @param previousTable Pointer to the previous symbol table.
 */
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
 * This function updates the function key of the provided symbol table to the specified key,
 * allowing the function to be found in the symbol table when needed.
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
 * @brief Sets the defined flag of a symbol in the symbol table by its key.
 *
 * @param table Pointer to the symbol table.
 * @param key The key of the symbol to set the defined flag for.
 */
void symbolTableSetDefined(SymbolTable *table, const char *key);

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
 * This function copies the parameters into the symbol table, ensuring that
 * the parameters are defined in the local scope and preventing shadowing.
 *
 * @param table Pointer to the symbol table.
 * @param params List of parameters to be copied.
 */
void symbolTableCopyFunctionParams(SymbolTable *table, List *params);

/**
 * @brief Sets the values of a symbol.
 *
 * This function initializes a symbol with the provided key, data type,
 * function flag, and defined flag.
 *
 * @param symbol Pointer to the symbol to be initialized.
 * @param key The key associated with the symbol.
 * @param type The data type of the symbol.
 * @param function Boolean flag indicating if the symbol represents a function.
 * @param defined Boolean flag indicating if the symbol is defined.
 */
void symbolSetValues(Symbol *symbol, char *key, DataType type, bool function, bool defined);

/**
 * @brief Resets the values of a symbol.
 *
 * This function resets the values of a symbol to their default state.
 *
 * @param symbol Pointer to the symbol to be reset.
 */
void symbolResetValues(Symbol *symbol);

/**
 * @brief Checks if all symbols in the symbol table have been used.
 *
 * This function iterates through the given symbol table and verifies
 * whether each symbol has been used. It can be used to ensure that
 * there are no unused symbols in the table, if there is any unused
 * variable, it will throw an error.
 * @param table Pointer to the symbol table to be checked.
 */
void symbolTableCheckUsed(SymbolTable *table);

#endif