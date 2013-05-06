// Defines things for files to use the word_list utility


// A node for linked lists of words *must be defined as 'struct word_node' because of recursive nature
struct word_node {
    char* word;
    char* sorted_word;
    int len;
    struct word_node* next;
};

// A set of linked lists, one for each possible word length
struct word_set {
    struct word_node* threes;
    struct word_node* fours;
    struct word_node* fives;
    struct word_node* sixes;
    struct word_node* sevens;
    struct word_node* eights;
};

void free_list(struct word_node* head);

struct word_node* create_node(char* word, char* sorted_word, int len);

int count_words(struct word_node* head);
