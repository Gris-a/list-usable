#include "include/b_list.h"

int main(void)
{
    List_t list = ListCtor();
    ListAppend(&list, list.next, 5);
    ListAppend(&list, list.next, 6);
    ListAppend(&list, list.next, 7);
    LIST_DUMP(&list);
    ListDtor(&list);
}