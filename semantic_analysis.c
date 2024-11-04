/*
 * IFJ Project
 * @brief Header file for semantic analysis
 *
 * @author Tomáš Lajda - xlajdat00
 *
 */

#include "semantic_analysis.h"

bool checkDeclaration(SymbolTable *table, const char *key) { return symbolTableSearch(table, key); }

bool checkAssignmentType(SymbolTable *table, const char *key, DataType valueType) {
    Symbol *symbol = symbolTableGetSymbol(table, key);
    return symbol != NULL && symbol->type == valueType;
}

bool checkVariableAssigned(SymbolTable *table, const char *key) {
    Symbol *symbol = symbolTableGetSymbol(table, key);
    return symbol != NULL && symbol->defined;
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

    Symbol *symbol = symbolTableGetSymbol(table, table->functionKey);

    return symbol != NULL && symbol->type == type;
}