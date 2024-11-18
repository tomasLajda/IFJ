/*
 * IFJ Project
 * @brief Header file for linked list
 *
 * @author Tomáš Lajda - xlajdat00
 *
 */

#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "enums.h"
#include "error_codes.h"

typedef struct List List;

typedef struct ListData {
    char *key;
    DataType type;
    List *params;
} ListData;

typedef struct ListElement {
    ListData data;
    struct ListElement *nextElement;
} ListElement;

typedef ListElement *ListElementPtr;

typedef struct List {
    ListElementPtr firstElement;
    ListElementPtr activeElement;
    int currentLength;
} List;

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
 * @param data Data to be inserted.
 */
void listInsertFirst(List *list, ListData data);

/**
 * @brief Sets the first element of the list as the active element.
 *
 * @param list Pointer to the list.
 */
void listFirst(List *list);

/**
 * @brief Retrieves the data of the first element in the list.
 *
 * @param list Pointer to the list.
 * @param data Pointer to store the retrieved data.
 */
void listGetFirst(List *list, ListData *data);

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
 * @param data Data to be inserted.
 */
void listInsertAfter(List *list, ListData data);

/**
 * @brief Moves the active element to the next element in the list.
 *
 * @param list Pointer to the list.
 */
void listNext(List *list);

/**
 * @brief Retrieves the data of the active element in the list.
 *
 * @param list Pointer to the list.
 * @param data Pointer to store the retrieved data.
 */
void listGetValue(List *list, ListData *data);

/**
 * @brief Sets the data of the active element in the list.
 *
 * @param list Pointer to the list.
 * @param data Data to be set.
 */
void listSetValue(List *list, ListData data);

/**
 * @brief Checks if the list is active.
 *
 * @param list Pointer to the list.
 * @return int 1 if the list is active, 0 otherwise.
 */
int listIsActive(List *list);

#endif
