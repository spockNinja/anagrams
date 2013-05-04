// usage:
// include server.h, builder.h, word_list.h in the *.c file which
// uses this test framework.
// Include this file in a conditional pre-compile statement.
// See accept.c for example usage.


Player p[] = {
    { 0, 0, "bob", 8081, 1, 1},
    { 0, 1, "jill", 8082, 1, 1},
    { 0, 1, "", 8082, 0, 1},
    { 0, 1, "", 8082, 0, 1},
    { 0, 1, "", 8082, 0, 1},
    { 0, 1, "", 8082, 0, 1},
    { 0, 1, "", 8082, 0, 1},
    { 0, 1, "", 8082, 0, 1},
    { 0, 1, "", 8082, 0, 1},
    { 0, 1, "", 8082, 0, 1}
};

struct word_node base = {"llamas", "aallms", 6, NULL};

struct word_node fours = {"mall", "allm", 4, NULL};

struct word_node fivetemp1 = {"small", "aallms", 5, NULL};
struct word_node fives = {"llama", "aallm", 5, &fivetemp1} ;

struct word_set factors = {
    NULL,
    &fours,
    &fives,
    NULL,
    NULL,
    NULL
};

int usedwords[2] = {2, 3};

Server_Info server_info = {
    p,
    "8080",
    2,
    20,
    5,
    usedwords,
    &base,
    &factors,
    "ms"
};


