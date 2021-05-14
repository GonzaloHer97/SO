#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

//Estado del bus
#define EN_RUTA 0		// autobús en ruta
#define EN_PARADA 1		// autobús en la parada
#define PARADAS 5		// número de paradas de la ruta
#define PASAJEROS 4		// numero de usuarios

// estado inicial
int estado = EN_RUTA;  // = 0, = 1 para EN_PARADA
int parada_actual = 0; // parada en la que se encuentra el autobus
int n_ocupantes = 0;   // ocupantes que tiene el autobús
//Arrays dinamicos para poder ajustar el tamaño segun los parametros de entrada
int *esperandoEnParada; //= {0,0,...0}; // personas que desean subir en cada parada
int *esperandoBajar;  //= {0,0,...0}; // personas que desean bajar en cada parada
//Mutex
pthread_mutex_t mutex;
//Variables de condicion
pthread_cond_t varParada;
pthread_cond_t varBusPreparado;
//Threads
pthread_t bus;
pthread_t *pasajeros; //Array dinamico para poder ajustar el tamaño segun los parametros de entrada


void inicializarSimulación(pthread_t mutex, pthread_cond_t varBusPreparado, pthread_cond_t varParada){
	pthread_mutex_init(&mutex, NULL);		//Mutex
	pthread_cond_init(&varBusPreparado, NULL); 	//Condicion de arranque del bus
	pthread_cond_init(&varParada, NULL);	//Condicion de parada del bus
}

int main(int argc, char *argv[])
{
	int i;
	pasajeros = malloc(PASAJEROS * sizeof(int));
	esperandoEnParada = malloc(PARADAS * sizeof(int));
	esperandoBajar = malloc(PARADAS * sizeof(int));

	pthread_create(&bus, NULL, thread_autobus, NULL); //thread_autobus es fucnión

	inicializarSimulación(&mutex, varBusPreparado, varParada);
	
	for (i = 0; i < PASAJEROS; i++)
		pthread_create(&pasajeros[i], NULL, thread_usuario, (void *)i); 

	pthread_join(bus, NULL);

	for (i = 0; i < PASAJEROS; i++)
		pthread_join(pasajeros[i], NULL);
	

	return 0;
}

