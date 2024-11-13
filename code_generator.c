/*
IFJ project

@brief Code generator implementation

@author Vojtěch Gajdušek - xgajduv00
*/
#include "code_generator.h"
#include "error_codes.h"

DynamicString codeBuffer;

int generateCode(FILE *outputFile, AST *ast) {
    if (ast == NULL) {
        return INTERNAL_ERROR;
    }
    if (outputFile == NULL) {
        return INTERNAL_ERROR;
    }

    dynamicStringInit(&codeBuffer);

    generateCodeHeader();

    codeGeneratorFlush(outputFile);

    dynamicStringFree(&codeBuffer);
    return 0;
}

int generateCodeHeader() {
    dynamicStringAddString(&codeBuffer, "# Header\n");
    dynamicStringAddString(&codeBuffer, ".IFJcode24\n");
    dynamicStringAddString(&codeBuffer, "JUMP $$main\n");
    return 0;
}

int generateMain() {
    dynamicStringAddString(&codeBuffer, "$$main\n");
    dynamicStringAddString(&codeBuffer, "CREATEFRAME\n");
    dynamicStringAddString(&codeBuffer, "PUSHFRAME\n");

    dynamicStringAddString(&codeBuffer, "POPFRAME\n");
    dynamicStringAddString(&codeBuffer, "CLEARS\n");
    return 0;
}

int generateExpression(ASTNode *node) {
    if (node == NULL) {
        return INTERNAL_ERROR;
    }

    switch (node->token->type) {
        // case TOKEN_TYPE_ASSIGN:
        //     break;

    case TOKEN_TYPE_PLUS:
        // codeGenADD node.left break;

    case TOKEN_TYPE_MINUS:
        break;

    case TOKEN_TYPE_MUL:
        break;

    case TOKEN_TYPE_DIV:
        break;

    case TOKEN_TYPE_LTH:
        break;

    case TOKEN_TYPE_LEQ:
        break;

    case TOKEN_TYPE_GTH:
        break;

    case TOKEN_TYPE_GEQ:
        break;

    case TOKEN_TYPE_EQ:
        break;

    case TOKEN_TYPE_NEQ:
        break;

    default:
        return INTERNAL_ERROR;
    }

    return 0;
}

void codeGeneratorFlush(FILE *outputFile) { fputs(codeBuffer.string, outputFile); }