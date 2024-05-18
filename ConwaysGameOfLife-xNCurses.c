#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //sleep
#include <Windows.h> //windows commands on all devices, useful for sytem("cls")
#include <string.h>
#include <ncurses/curses.h> //library that allows to further modify the console

#define GRID_HEIGHT 60
#define GRID_WIDTH 125
#define SPEED 50

#define BACKROUND ' '
#define CELL '#'

typedef enum {
    DEAD,
    ALIVE,
} State;  //alow my cells to have a state (makes updating them easier)

typedef struct{
    char symbol;
    State state;
} Cell;  // defining cells

typedef struct Coordinates{
    int x;
    int y;
    struct Coordinates * next;
}Cords;  //defining cooridnate structure

void initGrid(Cell grid[GRID_HEIGHT][GRID_WIDTH], int random, Cords * head);  //initializes all grid cell values to be zero except if random is set to 1
void printGrid(Cell grid[GRID_HEIGHT][GRID_WIDTH], WINDOW *wn);  // prints out the matrix of cells on the board
void copyMatrix(Cell original[GRID_HEIGHT][GRID_WIDTH], Cell tbCopied[GRID_HEIGHT][GRID_WIDTH]); //copies one matrix to another as a means to save and transfer data

void update_matrix(Cell display1[GRID_HEIGHT][GRID_WIDTH], Cell display2[GRID_HEIGHT][GRID_WIDTH]); //checks the rules and moves to the next generation

char * inputs(size_t size, char special); //unlimited letter input
void read_cords_from_file(char * filename, Cords ** head_ofCordList); // allows file to be read so that the coordinate of alive cells to be pointed
Cords * initNode(); // iniatializes the nodes of linked lsit

int main(void) {
    // setting up variables
    int seed, choice, counter = 0;
    Cell display1[GRID_HEIGHT][GRID_WIDTH];
    Cell display2[GRID_HEIGHT][GRID_WIDTH];
    char * filename = NULL;
    system("cls");

    initscr();  // from here on out the console is a screen
    refresh();

    WINDOW * board_win = newwin(GRID_HEIGHT+1, GRID_WIDTH+1, 0, GRID_WIDTH/2); //the box the data gets displayed in
    box(board_win, 0, 0); //its border
    wrefresh(board_win);  // refreshing the box

    //beginning introduction
    printw("Enter a seed for the random Generator: "); //printw is like printf. but it writes on the screen
    box(board_win, 0, 0);
    refresh();                                           // the screen needs to be refreshed every time it is written on and the box needs to be reapplied
    wrefresh(board_win);
    scanw("%d", &seed);                                 // same as printf and printw
    srand(seed);                                       

    printw("Press 1 for random cell placement\nPress 2 to load a programme\nchoice: ");
    box(board_win, 0, 0);
    refresh();
    wrefresh(board_win);
    scanw("%d", &choice);
    // finished introduction

    while(1) {
        if (choice == 1) {
            if (counter == 0) {  //for the first time
                initGrid(display1, 1, NULL); //initialize the grid
                curs_set(0);            //make the cursor invisivle
            }
        } 
        if (choice == 2) { //make input available (maybe python program?)
            if (counter == 0) {
            Cords * head = initNode();  // create the head of the linked list
            
            printw("what file would you like to load?\nfile: ");
            box(board_win,0,0);
            refresh();
            wrefresh(board_win);
            filename = inputs(10, '\n');  //give filename
            refresh();

            read_cords_from_file(filename, &head); //open said file

            initGrid(display1, 0, head);
            curs_set(0);
            }
        } 
        counter++;
        if (choice!=1 && choice!=2) { //if the user doesnt select something allowed
            printw("Invalid entry\n");  //tell them
            printw("Press 1 for random cell placement\nPress 2 to load a programme\nchoice: ");  //re-tell them the options
            box(board_win, 0, 0);
            refresh();
            wrefresh(board_win);
            scanw("%d", &choice);
            counter = 0;
        }
        copyMatrix(display1, display2); //copy the first to the second matrix
        usleep(SPEED*1000);                     // sleep for speed*1000 microseconds
        printGrid(display1, board_win);         // print on the board the matrix
        update_matrix(display1, display2);      // apply the rules
    }

    endwin(); //unmake the consol a screen

    return 0;
}

void initGrid(Cell grid[GRID_HEIGHT][GRID_WIDTH], int random, Cords * head) { //initialized the grid
    int randnum;
    Cords * temp = head;
    int checker = 0;

    if(random==0 && head!=NULL){  // if the user hasnt selected random and a node is given
        for (int y = 0; y<GRID_HEIGHT; y++){
            for (int x = 0; x<GRID_WIDTH; x++){
                while(temp->next!=NULL) {  // for all nodes
                    if (temp->x==x && temp->y==y){ //if said node has the same coordianes as the current cell
                        grid[y][x].symbol = CELL;
                        grid[y][x].state = ALIVE;  
                        temp = head;
                        checker++;  // reset the list and exit the while loop
                        break;
                    } else {
                        temp = temp->next;  // check the next node if the prev wasnt the cell
                    }
                }
                temp = head;  // reset the list regardless
                if (checker==0) {  // if a cell didn't corespond to the coordinates
                    grid[y][x].symbol = BACKROUND;
                    grid[y][x].state = DEAD;        // set the cell at said cordinates as dead
                } else {
                    checker = 0;  //if it was alive reset checker
                }
            }
        }
        return;
    }

    for(int i = 0; i<GRID_HEIGHT; i++) for(int x = 0; x<GRID_WIDTH; x++) {
        randnum = rand()%10;
        if (random!=0 && randnum == 0) {  //if the user has selected the random setting then there is a 1 in 10 change for a cell to be alive
            grid[i][x].symbol = CELL;
            grid[i][x].state = ALIVE;
        }else {                            // else the cell is dead
            grid[i][x].symbol = BACKROUND;
            grid[i][x].state = DEAD;
        }
    }   
    return;
}

void printGrid(Cell grid[GRID_HEIGHT][GRID_WIDTH], WINDOW * wn) { //prints the things that are in the grid
    for(int i = 0; i<GRID_HEIGHT; i++) {
        for(int x = 0; x<GRID_WIDTH; x++) {
            wprintw(wn, "%c", grid[i][x].symbol);
        }
        wprintw(wn, "\n");
    } //prints the whole board (its invisible)
    box(wn, 0, 0);
    wrefresh(wn);
    werase(wn); //erase everything from the box
    return;
}

void copyMatrix(Cell original[GRID_HEIGHT][GRID_WIDTH], Cell tbCopied[GRID_HEIGHT][GRID_WIDTH]) { // copies one grid to another
    for(int y = 0; y<GRID_HEIGHT; y++) {
        for(int x = 0; x<GRID_WIDTH; x++) {
            tbCopied[y][x].symbol = original[y][x].symbol;
            tbCopied[y][x].state = original[y][x].state;  //copies both symbol and state
        }
    }
    return;
}

void update_matrix(Cell display1[GRID_HEIGHT][GRID_WIDTH], Cell display2[GRID_HEIGHT][GRID_WIDTH]) {  //applies the rules
    int number_of_alive_cells;

    for(int y = 0; y<GRID_HEIGHT; y++) {
        for(int x = 0; x<GRID_WIDTH; x++) {
            number_of_alive_cells = 0;

            if (y-1>0 && y-1<GRID_HEIGHT && x-1>0 && x-1<GRID_WIDTH && display2[y-1][x-1].state == ALIVE) number_of_alive_cells++;
            if (y-1>0 && y-1<GRID_HEIGHT && x>0 && x<GRID_WIDTH && display2[y-1][x].state == ALIVE) number_of_alive_cells++;
            if (y-1>0 && y-1<GRID_HEIGHT && x+1>0 && x+1<GRID_WIDTH && display2[y-1][x+1].state == ALIVE) number_of_alive_cells++;
            if (y>0 && y<GRID_HEIGHT && x-1>0 && x-1<GRID_WIDTH && display2[y][x-1].state == ALIVE) number_of_alive_cells++;
            if (y>0 && y<GRID_HEIGHT && x+1>0 && x+1<GRID_WIDTH && display2[y][x+1].state == ALIVE) number_of_alive_cells++;
            if (y+1>0 && y+1<GRID_HEIGHT && x-1>0 && x-1<GRID_WIDTH && display2[y+1][x-1].state == ALIVE) number_of_alive_cells++;
            if (y+1>0 && y+1<GRID_HEIGHT && x>0 && x<GRID_WIDTH && display2[y+1][x].state == ALIVE) number_of_alive_cells++;
            if (y+1>0 && y+1<GRID_HEIGHT && x+1>0 && x+1<GRID_WIDTH && display2[y+1][x+1].state == ALIVE) number_of_alive_cells++;  //checks all the cells around the targeted cell

            if (display2[y][x].state == ALIVE) {  //rules
                if (number_of_alive_cells<2) {  //if a cell is alive and it has less than 2 alive neighbors it dies, as of underpopulation
                    display1[y][x].symbol = BACKROUND;
                    display1[y][x].state = DEAD;  //if a cell is alive and has 2 or 3 alive neightbors it remains alive
                }
                if (number_of_alive_cells>3) { //if a cell is alice and it has over 3 alive neighbors it dies, as of ovepopulation
                    display1[y][x].symbol = BACKROUND;
                    display1[y][x].state = DEAD;
                }
            } else if (display2[y][x].state == DEAD) { //if a cell is dead and has EXACTLY 3 alive neightbors then it becomes alive, as in through reproduction
                if (number_of_alive_cells==3) {
                    display1[y][x].symbol = CELL;
                    display1[y][x].state = ALIVE;
                }
            }
        }
    }

    return;
}

char * inputs(size_t size, char special) {
    char * str = NULL;
    char ch;
    size_t len = 0;
    str = realloc(str, sizeof(char)*size); // give a starting size
    if (!str) return str;
    while (EOF!=(ch=getch()) && ch != special) {  //getch is part of the ncurses lib. it takes one char input from stdin
        str[len++] = ch;
        if (len==size) { //if len exceeds the size give to begin with
            str = realloc(str, sizeof(char)*(size += 16)); //extend that size by 16 bytes
            if (!str) return str;
        }
    }
    str[len++] = '\0'; //make the last char NULL

    return realloc(str, sizeof(char)*len); //return that pointer
}

Cords * initNode() { //typical node initialization function
    Cords * node;
    node = malloc(sizeof(Cords));
    node->x = 0;
    node->y = 0;
    node->next = NULL;
    return node;
}

void read_cords_from_file(char * filename, Cords ** head_ofCordList) {
    char filedir[] = "./Programmes/"; //ad the extension of current directory
    int filenameLen = strlen(filename);  // get length of filename

    char * name = (char*) malloc((13+filenameLen+4+1)*sizeof(char));  //create an apropriately sized space on the heap for full directory
    name = strcat(filedir, filename);
    name = strcat(name, ".gol");

    FILE * file = fopen(name, "r");
    if (file == NULL) {
        printw("file not found\nno actions taken");
        refresh();
        return;
    }

    char * token = calloc(3, sizeof(char));
    Cords * temp = *head_ofCordList;
    char setOfCords[5];
    char * dummy;

    int eof_checker = fscanf(file, "%s", setOfCords);
    while (eof_checker==1){
        token = strtok(setOfCords, ","); //strtok is a function that belong in the string library it read the string untill the specified part of the string appears
        temp->x = strtol(token, &dummy, 10); //strtol is part of the string library it takes the string and with the help of a pointer converts it to the desired number base <36

        token = strtok(NULL, ","); //can be used again on the same string by using null all the times after the first
        temp->y = strtol(token, &dummy, 10);

        temp->next = initNode();
        temp = temp->next;

        eof_checker = fscanf(file, "%s", setOfCords);
    }
    
    return;
}
