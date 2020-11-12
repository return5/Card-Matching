/*----------------------------------------- program info------------------------------------------*/
	/* simple card matching game using the ncurses library.
	 * written by: github/return5
	 * licensed under GPL v.2
	 */ 

/*----------------------------------------- header files -----------------------------------------*/
#include <ncurses.h>  //ncurses stuff
#include <stdlib.h> //malloc
#include <string.h> //strncpy
#include <time.h> //time()
#include <unistd.h>//sleep() 

/*----------------------------------------- enums and typedefs -----------------------------------*/

enum Suites{Clubs=1,Spades=2,Diamonds=3,Hearts=4};
enum Match{YES,NO};

typedef struct card{
	int value,x,y;
	char *face,*alt_face;
	enum Suites suite;
	enum Match matched;
}card;

/*----------------------------------------- prototypes -------------------------------------------*/

int checkIfAllMatched(void);
int checkFaceMatch(const int choice_1,const int choice_2);
int checkSuiteMatch(const int choice_1, const int choice_2);
void freeTemp(card **const temp);
void checkCardsMatch(int *const choices);
void printCard(const int index);
int getCard(const int x, const int y);
void printDeck(void);
void setXY(void);
enum Suites getSuite(const int value);
void swapFace(const int index); 
char *getFace(const int value);
card* makeCard(const int index, card **const temp);
int checkIfUsed(const int limit, const int index_used,const int *const already_used);
void fillDeck(card **const temp);
void initDeck(void);
void makeColors(void);
void initScreen(void);
void checkInput(const int c, int *const i,int *const choices);
void flipDeckFaces(void);
void showFaces(void);
void runLoop(void);
void endGame(void);
int checkIfAllMatched(void);
int checkFaceMatch(const int choice_1,const int choice_2);
void printFace(const int index); 
void endGame(void);

/*----------------------------------------- const and global vars --------------------------------*/

static const size_t SIZE_CARD = sizeof(card);
static const size_t SIZE_CARD_PTR = sizeof(card*);
static time_t start_time; 
static time_t end_time;
static int moves = 0;  //number of times player tried to match two cards
static card **deck;
static WINDOW *game_win;

/*----------------------------------------- code -------------------------------------------------*/

//checks if al cards have been matched and if so, returns 1, otherwise return 0
int checkIfAllMatched(void) {
	for(int i = 0; i < 20; i++) {
		if(deck[i]->matched == NO) {
			return 0;
		}
	}
	return 1;
}

//checks if value of two cards match and if so, returns 1, otherwise return 0
int checkFaceMatch(const int choice_1,const int choice_2) {
	return (deck[choice_1]->value == deck[choice_2]->value) ? 1 :  0;
}

//checks if both cards have the same color and if so, returns 1, otherwise return 0
int checkSuiteMatch(const int choice_1, const int choice_2) {
	moves++; 
	//if both cards are same suite, consider it a match
	if(deck[choice_1]->suite == deck[choice_2]->suite) {
		return 1;	
	}
	//if both cards are clubs and spades or both are diamonds and hearts, consider it a match.
	switch(deck[choice_1]->suite + deck[choice_2]->suite) {
		case Clubs + Spades:  
		case Diamonds + Hearts: return 1;
		default: return 0;
	}		
	
}

//checks if cards match. if they match, swap enum value to 'matched', otherwise flip cards over to hide face value
void checkCardsMatch(int *const choices) {
	if(checkSuiteMatch(choices[0],choices[1]) == 1 && checkFaceMatch(choices[0],choices[1]) == 1) {
		deck[choices[0]]->matched = YES;
		deck[choices[1]]->matched = YES;
	}
	else {    //turn cards over to hide value and suite
		swapFace(choices[0]);
		swapFace(choices[1]);
		printFace(choices[0]);
		printFace(choices[1]);
	}
}

//print face value on card located at 'index' in deck
void printFace(const int index) {
	wattron(game_win,COLOR_PAIR(deck[index]->suite));  //print correct color for card based on suite
	mvwprintw(game_win,deck[index]->y+1,deck[index]->x+1,"%-2s",deck[index]->face); //upper left hand corner
	mvwprintw(game_win,deck[index]->y+4,deck[index]->x+4,"%-2s",deck[index]->face); // middle
	mvwprintw(game_win,deck[index]->y+7,deck[index]->x+1,"%-2s",deck[index]->face); //lower left hand corner
	mvwprintw(game_win,deck[index]->y+1,deck[index]->x+7,"%2s",deck[index]->face); //upper ight hand corner
	mvwprintw(game_win,deck[index]->y+7,deck[index]->x+7,"%2s",deck[index]->face); //lower right hand corner
	wattroff(game_win,COLOR_PAIR(deck[index]->suite)); //stop printing color
}

//prints outline of card at 'index' of deck
void printCard(const int index) {
	mvwprintw(game_win,deck[index]->y,deck[index]->x,"----------");  //top of card
	mvwprintw(game_win,deck[index]->y + 8,deck[index]->x,"----------");  //bottom of card
	for(int i = 1; i < 8; i++) {
		mvwprintw(game_win,deck[index]->y + i,deck[index]->x,"|"); //left hand side
		mvwprintw(game_win,deck[index]->y + i,deck[index]->x+9,"|"); //right hand side
	}
	printFace(index);
}

//swap face value and blank side to show/hide card when needed.
void swapFace(const int index) {
	char temp[4];
	strncpy(temp,deck[index]->face,4); //currently showing side saved to temp
	strncpy(deck[index]->face,deck[index]->alt_face,4); //currently showing side changed to other side
	strncpy(deck[index]->alt_face,temp,4); //previous side saved to alt_face
}

//if mouse clicked on card, return the index for that card to save in choices array located in 'gameloop' function
int getCard(const int x, const int y) {
	for(int i = 0; i < 20; i++) {	
		//if mouse clicked was between left hand side and right hand side of card
		if(deck[i]->x < x && deck[i]->x+10 > x) {
			//if mouse click was between upper and lower side of card
			if(deck[i]->y < y && deck[i]->y+10 > y) {
				//if the card hasnt already been matched
				if(deck[i]->matched != YES) {
					swapFace(i); //swap card over to show face
					printFace(i); //print it
					return i; //return index number
				}
			}
		}
	}
	
	return -1; //didnt click on card or it was already matched.
}

//print entire deck
void printDeck(void) {	
	for(int i = 0; i < 20; i++) {
		printCard(i); //print card stored at deck[i]
	}	
}

//set x and y values for each card so it aligns propery when printed to screen. x,y is the location of top left corner of card
void setXY(void) {
	for(int i = 0; i < 20; i++) {
		deck[i]->x = (i%5)*13 +1; //x values should be 0,14,27,40,53
		deck[i]->y = (i/5) * 10; //y values should 0,10,20,30
	}
}

//return the correct suite for the card
enum Suites getSuite(const int value) {
	switch(value) {
		case 1: return Hearts; 
			break;
		case 2: return Clubs;
			break;
		case 3: return Diamonds;
			break;
		case 4: return Spades;
			break;
		default: return Spades;
			break;
	}
}

//get face value for card adn return it
char *getFace(const int value) {
	char *c = malloc(4);
	switch(value) { 
		case 1: return "A"; 	
		case 2 ... 10: 
			snprintf(c,sizeof(int),"%d",value); //convert integet to string and store it inside c
		    return c; 	
		case 11: return "J";
		case 12: return "Q";
		case 13: return "K";	
		default: return "K";
			
	}
}

//gets rid of temp after it is no longer needed
void freeTemp(card **const temp) {
	for(int i = 0 ; i < 10; i++) {
		free(temp[i]);
	}
	free(temp);
}

//make a card for deck copying values from temp
card* makeCard(const int index, card **const temp) {
	card *new = malloc(SIZE_CARD);
	new->face = malloc(4);
	new->alt_face = malloc(4);
	new->value = temp[index]->value;
	new->matched = NO;
	new->suite = temp[index]->suite;
	strncpy(new->face,temp[index]->face,4);  //copy face value
	strncpy(new->alt_face,temp[index]->alt_face,4); //copy alt_face value
	return new;
}

//checks if index value has alread been used to index something inside deck array. makes sure not trying to store two cards in same index location
int checkIfUsed(const int limit, const int index_used,const int *const already_used) {
	for (int i = 0; i < limit; i++) {
		if(index_used == already_used[i]) { //if index has already been used, return 1
			return 1;
		} 
	}
	return 0; //no match then return 0
}

//make and fill up deck array with cards from temp array, store each card in a random index inside deck
void fillDeck(card **const temp) {
	deck = malloc(SIZE_CARD_PTR * 20);
	int i; //used as index to access deck
	int index = 0; //used as index for temp
	int j = 0;  //used as index for already_used
	int already_used[20];  //hold list of index values used for temp
	while(j < 20) {
		i = random()%20; //get random int betwen 0-19. if not on linux you may need to repalce random() with another function
		if(!checkIfUsed(j,i,already_used)) {
			index = j%10;  //index fluctuates between 0 and 9, each card is used twice
			deck[i] = makeCard(index,temp);
			already_used[j++] = i;
		}
	}
}

//initialize deck of cards
void initDeck(void) {
	card **temp = malloc(SIZE_CARD_PTR * 10);
	srandom(time(NULL)); //seed random. if not on linux you may need an alternative to random() and srandom()
	//make first half of deck. the other half is a copy of it
	for(int i = 0; i < 10; i++) { 
		temp[i] = malloc(SIZE_CARD);	
		temp[i]->value = (random() % 13) +1; //random int between 1 and 13
		temp[i]->alt_face = malloc(4);
		temp[i]->face = malloc(4);
		snprintf(temp[i]->face,4,getFace(temp[i]->value));
		snprintf(temp[i]->alt_face,4," ");		
		temp[i]->suite = getSuite((random()%4)+1); //call getSuite with a random int between 1 and 4
		temp[i]->matched = NO;
	}
	fillDeck(temp);
	freeTemp(temp);
}

//sets up ncurses colors
void makeColors(void) {
	start_color();
	init_pair(Clubs,COLOR_YELLOW,COLOR_BLACK);
	init_pair(Spades,COLOR_YELLOW,COLOR_BLACK);
	init_pair(Hearts,COLOR_GREEN,COLOR_BLACK);
	init_pair(Diamonds,COLOR_GREEN,COLOR_BLACK);
}

//initializes ncruses
void initScreen(void) {
	initscr(); //start ncuurses
	noecho(); //dont show user input on screen
	cbreak(); //disable line buffering
	curs_set(0); //hide cursor
	keypad(stdscr,TRUE); //enable keypad. needed for mouse
	mousemask(ALL_MOUSE_EVENTS,NULL); //enable mouse input
	game_win = newwin(40,65,0,0); //make game_win (height,length,start y, start x))
	refresh(); //refresh screen
}

//check user input
void checkInput(const int c, int *const i,int *const choices) {
	MEVENT event; //mouse event struct
	switch(c) {
		case KEY_MOUSE:
			if(getmouse(&event) == OK) { 
				if(event.bstate & BUTTON1_CLICKED){ //if left mouse button was clicked
					int choice = getCard(event.x,event.y); //get card user clicked on, if they did click on one
					if(choice >= 0) { //if they did click on one
						choices[(*i)++] = choice; //store index of card inside choices and incr i by one
					}
					wrefresh(game_win);
				}
			}
			break;
		default : //do nothing
			break;
	}
}

//flip all cards over to hide face value
void flipDeckFaces(void) {
	for(int i = 0; i < 20; i++) {
		swapFace(i);
		printCard(i);
	}
}

//count down from 10 while showing face of all cards, then flip cards over and play game
void showFaces(void) {
	WINDOW *timer_win = newwin(3,30,2,70); //display timer in a seperate window to right of game_win
	mvwprintw(timer_win,0,0,"time till cards are hidden:");
	for(int i = 0; i< 10; i++) { 
		mvwprintw(timer_win,1,9,"%2d", 10-i); 
		wrefresh(timer_win);
		sleep(1); //sleep for 1 second 
	}
	delwin(timer_win); //done with timer_win so delete it
	clear(); //clear entire screen
	flipDeckFaces();
	refresh();
	wrefresh(game_win);
}

//game loop. run until either all cards are matched or user press key 'q'
void runLoop(void) {
	int c,i;
	int choices [2]; //holds index of two cards user is trying to match
	c = i = 0; //index for choices
	start_time = time(0); //time when user starts to play
	while(c != 'q') {
		checkInput((c = getch()), &i,choices); //check user input
		if(i > 1) { 
			i = 0;
			checkCardsMatch(choices);
			if(checkIfAllMatched()) { //if user matches all cards break out of loop
				break;
			}
		}
	}
	end_time = time(0); //time when user stopped playing
	if(c !='q') { //is user didnt press 'q' but won by matching all cards
		endGame();
	}
}

//if user won the game display a message
void endGame(void) {
	wclear(game_win);
	mvwprintw(game_win,10,10,"you won! it took you %d seconds and %d moves to finish.",end_time - start_time,moves);
	wrefresh(game_win);
	getch();
}

int main(void) {
	initScreen();
	initDeck();
	makeColors();
	setXY();
	printDeck();	
	wrefresh(game_win);
	showFaces();
	runLoop();
	endwin(); //end ncurses control of terminal
	return 0;
}
