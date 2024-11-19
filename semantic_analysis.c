/*
 * IFJ Project
 * @brief Implementation file for semantic analysis
 *
 * @author Tomáš Lajda - xlajdat00
 *
 */

#include "semantic_analysis.h"
#include "ast.h"

extern AST ast;
Stack symbolTableStack;
Symbol currentSymbol;
ListData currentParameter;
DataType returnType;
ASTNode *currentNode;
ASTNode *constructNode;

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
        return false;
    }

    listFirst(symbol->params);
    while (parameterIndex > 0) {
        if (!listIsActive(symbol->params)) {
            return false;
        }

        listNext(symbol->params);
        parameterIndex--;
    }

    if (!listIsActive(symbol->params)) {
        return false;
    }

    ListData data;
    listGetValue(symbol->params, &data);

    return data.type == type;
}

bool checkReturnType(SymbolTable *table, DataType type) {
    char *functionKey = table->functionKey;
    while (table->previousTable != NULL) {
        if (table->functionKey != NULL) {
            functionKey = table->functionKey;
        }
        table = table->previousTable;
    }

    if (functionKey == NULL) {
        return false;
    }

    Symbol *symbol = symbolTableGetSymbol(table, functionKey);

    return symbol != NULL && symbol->type == type;
}

bool checkFunctionDefined(SymbolTable *table, const char *key) {
    Symbol *symbol = symbolTableGetSymbol(table, key);
    return symbol != NULL && symbol->function;
}

bool isConstruct() {
    return constructNode->token->type == TOKEN_TYPE_KEYWORD &&
           (constructNode->token->attribute.keyword == KEYWORD_IF ||
            constructNode->token->attribute.keyword == KEYWORD_PUB ||
            constructNode->token->attribute.keyword == KEYWORD_WHILE);
}

void jumpToPreviousConstruct() {
    while (!isConstruct()) {
        constructNode = constructNode->parent;
    }
}

bool checkBuildInFunction(const char *key) {
    return strcmp(key, "readstr") == 0 || strcmp(key, "readi32") == 0 ||
           strcmp(key, "readf64") == 0 || strcmp(key, "write") == 0 || strcmp(key, "i2f") == 0 ||
           strcmp(key, "f2i") == 0 || strcmp(key, "string") == 0 || strcmp(key, "length") == 0 ||
           strcmp(key, "concat") == 0 || strcmp(key, "substring") == 0 ||
           strcmp(key, "strcmp") == 0 || strcmp(key, "ord") == 0 || strcmp(key, "chr") == 0;
}

void functionParameterAnalysis() {
    if (currentNode == NULL) {
        return;
    }

    currentParameter.key = currentNode->token->attribute.string;
    currentParameter.type = (DataType)currentNode->left->token->attribute.keyword;

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

    currentNode = currentNode->right;
    functionParameterAnalysis();
}

void functionAnalysis() {
    if (currentNode == NULL) {
        return;
    }

    currentSymbol.function = true;
    currentSymbol.constant = true;
    currentSymbol.used = true;
    currentSymbol.compileTime = false;

    constructNode = constructNode->left;
    currentNode = constructNode->right;

    currentSymbol.type = (DataType)currentNode->token->attribute.keyword;

    constructNode = constructNode->left;
    currentNode = constructNode;
    if (currentNode->token->type == TOKEN_TYPE_KEYWORD &&
        currentNode->token->attribute.keyword == KEYWORD_MAIN) {
        currentSymbol.key = "main";
    } else {
        currentSymbol.key = currentNode->token->attribute.string;
    }

    if (checkBuildInFunction(currentSymbol.key)) {
        HANDLE_ERROR("Cannot redefine build-in function", REDEFINITION_ERROR);
    }

    if (checkDeclaration(symbolTableTop(&symbolTableStack), currentSymbol.key)) {
        HANDLE_ERROR("Function redefinition", REDEFINITION_ERROR);
    }

    currentNode = constructNode->left;
    functionParameterAnalysis();
    currentNode = constructNode->right;

    if (strcmp(currentSymbol.key, "main") == 0) {
        if (currentSymbol.type != TYPE_VOID) {
            HANDLE_ERROR("Main function must return void", PARAMETER_ERROR);
        }
        if (currentSymbol.params != NULL) {
            HANDLE_ERROR("Main function cannot have parameters", PARAMETER_ERROR);
        }
    }

    jumpToPreviousConstruct();
    constructNode = constructNode->right;
    functionAnalysis();
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
    returnType = (DataType)node->right->token->attribute.keyword;

    node = node->left;
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
    if (node->token->type == TOKEN_TYPE_KEYWORD && node->token->attribute.keyword == KEYWORD_NULL) {
        node = node->left;
    }

    // TODO add ast analysis

    statementAnalysis(node->left);
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

    // TODO add null set variable
    if (node->token->type == TOKEN_TYPE_KEYWORD && node->token->attribute.keyword == KEYWORD_NULL) {
        node = node->left;
    }

    // TODO add ast analysis

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

    node = node->left;
    currentSymbol.key = node->token->attribute.string;
    // TODO check type properly when we have to infer it
    if (node->left != NULL) {
        currentSymbol.type = (DataType)node->left->token->attribute.keyword;
    } else {
        currentSymbol.type = TYPE_ANY;
    }

    if (checkDeclaration(symbolTableTop(&symbolTableStack), currentSymbol.key)) {
        HANDLE_ERROR("Variable redefinition", REDEFINITION_ERROR);
    }

    node = node->right;

    // TODO add ast analysis

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

        valueType =
            getVariableType(symbolTableTop(&symbolTableStack), node->token->attribute.string);
    }

    node = node->left;

    // TODO add ast analysis
}

void returnAnalysis(ASTNode *node) {
    if (node->exprTree == NULL) {
        if (returnType != TYPE_VOID) {
            HANDLE_ERROR("Return type does not match", RETURN_EXPRESSION_ERROR);
        }
    } else {
        // TODO add ast analysis
    }
}

void functionCallAnalysis(ASTNode *node) {
    if (!checkFunctionDefined(symbolTableTop(&symbolTableStack), node->token->attribute.string)) {
        HANDLE_ERROR("Function not defined", UNDEFINED_ERROR);
    }

    unsigned parameterIndex = 0;

    node = node->left;
    while (node != NULL) {
        // TODO add ast analysis

        node = node->right;
        parameterIndex++;
    }
}

bool isDoubleInteger(double number) { return number - (int)number > 0 ? false : true; }

bool isEqualOperator(TokenType operator) {
    return operator== TOKEN_TYPE_EQ || operator== TOKEN_TYPE_NEQ;
}

bool isRelationalOperator(TokenType operator) {
    return operator== TOKEN_TYPE_LTH || operator== TOKEN_TYPE_LEQ || operator==
        TOKEN_TYPE_GTH || operator== TOKEN_TYPE_GEQ || operator== TOKEN_TYPE_EQ || operator==
        TOKEN_TYPE_NEQ;
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

int semanticAnalysis() {
    SymbolTable *globalTable = malloc(sizeof(SymbolTable));
    if (globalTable == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR);
    }

    symbolTableInit(globalTable, NULL);
    symbolTablePush(&symbolTableStack, globalTable);

    symbolSetValues(&currentSymbol, "ifj", TYPE_VOID, false, true, true);

    symbolTableInsert(globalTable, currentSymbol);
    symbolResetValues(&currentSymbol);

    if (ast.root == NULL) {
        HANDLE_ERROR("AST is NULL", INTERNAL_ERROR);
    }

    currentNode = ast.root->right;
    constructNode = ast.root->right;

    // Adds functions to the global scope
    functionAnalysis();

    if (!checkFunctionDefined(globalTable, "main")) {
        HANDLE_ERROR("Main function not defined", UNDEFINED_ERROR);
    }

    statementAnalysis(ast.root->right);

    symbolTablePop(&symbolTableStack);
    return 0;
}