#include "../enums.h"
#include "../semantic_analysis.h"
#include <assert.h>
#include <stdio.h>

void test_checkDeclaration() {
    SymbolTable table;
    symbolTableInit(&table, NULL);
    printf("test_checkDeclaration\n");

    Symbol symbol = {.key = "a", .type = TYPE_I_32, .defined = false, .function = false};
    symbolTableInsert(&table, symbol);

    assert(checkDeclaration(&table, "a") == true);
    assert(checkDeclaration(&table, "b") == false);

    symbolTableDispose(&table);
}

void test_checkAssignmentType() {
    SymbolTable table;
    symbolTableInit(&table, NULL);

    Symbol symbol = {.key = "a", .type = TYPE_I_32, .defined = false, .function = false};
    symbolTableInsert(&table, symbol);

    assert(checkAssignmentType(&table, "a", TYPE_I_32) == true);
    assert(checkAssignmentType(&table, "a", TYPE_F_64) == false);

    symbolTableDispose(&table);
}

void test_checkVariableDefined() {
    SymbolTable table;
    symbolTableInit(&table, NULL);

    Symbol symbol = {.key = "a", .type = TYPE_I_32, .defined = true, .function = false};
    symbolTableInsert(&table, symbol);

    assert(checkVariableDefined(&table, "a") == true);
    assert(checkVariableDefined(&table, "b") == false);

    symbolTableDispose(&table);
}

void test_checkFunctionParameter() {
    SymbolTable table;
    symbolTableInit(&table, NULL);

    Symbol symbol = {.key = "a", .type = TYPE_I_32, .defined = false, .function = true};
    List params;
    listInit(&params);
    symbol.params = &params;
    ListData data = {.type = TYPE_I_32};
    listInsertFirst(symbol.params, data);

    ListData data2 = {.type = TYPE_F_64};
    listFirst(&params);
    listInsertAfter(&params, data2);
    listNext(&params);

    symbolTableInsert(&table, symbol);

    assert(checkFunctionParameter(&table, "a", TYPE_I_32, 0) == true);
    assert(checkFunctionParameter(&table, "a", TYPE_F_64, 1) == true);
    assert(checkFunctionParameter(&table, "a", TYPE_F_64, 2) == false);

    symbolTableDispose(&table);
}

void test_checkMultipleLevelTable() {
    SymbolTable table;
    symbolTableInit(&table, NULL);

    Symbol symbol = {.key = "a", .type = TYPE_I_32, .defined = true, .function = false};
    symbolTableInsert(&table, symbol);

    SymbolTable table2;
    symbolTableInit(&table2, &table);

    Symbol symbol2 = {.key = "b", .type = TYPE_F_64, .defined = true, .function = false};
    symbolTableInsert(&table2, symbol2);
    symbolTableSetFunctionKey(&table2, "a");

    assert(checkDeclaration(&table2, "a") == true);
    assert(checkDeclaration(&table2, "b") == true);
    assert(checkDeclaration(&table2, "c") == false);
    assert(checkReturnType(&table2, TYPE_I_32) == true);
    assert(checkReturnType(&table2, TYPE_F_64) == false);

    symbolTableDispose(&table2);
    symbolTableDispose(&table);
}