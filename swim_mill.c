#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <time.h>
#include <pthread.h>
#include <signal.h>


#define MAXSIZE     27

char shared [10][10];
char c;
int shmid;
key_t key = 5680;
char (*stream)[10];

int pindex = 0;

int proccess[100]; 
int numOfProcess=0;

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
    printf("----------------------\nCurrent amount of process: %d \n",numOfProcess);
    
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

    if(pid==0){

        char coordY[2];
        sprintf(coordY,"%d",randomCord(9));
        
        char coordX[2];
        printf(coordX,"%d",randomCord(9));
    
        char *cmd[] = {"./pellet",coordX,coordY,NULL};

        err = execv(cmd[0],cmd);
    }
    else if(pid!=0) {
        proccess[pindex] = pid;
        pindex++;

        numOfProcess++;
        wait(NULL);
        numOfProcess--;
    }
    

}

void * spawnFish(){
    int pid = fork();
    
    if(pid==0){
        char *cmd[3];

        cmd[0] = "./fish";
        int err = execlp(cmd[0],cmd[0]);
        printf("Spawn fish failed err: %d ",err);
    }
    else{

        proccess[pindex] = pid;
        pindex++;

        numOfProcess++;
        wait(NULL);
        numOfProcess--;
    
    }
}

void handle_terminate(int sig){

    printf("\nCaught: %d = Terminate pindex=%d\n",sig,pindex);
    
     for(int i=0; i<pindex; i++){
        int status = kill(proccess[i],SIGINT);
    }

    fprintf(stderr, "\n");

    //Remove the shared memory from the system
    shmctl(shmid,IPC_RMID,NULL);

    _Exit(0);
}

int main()
{

    pthread_t threads[100];

    signal(SIGINT, handle_terminate);

    connect();
    fillStream();

    sleep(2);

    pthread_create(&threads[0],NULL,spawnFish,NULL);

    int i= 0;

    while(1){
        printStream();
        if(numOfProcess<20){
            pthread_create(&threads[i],NULL,spawnPellet,NULL);
            i++;
        }

        printf("\n----------------------\n");
    }

}
