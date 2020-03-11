#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXSIZE     27

char shared [10][10];

void die(char *s)
{
    perror(s);
    exit(1);
}

int main()
{
    char c;
    int shmid;
    key_t key;
    char (*shm)[10], *s;

    key = 5679;

    //get shared mem id
    if ((shmid = shmget(key,sizeof(shared), IPC_CREAT | 0666)) < 0)
        die("shmget");

    //attach to the shared memory 
    if ((shm = shmat(shmid, NULL, 0)) ==  -1)
        die("shmat");

    //test
    shm[0][1] = '*';

    //run 
    int run = 1;

    //keep program running
    while(run){
        scanf("%d",&run);
        scanf("%c",&shm[0][1]);
    }

    exit(0);
}
