#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
    if ((stream= shmat(shmid, NULL, 0)) ==  -1)
        die("shmat");
}

void spawnProcces(char * exeName){
    numOfProcess = fork();

    if(numOfProcess==0){
        char *args[] = {exeName,NULL};
        execvp(args[0],args);
    }
    printf("%d",numOfProcess);
}

int main()
{

    connect();
    fillStream();

    sleep(2);

    spawnProcces("./fish");
    spawnProcces("./pellet");

    while(1){
        printStream();
        printf("%d",numOfProcess);
    }

    exit(0);
}
