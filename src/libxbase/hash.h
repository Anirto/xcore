/* Hash table declarations.
   Copyright (C) 2000 Free Software Foundation, Inc.
 */

#ifndef HASH_H
#define HASH_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/**
 * @brief 哈希表结构体
 */
struct hash_table;

/**
 * @brief 哈希表类型
 */
typedef struct hash_table hash_table_t;

/**
 * @brief 哈希表迭代器结构体
 */
struct hash_iter {
    hash_table_t *ht; /**< 哈希表指针 */
    void *key;        /**< 键指针 */
    void *val;        /**< 值指针 */
    int cur;          /**< 当前位置 */
};

/**
 * @brief 哈希表迭代器类型
 */
typedef struct hash_iter ht_iter_t;

/**
 * @brief 创建一个新的哈希表
 * @param size 哈希表大小
 * @param hash_func 哈希函数
 * @param compare_func 比较函数
 * @return 哈希表指针
 */
hash_table_t *hash_table_new(int size, unsigned long (*hash_func)(const void *),
                             int (*compare_func)(const void *, const void *));

/**
 * @brief 销毁哈希表
 * @param ht 哈希表指针
 */
void hash_table_destroy(hash_table_t *ht);

/**
 * @brief 获取哈希表中指定键的值
 * @param ht 哈希表指针
 * @param key 键指针
 * @return 键对应的值指针，如果键不存在则返回 NULL
 */
void *hash_table_get(const hash_table_t *ht, const void *key);

/**
 * @brief 获取哈希表中指定键值对的键和值
 * @param ht 哈希表指针
 * @param key 键指针
 * @param key_out 键输出指针
 * @param val_out 值输出指针
 * @return 如果键存在，则将键和值存储在对应的指针中并返回 0，否则返回 -1
 */
int hash_table_get_pair(const hash_table_t *ht, const void *key, void *key_out, void *val_out);

/**
 * @brief 判断哈希表中是否包含指定键
 * @param ht 哈希表指针
 * @param key 键指针
 * @return 如果键存在则返回 1，否则返回 0
 */
int hash_table_contains(const hash_table_t *ht, const void *key);

/**
 * @brief 向哈希表中插入一个键值对
 * @param ht 哈希表指针
 * @param key 键指针
 * @param val 值指针
 */
void hash_table_put(hash_table_t *ht, const void *key, void *val);

/**
 * @brief 从哈希表中删除指定键的键值对
 * @param ht 哈希表指针
 * @param key 键指针
 * @return 如果键存在则删除并返回 0，否则返回 -1
 */
int hash_table_remove(hash_table_t *ht, const void *key);

/**
 * @brief 清空哈希表
 * @param ht 哈希表指针
 */
void hash_table_clear(hash_table_t *ht);

/**
 * @brief 遍历哈希表并对每个键值对执行指定的操作
 * @param ht 哈希表指针
 * @param func 操作函数指针
 * @param ctx 上下文指针
 */
void hash_table_map(hash_table_t *ht, int (*func)(void *, void *, void *), void *ctx);

/**
 * @brief 获取哈希表中键值对的数量
 * @param ht 哈希表指针
 * @return 键值对数量
 */
int hash_table_count(const hash_table_t *ht);

/**
 * @brief 创建一个字符串哈希表
 * @param size 哈希表大小
 * @return 哈希表指针
 */
hash_table_t *make_string_hash_table(int size);

/**
 * @brief 创建一个不区分大小写的字符串哈希表
 * @param size 哈希表大小
 * @return 哈希表指针
 */
hash_table_t *make_nocase_string_hash_table(int size);

/**
 * @brief 计算指针的哈希值
 * @param ptr 指针
 * @return 哈希值
 */
unsigned long hash_pointer(const void *ptr);

/**
 * @brief 获取哈希表中第一个键值对的迭代器
 * @param ht 哈希表指针
 * @return 哈希表迭代器
 */
ht_iter_t hash_table_first(hash_table_t *ht);

/**
 * @brief 获取哈希表中下一个键值对的迭代器
 * @param iter 哈希表迭代器
 * @return 下一个键值对的迭代器
 */
ht_iter_t hash_table_next(ht_iter_t iter);

/**
 * @brief 判断哈希表迭代器是否到达末尾
 * @param iter 哈希表迭代器
 * @return 如果到达末尾则返回 true，否则返回 false
 */
bool hash_table_end(ht_iter_t iter);

/**
 * @brief hash表遍历循环
 *
 * @param ht 哈希表指针
 * @param it 哈希表迭代器
 * @example
 *          hash_table_foreach(ht, iter) {
 *              printf("%s: %s\n", (char *)iter.key, (char *)iter.value);
 *          }
 *
 * @note 遍历时不要修改key和vvalue
 */
#define hash_table_foreach(ht, it)                                 \
    for (ht_iter_t it = hash_table_first(ht); !hash_table_end(it); \
         it = hash_table_next(it))

/* From XEmacs, and hence from Dragon book. */
#define GOOD_HASH 65599 /* prime number just over 2^16; Dragon book, p. 435 */
#define HASH2(a, b) (GOOD_HASH * (a) + (b))
#define HASH3(a, b, c) (GOOD_HASH * HASH2(a, b) + (c))
#define HASH4(a, b, c, d) (GOOD_HASH * HASH3(a, b, c) + (d))
#define HASH5(a, b, c, d, e) (GOOD_HASH * HASH4(a, b, c, d) + (e))
#define HASH6(a, b, c, d, e, f) (GOOD_HASH * HASH5(a, b, c, d, e) + (f))
#define HASH7(a, b, c, d, e, f, g) (GOOD_HASH * HASH6(a, b, c, d, e, f) + (g))
#define HASH8(a, b, c, d, e, f, g, h) (GOOD_HASH * HASH7(a, b, c, d, e, f, g) + (h))
#define HASH9(a, b, c, d, e, f, g, h, i) (GOOD_HASH * HASH8(a, b, c, d, e, f, g, h) + (i))

#ifdef __cplusplus
}
#endif
#endif /* HASH_H */
