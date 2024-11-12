#include "tests/test_semantic.c"
#include "tests/test_symtable.c"
#include <stdio.h>

void test_treeInsert();
void test_treeReassign();
void test_checkDeclaration();
void test_checkAssignmentType();
void test_checkVariableDefined();
void test_checkFunctionParameter();
void test_checkMultipleLevelTable();

int main() {
    printf("Running symtable tests...\n");
    test_treeInsert();
    test_treeReassign();
    printf("Symtable tests passed!\n");

    printf("Running semantic analysis tests...\n");
    test_checkDeclaration();
    test_checkAssignmentType();
    test_checkVariableDefined();
    test_checkFunctionParameter();
    test_checkMultipleLevelTable();
    printf("Semantic analysis tests passed!\n");

    printf("All tests passed!\n");
    return 0;
}