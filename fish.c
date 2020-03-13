#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#define MAXSIZE     27

int fishPos = 10/2;
char (*stream)[10], *s;

void die(char *s)
{
    perror(s);
    exit(1);
}


void moveLeft(){
    if(fishPos>0){
        stream[9][fishPos] = '|';
        fishPos--;
        stream[9][fishPos] = '^';
    }
}

void moveRight(){
    if(fishPos<9){
        stream[9][fishPos] = '|';
        fishPos++;
        stream[9][fishPos] = '^';
    }
}

void spawnFish(){
    stream[9][fishPos] = '^';
}

void sideToSide(){
    while(1){

        for(int i = fishPos; i>0; i--){
            moveLeft();
            sleep(2);
        }
        
        sleep(1);

        for(int i = 0; i<10 ;i++){
            moveRight();
            sleep(2);
        }
        
    }
}

int main()
{
    int shmid;
    key_t key;

    key = 5679;

    //get shared mem id 
    if ((shmid = shmget(key,sizeof(char[10][10]), 0666)) < 0)
        die("shmget");

    //attach to the shared memory
    stream = shmat(shmid, NULL, 0);

    spawnFish();
    sideToSide();


    exit(0);
}