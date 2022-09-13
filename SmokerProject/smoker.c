#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <semaphore.h>
#include <unistd.h>

int smokeCount; //  Amount of smokes before smoker dies
int smokersCount = 3; // Number of Smokers
sem_t table; // table that contains the resources for a smoker to smoke
sem_t agent; // places the resource on the table
sem_t smokers[3]; // contains the smokers

// Generates a random resource and notifies the smokers
// Runs while the total number of possible smokes > 0
// Keeps track of how much each smoker has smoked in an array

void* agentThdFunc(){
    int randNum=0;
    int totalSmokes = smokeCount*smokersCount;
    int smokersAlive[smokersCount];
    while(totalSmokes > 0){
    sem_wait(&table);
    do{
        randNum = rand() % smokersCount;
    }while(smokersAlive[randNum] == smokeCount);
    switch(randNum){
        case 0:
            printf("agent produced tobacco and paper\n");
            break;
        case 1:
            printf("agent produced matches and tobacco\n");
            break;
        case 2:
            printf("agent produced matches and paper\n");
            break;
    }
    totalSmokes--;
    smokersAlive[randNum]++;
    sem_post(&table);
    sem_post(&smokers[randNum]);
    sem_wait(&agent);
    }
    return NULL;
}

// Waits for the agent to put a resource on the table
// the appropriate smoker gets the resource uses it
// and then goes back to sleep

void* smokersThdFunc(void* arg){
    srand(time(0));
    int currentSmoker = (intptr_t)arg;
    int smoked = 0;
    printf("Smoker %i starts...\n", currentSmoker);
    while(smoked < smokeCount){
    sem_wait(&smokers[currentSmoker]);
    sem_wait(&table);
    usleep(rand() % 1500000);
    switch(currentSmoker){
        case 0:
            printf("\033[0;31mSmoker %d completed smoking\033[0m\n",currentSmoker);
            break;
        case 1:
            printf("\033[0;32mSmoker %d completed smoking\033[0m\n",currentSmoker);
            break;
        case 2:
            printf("\033[0;34mSmoker %d completed smoking\033[0m\n",currentSmoker);
            break;
    }
    smoked++;
    sem_post(&table);
    sem_post(&agent);
    }
    printf("Smoker %d dies of cancer.\n", currentSmoker);

    return NULL;
}   


int main(int argc, char* argv[]){

    // Command line argument checking
    if(argc != 3){
        printf("Usage: -s [smokeCount]\n");
        return 0;
    }
    smokeCount = atoi(argv[2]);
    if(smokeCount < 3 || smokeCount > 10){
        printf("Error: Smoke Count must be between 3 and 10 (inclusive)\n");
        return 0;
    }
    
    // Initialize semaphores and threads
    sem_init(&table,1,1);
    sem_init(&agent,1,0);
    for(int i = 0; i < smokersCount;i++){
    sem_init(&smokers[i],1,0);
    }
    unsigned long int aError, sError;
    pthread_t agentThd;
    pthread_t smokersThd[smokersCount];

    // Creates agent and smoker threads

    aError = pthread_create(&agentThd,NULL,&agentThdFunc,NULL);

    if(aError != 0)
        printf("Error: Could not intialize agent thread");
    

    for(int i = 0;i < smokersCount;i++){
        sError = pthread_create(&smokersThd[i],NULL,&smokersThdFunc,(void*)(intptr_t)i);

        if(sError != 0){
        printf("Error: Could not intialize smoker thread %d", i);
        return 0;
    }
    }
    // Waits for the threads to finish 
    pthread_join(agentThd,NULL);

    for(int i = 0;i < smokersCount;i++){
        pthread_join(smokersThd[i],NULL);
    }


    return 0;
}