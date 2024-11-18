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

addFloatToBuffer(float value) {
    ADD_TO_BUFFER("float@");
    int enoughSpaceForDouble = 64;
    char doubleStr[enoughSpaceForDouble];
    snprintf(doubleStr, sizeof(doubleStr), "%a", value);
    ADD_TO_BUFFER(doubleStr);
    ADD_TO_BUFFER(" \n");
}

addIntToBuffer(int value) {
    ADD_TO_BUFFER("int@");
    int enoughSpaceForInt = (int)((ceil(log10(value)) + 1) * sizeof(char));
    char intStr[enoughSpaceForInt];
    snprintf(intStr, sizeof(intStr), "%d", value);
    ADD_TO_BUFFER(intStr);
    ADD_TO_BUFFER(" \n");
}

// TODO: function for generating unique labels
// TODO : function for generating unique temporary variables? mozna nebude potreba - diky pouziti
// datoveho zasobniku pro expressions

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
    int paramID = 0;
    while (currentNode != NULL) {
        generateParam(currentNode, paramID); // DEFVAR pro parametry MOV z TF do LF
        currentNode = currentNode->right;
        paramID++;
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
        if (node->isAssignment == true) {
            // TODO: GENERATE ASSIGNMENT
            // pro node.left.exprTree rozhodnout jestli je func call nebo expr
            if (node->left->exprTree->isExpression == true) {
                generateExpression(node->left->exprTree->root);
            } else {
                // gen func call
                generateFuncCall(node->left->exprTree->root);
            }

        } else {
            generateFuncCall(node);
        }
        break;

    case TOKEN_TYPE_KEYWORD:
        if (node->token->attribute.keyword == KEYWORD_IF) {
            // TODO: generate if
            if (node->left->token->type == TOKEN_TYPE_NULL_COND) {
                // TODO: nullable if
            } else {
                // TODO: normal if
            }

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
            if (node->left->token->type == TOKEN_TYPE_NULL_COND) {
                // TODO: nullable while
            } else {
                // TODO: normal while
            }

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
        }
        break;

    // TODO: odstranit, sam o sobe nikdy nebude
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
    ADD_TO_BUFFER("CLEARS\n");
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
        ADD_TO_BUFFER("PUSHS ");
        addIntToBuffer(node->token->attribute.integer);

    } else if (currentTokenType == TOKEN_TYPE_DOUBLE_VALUE) {
        // Push double value onto the data stack
        // PUSHS float@value
        ADD_TO_BUFFER("PUSHS ");
        addFloatToBuffer(node->token->attribute.decimal);

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
            ADD_TO_BUFFER("EQS\n");
        } else if (currentTokenType == TOKEN_TYPE_NEQ) {
            ADD_TO_BUFFER("EQS\n");
            ADD_TO_BUFFER("NOTS\n");
        } else if (currentTokenType == TOKEN_TYPE_LTH) {
            ADD_TO_BUFFER("LTS\n");
        } else if (currentTokenType == TOKEN_TYPE_LEQ) {
            ADD_TO_BUFFER("GTS\n");
            ADD_TO_BUFFER("NOTS\n");
        } else if (currentTokenType == TOKEN_TYPE_GTH) {
            ADD_TO_BUFFER("GTS\n");
        } else if (currentTokenType == TOKEN_TYPE_GEQ) {
            ADD_TO_BUFFER("LTS\n");
            ADD_TO_BUFFER("NOTS\n");
        } else {
            return INTERNAL_ERROR;
        }
    } else {
        return INTERNAL_ERROR;
    }

    return 0;
}

int generateFuncCall(ASTNode *node) {
    if (node == NULL) {
        return INTERNAL_ERROR;
    }
    // node - function name
    ADD_TO_BUFFER("CREATEFRAME\n");
    ADD_TO_BUFFER("DEFVAR TF@%%retval\n");

    // generate parameters
    ASTNode *currentNode = node->left;
    // currentNode - each parameter
    while (currentNode != NULL) {
        // TODO u defvaru pouzit unikatni jmeno?
        ADD_TO_BUFFER("DEFVAR TF@%%arg");
        // ADD_TO_BUFFER(uniqueID);
        ADD_TO_BUFFER("\n");
        ADD_TO_BUFFER("MOVE TF@%%arg");
        // ADD_TO_BUFFER(uniqueID);
        ADD_TO_BUFFER(" ");

        TokenType currentType = currentNode->exprTree->root->token->type;

        if (currentType == TOKEN_TYPE_IDENTIFIER) {
            ADD_TO_BUFFER("LF@");
            ADD_TO_BUFFER(currentNode->exprTree->root->token->attribute.string);
            ADD_TO_BUFFER(" \n");
        } else if (currentType == TOKEN_TYPE_INTEGER_VALUE) {
            addIntToBuffer(currentNode->exprTree->root->token->attribute.integer);
        } else if (currentType == TOKEN_TYPE_DOUBLE_VALUE) {
            addFloatToBuffer(currentNode->exprTree->root->token->attribute.decimal);
        } else {
            return INTERNAL_ERROR;
        }
        currentNode = currentNode->right;
    }

    ADD_TO_BUFFER("CALL $");
    ADD_TO_BUFFER(node->token->attribute.string);
    ADD_TO_BUFFER(" \n");
    return 0;
}

int generateParam(ASTNode *node, int paramID) {
    if (node == NULL) {
        return INTERNAL_ERROR;
    }
    char *paramName = node->token->attribute.string;

    ADD_TO_BUFFER("DEFVAR LF@");
    ADD_TO_BUFFER(paramName);
    ADD_TO_BUFFER("\n");
    ADD_TO_BUFFER("MOVE LF@");
    ADD_TO_BUFFER(paramName);
    ADD_TO_BUFFER(" LF@");
    // ADD_TO_BUFFER(argID);
    ADD_TO_BUFFER("\n");
    return 0;
}

void codeGeneratorFlush(FILE *outputFile) { fputs(codeBuffer.string, outputFile); }