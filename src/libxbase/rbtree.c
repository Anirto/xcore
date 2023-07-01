/*
 * rbtree - Implements a red-black tree with parent pointers.
 *
 * Copyright (C) 2010 Franck Bui-Huu <fbuihuu@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2 of the
 * License.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

/*
 * For recall a red-black tree has the following properties:
 *
 *     1. All nodes are either BLACK or RED
 *     2. Leafs are BLACK
 *     3. A RED node has BLACK children only
 *     4. Path from a node to any leafs has the same number of BLACK nodes.
 *
 */

#define HEMAPI_IMPLEMENT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "xmalloc.h"
#include "rbtree.h"

typedef enum rb_color_t {
    RB_BLACK,
    RB_RED,
} rb_color_t;

typedef struct rbtree_node_st {
    rb_cursor_st cs;
    struct rbtree_node_st *left, *right;
    struct rbtree_node_st *parent;
    rb_color_t color;
} rbtree_node_st;

struct rbtree_st {
    rbtree_node_st *root;
    rbtree_node_st *first, *last;
    uint32_t nelem;
    int flag;
    rbtree_cmp_func_t cmp_fn;
    rbtree_data_free_func_t data_free;
};

#define get_color(node) ((node)->color)
#define set_color(col, node) ((node)->color = (col))
#define get_parent(node) ((node)->parent)
#define set_parent(par, node) ((node)->parent = (par))
#define is_root(node) (get_parent(node) == NULL)
#define is_black(node) (get_color(node) == RB_BLACK)
#define is_red(node) (get_color(node) != RB_BLACK)

#ifdef _MSC_VER
#define inline __inline
#endif

static inline rbtree_node_st *get_first(rbtree_node_st *node)
{
    while (node->left)
        node = node->left;
    return node;
}

static inline rbtree_node_st *get_last(rbtree_node_st *node)
{
    while (node->right)
        node = node->right;
    return node;
}

const rb_cursor_st *rbtree_first(const rbtree_st *tree)
{
    return (const rb_cursor_st *)tree->first;
}

const rb_cursor_st *rbtree_last(const rbtree_st *tree)
{
    return (const rb_cursor_st *)tree->last;
}

static rbtree_node_st *rb_next(rbtree_node_st *node)
{
    rbtree_node_st *parent;

    if (node->right)
        return get_first(node->right);

    while ((parent = get_parent(node)) && parent->right == node)
        node = parent;

    return parent;
}

const rb_cursor_st *rbtree_next(const rb_cursor_st *_itor)
{
    return (const rb_cursor_st *)rb_next((rbtree_node_st *)_itor);
}

static rbtree_node_st *rb_prev(const rbtree_node_st *node)
{
    rbtree_node_st *parent;

    if (node->left)
        return get_last(node->left);

    while ((parent = get_parent(node)) && parent->left == node)
        node = parent;

    return parent;
}

const rb_cursor_st *rbtree_prev(const rb_cursor_st *_itor)
{
    return (const rb_cursor_st *)rb_prev((rbtree_node_st *)_itor);
}

/*
 * 'pparent' and 'is_left' are only used for insertions. Normally GCC
 * will notice this and get rid of them for lookups.
 */
static inline rbtree_node_st *do_lookup(const rbtree_st *tree,
                                        const void *key,
                                        uint32_t ksize,
                                        rbtree_node_st **pparent,
                                        int *is_left)
{
    rbtree_node_st *node = tree->root;

    *pparent = NULL;
    *is_left = 0;

    while (node) {
        int res = tree->cmp_fn(node->cs.key, node->cs.ksize, key, ksize);
        if (res == 0)
            return node;

        *pparent = node;
        if ((*is_left = (res > 0)))
            node = node->left;
        else
            node = node->right;
    }
    return NULL;
}

/*
 * Rotate operations (They preserve the binary search tree property,
 * assuming that the keys are unique).
 */
static void rotate_left(rbtree_st *tree, rbtree_node_st *node)
{
    rbtree_node_st *p = node;
    rbtree_node_st *q = node->right; /* can't be NULL */
    rbtree_node_st *parent = get_parent(p);

    if (!is_root(p)) {
        if (parent->left == p)
            parent->left = q;
        else
            parent->right = q;
    } else
        tree->root = q;
    set_parent(parent, q);
    set_parent(q, p);

    p->right = q->left;
    if (p->right)
        set_parent(p, p->right);
    q->left = p;
}

static void rotate_right(rbtree_st *tree, rbtree_node_st *node)
{
    rbtree_node_st *p = node;
    rbtree_node_st *q = node->left; /* can't be NULL */
    rbtree_node_st *parent = get_parent(p);

    if (!is_root(p)) {
        if (parent->left == p)
            parent->left = q;
        else
            parent->right = q;
    } else
        tree->root = q;
    set_parent(parent, q);
    set_parent(q, p);

    p->left = q->right;
    if (p->left)
        set_parent(p, p->left);
    q->right = p;
}

int rbtree_search(const rbtree_st *tree, const void *key, uint32_t ksize, void **val)
{
    rbtree_node_st *parent;
    int is_left;
    rbtree_node_st *nd = do_lookup(tree, key, ksize, &parent, &is_left);

    if (!nd)
        return -1;
    if (val)
        *val = nd->cs.data;
    return 0;
}

const rb_cursor_st *rbtree_get_cursor(const rbtree_st *tree, const void *key, uint32_t ksize)
{
    rbtree_node_st *parent;
    int is_left;

    return (const rb_cursor_st *)do_lookup(tree, key, ksize, &parent, &is_left);
}

const rb_cursor_st *rbtree_lower_bound(const rbtree_st *tree, const void *key, uint32_t ksize)
{
    rbtree_node_st *parent;
    int is_left;
    rbtree_node_st *ret = do_lookup(tree, key, ksize, &parent, &is_left);
    if (ret)
        return (const rb_cursor_st *)ret;

    while (parent && tree->cmp_fn(parent->cs.key, parent->cs.ksize, key, ksize) < 0) {
        parent = rb_next(parent);
    }

    return (const rb_cursor_st *)parent;
}

const rb_cursor_st *rbtree_upper_bound(const rbtree_st *tree, const void *key, uint32_t ksize)
{
    rbtree_node_st *parent;
    int is_left;
    rbtree_node_st *ret = do_lookup(tree, key, ksize, &parent, &is_left);
    if (ret)
        return (const rb_cursor_st *)ret;

    while (parent && tree->cmp_fn(parent->cs.key, parent->cs.ksize, key, ksize) > 0) {
        parent = rb_prev(parent);
    }

    return (const rb_cursor_st *)parent;
}

static void set_child(rbtree_node_st *child, rbtree_node_st *node, int left)
{
    if (left)
        node->left = child;
    else
        node->right = child;
}

void rbtree_insert(rbtree_st *tree, const void *key, uint32_t ksize, void *val)
{
    rbtree_node_st *parent;
    int is_left;
    rbtree_node_st *node = do_lookup(tree, key, ksize, &parent, &is_left);

    if (node) {
        if ((void *)tree->data_free)
            tree->data_free(node->cs.data);

        if (tree->flag & RFLAG_EXTERN_KEY) {
            node->cs.key = (void *)key;
            node->cs.ksize = ksize;
        }
        node->cs.data = val;
        return;
    }

    node = xmalloc0(sizeof(rbtree_node_st));

    /* copy key */
    if (tree->flag & RFLAG_EXTERN_KEY) {
        node->cs.key = (void *)key;
    } else {
        node->cs.key = xmalloc(ksize);

        memcpy(node->cs.key, key, ksize);
    }

    node->cs.ksize = ksize;
    node->cs.data = val;

    node->left = NULL;
    node->right = NULL;
    set_color(RB_RED, node);
    set_parent(parent, node);

    if (parent) {
        if (is_left) {
            if (parent == tree->first)
                tree->first = node;
        } else {
            if (parent == tree->last)
                tree->last = node;
        }
        set_child(node, parent, is_left);
    } else {
        tree->root = node;
        tree->first = node;
        tree->last = node;
    }

    /*
     * Fixup the modified tree by recoloring nodes and performing
     * rotations (2 at most) hence the red-black tree properties are
     * preserved.
     */
    while ((parent = get_parent(node)) && is_red(parent)) {
        rbtree_node_st *grandpa = get_parent(parent);

        if (parent == grandpa->left) {
            rbtree_node_st *uncle = grandpa->right;

            if (uncle && is_red(uncle)) {
                set_color(RB_BLACK, parent);
                set_color(RB_BLACK, uncle);
                set_color(RB_RED, grandpa);
                node = grandpa;
            } else {
                if (node == parent->right) {
                    rotate_left(tree, parent);
                    node = parent;
                    parent = get_parent(node);
                }
                set_color(RB_BLACK, parent);
                set_color(RB_RED, grandpa);
                rotate_right(tree, grandpa);
            }
        } else {
            rbtree_node_st *uncle = grandpa->left;

            if (uncle && is_red(uncle)) {
                set_color(RB_BLACK, parent);
                set_color(RB_BLACK, uncle);
                set_color(RB_RED, grandpa);
                node = grandpa;
            } else {
                if (node == parent->left) {
                    rotate_right(tree, parent);
                    node = parent;
                    parent = get_parent(node);
                }
                set_color(RB_BLACK, parent);
                set_color(RB_RED, grandpa);
                rotate_left(tree, grandpa);
            }
        }
    }
    set_color(RB_BLACK, tree->root);
    tree->nelem++;
}

void rbtree_delete(rbtree_st *tree, const void *key, uint32_t ksize)
{
    rbtree_node_st *parent, *left, *right, *next;
    rb_color_t color;
    int is_left;
    rbtree_node_st *oldnode = do_lookup(tree, key, ksize, &parent, &is_left);
    rbtree_node_st *node;

    if (!oldnode)
        return;

    node = oldnode;
    left = node->left;
    right = node->right;

    if (node == tree->first)
        tree->first = rb_next(node);
    if (node == tree->last)
        tree->last = rb_prev(node);

    if (!left)
        next = right;
    else if (!right)
        next = left;
    else
        next = get_first(right);

    if (parent)
        set_child(next, parent, parent->left == node);
    else
        tree->root = next;

    if (left && right) {
        color = get_color(next);
        set_color(get_color(node), next);

        next->left = left;
        set_parent(next, left);

        if (next != right) {
            parent = get_parent(next);
            set_parent(get_parent(node), next);

            node = next->right;
            parent->left = node;

            next->right = right;
            set_parent(next, right);
        } else {
            set_parent(parent, next);
            parent = next;
            node = next->right;
        }
    } else {
        color = get_color(node);
        node = next;
    }
    /*
     * 'node' is now the sole successor's child and 'parent' its
     * new parent (since the successor can have been moved).
     */
    if (node)
        set_parent(parent, node);

    /*
     * The 'easy' cases.
     */
    if (color == RB_RED)
        goto end_delete;
    if (node && is_red(node)) {
        set_color(RB_BLACK, node);
        goto end_delete;
    }

    do {
        if (node == tree->root)
            break;

        if (node == parent->left) {
            rbtree_node_st *sibling = parent->right;

            if (is_red(sibling)) {
                set_color(RB_BLACK, sibling);
                set_color(RB_RED, parent);
                rotate_left(tree, parent);
                sibling = parent->right;
            }
            if ((!sibling->left || is_black(sibling->left)) &&
                (!sibling->right || is_black(sibling->right))) {
                set_color(RB_RED, sibling);
                node = parent;
                parent = get_parent(parent);
                continue;
            }
            if (!sibling->right || is_black(sibling->right)) {
                set_color(RB_BLACK, sibling->left);
                set_color(RB_RED, sibling);
                rotate_right(tree, sibling);
                sibling = parent->right;
            }
            set_color(get_color(parent), sibling);
            set_color(RB_BLACK, parent);
            set_color(RB_BLACK, sibling->right);
            rotate_left(tree, parent);
            node = tree->root;
            break;
        } else {
            rbtree_node_st *sibling = parent->left;

            if (is_red(sibling)) {
                set_color(RB_BLACK, sibling);
                set_color(RB_RED, parent);
                rotate_right(tree, parent);
                sibling = parent->left;
            }
            if ((!sibling->left || is_black(sibling->left)) &&
                (!sibling->right || is_black(sibling->right))) {
                set_color(RB_RED, sibling);
                node = parent;
                parent = get_parent(parent);
                continue;
            }
            if (!sibling->left || is_black(sibling->left)) {
                set_color(RB_BLACK, sibling->right);
                set_color(RB_RED, sibling);
                rotate_left(tree, sibling);
                sibling = parent->left;
            }
            set_color(get_color(parent), sibling);
            set_color(RB_BLACK, parent);
            set_color(RB_BLACK, sibling->left);
            rotate_right(tree, parent);
            node = tree->root;
            break;
        }
    } while (is_black(node));

    if (node)
        set_color(RB_BLACK, node);

end_delete:
    if ((void *)tree->data_free)
        tree->data_free(oldnode->cs.data);

    if (!(tree->flag & RFLAG_EXTERN_KEY))
        free(oldnode->cs.key);

    free(oldnode);
    tree->nelem--;
}

static int default_cmp(const void *key1, uint32_t ksize1,
                       const void *key2, uint32_t ksize2)
{
    uint32_t kmin = ksize1 < ksize2 ? ksize1 : ksize2;
    int res;

    if ((res = memcmp(key1, key2, kmin)))
        return res;

    return ((int)ksize1 - (int)ksize2);
}

rbtree_st *rbtree_create(rbtree_data_free_func_t data_free, rbtree_cmp_func_t cmp, int flag)
{
    rbtree_st *rb = xmalloc0(sizeof(rbtree_st));
    rb->flag = flag;
    rb->data_free = data_free;
    rb->cmp_fn = cmp ? cmp : default_cmp;
    return rb;
}

static void node_destroy(rbtree_st *tree, rbtree_node_st *node)
{
    if (node->left)
        node_destroy(tree, node->left);
    if (node->right)
        node_destroy(tree, node->right);

    if (!(tree->flag & RFLAG_EXTERN_KEY))
        free(node->cs.key);

    if ((void *)tree->data_free)
        tree->data_free(node->cs.data);

    free(node);
}

void rbtree_destroy(rbtree_st *tree)
{
    if (!tree)
        return;

    if (tree->root)
        node_destroy(tree, tree->root);

    free(tree);
}

uint32_t rbtree_count(rbtree_st *rb)
{
    return rb->nelem;
}

int rbtree_foreach(rbtree_st *rb, void *data, rbtree_foreach_func_t fn)
{
    const rb_cursor_st *cs;
    int res;

    if (!(void *)fn)
        return 0;

    for (cs = rbtree_first(rb); cs; cs = rbtree_next(cs)) {
        if ((res = fn(cs->key, cs->ksize, cs->data, data)))
            return res;
    }

    return 0;
}

uint32_t rbtree_size(rbtree_st *rb)
{
    return rb->nelem * sizeof(rbtree_node_st);
}
