/*
IFJ project

@brief Code generator implementation

@author Vojtěch Gajdušek - xgajduv00
*/
#include "code_generator.h"
#include "error_codes.h"

#define ADD_TO_BUFFER(_code) dynamicStringAddString(&codeBuffer, (const char *)(_code))

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

    // Traverse all nodes to the right of the root - functions and generate code for them
    ASTNode *currentNode = ast->root;
    while (currentNode != NULL) {
        if (currentNode->left->token->attribute.string == "main") {
            mainStart();
            generateFuncBody(currentNode->left);
            mainEnd();
        } else {
            functionStart(currentNode->left->token->attribute.string);
            generateFuncBody(currentNode->left);
            functionEnd(currentNode->left->token->attribute.string);
        }

        currentNode = currentNode->right;
    }

    ADD_TO_BUFFER("$$END\n");
    codeGeneratorFlush(outputFile);

    dynamicStringFree(&codeBuffer);
    return 0;
}

int generateCodeHeader() {
    ADD_TO_BUFFER("# Header\n");
    ADD_TO_BUFFER(".IFJcode24\n");
    generateBuiltInFunctions();
    ADD_TO_BUFFER("JUMP $$main\n");
    ADD_TO_BUFFER("# End of Header\n");
    return 0;
}

int generateBuiltInFunctions() {
    ADD_TO_BUFFER("# Built-in functions\n");
    ADD_TO_BUFFER(BUILT_IN_FUNCTION_READ);
    ADD_TO_BUFFER(BUILT_IN_FUNCTION_WRITE);
    ADD_TO_BUFFER(BUILT_IN_FUNCTION_I2F);
    ADD_TO_BUFFER(BUILT_IN_FUNCTION_F2I);
    ADD_TO_BUFFER(BUILT_IN_FUNCTION_CONCAT);
    ADD_TO_BUFFER(BUILT_IN_FUNCTION_SUBSTRING);
    ADD_TO_BUFFER(BUILT_IN_FUNCTION_ORD);
    ADD_TO_BUFFER(BUILT_IN_FUNCTION_CHR);
    ADD_TO_BUFFER("# End of Built-in functions\n");
}

int generateFuncBody(ASTNode *node) {

    // TODO: pridat list se vsemi promennymi do parametru funkce
    // TODO: DEFVAR pro vsechny promenne z listu
    ASTNode *currentNode = node;

    while (currentNode != NULL) {
        processNode(currentNode);
        currentNode = currentNode->right;
    }
}

int processNode(ASTNode *node) {
    if (node == NULL) {
        return INTERNAL_ERROR;
    }

    switch (node->token->type) {
    case TOKEN_TYPE_IDENTIFIER:
        // TODO: fix if codnitons accoardingly to the ast node structure
        if (node.isAssignment == true) {
            // TODO: GENERATE ASSIGNMENT
        } else if (node.isFunctionCall == true) {
            // TODO: GENERATE FUNCTION CALL
        } else {
            return INTERNAL_ERROR;
        }
        break;

    case TOKEN_TYPE_KEYWORD:
        if (node->token->attribute.keyword == KEYWORD_IF) {
            // TODO: generate if
        } else if (node->token->attribute.keyword == KEYWORD_WHILE) {
            // TODO: generate while loop
        } else {
            // TODO: Skip VARDEF and continue with the next node
        }
        break;

    case TOKEN_TYPE_EXPR:
        // todo: generateExpression(node);
        break;

    default:
        break;
    }
}

int mainStart() {
    ADD_TO_BUFFER("# Start of Main:\n");
    ADD_TO_BUFFER("$$main\n");
    ADD_TO_BUFFER("CREATEFRAME\n");
    ADD_TO_BUFFER("PUSHFRAME\n");
    return 0;
}

int mainEnd() {
    ADD_TO_BUFFER("POPFRAME\n");
    ADD_TO_BUFFER("CLEARS\n"); // todo: odstranit pokud nebude potreba datovy zasobnik
    ADD_TO_BUFFER("JUMP $$END\n");

    return 0;
}

int functionStart(char *functionName) {
    ADD_TO_BUFFER("# Start of function ");
    ADD_TO_BUFFER(functionName);
    ADD_TO_BUFFER(":\n");
    ADD_TO_BUFFER("LABEL $");
    ADD_TO_BUFFER(functionName);
    ADD_TO_BUFFER("\n");
    ADD_TO_BUFFER("PUSHFRAME\n");
    return 0;
}

int functionEnd(char *functionName) {
    ADD_TO_BUFFER("POPFRAME\n");
    ADD_TO_BUFFER("RETURN\n");
    return 0;
}

// int ifStart() { generateExpression(); }

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

        // TODO: problem pri vytvareni novych ramcu, promenna nebude v LF

    case TOKEN_TYPE_INTEGER_VALUE:
        // Deklarace bufferu pro konverzi integeru na řetězec
        char int_str[21]; // 64bitový integer může mít až 20 číslic + znak pro ukončení řetězce

        // Konverze integeru na řetězec
        // Používáme formátovací řetězec "%lld" pro 64bitový integer typu long long int
        snprintf(int_str, sizeof(int_str), "%lld", node->token->attribute.integer);

        dynamicStringAddString(&codeBuffer, "MOVE LF@result int@");
        dynamicStringAddString(&codeBuffer, int_str);
        dynamicStringAddString(&codeBuffer, "\n");
        break;

    case TOKEN_TYPE_DOUBLE_VALUE:
        // Deklarace bufferu pro konverzi double na řetězec
        char double_str[32]; // Buffer pro float hodnotu, 32 znaků by mělo být dostatečné

        // Konverze double na řetězec v hexadecimálním formátu
        // Používáme formátovací řetězec "%a" pro hexadecimální zápis double (kompatibilní s
        // IFJcode24)
        snprintf(double_str, sizeof(double_str), "%a", node->token->attribute.decimal);

        // Přidání instrukce do codeBuffer
        dynamicStringAddString(&codeBuffer, "MOVE LF@result float@");
        dynamicStringAddString(&codeBuffer, double_str);
        dynamicStringAddString(&codeBuffer, "\n");
        break;

    case TOKEN_TYPE_IDENTIFIER:
        dynamicStringAddString(&codeBuffer, "MOVE LF@result LF@");
        dynamicStringAddString(&codeBuffer, node->token->attribute.string);
        dynamicStringAddString(&codeBuffer, "\n");
        break;

    default:
        return INTERNAL_ERROR;
    }

    return 0;
}

void codeGeneratorFlush(FILE *outputFile) { fputs(codeBuffer.string, outputFile); }