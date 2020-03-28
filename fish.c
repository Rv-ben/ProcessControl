#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>


#define x 0
#define y 1

//Fish Coordinates
int fishPos_x= 10/2,fishPos_y =9;

//Water Stream memory
char (*stream)[10];

//Pellet variables 
int pelletPostions[2][20], nearestPellet_x, pelletsOnField = 0;

//Shared mem variables 
int shmid;
key_t key = 5680;

//kill 
void die(char *s)
{
    perror(s);
    exit(1);
}

//Move left, sleep 2 secs
void moveLeft(){
    if(fishPos_x>0){
        stream[9][fishPos_x] = '|';
        fishPos_x--;
        stream[9][fishPos_x] = 'F';
    }
    sleep(2);
}

//Move right, sleep 2 secs
void moveRight(){
    if(fishPos_x<9){
        stream[9][fishPos_x] = '|';
        fishPos_x++;
        stream[9][fishPos_x] = 'F';
    }
    sleep(2);
}

//Draw init position of fish in mem
void spawnFish(){
    stream[9][fishPos_x] = 'F';
}

//Finds every pellet currently on the field, and stores every position
void findAllPellets(){
    pelletsOnField = 0;

    for(int i = 0; i<10; i++){
        for(int j= 0; j<10; j++){
            if(stream[i][j] == '0'){
                pelletPostions[y][pelletsOnField] = i; //Capture y coord
                pelletPostions[x][pelletsOnField] = j; //Capture x coord
                pelletsOnField ++;
            }
        }
    }
}

//Finds the nearest pellet to the fish
void findNearestPellet(){

    // in a grid of 10x10 the max length will not be > 25
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

//move side to side 
void sideToSide(){
    while(1){

        for(int i = fishPos_x; i>0; i--){
            moveLeft();
        }
        
        sleep(1);

        for(int i = 0; i<10 ;i++){
            moveRight();
        }
        
    }
}

//Move to nearest pellet
void seek(){
    if(pelletsOnField!=0){
        if( (fishPos_x - nearestPellet_x) > 0 )
            moveLeft();
        else if( (fishPos_x - nearestPellet_x < 0))
            moveRight();
    }
}

//Attach to shared memory
void connect(){
    //get shared mem id 
    if ((shmid = shmget(key,sizeof(char[10][10]), 0666)) < 0)
        die("shmget");

    //attach to the shared memory
    stream = shmat(shmid, NULL, 0);
}

void checkIfEat(){
    if (stream[fishPos_y][fishPos_x] == '0')
        stream[fishPos_y][fishPos_x] = 'F';
}

//test commit

int main()
{
    connect();
    spawnFish();

    printf("Spawned fish ok");
    
    while(1){
        findAllPellets();
        findNearestPellet();
        seek();
        checkIfEat();
    }

    //exits and detaches from shared memory 
    _Exit(0);
}