/*
 * IFJ Project
 * @brief Implementation file for stack data structure
 *
 * @author Matúš Csirik - xcsirim00
 *
 */

#include "stack.h"
#include "error_codes.h"
#include <stdio.h>
#include <stdlib.h>

void initStack(Stack *stack) { stack->top = NULL; }

void initStackElement(StackElement *element, Token *tokenPtr) {
    element->tokenPtr = tokenPtr;
    element->next = NULL;
}

bool isEmpty(Stack *stack) { return stack->top == NULL; }

void push(Stack *stack, StackElement *elementPtr) {
    if (isEmpty(stack)) {
        stack->top = elementPtr;
    } else {
        elementPtr->next = stack->top;
        stack->top = elementPtr;
    }
}

void pop(Stack *stack) {
    if (isEmpty(stack)) {
        fprintf(stderr, "Stack is empty\n");
        exit(INTERNAL_ERROR);
    }
    StackElement *tmp = stack->top;
    stack->top = stack->top->next;
    free(tmp);
}

StackElement *top(Stack *stack) {
    if (isEmpty(stack)) {
        fprintf(stderr, "Stack is empty\n");
        exit(INTERNAL_ERROR);
    }
    return stack->top;
}

Token *topToken(Stack *stack) {
    if (isEmpty(stack)) {
        fprintf(stderr, "Stack is empty\n");
        exit(INTERNAL_ERROR);
    }
    return stack->top->tokenPtr;
}

void display(Stack *stack) {
    if (isEmpty(stack)) {
        printf("Stack is empty\n");
        return;
    }
    printf("Top -> ");
    StackElement *current = stack->top;
    while (current != NULL) {
        printf("| %d ", current->tokenPtr->type);
        current = current->next;
    }
    printf("|\n");
}

void cleanupStack(Stack *stack) {
    while (!isEmpty(stack)) {
        pop(stack);
    }
}