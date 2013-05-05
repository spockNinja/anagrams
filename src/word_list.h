// Defines things for files to use the word_list utility

// A node for linked lists of words *must be defined as 'struct word_node' because of recursive nature
struct word_node {
    char* word;
    char* sorted_word;
    int len;
    struct word_node* next;
};

void free_list(struct word_node* head);

struct word_node* create_node(char* word, char* sorted_word, int len);

int count_words(struct word_node* head);
