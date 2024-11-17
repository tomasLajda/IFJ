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
 * @brief Check if the value being assigned to a variable is of the correct type.
 *
 * @param table Pointer to the symbol table.
 * @param key Key of the variable to check.
 * @param valueType Data type of the value being assigned.
 * @return true if the assignment is valid, false otherwise.
 */
bool checkAssignmentType(SymbolTable *table, const char *key, DataType valueType);

/**
 * @brief Check if a variable has already been assigned a value.
 *
 * @param table Pointer to the symbol table.
 * @param key Key of the variable to check.
 * @return true if the variable has already been defined, false otherwise.
 */
bool checkVariableDefined(SymbolTable *table, const char *key);

/**
 * @brief Check if a function call parameter is valid in the symbol table.
 *
 * @param table Pointer to the symbol table.
 * @param key Key of the function to check.
 * @param type Expected data type of the parameter.
 * @param parameterIndex Index of the parameter to check.
 * @return true if the function call parameter is valid, false otherwise.
 */
bool checkFunctionParameter(SymbolTable *table, const char *key, DataType type,
                            unsigned parameterIndex);

/**
 * @brief Check if a return statement is valid in the symbol table.
 *
 * @param table Pointer to the symbol table.
 * @param key Key of the function to check.
 * @param type Expected return type.
 * @return true if the return statement is valid, false otherwise.
 */
bool checkReturnType(SymbolTable *table, DataType type);

/**
 * @brief Checks if a function is defined in the symbol table.
 *
 * This function searches the symbol table for a function with the specified key
 * and determines if it has been defined.
 *
 * @param table Pointer to the symbol table.
 * @param key The key representing the function to check.
 * @return true if the function is defined, false otherwise.
 */
bool checkFunctionDefined(SymbolTable *table, const char *key);

/**
 * @brief Checks if a function is defined within the global scope in the symbol table.
 *
 * This function verifies that a function is defined within
 * the global scope in the symbol table.
 *
 * @param table Pointer to the symbol table.
 * @return true if the function is defined within the correct scope, false otherwise.
 */
bool checkFunctionDefinitionScope(SymbolTable *table);

#endif