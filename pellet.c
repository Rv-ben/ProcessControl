#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <math.h>
#include <signal.h>


int pellet_pos_x , pellet_pos_y;

unsigned char shared [10][10];
char c;
int shmid;
key_t key = 5680;
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

    stream[pellet_pos_y][pellet_pos_x] = 0x80;
}

//makes the pellet drop by updating it's position
void drop(){
    while (pellet_pos_y != 9 )
    {   
        stream[pellet_pos_y][pellet_pos_x] = '|';
        pellet_pos_y++;
        stream[pellet_pos_y][pellet_pos_x] = 0x80;
        sleep(2);
        
        if(stream[pellet_pos_y][pellet_pos_x] == 'F'){
            stream[pellet_pos_y][pellet_pos_x] |= 0x80;
            fprintf(stderr,"I got ate! pid: %d \n",getpid());
            _Exit(0);
        }
        
    }

    fprintf(stderr,"I was missed! pid: %d \n",getpid());
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

void handle_terminate(int sig){
    fprintf(stderr,"Pellet terminated id: %d  signal: %d\n",getpid(),sig);
    _Exit(0);
}

int main(int argc, char** argv){

    signal(SIGINT, handle_terminate);

    pellet_pos_y = parseNum(argv[1]);
    pellet_pos_x = parseNum(argv[2]);

    connect();
    spawnPellet();
    fprintf(stderr,"Spawned pellet pid: %d   x:%d   y:%d \n",getpid(),pellet_pos_x, pellet_pos_y);
    drop();

    //exits and detaches from shared memory 
    _exit(0);
}