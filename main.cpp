#include "include/list.h"

int main(void)
{
    List list = ListCtor();
    LIST_DUMP(&list);
    ListAppend(&list, ListTail(&list), 5);
    LIST_DUMP(&list);
    ListAppend(&list, ListTail(&list), 6);
    LIST_DUMP(&list);
    ListAppend(&list, ListTail(&list), 7);
    LIST_DUMP(&list);
    ListDtor(&list);
    LIST_DUMP(&list);
}