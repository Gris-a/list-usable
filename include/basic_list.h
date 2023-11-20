#ifndef LIST_H
#define LIST_H

#include <limits.h>
#include <stdbool.h>

#include "log.h"
#include "colors.h"

typedef int data_t;
#define DATA_FORMAT "%14d"
#define DATA_MAX INT_MAX

struct Block
{
    data_t val;

    Block *next;
    Block *prev;
};

struct List
{
    Block *data;

    size_t size;
};

#define LIST_DUMP(list_ptr) fprintf(stderr, "Called from %s:%s:%d:\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);\
                            ListDump(list_ptr)

#ifdef PROTECT
#define LIST_VERIFICATION(list_ptr, ret_val_on_fail) if(!IsListValid(list_ptr))\
                                                     {\
                                                         LOG("%s:%s: Error: invalid list.\n", __FILE__, __PRETTY_FUNCTION__);\
                                                         LIST_DUMP(list_ptr);\
                                                         return ret_val_on_fail;\
                                                     }
#else
#define LIST_VERIFICATION(...)
#endif

List ListCtor(void);

int ListDtor(List *list);

Block *ListHead(List *list);

Block *ListTail(List *list);

Block *ListAppend(List *list, Block *prev_p, const data_t val);

int ListDelete(List *list, Block *del_p, data_t *val = NULL);

Block *ListSearch(List *const list, const data_t val);

Block *GetPos(List *const list, const size_t ord_pos);

void ListDump(List *const list);

#ifdef PROTECT
bool IsListValid(List *const list);
#endif

#endif //LIST_H