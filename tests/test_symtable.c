#include "../enums.h"
#include "../symtable.h"
#include <assert.h>
#include <stdio.h>

void test_treeInsert() {
    SymbolTable table;
    symbolTableInit(&table, NULL);

    Symbol symbol1 = {.key = "a", .type = TYPE_I_32, .defined = false, .function = false};
    Symbol symbol2 = {.key = "b", .type = TYPE_F_64, .defined = false, .function = false};
    Symbol symbol3 = {.key = "c", .type = TYPE_U_8_ARRAY, .defined = false, .function = false};
    Symbol symbol4 = {.key = "d", .type = TYPE_VOID, .defined = false, .function = false};
    Symbol symbol5 = {.key = "e", .type = TYPE_I_32, .defined = false, .function = false};
    Symbol symbol6 = {.key = "f", .type = TYPE_F_64, .defined = false, .function = false};
    Symbol symbol7 = {.key = "g", .type = TYPE_U_8_ARRAY, .defined = false, .function = false};
    Symbol symbol8 = {.key = "h", .type = TYPE_VOID, .defined = false, .function = false};
    Symbol symbol9 = {.key = "i", .type = TYPE_I_32, .defined = false, .function = false};
    Symbol symbol10 = {.key = "j", .type = TYPE_F_64, .defined = false, .function = false};

    symbolTableInsert(&table, symbol1);
    printf("After inserting symbol1:\n");
    symbolTablePrint(&table);

    symbolTableInsert(&table, symbol2);
    printf("After inserting symbol2:\n");
    symbolTablePrint(&table);

    symbolTableInsert(&table, symbol3);
    printf("After inserting symbol3:\n");
    symbolTablePrint(&table);

    symbolTableInsert(&table, symbol4);
    printf("After inserting symbol4:\n");
    symbolTablePrint(&table);

    symbolTableInsert(&table, symbol5);
    printf("After inserting symbol5:\n");
    symbolTablePrint(&table);

    symbolTableInsert(&table, symbol6);
    printf("After inserting symbol6:\n");
    symbolTablePrint(&table);

    symbolTableInsert(&table, symbol7);
    printf("After inserting symbol7:\n");
    symbolTablePrint(&table);

    symbolTableInsert(&table, symbol8);
    printf("After inserting symbol8:\n");
    symbolTablePrint(&table);

    symbolTableInsert(&table, symbol9);
    printf("After inserting symbol9:\n");
    symbolTablePrint(&table);

    symbolTableInsert(&table, symbol10);
    printf("After inserting symbol10:\n");
    symbolTablePrint(&table);

    printf("Root left: %s\n", table.root->right->data.key);

    assert(symbolTableSearch(&table, "a") == true);
    assert(symbolTableSearch(&table, "b") == true);
    assert(symbolTableSearch(&table, "c") == true);
    assert(symbolTableSearch(&table, "d") == true);
    assert(symbolTableSearch(&table, "e") == true);
    assert(symbolTableSearch(&table, "f") == true);
    assert(symbolTableSearch(&table, "g") == true);
    assert(symbolTableSearch(&table, "h") == true);
    assert(symbolTableSearch(&table, "i") == true);
    assert(symbolTableSearch(&table, "j") == true);
    assert(symbolTableSearch(&table, "k") == false);

    symbolTableDispose(&table);
}

void test_treeReassign() {
    SymbolTable table;
    symbolTableInit(&table, NULL);

    Symbol symbol1 = {.key = "a", .type = TYPE_I_32, .defined = true, .function = false};
    Symbol symbol2 = {.key = "a", .type = TYPE_F_64, .defined = false, .function = false};

    symbolTableInsert(&table, symbol1);
    printf("After inserting symbol1:\n");
    printf("Symbol a: %b\n", symbolTableGetSymbol(&table, "a")->defined);
    symbolTableReassign(&table, "a", symbol2);
    printf("After reassigning symbol2:\n");
    printf("Symbol a: %b\n", symbolTableGetSymbol(&table, "a")->defined);
    symbolTableSetDefined(&table, "a");
    printf("After setting defined flag:\n");
    printf("Symbol a: %b\n", symbolTableGetSymbol(&table, "a")->defined);

    Symbol *symbol = symbolTableGetSymbol(&table, "a");
    assert(symbol != NULL);
    assert(symbol->type == TYPE_F_64);
    assert(symbol->defined == true);

    symbolTableDispose(&table);
}

void test_stackTable() {
    SymbolTable table;
    symbolTableInit(&table, NULL);

    Symbol symbol1 = {.key = "a", .type = TYPE_I_32, .defined = true, .function = false};
    Symbol symbol2 = {.key = "b", .type = TYPE_F_64, .defined = false, .function = false};
    Symbol symbol3 = {.key = "c", .type = TYPE_U_8_ARRAY, .defined = false, .function = false};
    Symbol symbol4 = {.key = "d", .type = TYPE_VOID, .defined = false, .function = false};

    symbolTableInsert(&table, symbol1);
    symbolTableInsert(&table, symbol2);
    symbolTableInsert(&table, symbol3);
    symbolTableInsert(&table, symbol4);

    Stack stack;
    initStack(&stack);

    symbolTablePush(&stack, &table);
    SymbolTable *top = symbolTableTop(&stack);
    symbolTablePrint(top);
    assert(top != NULL);
    assert(top->root != NULL);

    SymbolTable table2;
    symbolTableInit(&table2, &table);

    Symbol symbol5 = {.key = "e", .type = TYPE_I_32, .defined = true, .function = false};
    Symbol symbol6 = {.key = "f", .type = TYPE_F_64, .defined = false, .function = false};
    Symbol symbol7 = {.key = "g", .type = TYPE_U_8_ARRAY, .defined = false, .function = false};
    Symbol symbol8 = {.key = "h", .type = TYPE_VOID, .defined = false, .function = false};

    symbolTableInsert(&table2, symbol5);
    symbolTableInsert(&table2, symbol6);
    symbolTableInsert(&table2, symbol7);
    symbolTableInsert(&table2, symbol8);

    symbolTablePush(&stack, &table2);
    top = symbolTableTop(&stack);
    symbolTablePrint(top);
    assert(top != NULL);

    symbolTablePop(&stack);
    top = symbolTableTop(&stack);
    symbolTablePrint(top);
    assert(top != NULL);

    symbolTablePop(&stack);
    top = symbolTableTop(&stack);
    assert(top == NULL);
}