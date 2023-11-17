#ifndef LIST_H
#define LIST_H

#include <limits.h>
#include <stdbool.h>

#include "log.h"

#define HTML
#include "colors.h"

typedef int data_t;
#define DATA_FORMAT "%10d"
#define DATA_MAX INT_MAX

const size_t BASE_CAPACITY = 2;

const int MAX_STR_LEN = 1000;

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

#define LIST_DUMP(list_ptr) ListDump(list_ptr, __FILE__, __PRETTY_FUNCTION__, __LINE__)

#ifdef PROTECT
#define LIST_VERIFICATION(list_ptr, ret_val_on_fail) if(!IsListValid(list_ptr))\
                                                     {\
                                                         LOG("Error: invalid list.\n");\
                                                         LIST_DUMP(list_ptr);\
                                                         return ret_val_on_fail;\
                                                     }
#else
#define LIST_VERIFICATION(...)
#endif

List ListCtor(const size_t capasity = BASE_CAPACITY);

int ListDtor(List *list);

size_t ListHead(List *list);

size_t ListTail(List *list);

size_t ListAppend(List *list, const size_t id_prev, const data_t val);

int ListDelete(List *list, const size_t id_del, data_t *val = NULL);

size_t ListSearch(List *const list, const data_t val);

size_t GetPos(List *const list, const size_t ord_pos);

void ListDot(List *list, const char *png_file_name);

void ListDump(List *list, const char *file, const char *func, const int line);

#ifdef PROTECT
bool IsListValid(List *const list);
#endif

#endif //LIST_H