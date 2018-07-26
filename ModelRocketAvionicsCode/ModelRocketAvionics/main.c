#include "stdint.h"
#include "stdbool.h"

//#include "inc/hw_types.h"
//#include "inc/hw_memmap.h"

#include "sch.h"

void Setup(void) {
    InitScheduler();
}

void Loop(void) {
    UpdateScheduler();
}

/*
 * Main function
 *  You shouldn't need to edit anything here as..
 *   its brought out to Setup() and Loop().
 */
int main(void) {
    Setup();
    while(1){
        Loop();
    }
}
