#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <math.h>
#include <signal.h>

#define size 15

//X Y positions
int pellet_pos_x , pellet_pos_y;

//Shared memory variables
int shmid;
key_t key = 5680;
char (*stream)[size];

void die(char *s)
{
    //prints error and kills process
    perror(s);
    exit(3); //Exit stat 3
}

void connect(){
    //get and id for the shared memory given size, shmget 
    if ((shmid = shmget(key,sizeof(unsigned char [size][size]), IPC_CREAT | 0666)) < 0)
        die("shmget");

    //attach to the shared memory 
    stream = shmat(shmid, NULL, 0);

    //if shmat is unsuccesful, it returns a (void*) -1
    if(stream == (void *)-1)
        die("shmat");
}

//place dot in the stream
void spawnPellet(){
    stream[pellet_pos_y][pellet_pos_x] = 0x80;
}

//makes the pellet drop by updating it's position
void drop(){

    //While not at the bottom
    while (pellet_pos_y != size-1 )
    {   
        //Keep updating it's pos to drop
        stream[pellet_pos_y][pellet_pos_x] = '-';
        pellet_pos_y++;
        stream[pellet_pos_y][pellet_pos_x] = 0x80;

        //Sleep so pellet can be slower than fish
        sleep(2);

        //If fish and pellet have same pos, then indicate that it was eaten
        if(stream[pellet_pos_y][pellet_pos_x] == 'F'){
            stream[pellet_pos_y][pellet_pos_x] |= 0x80; //Bit wise or so we can return to 'F'
            fprintf(stderr,"I got ate! pid: %d \n",getpid());
            _Exit(0); //Exit stat 0 if ate
        }
        
    }
    fprintf(stderr,"I was missed! pid: %d \n",getpid()); 
    stream[pellet_pos_y][pellet_pos_x] = '-';
    
}

//Terminates if SIG_INT is sent
void handle_terminate(int sig){
    fprintf(stderr,"Pellet terminated id: %d  signal: %d\n",getpid(),sig);
    _Exit(4); //EXIT STATUS 4 to indicate it was inturrupted
}

int main(int argc, char** argv){

    //Tell the process how to handle the interupt, in my case send it to "Handle_terminate"
    signal(SIGINT, handle_terminate);

    //Parse arguments into place
    pellet_pos_y = atoi(argv[1]);
    pellet_pos_x = atoi(argv[2]);

    connect();// Attach to shared mem
    spawnPellet(); //Set init pos of pellet
    //Print to Stderr
    fprintf(stderr,"Spawned pellet pid: %d   x:%d   y:%d \n",getpid(),pellet_pos_x, pellet_pos_y);
    drop(); //Drop the pellet

    //exits and detaches from shared memory 
    _Exit(1); //Exit stat 1 since it was eaten
} 