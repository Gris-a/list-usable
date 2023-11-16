#ifndef LIST_H
#define LIST_H

#include "log.h"

typedef int data_t;
#define DTS "%4d"

struct Block
{
    data_t  val;
    size_t  next;
    ssize_t prev;
};

struct List
{
    Block *data;
    size_t free;

    size_t capacity;
    size_t size;
};

#define LIST_DUMP(list_ptr) LOG("%s:%s:%d:\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);\
                            ListDump(list_ptr)

#ifdef PROTECT
#define LIST_VER(list_ptr, ret_val_on_fail) if(ListVer(list_ptr))\
                                            {\
                                                LOG("%s:%s: Error: invalid list.\n", __FILE__, __PRETTY_FUNCTION__);\
                                                LIST_DUMP(list_ptr);\
                                                return ret_val_on_fail;\
                                            }
#else
#define LIST_VER(...)
#endif

List ListCtor(const size_t capasity = 2);

int ListDtor(List *list);

ssize_t ListHead(List *list);

size_t ListTail(List *list);

size_t ListAppend(List *list, const size_t id, const data_t val);

int ListDelete(List *list, const size_t id, data_t *val = NULL);

size_t ListSearch(List *const list, const data_t val);

size_t GetPos(List *const list, const size_t ord_pos);

void ListDump(List *const list);

void ListDot(List *list);

#ifdef PROTECT
int ListVer(List *const list);
#endif

#endif //LIST_H