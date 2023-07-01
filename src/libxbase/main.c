#if 0

#include "trie.h"
#include <stdio.h>

int each_trie_node(trie_node_t *node, void *data)
{
    printf("%s\n", trie_node_get_token(node));
    return 0;
}

int main()
{
    while (1) {
        trie_node_t *root = trie_node_new();

        trie_node_t *node = trie_node_insert(root, "/aaaa/bbbb/cccc");

        node = trie_node_insert(root, "/aaaa/bbbb/ddddd");

        node = trie_node_insert(root, "/aaaa/bbbb/eeeee/1");
        node = trie_node_insert(root, "/aaaa/bbbb/eeeee/2");
        node = trie_node_insert(root, "/aaaa/bbbb/eeeee/3");
        node = trie_node_insert(root, "/aaaa/bbbb/eeeee/4");
        node = trie_node_insert(root, "/aaaa/bbbb/eeeee/5");
        node = trie_node_insert(root, "/aaaa/bbbb/eeeee/5/1");
        node = trie_node_insert(root, "/aaaa/bbbb/eeeee/5/2");

        node = trie_node_search(root, "/aaaa/bbbb/eeeee");
        node = trie_node_search(root, "/aaaa");

        // trie_node_remove(root, "/aaaa/bbbb/eeeee");
        // trie_node_remove(root, "/aaaa");

        printf("\n\n\n");
        trie_node_foreach(root, each_trie_node, NULL);

        // printf("%s\n", node->token);

        node = trie_node_search(root, "/aaaa/bbbb/cccc/dddd");

        trie_node_delete(root);
    }

    return 0;
}

#endif