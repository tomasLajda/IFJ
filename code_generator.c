/*
IFJ project

@brief Code generator implementation

@author Vojtěch Gajdušek - xgajduv00
*/
#include "code_generator.h"
#include "error_codes.h"

DynamicString codeBuffer;
int labelCounter = 0;

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

int mainStart() {
    dynamicStringAddString(&codeBuffer, "# Start of Main:\n");
    dynamicStringAddString(&codeBuffer, "$$main\n");
    dynamicStringAddString(&codeBuffer, "CREATEFRAME\n");
    dynamicStringAddString(&codeBuffer, "PUSHFRAME\n");
    return 0;
}

int mainEnd() {
    dynamicStringAddString(&codeBuffer, "POPFRAME\n");
    dynamicStringAddString(&codeBuffer, "CLEARS\n");
    return 0;
}

int functionStart(char *functionName) {
    dynamicStringAddString(&codeBuffer, "LABEL $");
    dynamicStringAddString(&codeBuffer, functionName);
    dynamicStringAddString(&codeBuffer, "\n");
    dynamicStringAddString(&codeBuffer, "PUSHFRAME\n");
    return 0;
}

int functionEnd(char *functionName) {
    dynamicStringAddString(&codeBuffer, "POPFRAME\n");
    dynamicStringAddString(&codeBuffer, "RETURN\n");
    return 0;
}

int generateExpression(ASTNode *node) {
    if (node == NULL) {
        return INTERNAL_ERROR;
    }
    // TODO: kontrola typu (int + int atd int + double nelze!)
    switch (node->token->type) {

    case TOKEN_TYPE_PLUS:
        dynamicStringAddString(&codeBuffer, "ADD LF@result LF@left LF@right\n");
        // codeGenADD node.left break;
        break;

    case TOKEN_TYPE_MINUS:
        dynamicStringAddString(&codeBuffer, "SUB LF@result LF@left LF@right\n");
        break;

    case TOKEN_TYPE_MUL:
        dynamicStringAddString(&codeBuffer, "MUL LF@result LF@left LF@right\n");
        break;

    case TOKEN_TYPE_DIV:
        // todo: kontrola typu

        //  double / double -> DIV
        dynamicStringAddString(&codeBuffer, "DIV LF@result LF@left LF@right\n");
        // int / int -> IDIV
        dynamicStringAddString(&codeBuffer, "IDIV LF@result LF@left LF@right\n");

        break;

    case TOKEN_TYPE_LTH:
        dynamicStringAddString(&codeBuffer, "LT LF@result LF@left LF@right\n");
        break;

    case TOKEN_TYPE_LEQ:
        dynamicStringAddString(&codeBuffer, "GT LF@result LF@left LF@right\n");
        dynamicStringAddString(&codeBuffer, "NOT LF@result LF@result\n");
        break;

    case TOKEN_TYPE_GTH:
        dynamicStringAddString(&codeBuffer, "GT LF@result LF@left LF@right\n");
        break;

    case TOKEN_TYPE_GEQ:
        dynamicStringAddString(&codeBuffer, "LT LF@result LF@left LF@right\n");
        dynamicStringAddString(&codeBuffer, "NOT LF@result LF@result\n");
        break;

    case TOKEN_TYPE_EQ:
        dynamicStringAddString(&codeBuffer, "EQ LF@result LF@left LF@right\n");
        break;

    case TOKEN_TYPE_NEQ:
        dynamicStringAddString(&codeBuffer, "EQ LF@result LF@left LF@right\n");
        dynamicStringAddString(&codeBuffer, "NOT LF@result LF@result\n");
        break;

    default:
        return INTERNAL_ERROR;
    }

    return 0;
}

void codeGeneratorFlush(FILE *outputFile) { fputs(codeBuffer.string, outputFile); }