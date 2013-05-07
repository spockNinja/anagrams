# CSC 460 Team Project — Anagrams — Spring 2013

See the [problem specification](http://courses.missouristate.edu/EricShade/460/anagrams.html) for details.

(copied here for historical purposes, as last posed by Dr. Eric Shade, Tue Apr 30 13:58:39 CDT 2013)



# 1 Overview

This is a multiplayer, networked, real-time anagram game. Each game consists of one or more rounds. On each round, the server randomly picks a word from the dictionary and the letters are displayed in sorted order. Blank entries of the form `-----` (for a five-letter word) are displayed for all words that can be made from those letters. The entries are sorted from shortest to longest, and entries of the same length are in alphabetical order. Each round is timed. Players try to find as many words as they can until either all words are found or the timer expires. When a word is found, it is displayed in the correct location on the screen using a distinct color for each player. There are bonuses for being the first to find a maximum-length word, for finding the last word in the round, and for finding words that use the least frequently occurring letter within that round. At the end of each round, players are ranked by their scores.

# 2 The server

## 2.1 Starting the server

Run

&gt; `./server` *port* [*rounds*]

If the number of rounds is not specified, use a default of 5. Clients have 30 seconds to join a game. If no clients join, then the server must exit. The maximum number of players is 10.

## 2.2 Accepting client connections

The server must always accept incoming client connections. If the game has already started, the server must send a suitable response and deny the connection. Otherwise, the server must verify that the username is unique and append a digit if necessary to avoid a collision. For example, if three clients join, all with the username *joe*, then the second and third to join must be assigned the names *joe1* and *joe2*. The server must send the possibly-modified username and seconds until game start back to the client.

## 2.3 Generating puzzles

Read the entire dictionary from `/460/words` into memory, convert all words to uppercase, then create a second array that is a copy of the in-memory dictionary except that the letters in each word are sorted in alphabetical order. Do this *before* starting the 30-second connection timer.

To generate a puzzle, select a word of length 8 or more at random, then sum the lengths of all the words that can be made from those letters. You must select a word such that the entries fill between 50% and 100% of the available space. (If the first word you pick doesn’t work, randomly choose a different one.) You must not select the same word twice in the same game.

You must use a linear-time algorithm to find all the words. If you sort the letters in the target word, and use the sorted-letter words instead of the original words, it’s easy to do. Then it’s just a matter of implementing a function that takes two strings of sorted letters and tells whether the first can be spelled with the second. Note that you must correctly account for duplicate letters.

# 3 The Client

## 3.1 Joining a game

Run

&gt; `./client` *port* [*username*]

If *username* is not specified, use the user’s login name. If the game has already started, the client must display a suitable message and exit. If not, the client must display the username returned from the server (which may differ from *username* in case of a name collision), and a timer that counts down the seconds until the game starts.

## 3.2 Client keystrokes

`a`–`z`, `A`–`Z`

If the letter is valid for the current round, append the uppercase form of the letter to the word. If the word is already of the maximum length for the current round, or the letter is not valid for this round, ignore the letter and ring the bell. Pressing a letter key will never send the word to the server, even if the word is of maximum length: the user must press `ENTER` to send a word. 

`BKSP` or `Ctrl-H` (`'b'`), `DEL` (`'x7f'`)

Delete the last letter in the current word. If the current word is empty, ring the bell. 

`ENTER` or `Ctrl-J` (`'n'`), `RETURN` or `Ctrl-M` (`'r'`)

Send the word to the server. If no letters have been entered, behave as if the `SPACE` key were pressed. 

`SPACE` (`' '`), `TAB` or `Ctrl-I` (`'t'`)

Complete the current word. Select the most recent word entered by the user that begins with the letters already in the word; ring the bell if no previous words have been entered in this round. For example, if the user had entered APPLE, ALE, APPLET, and PLEAT, typing `A`, `P`, `SPACE` would make the current word APPLET. If no letters have been entered, make the current word equal to the last word entered. Completing a word does *not* send it to the server. The client must remember at least the last 100 words entered in the current round. Clear the history at the beginning of each round. 

`Ctrl-C` (`'x03'`), `Ctrl-Q` (`'x11'`)

Pop up a window asking the user if she wants to quit. If she confirms the choice, quit the game, and otherwise return to the game. Note that you have to set curses to raw mode to get these keystrokes. 

Any other key

Ring the bell. 

## 3.3 "Ringing the bell"

Printing the “alert” character `'a'` will usually make the terminal beep. But this is annoying to other users, and on some terminals takes a surprisingly long amount of time. Most people who use a terminal frequently prefer to use a “visual bell”, which is a window that pops up in the middle of the screen for a fraction of a second, usually displaying an ASCII picture of a bell. In this spec, “ringing the bell” means displaying a visual bell in the middle of the screen for 0.1 seconds. You must implement this yourself, because you can’t assume that a user’s terminal supports visual bells or has that setting enabled.

# 4 Gameplay

## 4.1 The Screen

You may assume that the screen is at least 80x25 (eighty columns and twenty-five rows). You can either use the actual size of the screen as reported by curses, or you can just use 80x25 regardless of how large the screen is. You can assume that clients will not resize their window while the game is playing (usually results in a `SIGWINCH` signal).

There are four sections of the screen:

1.  A top bar that shows the round number, the letters in the puzzle in alphabetical order separated by spaces, the least frequently occurring letter in the round (ties broken in reverse alphabetical order), and the timer showing the time left in the round.
2.  A score table running down the right side of the screen that shows the players’ names and scores in a unique color.
3.  The words, sorted first by length and then by dictionary order. The words may be in columns going down the screen or in rows going across the screen. Use dashes to indicate the letters in the word. Horizontally adjacent words must be separated by at least one space.
4.  The word entry area at the bottom of the screen.

When a word is found, it is filled in with the color of the player who found it.

On the first round the players are listed in alphabetical order by name. On the second and subsequent rounds, the players are listed in order of their rank based on the start of that round. Do not shuffle the player positions based on their scores *during* a round.

## 4.2 Colors

You have 256 colors to play with. Use them wisely, and avoid color combinations that are hard to read. Each player must have a distinct color, and you must also use color to highlight bonus words and letters. One approach is to (1) use a black background for normal words, a gray background for bonus words, and a different-color background for bonus letters; (2) use different background colors for the top bar and the word entry area; and (3) use a column of `|` characters to separate the word area of the screen from the scoring table.

## 4.3 Scoring

Words score as follows:

    Length:  2  3  4  5  6  7  8  9 10 11 12 13
    Points:  2  3  5  8 12 17 22 28 35 43 52 95

Bonus points are awarded as follows:

*   The first player to find a maximum-length word scores a 100-point bonus.
*   The player who finds the last word in the round scores a 50-point bonus; this bonus applies whether or not all words are found.
*   Words containing the least frequently occurring letter in the round have a nonus of five points per occurrence of that letter.

Bonus words and letters must have special, distinct highlighting in addition to the player’s color.

All scores must be updated in real time.

## 4.4 Rounds

A round is one second per letter. The round ends when the timer expires or all the words have been found. At the end of each round, the server must send a notification to the client to display an `END OF ROUND` message in the word-entry area. This allows players to look at all the words.

After 15 seconds, the server must send a score update. The client clears the screen and displays a player ranking, sent from the server, for ten seconds:

    01. frosty    14878 %2B 180 = 14978
    02. xyz       12340 %2B 250 = 12590
    03. juliet    12500 %2B  90 = 12590
    04. raul      10763 %2B 320 = 11083
    05. skronk     8125 %2B   5 =  8130
    06. linus       467 %2B   0 =   467 DNF

The first component of the score counts only the word values; the second counts only the bonuses. If two or more players have the same score, the one with the most bonus points wins. If there is still a tie, the player who found a word most recently wins.

If a player quit the game, indicate it with `DNF` (for Did Not Finish).

## 4.5 End of game

After the last round the game is over. Clear the screen, display the final standings, and exit the client (leaving the standings visible on the screen).

# 5 Architecture

Use a smart multithreaded server with dumb single-threaded clients. Neither the server nor the clients may busy-wait for any reason.

Clients use *select* to wait for input from the keyboard and the server. They send completed words and quit commands to the server, and update the screen based on commands received from the server. Clients don’t know the dictionary, the word list, the other player’s names, the scores, etc. Clients just display whatever the server tells them to.

The server will typically need one main thread, one thread that handles incoming connections (even after the game has started), and one thread per client. There are several different implementation strategies; pick the one that makes the most sense to your team.


