/*
 * IFJ Project
 * @brief Header file for linked list
 *
 * @author Tomáš Lajda - xlajdat00
 *
 */

#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

#include <stdio.h>
#include <stdlib.h>

#include "error_codes.h"

struct list_element_t {
    char *key;
    struct list_element_t *nextElement;
} typedef ListElement;

typedef ListElement *ListElementPtr;

struct list_t {
    ListElementPtr firstElement;
    ListElementPtr activeElement;
    int currentLength;
} typedef List;

/**
 * @brief Initializes the list.
 *
 * @param list Pointer to the list to be initialized.
 */
void listInit(List *list);

/**
 * @brief Disposes of the list and frees allocated memory.
 *
 * @param list Pointer to the list to be disposed of.
 */
void listDispose(List *list);

/**
 * @brief Inserts a new element at the beginning of the list.
 *
 * @param list Pointer to the list.
 * @param key Key to be inserted.
 */
void listInsertFirst(List *list, char *key);

/**
 * @brief Sets the first element of the list as the active element.
 *
 * @param list Pointer to the list.
 */
void listFirst(List *list);

/**
 * @brief Retrieves the key of the first element in the list.
 *
 * @param list Pointer to the list.
 * @param key Pointer to store the retrieved key.
 */
void listGetFirst(List *list, char **key);

/**
 * @brief Deletes the first element of the list.
 *
 * @param list Pointer to the list.
 */
void listDeleteFirst(List *list);

/**
 * @brief Deletes the element after the active element.
 *
 * @param list Pointer to the list.
 */
void listDeleteAfter(List *list);

/**
 * @brief Inserts a new element after the active element.
 *
 * @param list Pointer to the list.
 * @param key Key to be inserted.
 */
void listInsertAfter(List *list, char *key);

/**
 * @brief Moves the active element to the next element in the list.
 *
 * @param list Pointer to the list.
 */
void listNext(List *list);

/**
 * @brief Retrieves the key of the active element in the list.
 *
 * @param list Pointer to the list.
 * @param key Pointer to store the retrieved key.
 */
void listGetValue(List *list, char **key);

/**
 * @brief Sets the key of the active element in the list.
 *
 * @param list Pointer to the list.
 * @param key Key to be set.
 */
void listSetValue(List *list, char *key);

/**
 * @brief Checks if the list is active.
 *
 * @param list Pointer to the list.
 * @return int 1 if the list is active, 0 otherwise.
 */
int listIsActive(List *list);

#endif
