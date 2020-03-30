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

#define size  15

//Shared memory variables
int shmid;
key_t key = 5680;
char (*stream)[size];

//Process Variables
int pindex = 0;
int proccess[100]; 
int numOfProcess=0;

//File to write and append to
FILE *file;

//Kill proccess if error
void die(char *s)
{   
    //prints error and kills process
    perror(s);
    exit(1);
}

//Fill double array 
void fillStream(){
    
    //Fill the array with - 
    for(int i = 0;i<size;i++){
        for(int j = 0; j<size;j++){
            stream[i][j]='-';
        }
    }
}

//Print double array
void printStream(){
    
    //print the stream to the file 
    for(int i = 0;i<size;i++){
        for(int j = 0; j<size;j++){
            fprintf(file,"%c",stream[i][j]);
            fprintf(file," ");
        }
        fprintf(file,"\n");
    }
    fprintf(file,"----------------------\nCurrent amount of running process: %d \n",numOfProcess);
}

//Create shared memory
void connect(){
    printf("HERE");
    //get and id for the shared memory given size, shmget 
    if ((shmid = shmget(key,sizeof(unsigned char [size][size]), IPC_CREAT | 0666)) < 0)
        die("shmget");
    printf("HERE");
    //attach to the shared memory 
    stream = shmat(shmid, NULL, 0);

    //if shmat is unsuccesful, it returns a (void*) -1
    if(stream == (void *)-1)
        die("shmat");
    
}

//Returns a random coordinate given a bound
int randomNum(int bound){
    srand(time(NULL)); //Seeds rand with system time
    int ran = rand()%bound; //give me a rand variable between 0-bound
    return ran;
}

void * spawnPellet(){
    //fork a proccess
    //child will set pid = 0
    //parent will set pid = pid of child
    int pid = fork(); 

    //children enter if 
    if(pid==0){

        //Place a random coordinate into Y and X
        char coordY[3];
        sprintf(coordY,"%d",randomNum(size-1));
        
        char coordX[3];
        sprintf(coordX,"%d",randomNum(size-1));

        //Make an array of arguments
        char *cmd[] = {"./pellet",coordX,coordY,NULL};

        //send the arguments and start the exeution of the program given
        if(execv(cmd[0],cmd) == -1)  // The child process now executes ./pellet, detaches from memeory and clears local variables
            printf("Error executing pellet \n");
    }
    else if(pid!=0 && pid!=-1) {

        //Place the next proccess in the process array and increment index
        proccess[pindex] = pid;
        pindex++;
        
        int status;

        //increment the amount of processes running
        numOfProcess++;
        wait(&status); //Wait for the children process to finish executing
        numOfProcess--; //Decrement since it finished running

        switch (WEXITSTATUS(status))
        {
            case 1:fprintf(file,"SWIM_MILL -- Pellet: %d was missed\n",pid);
                break;
            case 0: fprintf(file,"SWIM_MILL -- Pellet: %d was eaten\n",pid);
                break;
            case 3: fprintf(file,"SWIM_MILL -- Pellet: %d could not attach to memory\n",pid);
                break;
            case 4: fprintf(file,"SWIM_MILL -- Pellet: %d recieved SIG_INT\n",pid);
                break;

        };
    }
    else{
        //if not caught means there was and error since pid will = -1
        printf("Error forking: %d \n",pid);
    }
    

}

void * spawnFish(){
    int pid = fork();
    
    if(pid==0){
        char *cmd[3];

        cmd[0] = "./fish";
        int err = execv(cmd[0],cmd); // The child process now executes ./fish, detaches from memeory and clears local variables
        printf("Spawn fish failed err: %d ",err);
    }
    else if(pid!=0 && pid!=-1) {

        //Place the next proccess in the process array and increment index
        proccess[pindex] = pid;
        pindex++;

        int status; //hold exit status

        //increment the amount of processes running
        numOfProcess++;
        wait(&status); //Wait for the children process to finish executing, put exit status in status
        numOfProcess--; //Decrement since it finished running

        switch (WEXITSTATUS(status))
        {
            case 0: fprintf(file,"SWIM_MILL -- Fish: %d recieved SIG_INT\n",pid);
                break;
            case 3: fprintf(file,"SWIM_MILL -- Fish: %d could not attach to memory\n",pid);
                break;
        };

        printf("Fish was exited with status: %d",status);
    }
    else{
        //if not caught means there was and error since pid will = -1
        printf("Error forking: %d \n",pid);
    }
}

void handle_terminate(int sig){

    //SIGINT =2
    if(sig == 2)
        fprintf(stderr,"Interupt caught\n");
    else
        fprintf(stderr,"Killing all processes\n");
    
    //Send SIGINT to every running proccess
    for(int i=0; i<pindex; i++){
        int status = kill(proccess[i],SIGINT);
    }

    fprintf(stderr, "\n");

    //Remove the shared memory from the system, as well as detach
    shmctl(shmid,IPC_RMID,NULL);

    fclose(file);//close the 

    _Exit(0); //Exits the program but also ensures the shared memory is detached
}

void * timer(){

    fprintf(file,"\nStarted 30 second timer\n");
    sleep(30); //Wait for 30 secs 
    fprintf(file,"\n 30 seconds are up!\n");
    handle_terminate(1); //Run the clean up and kill everything
}

int main()
{
    printf("HERE");
    pthread_t threads[100]; //Threads that can be used to wait for processes

    signal(SIGINT, handle_terminate);//Tell the process how to handle the interupt, in my case send it to "Handle_terminate"

    connect(); //Attach to shared mem
    fillStream(); //Fill the stream with '-' chars 

    pthread_create(&threads[1],NULL,spawnFish,NULL); //Second thread waits for fish process

    int i= 2; //Index 2 because 0,1 already taken 

    //Makes a new file or erases all content in com.txt
    file = fopen("comp.txt","w"); // 'w' indicates write 
    fclose(file); //Close file so we can append

    //makes the file able to append
    file = fopen("comp.txt","a");

    pthread_create(&threads[0],NULL,timer,NULL); //First thread starts the timer 

    while(1){

        printStream(); //Print the contents to a file
        sleep(1);//Sleep makes it pretty

        //if we have less than 20 processes then its ok to spawn more
        //random number creates a random spawn rate
        if(numOfProcess<20 && (randomNum(2) == 1)){

            //Sets the next thread to wait for a new pellet process
            pthread_create(&threads[i],NULL,spawnPellet,NULL);
            i++;
        }

        fprintf(file,"\n----------------------\n");
    }

}
