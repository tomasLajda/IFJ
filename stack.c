/**
 * IFJ Project
 *
 * @brief Implementation file for stack data structure. Functions exit with INTERNAL_ERROR (99) if a
 * null pointer is passed or a memory allocation fails.
 *
 * @author Matúš Csirik - xcsirim00
 */

#include "stack.h"
#include "ast.h"
#include "error_codes.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>

void initStack(Stack *stack) {
    if (stack == NULL) {
        HANDLE_ERROR("Stack pointer is NULL", INTERNAL_ERROR);
    }
    stack->top = NULL;
}

void initStackElement(StackElement *element, Token *tokenPtr) {
    if (element == NULL) {
        HANDLE_ERROR("StackElement pointer is NULL", INTERNAL_ERROR);
    }
    element->tokenPtr = tokenPtr;
    element->ASTNodePtr = NULL;
    element->next = NULL;
}

bool isEmpty(Stack *stack) {
    if (stack == NULL) {
        HANDLE_ERROR("Stack pointer is NULL", INTERNAL_ERROR, true);
    }
    return stack->top == NULL;
}

void push(Stack *stack, StackElement *elementPtr) {
    if (stack == NULL || elementPtr == NULL) {
        HANDLE_ERROR("Stack or StackElement pointer is NULL", INTERNAL_ERROR);
    }
    if (isEmpty(stack)) {
        stack->top = elementPtr;
    } else {
        elementPtr->next = stack->top;
        stack->top = elementPtr;
    }
}

void pop(Stack *stack) {
    if (stack == NULL) {
        HANDLE_ERROR("Stack pointer is NULL", INTERNAL_ERROR);
    }
    if (isEmpty(stack)) {
        HANDLE_ERROR("Stack is empty", INTERNAL_ERROR);
    }
    // Temporarily store the top element of the stack
    StackElement *tmp = stack->top;
    // Update the top element of the stack
    stack->top = stack->top->next;

    // Free the token and ASTNode subtree associated with the element
    if (tmp->tokenPtr != NULL) {
        freeToken(tmp->tokenPtr);
    }
    if (tmp->ASTNodePtr != NULL) {
        disposeSubtree(tmp->ASTNodePtr);
    }
    free(tmp);
}

StackElement *top(Stack *stack) {
    if (stack == NULL) {
        HANDLE_ERROR("Stack pointer is NULL", INTERNAL_ERROR, NULL);
    }
    if (isEmpty(stack)) {
        return NULL;
    }
    return stack->top;
}

Token *topToken(Stack *stack) {
    if (stack == NULL) {
        HANDLE_ERROR("Stack pointer is NULL", INTERNAL_ERROR, NULL);
    }
    if (isEmpty(stack)) {
        return NULL;
    }
    if (stack->top->tokenPtr == NULL) {
        return NULL;
    }
    return stack->top->tokenPtr;
}

void display(Stack *stack) {
    if (stack == NULL) {
        HANDLE_ERROR("Stack pointer is NULL", INTERNAL_ERROR);
    }
    if (isEmpty(stack)) {
        printf("Stack is empty\n");
        return;
    }
    printf("Top -> ");
    StackElement *current = stack->top;
    while (current != NULL) {
        printf("| %s ", TokenTypeToString(current->tokenPtr->type));
        current = current->next;
    }
    printf("|\n");
}

void cleanupStack(Stack *stack) {
    if (stack == NULL) {
        HANDLE_ERROR("Stack pointer is NULL", INTERNAL_ERROR);
    }
    while (!isEmpty(stack)) {
        pop(stack);
    }
}

int getStackLength(Stack *stack) {
    if (stack == NULL) {
        HANDLE_ERROR("Stack pointer is NULL", INTERNAL_ERROR, -1);
    }
    int length = 0;
    StackElement *current = stack->top;
    while (current != NULL) {
        length++;
        current = current->next;
    }
    return length;
}