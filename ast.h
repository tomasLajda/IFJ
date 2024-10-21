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

// Type of AST nodes
typedef enum {
    NODE_FUNC_DEF,
    NODE_FUNC_TYPE,
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

typedef union {
    DataType type;
} ExpressionData;

// Expression node
struct Expression {
    NodeType type;
    ExpressionData data;
};

#endif _AST_H