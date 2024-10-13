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
} typedef DynamicString;

void DynamicStringInit(DynamicString *ds);

void DynamicStringFree(DynamicString *ds);

void DynamicStringClear(DynamicString *ds);

void DynamicStringResize(DynamicString *ds, unsigned size);

void DynamicStringAddChar(DynamicString *ds, char c);

void DynamicStringAddString(DynamicString *ds, const char *string);

void DynamicStringReplaceChar(DynamicString *ds, char c, unsigned index);

void DynamicStringCopy(const DynamicString *ds, DynamicString *copy);

bool DynamicStringCompare(const DynamicString *ds, const char *string);

void DynamicStringPrint(const DynamicString *ds);

char *DynamicStringToCString(const DynamicString *ds);

#endif _DYNAMIC_STRING_H