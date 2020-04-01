#include "stdio.h"  
#include <stdlib.h>  
#include <pthread.h>  
#include <unistd.h> 
#include <sys/syscall.h>
#include <semaphore.h>
#define N_WRITER 3  
#define N_READER 6   
pthread_t wid[N_WRITER],rid[N_READER];  
pthread_mutex_t writeLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int data = 0;  
int reader_reading = 0; 
void * writer(void * arg);
void * reader(void * arg); 
sem_t sem1, sem2;

int main()  
{  
	int i = 0; 
	sem_init(&sem1, 0, 1);
	sem_init(&sem2, 0, 1);
    	for(i = 0; i < N_WRITER; i++)  
    	{  
        	pthread_create(&rid[i],NULL,writer,NULL);  
    	}  
    	for(i = 0; i < 3; i++)  
    	{  
        	pthread_create(&wid[i],NULL,reader,NULL);  
    	} 
	while(1){
			sleep(3.5);
    	for(i = 0; i < 3; i++)  
    	{  
        	pthread_create(&wid[i+3],NULL,reader,NULL);  
    	}  
	break;
	} 
	sleep(10); 
    	return 0;  
}
void * writer(void * arg)  
{  
        pthread_mutex_lock(&writeLock);
	sem_wait(&sem1);
//write
	int temp = rand()%100;   
    	data = temp;   
    	printf("write %d\n",data);  
        sleep(1); 
//
	sem_post(&sem1);
        pthread_mutex_unlock(&writeLock);
    	pthread_exit((void *) 0); 

}  
void * reader(void * arg)
{
        pthread_mutex_lock(&mutex);
	reader_reading++;
	sem_wait(&sem2);
	if(reader_reading == 1){
		sem_wait(&sem1);
	}
	sem_post(&sem2);
        pthread_mutex_unlock(&mutex);
//read
    	printf("read %d\n",data);
	sleep(1);
//
        pthread_mutex_lock(&mutex);
	reader_reading--;
	sem_wait(&sem2);
	if(reader_reading == 0){
		sem_post(&sem1);
	}
	sem_post(&sem2);
        pthread_mutex_unlock(&mutex);
    	pthread_exit((void *) 0); 
}























