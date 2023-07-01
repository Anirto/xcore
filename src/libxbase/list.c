/*
 * BSD-3-Clause
 *
 * Copyright 2021 Ozan Tezcan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "list.h"

void list_init(list_t *l)
{
    l->next = l;
    l->prev = l;
}

void list_clear(list_t *l)
{
    list_t *tmp, *elem;

    list_foreach_safe(l, tmp, elem)
    {
        list_del(NULL, elem);
    }
}

bool list_is_empty(list_t *l)
{
    return l->next == l;
}

size_t list_count(list_t *l)
{
    size_t count = 0;
    list_t *elem;

    list_foreach(l, elem)
    {
        count++;
    }

    return count;
}

list_t *list_head(list_t *l)
{
    return l->next != l ? l->next : NULL;
}

list_t *list_tail(list_t *l)
{
    return l->prev != l ? l->prev : NULL;
}

void list_add_tail(list_t *l, list_t *elem)
{
    list_t *prev;

    // Delete if exists to prevent adding same item twice
    list_del(l, elem);

    prev = l->prev;
    l->prev = elem;
    elem->next = l;
    elem->prev = prev;
    prev->next = elem;
}

list_t *list_pop_tail(list_t *l)
{
    list_t *tail = l->prev;

    if (list_is_empty(l)) {
        return NULL;
    }

    list_del(l, l->prev);

    return tail;
}

void list_add_head(list_t *l, list_t *elem)
{
    list_t *next;

    // Delete if exists to prevent adding same item twice
    list_del(l, elem);

    next = l->next;
    l->next = elem;
    elem->prev = l;
    elem->next = next;
    next->prev = elem;
}

list_t *list_pop_head(list_t *l)
{
    list_t *head = l->next;

    if (list_is_empty(l)) {
        return NULL;
    }

    list_del(l, l->next);

    return head;
}

void list_add_after(list_t *l, list_t *prev,
                    list_t *elem)
{
    (void)l;
    list_t *next;

    // Delete if exists to prevent adding same item twice
    list_del(l, elem);

    next = prev->next;
    prev->next = elem;
    elem->next = next;
    elem->prev = prev;
    next->prev = elem;
}

void list_add_before(list_t *l, list_t *next,
                     list_t *elem)
{
    (void)l;
    list_t *prev;

    // Delete if exists to prevent adding same item twice
    list_del(l, elem);

    prev = next->prev;
    next->prev = elem;
    elem->next = next;
    elem->prev = prev;
    prev->next = elem;
}

void list_del(list_t *l, list_t *elem)
{
    (void)(l);

    elem->prev->next = elem->next;
    elem->next->prev = elem->prev;
    elem->next = elem;
    elem->prev = elem;
}