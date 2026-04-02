/**
 * IFJ Project
 *
 * @brief Header file for stack data structure. Functions exit with INTERNAL_ERROR (99) if a
 * null pointer is passed  or a memory allocation fails.
 *
 * @author Matúš Csirik - xcsirim00
 */

#ifndef STACK_H
#define STACK_H

#include "ast.h"
#include "scanner.h"
#include <stdbool.h>

/**
 * @brief Structure representing an element in the stack.
 */
typedef struct StackElement {
    ASTNode *ASTNodePtr;
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
 * @brief Initializes the stack by setting its top pointer to NULL.
 *
 * Exits with INTERNAL_ERROR (99) if the stack pointer is NULL.
 *
 * @param stack Pointer to the stack to initialize.
 */
void initStack(Stack *stack);

/**
 * @brief Initializes a stack element with the provided token.
 *
 * Sets the ASTNode pointer to NULL and the next pointer to NULL.
 * Exits with INTERNAL_ERROR (99) if the element pointer is NULL.
 *
 * @param element Pointer to the element to initialize.
 * @param tokenPtr Pointer to the token to store in the element (can be NULL).
 */
void initStackElement(StackElement *element, Token *tokenPtr);

/**
 * @brief Checks if the stack is empty.
 *
 * Exits with INTERNAL_ERROR (99) if the stack pointer is NULL.
 *
 * @param stack Pointer to the stack to check.
 * @return true if the stack is empty, false otherwise.
 */
bool isEmpty(Stack *stack);

/**
 * @brief Pushes an element onto the stack.
 *
 * Exits with INTERNAL_ERROR (99) if the stack pointer or element pointer is NULL.
 *
 * @param stack Pointer to the stack.
 * @param elementPtr Pointer to the element to be pushed onto the stack.
 */
void push(Stack *stack, StackElement *elementPtr);

/**
 * @brief Pops (removes) the top element from the stack and frees associated resources.
 *
 * Frees the StackElement, and if present, frees the associated Token and disposes of the ASTNode
 * subtree. Exits with INTERNAL_ERROR (99) if the stack pointer is NULL or the stack is empty.
 *
 * @param stack Pointer to the stack.
 */
void pop(Stack *stack);

/**
 * @brief Returns a pointer to the top element of the stack without removing it.
 *
 * Exits with INTERNAL_ERROR (99) if the stack pointer is NULL.
 *
 * @param stack Pointer to the stack.
 * @return A pointer to the element at the top of the stack, or NULL if the stack is empty.
 */
StackElement *top(Stack *stack);

/**
 * @brief Retrieves the token from the top element of the stack without removing it.
 *
 * Exits with INTERNAL_ERROR (99) if the stack pointer is NULL.
 *
 * @param stack Pointer to the stack.
 * @return Pointer to the token of the element at the top of the stack,
 *         or NULL if the stack is empty or the token is NULL.
 */
Token *topToken(Stack *stack);

/**
 * @brief Prints the contents of the stack to standard output for debugging purposes.
 *
 * Exits with INTERNAL_ERROR (99) if the stack pointer is NULL.
 *
 * @param stack Pointer to the stack to display.
 */
void display(Stack *stack);

/**
 * @brief Cleans up the stack by popping all elements and freeing associated resources.
 *
 * Frees each StackElement, its associated Token, and disposes of any ASTNode subtrees.
 * Exits with INTERNAL_ERROR (99) if the stack pointer is NULL.
 *
 * @param stack Pointer to the stack to clean up.
 */
void cleanupStack(Stack *stack);

/**
 * @brief Returns the number of elements in the stack.
 *
 * Exits with INTERNAL_ERROR (99) if the stack pointer is NULL.
 *
 * @param stack Pointer to the stack.
 * @return The number of elements in the stack.
 */
int getStackLength(Stack *stack);

#endif // STACK_H