/*
 * IFJ Project
 * @brief Implementation file for semantic analysis
 *
 * @author Tomáš Lajda - xlajdat00
 *
 */

#include <math.h>
#include <stdio.h>

#include "ast.h"
#include "helpers.h"
#include "semantic_analysis.h"

#define EPSILON 1e-15

extern AST *ast;
AST *listOfVariables;
Stack symbolTableStack;
Symbol currentSymbol;

double getConstValue(SymbolTable *table, const char *key) {
    Symbol *symbol = symbolTableGetSymbol(table, key);
    return symbol->value;
}

void checkDivisionByZero(double value) {
    if (fabs(value) < EPSILON) {
        HANDLE_ERROR("Division by zero", OTHER_SEMANTIC_ERROR);
    }
}

char *createNewVariableName() {
    char *baseString = "var__";

    static unsigned variableCount = 0;

    int newVariableLength = snprintf(NULL, 0, "%u", variableCount) + strlen(baseString) + 1;

    char *newVariable = malloc(newVariableLength * sizeof(char));
    if (newVariable == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR);
    }

    sprintf(newVariable, "%s%u", baseString, variableCount);
    variableCount++;

    return newVariable;
}

ASTNode *addVariableToASTList(Token *oldId) {
    if (oldId == NULL) {
        HANDLE_ERROR("Token is NULL", INTERNAL_ERROR);
    }

    if (listOfVariables == NULL) {
        HANDLE_ERROR("List of variables is NULL", INTERNAL_ERROR);
    }

    ASTNode *nodeCopy = initASTNode();
    Token *tokenCopy = malloc(sizeof(Token));
    if (tokenCopy == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR);
    }
    tokenCopy->type = TOKEN_TYPE_IDENTIFIER;
    tokenCopy->attribute.string = createNewVariableName();
    nodeCopy->token = tokenCopy;

    nodeCopy->left = initASTNode();
    nodeCopy->left->token = malloc(sizeof(Token));
    if (nodeCopy->left->token == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR);
    }
    nodeCopy->left->token->type = TOKEN_TYPE_IDENTIFIER;
    nodeCopy->left->token->attribute.string = oldId->attribute.string;
    nodeCopy->left->parent = nodeCopy;

    if (listOfVariables->root == NULL) {
        listOfVariables->root = nodeCopy;
    } else {
        ASTNode *current = listOfVariables->root;
        while (current->right != NULL) {
            current = current->right;
        }
        current->right = nodeCopy;
        nodeCopy->parent = current;
    }

    return nodeCopy;
}

void insertParamsToASTList(List *params) {
    if (params == NULL) {
        return;
    }

    listFirst(params);
    while (listIsActive(params)) {
        ListData data;
        listGetValue(params, &data);

        Token *tokenCopy = malloc(sizeof(Token));
        if (tokenCopy == NULL) {
            HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR);
        }
        tokenCopy->type = TOKEN_TYPE_IDENTIFIER;
        tokenCopy->attribute.string = stringDuplicate(data.key);

        ASTNode *nodeCopy = initASTNode();
        nodeCopy->token = tokenCopy;

        if (listOfVariables->root == NULL) {
            listOfVariables->root = nodeCopy;
        } else {
            ASTNode *current = listOfVariables->root;
            while (current->right != NULL) {
                current = current->right;
            }
            current->right = nodeCopy;
            nodeCopy->parent = current;
        }
        listNext(params);
    }
}

bool isDoubleInt(double number) { return floor(number) == number; }

bool isEqualOperator(TokenType operator) {
    return operator== TOKEN_TYPE_EQ || operator== TOKEN_TYPE_NEQ;
}

bool isRelationalOperator(TokenType operator) {
    return operator== TOKEN_TYPE_LTH || operator== TOKEN_TYPE_LEQ || operator==
        TOKEN_TYPE_GTH || operator== TOKEN_TYPE_GEQ || operator== TOKEN_TYPE_EQ || operator==
        TOKEN_TYPE_NEQ;
}

bool isNullableType(DataType type) {
    return type == TYPE_I_32_NULL || type == TYPE_F_64_NULL || type == TYPE_U_8_ARRAY_NULL ||
           type == TYPE_NULL;
}

bool checkNullAssignment(DataType valueType, DataType varType) {
    return (valueType == TYPE_NULL && isNullableType(varType));
}

bool checkDeclaration(SymbolTable *table, const char *key) { return symbolTableSearch(table, key); }

bool checkAssignmentType(SymbolTable *table, const char *key, DataType valueType) {
    Symbol *symbol = symbolTableGetSymbol(table, key);
    return symbol != NULL && symbol->type == valueType;
}

DataType getVariableType(SymbolTable *table, const char *key) {
    Symbol *symbol = symbolTableGetSymbol(table, key);
    return symbol->type;
}

bool checkVariableConstant(SymbolTable *table, const char *key) {
    Symbol *symbol = symbolTableGetSymbol(table, key);
    return symbol != NULL && symbol->constant;
}

bool checkVariableCompileTime(SymbolTable *table, const char *key) {
    Symbol *symbol = symbolTableGetSymbol(table, key);
    return symbol != NULL && symbol->compileTime;
}

DataType getFunctionParameterType(SymbolTable *table, const char *key, unsigned parameterIndex) {
    Symbol *symbol = symbolTableGetSymbol(table, key);

    if (symbol == NULL || !symbol->function) {
        HANDLE_ERROR("Symbol is not a function or doesn't exist", UNDEFINED_ERROR);
    }

    listFirst(symbol->params);
    while (parameterIndex > 0) {
        if (!listIsActive(symbol->params)) {
            HANDLE_ERROR("Too many parameters", PARAMETER_ERROR);
        }

        listNext(symbol->params);
        parameterIndex--;
    }

    if (!listIsActive(symbol->params)) {
        HANDLE_ERROR("Too many parameters", PARAMETER_ERROR);
    }

    ListData data;
    listGetValue(symbol->params, &data);

    return data.type;
}

unsigned getFunctionParameterCount(SymbolTable *table, const char *key) {
    Symbol *symbol = symbolTableGetSymbol(table, key);

    if (symbol == NULL || !symbol->function) {
        HANDLE_ERROR("Symbol is not a function or doesn't exist", UNDEFINED_ERROR);
    }

    unsigned count = 0;
    listFirst(symbol->params);
    while (listIsActive(symbol->params)) {
        count++;
        listNext(symbol->params);
    }

    return count;
}

DataType getReturnType(SymbolTable *table) {
    char *functionKey = table->functionKey;
    while (table->previousTable != NULL) {
        if (table->functionKey != NULL) {
            functionKey = table->functionKey;
        }
        table = table->previousTable;
    }

    if (functionKey == NULL) {
        HANDLE_ERROR("Function key is NULL", INTERNAL_ERROR);
    }

    Symbol *symbol = symbolTableGetSymbol(table, functionKey);
    if (symbol == NULL) {
        HANDLE_ERROR("Function not found", UNDEFINED_ERROR);
    }

    return symbol->type;
}

bool checkFunctionDefined(SymbolTable *table, const char *key) {
    Symbol *symbol = symbolTableGetSymbol(table, key);
    return symbol != NULL && symbol->function;
}

bool checkBuiltInFunction(Keyword key) {
    return key == KEYWORD_STRING || key == KEYWORD_LENGTH || key == KEYWORD_CONCAT ||
           key == KEYWORD_SUBSTRING || key == KEYWORD_STRCMP || key == KEYWORD_ORD ||
           key == KEYWORD_CHR || key == KEYWORD_WRITE || key == KEYWORD_READSTR ||
           key == KEYWORD_READI32 || key == KEYWORD_READF64 || key == KEYWORD_I2F ||
           key == KEYWORD_F2I;
}

DataType convertNullableType(DataType type) {
    switch (type) {
    case TYPE_I_32_NULL:
        return TYPE_I_32;
    case TYPE_F_64_NULL:
        return TYPE_F_64;
    case TYPE_U_8_ARRAY_NULL:
        return TYPE_U_8_ARRAY;
    default:
        return type;
    }
}

void functionParameterAnalysis(ASTNode *node) {
    if (node == NULL) {
        return;
    }

    ListData currentParameter;
    currentParameter.key = node->token->attribute.string;

    if (checkDeclaration(symbolTableTop(&symbolTableStack), currentParameter.key)) {
        HANDLE_ERROR("Parameter redefinition", REDEFINITION_ERROR);
    }

    if (node->left == NULL || node->left->token->type != TOKEN_TYPE_KEYWORD) {
        HANDLE_ERROR("Expected parameter type", INTERNAL_ERROR);
    }
    currentParameter.type = (DataType)node->left->token->attribute.keyword;

    if (currentSymbol.params == NULL) {
        currentSymbol.params = malloc(sizeof(List));
        if (currentSymbol.params == NULL) {
            HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR);
        }
        listInit(currentSymbol.params);
        listInsertFirst(currentSymbol.params, currentParameter);
        listFirst(currentSymbol.params);
    } else {
        listInsertAfter(currentSymbol.params, currentParameter);
        listNext(currentSymbol.params);
    }

    Symbol paramSymbol = {
        .key = currentParameter.key,
        .type = currentParameter.type,
        .constant = true,
        .compileTime = false,
        .used = false,
        .function = false,
        .params = NULL,
    };

    symbolTableInsert(symbolTableTop(&symbolTableStack), paramSymbol);

    node = node->right;
    functionParameterAnalysis(node);
}

void functionAnalysis(ASTNode *node) {
    if (node == NULL) {
        return;
    }

    SymbolTable *table = malloc(sizeof(SymbolTable));
    if (table == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR);
    }
    symbolTableInit(table, symbolTableTop(&symbolTableStack));
    symbolTablePush(&symbolTableStack, table);

    ASTNode *tempNode;

    currentSymbol.function = true;
    currentSymbol.constant = true;
    currentSymbol.used = true;
    currentSymbol.compileTime = false;

    node = node->left;
    if (node == NULL) {
        HANDLE_ERROR("Expected function keyword", INTERNAL_ERROR);
    }

    tempNode = node->right;
    if (tempNode == NULL || tempNode->token->type != TOKEN_TYPE_KEYWORD) {
        HANDLE_ERROR("Expected function return type", INTERNAL_ERROR);
    }

    currentSymbol.type = (DataType)tempNode->token->attribute.keyword;

    node = node->left;
    if (node == NULL) {
        HANDLE_ERROR("Expected function id", INTERNAL_ERROR);
    }

    tempNode = node;
    if (tempNode->token->type == TOKEN_TYPE_KEYWORD) {
        if (tempNode->token->attribute.keyword != KEYWORD_MAIN) {
            HANDLE_ERROR("Expected function main keyword", INTERNAL_ERROR);
        }

        currentSymbol.key = "main";
    } else {
        currentSymbol.key = tempNode->token->attribute.string;
    }

    if (checkDeclaration(symbolTableTop(&symbolTableStack), currentSymbol.key)) {
        HANDLE_ERROR("Function redefinition", REDEFINITION_ERROR);
    }

    tempNode = node->left;
    functionParameterAnalysis(tempNode);

    if (strcmp(currentSymbol.key, "main") == 0) {
        if (currentSymbol.type != TYPE_VOID) {
            HANDLE_ERROR("Main function must return void", PARAMETER_ERROR);
        }
        if (currentSymbol.params != NULL) {
            HANDLE_ERROR("Main function cannot have parameters", PARAMETER_ERROR);
        }
    }

    node = node->parent;
    if (node == NULL) {
        HANDLE_ERROR("Function id doesn't have a parent", INTERNAL_ERROR);
    }

    node = node->parent;
    if (node == NULL) {
        HANDLE_ERROR("Function keyword doesn't have a parent", INTERNAL_ERROR);
    }

    symbolTablePop(&symbolTableStack);

    symbolTableInsert(symbolTableTop(&symbolTableStack), currentSymbol);
    symbolResetValues(&currentSymbol);

    node = node->right;
    functionAnalysis(node);
}

void statementAnalysis(ASTNode *node) {
    if (node == NULL) {
        return;
    }

    switch (node->token->attribute.keyword) {
    case KEYWORD_PUB:
        functionBodyAnalysis(node);
        break;

    case KEYWORD_IF:
    case KEYWORD_WHILE:
        ifWhileAnalysis(node);
        break;

    case KEYWORD_CONST:
    case KEYWORD_VAR:
        variableDefinitionAnalysis(node);
        break;

    case KEYWORD_UNDERSCORE:
        variableAssignmentAnalysis(node);
        break;

    case KEYWORD_RETURN:
        returnAnalysis(node);
        break;

    default:
        if (node->isAssignment) {
            variableAssignmentAnalysis(node);
        } else {
            Operand expressionAnalysis = functionCallAnalysis(node);

            if (expressionAnalysis.type != TYPE_VOID) {
                HANDLE_ERROR("Expression result not used", UNUSED_VARIABLE_ERROR);
            }
        }
        break;
    }

    statementAnalysis(node->right);
}

void functionBodyAnalysis(ASTNode *node) {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    if (table == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR);
    }
    symbolTableInit(table, symbolTableTop(&symbolTableStack));
    symbolTablePush(&symbolTableStack, table);

    AST *newAST = initAST();
    node->exprTree = newAST;
    listOfVariables = newAST;

    node = node->left;
    node = node->left;

    if (node->token->type == TOKEN_TYPE_KEYWORD) {
        symbolTableSetFunctionKey(table, "main");
    } else {
        List *params = symbolTableGetSymbol(table, node->token->attribute.string)->params;
        symbolTableSetFunctionKey(table, node->token->attribute.string);
        symbolTableCopyFunctionParams(table, params);
        insertParamsToASTList(params);
    }

    statementAnalysis(node->right);

    symbolTableCheckUsed(table);
    symbolTableCheckChanged(table);
    symbolTablePop(&symbolTableStack);
}

void ifWhileAnalysis(ASTNode *node) {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    if (table == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR);
    }
    symbolTableInit(table, symbolTableTop(&symbolTableStack));
    symbolTablePush(&symbolTableStack, table);

    node = node->left;
    if (node == NULL) {
        HANDLE_ERROR("Expected if condition or null condition", INTERNAL_ERROR);
    }

    bool nullCond = false;
    char *oldName = NULL;
    if (node->token->type == TOKEN_TYPE_VB) {
        if (checkDeclaration(symbolTableTop(&symbolTableStack),
                             node->right->token->attribute.string)) {
            HANDLE_ERROR("Variable redefinition in null cond", REDEFINITION_ERROR);
        }

        oldName = stringDuplicate(node->right->token->attribute.string);
        ASTNode *newNameNode = addVariableToASTList(node->right->token);
        free(node->right->token);
        node->right->token = copyToken(newNameNode->token);

        symbolSetValues(&currentSymbol, node->right->token->attribute.string, TYPE_NULL, false,
                        true, true);
        currentSymbol.compileTime = false;
        currentSymbol.changed = false;

        nullCond = true;
        node = node->left;
        if (node == NULL) {
            HANDLE_ERROR("Expected if condition", INTERNAL_ERROR);
        }
    }

    if (node->exprTree == NULL) {
        HANDLE_ERROR("Expected if condition expression", INTERNAL_ERROR);
    }

    DataType type = expressionAnalysis(node->exprTree->root).type;
    if (!nullCond) {
        if (type != TYPE_BOOL) {
            HANDLE_ERROR("Invalid condition type", TYPE_COMPATIBILITY_ERROR);
        }
    } else {
        if (type == TYPE_NULL || !isNullableType(type)) {
            HANDLE_ERROR("Invalid variable type", TYPE_COMPATIBILITY_ERROR);
        }
        currentSymbol.type = convertNullableType(type);

        renameVariableInAst(node->left, oldName);

        symbolTableInsert(table, currentSymbol);
        symbolResetValues(&currentSymbol);
    }

    statementAnalysis(node->left);

    symbolTableCheckUsed(table);
    symbolTableCheckChanged(table);
    symbolTablePop(&symbolTableStack);

    if (oldName != NULL) {
        free(oldName);
    }

    if (node->right == NULL) {
        return;
    }

    table = malloc(sizeof(SymbolTable));
    if (table == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR);
    }
    symbolTableInit(table, symbolTableTop(&symbolTableStack));
    symbolTablePush(&symbolTableStack, table);

    statementAnalysis(node->right);

    symbolTableCheckUsed(table);
    symbolTableCheckChanged(table);
    symbolTablePop(&symbolTableStack);
}

void variableDefinitionAnalysis(ASTNode *node) {
    if (node->token->attribute.keyword == KEYWORD_CONST) {
        currentSymbol.constant = true;
    } else {
        currentSymbol.constant = false;
    }
    currentSymbol.compileTime = false;
    currentSymbol.used = false;
    currentSymbol.changed = false;

    node = node->left;
    if (node == NULL || node->token->type != TOKEN_TYPE_IDENTIFIER) {
        HANDLE_ERROR("Expected variable id", INTERNAL_ERROR);
    }

    if (checkDeclaration(symbolTableTop(&symbolTableStack), node->token->attribute.string)) {
        HANDLE_ERROR("Variable redefinition", REDEFINITION_ERROR);
    }

    char *oldName = stringDuplicate(node->token->attribute.string);
    ASTNode *newNameNode = addVariableToASTList(node->token);
    free(node->token);
    node->token = copyToken(newNameNode->token);

    currentSymbol.key = node->token->attribute.string;

    if (node->left != NULL) {
        currentSymbol.type = (DataType)node->left->token->attribute.keyword;
    } else {
        currentSymbol.type = TYPE_ANY;
    }

    node = node->right;
    if (node == NULL) {
        HANDLE_ERROR("Expected variable definition assignment", INTERNAL_ERROR);
    }
    if (node->exprTree == NULL) {
        HANDLE_ERROR("Expected variable definition assignment expression", INTERNAL_ERROR);
    }

    Operand expressionResult = node->exprTree->isExpression
                                   ? expressionAnalysis(node->exprTree->root)
                                   : functionCallAnalysis(node->exprTree->root);

    // Convert integer to double for consistent type
    if ((currentSymbol.type == TYPE_F_64 || currentSymbol.type == TYPE_F_64_NULL) &&
        expressionResult.type == TYPE_I_32 && expressionResult.compileTime) {
        double value = (long long)expressionResult.token->attribute.integer;
        expressionResult.token->type = TOKEN_TYPE_DOUBLE_VALUE;
        expressionResult.token->attribute.decimal = value;
        expressionResult.type = TYPE_F_64;
    }

    if (expressionResult.compileTime && currentSymbol.constant &&
        !isNullableType(currentSymbol.type)) {
        currentSymbol.compileTime = true;
        currentSymbol.value = expressionResult.token->attribute.decimal;
        if (expressionResult.token->type == TOKEN_TYPE_DOUBLE_VALUE) {
            currentSymbol.value = expressionResult.token->attribute.decimal;
        } else {
            currentSymbol.value = (double)expressionResult.token->attribute.integer;
        }
    }

    if (currentSymbol.type != TYPE_ANY && currentSymbol.type != expressionResult.type &&
        !checkNullAssignment(expressionResult.type, currentSymbol.type) &&
        convertNullableType(currentSymbol.type) != expressionResult.type) {
        HANDLE_ERROR("Invalid expression type assignment", TYPE_COMPATIBILITY_ERROR);
    }

    if (expressionResult.type == TYPE_U_8_ARRAY && expressionResult.compileTime) {
        HANDLE_ERROR("Cannot assign compile time string to variable", TYPE_COMPATIBILITY_ERROR);
    }

    if ((currentSymbol.type == TYPE_ANY && expressionResult.type == TYPE_NULL)) {
        HANDLE_ERROR("Cannot infer variable type", TYPE_INFERENCE_ERROR);
    }

    if (currentSymbol.type == TYPE_ANY) {
        currentSymbol.type = expressionResult.type;
    }

    renameVariableInAst(node->parent->parent->right, oldName);
    symbolTableInsert(symbolTableTop(&symbolTableStack), currentSymbol);
    symbolResetValues(&currentSymbol);
    free(oldName);
}

void variableAssignmentAnalysis(ASTNode *node) {
    DataType valueType = TYPE_ANY;

    if (node->token->type != TOKEN_TYPE_KEYWORD) {
        if (node->token->type != TOKEN_TYPE_IDENTIFIER) {
            HANDLE_ERROR("Expected variable id", INTERNAL_ERROR);
        }

        if (!checkDeclaration(symbolTableTop(&symbolTableStack), node->token->attribute.string)) {
            HANDLE_ERROR("Variable not defined", UNDEFINED_ERROR);
        }

        if (checkVariableConstant(symbolTableTop(&symbolTableStack),
                                  node->token->attribute.string)) {
            HANDLE_ERROR("Cannot assign to constant", REDEFINITION_ERROR);
        }
        symbolTableSetUsed(symbolTableTop(&symbolTableStack), node->token->attribute.string);
        symbolTableSetChanged(symbolTableTop(&symbolTableStack), node->token->attribute.string);

        valueType =
            getVariableType(symbolTableTop(&symbolTableStack), node->token->attribute.string);
    }

    if (node->token->type == TOKEN_TYPE_KEYWORD &&
        node->token->attribute.keyword != KEYWORD_UNDERSCORE) {
        HANDLE_ERROR("Expected underscore keyword", INTERNAL_ERROR);
    }

    node = node->left;
    if (node == NULL) {
        HANDLE_ERROR("Expected assignment expression", INTERNAL_ERROR);
    }
    if (node->exprTree == NULL) {
        HANDLE_ERROR("Expected assignment expression", INTERNAL_ERROR);
    }

    Operand expressionResult = node->exprTree->isExpression
                                   ? expressionAnalysis(node->exprTree->root)
                                   : functionCallAnalysis(node->exprTree->root);

    // Convert integer to double if needed for consistent type
    if ((valueType == TYPE_F_64 || valueType == TYPE_F_64_NULL) &&
        expressionResult.type == TYPE_I_32 && expressionResult.compileTime) {
        double value = (long long)expressionResult.token->attribute.integer;
        expressionResult.token->type = TOKEN_TYPE_DOUBLE_VALUE;
        expressionResult.token->attribute.decimal = value;
        expressionResult.type = TYPE_F_64;
    }

    if (valueType != TYPE_ANY && valueType != expressionResult.type &&
        !checkNullAssignment(expressionResult.type, valueType) &&
        convertNullableType(valueType) != expressionResult.type) {
        HANDLE_ERROR("Invalid variable type", TYPE_COMPATIBILITY_ERROR);
    }

    if (expressionResult.type == TYPE_U_8_ARRAY && expressionResult.compileTime) {
        HANDLE_ERROR("Cannot assign compile time string to variable", TYPE_COMPATIBILITY_ERROR);
    }
}

void returnAnalysis(ASTNode *node) {
    DataType returnType = getReturnType(symbolTableTop(&symbolTableStack));

    if (node->exprTree == NULL) {
        if (returnType != TYPE_VOID) {
            HANDLE_ERROR("Return type does not match", RETURN_EXPRESSION_ERROR);
        }
    } else {
        if (returnType == TYPE_VOID && node->exprTree != NULL) {
            HANDLE_ERROR("Return type does not match", RETURN_EXPRESSION_ERROR);
        }

        Operand expressionResult = node->exprTree->isExpression
                                       ? expressionAnalysis(node->exprTree->root)
                                       : functionCallAnalysis(node->exprTree->root);

        // Convert integer to double if needed for consistent type
        if ((returnType == TYPE_F_64 || returnType == TYPE_F_64_NULL) &&
            expressionResult.type == TYPE_I_32 && expressionResult.compileTime) {
            double value = (long long)expressionResult.token->attribute.integer;
            expressionResult.token->type = TOKEN_TYPE_DOUBLE_VALUE;
            expressionResult.token->attribute.decimal = value;
            expressionResult.type = TYPE_F_64;
        }

        if (expressionResult.type != returnType &&
            (expressionResult.type != TYPE_NULL || !isNullableType(returnType))) {
            HANDLE_ERROR("Return type does not match", PARAMETER_ERROR);
        }

        if (expressionResult.type == TYPE_U_8_ARRAY && expressionResult.compileTime) {
            HANDLE_ERROR("Cannot return compile time string", PARAMETER_ERROR);
        }
    }
}

Operand functionCallAnalysis(ASTNode *node) {
    if (node == NULL) {
        HANDLE_ERROR("Function call root node is NULL", INTERNAL_ERROR);
    }

    if (node->token->type == TOKEN_TYPE_KEYWORD) {
        return builtInFunctionAnalysis(node);
    }

    char *functionKey = node->token->attribute.string;

    if (!checkFunctionDefined(symbolTableTop(&symbolTableStack), node->token->attribute.string)) {
        HANDLE_ERROR("Function not defined", UNDEFINED_ERROR);
    }

    unsigned parameterCount =
        getFunctionParameterCount(symbolTableTop(&symbolTableStack), node->token->attribute.string);
    unsigned parameterIndex = 0;

    node = node->left;
    while (node != NULL) {
        DataType parameterType = getFunctionParameterType(symbolTableTop(&symbolTableStack),
                                                          functionKey, parameterIndex);

        Operand expressionResult = node->exprTree->isExpression
                                       ? expressionAnalysis(node->exprTree->root)
                                       : functionCallAnalysis(node->exprTree->root);

        // Convert integer to double if needed for consistent type
        if ((parameterType == TYPE_F_64 || parameterType == TYPE_F_64_NULL) &&
            expressionResult.type == TYPE_I_32 && expressionResult.compileTime) {
            double value = (long long)expressionResult.token->attribute.integer;
            expressionResult.token->type = TOKEN_TYPE_DOUBLE_VALUE;
            expressionResult.token->attribute.decimal = value;
            expressionResult.type = TYPE_F_64;
        }

        if (parameterType != expressionResult.type &&
            !checkNullAssignment(expressionResult.type, parameterType) &&
            convertNullableType(parameterType) != expressionResult.type) {
            HANDLE_ERROR("Invalid parameter type", PARAMETER_ERROR);
        }

        if (expressionResult.type == TYPE_U_8_ARRAY && expressionResult.compileTime) {
            HANDLE_ERROR("Cannot assign compile time string to parameter", PARAMETER_ERROR);
        }

        node = node->right;
        parameterIndex++;
    }

    if (parameterCount != parameterIndex) {
        HANDLE_ERROR("Invalid number of parameters", PARAMETER_ERROR);
    }

    Operand functionReturn =
        (Operand){.type = getVariableType(symbolTableTop(&symbolTableStack), functionKey),
                  .compileTime = false};

    return functionReturn;
}

Operand builtInFunctionAnalysis(ASTNode *node) {
    DataType parameterType[] = {TYPE_VOID, TYPE_VOID, TYPE_VOID, TYPE_VOID};
    DataType returnType = TYPE_VOID;
    Keyword key = node->token->attribute.keyword;

    switch (key) {
    case KEYWORD_STRING:
        // 1 param
        parameterType[0] = TYPE_U_8_ARRAY;
        returnType = TYPE_U_8_ARRAY;
        break;
    case KEYWORD_LENGTH:
        // 1 param
        parameterType[0] = TYPE_U_8_ARRAY;
        returnType = TYPE_I_32;
        break;
    case KEYWORD_CONCAT:
        // 2 params
        parameterType[0] = TYPE_U_8_ARRAY;
        parameterType[1] = TYPE_U_8_ARRAY;
        returnType = TYPE_U_8_ARRAY;
        break;
    case KEYWORD_SUBSTRING:
        // 3 params
        parameterType[0] = TYPE_U_8_ARRAY;
        parameterType[1] = TYPE_I_32;
        parameterType[2] = TYPE_I_32;
        returnType = TYPE_U_8_ARRAY_NULL;
        break;
    case KEYWORD_STRCMP:
        // 2 params
        parameterType[0] = TYPE_U_8_ARRAY;
        parameterType[1] = TYPE_U_8_ARRAY;
        returnType = TYPE_I_32;
        break;
    case KEYWORD_ORD:
        // 2 params
        parameterType[0] = TYPE_U_8_ARRAY;
        parameterType[1] = TYPE_I_32;
        returnType = TYPE_I_32;
        break;
    case KEYWORD_CHR:
        // 1 param
        parameterType[0] = TYPE_I_32;
        returnType = TYPE_U_8_ARRAY;
        break;
    case KEYWORD_WRITE:
        // 1 param
        parameterType[0] = TYPE_ANY;
        break;
    case KEYWORD_READSTR:
        // 0 params
        returnType = TYPE_U_8_ARRAY_NULL;
        break;
    case KEYWORD_READI32:
        // 0 params
        returnType = TYPE_I_32_NULL;
        break;
    case KEYWORD_READF64:
        // 0 params
        returnType = TYPE_F_64_NULL;
        break;
    case KEYWORD_I2F:
        // 1 param
        parameterType[0] = TYPE_I_32;
        returnType = TYPE_F_64;
        break;
    case KEYWORD_F2I:
        // 1 param
        parameterType[0] = TYPE_F_64;
        returnType = TYPE_I_32;
        break;
    default:
        HANDLE_ERROR("Function not defined", UNDEFINED_ERROR);
        break;
    }

    node = node->left;
    for (int i = 0; i < 4; i++) {
        if (node == NULL && parameterType[i] == TYPE_VOID) {
            break;
        }

        if ((node->exprTree == NULL && parameterType[i] != TYPE_VOID) ||
            (node->exprTree != NULL && parameterType[i] == TYPE_VOID)) {
            HANDLE_ERROR("Invalid number of parameters", PARAMETER_ERROR);
        }

        Operand expressionResult = node->exprTree->isExpression
                                       ? expressionAnalysis(node->exprTree->root)
                                       : functionCallAnalysis(node->exprTree->root);

        // Convert integer to double for consistent type
        if ((parameterType[i] == TYPE_F_64 || parameterType[i] == TYPE_F_64_NULL) &&
            expressionResult.type == TYPE_I_32 && expressionResult.compileTime) {
            double value = (double)expressionResult.token->attribute.integer;
            expressionResult.token->type = TOKEN_TYPE_DOUBLE_VALUE;
            expressionResult.token->attribute.decimal = value;
            expressionResult.type = TYPE_F_64;
        }

        if (expressionResult.type != parameterType[i] &&
            (expressionResult.type != TYPE_NULL || !isNullableType(parameterType[i])) &&
            parameterType[i] != TYPE_ANY) {
            HANDLE_ERROR("Invalid function parameter", PARAMETER_ERROR);
        }

        if (expressionResult.type == TYPE_U_8_ARRAY && expressionResult.compileTime &&
            key != KEYWORD_STRING && key != KEYWORD_WRITE) {
            HANDLE_ERROR("Cannot assign compile time string to parameter", PARAMETER_ERROR);
        }

        node = node->right;
    }

    return (Operand){.type = returnType, .compileTime = false};
}

Operand reduceExpression(Operand left, Operand right, ASTNode *node) {
    TokenType operator= node->token->type;

    if (left.type == TYPE_I_32 && right.type == TYPE_I_32) {
        switch (operator) {
        case TOKEN_TYPE_PLUS:
            long long result = left.token->attribute.integer + right.token->attribute.integer;
            node->token->type = TOKEN_TYPE_INTEGER_VALUE;
            node->token->attribute.integer = result;
            break;

        case TOKEN_TYPE_MINUS:
            result = left.token->attribute.integer - right.token->attribute.integer;
            node->token->type = TOKEN_TYPE_INTEGER_VALUE;
            node->token->attribute.integer = result;
            break;

        case TOKEN_TYPE_MUL:
            result = left.token->attribute.integer * right.token->attribute.integer;
            node->token->type = TOKEN_TYPE_INTEGER_VALUE;
            node->token->attribute.integer = result;
            break;

        case TOKEN_TYPE_DIV:
            if (right.token->attribute.integer == 0) {
                HANDLE_ERROR("Division by zero", OTHER_SEMANTIC_ERROR);
            }
            // TAK TOTO JE EDGE CASE JAKO PRASE
            double divisionResult =
                (double)left.token->attribute.integer / (double)right.token->attribute.integer;
            result = floor(divisionResult);
            node->token->type = TOKEN_TYPE_INTEGER_VALUE;
            node->token->attribute.integer = result;
            break;

        default:
            HANDLE_ERROR("Invalid reduction operator", INTERNAL_ERROR);
            break;
        }

        disposeSubtree(node->left);
        disposeSubtree(node->right);
        node->left = NULL;
        node->right = NULL;

        return (Operand){.type = TYPE_I_32, .compileTime = true, .token = node->token};
    }

    if (left.type == TYPE_F_64 && right.type == TYPE_F_64) {
        double result;
        switch (operator) {
        case TOKEN_TYPE_PLUS:
            result = left.token->attribute.decimal + right.token->attribute.decimal;
            node->token->type = TOKEN_TYPE_DOUBLE_VALUE;
            node->token->attribute.decimal = result;
            break;

        case TOKEN_TYPE_MINUS:
            result = left.token->attribute.decimal - right.token->attribute.decimal;
            node->token->type = TOKEN_TYPE_DOUBLE_VALUE;
            node->token->attribute.decimal = result;
            break;

        case TOKEN_TYPE_MUL:
            result = left.token->attribute.decimal * right.token->attribute.decimal;
            node->token->type = TOKEN_TYPE_DOUBLE_VALUE;
            node->token->attribute.decimal = result;
            break;

        case TOKEN_TYPE_DIV:
            checkDivisionByZero(right.token->attribute.decimal);
            result = left.token->attribute.decimal / right.token->attribute.decimal;
            node->token->type = TOKEN_TYPE_DOUBLE_VALUE;
            node->token->attribute.decimal = result;
            break;

        default:
            HANDLE_ERROR("Invalid reduction operator", INTERNAL_ERROR);
            break;
        }

        if (isDoubleInt(result)) {
            node->token->type = TOKEN_TYPE_INTEGER_VALUE;
            node->token->attribute.integer = (long long)result;
        }

        disposeSubtree(node->left);
        disposeSubtree(node->right);
        node->left = NULL;
        node->right = NULL;

        if (node->token->type == TOKEN_TYPE_INTEGER_VALUE) {
            return (Operand){.type = TYPE_I_32, .compileTime = true, .token = node->token};
        }

        return (Operand){.type = TYPE_F_64, .compileTime = true, .token = node->token};
    }

    if (left.type == TYPE_I_32 && right.type == TYPE_F_64) {
        double result;

        switch (operator) {
        case TOKEN_TYPE_PLUS:
            result = left.token->attribute.integer + right.token->attribute.decimal;
            node->token->type = TOKEN_TYPE_DOUBLE_VALUE;
            node->token->attribute.decimal = result;
            break;

        case TOKEN_TYPE_MINUS:
            result = left.token->attribute.integer - right.token->attribute.decimal;
            node->token->type = TOKEN_TYPE_DOUBLE_VALUE;
            node->token->attribute.decimal = result;
            break;

        case TOKEN_TYPE_MUL:
            result = left.token->attribute.integer * right.token->attribute.decimal;
            node->token->type = TOKEN_TYPE_DOUBLE_VALUE;
            node->token->attribute.decimal = result;
            break;

        case TOKEN_TYPE_DIV:
            checkDivisionByZero(right.token->attribute.decimal);

            if (isDoubleInt(right.token->attribute.decimal)) {
                result = (double)left.token->attribute.integer / right.token->attribute.decimal;
                node->token->type = TOKEN_TYPE_DOUBLE_VALUE;
                node->token->attribute.decimal = result;
            } else {
                HANDLE_ERROR("Type compatibility error", TYPE_COMPATIBILITY_ERROR);
            }
            break;

        default:
            HANDLE_ERROR("Invalid reduction operator", INTERNAL_ERROR);
            break;
        }

        if (isDoubleInt(result)) {
            node->token->type = TOKEN_TYPE_INTEGER_VALUE;
            node->token->attribute.integer = (long long)result;
        }

        disposeSubtree(node->left);
        disposeSubtree(node->right);
        node->left = NULL;
        node->right = NULL;

        if (node->token->type == TOKEN_TYPE_INTEGER_VALUE) {
            return (Operand){.type = TYPE_I_32, .compileTime = true, .token = node->token};
        }

        return (Operand){.type = TYPE_F_64, .compileTime = true, .token = node->token};
    }

    if (left.type == TYPE_F_64 && right.type == TYPE_I_32) {
        double result;
        switch (operator) {
        case TOKEN_TYPE_PLUS:
            result = left.token->attribute.decimal + right.token->attribute.integer;
            node->token->type = TOKEN_TYPE_DOUBLE_VALUE;
            node->token->attribute.decimal = result;
            break;

        case TOKEN_TYPE_MINUS:
            result = left.token->attribute.decimal - right.token->attribute.integer;
            node->token->type = TOKEN_TYPE_DOUBLE_VALUE;
            node->token->attribute.decimal = result;
            break;

        case TOKEN_TYPE_MUL:
            result = left.token->attribute.decimal * right.token->attribute.integer;
            node->token->type = TOKEN_TYPE_DOUBLE_VALUE;
            node->token->attribute.decimal = result;
            break;

        case TOKEN_TYPE_DIV:
            if (right.token->attribute.integer == 0) {
                HANDLE_ERROR("Division by zero", OTHER_SEMANTIC_ERROR);
            }

            if (isDoubleInt(left.token->attribute.decimal)) {
                result = right.token->attribute.decimal / (double)left.token->attribute.integer;
                node->token->type = TOKEN_TYPE_DOUBLE_VALUE;
                node->token->attribute.decimal = result;
            } else {
                HANDLE_ERROR("Type compatibility error", TYPE_COMPATIBILITY_ERROR);
            }
            break;

        default:
            HANDLE_ERROR("Invalid reduction operator", INTERNAL_ERROR);
            break;
        }

        if (isDoubleInt(result)) {
            node->token->type = (long long)TOKEN_TYPE_INTEGER_VALUE;
            node->token->attribute.integer = result;
        }

        disposeSubtree(node->left);
        disposeSubtree(node->right);
        node->left = NULL;
        node->right = NULL;

        if (node->token->type == TOKEN_TYPE_INTEGER_VALUE) {
            return (Operand){.type = TYPE_I_32, .compileTime = true, .token = node->token};
        }

        return (Operand){.type = TYPE_F_64, .compileTime = true, .token = node->token};
    }

    HANDLE_ERROR("Invalid reduction", INTERNAL_ERROR);
}

Operand determineNextOperand(Operand left, Operand right, ASTNode *node) {
    Operand result;
    result.compileTime = left.compileTime && right.compileTime;

    TokenType operator= node->token->type;

    switch (left.type) {
    case TYPE_I_32:
        if (right.type == TYPE_I_32) {
            if (result.compileTime && !isRelationalOperator(operator)) {
                return reduceExpression(left, right, node);
            }

            result.type = isRelationalOperator(operator) ? TYPE_BOOL : TYPE_I_32;
            return result;
        }

        if (right.type == TYPE_F_64 && left.compileTime) {
            if (result.compileTime && !isRelationalOperator(operator)) {
                return reduceExpression(left, right, node);
            }

            if (isRelationalOperator(operator)) {
                result.type = TYPE_BOOL;
                left.token->type = TOKEN_TYPE_DOUBLE_VALUE;
                left.token->attribute.decimal = (double)left.token->attribute.integer;
                return result;
            }

            result.type = TYPE_F_64;
            left.token->type = TOKEN_TYPE_DOUBLE_VALUE;
            left.token->attribute.decimal = (double)left.token->attribute.integer;
            return result;
        }
        break;

    case TYPE_F_64:
        if (right.type == TYPE_F_64) {
            if (result.compileTime && !isRelationalOperator(operator)) {
                return reduceExpression(left, right, node);
            }

            result.type = isRelationalOperator(operator) ? TYPE_BOOL : TYPE_F_64;
            return result;
        }

        if (right.type == TYPE_I_32 && right.compileTime) {
            if (result.compileTime && !isRelationalOperator(operator)) {
                return reduceExpression(left, right, node);
            }

            if (isRelationalOperator(operator)) {
                result.type = TYPE_BOOL;
                right.token->type = TOKEN_TYPE_DOUBLE_VALUE;
                right.token->attribute.decimal = (double)right.token->attribute.integer;
                return result;
            }

            result.type = TYPE_F_64;
            right.token->type = TOKEN_TYPE_DOUBLE_VALUE;
            right.token->attribute.decimal = (double)right.token->attribute.integer;
            return result;
        }
        break;

    case TYPE_I_32_NULL:
    case TYPE_F_64_NULL:
    case TYPE_U_8_ARRAY_NULL:
        if (right.type == TYPE_NULL && isEqualOperator(operator)) {
            result.type = TYPE_BOOL;
            return result;
        }
        break;

    case TYPE_BOOL:
        if (right.type == TYPE_BOOL && isEqualOperator(operator)) {
            result.type = TYPE_BOOL;
            return result;
        }
        break;

    case TYPE_NULL:
        if ((right.type == TYPE_NULL || right.type == TYPE_I_32_NULL ||
             right.type == TYPE_F_64_NULL || right.type == TYPE_U_8_ARRAY_NULL) &&
            isEqualOperator(operator)) {
            result.type = TYPE_NULL;
            return result;
        }
        break;

    default:
        HANDLE_ERROR("Invalid type", INTERNAL_ERROR);
        break;
    }

    HANDLE_ERROR("Incompatible types", TYPE_COMPATIBILITY_ERROR);
}

Operand expressionAnalysis(ASTNode *node) {
    if (node == NULL) {
        HANDLE_ERROR("Expected expression", INTERNAL_ERROR);
    }

    if (node->left == NULL && node->right == NULL) {
        if (node->token->type == TOKEN_TYPE_IDENTIFIER) {
            if (!checkDeclaration(symbolTableTop(&symbolTableStack),
                                  node->token->attribute.string)) {
                HANDLE_ERROR("Variable not defined", UNDEFINED_ERROR);
            }
            symbolTableSetUsed(symbolTableTop(&symbolTableStack), node->token->attribute.string);

            Operand varOperand;
            varOperand.compileTime = checkVariableCompileTime(symbolTableTop(&symbolTableStack),
                                                              node->token->attribute.string);
            varOperand.type =
                getVariableType(symbolTableTop(&symbolTableStack), node->token->attribute.string);

            if (varOperand.compileTime) {
                double constValue =
                    getConstValue(symbolTableTop(&symbolTableStack), node->token->attribute.string);
                if (isDoubleInt(constValue)) {
                    long long value = (long long)constValue;
                    node->token->type = TOKEN_TYPE_INTEGER_VALUE;
                    node->token->attribute.integer = value;
                    varOperand.type = TYPE_I_32;
                } else {
                    varOperand.compileTime = false;
                }
            }

            varOperand.token = node->token;
            return varOperand;
        }

        if (node->token->type == TOKEN_TYPE_INTEGER_VALUE) {
            return (Operand){.type = TYPE_I_32, .compileTime = true, .token = node->token};
        }

        if (node->token->type == TOKEN_TYPE_DOUBLE_VALUE) {
            if (isDoubleInt(node->token->attribute.decimal)) {
                long long value = (long long)node->token->attribute.decimal;
                node->token->type = TOKEN_TYPE_INTEGER_VALUE;
                node->token->attribute.integer = value;
                return (Operand){.type = TYPE_I_32, .compileTime = true, .token = node->token};
            }

            return (Operand){.type = TYPE_F_64, .compileTime = true, .token = node->token};
        }

        if (node->token->type == TOKEN_TYPE_STRING_VALUE) {
            return (Operand){.type = TYPE_U_8_ARRAY, .compileTime = true, .token = node->token};
        }

        if (node->token->type == TOKEN_TYPE_KEYWORD &&
            node->token->attribute.keyword == KEYWORD_NULL) {
            return (Operand){.type = TYPE_NULL, .compileTime = true, .token = node->token};
        }

        HANDLE_ERROR("Invalid token type", INTERNAL_ERROR);
    }

    Operand leftType = expressionAnalysis(node->left);
    Operand rightType = expressionAnalysis(node->right);

    return determineNextOperand(leftType, rightType, node);
}

void renameVariableInAst(ASTNode *node, char *oldname) {
    if (node == NULL) {
        return;
    }

    if (node->token->type == TOKEN_TYPE_IDENTIFIER &&
        strcmp(node->token->attribute.string, oldname) == 0) {
        free(node->token->attribute.string);
        node->token->attribute.string = stringDuplicate(currentSymbol.key);
    }

    if (node->exprTree != NULL) {
        renameVariableInAst(node->exprTree->root, oldname);
    }

    renameVariableInAst(node->left, oldname);
    renameVariableInAst(node->right, oldname);
}

void semanticAnalysis() {
    if (ast == NULL) {
        HANDLE_ERROR("AST is NULL", INTERNAL_ERROR);
    }

    SymbolTable *globalTable = malloc(sizeof(SymbolTable));
    if (globalTable == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR);
    }

    symbolTableInit(globalTable, NULL);
    symbolTablePush(&symbolTableStack, globalTable);

    symbolSetValues(&currentSymbol, "ifj", TYPE_VOID, false, true, true);

    symbolTableInsert(globalTable, currentSymbol);
    symbolResetValues(&currentSymbol);

    if (ast->root->right == NULL) {
        HANDLE_ERROR("Main function not defined", UNDEFINED_ERROR);
    }

    // Adds functions to the global scope
    functionAnalysis(ast->root->right);
    if (!checkFunctionDefined(globalTable, "main")) {
        HANDLE_ERROR("Main function not defined", UNDEFINED_ERROR);
    }

    statementAnalysis(ast->root->right);

    symbolTablePop(&symbolTableStack);
}