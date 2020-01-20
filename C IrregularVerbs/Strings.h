//
// Created by Martin on 16/01/2020.
//

#ifndef IRREGULARVERBS_MULTISTRINGS_H
#define IRREGULARVERBS_MULTISTRINGS_H

#include <stdbool.h>
#include <stdlib.h>

typedef const char* STRING;

typedef struct MultiStrings_s {
    size_t length;
    STRING* array;
} MultiStrings;

MultiStrings* makeMultiStrings(STRING input);
MultiStrings* copyMultiStrings(const MultiStrings*);
void freeMultiStrings(MultiStrings*);

bool isSubstringOfString(STRING potential, STRING toCompareTo);
bool isSubstringOfMultiStrings(STRING potential, const MultiStrings* toCompareTo);
bool matchesString(STRING potential, STRING toCompareTo);
bool matchesMultiStrings(STRING potential, const MultiStrings* toCompareTo);

STRING makeStringFromMultiStrings(MultiStrings *input);
void freeStringFromMultiStrings(STRING);

#endif //IRREGULARVERBS_MULTISTRINGS_H