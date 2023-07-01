/**
 * @file rbtree.h
 * @brief Definition of a red-black tree data structure
 */

#ifndef __RBTREE_H__
#define __RBTREE_H__

#include <stdint.h>

/**
 * @brief Structure representing a red-black tree
 */
struct rbtree_st;
typedef struct rbtree_st rbtree_st;

/**
 * @brief Function pointer type for comparing two keys
 * @param key1 Pointer to the first key
 * @param ksize1 Size of the first key
 * @param key2 Pointer to the second key
 * @param ksize2 Size of the second key
 * @return An integer less than, equal to, or greater than
 *         zero if the first key is found to be less than, equal to, or greater than the second key, respectively
 */
typedef int (*rbtree_cmp_func_t)(const void *key1, uint32_t ksize1,
                                 const void *key2, uint32_t ksize2);

/**
 * @brief Function pointer type for freeing data associated with a node
 * @param data Pointer to the data to be freed
 */
typedef void (*rbtree_data_free_func_t)(void *data);

/**
 * @brief Flag indicating that the red-black tree should use external memory for keys
 */
#define RFLAG_EXTERN_KEY 0x1

/**
 * @brief Create a new red-black tree
 * @param data_free Function pointer for freeing data associated with a node
 * @param cmp Function pointer for comparing two keys
 * @param flag Flag indicating whether to use external memory for keys
 * @return Pointer to the newly created red-black tree
 */
rbtree_st *rbtree_create(rbtree_data_free_func_t data_free, rbtree_cmp_func_t cmp, int flag);

/**
 * @brief Destroy a red-black tree and free all associated memory
 * @param rb Pointer to the red-black tree to be destroyed
 */
void rbtree_destroy(rbtree_st *rb);

/**
 * @brief Get the number of nodes in a red-black tree
 * @param rb Pointer to the red-black tree
 * @return The number of nodes in the tree
 */
uint32_t rbtree_count(rbtree_st *rb);

/**
 * @brief Structure representing a cursor for iterating over a red-black tree
 */
typedef struct rb_cursor_st {
    void *key;      /**< Pointer to the key */
    uint32_t ksize; /**< Size of the key */
    void *data;     /**< Pointer to the data */
} rb_cursor_st;

/**
 * @brief Get the cursor for the first node in a red-black tree
 * @param rb Pointer to the red-black tree
 * @return Pointer to the cursor for the first node, or NULL if the tree is empty
 */
const rb_cursor_st *rbtree_first(const rbtree_st *rb);

/**
 * @brief Get the cursor for the last node in a red-black tree
 * @param rb Pointer to the red-black tree
 * @return Pointer to the cursor for the last node, or NULL if the tree is empty
 */
const rb_cursor_st *rbtree_last(const rbtree_st *rb);

/**
 * @brief Get the cursor for the next node in a red-black tree
 * @param itor Pointer to the cursor for the current node
 * @return Pointer to the cursor for the next node, or NULL if there is no next node
 */
const rb_cursor_st *rbtree_next(const rb_cursor_st *itor);

/**
 * @brief Get the cursor for the previous node in a red-black tree
 * @param itor Pointer to the cursor for the current node
 * @return Pointer to the cursor for the previous node, or NULL if there is no previous node
 */
const rb_cursor_st *rbtree_prev(const rb_cursor_st *itor);

/**
 * @brief Search for a node with a given key in a red-black tree and return its value
 * @param rb Pointer to the red-black tree
 * @param key Pointer to the key to search for
 * @param ksize Size of the key
 * @param val Pointer to a variable to store the value of the node, or NULL if the value is not needed
 * @return 0 if the node is found, -1 otherwise
 */
int rbtree_search(const rbtree_st *rb, const void *key, uint32_t ksize, void **val);

/**
 * @brief Search for a node with a given key in a red-black tree and return its cursor
 * @param rb Pointer to the red-black tree
 * @param key Pointer to the key to search for
 * @param ksize Size of the key
 * @return Pointer to the cursor for the node, or NULL if the node is not found
 */
const rb_cursor_st *rbtree_get_cursor(const rbtree_st *rb, const void *key, uint32_t ksize);

/**
 * @brief Search for the first node in a red-black tree with a key greater than or equal to a given key
 * @param rb Pointer to the red-black tree
 * @param key Pointer to the key to search for
 * @param ksize Size of the key
 * @return Pointer to the cursor for the node, or NULL if the node is not found
 */
const rb_cursor_st *rbtree_lower_bound(const rbtree_st *rb, const void *key, uint32_t ksize);

/**
 * @brief Search for the first node in a red-black tree with a key less than or equal to a given key
 * @param rb Pointer to the red-black tree
 * @param key Pointer to the key to search for
 * @param ksize Size of the key
 * @return Pointer to the cursor for the node, or NULL if the node is not found
 */
const rb_cursor_st *rbtree_upper_bound(const rbtree_st *rb, const void *key, uint32_t ksize);

/**
 * @brief Insert a node with a given key and value into a red-black tree
 * @param rb Pointer to the red-black tree
 * @param key Pointer to the key to insert
 * @param ksize Size of the key
 * @param val Pointer to the value to insert
 */
void rbtree_insert(rbtree_st *rb, const void *key, uint32_t ksize, void *val);

/**
 * @brief Delete a node with a given key from a red-black tree
 * @param rb Pointer to the red-black tree
 * @param key Pointer to the key to delete
 * @param ksize Size of the key
 */
void rbtree_delete(rbtree_st *rb, const void *key, uint32_t ksize);

/**
 * @brief Function pointer type for iterating over a red-black tree
 * @param key Pointer to the key of the current node
 * @param ksize Size of the key
 * @param val Pointer to the value of the current node
 * @param userdata Pointer to user-defined data
 * @return A non-zero value to stop iterating and return that value from rbtree_foreach()
 */
typedef int (*rbtree_foreach_func_t)(const void *key, int ksize, void *val, void *userdata);

/**
 * @brief Iterate over a red-black tree and call a function for each node
 * @param rb Pointer to the red-black tree
 * @param userdata Pointer to user-defined data to pass to the function
 * @param fn Function to call for each node
 * @return 0 if the iteration completes successfully, or the non-zero value returned by the function if it stops the iteration early
 */
int rbtree_foreach(rbtree_st *rb, void *userdata, rbtree_foreach_func_t fn);

/**
 * @brief Get the size of a red-black tree in bytes, not including the memory used by keys and values
 * @param rb Pointer to the red-black tree
 * @return The size of the tree in bytes
 */
uint32_t rbtree_size(rbtree_st *rb);

#endif
