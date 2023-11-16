#include "include/list.h"

int main(void)
{
    List list = ListCtor();
    ListAppend(&list, 0, 5);
    ListAppend(&list, 1, 6);
    ListAppend(&list, 0, 7);
    LIST_DUMP(&list);
    ListDtor(&list);
}