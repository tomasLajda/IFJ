/*
 * IFJ Project
 * @brief Implementation file for semantic analysis
 *
 * @author Tomáš Lajda - xlajdat00
 *
 */

#include "semantic_analysis.h"
#include "ast.h"

extern AST *ast;
Stack symbolTableStack;
Symbol currentSymbol;

bool isDoubleInteger(double number);
bool isEqualOperator(TokenType operator);
bool isRelationalOperator(TokenType operator);
bool isNullableType(DataType type);
bool checkDeclaration(SymbolTable *table, const char *key);
bool checkAssignmentType(SymbolTable *table, const char *key, DataType valueType);
DataType getVariableType(SymbolTable *table, const char *key);
bool checkVariableConstant(SymbolTable *table, const char *key);
bool checkVariableCompileTime(SymbolTable *table, const char *key);
bool checkFunctionParameter(SymbolTable *table, const char *key, DataType type,
                            unsigned parameterIndex);
unsigned getFunctionParameterCount(SymbolTable *table, const char *key);
bool checkFunctionDefined(SymbolTable *table, const char *key);
bool isConstruct(ASTNode *node);
void jumpToPreviousConstruct(ASTNode *node);
bool checkBuildInFunction(const char *key);
DataType convertNullableType(DataType type);
void functionParameterAnalysis(ASTNode *node);
void functionAnalysis(ASTNode *node);
void statementAnalysis(ASTNode *node);
void functionBodyAnalysis(ASTNode *node);
void ifAnalysis(ASTNode *node);
void whileAnalysis(ASTNode *node);
void variableDefinitionAnalysis(ASTNode *node);
void variableAssignmentAnalysis(ASTNode *node);
void returnAnalysis(ASTNode *node);
void functionCallAnalysis(ASTNode *node);
Operand determineNextOperand(Operand left, Operand right, TokenType operator);
Operand expressionAnalysis(ASTNode *node);
void semanticAnalysis();

bool isDoubleInteger(double number) { return number - (int)number > 0 ? false : true; }

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

bool checkFunctionParameter(SymbolTable *table, const char *key, DataType type,
                            unsigned parameterIndex) {
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

    return data.type == type || (data.type == TYPE_NULL && isNullableType(type));
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

bool isConstruct(ASTNode *node) {
    return node->token->type == TOKEN_TYPE_KEYWORD &&
           (node->token->attribute.keyword == KEYWORD_IF ||
            node->token->attribute.keyword == KEYWORD_PUB ||
            node->token->attribute.keyword == KEYWORD_WHILE);
}

void jumpToPreviousConstruct(ASTNode *node) {
    while (!isConstruct(node)) {
        node = node->parent;
    }
}

bool checkBuildInFunction(const char *key) {
    return strcmp(key, "readstr") == 0 || strcmp(key, "readi32") == 0 ||
           strcmp(key, "readf64") == 0 || strcmp(key, "write") == 0 || strcmp(key, "i2f") == 0 ||
           strcmp(key, "f2i") == 0 || strcmp(key, "string") == 0 || strcmp(key, "length") == 0 ||
           strcmp(key, "concat") == 0 || strcmp(key, "substring") == 0 ||
           strcmp(key, "strcmp") == 0 || strcmp(key, "ord") == 0 || strcmp(key, "chr") == 0;
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

    node = node->right;
    functionParameterAnalysis(node);
}

void functionAnalysis(ASTNode *node) {
    if (node == NULL) {
        return;
    }

    ASTNode *currentNode = node->left;

    currentSymbol.function = true;
    currentSymbol.constant = true;
    currentSymbol.used = true;
    currentSymbol.compileTime = false;

    node = node->left;
    currentNode = node->right;

    currentSymbol.type = (DataType)currentNode->token->attribute.keyword;

    node = node->left;
    currentNode = node;
    if (currentNode->token->type == TOKEN_TYPE_KEYWORD &&
        currentNode->token->attribute.keyword == KEYWORD_MAIN) {
        currentSymbol.key = "main";
    } else {
        currentSymbol.key = currentNode->token->attribute.string;
    }

    if (checkDeclaration(symbolTableTop(&symbolTableStack), currentSymbol.key)) {
        HANDLE_ERROR("Function redefinition", REDEFINITION_ERROR);
    }

    currentNode = node->left;
    functionParameterAnalysis(currentNode);

    if (strcmp(currentSymbol.key, "main") == 0) {
        if (currentSymbol.type != TYPE_VOID) {
            HANDLE_ERROR("Main function must return void", PARAMETER_ERROR);
        }
        if (currentSymbol.params != NULL) {
            HANDLE_ERROR("Main function cannot have parameters", PARAMETER_ERROR);
        }
    }

    jumpToPreviousConstruct(node);
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
        ifAnalysis(node);
        break;

    case KEYWORD_WHILE:
        whileAnalysis(node);
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
            functionCallAnalysis(node);
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

    node = node->left;
    node = node->left;
    symbolTableCopyFunctionParams(
        table, symbolTableGetSymbol(table, node->token->attribute.string)->params);

    statementAnalysis(node->right);

    symbolTableCheckUsed(table);
    symbolTablePop(&symbolTableStack);
}

void ifAnalysis(ASTNode *node) {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    if (table == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR);
    }
    symbolTableInit(table, symbolTableTop(&symbolTableStack));
    symbolTablePush(&symbolTableStack, table);

    node = node->left;

    // TODO add null set variable
    bool nullCond = false;
    if (node->token->type == TOKEN_TYPE_NULL_COND) {
        if (!checkDeclaration(symbolTableTop(&symbolTableStack),
                              node->right->token->attribute.string)) {
            HANDLE_ERROR("Invalid condition type", TYPE_COMPATIBILITY_ERROR);
        }
        symbolSetValues(&currentSymbol, node->right->token->attribute.string, TYPE_NULL, false,
                        true, true);
        currentSymbol.compileTime = false;

        nullCond = true;
        node = node->left;
    }

    // TODO add ast analysis
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
        symbolTableInsert(table, currentSymbol);
        symbolResetValues(&currentSymbol);
    }

    statementAnalysis(node->left);

    symbolTableCheckUsed(table);
    symbolTablePop(&symbolTableStack);

    table = malloc(sizeof(SymbolTable));
    if (table == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR);
    }
    symbolTableInit(table, symbolTableTop(&symbolTableStack));
    symbolTablePush(&symbolTableStack, table);

    statementAnalysis(node->right);

    symbolTableCheckUsed(table);
    symbolTablePop(&symbolTableStack);
}

void whileAnalysis(ASTNode *node) {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    if (table == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR);
    }
    symbolTableInit(table, symbolTableTop(&symbolTableStack));
    symbolTablePush(&symbolTableStack, table);

    node = node->left;

    bool nullCond = false;
    if (node->token->type == TOKEN_TYPE_NULL_COND) {
        if (!checkDeclaration(symbolTableTop(&symbolTableStack),
                              node->right->token->attribute.string)) {
            HANDLE_ERROR("Invalid condition type", TYPE_COMPATIBILITY_ERROR);
        }
        symbolSetValues(&currentSymbol, node->right->token->attribute.string, TYPE_NULL, false,
                        true, true);
        currentSymbol.compileTime = false;

        nullCond = true;
        node = node->left;
    }

    // TODO add ast analysis
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
        symbolTableInsert(table, currentSymbol);
        symbolResetValues(&currentSymbol);
    }

    statementAnalysis(node->left);

    symbolTableCheckUsed(table);
    symbolTablePop(&symbolTableStack);
}

void variableDefinitionAnalysis(ASTNode *node) {
    if (node->token->attribute.keyword == KEYWORD_CONST) {
        currentSymbol.constant = true;
    } else {
        currentSymbol.constant = false;
    }
    currentSymbol.compileTime = false;

    node = node->left;
    currentSymbol.key = node->token->attribute.string;
    if (node->left != NULL) {
        currentSymbol.type = (DataType)node->left->token->attribute.keyword;
    } else {
        currentSymbol.type = TYPE_ANY;
    }

    if (checkDeclaration(symbolTableTop(&symbolTableStack), currentSymbol.key)) {
        HANDLE_ERROR("Variable redefinition", REDEFINITION_ERROR);
    }

    node = node->right;

    // TODO review tomorrow
    if (node->exprTree->isExpression) {
        Operand expressionResult = expressionAnalysis(node->exprTree->root);
        if (currentSymbol.type != TYPE_ANY && currentSymbol.type != expressionResult.type &&
            (expressionResult.type != TYPE_NULL || !isNullableType(currentSymbol.type))) {
            HANDLE_ERROR("Invalid variable type", TYPE_COMPATIBILITY_ERROR);
        }

        if (currentSymbol.type == TYPE_ANY && expressionResult.type == TYPE_NULL) {
            HANDLE_ERROR("Cannot infer variable type", TYPE_INFERENCE_ERROR);
        }

        currentSymbol.type = expressionResult.type;
        if (currentSymbol.constant) {
            currentSymbol.compileTime = expressionResult.compileTime;
        }
    } else {
        functionCallAnalysis(node->exprTree->root);
        if (currentSymbol.type != TYPE_ANY) {
            if (!checkAssignmentType(symbolTableTop(&symbolTableStack), currentSymbol.key,
                                     currentSymbol.type)) {
                HANDLE_ERROR("Invalid variable type", TYPE_COMPATIBILITY_ERROR);
            }
        }

        if (checkAssignmentType(symbolTableTop(&symbolTableStack), currentSymbol.key, TYPE_VOID)) {
            HANDLE_ERROR("Invalid variable type", TYPE_COMPATIBILITY_ERROR);
        }
        currentSymbol.type = getVariableType(symbolTableTop(&symbolTableStack), currentSymbol.key);
    }

    symbolTableInsert(symbolTableTop(&symbolTableStack), currentSymbol);
    symbolResetValues(&currentSymbol);
}

void variableAssignmentAnalysis(ASTNode *node) {
    DataType valueType = TYPE_ANY;

    if (node->token->type != TOKEN_TYPE_KEYWORD) {
        if (!checkDeclaration(symbolTableTop(&symbolTableStack), node->token->attribute.string)) {
            HANDLE_ERROR("Variable not defined", UNDEFINED_ERROR);
        }

        if (checkVariableConstant(symbolTableTop(&symbolTableStack),
                                  node->token->attribute.string)) {
            HANDLE_ERROR("Cannot assign to constant", REDEFINITION_ERROR);
        }
        symbolTableSetUsed(symbolTableTop(&symbolTableStack), node->token->attribute.string);

        valueType =
            getVariableType(symbolTableTop(&symbolTableStack), node->token->attribute.string);
    }

    node = node->left;

    // TODO review tomorrow
    if (node->exprTree->isExpression) {
        Operand expressionResult = expressionAnalysis(node->exprTree->root);
        if (valueType != TYPE_ANY && valueType != expressionResult.type &&
            (expressionResult.type != TYPE_NULL || !isNullableType(valueType))) {
            HANDLE_ERROR("Invalid variable type", TYPE_COMPATIBILITY_ERROR);
        }

        currentSymbol.type = expressionResult.type;
        if (currentSymbol.constant) {
            currentSymbol.compileTime = expressionResult.compileTime;
        }
    } else {
        functionCallAnalysis(node->exprTree->root);
        if (currentSymbol.type != TYPE_ANY) {
            if (!checkAssignmentType(symbolTableTop(&symbolTableStack), currentSymbol.key,
                                     currentSymbol.type)) {
                HANDLE_ERROR("Invalid variable type", TYPE_COMPATIBILITY_ERROR);
            }
        }
        currentSymbol.type = getVariableType(symbolTableTop(&symbolTableStack), currentSymbol.key);
    }
}

void returnAnalysis(ASTNode *node) {
    DataType returnType = getReturnType(symbolTableTop(&symbolTableStack));

    if (node->exprTree == NULL) {
        if (returnType != TYPE_VOID) {
            HANDLE_ERROR("Return type does not match", RETURN_EXPRESSION_ERROR);
        }
    } else {
        if (returnType == TYPE_VOID && node->exprTree->root != NULL) {
            HANDLE_ERROR("Return type does not match", RETURN_EXPRESSION_ERROR);
        }

        DataType type = expressionAnalysis(node->exprTree->root).type;
        if (type != returnType && (type != TYPE_NULL || !isNullableType(returnType))) {
            HANDLE_ERROR("Return type does not match", RETURN_EXPRESSION_ERROR);
        }
    }
}

void functionCallAnalysis(ASTNode *node) {
    if (!checkFunctionDefined(symbolTableTop(&symbolTableStack), node->token->attribute.string)) {
        HANDLE_ERROR("Function not defined", UNDEFINED_ERROR);
    }

    unsigned parameterCount =
        getFunctionParameterCount(symbolTableTop(&symbolTableStack), node->token->attribute.string);
    unsigned parameterIndex = 0;

    node = node->left;
    while (node != NULL) {
        DataType type = expressionAnalysis(node->exprTree->root).type;
        if (!checkFunctionParameter(symbolTableTop(&symbolTableStack),
                                    node->token->attribute.string, type, parameterIndex)) {
            HANDLE_ERROR("Invalid function parameter", PARAMETER_ERROR);
        }

        node = node->right;
        parameterIndex++;
    }

    if (parameterCount != parameterIndex) {
        HANDLE_ERROR("Invalid number of parameters", PARAMETER_ERROR);
    }
}

Operand determineNextOperand(Operand left, Operand right, TokenType operator) {
    Operand result;
    result.compileTime = left.compileTime && right.compileTime;

    switch (left.type) {
    case TYPE_I_32:
        if (right.type == TYPE_I_32) {
            result.type = isRelationalOperator(operator) ? TYPE_BOOL : TYPE_I_32;
            return result;
        }

        if (right.type == TYPE_F_64 && !left.compileTime) {
            result.type = isRelationalOperator(operator) ? TYPE_BOOL : TYPE_F_64;
            return result;
        }
        break;

    case TYPE_F_64:
        if (right.type == TYPE_F_64) {
            result.type = isRelationalOperator(operator) ? TYPE_BOOL : TYPE_F_64;
            return result;
        }

        if (right.type == TYPE_I_32 && !right.compileTime) {
            result.type = isRelationalOperator(operator) ? TYPE_BOOL : TYPE_F_64;
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
        HANDLE_ERROR("Expression is NULL", INTERNAL_ERROR);
    }

    if (node->left == NULL && node->right == NULL) {
        if (node->token->type == TOKEN_TYPE_IDENTIFIER) {
            if (!checkDeclaration(symbolTableTop(&symbolTableStack),
                                  node->token->attribute.string)) {
                HANDLE_ERROR("Variable not defined", UNDEFINED_ERROR);
            }
            symbolTableSetUsed(symbolTableTop(&symbolTableStack), node->token->attribute.string);

            return (Operand){.type = getVariableType(symbolTableTop(&symbolTableStack),
                                                     node->token->attribute.string),
                             .compileTime = checkVariableCompileTime(
                                 symbolTableTop(&symbolTableStack), node->token->attribute.string)};
        }

        if (node->token->type == TOKEN_TYPE_INTEGER_VALUE) {
            return (Operand){.type = TYPE_I_32, .compileTime = true};
        }

        if (node->token->type == TOKEN_TYPE_DOUBLE_VALUE) {
            return (Operand){.type = TYPE_F_64, .compileTime = true};
        }

        if (node->token->type == TOKEN_TYPE_STRING_VALUE) {
            return (Operand){.type = TYPE_U_8_ARRAY, .compileTime = true};
        }

        if (node->token->type == TOKEN_TYPE_KEYWORD &&
            node->token->attribute.keyword == KEYWORD_NULL) {
            return (Operand){.type = TYPE_NULL, .compileTime = true};
        }

        HANDLE_ERROR("Invalid token type", INTERNAL_ERROR);
    }

    Operand leftType = expressionAnalysis(node->left);
    Operand rightType = expressionAnalysis(node->right);

    TokenType operator= node->token->type;

    return determineNextOperand(leftType, rightType, operator);
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
        HANDLE_ERROR("AST is NULL", INTERNAL_ERROR);
    }

    // Adds functions to the global scope
    functionAnalysis(ast->root->right);
    if (!checkFunctionDefined(globalTable, "main")) {
        HANDLE_ERROR("Main function not defined", UNDEFINED_ERROR);
    }

    statementAnalysis(ast->root->right);

    symbolTablePop(&symbolTableStack);
}