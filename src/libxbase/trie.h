/**
 * @file trie.h
 * @brief 基于trie树的字符串查找数据结构
 */

#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>

#define TRIE_NODE_TAKEN 0x1
#define TRIE_NODE_CONTINUE 0x1


/**
 * @brief trie树节点结构体
 */
typedef struct trie_node trie_node_t;

/**
 * @brief 遍历trie树节点时的回调函数类型
 * @param node 当前遍历到的trie树节点
 * @param arg 传递给回调函数的参数
 * @return TRIE_NODE_TAKEN 表示停止遍历，TRIE_NODE_CONTINUE表示继续遍历
 */
typedef int (*trie_node_visit_fn_t)(trie_node_t *node, void *arg);

/**
 * @brief 释放用户数据的函数类型
 * @param data 用户数据指针
 */
typedef void (*trie_node_free_fn_t)(void *data);

/**
 * @brief 创建一个新的trie树节点
 * @return 新创建的trie树节点指针
 */
trie_node_t *trie_node_new();

/**
 * @brief 删除一个trie树节点及其所有子节点
 * @param node 待删除的trie树节点指针
 */
void trie_node_delete(trie_node_t *node);

/**
 * @brief 在trie树中插入一个字符串
 * @param parent 父节点指针
 * @param prefix 待插入的字符串
 * @return 插入的trie树节点指针，如果字符串已存在则返回NULL
 */
trie_node_t *trie_node_insert(trie_node_t *parent, const char *prefix);

/**
 * @brief 在trie树中查找一个字符串
 * @param parent 父节点指针
 * @param prefix 待查找的字符串
 * @return 查找到的trie树节点指针，如果字符串不存在则返回NULL
 */
trie_node_t *trie_node_search(trie_node_t *parent, const char *prefix);

/**
 * @brief 在trie树中删除一个字符串
 * @param node 待删除的trie树节点指针
 * @param prefix 待删除的字符串
 */
void trie_node_remove(trie_node_t *node, const char *prefix);

/**
 * @brief 设置trie树节点的用户数据和释放函数
 * @param node 待设置的trie树节点指针
 * @param data 用户数据指针
 * @param fun 释放用户数据的函数指针
 */
void trie_node_set_data(trie_node_t *node, void *data, trie_node_free_fn_t fun);

/**
 * @brief 获取trie树节点的用户数据指针
 * @param node 待获取用户数据的trie树节点指针
 * @return 用户数据指针
 */
void *trie_node_get_data(trie_node_t *node);

/**
 * @brief 获取trie树节点的token字符串
 * @param node 待获取用户数据的trie树节点指针
 * @return token字符串
 */
const char *trie_node_get_token(trie_node_t *node);

/**
 * @brief 前序遍历trie树
 * @param node 待遍历的trie树节点指针
 * @param fun 遍历时的回调函数指针
 * @param arg 传递给回调函数的参数
 */
void trie_node_foreach(trie_node_t *node, trie_node_visit_fn_t fun, void *arg);

#endif /* TRIE_H */