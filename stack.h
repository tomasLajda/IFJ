/*
 * IFJ Project
 * @brief Header file for stack data structure
 *
 * @author Matúš Csirik - xcsirim00
 *
 */

#ifndef STACK_H
#define STACK_H

#include "scanner.h"
#include <stdbool.h>

/**
 * @brief Structure representing an element in the stack.
 */
typedef struct StackElement {
    Token *tokenPtr;
    struct StackElement *next;
} StackElement;

/**
 * @brief Structure representing the stack.
 */
typedef struct {
    StackElement *top;
} Stack;

/**
 * @brief Initializes the stack.
 *
 * @param stack Pointer to the stack to initialize.
 */
void initStack(Stack *stack);

/**
 * @brief Checks if the stack is empty.
 *
 * @param stack Pointer to the stack to check.
 * @return true if the stack is empty, false otherwise.
 */
bool isEmpty(Stack *stack);

/**
 * @brief Pushes an element onto the stack.
 *
 * @param stack Pointer to the stack.
 * @param elementPtr Pointer to the element to be pushed onto the stack.
 */
void push(Stack *stack, StackElement *elementPtr);

/**
 * @brief Pops (removes) an element from the stack.
 *
 * @param stack Pointer to the stack.
 */
void pop(Stack *stack);

/**
 * @brief Peeks at the top element of the stack without removing it.
 *
 * @param stack Pointer to the stack.
 * @return A pointer to the element at the top of the stack.
 */
StackElement *top(Stack *stack);

/**
 * @brief Peeks at the token of top element of the stack without removing it.
 *
 * @param stack Pointer to the stack.
 * @return Pointer to the token of the element at the top of the stack.
 */
Token *topToken(Stack *stack);

/**
 * @brief Prints the contents of the stack to standard output.
 *
 * @param stack Pointer to the stack to display.
 */
void display(Stack *stack);

/**
 * @brief Cleans up the stack, freeing all allocated memory.
 *
 * @param stack Pointer to the stack to clean up.
 */
void cleanupStack(Stack *stack);

#endif // _STACK_H