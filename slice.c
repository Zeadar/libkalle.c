#include <stdlib.h>
#include "libmemhandle.h"
#include "platform.h"

static size_t stepsize = 0;

Slice slice_create(size_t width) {
    if (!stepsize)
        stepsize = STEPSIZE;
    size_t init_size = stepsize + (width / stepsize) * stepsize;

    Slice slice = {
        .begin = malloc(init_size),
        .head = slice.begin,
        .end = slice.begin + init_size,
        .width = width,
    };

    return slice;
}

void slice_destroy(Slice *slice) {
    free(slice->begin);
    slice->begin = 0;
    slice->head = 0;
    slice->end = 0;
    slice->width = 0;
}

static void slice_check_grow(Slice *slice) {
    if (slice->head + slice->width < slice->end)
        return;

    size_t used_size = slice->head - slice->begin;
    size_t grow_size =
        slice->end - slice->begin + stepsize +
        (slice->width / stepsize) * stepsize;

    slice->begin = realloc(slice->begin, grow_size);
    slice->head = slice->begin + used_size;
    slice->end = slice->begin + grow_size;
}

int slice_ptr_in_bounds(const Slice *slice, void *ptr) {
    uint8_t *find_me = ptr;
    return find_me >= slice->begin && find_me < slice->head;
}

int slice_index_in_bounds(const Slice *slice, slice_index index) {
    return index < slice_size(slice) && index >= 0;
}

slice_index slice_find(const Slice *slice, void *ptr) {
    uint8_t *find_me = ptr;
    return (find_me - slice->begin) / slice->width;
}

void *slice_get_ptr(const Slice *slice, slice_index index) {
    return slice->begin + index * slice->width;
}

void *slice_allocate(Slice *slice) {
    slice_check_grow(slice);

    slice->head += slice->width;
    return slice->head - slice->width;
}

slice_index slice_push(Slice *slice, const void *data) {
    slice_check_grow(slice);

    memcpy(slice->head, data, slice->width);
    slice->head += slice->width;

    return (slice->head - slice->begin - slice->width) / slice->width;
}

void slice_remove_dirty(Slice *slice, slice_index index) {
    if (index != slice_size(slice) - 1) {
        uint8_t *last = slice->head - slice->width;
        uint8_t *here = slice->begin + index * slice->width;

        memcpy(here, last, slice->width);
    }

    slice->head = slice->head - slice->width;
}

void slice_remove_serial(Slice *slice, slice_index index) {
    slice_index last_index = slice_size(slice) - 1;

    if (index != last_index) {
        uint8_t *here = slice->begin + index * slice->width;
        uint8_t *ahead = here + slice->width;

        memmove(here, ahead, slice->head - ahead);
    }

    slice->head = slice->head - slice->width;
}

void slice_replace(Slice *slice, const slice_index index, const void *data) {
    memcpy(slice->begin + index * slice->width, data, slice->width);
}

void slice_foreach(Slice *slice, void (*fn)(void *)) {
    slice_index size = slice_size(slice);
    slice_index index;

    for (index = 0; index != size; ++index)
        fn(slice->begin + index * slice->width);
}

void slice_qsort(Slice *slice, int (*cmp)(const void *a, const void *b)) {
    qsort(slice->begin, slice_size(slice), slice->width, cmp);
}
