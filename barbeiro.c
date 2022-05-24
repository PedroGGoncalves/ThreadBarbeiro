#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#define NUM 30

struct job{
	int n;
	struct job* next;
};

struct job2{
	int n;
};

struct job* lista;
struct job* fim_lista;

int barbeiros;
int cadeiras;
int tempo_corte;
int intervalo;

int thread_flag;
pthread_cond_t thread_flag_cv;
pthread_mutex_t thread_flag_mutex;

void initialize_flag (){
	pthread_mutex_init (&thread_flag_mutex, NULL);
	pthread_cond_init (&thread_flag_cv, NULL);
	thread_flag = 0;
}

void * thread_function(void * thread_arg){
	struct job* next_job;
	struct job2* arg = (struct job2*)thread_arg;
	int n = arg->n;
	while(1){
		
		pthread_mutex_lock(&thread_flag_mutex);
		if(lista == NULL)		{
			while(thread_flag == 0)			
			{
				printf("Barbeiro %d dormindo.\n", n);	
				pthread_cond_wait(&thread_flag_cv,&thread_flag_mutex);
				printf("Barbeiro %d acordou.\n", n);
			}
		}
		thread_flag--;
		next_job = lista;
		lista = next_job->next;
		pthread_mutex_unlock(&thread_flag_mutex);
		
		printf("Barbeiro %d cortando o cabelo do cliente %d.\n", n, next_job->n);
		usleep(tempo_corte*1000000);
		printf("Barbeiro %d terminou de cortar o cabelo do cliente %d.\n", n, next_job->n);
	}
	return NULL;
}

int main(int argc, char* argv[]){

	initialize_flag ();
	
	barbeiros = atoi(argv[1]);
	cadeiras = atoi(argv[2]);
	tempo_corte = atoi(argv[3]);
	intervalo = atoi(argv[4]);
	
	struct job2 thread_args;
	pthread_t barbers[barbeiros];
	
	for(int i=1;i<=barbeiros;i++)
	{
		thread_args.n = i;
		pthread_create(&barbers[i], NULL, (void *)thread_function, (void *)&thread_args);
		sleep(1);
	}
	
	for(int j = 1; j <= NUM; j++){
		usleep(intervalo*1000000);
		printf("Cliente %d chegou.\n", j);
		if(thread_flag < cadeiras)
		{
			struct job* next_job = malloc(sizeof(struct job));
			next_job->n = j;
			next_job->next= NULL;
			if(lista == NULL)
				lista = next_job;
			else
				fim_lista ->next = next_job;
			fim_lista  = next_job;
			pthread_mutex_lock(&thread_flag_mutex);
			thread_flag++;
			pthread_cond_signal(&thread_flag_cv);
			pthread_mutex_unlock(&thread_flag_mutex);
		}
		else	
			printf("Cliente %d foi embora sem cortar o cabelo. Sala de espera cheia.\n", j);		
	}
}
