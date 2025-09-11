#include "libmemhandle.h"
#include <string.h>

static Branch *new_branch(StrSet *strset, const char *str) {
    Branch *branch = slice_allocate(&strset->branches);
    slice_index index = sarray_push(&strset->strings, str);
    branch->str = sarray_get(&strset->strings, index);
    branch->left = 0;
    branch->right = 0;
    return branch;
}

static void insert(StrSet *strset, const char *str, Branch *branch) {
    int r = strcmp(branch->str, str);

    if (r < 0) {
        if (branch->left) {
            insert(strset, str, branch->left);
        } else {
            branch->left = new_branch(strset, str);
        }
    } else if (r > 0) {
        if (branch->right) {
            insert(strset, str, branch->right);
        } else {
            branch->right = new_branch(strset, str);
        }
    }
}

StrSet strset_create() {
    StrSet strset;
    strset.branches = slice_new(Branch);
    strset.strings = sarray_create();
    strset.root = 0;

    return strset;
}

void strset_destroy(StrSet *strset) {
    slice_destroy(&strset->branches);
    sarray_destroy(&strset->strings);
    strset->root = 0;
}

void strset_set(StrSet *strset, const char *str) {
    if (strset->root) {
        if (strcmp(strset->root->str, str) == 0)
            return;
        insert(strset, str, strset->root);
    } else {
        strset->root = new_branch(strset, str);
    }
}
