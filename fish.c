#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>


#define x 0
#define y 1
#define size 15

//Fish Coordinates
int fishPos_x= size/2,fishPos_y =size-1;

//Water Stream memory
char (*stream)[size];

//Pellet variables 
int pelletPostions[2][20], nearestPellet_x, pelletsOnField = 0;

//Shared mem variables 
int shmid;
key_t key = 5680;

//kill 
void die(char *s)
{
    //prints error and kills process
    perror(s);
    _Exit(3); //Exit stat 3
}

//Move left, sleep 2 secs
void moveLeft(){
    //if not out of bounds 
    if(fishPos_x>0){
        //Overwrite the last pos
        stream[size-1][fishPos_x] = '-';
        fishPos_x--;
        //Write new pos
        stream[size-1][fishPos_x] = 'F';
    }
}

//Move right, sleep 2 secs
void moveRight(){
    //if not out of bounds 
    if(fishPos_x<size-1){
        //Overwrite the last pos
        stream[size-1][fishPos_x] = '-';
        fishPos_x++;
        //Write new pos
        stream[size-1][fishPos_x] = 'F';
    }
}

//Draw init position of fish in mem
void spawnFish(){
    stream[size-1][fishPos_x] = 'F';
}

//Finds every pellet currently on the field, and stores every position
void findAllPellets(){
    pelletsOnField = 0;

    //scan double array 
    for(int i = 0; i<size; i++){
        for(int j= 0; j<size; j++){
            if(stream[i][j] == -0x80){
                pelletPostions[y][pelletsOnField] = i; //Capture y coord
                pelletPostions[x][pelletsOnField] = j; //Capture x coord
                pelletsOnField ++;
            }
        }
    }
}

//Finds the nearest pellet to the fish
void findNearestPellet(){

    // in a grid of sizexsize the max length will not be > 25
    int minLength = 25;
    
    //for each pellet on the field
    for(int i = 0;i< pelletsOnField; i++){

        int length_x = pelletPostions[x][i] - fishPos_x;
        int length_y = pelletPostions[y][i] - fishPos_y;

        //use the pyth theorem to find the actual distance 
        double length = sqrt( pow(length_x,2) + pow(length_y,2) );
        
        //keep minimal distance
        if(length<minLength){
            minLength = length;
            nearestPellet_x = pelletPostions[x][i];
        }
    }

}

//Move to nearest pellet
void seek(){
    //if we have pellets on field
    if(pelletsOnField!=0){
        //if nearest is to the left move left
        if( (fishPos_x - nearestPellet_x) > 0 )
            moveLeft();
        //if nearest is to the right move right
        else if( (fishPos_x - nearestPellet_x < 0))
            moveRight();
    }
}

//Attach to shared memory
void connect(){

    //get and id for the shared memory given size, shmget 
    if ((shmid = shmget(key,sizeof(unsigned char [size][size]),  0666)) < 0)
        die("shmget");

    //attach to the shared memory 
    stream = shmat(shmid, NULL, 0);

    //if shmat is unsuccesful, it returns a (void*) -1
    if(stream == (void *)-1)
        die("shmat");
}

//test commit

void handle_terminate(int sig){
    //Shows the signal captured 
    fprintf(stderr,"Fish terminated id:%d signal: %d\n",getpid(),sig);

    //Exit with status 0, and detaches from memory 
    _Exit(0); 
}

int main()
{
    //Tell the process how to handle the interupt, in my case send it to "Handle_terminate"
    signal(SIGINT, handle_terminate);

    connect(); //Attach to shared memory
    spawnFish();//Sets init pos of fish
    
    while(1){
        findAllPellets();//Scan the double array
        findNearestPellet(); //Compute the nearest 
        seek(); //Seek nearest
    }
}