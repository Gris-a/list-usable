#include <stdlib.h>
#include <string.h>

#include "../include/list.h"

List ListCtor(const size_t capacity)
{
    if(capacity == 0)
    {
        LOG("Capacity should be greater than zero.\n");
        return {};
    }

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
    LIST_VERIFICATION(list, EXIT_FAILURE);

    free(list->data);
    list->data = NULL;

    list->size     = 0;
    list->capacity = 0;
    list->free     = 0;

    return EXIT_SUCCESS;
}


size_t ListHead(List *list)
{
    LIST_VERIFICATION(list, ULLONG_MAX);

    return (size_t)list->data[0].prev;
}

size_t ListTail(List *list)
{
    LIST_VERIFICATION(list, ULLONG_MAX);

    return list->data[0].next;
}


static int ExpandList(List *list)
{
    if(list->size == list->capacity)
    {
        Block *data_r = (Block *)realloc(list->data, sizeof(Block) * (list->capacity * 2 + 1));
        ASSERT(data_r, return EXIT_FAILURE);

        list->data = data_r;

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
    LIST_VERIFICATION(list, 0);

    ASSERT(id <= list->capacity && list->data[id].prev != EOF, return 0);

    size_t free = list->free;
    list->free  = list->data[list->free].next;

    list->data[free].val = val;

    list->data[free               ].next = list->data[id].next;
    list->data[list->data[id].next].prev = (ssize_t)free;

    list->data[id  ].next = free;
    list->data[free].prev = (ssize_t)id;

    list->size++;

    int exit_status = ExpandList(list);

    return ((exit_status == EXIT_SUCCESS) ? free : 0);
}


int ListDelete(List *list, const size_t id, data_t *val)
{
    LIST_VERIFICATION(list, EXIT_FAILURE);

    ASSERT(id <= list->capacity && list->data[id].prev != EOF, return EXIT_FAILURE);

    if(val) *val = list->data[id].val;

    if(id == 0)return ListDtor(list);

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
    LIST_VERIFICATION(list, 0);

    for(size_t pos = ListTail(list); pos != 0; pos = list->data[pos].next)
    {
        if(list->data[pos].val == val) return pos;
    }

    return 0;
}


size_t GetPos(List *const list, const size_t ord_pos)
{
    LIST_VERIFICATION(list, 0);

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
    system("mkdir dump_list/html");
}


static void ListTextLabelDump(List *list, FILE *dest)
{
    fprintf(dest, color_red("\t[      NULL]\t"));

    for(size_t i = 1; i <= list->capacity; i++)
    {
             if(i ==         list->data[0].next) fprintf(dest, color_blue  ("[      TAIL]\t"));
        else if(i == (size_t)list->data[0].prev) fprintf(dest, color_purple("[      HEAD]\t"));
        else if(i ==         list->free        ) fprintf(dest, color_yellow("[      FREE]\t"));
        else                                     fprintf(dest, "\t\t");
    }

    fprintf(dest, "\n\n\t");
}

static void ListTextIndexDump(List *list, FILE *dest)
{
    fprintf(dest, color_red(" %10d \t"), 0);

    for(size_t i = 1; i <= list->capacity; i++)
    {
             if(i ==         list->data[0].next) fprintf(dest, color_blue  (" %10zu \t"), i);
        else if(i == (size_t)list->data[0].prev) fprintf(dest, color_purple(" %10zu \t"), i);
        else if(i ==         list->free        ) fprintf(dest, color_yellow(" %10zu \t"), i);
        else                                     fprintf(dest, color_green (" %10zu \t"), i);
    }

    fprintf(dest, "\n\n");
}

static void ListTextValDump(List *list, FILE *dest)
{
    fprintf(dest, color_white("DATA:\t"));

    fprintf(dest, color_red("[" DATA_FORMAT "]\t"), list->data[0].val);

    for(size_t i = 1; i <= list->capacity; i++)
    {
             if(i ==         list->data[0].next) fprintf(dest, color_blue  ("[" DATA_FORMAT "]\t"), list->data[i].val);
        else if(i == (size_t)list->data[0].prev) fprintf(dest, color_purple("[" DATA_FORMAT "]\t"), list->data[i].val);
        else if(list->data[i].prev == EOF      ) fprintf(dest, color_yellow("[" DATA_FORMAT "]\t"), list->data[i].val);
        else                                     fprintf(dest, color_green ("[" DATA_FORMAT "]\t"), list->data[i].val);
    }

    fprintf(dest, "\n\n");
}

static void ListTextNextDump(List *list, FILE *dest)
{
    fprintf(dest, color_white("NEXT:\t"));

    fprintf(dest, color_red("[%10zu]\t"), list->data[0].next);

    for(size_t i = 1; i <= list->capacity; i++)
    {
             if(i ==         list->data[0].next) fprintf(dest, color_blue  ("[%10zu]\t"), list->data[i].next);
        else if(i == (size_t)list->data[0].prev) fprintf(dest, color_purple("[%10zu]\t"), list->data[i].next);
        else if(list->data[i].prev == EOF      ) fprintf(dest, color_yellow("[%10zu]\t"), list->data[i].next);
        else                                     fprintf(dest, color_green ("[%10zu]\t"), list->data[i].next);
    }

    fprintf(dest, "\n\n");
}

static void ListTextPrevDump(List *list, FILE *dest)
{
    fprintf(dest, color_white("PREV:\t"));

    fprintf(dest, color_red("[%10zd]\t"), list->data[0].prev);

    for(size_t i = 1; i <= list->capacity; i++)
    {
             if(i ==         list->data[0].next) fprintf(dest, color_blue  ("[%10zd]\t"), list->data[i].prev);
        else if(i == (size_t)list->data[0].prev) fprintf(dest, color_purple("[%10zd]\t"), list->data[i].prev);
        else if(list->data[i].prev == EOF      ) fprintf(dest, color_yellow("[      FREE]\t"));
        else                                     fprintf(dest, color_green ("[%10zd]\t"), list->data[i].prev);
    }

    fprintf(dest, "\n\n");
}

static void ListTextDump(List *const list, const char *path, const char *file, const char *func, const int line, const int img_num)
{
    ASSERT(file && func, return);

    FILE *html = fopen(path, "wb");

    ASSERT(html, return);

    fprintf(html, "<body bgcolor=\"000000\"><pre>\n");
    fprintf(html, color_white("Called from %s:%s:%d\n\n"), file, func, line);
    fprintf(html, color_white("LIST[%p]:       \n"
                              "\tsize:    %5zu,\n"
                              "\tcapacity:%5zu,\n\n"), list, list->size, list->capacity);
    if(!list->data)
    {
        fclose(html);

        return;
    }

    ListTextLabelDump(list, html);

    ListTextIndexDump(list, html);

    ListTextValDump(list, html);

    ListTextNextDump(list, html);

    ListTextPrevDump(list, html);

    fprintf(html, "<img src=\"../img/list_dump%d.png\"/>", img_num);

    fclose(html);
}


static void DotNodesCtor(List *list, FILE *graph)
{
    fprintf(graph, "nodel[label = \"free: %zu|{head: %zd|tail: %zu}|{size: %zu|capacity: %zu}\"; fillcolor = \"orchid\"]\n",
                                            list->free, list->data[0].prev, list->data[0].next, list->size, list->capacity);

    for(size_t i = 0; i <= list->capacity; i++)
    {
        fprintf(graph, "node%zu[label = \"<prev> p:%zd | <id> id: %zu | <val> val: " DATA_FORMAT " | <next> n:%zu\"];\n",
                                                        i, list->data[i].prev , i, list->data[i].val, list->data[i].next);
    }

    for(size_t i = 1; i <= list->capacity; i++)
    {
        fprintf(graph, "node%zu -> node%zu[style=\"invis\"; weight = 100];\n", i - 1, i);
    }
}

static void DotEdgesCtor(List *list, FILE *graph)
{
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
}

void ListDot(List *list, const char *png_file_name)
{
    if(!(list && list->data)) return;

    ASSERT(png_file_name, return);

    FILE *graph = fopen("list.dot", "wb");
    ASSERT(graph, return);

    fprintf(graph, "digraph             \n"
                   "{                   \n"
                   "bgcolor =\"#766B6B\"\n"
                   "ranksep = equally   \n"
                   "node[shape = Mrecord; style=filled; fillcolor=\"gray\"];\n");

    DotNodesCtor(list, graph);
    DotEdgesCtor(list, graph);

    fprintf(graph, "}\n");

    fclose(graph);

    char sys_cmd[MAX_STR_LEN] = {};
    sprintf(sys_cmd, "dot list.dot -T png -o %s", png_file_name);
    system(sys_cmd);

    remove("list.dot");
}


void ListDump(List *list, const char *file, const char *func, const int line)
{
    static int num = 0;

    ASSERT(list, return);

    if(num == 0) MakeDumpDir();

    char path[MAX_STR_LEN] = {};

    sprintf(path, "dump_list/html/list_dump%d.html", num);
    ListTextDump(list, path, file, func, line, num);

    sprintf(path, "dump_list/img/list_dump%d.png", num);
    ListDot(list, path);

    num++;
}

#ifdef PROTECT
static bool IsEndsValid(List *const list)
{
    size_t tail_pos    = 0;
    size_t tails_count = 0;

    size_t head_pos    = 0;
    size_t heads_count = 0;

    for(size_t i = 0; i <= list->capacity; i++)
    {
        if(list->data[i].next == 0)
        {
            head_pos = i;
            heads_count++;
        }

        if(list->data[i].prev == 0)
        {
            tail_pos = i;
            tails_count++;
        }
    }

    ASSERT(tail_pos == list->data[0].next         && tails_count == 1, return false);
    ASSERT(head_pos == (size_t)list->data[0].prev && heads_count == 1, return false);

    return true;
}

static bool IsCycleValid(List *const list)
{
    size_t cur_pos = list->data[0].next;
    size_t head    = (size_t)list->data[0].prev;

    for(size_t i = 1; i < list->size; i++)
    {
        ASSERT(cur_pos != head && list->data[cur_pos].next <= list->capacity, return false);
        ASSERT(list->data[list->data[cur_pos].next].prev == (ssize_t)cur_pos, return false);

        cur_pos = list->data[cur_pos].next;
    }

    ASSERT(cur_pos == head, return false);

    return true;
}

static bool IsFreeMemValid(List *const list)
{
    size_t free_pos = list->free;

    for(size_t i = 1; i < list->capacity - list->size; i++)
    {
        ASSERT(list->data[free_pos].val  == 0   &&
               list->data[free_pos].prev == EOF &&
               list->data[free_pos].next <= list->capacity, return false);

        free_pos = list->data[free_pos].next;
    }

    ASSERT(list->data[free_pos].val  == 0   &&
           list->data[free_pos].prev == EOF &&
           list->data[free_pos].next == list->capacity + 1, return false);

    return true;
}

bool IsListValid(List *const list)
{
    ASSERT(list && list->data && list->data[0].val == DATA_MAX, return false);

    ASSERT(list->capacity <= UINT_MAX && list->capacity != 0, return false);
    ASSERT(list->size <= list->capacity                     , return false);

    ASSERT(list->free != 0 && list->free <= list->capacity, return false);

    ASSERT(IsEndsValid(list)   , return false);
    ASSERT(IsCycleValid(list)  , return false);
    ASSERT(IsFreeMemValid(list), return false);

    return true;
}
#endif