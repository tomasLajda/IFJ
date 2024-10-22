/*
 * IFJ Project
 * @brief Header file for dynamic string
 *
 * @author Tomáš Lajda - xlajdat00
 *
 */

#ifndef _DYNAMIC_STRING_H
#define _DYNAMIC_STRING_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct dynamic_string_t {
    char *string;
    unsigned length;
    unsigned allocated;
};

typedef struct dynamic_string_t DynamicString;

/**
 * @brief Initializes a dynamic string.
 *
 * @param ds Pointer to the dynamic string to initialize.
 */
void dynamicStringInit(DynamicString *ds);

/**
 * @brief Frees the memory allocated for the dynamic string.
 *
 * @param ds Pointer to the dynamic string to free.
 */
void dynamicStringFree(DynamicString *ds);

/**
 * @brief Resizes the dynamic string to the specified size.
 *
 * @param ds Pointer to the dynamic string to resize.
 * @param size New size for the dynamic string.
 */
void dynamicStringResize(DynamicString *ds, unsigned size);

/**
 * @brief Adds a character to the dynamic string.
 *
 * @param ds Pointer to the dynamic string.
 * @param c Character to add.
 */
void dynamicStringAddChar(DynamicString *ds, char c);

/**
 * @brief Adds a C-string to the dynamic string.
 *
 * @param ds Pointer to the dynamic string.
 * @param string C-string to add.
 */
void dynamicStringAddString(DynamicString *ds, const char *string);

/**
 * @brief Replaces a character at a specified index in the dynamic string.
 *
 * @param ds Pointer to the dynamic string.
 * @param c Character to replace with.
 * @param index Index at which to replace the character.
 */
void dynamicStringReplaceChar(DynamicString *ds, char c, unsigned index);

/**
 * @brief Copies the content of one dynamic string to another.
 *
 * @param ds Pointer to the source dynamic string.
 * @param copy Pointer to the destination dynamic string.
 */
void dynamicStringCopy(const DynamicString *ds, DynamicString *copy);

/**
 * @brief Compares the dynamic string with a C-string.
 *
 * @param ds Pointer to the dynamic string.
 * @param string C-string to compare with.
 * @return true if the strings are equal, false otherwise.
 */
bool dynamicStringCompare(const DynamicString *ds, const char *string);

/**
 * @brief Prints the dynamic string.
 *
 * @param ds Pointer to the dynamic string to print.
 */
void dynamicStringPrint(const DynamicString *ds);

/**
 * @brief Converts the dynamic string to a C-string.
 *
 * @param ds Pointer to the dynamic string.
 * @return Pointer to the C-string.
 */
char *dynamicStringToCString(const DynamicString *ds);

#endif