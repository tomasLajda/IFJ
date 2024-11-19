#include "ast.h"
#include "error_codes.h"
#include "scanner.h"
#include <stdio.h>

FILE *sourceFile;
AST *programAST;

int mock_parse() { return 0; }
int mock_semantic_analysis() { return 0; }
int mock_code_gen(FILE *outputFile, AST *ast) {
    (void)outputFile;
    (void)ast;
    return 0;
}

int main() {
    // INIT
    sourceFile = stdin; // TODO: Change to stdin
    if (sourceFile == NULL) {
        HANDLE_ERROR("Failed to open source file.\n", INTERNAL_ERROR);
    }
    programAST = initAST();
    if (programAST == NULL) {
        HANDLE_ERROR("Failed to initialize AST.\n", INTERNAL_ERROR);
    }
    ASTNode *nullRoot = initASTNode();
    if (nullRoot == NULL) {
        HANDLE_ERROR("Failed to initialize AST node.\n", INTERNAL_ERROR);
    }
    programAST->root = nullRoot;

    // PARSE (1st pass)
    int parseResult = mock_parse();
    switch (parseResult) {
    case 0:
        printf("Parse successful.\n");
        break;
    default:
        printf("Parse failed.\n");
        return parseResult;
        break;
    }

    // SEMANTIC ANALYSIS (2nd pass)
    int semanticResult = mock_semantic_analysis();
    switch (semanticResult) {
    case 0:
        printf("Semantic analysis successful.\n");
        break;
    default:
        printf("Semantic analysis failed.\n");
        return semanticResult;
        break;
    }

    displayAST(programAST); // TODO: Remove

    // GENERATE CODE (3rd pass)
    int codeGenResult = mock_code_gen(stdout, programAST);
    switch (codeGenResult) {
    case 0:
        printf("Code generation successful.\n");
        break;
    default:
        printf("Code generation failed.\n");
        return codeGenResult;
        break;
    }

    // CLEANUP
    freeAST(programAST);
    fclose(sourceFile);

    return 0;
}