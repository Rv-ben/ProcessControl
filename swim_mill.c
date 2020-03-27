#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define MAXSIZE     27

char shared [10][10];
char c;
int shmid;
key_t key = 5679;
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
    sleep(2);
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
char randomCord(int bound){
    srand(time(0));
    int ran = rand()%bound +48;
    return ran;
}

void spawnPellet(){
    numOfProcess = fork();
    int err = 0;

    if(numOfProcess==0){
        char *cmd[3];

        cmd[0] = "./pellet";
        cmd[1] = (char)53+"";
        cmd[2] = (char)53+"";

        err = execlp(cmd[0],cmd[1],cmd[2]);

        printf("%d",err);
    }
    

}

int main()
{

    connect();
    fillStream();

    sleep(2);
    //spawnProcces("./fish");
    spawnPellet();

    while(1){
        printStream();
        printf("\n----------------------\n");
    }

    exit(0);
}
