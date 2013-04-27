#define _BSD_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <ncurses.h>
#include <panel.h>


#define COLUMNS 80
#define ROWS 25

WINDOW *round_info;
WINDOW *rankings;
WINDOW *puzzle_words;
WINDOW *word_input;
WINDOW *prompt;
WINDOW *bell;

PANEL *my_panels[6];

static void quit(int sig);
static void draw_bell();
static void draw_prompt();
static void ring_bell();

int main(){
    // capture ctrl-c to ask if they want to quit
    (void) signal(SIGINT, quit);

    initscr();
    cbreak();
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

    // Do a on-time draw for bell and prompt
    // they shouldn't have to change throughout
    draw_bell();
    draw_prompt();

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
            case 9:
            case 32:
                // space, tab, ctrl-i
                // TODO implement auto-fill words
                break;
            case 10:
            case 13:
                // return
                // TODO send word and clear
                break;
            default:
                // anything else

                // make lowercase uppercase
                if (ch >= 'a' && ch <= 'z') {
                    ch = ch - 32;
                }
                if (ch >= 'A' && ch <= 'Z') {
                    if (len < 8) {
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
    wmove(prompt, 1, 1);
    waddstr(prompt, " Are your sure you want to quit?");

    wmove(prompt, 2, 1);
    waddstr(prompt, " Press y or Y to confirm.");

    wmove(prompt, 3, 1);
    waddstr(prompt, " Press anything else to dismiss.");

    wrefresh(prompt);
};

static void draw_bell() {
    mvwaddch(bell, 0, 4, '_');
    mvwaddch(bell, 0, 5, '(');
    mvwaddch(bell, 0, 6, '#');
    mvwaddch(bell, 0, 7, ')');
    mvwaddch(bell, 0, 8, '_');

    mvwaddch(bell, 1, 3, '/');
    mvwaddch(bell, 1, 9, '\\');

    mvwaddch(bell, 2, 2, '|');
    mvwaddch(bell, 2, 10, '|');

    mvwaddch(bell, 3, 2, '|');
    mvwaddch(bell, 3, 3, '_');
    mvwaddch(bell, 3, 4, '_');
    mvwaddch(bell, 3, 5, '_');
    mvwaddch(bell, 3, 6, '_');
    mvwaddch(bell, 3, 7, '_');
    mvwaddch(bell, 3, 8, '_');
    mvwaddch(bell, 3, 9, '_');
    mvwaddch(bell, 3, 10, '|');

    mvwaddch(bell, 4, 1, '/');
    mvwaddch(bell, 4, 11, '\\');

    mvwaddch(bell, 5, 0, '|');
    mvwaddch(bell, 5, 1, '=');
    mvwaddch(bell, 5, 2, '=');
    mvwaddch(bell, 5, 3, '=');
    mvwaddch(bell, 5, 4, '=');
    mvwaddch(bell, 5, 5, '(');
    mvwaddch(bell, 5, 6, '-');
    mvwaddch(bell, 5, 7, ')');
    mvwaddch(bell, 5, 8, '=');
    mvwaddch(bell, 5, 9, '=');
    mvwaddch(bell, 5, 10, '=');
    mvwaddch(bell, 5, 11, '=');
    mvwaddch(bell, 5, 12, '|');

    wrefresh(bell);
};


static void quit(int sig)
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
