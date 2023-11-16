#ifndef LIST_H
#define LIST_H

#include <limits.h>

#include "log.h"

#define HTML
#include "colors.h"

typedef int data_t;
#define DTS "%10d"
#define DATA_MAX INT_MAX

const int MAX_CMD_LEN = 1000;

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

#define LIST_DUMP(list_ptr) LOG("Called from %s:%s:%d:\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);\
                            ListDump(list_ptr, __FILE__, __PRETTY_FUNCTION__, __LINE__)

#ifdef PROTECT
#define LIST_VER(list_ptr, ret_val_on_fail) if(ListVer(list_ptr))\
                                            {\
                                                LOG("Error: invalid list.\n");\
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

void ListDump(List *list, const char *const file, const char *const func, const int line);

#ifdef PROTECT
int ListVer(List *const list);
#endif

#endif //LIST_H