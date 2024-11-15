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

    // Get the first token
    int result = getNextToken(currentToken);
    if (result != TOKEN_OK) {
        fprintf(stderr, "Error: getNextToken returned %d\n", result);
        free(currentToken);
        fclose(sourceFile);
        return result;
    }

    // Token token;
    // int result;

    // while (true) {
    //     result = getNextToken(&token);
    //     if (result != TOKEN_OK) {
    //         fprintf(stderr, "Error: getNextToken returned %d\n", result);
    //         fclose(sourceFile);
    //         printf("RESULT: %d\n", result);
    //         return result;
    //     }

    //     if (token.type == TOKEN_TYPE_EOF) {
    //         printf("End of File reached.\n");
    //         break;
    //     }

    //     printf("Token type: %s\n", getTokenTypeName(token.type));

    //     switch (token.type) {
    //         case TOKEN_TYPE_IDENTIFIER:
    //             printf("Identifier: %s\n", token.attribute.string);
    //             free(token.attribute.string);
    //             break;
    //         case TOKEN_TYPE_STRING_VALUE:
    //             printf("String: \"%s\"\n", token.attribute.string);
    //             free(token.attribute.string);
    //             break;
    //         case TOKEN_TYPE_INTEGER_VALUE:
    //             printf("Integer: %d\n", token.attribute.integer);
    //             break;
    //         case TOKEN_TYPE_DOUBLE_VALUE:
    //             printf("Double: %f\n", token.attribute.decimal);
    //             break;
    //         case TOKEN_TYPE_KEYWORD:
    //             printf("Keyword: %s\n", getKeywordName(token.attribute.keyword));
    //             break;
    //         case TOKEN_TYPE_EOL:
    //             printf("End of Line\n");
    //             break;
    //         case TOKEN_TYPE_COMMENT:
    //             printf("Comment\n");
    //             break;
    //         case TOKEN_TYPE_EQ:
    //         case TOKEN_TYPE_NEQ:
    //         case TOKEN_TYPE_LTH:
    //         case TOKEN_TYPE_LEQ:
    //         case TOKEN_TYPE_GTH:
    //         case TOKEN_TYPE_GEQ:
    //             printf("Operator: %s\n", getTokenTypeName(token.type));
    //             break;
    //         default:
    //             break;
    //     }

    //     printf("-------------------\n");
    // }

    int parseResult = parse();

    printf("\nRESULT: %d\n", parseResult);
    free(currentToken);
    fclose(sourceFile);
    return 0;
}