#include <stdlib.h>
#include <string.h>

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

    list.data[0].val = DATA_MAX;
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
    list->data = NULL;

    list->size     = ULLONG_MAX;
    list->capacity = 0;
    list->free     = 0;

    return EXIT_SUCCESS;
}


size_t ListHead(List *list)
{
    LIST_VER(list, ULLONG_MAX);

    return (size_t)list->data[0].prev;
}

size_t ListTail(List *list)
{
    LIST_VER(list, ULLONG_MAX);

    return list->data[0].next;
}


static int ExpList(List *list)
{
    if(list->size == list->capacity)
    {
        ASSERT(list->capacity <= ULLONG_MAX / 2, return EXIT_FAILURE);

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
    }

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

    list->data[id  ].next = free;
    list->data[free].prev = (ssize_t)id;

    list->size++;

    EXEC_ASSERT(!ExpList(list), return 0);

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

    for(size_t pos = ListTail(list); pos != 0; pos = list->data[pos].next)
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


static void MakeDumpDir(void)
{
    system("rm -rf dump_list");
    system("mkdir dump_list");
    system("mkdir dump_list/img");
    system("mkdir dump_list/source");
    system("mkdir dump_list/html");
}

static void ListText(List *const list, const char *path, const char *file, const char *func, const int line, const int img_num) //TODO cringe
{
    ASSERT(file && func, return);

    FILE *html = fopen(path, "wb");
    ASSERT(html, return);

    fprintf(html, "<body bgcolor=\"000000\"><pre>\n");
    fprintf(html, color_white("Called from %s:%s:%d\n\n"), file, func, line);
    fprintf(html, color_white("LIST[%p]:       \n"
                              "\tsize:    %5zu,\n"
                              "\tcapacity:%5zu,\n"
                              "\n"), list, list->size, list->capacity);

    ASSERT(list->data, fclose(html);
                       return);

    fprintf(html, color_red("\t[      NULL]\t"));
    for(size_t i = 1; i <= list->capacity; i++)
    {
             if(i ==         list->data[0].next) fprintf(html, color_blue  ("[      TAIL]\t"));
        else if(i == (size_t)list->data[0].prev) fprintf(html, color_purple("[      HEAD]\t"));
        else if(i ==         list->free        ) fprintf(html, color_yellow("[      FREE]\t"));
        else                                     fprintf(html, "\t\t");
    }
    fprintf(html, "\n\n\t");

#define DUMP_COLORED(format, iterator, val) for(size_t iterator = 1; iterator <= list->capacity; iterator++)\
                                        {\
                                                 if(iterator ==         list->data[0].next) fprintf(html, color_blue  (format), val);\
                                            else if(iterator == (size_t)list->data[0].prev) fprintf(html, color_purple(format), val);\
                                            else if(list->data[iterator].prev == EOF      ) fprintf(html, color_yellow(format), val);\
                                            else                                            fprintf(html, color_green (format), val);\
                                        }\
                                        fprintf(html, "\n\n");

    fprintf(html, color_red(" %10d \t"), 0);
    DUMP_COLORED(" %10zu \t", iterator, iterator);

    fprintf(html, color_white("DATA:\t"));
    fprintf(html, color_red("[" DTS "]\t"), list->data[0].val);
    DUMP_COLORED("[" DTS "]\t", iterator, list->data[iterator].val);

    fprintf(html, color_white("NEXT:\t"));
    fprintf(html, color_red("[%10zu]\t"), list->data[0].next);
    DUMP_COLORED("[%10zu]\t", iterator, list->data[iterator].next);

    fprintf(html, color_white("PREV:\t"));
    fprintf(html, color_red("[%10zd]\t"), list->data[0].prev);
    DUMP_COLORED("[%10zd]\t", iterator, list->data[iterator].prev);
#undef DUMP_COLORED

    fprintf(html, "<img src=\"../img/list_dump%d.png\"/>", img_num);

    fclose(html);
}

static void ListDot(List *list, char *path, const int img_num)
{
    ASSERT(list->data, return);

    FILE *graph = fopen(path, "wb");
    ASSERT(graph, return);

    fprintf(graph, "digraph             \n"
                   "{                   \n"
                   "bgcolor =\"#766B6B\"\n"
                   "ranksep = equally   \n"
                   "node[shape = Mrecord; style=filled; fillcolor=\"gray\"];\n");

    fprintf(graph, "nodel[label = \"free: %zu|{head: %zd|tail: %zu}|{size: %zu|capacity: %zu}\"; fillcolor = \"orchid\"]\n", list->free,
                                                                                                                             list->data[0].prev,
                                                                                                                             list->data[0].next,
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

    sprintf(path, "dot dump_list/source/list_dump%d.dot -T png -o dump_list/img/list_dump%d.png", img_num, img_num);
    system(path);
}

void ListDump(List *list, const char *file, const char *func, const int line)
{
    static int num = 0;

    ASSERT(list, return);

    if(num == 0) MakeDumpDir();

    char path[MAX_CMD_LEN] = {};

    sprintf(path, "dump_list/html/list_dump%d.html", num);
    ListText(list, path, file, func, line, num);

    sprintf(path, "dump_list/source/list_dump%d.dot", num);
    ListDot(list, path, num);

    num++;
}

#ifdef PROTECT
static int TailHeadVer(List *const list)
{
    size_t tail_pos    = 0;
    size_t tails_count = 0;

    size_t head_pos    = 0;
    size_t heads_count = 0;

    for(size_t i = 0; i <= list->capacity; i++)
    {
        if(list->data[i].next == 0) {head_pos = i; heads_count++;}
        if(list->data[i].prev == 0) {tail_pos = i; tails_count++;}
    }
    ASSERT(tail_pos == list->data[0].next         && tails_count == 1, return EXIT_FAILURE);
    ASSERT(head_pos == (size_t)list->data[0].prev && heads_count == 1, return EXIT_FAILURE);

    return EXIT_SUCCESS;
}

static int CycleVer(List *const list)
{
    size_t cur_pos = list->data[0].next;
    size_t head    = (size_t)list->data[0].prev;

    for(size_t i = 1; i < list->size; i++)
    {
        ASSERT(cur_pos != head && list->data[cur_pos].next <= list->capacity, return EXIT_FAILURE);
        ASSERT(list->data[list->data[cur_pos].next].prev == (ssize_t)cur_pos, return EXIT_FAILURE);

        cur_pos = list->data[cur_pos].next;
    }

    ASSERT(cur_pos == head, return EXIT_FAILURE);

    return EXIT_SUCCESS;
}

static int FreeMemVer(List *const list)
{
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

int ListVer(List *const list)
{
    ASSERT(list && list->data && list->data[0].val == DATA_MAX, return EXIT_FAILURE);
    ASSERT(list->size <= list->capacity && list->capacity != 0, return EXIT_FAILURE);
    ASSERT(list->free != 0 && list->free <= list->capacity    , return EXIT_FAILURE);

    EXEC_ASSERT(!TailHeadVer(list), return EXIT_FAILURE);
    EXEC_ASSERT(!CycleVer(list)   , return EXIT_FAILURE);
    EXEC_ASSERT(!FreeMemVer(list) , return EXIT_FAILURE);

    return EXIT_SUCCESS;
}
#endif