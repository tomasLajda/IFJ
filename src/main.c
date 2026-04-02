#include "code_generator.h"
#include "enums.h"
#include "error_codes.h"
#include "parser.h"
#include "scanner.h"
#include "semantic_analysis.h"
#include <stdio.h>
#include <stdlib.h>

FILE *sourceFile;
Token *currentToken = NULL;
AST *ast = NULL;

int main() {
    sourceFile = stdin;

    currentToken = malloc(sizeof(Token));
    if (currentToken == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for currentToken.\n");
        fclose(sourceFile);
        return INTERNAL_ERROR;
    }

    int result = getNextToken(currentToken);
    if (result != TOKEN_OK) {
        fprintf(stderr, "Error: getNextToken returned %d\n", result);
        free(currentToken);
        fclose(sourceFile);
        return result;
    }

    parse();
    semanticAnalysis();
    generateCode(stdout, ast);

    free(currentToken);
    fclose(sourceFile);

    return 0;
}