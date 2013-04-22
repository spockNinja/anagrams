#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <curses.h>
#include <panel.h>

#define DEBUG_CLIENT_VIEW 1

#define COLUMNS 80
#define ROWS 25


/*  This lists the start and end coordinates for the main sections as defined
in the specification, section 4.1.
There are four sections, and each section, as defined here, has a start
and end coordinate. The values are ordered as:
	startX, startY, numLines, numColumns 
*/	
int coords[5][4] = {
	{0,  0, 4, 80},   // section 1: 4 lines, 80 columns
	{59, 3, 21, 21},  // section 2: 21 lines, 20 columsn
	{0,  3, 19, 60},  // section 3: 19 lines, 60 columns
	{0, 21, 3, 60},   // section 4: 2 lines, 60 columns
    {30, 8, 6, 20}    // confirm dialog
};

struct client_section {
    char* label;
    int startX;
    int startY;
    int numLines;
    int NumColumns;
};

struct client_section round_info = { "", 0, 0, 4, 80};


#if DEBUG_CLIENT_VIEW > 0
static void finish(int sig);

int main(){
	(void) signal(SIGINT, finish);

	WINDOW *my_wins[5];
	PANEL  *my_panels[5];
	int lines = 10, cols = 40, y = 2, x = 4, i;
    int ch;


	initscr();
	cbreak();
	noecho();

	if (!has_colors()) {
		fprintf(stderr, "No color support on this terminal\n");
		exit(1);
	}
	if (start_color() != OK) {
		endwin();
		fprintf(stderr, "Error - could not initialize colors\n");
		exit(2);
	}

	/* Create windows for the panels */
	my_wins[0] = newwin(coords[0][2], coords[0][3], coords[0][1], coords[0][0]);
	my_wins[1] = newwin(coords[1][2], coords[1][3], coords[1][1], coords[1][0]);
	my_wins[2] = newwin(coords[2][2], coords[2][3], coords[2][1], coords[2][0]);
	my_wins[3] = newwin(coords[3][2], coords[3][3], coords[3][1], coords[3][0]);
	my_wins[4] = newwin(coords[4][2], coords[4][3], coords[4][1], coords[4][0]);

	/* 
	 * Create borders around the windows so that you can see the effect
	 * of panels
	 */
	for(i = 0; i < 5; ++i)
		box(my_wins[i], 0, 0);

	/* Attach a panel to each window */ 	/* Order is bottom up */
	my_panels[0] = new_panel(my_wins[0]); 	/* Push 0, order: stdscr-0 */
	my_panels[1] = new_panel(my_wins[1]); 	/* Push 1, order: stdscr-0-1 */
	my_panels[2] = new_panel(my_wins[2]); 	/* Push 2, order: stdscr-0-1-2 */
	my_panels[3] = new_panel(my_wins[3]); 	/* Push 3, order: stdscr-0-1-3 */
	my_panels[4] = new_panel(my_wins[4]); 	/* Push 3, order: stdscr-0-1-3 */

	/* Update the stacking order. my_panels[2] will be on top */
	update_panels();

	/* Show it on the screen */
	doupdate();

    while ((ch = getch()) != KEY_F(1)){
        if (ch >= 'a' && ch <='z'){
            
        }
    }
	
	getch();
	endwin();
}


static void finish(int sig)
{
    endwin();

    /* do your non-curses wrapup here */

    exit(0);
}


#endif
