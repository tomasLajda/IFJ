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
#include "dynamic_string.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
* TODO: FIX EXPR, ADD MORE STRUCTURES
*/

// Type of AST nodes
typedef enum {
    NODE_FUNC_DEF,
    NODE_PARAMS,
    NODE_STATEMENT,
    NODE_EXPR,
    NODE_TYPE,
    NODE_IF,
    NODE_WHILE,
    NODE_VAR_DEF,
    NODE_VAR_ASS,
    NODE_RETURN,
    NODE_FUNC_CALL,
    NODE_DISCARD,
    NODE_ARGS,
    NODE_VAL,
    NODE_BIN_OP
} NodeType;

// Enum for variable types
typedef enum {
    TYPE_I32,
    TYPE_F64,
    TYPE_STRING,
    TYPE_OPTIONAL_I32,
    TYPE_OPTIONAL_F64,
    TYPE_OPTIONAL_STRING,
    TYPE_VOID
} DataType;

// Enum for variable definition type
typedef enum {
    VAR_TYPE_VAR,
    VAR_TYPE_CONST
} VarType;

// Enum for binary operators
typedef enum {
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
} BinaryOperator;

// Structure for binary operations
typedef struct {
    BinaryOperator op;
    struct Expression* left;
    struct Expression* right;
} BinaryOperation;

typedef union {
    DataType type;
    // TODO: FIX
} ExpressionData;

// Expression node
struct Expression {
    NodeType type;
    ExpressionData data;
};

// Structure for function parameters
typedef struct Parameter {
    DynamicString name;
    DataType type;
    struct Parameter* next;
} Parameter;

// Structure for variable declaration
typedef struct {
    DynamicString name;
    DataType type;
    Expression* initialValue;
    bool isConst;
} VarDefinition;

// Structure for variable assignment
typedef struct {
    DynamicString name;
    Expression* value;
} VarAssignment;

// Structure if statement
typedef struct {
    Expression* condition;
    StatementList* ifBody;
    StatementList* elseBody;
} IfStatement;

// Structure for while statement
typedef struct {
    Expression* condition;
    StatementList* body;
} WhileStatement;

// Structure for return statement
typedef struct {
    Expression* returnValue;
} ReturnStatement;

// Structure for function definition
typedef struct {
    DynamicString name;
    Parameter* parameters;
    DataType type;
    StatementList* body;
} FunctionDefinition;

// Union for statement data
typedef union {
    VarDefinition varDef;
    VarAssignment varAssign;
    IfStatement ifStmt;
    WhileStatement whileStmt;
    ReturnStatement returnStmt;
    FunctionDefinition funcDef;
    Expression* discardExpr;
} StatementData;

// Statement node
struct Statement {
    NodeType type;
    StatementData data;
    Statement* next;
};

// Statement list for function bodies and block statements
struct StatementList {
    Statement* first;
    Statement* last;
};

// Structure of AST node
struct ASTNode {
    NodeType type;
    struct ASTNode* left;
    struct ASTNode* right;
};

// Structure of AST root node
struct AST {
    StatementList* statements;
};

/**
 * @brief Creates AST
 */
AST* createAST();

/**
 * @brief Frees AST from memory
 */
void freeAST(AST* ast);

/**
 * @brief Creates Binary Operation node
 * 
 * @param op Binary operator
 * @param left Left operand
 * @param right Right operand
 */
ASTNode* createBinaryOpNode(BinaryOperator op, Expression* left, Expression* right);

#endif _AST_H