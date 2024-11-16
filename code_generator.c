/*
IFJ project

@brief Code generator implementation

@author Vojtěch Gajdušek - xgajduv00
*/
#include "code_generator.h"
#include "enums.h"
#include "error_codes.h"
#include "helpers.h"

#define ADD_TO_BUFFER(_code) dynamicStringAddString(&codeBuffer, (const char *)(_code))

DynamicString codeBuffer;
int labelCounter = 0;
int expressionCounter = 0;
int tempVarCounter = 0;

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
    // zrovna udelat node.right a parametry vygenerovat v func start?

    // FUNCTION PARAMETERS
    ASTNode *currentNode = node->left;

    while (currentNode != NULL) {
        createParam(currentNode); // DEFVAR pro parametry MOV z TF do LF
        currentNode = currentNode->left;
    }

    // FUNCTION CODE
    currentNode = node->right;

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
        // TODO: fix if conditions accoardingly to the ast node structure
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

            // ASTNode *conditionNode = node->left;
            // generateExpression(conditionNode); // conditionNode.exprTree.root
            // defvar LF@condition
            // pops LF@condition  ( vysledek vyrazu ze zasobniku)
            // ADD_TO_BUFFER("JUMPIFEQ $if_false_lbl_counter LF@condition bool@false\n");
            // processNode(conditionNode.left); // true
            // ADD_TO_BUFFER("JUMP $if_end_lbl_counter\n");
            // ADD_TO_BUFFER("$if_false_lbl_counter\n");
            // processNode(conditionNode.right); // false
            // ADD_TO_BUFFER("$if_end_lbl_counter\n");

        } else if (node->token->attribute.keyword == KEYWORD_WHILE) {
            // TODO: generate while loop

            // ASTNode *conditionNode = node->left;
            // ADD_TO_BUFFER("$while_start_lbl_counter\n");
            // generateExpression(conditionNode); // conditionNode.exprTree.root
            // defvar LF@condition
            // pops LF@condition  ( vysledek vyrazu ze zasobniku)
            // ADD_TO_BUFFER("JUMPIFEQ $while_end_lbl_counter LF@condition bool@false\n");
            // processNode(conditionNode.left); // while body
            // ADD_TO_BUFFER("JUMP $while_start_lbl_counter\n");
            // ADD_TO_BUFFER("$while_end_lbl_counter\n");
        } else {
            // TODO: Skip VARDEF and continue with the next node
            // TODO: nullable if (while?)
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

    // process left subtree
    generateExpression(node->left);
    // process right subtree
    generateExpression(node->right);

    TokenType currentTokenType = node->token->type;

    if (currentTokenType == TOKEN_TYPE_IDENTIFIER) {
        // Push variable onto the data stack
        ADD_TO_BUFFER("PUSHS LF@");
        ADD_TO_BUFFER(node->token->attribute.string);
        ADD_TO_BUFFER(" \n");

    } else if (currentTokenType == TOKEN_TYPE_INTEGER_VALUE) {
        // Push integer value onto the data stack
        // PUSHS int@value
        ADD_TO_BUFFER("PUSHS int@");
        int enoughSpaceForInt =
            (int)((ceil(log10(node->token->attribute.integer)) + 1) * sizeof(char));
        char intStr[enoughSpaceForInt];
        snprintf(intStr, sizeof(intStr), "%d", node->token->attribute.integer);
        ADD_TO_BUFFER(intStr);
        ADD_TO_BUFFER(" \n");

    } else if (currentTokenType == TOKEN_TYPE_DOUBLE_VALUE) {
        // Push double value onto the data stack
        // PUSHS float@value
        ADD_TO_BUFFER("PUSHS float@");
        int enoughSpaceForDouble = 64;
        char doubleStr[enoughSpaceForDouble];
        snprintf(doubleStr, sizeof(doubleStr), "%a", node->token->attribute.decimal);
        ADD_TO_BUFFER(doubleStr);
        ADD_TO_BUFFER(" \n");

    } else if (isTokenTypeOperator(currentTokenType)) {
        if (currentTokenType == TOKEN_TYPE_PLUS) {
            ADD_TO_BUFFER("ADDS\n");
        } else if (currentTokenType == TOKEN_TYPE_MINUS) {
            ADD_TO_BUFFER("SUBS\n");
        } else if (currentTokenType == TOKEN_TYPE_MUL) {
            ADD_TO_BUFFER("MULS\n");
        } else if (currentTokenType == TOKEN_TYPE_DIV) {
            ADD_TO_BUFFER("DIVS\n");
        } else if (currentTokenType == TOKEN_TYPE_EQ) {
            // todo: implement
        } else if (currentTokenType == TOKEN_TYPE_NEQ) {
            // todo: implement
        } else if (currentTokenType == TOKEN_TYPE_LTH) {
            // todo: implement
        } else if (currentTokenType == TOKEN_TYPE_LEQ) {
            // todo: implement
        } else if (currentTokenType == TOKEN_TYPE_GTH) {
            // todo: implement
        } else if (currentTokenType == TOKEN_TYPE_GEQ) {
            // todo: implement
        } else {
            return INTERNAL_ERROR;
        }
    } else {
        return INTERNAL_ERROR;
    }

    return 0;
}

void codeGeneratorFlush(FILE *outputFile) { fputs(codeBuffer.string, outputFile); }