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

    return 0;
}

int generateCodeHeader() {
    dynamicStringAddString(&codeBuffer, "# Header\n");
    dynamicStringAddString(&codeBuffer, ".IFJcode24\n");
    return 0;
}

void codeGeneratorFlush(FILE *outputFile) {
    fputs(codeBuffer.string, outputFile);
    dynamicStringFree(&codeBuffer);
}