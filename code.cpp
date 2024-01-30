#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
#include<string.h>
#include<sys/types.h>
//#include <sys/wait.h>
#include<time.h>

#define capacity 20

//void *Patient(void *count);
//void *Doctor(void *);

sem_t waitingRoom;
sem_t doctorsChair;
sem_t doctorsBed;
sem_t patientOnChair;

pthread_mutex_t treatment;

int no_more = 0;
int i, patientCount, chairCount;

void *Patient(void *number){

    int num = (int)number;

    printf("\n\t\tPatient number %d came to chamber.\n", num);

    sem_wait(&waitingRoom);
    printf("\t\tPatient number %d is waiting in the waiting room.\n", num);

    sem_wait(&doctorsChair);
    sem_post(&waitingRoom);

    printf("\t\tPatient number %d going to doctor.\n", num);

    sem_post(&doctorsBed);
    sem_wait(&patientOnChair);
    sem_post(&doctorsChair);

    pthread_mutex_lock(&treatment);
    printf("\t\tPatient number %d leaving the chamber after treatment.\n", num);
    pthread_mutex_unlock(&treatment);
}

void *Doctor(void *parameter){
    int c=0;
    while(!no_more){

         sem_wait(&doctorsBed);

        if(!no_more){
            printf("\n\t\tDoctor is treating.\n");
            printf("\t\tTreatment Finished.\n");
            sem_post(&patientOnChair);
            c++;

        if(chairCount == c || patientCount == c){
            printf("\n\t\tNo one is in the waiting room. Doctor is sleeping on his chair.\n\n");
            exit(-1);
            }
        }
    }
}

int main(int args, char* argc[]){

    pthread_t dtid;
    pthread_t ptid[capacity];
    int serial[capacity];

    printf("\n\n=======================================================================\n");
    printf("===================== WELCOME to Dental Clinic ========================\n");
    printf("=======================================================================\n");

    printf("\n\tEnter the number of patient : ");
    scanf("%d",&patientCount) ;

    printf("\tEnter the number of empty chairs in the waiting room: ");
    scanf("%d",&chairCount);
    if(chairCount > capacity){
        printf("\n\tEmpty chair count cannot be greater than capacity.\n");
        exit(-1);
    }

    if(patientCount > chairCount){
        printf("\tSORRY...!!! Maximum capacity is %d. \n\tPlease last %d patient come sometime later.\n", capacity, patientCount-chairCount);
        patientCount = chairCount;
    }

    if(chairCount==capacity){
        printf("\n\n\tPatient 1 is waking up the doctor.\n");
    }

    for(i=1; i<=capacity; i++){
        serial[i] = i;
    }

    sem_init(&waitingRoom, 0, chairCount);
    sem_init(&doctorsChair, 0, 1);
    sem_init(&doctorsBed, 0, 0);
    sem_init(&patientOnChair, 0, 0);

    pthread_create(&dtid, NULL, Doctor, NULL);

    for(i=1; i<=patientCount; i++){
        pthread_create(&ptid[i], NULL, Patient, (void*)&serial[i]);
        sleep(1);
    }

     for(i=1; i<=patientCount; i++){
         pthread_join(ptid[i], NULL);
         sleep(1);
     }

     no_more = 1;
     pthread_join(dtid, NULL);

     sem_destroy(&waitingRoom);
     sem_destroy(&doctorsChair);
     sem_destroy(&patientOnChair);
     sem_destroy(&doctorsBed);

     pthread_mutex_destroy(&treatment);

     return 0;
}
