#include "trie.h"
#include "hash.h"
#include "xmalloc.h"
#include "xstring.h"
#include "stack.h"

#include <string.h>
#include <stddef.h>

#define INVID_PTR(p) (!(p) || (p) == (void *)0x1 || (void *)(p) == (void *)(unsigned long)~0)

struct trie_node {

    trie_node_t *parent;
    hash_table_t *childs;

    char *token;
    void *udata;

    trie_node_free_fn_t ufree;
};

trie_node_t *trie_node_new()
{
    trie_node_t *node = xnew0(trie_node_t);

    node->parent = NULL;
    node->childs = make_string_hash_table(0);

    node->token = NULL;
    node->udata = NULL;

    node->ufree = NULL;

    return node;
}

static void trie_node_free(trie_node_t *node)
{
    if (node->ufree)
        node->ufree(node->udata);

    hash_table_destroy(node->childs);
    xfree(node->token);
    xfree(node);
}

trie_node_t *trie_node_insert(trie_node_t *parent, const char *prefix)
{
    int found = 1;
    for (const char *pos = prefix + 1; !INVID_PTR(pos); pos = strchr(pos, '/') + 1) {

        char *token = strprefix(pos, "/");

        trie_node_t *child = hash_table_get(parent->childs, token);
        if (child == NULL) {
            found = 0;

            child = trie_node_new();
            child->parent = parent;
            child->token = token;

            hash_table_put(parent->childs, token, child);
        } else {
            xfree(token);
        }

        parent = child;
    }

    return found ? NULL : parent;
}

trie_node_t *trie_node_search(trie_node_t *parent, const char *prefix)
{
    trie_node_t *child = NULL;

    for (const char *pos = prefix + 1; !INVID_PTR(pos); pos = strchr(pos, '/') + 1) {
        char *token = strprefix(pos, "/");

        child = hash_table_get(parent->childs, token);
        if (child == NULL) {
            xfree(token);
            break;
        }

        parent = child;
        xfree(token);
    }

    return child;
}

bool trie_node_isleaf(trie_node_t *node)
{
    return hash_table_count(node->childs) == 0;
}

void trie_node_remove(trie_node_t *node, const char *prefix)
{
    node = trie_node_search(node, prefix);
    if (!node)
        return;

    stack_t *sk = stack_new();
    stack_push(sk, node);

    while (!stack_empty(sk)) {
        trie_node_t *top = stack_top(sk);

        hash_table_foreach(top->childs, it)
            stack_push(sk, it.val);

        top = stack_top(sk);
        while (top && trie_node_isleaf(top)) {

            hash_table_remove(top->parent->childs, top->token);
            trie_node_free(top);

            top = stack_pop(sk);
            top = stack_top(sk);
        }
    }

    stack_free(sk);
}

void trie_node_delete(trie_node_t *node)
{
    char prefix[128];
    hash_table_foreach(node->childs, it)
    {
        prefix[0] = '/';
        prefix[1] = '\0';
        strcat(prefix, it.key);

        trie_node_remove(node, prefix);
    }

    trie_node_free(node);
}

void trie_node_foreach(trie_node_t *node, trie_node_visit_fn_t fun, void *arg)
{
    stack_t *sk = stack_new();
    stack_push(sk, node);

    while (!stack_empty(sk)) {
        trie_node_t *top = stack_pop(sk);
        if (top->parent && fun(top, arg) == TRIE_NODE_TAKEN)
            break;

        hash_table_foreach(top->childs, it)
            stack_push(sk, it.val);
    }

    stack_free(sk);
}

void trie_node_set_data(trie_node_t *node, void *data, trie_node_free_fn_t fun)
{
    node->udata = data;
    node->ufree = fun;
}

void *trie_node_get_data(trie_node_t *node)
{
    return node->udata;
}

const char *trie_node_get_token(trie_node_t *node)
{
    return node->token;
}
