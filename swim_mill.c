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
char (*shm)[10], *s;


void die(char *s)
{
    perror(s);
    exit(1);
}

void fillStream(){
    for(int i = 0;i<10;i++){
        for(int j = 0; j<10;j++){
            shm[i][j]='|';
        }
    }
}

void printStream(){
    for(int i = 0;i<10;i++){
        for(int j = 0; j<10;j++){
            printf("%c",shm[i][j]);
            printf(" ");
        }
        printf("\n");
    }
    sleep(2);
    printf("----------------------\n\n");
}

int main()
{
    
     //get shared mem id
    if ((shmid = shmget(key,sizeof(shared), IPC_CREAT | 0666)) < 0)
        die("shmget");

    //attach to the shared memory 
    if ((shm = shmat(shmid, NULL, 0)) ==  -1)
        die("shmat");

    fillStream();
    while(1){
        printStream();
    }



    exit(0);
}
