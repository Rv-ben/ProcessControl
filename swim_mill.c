 
#include <sys/ipc.h> 
#include <stdio.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define SHM_KEY 1246


int sharedSpace[10][10];

key_t key;

int main() 
{ 


	int shmid = shmget(key,sizeof(sharedSpace),IPC_CREAT|0666); 

	printf("%d", shmid);

	int  (*stream)[10] = shmat(shmid,NULL,0);

	stream[0][0] =1;

	shmdt(stream);

	return 0; 
} 
