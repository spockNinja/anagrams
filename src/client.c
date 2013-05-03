#define _BSD_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <ncurses.h>
#include <panel.h>
#include "word_list.h"

#define COLUMNS 80
#define ROWS 25
#define clr_wrd "                                                          |"

WINDOW *round_info;
WINDOW *rankings;
WINDOW *puzzle_words;
WINDOW *word_input;
WINDOW *prompt;
WINDOW *bell;

PANEL *my_panels[6];

//just a sample - will come from server
char *round_letters = "BEEEILSV";

// The history linked list will have words added to the front
// to make it easy to traverse in last-added order
struct word_node* history_head = NULL;

static void quit();
static void draw_bell();
static void draw_prompt();
static void draw_round_info();
static void ring_bell();
static void update_round_info(int round, char* letters, char* timestamp);
static void send_word(char* word);

int main(){
    // The client will be dumb and single threaded.
    // All of it will be contained in this file
    initscr();
    raw();
    noecho();

    if (!has_colors()) {
        endwin();
        fprintf(stderr, "No color support on this terminal.\n");
        exit(EXIT_FAILURE);
    }
    if (start_color() != OK) {
        endwin();
        fprintf(stderr, "Error - could not initialize colors.\n");
        exit(EXIT_FAILURE);
    }

    // Create windows for each section of the screen
    round_info = newwin(4, 80, 0, 0);
    rankings = newwin(21, 20, 4, 60);
    puzzle_words = newwin(18, 60, 4, 0);
    word_input = newwin(3, 60, 22, 0);
    prompt = newwin(5, 35, 8, 20);
    bell = newwin(6, 13, 8, 30);

    // Do a one-time draw for things that shouldn't have to change
    draw_bell();
    draw_prompt();
    draw_round_info();

    // Attach overlapping windows to panels
    // putting word_input up last to be on top
    my_panels[0] = new_panel(prompt);
    my_panels[1] = new_panel(rankings);
    my_panels[2] = new_panel(puzzle_words);
    my_panels[3] = new_panel(round_info);
    my_panels[4] = new_panel(bell);
    my_panels[5] = new_panel(word_input);

    // Create borders around the windows
    box(round_info, 0, 0);
    box(rankings, 0, 0);
    box(puzzle_words, 0, 0);
    box(word_input, 0, 0);
    box(prompt, 0, 0);

    // Make sure the prompt and bell are hidden to start
    hide_panel(my_panels[4]);
    hide_panel(my_panels[0]);

    // Update the stacking order
    update_panels();

    // Show it on the screen
    doupdate();

    // move the cursor to the beginning of the word_input box
    wmove(word_input, 1, 1);

    char *current_word = calloc(9, sizeof(char));

    // temp node since you can't declare inside switch
    struct word_node* history_next = NULL;

    // process input as long as it keeps coming
    int ch;
    for (;;) {
        int len = strlen(current_word);
        ch = getch();
        switch(ch) {
            case 8:
            case 127:
                // backspace and delete
                if (len > 0) {
                    *(current_word + len - 1) = '\0';
                    mvwdelch(word_input, 1, len);
                    winsch(word_input, ' ');
                    wmove(word_input, 1, len);
                    wrefresh(word_input);
                }
                else {
                    ring_bell();
                }
                break;
            case 10:
            case 13:
                // return
                if (len > 0) {
                    // add word to history
                    history_next = create_node(current_word, "", len);
                    history_next->next = history_head;
                    history_head = history_next;

                    // send word to server
                    send_word(current_word);

                    // reset word and clear the input area
                    memset(current_word, 0, len);
                    mvwaddstr(word_input, 1, 1, clr_wrd);
                    wmove(word_input, 1, 1);
                    wrefresh(word_input);
                    break;
                }
                // else don't break and behave like space/tab/ctrl-i
            case 9:
            case 32:
                // space & tab -- auto-complete words

                // if no words have been entered, ring_bell()
                if (history_head == NULL) {
                    ring_bell();
                    break;
                }

                // if the current_word is size 0, autofill with last word
                if (len == 0) {
                    strcpy(current_word, history_head->word);
                    waddstr(word_input, current_word);
                    wrefresh(word_input);
                }
                // otherwise, find the first word that begins with current word
                else {
                    history_next = history_head;
                    while (history_next != NULL) {
                        if (strncmp(current_word, history_next->word, len) == 0) {
                            memset(current_word, 0, len);
                            strcpy(current_word, history_next->word);
                            mvwaddstr(word_input, 1, 1, clr_wrd);
                            wmove(word_input, 1, 1);
                            waddstr(word_input, current_word);
                            wrefresh(word_input);
                            break;
                        }
                        history_next = history_next->next;
                    }
                }

                // draw the updated word to the screen
                break;
            case '\x03':
            case '\x11':
                // ctrl-c and ctrl-q -- quit
                quit();
                break;
            default:
                // anything else

                // make lowercase uppercase
                if (ch >= 'a' && ch <= 'z') {
                    ch = ch - 32;
                }
                if (ch >= 'A' && ch <= 'Z') {
                    if (len < 8 && strchr(round_letters, ch) != NULL) {
                        *(current_word + len) = ch;
                        wechochar(word_input, ch);
                    }
                    else {
                        ring_bell();
                    }
                }
                break;
        }
    }

    endwin();
}

static void send_word(char *word) {
};

static void ring_bell() {
    top_panel(my_panels[4]);
    update_panels();
    doupdate();
    usleep(100000);
    hide_panel(my_panels[4]);
    update_panels();
    doupdate();
};

static void draw_prompt() {
    mvwaddstr(prompt, 1, 1, " Are your sure you want to quit?");
    mvwaddstr(prompt, 2, 1, " Press y or Y to confirm.");
    mvwaddstr(prompt, 3, 1, " Press anything else to dismiss.");
    wrefresh(prompt);
};

static void draw_bell() {
    mvwaddstr(bell, 0, 4, "_(#)_");

    mvwaddch(bell, 1, 3, '/');
    mvwaddch(bell, 1, 9, '\\');

    mvwaddch(bell, 2, 2, '|');
    mvwaddch(bell, 2, 10, '|');

    mvwaddstr(bell, 3, 2, "|_______|");

    mvwaddch(bell, 4, 1, '/');
    mvwaddch(bell, 4, 11, '\\');

    mvwaddstr(bell, 5, 0, "|====(-)====|");

    wrefresh(bell);
};

static void draw_round_info() {
    mvwaddstr(round_info, 1, 1, "Round:");
    mvwaddstr(round_info, 1, 65, "Time Remaining:");

    wrefresh(round_info);
};

static void quit()
{
    top_panel(my_panels[0]);
    update_panels();
    doupdate();

    int response = getch();
    if (response == 'y' || response == 'Y') {
        endwin();
        exit(0);
    }

    hide_panel(my_panels[0]);
    update_panels();
    doupdate();
}
