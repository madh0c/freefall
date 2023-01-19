// Author: Jeffery Pan (z5310210)
// Date: 7/7/20
// UNSW Freefall assignment.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SIZE 15
#define EMPTY 0
#define STONE 1
#define MOVING 2



void printMap(int map[SIZE][SIZE], int playerX);
void place_block(int array[SIZE][SIZE]);
void move_player(int *player);
void fire_laser(int array[SIZE][SIZE], int *player);
int shift_down(int array[SIZE][SIZE]);
int check_if_won(int array[SIZE][SIZE]);
void vertical_flip(int array[SIZE][SIZE]);
void tnt_explosion(int array[SIZE][SIZE], double radius, int row_centre, int col_centre);


int main (void) {
    // This line creates our 2D array called "map" and sets all
    // of the blocks in the map to EMPTY.
    int map[SIZE][SIZE] = {EMPTY};

    // This line creates out playerX variable. The player starts in the
    // middle of the map, at position 7.
    int playerX = SIZE / 2;
     
    // setting up then printing the map (Stage 1 P1)  
    place_block(map);
    printMap(map, playerX);

    // The "main controller" loop
    // first integer input after 1st printMap 
    // will determine what the player does.      
    int command;
    int uses_left = 1; // to ensure one-time use of command 4
    while (scanf("%i", &command) != -1) { //loop continues as long as input!=EOF
        
        // pointer to give later functions access to the int playerX itself
        int *player = &playerX; 
        
        // 1: movement command (S1 P2)        
        if (command == 1) {
            move_player(player); 
            printMap(map, playerX);
        }
        
        // 2: firing the laser command (S1 P3) 
        if (command == 2) {
            fire_laser(map, player);
            printMap(map, playerX);   
            int pass = check_if_won(map);
            if (pass == 1) { // win condition (S2 P2)
                printf ("Game Won!\n");
                return 0;
            }       
        }
        
        // 3: shift stones down command (S2 P1) 
        if (command == 3) {
            int fail = shift_down(map);
            if (fail == 1) { // lose condition (S2 P2)
                printMap(map, playerX);
                printf ("Game Lost!\n");
                return 0;
            }
            printMap(map, playerX);
        }
        
        // 4: single-use vertical map flip (S3 P1)        
        if (command == 4) {     
            if (uses_left == 1) { // checks if flip has been used b4
                vertical_flip(map); 
            }
            printMap(map, playerX); 
            uses_left = 0;            
        }
    }    
    
    return 0;
}

// FUNCTIONS

// prints the map
void printMap(int map[SIZE][SIZE], int playerX) {
    
    // Print values from the map array.
    int i = 0;
    while (i < SIZE) {
        int j = 0;
        while (j < SIZE) {
            printf("%d ", map[i][j]);
            j++;
        }
        printf("\n");
        i++;
    }    
    
    // Print the player line.
    i = 0;
    while (i < playerX) {
        printf("  ");
        i++;
    }
    printf("P\n");
}

// Stage 1: P1
// allows user to setup the currently empty map
void place_block(int array[SIZE][SIZE]) {
    
    // scans in lines of stone required
    int line_stones;
    printf("How many lines of stone? ");
    scanf("%i", &line_stones);
    printf("Enter lines of stone:\n");
    
    // loop to scan in and execute 4-variable commands for placing stones 
    int line_counter = 0;    
    int stone_counter = 0;
    int row, col, length, block_type;
    while (line_counter < line_stones) {
        scanf("%i %i %i %i", &row, &col, &length, &block_type);
        if (// conditions to ensure input is valid
        col <= SIZE && row <= SIZE && length <= SIZE + 1  
        && col >= 0 && row >= 0 && length >= 0
        ) {
            stone_counter = 0;
            while (stone_counter < length) { 
                // blocks will be placed in a row at the specified length
                array[row][col + stone_counter] = block_type;
                stone_counter++;
            }
        }
        line_counter++;
    }
}

// Stage 1: P2 - movement
// if second integer user enters is 1, player  
// moves right if -1, player moves left.
// if user enters invalid input or tries to 
// go out of bounds then nothing happens.
void move_player(int *player) {
    int direction;
    scanf("%i", &direction);
    if (abs(direction) == 1) {
        // movement at the boundaries
        if (*player == SIZE - 1 && direction < 0) {
            *player += direction;
        }  
        if (*player == 0 && direction == 1) {
            *player += direction;
        }
        // movement within the boundaries
        if (*player > 0 && *player < SIZE - 1) { 
            *player += direction;
        }
    }         
}

// Stage 1: P3 - firing a laser that breaks a max of 3 stones above the player
void fire_laser(int array[SIZE][SIZE], int *player) {
    int laser_counter = 0;
    int break3stones = 0; // ensure laser can only break 3 stones max.
    break3stones = 0;
    while (laser_counter < SIZE && break3stones < 3) {
        if (array[SIZE - laser_counter - 1][*player] == STONE) {// break stone
            array[SIZE - laser_counter - 1][*player] = EMPTY;
            break3stones++;
        } else if (array[SIZE - laser_counter - 1][*player] != EMPTY) {// TNT
            // radius variable for tnt_explosion function
            double radius = array[SIZE - laser_counter - 1][*player];
            
            // 2 "coord" integers to define the centre of TNT
            int row_centre = SIZE - laser_counter - 1;
            int col_centre = *player;
            
            // explosion itself 
            tnt_explosion(array, radius, row_centre, col_centre);
            laser_counter = SIZE; // stop laser beam after explosion
        
        }
        laser_counter++;
    }          
} 

// Stage 2 - shift all stones down & lose conditions
int shift_down (int array[SIZE][SIZE]) {
    int row = 1; // check from second last row ([SIZE - row - 1] = [13] rn)
    int col = 0;
    
    // loop: if stones / tnt on row [14] when shift_down happens, then it's gg
    while (col < SIZE)  {
        if (
        array[SIZE - row][col] != EMPTY && 
        array[SIZE - row][col] != MOVING //
        ) {                    
            return 1;
        }
        col++;
    }
    
    // loop to shift stones down   
    while (SIZE - row - 1 >= 0) {// scanning from bottom to top 
        col = 0;
        while (col < SIZE) {
            if (
            array[SIZE - row - 1][col] != EMPTY
            && array[SIZE - row - 1][col] != MOVING
            ) { 
                int tmp = array[SIZE - row - 1][col]; // temp value 
                array[SIZE - row - 1][col] = EMPTY;
                array[SIZE - row][col] = tmp;
            }        
            col++;
        } 
        row++;
    }
    return 0; 
}

// Stage 2: win conditions
int check_if_won(int array[SIZE][SIZE]) {
    int row = 0;
    int col = 0;
    int blocks_left = 0; 
    
    // loops thru entire map array
    while (row < SIZE) {// if loop encounters block, then blocks_left gets + 1
        col = 0;
        while (col < SIZE) {
            if (array[row][col] != EMPTY) {
                blocks_left++;
            }         
            col++;
        }
        row++;
    }
    if (blocks_left == 0) {
        return 1;
    } else {
        return 0;
    }
    
}
// Stage 3 - vertical flip
void vertical_flip(int array[SIZE][SIZE]) {
    int row = 0;
    int col = 0;
    while (row < (SIZE / 2)) { // loops thru only the top half of map
        col = 0;
        while (col < SIZE) {
            // if current block and flip destination block 
            // are different then proceed with flip.
            if (array[row][col] != array[SIZE - row - 1][col]) {
                int tmp; // temporary int to store value for the switch
                
                // switch block with flip destination block
                // to emulate a vertical flip.
                tmp = array[row][col];
                array[row][col] = array[SIZE - row - 1][col];
                array[SIZE - row - 1][col] = tmp;
            }
            col++;
        }
        row++;
    }
}

// Stage 3 - TNT!
void tnt_explosion(int array[SIZE][SIZE], double radius, int row_centre, int col_centre) {
    int row = 0;
    int col = 0;
    double distance;
    while (row < SIZE) {
        col = 0;        
        while (col < SIZE) {
            // distance between centre and current element
            distance = sqrt(pow((row - row_centre), 2) 
            + pow((col - col_centre), 2));
           
            // check if current element is within radius
            if (distance < radius) { 
                array[row][col] = EMPTY;
            }
            col++;
        }
        row++;
    }        
}

/*/ Stage 4 - marching block (UNFINISHED / FAILED)
void marching_block(int array[SIZE][SIZE]) {
    int direction = 1 // starts on odd number 
    
    while (SIZE - row - 1 >= 0) {// scanning from bottom to top 
        col = 0;
        while (col < SIZE) {
            if (steps_marched % 2 == 0) { // if even, block moves down
                // same code as shift_down loop
                int tmp = array[SIZE - row - 1][col]; // temp value 
                array[SIZE - row - 1][col] = EMPTY;
                array[SIZE - row][col] = tmp;
            }        
            col++;
        } 
        row++;
    }
    
    
        
        
    } else if (direction % 2 == 0) {// left
        int tmp = array[SIZE - row - 1][col]
        array[SIZE - row - 1][col] = EMPTY;
    } else {// right
        
    }
    steps_marched++;

   
}

*/
