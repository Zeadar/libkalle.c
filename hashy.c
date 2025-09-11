#include <stdlib.h>
#include <string.h>
#include "libmemhandle.h"
#define GROWSIZE 512

static uint64_t fnv1a(const char *str) {
    uint64_t hash = 14695981039346656037UL;
    while (*str) {
        hash ^= (uint8_t) * str++;
        hash *= 1099511628211UL;
    }
    return hash;
}

static void hashy_rehash(Map *map) {
    memset(map->map, ~0, map->size * sizeof(slice_index));

    for (slice_index i = 0; i != map->used; ++i) {
        uint64_t hash_index = fnv1a(sarray_get(&map->keys, i)) % map->size;

        while (map->map[hash_index] != -1)
            hash_index = (hash_index + 1) % map->size;

        map->map[hash_index] = i;
    }
}

static void hashy_check_grow(Map *map) {
    if (map->used < map->size / 2)
        return;

    map->size *= 2;
    slice_index *new_map = malloc(map->size * sizeof(slice_index));

    free(map->map);
    map->map = new_map;

    hashy_rehash(map);
}

Map hashy_create(size_t byte_width) {
    size_t init_size = GROWSIZE * sizeof(slice_index);
    slice_index slots = init_size / sizeof(slice_index);
    Map map = {
        .keys = sarray_create(),
        .value_data = slice_create(byte_width),
        .map = malloc(init_size),
        .size = slots,
        .used = 0,
    };

    memset(map.map, ~0, map.size * sizeof(slice_index));

    return map;
}

void hashy_destroy(Map *map) {
    sarray_destroy(&map->keys);
    slice_destroy(&map->value_data);
    free(map->map);
    map->map = 0;
    map->size = 0;
    map->used = 0;
}

void *hashy_get(const Map *map, const char *key) {
    uint64_t hash_index = fnv1a(key) % map->size;
    char *current_key;

    while (map->map[hash_index] != -1) {
        current_key = sarray_get(&map->keys, map->map[hash_index]);
        if (strcmp(key, current_key) == 0)
            return slice_get_ptr(&map->value_data, map->map[hash_index]);

        hash_index = (hash_index + 1) % map->size;
    }

    return 0;
}

void hashy_set(Map *map, const char *key, const void *data) {
    hashy_check_grow(map);

    uint64_t hash_index = fnv1a(key) % map->size;

    while (map->map[hash_index] != -1) {
        if (strcmp(key, sarray_get(&map->keys, map->map[hash_index])) == 0) {
            slice_replace(&map->value_data, map->map[hash_index], data);
            return;
        }

        hash_index = (hash_index + 1) % (map->size - 1);
    }

    sarray_push(&map->keys, key);
    slice_push(&map->value_data, data);

    map->map[hash_index] = map->used++;
}

void hashy_remove(Map *map, const char *key) {
    uint64_t hash_index = fnv1a(key) % (map->size - 1);

    while (map->map[hash_index] != -1) {
        if (strcmp(key, sarray_get(&map->keys, map->map[hash_index])) == 0) {
            slice_remove_serial(&map->value_data, map->map[hash_index]);
            sarray_remove(&map->keys, map->map[hash_index]);
            map->used -= 1;
            hashy_rehash(map);
            return;
        }

        hash_index = (hash_index + 1) % (map->size - 1);
    }
}

void hashy_foreach(const Map *map, void (fn) (char *key, void *value)) {
    char *key;
    void *value;

    for (slice_index i = 0; i != map->used; ++i) {
        key = sarray_get(&map->keys, i);
        value = slice_get_ptr(&map->value_data, i);
        fn(key, value);
    }
}

void hashy_foreach_value(const Map *map, void (fn) (void *value)) {
    for (slice_index i = 0; i != map->used; ++i) {
        fn(slice_get_ptr(&map->value_data, i));
    }
}

void hashy_foreach_key(const Map *map, void (*fn)(char *key)) {
    for (slice_index i = 0; i != map->used; ++i) {
        fn(sarray_get(&map->keys, i));
    }
}
