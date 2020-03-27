#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

#define MAXSIZE     27

char shared [10][10];
char c;
int shmid;
key_t key = 5680;
char (*stream)[10];

int numOfProcess;

//Kill proccess if error
void die(char *s)
{
    perror(s);
    exit(1);
}

//Fill double array 
void fillStream(){
    for(int i = 0;i<10;i++){
        for(int j = 0; j<10;j++){
            stream[i][j]='|';
        }
    }
}

//Print double array
void printStream(){
    for(int i = 0;i<10;i++){
        for(int j = 0; j<10;j++){
            printf("%c",stream[i][j]);
            printf(" ");
        }
        printf("\n");
    }
    sleep(1);
    printf("----------------------\n\n");
}

//Create shared memory
void connect(){
    //get shared mem id
    if ((shmid = shmget(key,sizeof(shared), IPC_CREAT | 0666)) < 0)
        die("shmget");

    //attach to the shared memory 
    stream = shmat(shmid, NULL, 0);
    
}

//Returns a random coordinate given a bound
int randomCord(int bound){
    srand(time(NULL));
    int ran = rand()%bound+1;
    //printf("%c", (char)ran);
    return ran;
}

void * spawnPellet(){
    int pid = fork();
    int err = 0;

    if(numOfProcess==0){
        
        char coordY[2];
        sprintf(coordY,"%d",randomCord(9));
        
        char coordX[2];
        sprintf(coordX,"%d",randomCord(9));

        //printf("Pellet dropped at y:%s x:%s",coordY,coordX);

        char *cmd[] = {"./pellet",coordX,coordY,NULL};

        err = execv(cmd[0],cmd);

        printf("%d",err);
    }
    

}

void * spawnFish(){
    int fork_= fork();
    
    if(fork_==0){
        char *cmd[3];

        cmd[0] = "./fish";
        if(numOfProcess != 19){
            numOfProcess++;
            execlp(cmd[0],NULL);
            numOfProcess--;
        }

    }
}

int main()
{

    connect();
    fillStream();

    sleep(2);

    spawnFish();



    while(1){
        spawnPellet();
        printStream();
        printf("\n----------------------\n");
    }

    exit(0);
}
