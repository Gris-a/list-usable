#include <stdlib.h>
#include <stdbool.h>

#include "../include/b_list.h"

List_t ListCtor(void)
{
    List_t list = {};

    list.val  = 0;
    list.prev = &list;
    list.next = &list;

    return list;
}

int ListDtor(List_t *list)
{
    LIST_VER(list, EXIT_FAILURE);

    for(List_t *pos = list->next->next; pos != list; pos = pos->next) free(pos->prev);
    free(list->prev);

    list->prev = NULL;
    list->next = NULL;

    return EXIT_SUCCESS;
}

List_t *ListAppend(struct List_t *list, List_t *prev_p, const data_t val)
{
    LIST_VER(list, NULL);

    ASSERT(InList(list, prev_p), return NULL);

    List_t *elem = (List_t *)calloc(1, sizeof(List_t));
    ASSERT(elem, return NULL);

    elem->val = val;

    elem->next         = prev_p->next;
    prev_p->next->prev = elem;

    elem->prev   = prev_p;
    prev_p->next = elem;

    return elem;
}

int ListDelete(List_t *list, List_t *del_p, data_t *val)
{
    LIST_VER(list, EXIT_FAILURE);

    ASSERT(InList(list, del_p), return EXIT_FAILURE);

    if(del_p == list) return ListDtor(list);

    if(val) *val = del_p->val;

    del_p->prev->next = del_p->next;
    del_p->next->prev = del_p->prev;

    free(del_p);

    return EXIT_SUCCESS;
}

List_t *ListSearch(List_t *const list, const data_t val)
{
    LIST_VER(list, NULL);

    for(List_t *pos = list->next; pos != list; pos = pos->next)
    {
        if(pos->val == val) return pos;
    }

    return NULL;
}

List_t *GetPos(struct List_t *const list, const size_t ord_pos)
{
    LIST_VER(list, NULL);

    List_t *pos = list->next;

    for(size_t i = 0; i < ord_pos; i++)
    {
        pos = pos->next;

        ASSERT(pos != list, return NULL);
    }

    return pos;
}

void ListDump(List_t *const list)
{
    ASSERT(list, return);

    fprintf(stderr, "LIST[%p]:\n", list);

    fprintf(stderr, "\t\040\033[91m%p\033[0m \t", list);
    for(List_t *i = list->next; i != list; i = i->next)
    {
        ASSERT(i, return);
             if(i == list->next) fprintf(stderr, "\040\033[94m%p\033[0m\040\t", i);
        else if(i == list->prev) fprintf(stderr, "\040\033[95m%p\033[0m\040\t", i);
        else                     fprintf(stderr, "\040\033[92m%p\033[0m\040\t", i);
    }
    fprintf(stderr, "\n\n");

    fprintf(stderr, "DATA:\t\033[91m[" DTS "]\033[0m\t", list->val);
    for(List_t *i = list->next; i != list; i = i->next)
    {
             if(i == list->next) fprintf(stderr, "\033[94m[" DTS "]\033[0m\t", i->val);
        else if(i == list->prev) fprintf(stderr, "\033[95m[" DTS "]\033[0m\t", i->val);
        else                     fprintf(stderr, "\033[92m[" DTS "]\033[0m\t", i->val);
    }
    fprintf(stderr, "\n\n");

    fprintf(stderr, "NEXT:\t\033[91m[%4p]\033[0m\t", list->next);
    for(List_t *i = list->next; i != list; i = i->next)
    {
             if(i == list->next) fprintf(stderr, "\033[94m[%p]\033[0m\t", i->next);
        else if(i == list->prev) fprintf(stderr, "\033[95m[%p]\033[0m\t", i->next);
        else                     fprintf(stderr, "\033[92m[%p]\033[0m\t", i->next);
    }
    fprintf(stderr, "\n\n");

    fprintf(stderr, "PREV:\t\033[91m[%p]\033[0m\t", list->prev);
    for(List_t *i = list->next; i != list; i = i->next)
    {
             if(i == list->next) fprintf(stderr, "\033[94m[%p]\033[0m\t", i->prev);
        else if(i == list->prev) fprintf(stderr, "\033[95m[%p]\033[0m\t", i->prev);
        else                     fprintf(stderr, "\033[92m[%p]\033[0m\t", i->prev);
    }
    fprintf(stderr, "\n\n");
}

#ifdef PROTECT
int ListVer(List_t *const list)
{
    ASSERT(list      , return EXIT_FAILURE);
    ASSERT(list->next, return EXIT_FAILURE);

    for(List_t *i = list->next; i != list; i = i->next)
    {
        ASSERT(i->next           , return EXIT_FAILURE);
        ASSERT(i->next->prev == i, return EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

bool InList(List_t *const list, List_t *const elem)
{
    LIST_VER(list, false);

    ASSERT(elem, return false);

    if(list == elem) return true;
    for(List_t *i = list->next; i != list; i = i->next)
        if(i == elem) return true;
    return false;
}
#endif