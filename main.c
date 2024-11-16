#include "scanner.h"
#include "error_codes.h"
#include "enums.h"
#include "parser.h"
#include "testing_utils.h"
#include <stdio.h>
#include <stdlib.h>

FILE *sourceFile;
Token *currentToken = NULL;

int main() {
    sourceFile = fopen("test.txt", "r");
    if (sourceFile == NULL) {
        fprintf(stderr, "Error: Cannot open input file.\n");
        return INTERNAL_ERROR;
    }

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

    // while (true) {
    //     int result = getNextToken(currentToken);
    //     if (result != TOKEN_OK) {
    //         printTokenInfo(currentToken);
    //         fprintf(stderr, "Error: getNextToken returned %d\n", result);
    //         free(currentToken);
    //         fclose(sourceFile);
    //         return result;
    //     }

    //     printTokenInfo(currentToken);
        
    //     if (currentToken->type == TOKEN_TYPE_EOF) {
    //         break;
    //     }
    // }

    // printf("--------------------------------\n");
    // printf("Finished scanning tokens\n");


    
    int parseResult = parse();
    printf("\nRESULT: %d\n", parseResult);

    free(currentToken);
    fclose(sourceFile);
    return 0;
}