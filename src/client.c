#define _BSD_SOURCE
#define _GNU_SOURCE
#define _XOPEN_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ncurses.h>
#include <panel.h>
#include "word_list.h"
#include "colors.h"

#define COLUMNS 80
#define ROWS 25
#define MAX_WORD_LIST 19

const char LOCALHOST[] = "127.0.0.1";

int client;

WINDOW *round_info;
WINDOW *rankings;
WINDOW *puzzle_words;
WINDOW *word_input;
WINDOW *prompt;
WINDOW *bell;

PANEL *my_panels[6];

//just a sample - will come from server
char *round_letters = "BEEEILSV";
bool accept_user_input = false;

// The history linked list will have words added to the front
// to make it easy to traverse in last-added order
struct word_node* history_head = NULL;
struct word_node* history_next = NULL;

// The word that the user is in the process of building
char current_word[9];
char cmd_buffer[128];

static void quit();
static void init_windows();
static void ring_bell();
static void parse_server_command(char *cmd);
static void process_user_input(int ch);
static void send_message(char code, char* word);

int main(int argc, char* argv[]){
    // The client will be dumb and single threaded.
    // All of it will be contained in this file

    int SERVER_PORT = 0;
    char* USERNAME;

    // Parse args
    if (argc == 1) {
        fprintf(stderr, "Please specify the server port and an optional username.\n\tEX: %s 8000 jimmy", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (argc == 2) {
        SERVER_PORT = atoi(argv[1]);
        USERNAME = getlogin();
    }
    else {
        SERVER_PORT = atoi(argv[1]);
        USERNAME = argv[2];
    };

    // create a socket for the client
    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == -1) {
        perror("Can't create client socket");
        exit(EXIT_FAILURE);
    }

    // name the socket
    struct sockaddr_in address;
    memset(&address, 0, sizeof address);   // IMPORTANT!
    address.sin_family = AF_INET;
    inet_pton(AF_INET, LOCALHOST, &address.sin_addr);
    address.sin_port = htons(SERVER_PORT);

    // connect to the server
    if (connect(client, (struct sockaddr *) &address, sizeof address) == -1) {
        perror("Can't connect to server");
        exit(EXIT_FAILURE);
    }

    // tell the server what we want our username to be
    send_message('n', USERNAME);

    // setup fd_set for select
    fd_set inputs;
    FD_ZERO(&inputs);
    FD_SET(STDIN_FILENO, &inputs);
    FD_SET(client, &inputs);

    // Initialise all curses stuff and
    // do a one-time draw for things that shouldn't have to change
    init_windows();

    // Make sure current_word starts out with all null chars
    memset(current_word, 0, 9);

    // process input as long as it keeps coming
    for (;;) {
        // select may modify the fd set, so make a copy
        fd_set testfds = inputs;

        int sel_res = select(client + 1, &testfds, 0, 0, NULL);
        if (sel_res == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }
        else {
            if (FD_ISSET(client, &testfds)) {
                int read = recv(client, cmd_buffer, sizeof cmd_buffer, 0);
                int consumed = 0;
                char *buf = cmd_buffer;
                if (read == 0) {
                    // server has disconnected
                    endwin();
                    fprintf(stderr, "%s", "The server has closed.\n");
                    exit(EXIT_FAILURE);
                }
                while (consumed < read) {
                    consumed += strlen(buf) + 1;
                    parse_server_command(buf);
                    buf += consumed;
                }
            }
            if (FD_ISSET(STDIN_FILENO, &testfds)) {
                if (accept_user_input) {
                    process_user_input(getch());
                }
                else {
                    // only accept the quit commands
                    int in = getch();
                    if (in == '\x03' || in == '\x11') {
                        quit();
                    }
                }
            }
        }
    }

    endwin();
}

static void process_user_input(int ch) {
    int len = strlen(current_word);
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
                send_message('w', current_word);

                // reset word and clear the input area
                memset(current_word, 0, len);
                wmove(word_input, 1, 1);
                wclrtoeol(word_input);
                mvwaddch(word_input, 1, 59, '|');
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

                        // Clear input line and add new str
                        wmove(word_input, 1, 1);
                        wclrtoeol(word_input);
                        mvwaddch(word_input, 1, 59, '|');
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

static void update_base_word(char* cmd) {
    int x = 33;
    int cmd_ch;
    int rnd_word_ch = 0;
    round_letters = calloc(strlen(cmd), 1);
    for (cmd_ch=0; cmd_ch < strlen(cmd); cmd_ch++) {
        // if the char is supposed to be highlighted
        if (cmd[cmd_ch] == '.') {
            // user the reverse attribute and consume the '.'
            mvwaddch(round_info, 1, x, cmd[cmd_ch+1] | A_REVERSE);
            round_letters[rnd_word_ch] = cmd[cmd_ch+1];
            cmd_ch++;
        }
        else {
            mvwaddch(round_info, 1, x, cmd[cmd_ch]);
            round_letters[rnd_word_ch] = cmd[cmd_ch];
        }
        x += 2;
        rnd_word_ch++;
    }
    wrefresh(round_info);
}

static void update_word_list(char* cmd) {
    int word_len;
    int num_words;
    int column = 2;
    int offset = 0;
    bool wrapped = false;
    while(sscanf(cmd, "%i:%i,%n", &word_len, &num_words, &offset) > 0) {
        int i;
        int j;
        for(i=0; i <= num_words; i++) {
            if (!wrapped && i > MAX_WORD_LIST) {
                column += (word_len + 2);
                wrapped = true;
            }
            for(j=0; j < word_len; j++) {
                mvwaddch(puzzle_words, (i % MAX_WORD_LIST)+1, column+j, '_');
            }
        }
        column += (word_len+2);
        cmd += offset;
    }
    wrefresh(puzzle_words);
}

static void tell_username(char* cmd) {
}

static void update_player_list(char* cmd) {
    int player_num;
    int score;
    char* username = calloc(strlen(cmd), 1);
    int offset = 0;
    int y = 1;
    while(sscanf(cmd, "%i:%i:%[^,]%n", &player_num, &score, username, &offset) > 0) {
        wattron(rankings, COLOR_PAIR(player_num+1));
        mvwprintw(rankings, y, 1, "%s %5d", username, score);
        wattroff(rankings, COLOR_PAIR(player_num+1));
        cmd += offset;
        y++;
    }
    wrefresh(rankings);
    free(username);
}

static void update_time(char *cmd){
    mvwaddstr(round_info, 1, 75, "    ");
    mvwaddstr(round_info, 1, 75, cmd);
    wrefresh(round_info);
}

static void update_round_number(char *cmd){
    mvwaddstr(round_info, 1, 9, cmd);
    wrefresh(round_info);
}

static void parse_server_command(char* cmd) {
    char code;
    // know that it will never be longer than strlen(cmd)
    char* message = calloc(strlen(cmd), 1);

    sscanf(cmd, "%c%[^;^\n]", &code, message);

    switch(code) {
        case 'b':
            update_base_word(message);
            accept_user_input = true;
            break;
        case 'l':
            update_word_list(message);
            break;
        case 'n':
            tell_username(message);
            break;
        case 'p':
            update_player_list(message);
            break;
        case 't':
            update_time(message);
            break;
        case 'r':
            update_round_number(message);
            break;
        case '&':
            ring_bell();
            break;
        default:
            break;
    }

    free(message);
}

static void send_message(char code, char *word) {
    char *message;
    asprintf(&message, "%c%s;", code, word);
    write(client, message, strlen(message)+1);
}

static void ring_bell() {
    top_panel(my_panels[4]);
    update_panels();
    doupdate();
    usleep(100000);
    hide_panel(my_panels[4]);
    update_panels();
    doupdate();
}

static void init_windows() {
    // Do all of the curses initialization
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
    round_info = newwin(3, 80, 0, 0);
    rankings = newwin(23, 20, 2, 60);
    puzzle_words = newwin(21, 60, 2, 0);
    word_input = newwin(3, 60, 22, 0);
    prompt = newwin(5, 35, 8, 20);
    bell = newwin(6, 13, 8, 30);
    // Prompt
    mvwaddstr(prompt, 1, 1, " Are your sure you want to quit?");
    mvwaddstr(prompt, 2, 1, " Press y or Y to confirm.");
    mvwaddstr(prompt, 3, 1, " Press anything else to dismiss.");
    wrefresh(prompt);

    // Bell
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

    // Round Header
    mvwaddstr(round_info, 1, 2, "Round:");
    mvwaddstr(round_info, 1, 55, "Time Remaining:");
    wrefresh(round_info);

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

    // Initialize all color pairs for the possible players
    int i;
    for (i=1; i<11; i++) {
        init_pair(i, color_pairs[i-1][1], color_pairs[i-1][0]);
    }

    // Make sure the prompt and bell are hidden to start
    hide_panel(my_panels[4]);
    hide_panel(my_panels[0]);

    // Update the stacking order
    update_panels();

    // move the cursor to the beginning of the word_input box
    wmove(word_input, 1, 1);

    // Show it on the screen
    doupdate();
}

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
