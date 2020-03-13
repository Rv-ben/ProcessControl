#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

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
    if ((stream = shmat(shmid, NULL, 0)) ==  -1)
        die("shmat");
}

void spawnPellet(){
    srand(time(0));
    pellet_pos_x = rand()%9;
    srand(time(0));
    pellet_pos_y = rand()%9;

    stream[pellet_pos_y][pellet_pos_x] = 'o';
}

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


int main(){

    connect();
    spawnPellet();
    drop();
    
}