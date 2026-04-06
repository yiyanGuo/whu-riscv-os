#include "defs.h"

void proczero_code(void){

    for(;;){
        int i = 0;
        for(int i = 0; i < 10000000; i++);
        printf("i=%d\n", i);
    }
}