
#include "list.h"
#include "xmalloc.h"
#include "stack.h"

typedef struct {
    list_t list;
    void *data;
} stack_node_t;

struct stack_st {
    list_t list;
    size_t size;
};

stack_t *stack_new()
{
    stack_t *stack = xnew(stack_t);

    list_init(&stack->list);
    stack->size = 0;

    return stack;
}

void stack_push(stack_t *stack, void *data)
{
    stack_node_t *node = xnew(stack_node_t);
    list_init(&node->list);
    node->data = data;
    list_add_head(&stack->list, &node->list);
    stack->size++;
}

void *stack_pop(stack_t *stack)
{
    if (stack->size == 0)
        return NULL;

    stack_node_t *node = (stack_node_t *)list_pop_head(&stack->list);
    void *data = node->data;
    xfree(node);
    stack->size--;
    return data;
}

void *stack_top(stack_t *stack)
{
    if (stack->size == 0)
        return NULL;

    stack_node_t *node = (stack_node_t *)list_head(&stack->list);
    return node->data;
}

size_t stack_size(stack_t *stack)
{
    return stack->size;
}

bool stack_empty(stack_t *stack)
{
    return stack->size == 0;
}

void stack_clear(stack_t *stack)
{
	while (stack_pop(stack))
		;
}

void stack_free(stack_t *stack)
{
	stack_clear(stack);
    xfree(stack);
}