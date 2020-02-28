 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 

#define SHM_KEY 0x1234

int sharedSpace[10][10];

int main() 
{ 


	int shmid = shmget(SHM_KEY,sizeof(sharedSpace),IPC_CREAT); 

	int * shmp = shmat(shmid,NULL,0);

	

	return 0; 
} 
