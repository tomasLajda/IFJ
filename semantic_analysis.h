/*
 * IFJ Project
 * @brief Header file for semantic analysis - contains functions declaration for checking the
 * semantics of the code based on the AST. It also contains helper functions for the semantic
 * analysis.
 *
 * @author Tomáš Lajda - xlajdat00
 *
 */

#ifndef SEMANTIC_ANALYSIS_H
#define SEMANTIC_ANALYSIS_H

#include <math.h>
#include <stdio.h>

#include "ast.h"
#include "helpers.h"
#include "symtable.h"

typedef struct Operand {
    DataType type;
    bool compileTime;
    bool isLiteral;
    Token *token;
} Operand;

/**
 * @brief Retrieves the constant value associated with a given key from the symbol table.
 *
 * This function searches the provided symbol table for an entry matching the specified key
 * and returns the corresponding constant value if found.
 *
 * @param table A pointer to the symbol table from which to retrieve the constant value.
 * @param key A string representing the key associated with the desired constant value.
 * @return The constant value associated with the specified key.
 *         If the key is not found, the behavior is undefined.
 */
double getConstValue(SymbolTable *table, const char *key);

/**
 * @brief Checks if the given value is zero and handles division by zero error.
 *
 * This function takes a double value as input and checks if it is zero.
 * If the value is zero, it handles the division by zero error appropriately.
 *
 * @param value The double value to be checked for division by zero.
 */
void checkDivisionByZero(double value);

/**
 * @brief Checks if a double is an integer.
 * @param number The double to check.
 * @return True if the double is an integer, false otherwise.
 */
bool isDoubleInt(double number);

/**
 * @brief Checks if the operator is an equality/inequality operator.
 * @param operator The operator to check.
 * @return True if the operator is an equality operator, false otherwise.
 */
bool isEqualOperator(TokenType operator);

/**
 * @brief Checks if the operator is a relational operator.
 * @param operator The operator to check.
 * @return True if the operator is a relational operator, false otherwise.
 */
bool isRelationalOperator(TokenType operator);

/**
 * @brief Checks if the type is nullable.
 * @param type The type to check.
 * @return True if the type is nullable, false otherwise.
 */
bool isNullableType(DataType type);

/**
 * @brief Checks if a null value can be assigned to a variable.
 * @param valueType The type of the value being assigned.
 * @param varType The type of the variable.
 * @return True if the null value can be assigned, false otherwise.
 */
bool checkNullAssignment(DataType valueType, DataType varType);

/**
 * @brief Checks if a variable is declared in the symbol table.
 * @param table The symbol table to check.
 * @param key The key of the variable.
 * @return True if the variable is declared, false otherwise.
 */
bool checkDeclaration(SymbolTable *table, const char *key);

/**
 * @brief Checks if the assignment type matches the variable type.
 * @param table The symbol table to check.
 * @param key The key of the variable.
 * @param valueType The type of the value being assigned.
 * @return True if the types match, false otherwise.
 */
bool checkAssignmentType(SymbolTable *table, const char *key, DataType valueType);

/**
 * @brief Gets the type of a variable from the symbol table.
 * @param table The symbol table to check.
 * @param key The key of the variable.
 * @return The type of the variable.
 */
DataType getVariableType(SymbolTable *table, const char *key);

/**
 * @brief Checks if a variable is constant.
 * @param table The symbol table to check.
 * @param key The key of the variable.
 * @return True if the variable is constant, false otherwise.
 */
bool checkVariableConstant(SymbolTable *table, const char *key);

/**
 * @brief Checks if a variable is known at compile time.
 * @param table The symbol table to check.
 * @param key The key of the variable.
 * @return True if the variable is known at compile time, false otherwise.
 */
bool checkVariableCompileTime(SymbolTable *table, const char *key);

/**
 * @brief Gets the number of parameters of a function.
 * @param table The symbol table to check.
 * @param key The key of the function.
 * @return The number of parameters of the function.
 */
unsigned getFunctionParameterCount(SymbolTable *table, const char *key);

/**
 * @brief Gets the return type of the current function.
 * @param table The symbol table to check.
 * @return The return type of the current function.
 */
DataType getReturnType(SymbolTable *table);

/**
 * @brief Checks if a function is defined.
 * @param table The symbol table to check.
 * @param key The key of the function.
 * @return True if the function is defined, false otherwise.
 */
bool checkFunctionDefined(SymbolTable *table, const char *key);

/**
 * @brief Checks if a function is a built-in function.
 * @param key The keyword representing the function.
 * @return True if the function is a built-in function, false otherwise.
 */
bool checkBuiltInFunction(Keyword key);

/**
 * @brief Converts a nullable type to its non-nullable counterpart.
 * @param type The nullable type to convert.
 * @return The non-nullable counterpart of the type.
 */
DataType convertNullableType(DataType type);

/**
 * @brief Analyzes the parameters of a function.
 * @param node The AST subtree representing the function parameters.
 */
void functionParameterAnalysis(ASTNode *node);

/**
 * @brief Analyzes a function definition.
 * @param node The AST subtree representing the function definition.
 */
void functionAnalysis(ASTNode *node);

/**
 * @brief Analyzes a statements recursively. It is entry point for the whole analysis.
 * @param node The AST subtree representing the statement.
 */
void statementAnalysis(ASTNode *node);

/**
 * @brief Analyzes the body of a function.
 * @param node The AST subtree representing the function body.
 */
void functionBodyAnalysis(ASTNode *node);

/**
 * @brief Analyzes an if/while statement.
 * @param node The AST subtree representing the if statement.
 */
void ifWhileAnalysis(ASTNode *node);

/**
 * @brief Analyzes a variable definition.
 * @param node The AST subtree representing the variable definition.
 */
void variableDefinitionAnalysis(ASTNode *node);

/**
 * @brief Analyzes a variable assignment.
 * @param node The AST subtree representing the variable assignment.
 */
void variableAssignmentAnalysis(ASTNode *node);

/**
 * @brief Analyzes a return statement.
 * @param node The AST node representing the return statement.
 */
void returnAnalysis(ASTNode *node);

/**
 * @brief Analyzes a function call.
 * @param node The AST subtree representing the function call.
 * @return The operand representing the result of the function call.
 */
Operand functionCallAnalysis(ASTNode *node);

/**
 * @brief Determines the next operand in an expression.
 * @param left The left operand.
 * @param right The right operand.
 * @param operator The operator.
 * @return The operand representing the result of the operation.
 */
Operand determineNextOperand(Operand left, Operand right, ASTNode *node);

/**
 * @brief Analyzes an expression.
 * @param node The AST node representing the expression.
 * @return The operand representing the result of the expression.
 */
Operand expressionAnalysis(ASTNode *node);

/**
 * @brief Analyzes a built-in function call.
 * @param node The AST node representing the built-in function call.
 * @return The operand representing the result of the built-in function call.
 */
Operand builtInFunctionAnalysis(ASTNode *node);

/**
 * @brief Creates a new unique variable name.
 * @return The new variable name.
 */
char *createNewVariableName();

/**
 * @brief Adds a variable to the AST list.
 * @param oldId The old token representing the variable's original name.
 * @return The new AST node representing the variable's new generated name.
 */
ASTNode *addVariableToASTList(Token *oldId);

/**
 * @brief Inserts function parameters into the AST list.
 * @param params The list of function parameters.
 */
void insertParamsToASTList(List *params);

/**
 * @brief Recursively renames a variable in the Abstract Syntax Tree (AST).
 *
 * This function traverses the AST and renames all occurrences of a variable
 * from the old name to the new name.
 *
 * @param node Pointer to the root node of the AST.
 * @param oldName The old name of the variable to be renamed.
 */
void renameVariableInAst(ASTNode *node, char *oldName);

/**
 * @brief Performs semantic analysis on the given input.
 *
 * This function serves as a starting point for semantic analysis.
 * It checks the input for semantic correctness according to the
 * specified rules and constraints of the language.
 *
 */
void semanticAnalysis();

#endif