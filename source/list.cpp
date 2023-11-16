#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "../include/list.h"

List ListCtor(const size_t capacity)
{
    ASSERT(capacity, return {});

    List list = {};

    list.capacity = capacity;
    list.size     = 0;
    list.free     = 1;

    list.data = (Block *)calloc(capacity + 1, sizeof(Block));
    ASSERT(list.data, return {});

    for(size_t i = 1; i <= capacity; i++)
    {
        list.data[i].next = i + 1;
        list.data[i].prev = EOF;
    }

    return list;
}

int ListDtor(List *list)
{
    LIST_VER(list, EXIT_FAILURE);

    free(list->data);

    list->size     = ULLONG_MAX;
    list->capacity = 0;
    list->free     = 0;

    return EXIT_SUCCESS;
}

ssize_t ListHead(List *list)
{
    LIST_VER(list, -1);

    return list->data[0].prev;
}

size_t ListTail(List *list)
{
    LIST_VER(list, ULLONG_MAX);

    return list->data[0].next;
}

static int ExpList(List *list)
{
    Block *new_data = (Block *)realloc(list->data, sizeof(Block) * (list->capacity * 2 + 1));
    ASSERT(new_data, return EXIT_FAILURE);

    list->data = new_data;

    for(size_t i = list->capacity + 1; i < 2 * list->capacity + 1; i++)
    {
        list->data[i].val  = 0;
        list->data[i].next = i + 1;
        list->data[i].prev = EOF;
    }

    list->capacity *= 2;

    return EXIT_SUCCESS;
}

size_t ListAppend(List *list, const size_t id, const data_t val)
{
    LIST_VER(list, 0);

    ASSERT(id <= list->capacity && list->data[id].prev != EOF, return 0);

    size_t free = list->free;
    list->free  = list->data[list->free].next;

    list->data[free].val = val;

    list->data[free               ].next = list->data[id].next;
    list->data[list->data[id].next].prev = (ssize_t)free;
    list->data[id                 ].next = free;
    list->data[free               ].prev = (ssize_t)id;

    list->size++;

    EXEC_ASSERT(!(list->size == list->capacity && ExpList(list)), return 0);

    return free;
}

int ListDelete(List *list, const size_t id, data_t *val)
{
    LIST_VER(list, EXIT_FAILURE);

    ASSERT(id != 0 && id <= list->capacity && list->data[id].prev != EOF, return EXIT_FAILURE);

    if(val) *val = list->data[id].val;

    list->data[list->data[id].prev].next = list->data[id].next;
    list->data[list->data[id].next].prev = list->data[id].prev;

    list->data[id].val  = 0;
    list->data[id].prev = EOF;
    list->data[id].next = list->free;

    list->free = id;

    list->size--;

    return EXIT_SUCCESS;
}

size_t ListSearch(List *const list, const data_t val)
{
    LIST_VER(list, 0);

    for(size_t pos = ListTail(list); pos; pos = list->data[pos].next)
    {
        if(list->data[pos].val == val) return pos;
    }

    return 0;
}

size_t GetPos(List *const list, const size_t ord_pos)
{
    LIST_VER(list, 0);

    ASSERT(ord_pos < list->size, return 0);

    size_t pos = ListTail(list);
    for(size_t i = 0; i < ord_pos; i++)
    {
        pos = list->data[pos].next;
    }

    return pos;
}

void ListDump(List *const list)
{
    ASSERT(list, return);

    fprintf(stderr, "LIST[%p]:       \n"
                    "\tsize:    %5zu,\n"
                    "\tcapacity:%5zu,\n"
                    "\n", list, list->size, list->capacity);

    ASSERT(list->data, return);

    fprintf(stderr, "\t\033[91m[NULL]\033[0m\t");
    for(size_t i = 1; i <= list->capacity; i++)
    {
             if(i ==         ListTail(list)) fprintf(stderr, "\033[94m[TAIL]\033[0m\t");
        else if(i == (size_t)ListHead(list)) fprintf(stderr, "\033[95m[HEAD]\033[0m\t");
        else if(i ==         list->free        ) fprintf(stderr, "\033[93m[FREE]\033[0m\t");
        else fprintf(stderr, "\t");
    }
    fprintf(stderr, "\n\n");

    fprintf(stderr, "\t\040\033[91m%4d\033[0m \t", 0);
    for(size_t i = 1; i <= list->capacity; i++)
    {
             if(i ==         ListTail(list)) fprintf(stderr, "\040\033[94m%4zu\033[0m\040\t", i);
        else if(i == (size_t)ListHead(list)) fprintf(stderr, "\040\033[95m%4zu\033[0m\040\t", i);
        else if(list->data[i].prev == EOF      ) fprintf(stderr, "\040\033[93m%4zu\033[0m\040\t", i);
        else                                     fprintf(stderr, "\040\033[92m%4zu\033[0m\040\t", i);
    }
    fprintf(stderr, "\n\n");

    fprintf(stderr, "DATA:\t\033[91m[" DTS "]\033[0m\t", list->data[0].val);
    for(size_t i = 1; i <= list->capacity; i++)
    {
             if(i ==         ListTail(list)) fprintf(stderr, "\033[94m[" DTS "]\033[0m\t", list->data[i].val);
        else if(i == (size_t)ListHead(list)) fprintf(stderr, "\033[95m[" DTS "]\033[0m\t", list->data[i].val);
        else if(list->data[i].prev == EOF      ) fprintf(stderr, "\033[93m[" DTS "]\033[0m\t", list->data[i].val);
        else                                     fprintf(stderr, "\033[92m[" DTS "]\033[0m\t", list->data[i].val);
    }
    fprintf(stderr, "\n\n");

    fprintf(stderr, "NEXT:\t\033[91m[%4zu]\033[0m\t", ListTail(list));
    for(size_t i = 1; i <= list->capacity; i++)
    {
             if(i ==         ListTail(list)) fprintf(stderr, "\033[94m[%4zu]\033[0m\t", list->data[i].next);
        else if(i == (size_t)ListHead(list)) fprintf(stderr, "\033[95m[%4zu]\033[0m\t", list->data[i].next);
        else if(list->data[i].prev == EOF      ) fprintf(stderr, "\033[93m[%4zu]\033[0m\t", list->data[i].next);
        else                                     fprintf(stderr, "\033[92m[%4zu]\033[0m\t", list->data[i].next);
    }
    fprintf(stderr, "\n\n");

    fprintf(stderr, "PREV:\t\033[91m[%4zd]\033[0m\t", ListHead(list));
    for(size_t i = 1; i <= list->capacity; i++)
    {
             if(i ==         ListTail(list)) fprintf(stderr, "\033[94m[%4zd]\033[0m\t", list->data[i].prev);
        else if(i == (size_t)ListHead(list)) fprintf(stderr, "\033[95m[%4zd]\033[0m\t", list->data[i].prev);
        else if(list->data[i].prev == EOF      ) fprintf(stderr, "\033[93m[FREE]\033[0m\t");
        else                                     fprintf(stderr, "\033[92m[%4zd]\033[0m\t", list->data[i].prev);
    }
    fprintf(stderr, "\n\n");
}

void ListDot(List *list)
{
    static int num = 0;
    ASSERT(list, return);

    char name[1000] = {};
    sprintf(name, "dump/source/list_dump%d.dot", num);
    FILE *graph = fopen(name, "w");

    fprintf(graph, "digraph             \n"
                   "{                   \n"
                   "bgcolor =\"#766B6B\"\n"
                   "ranksep = equally   \n"
                   "node[shape = Mrecord; style=filled; fillcolor=\"gray\"];\n");

    fprintf(graph, "nodel[label = \"free: %zu|{head: %zd|tail: %zu}|{size: %zu|capacity: %zu}\"; fillcolor = \"orchid\"]\n", list->free,
                                                                                                                ListHead(list),
                                                                                                                ListTail(list),
                                                                                                                list->size,
                                                                                                                list->capacity);
    for(size_t i = 0; i <= list->capacity; i++)
    {
        fprintf(graph, "node%zu[label = \"<prev> p:%zd | <id> id: %zu | <val> val: " DTS " | <next> n:%zu\"];\n", i,
        list->data[i].prev , i, list->data[i].val, list->data[i].next);
    }

    for(size_t i = 1; i <= list->capacity; i++)
    {
        fprintf(graph, "node%zu -> node%zu[style=\"invis\"; weight = 100];\n", i - 1, i);
    }

    for(size_t i = 0; i <= list->capacity; i++)
    {
        if(list->data[i].prev == EOF)
        {
            if(list->data[i].next <= list->capacity)
            {
                fprintf(graph, "node%zu:<next> -> node%zu:<next>[color = \"orange\"; constraint = false];\n", i, list->data[i].next);
            }
            continue;
        }

        fprintf(graph, "node%zu:<next> -> node%zu:<next>[color = \"black\"; constraint = false];\n", i, list->data[i].next);
        fprintf(graph, "node%zu:<prev> -> node%zd:<prev>[color = \"green\"; constraint = false];\n", i, list->data[i].prev);
    }

    fprintf(graph, "}\n");
    fclose(graph);

    sprintf(name, "dump/html/list_dump%d.html", num);
    FILE *html = fopen(name, "wb");
    fprintf(html, "<img src=\"../img/list_dump%d.png\"/>", num);
    fclose(html);

    sprintf(name, "dot dump/source/list_dump%d.dot -T png -o dump/img/list_dump%d.png", num, num);
    system(name);

    num++;
}

#ifdef PROTECT
int ListVer(List *const list)
{
    ASSERT(list && list->data && list->data[0].val == 0       , return EXIT_FAILURE);
    ASSERT(list->size <= list->capacity && list->capacity != 0, return EXIT_FAILURE);
    ASSERT(list->free != 0 && list->free <= list->capacity    , return EXIT_FAILURE);

    size_t tail_pos    = 0;
    size_t tails_count = 0;

    size_t head_pos    = 0;
    size_t heads_count = 0;

    for(size_t i = 0; i <= list->capacity; i++)
    {
        if(list->data[i].next == 0) {head_pos = i; heads_count++;}
        if(list->data[i].prev == 0) {tail_pos = i; tails_count++;}
    }
    ASSERT( tail_pos ==         list->data[0].next && tails_count == 1 &&
            head_pos == (size_t)list->data[0].prev && heads_count == 1, return EXIT_FAILURE);

    for(size_t i = 1; i < list->size; i++)
    {
        ASSERT(tail_pos != head_pos && list->data[tail_pos].next <= list->capacity, return EXIT_FAILURE);
        ASSERT(list->data[list->data[tail_pos].next].prev == (ssize_t)tail_pos    , return EXIT_FAILURE);

        tail_pos = list->data[tail_pos].next;
    }
    ASSERT(tail_pos == head_pos, return EXIT_FAILURE);

    size_t free_pos = list->free;
    for(size_t i = 1; i < list->capacity - list->size; i++)
    {
        ASSERT(list->data[free_pos].val  == 0   &&
               list->data[free_pos].prev == EOF &&
               list->data[free_pos].next <= list->capacity, return EXIT_FAILURE);

        free_pos = list->data[free_pos].next;
    }
    ASSERT(list->data[free_pos].val  == 0   &&
           list->data[free_pos].prev == EOF &&
           list->data[free_pos].next == list->capacity + 1, return EXIT_FAILURE);

    return EXIT_SUCCESS;
}
#endif