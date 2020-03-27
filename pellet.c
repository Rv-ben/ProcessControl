#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <math.h>


int pellet_pos_x , pellet_pos_y;

char shared [10][10];
char c;
int shmid;
key_t key = 5679;
char (*stream)[10], *s;

void die(char *s)
{
    perror(s);
    exit(1);
}

void connect(){
    //get shared mem id
    if ((shmid = shmget(key,sizeof(shared), IPC_CREAT | 0666)) < 0)
        die("shmget");

    //attach to the shared memory 
    stream = shmat(shmid, NULL, 0);
}

//place dot in the stream
void spawnPellet(){

    stream[pellet_pos_y][pellet_pos_x] = 'o';
}

//makes the pellet drop by updating it's position
void drop(){
    while (pellet_pos_y != 9 )
    {   
        stream[pellet_pos_y][pellet_pos_x] = '|';
        pellet_pos_y++;
        stream[pellet_pos_y][pellet_pos_x] = 'o';
        sleep(2);
    }

    stream[pellet_pos_y][pellet_pos_x] = '|';
    
}

//parse a number given a string
int parseNum(char* numStr){
    int place = strlen(numStr);
    int value = 0;

    for(int i = 0; i<strlen(numStr); i++){
        value += (numStr[i] - 48  )* pow(10,place-1);
        place--;
    }

    return value;
}

int main(int argc, char** argv){

    pellet_pos_y = parseNum(argv[0]);
    pellet_pos_x = parseNum(argv[1]);

    connect();
    spawnPellet();
    drop();
    
}