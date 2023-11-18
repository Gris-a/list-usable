#include "include/basic_list.h"

int main(void)
{
    List list = ListCtor();
    ListAppend(&list, ListTail(&list), 5);
    ListAppend(&list, ListTail(&list), 6);
    ListAppend(&list, ListTail(&list), 7);
    LIST_DUMP(&list);
    ListDtor(&list);
}