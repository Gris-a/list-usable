#include <stdlib.h>

#include "../include/basic_list.h"

List ListCtor(void)
{
    List list = {};

    list.data = (Block *)calloc(1, sizeof(Block));

    ASSERT(list.data, return {});

    list.data->val  = DATA_MAX;
    list.data->prev = list.data;
    list.data->next = list.data;

    return list;
}

int ListDtor(List *list)
{
    LIST_VERIFICATION(list, EXIT_FAILURE);

    Block *head = ListHead(list);

    for(Block *pos = ListTail(list); pos != list->data; pos = pos->next) free(pos->prev);
    free(head);

    list->data = NULL;
    list->size = 0;

    return EXIT_SUCCESS;
}


Block *ListHead(List *list)
{
    LIST_VERIFICATION(list, NULL);

    return list->data->prev;
}

Block *ListTail(List *list)
{
    LIST_VERIFICATION(list, NULL);

    return list->data->next;
}


Block *ListAppend(struct List *list, Block *prev_p, const data_t val)
{
    LIST_VERIFICATION(list, NULL);

    ASSERT(InList(list, prev_p), return NULL);

    Block *append = (Block *)calloc(1, sizeof(Block));

    ASSERT(append, return NULL);

    append->val = val;

    append->next       = prev_p->next;
    prev_p->next->prev = append;

    append->prev   = prev_p;
    prev_p->next = append;

    list->size++;

    return append;
}


int ListDelete(List *list, Block *del_p, data_t *val)
{
    LIST_VERIFICATION(list, EXIT_FAILURE);

    ASSERT(InList(list, del_p), return EXIT_FAILURE);

    if(del_p == list->data) return ListDtor(list);

    if(val) *val = del_p->val;

    del_p->prev->next = del_p->next;
    del_p->next->prev = del_p->prev;

    free(del_p);

    list->size--;

    return EXIT_SUCCESS;
}


bool InList(List *const list, Block *const elem)
{
    LIST_VERIFICATION(list, false);

    ASSERT(elem, return false);

    if(elem == list->data) return true;

    for(Block *i = ListTail(list); i != list->data; i = i->next)
    {
        if(i == elem) return true;
    }

    return false;
}

Block *ListSearch(List *const list, const data_t val)
{
    LIST_VERIFICATION(list, NULL);

    for(Block *pos = ListTail(list); pos != list->data; pos = pos->next)
    {
        if(pos->val == val) return pos;
    }

    return NULL;
}

Block *GetPos(struct List *const list, const size_t ord_pos)
{
    LIST_VERIFICATION(list, NULL);

    Block *pos = ListTail(list);

    for(size_t i = 0; i < ord_pos; i++)
    {
        pos = pos->next;

        ASSERT(pos != list->data, return NULL);
    }

    return pos;
}


static void ListPointersDump(List *list)
{
    fprintf(stderr, color_red("\t %p \t"), list->data);

    for(Block *p = list->data->next; p != list->data && p != NULL; p = p->next)
    {
             if(p == list->data->next) fprintf(stderr, color_blue  (" %p \t"), p);
        else if(p == list->data->prev) fprintf(stderr, color_purple(" %p \t"), p);
        else                           fprintf(stderr, color_green (" %p \t"), p);
    }
}

static void ListValDump(List *list)
{
    fprintf(stderr, "DATA:\t");
    fprintf(stderr, color_red("[" DATA_FORMAT "]\t"), list->data->val);

    for(Block *p = list->data->next; p != list->data && p != NULL; p = p->next)
    {
             if(p == list->data->next) fprintf(stderr, color_blue  ("[" DATA_FORMAT "]\t"), p->val);
        else if(p == list->data->prev) fprintf(stderr, color_purple("[" DATA_FORMAT "]\t"), p->val);
        else                           fprintf(stderr, color_green ("[" DATA_FORMAT "]\t"), p->val);
    }
}

static void ListNextDump(List *list)
{
    fprintf(stderr, "NEXT:\t");
    fprintf(stderr, color_red("[%p]\t"), list->data->next);

    for(Block *p = list->data->next; p != list->data && p != NULL; p = p->next)
    {
             if(p == list->data->next) fprintf(stderr, color_blue  ("[%p]\t"), p->next);
        else if(p == list->data->prev) fprintf(stderr, color_purple("[%p]\t"), p->next);
        else                           fprintf(stderr, color_green ("[%p]\t"), p->next);
    }
}

static void ListPrevDump(List *list)
{
    fprintf(stderr, "PREV:\t");
    fprintf(stderr, color_red("[%p]\t"), list->data->prev);

    for(Block *p = list->data->next; p != list->data && p != NULL; p = p->next)
    {
             if(p == list->data->next) fprintf(stderr, color_blue  ("[%p]\t"), p->prev);
        else if(p == list->data->prev) fprintf(stderr, color_purple("[%p]\t"), p->prev);
        else                           fprintf(stderr, color_green ("[%p]\t"), p->prev);
    }
}

void ListDump(List *const list)
{
    ASSERT(list, return);

    fprintf(stderr, "LIST[%p]:  \n", list);
    fprintf(stderr, "\tdata: %p \n"
                    "\tsize: %zu\n", list->data, list->size);

    if(!list->data) return;

    ListPointersDump(list);
    fprintf(stderr, "\n\n");

    ListValDump(list);
    fprintf(stderr, "\n\n");

    ListNextDump(list);
    fprintf(stderr, "\n\n");

    ListPrevDump(list);
    fprintf(stderr, "\n\n");
}

#ifdef PROTECT
bool IsListValid(List *const list)
{
    ASSERT(list, return false);

    ASSERT(list->data            , return false);
    ASSERT(list->size <= UINT_MAX, return false);

    ASSERT(list->data->val == DATA_MAX, return false);
    ASSERT(list->data->next           , return false);

    size_t counter = 0;
    for(Block *i = list->data->next; i != list->data; i = i->next)
    {
        ASSERT(i->next           , return false);
        ASSERT(i->next->prev == i, return false);

        counter++;
    }

    ASSERT(counter == list->size, return false);

    return true;
}
#endif