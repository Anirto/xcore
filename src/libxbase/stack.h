#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include <stdlib.h>

/**
 * 栈结构体
 */
typedef struct stack_st stack_t;

/**
 * 创建一个新的栈
 *
 * @param fun 释放函数指针
 * @return 新的栈
 */
stack_t *stack_new();

/**
 * 将一个元素压入栈中
 *
 * @param stack 栈
 * @param data 数据指针
 */
void stack_push(stack_t *stack, void *data);

/**
 * 从栈中弹出一个元素，并返回其值
 *
 * @param stack 栈
 * @return 栈顶元素的值
 */
void *stack_pop(stack_t *stack);

/**
 * 返回栈顶元素的值
 *
 * @param stack 栈
 * @return 栈顶元素的值
 */
void *stack_top(stack_t *stack);

/**
 * 返回栈的大小
 *
 * @param stack 栈
 * @return 栈的大小
 */
size_t stack_size(stack_t *stack);

/**
 * 栈是否为空
 *
 * @param stack 栈
 * @return bool
 */
bool stack_empty(stack_t *stack);

/**
 * 清除栈
 *
 * @param stack 栈
 */
void stack_clear(stack_t * stack);

/**
 * 释放整个栈的内存
 *
 * @param stack 栈
 */
void stack_free(stack_t *stack);

#endif /* STACK_H */
