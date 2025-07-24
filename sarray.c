#pragma once

#include "libkalle.h"

//4096: assumed page size
#define STEPSIZE 4096
#define PADSIZE sizeof(size_t)

static size_t padsize(size_t bytesize) {
    return (bytesize + PADSIZE - 1) & ~(PADSIZE - 1);
}

static void sarray_check_grow(Sarray *sarray, size_t bytes) {
    if (bytes + sarray->head <= sarray->end)
        return;

    sarray->end += STEPSIZE + (bytes / STEPSIZE) * STEPSIZE;
    sarray->ptr = realloc(sarray->ptr, sarray->end);
}

Sarray sarray_create() {
    Sarray sarray = {
        .ptr = malloc(STEPSIZE),
        .head = 0,
        .end = STEPSIZE,
        .table = slice_new(str_index),
    };

    return sarray;
}

void sarray_destroy(Sarray *sarray) {
    free(sarray->ptr);
    sarray->ptr = 0;
    sarray->head = 0;
    sarray->end = 0;
    slice_destroy(&sarray->table);
}

char *sarray_get(const Sarray *sarray, slice_index index) {
    return sarray->ptr + slice_get(str_index, &sarray->table, index);
}

slice_index sarray_size(const Sarray *sarray) {
    return slice_size(&sarray->table);
}

slice_index sarray_push(Sarray *sarray, const char *str) {
    size_t bytes = strlen(str) + 1;

    sarray_check_grow(sarray, padsize(bytes));

    char *head = sarray->ptr + sarray->head;
    memcpy(head, str, bytes);

    slice_index index = slice_push(&sarray->table, &sarray->head);
    sarray->head += padsize(bytes);

    return index;
}

void sarray_remove(Sarray *sarray, slice_index index) {
    char *culprit = sarray_get(sarray, index);
    size_t bytes = padsize(strlen(culprit) + 1);

    u64 offset = culprit - sarray->ptr + bytes;
    memmove(culprit, culprit + bytes, sarray->head - offset);
    sarray->head -= bytes;

    slice_remove_serial(&sarray->table, index);

    while (index != slice_size(&sarray->table)) {
        str_index *si = slice_get_ptr(&sarray->table, index++);
        *si -= bytes;
    }
}
