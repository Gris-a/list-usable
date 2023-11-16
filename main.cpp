#include "include/list.h"

int main(void)
{
    List list = ListCtor();
    ListAppend(&list, 0, 5);
    ListAppend(&list, 1, 6);
    ListAppend(&list, 0, 7);
    ListAppend(&list, 3, 8);
    ListAppend(&list, 0, 9);
    ListDot(&list);
    ListDot(&list);
    ListDot(&list);
    ListDtor(&list);
}