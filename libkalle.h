#pragma once

#include "types.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// Slice
typedef ptrdiff_t slice_index;
typedef struct {
    u8 *begin;
    u8 *head;
    u8 *end;
    size_t width;
} Slice;

#define slice_new(data_t) slice_create(sizeof(data_t))
#define slice_get(data_t, slice, index) (*(data_t *)slice_get_ptr(slice, index))
#define slice_size(slice) (((slice)->head - (slice)->begin) / (slice)->width)
#define slice_room(slice) (((slice)->end - (slice)->begin) / (slice)->width)
#define slice_get_index(slice, ptr) ((ptr - (slice)->begin) / (slice)->width)

Slice slice_create(size_t width);
void slice_destroy(Slice *slice);
s8 slice_ptr_in_bounds(const Slice *slice, void *ptr);
slice_index slice_find(const Slice *slice, void *ptr);
void *slice_get_ptr(const Slice *slice, slice_index index);
void *slice_allocate(Slice *slice);
slice_index slice_push(Slice *slice, const void *data);
void slice_remove_dirty(Slice *slice, slice_index index);
void slice_remove_serial(Slice *slice, slice_index index);
void slice_replace(Slice *slice, const slice_index index, const void *data);
void slice_foreach(Slice *slice, void (*fn)(void *));
void slice_qsort(Slice *slice, int (*cmp)(const void *a, const void *b));

// Sarray
typedef struct {
    char *ptr;
    size_t head;
    size_t end;
    Slice table;
} Sarray;
typedef size_t str_index;

Sarray sarray_create();
void sarray_destroy(Sarray *sarray);
char *sarray_get(const Sarray *sarray, slice_index index);
slice_index sarray_size(const Sarray *sarray);
slice_index sarray_push(Sarray *sarray, const char *str);
void sarray_remove(Sarray *sarray, slice_index index);

// Hashy
typedef struct {
    Sarray keys;
    Slice value_data;
    slice_index *map;
    slice_index size;
    slice_index used;
} Map;

#define hashy_new(data_t) hashy_create(sizeof(data_t))

Map hashy_create(size_t byte_width);
void hashy_destroy(Map *map);
void *hashy_get(const Map *map, const char *key);
void hashy_set(Map *map, const char *key, const void *data);
void hashy_remove(Map *map, const char *key);
void hashy_foreach(const Map *map, void (fn) (char *key, void *value));
void hashy_foreach_value(const Map *map, void (fn) (void *value));
