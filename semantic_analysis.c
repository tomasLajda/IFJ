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
ListData currentParameter;
DataType returnType;
ASTNode *currentNode;
ASTNode *constructNode;

bool checkDeclaration(SymbolTable *table, const char *key) { return symbolTableSearch(table, key); }

bool checkAssignmentType(SymbolTable *table, const char *key, DataType valueType) {
    Symbol *symbol = symbolTableGetSymbol(table, key);
    return symbol != NULL && symbol->type == valueType;
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
        break;

    case KEYWORD_WHILE:

        break;

    case KEYWORD_CONST:

        break;

    case KEYWORD_VAR:

        break;

    case KEYWORD_UNDERSCORE:

        break;

    case KEYWORD_RETURN:

        break;

    default:
        if (node->isAssignment) {

        } else {
        }
        break;
    }

    statementAnalysis(node->right);
}

void functionBodyAnalysis(ASTNode *node) {
    if (node == NULL) {
        return;
    }

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
    if (node == NULL) {
        return;
    }

    SymbolTable *table = malloc(sizeof(SymbolTable));
    if (table == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR);
    }
    symbolTableInit(table, symbolTableTop(&symbolTableStack));
    symbolTablePush(&symbolTableStack, table);

    node = node->left;

    if (node->token->type == TOKEN_TYPE_KEYWORD && node->token->attribute.keyword == KEYWORD_NULL) {
        node = node->left;
    }

    // TODO add ast analysis

    statementAnalysis(node->left);
    statementAnalysis(node->right);

    symbolTableCheckUsed(table);
    symbolTablePop(&symbolTableStack);
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

    if (ast == NULL) {
        HANDLE_ERROR("AST is NULL", INTERNAL_ERROR);
    }

    currentNode = ast->root->right;
    constructNode = ast->root->right;

    // Functions are in the global scope
    functionAnalysis();

    if (!checkFunctionDefined(globalTable, "main")) {
        HANDLE_ERROR("Main function not defined", UNDEFINED_ERROR);
    }

    statementAnalysis(ast->root->right);

    symbolTablePop(&symbolTableStack);
    return 0;
}