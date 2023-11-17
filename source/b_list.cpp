#include <stdlib.h>

#include "../include/b_list.h"

List_t ListCtor(void)
{
    List_t list = {};

    list.val  = DATA_MAX;
    list.prev = &list;
    list.next = &list;

    return list;
}

int ListDtor(List_t *list)
{
    LIST_VERIFICATION(list, EXIT_FAILURE);

    for(List_t *pos = list->next->next; pos != list; pos = pos->next) free(pos->prev);
    free(list->prev);

    list->val  = 0;
    list->prev = NULL;
    list->next = NULL;

    return EXIT_SUCCESS;
}


List_t *ListHead(List_t *list)
{
    LIST_VERIFICATION(list, NULL);

    return list->prev;
}

List_t *ListTail(List_t *list)
{
    LIST_VERIFICATION(list, NULL);

    return list->next;
}


List_t *ListAppend(struct List_t *list, List_t *prev_p, const data_t val)
{
    LIST_VERIFICATION(list, NULL);

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
    LIST_VERIFICATION(list, EXIT_FAILURE);

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
    LIST_VERIFICATION(list, NULL);

    for(List_t *pos = ListTail(list); pos != list; pos = pos->next)
    {
        if(pos->val == val) return pos;
    }

    return NULL;
}

List_t *GetPos(struct List_t *const list, const size_t ord_pos)
{
    LIST_VERIFICATION(list, NULL);

    List_t *pos = ListTail(list);

    for(size_t i = 0; i < ord_pos; i++)
    {
        pos = pos->next;

        ASSERT(pos != list, return NULL);
    }

    return pos;
}


static void ListIndexDump(List_t *list)
{
    fprintf(stderr, color_red(" %p \t"), list);

    for(List_t *p = list->next; p != list; p = p->next)
    {
        ASSERT(p, return);
             if(p == list->next) fprintf(stderr, color_blue  (" %p \t"), p);
        else if(p == list->prev) fprintf(stderr, color_purple(" %p \t"), p);
        else                     fprintf(stderr, color_green (" %p \t"), p);
    }
}

static void ListValDump(List_t *list)
{
    fprintf(stderr, "DATA:\t");
    fprintf(stderr, color_red("[" DATA_FORMAT "]\t"), list->val);

    for(List_t *p = list->next; p != list; p = p->next)
    {
        ASSERT(p, return);
             if(p == list->next) fprintf(stderr, color_blue  ("[" DATA_FORMAT "]\t"), p->val);
        else if(p == list->prev) fprintf(stderr, color_purple("[" DATA_FORMAT "]\t"), p->val);
        else                     fprintf(stderr, color_green ("[" DATA_FORMAT "]\t"), p->val);
    }
}

static void ListNextDump(List_t *list)
{
    fprintf(stderr, "NEXT:\t");
    fprintf(stderr, color_red("[%p]\t"), list->next);

    for(List_t *p = list->next; p != list; p = p->next)
    {
        ASSERT(p, return);
             if(p == list->next) fprintf(stderr, color_blue  ("[%p]\t"), p->next);
        else if(p == list->prev) fprintf(stderr, color_purple("[%p]\t"), p->next);
        else                     fprintf(stderr, color_green ("[%p]\t"), p->next);
    }
}

static void ListPrevDump(List_t *list)
{
    fprintf(stderr, "PREV:\t");
    fprintf(stderr, color_red("[%p]\t"), list->prev);

    for(List_t *p = list->next; p != list; p = p->next)
    {
        ASSERT(p, return);
             if(p == list->next) fprintf(stderr, color_blue  ("[%p]\t"), p->prev);
        else if(p == list->prev) fprintf(stderr, color_purple("[%p]\t"), p->prev);
        else                     fprintf(stderr, color_green ("[%p]\t"), p->prev);
    }
}

void ListDump(List_t *const list)
{
    ASSERT(list, return);

    fprintf(stderr, "LIST[%p]:\n\t", list);

    ListIndexDump(list);
    fprintf(stderr, "\n\n");

    ListValDump(list);
    fprintf(stderr, "\n\n");

    ListNextDump(list);
    fprintf(stderr, "\n\n");

    ListPrevDump(list);
    fprintf(stderr, "\n\n");
}

#ifdef PROTECT
bool IsListValid(List_t *const list)
{
    ASSERT(list                 , return false);
    ASSERT(list->val == DATA_MAX, return false);
    ASSERT(list->next           , return false);

    for(List_t *i = list->next; i != list; i = i->next)
    {
        ASSERT(i->next           , return false);
        ASSERT(i->next->prev == i, return false);
    }

    return true;
}

bool InList(List_t *const list, List_t *const elem)
{
    LIST_VERIFICATION(list, false);

    ASSERT(elem, return false);

    if(list == elem) return true;

    for(List_t *i = ListTail(list); i != list; i = i->next)
    {
        if(i == elem) return true;
    }

    return false;
}
#endif