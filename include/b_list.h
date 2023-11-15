#ifndef B_LIST_H
#define B_LIST_H

#include "log.h"

typedef int data_t;
#define DTS "%14d"

struct List_t
{
    data_t val;

    List_t *next;
    List_t *prev;
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

List_t ListCtor(void);

int ListDtor(List_t *list);

List_t *ListAppend(List_t *list, List_t *prev_p, const data_t val);

int ListDelete(List_t *list, List_t *del_p, data_t *val = NULL);

List_t *ListSearch(List_t *const list, const data_t val);

List_t *GetPos(List_t *const list, const size_t ord_pos);

void ListDump(List_t *const list);

#ifdef PROTECT
int ListVer(List_t *const list);

bool InList(struct List_t *const list, List_t *const elem);
#endif

#endif //B_LIST_H