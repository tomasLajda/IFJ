/*
 * IFJ Project
 * @brief Header file for semantic analysis
 *
 * @author Tomáš Lajda - xlajdat00
 *
 */

#ifndef SEMANTIC_ANALYSIS_H
#define SEMANTIC_ANALYSIS_H

#include "symtable.h"

/**
 * @brief Check if a symbol is declared in the symbol table.
 *
 * @param table Pointer to the symbol table.
 * @param key Key of the symbol to check.
 * @return true if the symbol is declared, false otherwise.
 */
bool checkDeclaration(SymbolTable *table, const char *key);

/**
 * @brief Check if an assignment is valid in the symbol table.
 *
 * @param table Pointer to the symbol table.
 * @param key Key of the symbol to check.
 * @param type Expected data type of the symbol.
 * @return true if the assignment is valid, false otherwise.
 */
bool checkAssignment(SymbolTable *table, const char *key, DataType type);

/**
 * @brief Check if a function call is valid in the symbol table.
 *
 * @param table Pointer to the symbol table.
 * @param key Key of the function to check.
 * @param type Expected return type of the function.
 * @param paramIndex Index of the parameter to check.
 * @return true if the function call is valid, false otherwise.
 */
bool checkFunctionCall(SymbolTable *table, const char *key, DataType type, unsigned paramIndex);

/**
 * @brief Check if a list of function call parameters is valid in the symbol table.
 *
 * @param table Pointer to the symbol table.
 * @param key Key of the function to check.
 * @param params List of parameters to check.
 * @return true if the parameter list is valid, false otherwise.
 */
bool checkFunctionCallList(SymbolTable *table, const char *key, List *params);

/**
 * @brief Get the data type of a symbol in the symbol table.
 *
 * @param table Pointer to the symbol table.
 * @param key Key of the symbol to check.
 * @return DataType of the symbol.
 */
DataType getDataType(SymbolTable *table, const char *key);

/**
 * @brief Check if a return statement is valid in the symbol table.
 *
 * @param table Pointer to the symbol table.
 * @param type Expected return type.
 * @return true if the return statement is valid, false otherwise.
 */
bool checkReturn(SymbolTable *table, DataType type);

#endif