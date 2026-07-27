#include <stdio.h>
#include "Types.h"

// If you have your initialization function in board.c, 
// you can declare it here so main knows about it.
extern void initializing_Board(); 

int main() {
    printf("Starting MONOPOLY-LK Simulation...\n");
    
    // Call the function you just wrote!
    initializing_Board();
    
    printf("Board initialized successfully.\n");
    
    return 0; // Tells the computer the program finished without errors
}