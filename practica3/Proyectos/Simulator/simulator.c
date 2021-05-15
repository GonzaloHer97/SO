#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define N_PARADAS 5 // número de paradas de la ruta
#define EN_RUTA 0 // autobús en ruta
#define EN_PARADA 1 // autobús en la parada
#define MAX_USUARIOS 40 // capacidad del autobús
#define USUARIOS 4 // numero de usuarios
///////////////////////////////
#define TRAYECTO 3 //Variable que indica el tiempo de trayecto
// estado inicial
int estado = EN_RUTA;
int parada_actual = 0; // parada en la que se encuentra el autobus
int n_ocupantes = 0; // ocupantes que tiene el autobús

// personas que desean subir en cada parada
int esperando_parada[N_PARADAS]; //= {0,0,...0};

// personas que desean bajar en cada parada
int esperando_bajar[N_PARADAS]; //= {0,0,...0};

// Otras definiciones globales (comunicación y sincronización)
pthread_mutex_t mutex;

pthread_cond_t varParada;
pthread_cond_t varBusPreparado;

pthread_t bus;
pthread_t pasajeros[USUARIOS]; //Array dinamico para poder ajustar el tamaño segun los parametros de entrada



void Autobus_En_Parada(){
	/* Ajustar el estado y bloquear al autobús hasta que no haya pasajeros que
	quieran bajar y/o subir la parada actual. Después se pone en marcha */
	pthread_mutex_lock(&mutex);
	estado = EN_PARADA;	
	printf("Autobus en la parada %d\n", parada_actual);

	while(((esperando_bajar[parada_actual] > 0)||(esperando_parada[parada_actual]>0))
	 && (n_ocupantes < MAX_USUARIOS)){
		pthread_cond_broadcast(&varParada);
		pthread_cond_wait(&varBusPreparado,&mutex);
	}

	printf("Autobus saliendo de la parada %d\n", parada_actual);
	pthread_mutex_unlock(&mutex);

	
}
void Conducir_Hasta_Siguiente_Parada(){
	/* Establecer un Retardo que simule el trayecto y actualizar numero de parada */
	printf("\n\nConduciendo... \n\n");
	sleep(TRAYECTO);
	
	pthread_mutex_lock(&mutex);
	estado = EN_RUTA;

	printf("El autobús está en la parada %d\n", parada_actual);
	parada_actual = (parada_actual + 1) % N_PARADAS;	
	pthread_mutex_unlock(&mutex);
}

void Subir_Autobus(int id_usuario, int origen){
	/* El usuario indicará que quiere subir en la parada ’origen’, esperará a que
	el autobús se pare en dicha parada y subirá. El id_usuario puede utilizarse para
	proporcionar información de depuración */
	pthread_mutex_lock(&mutex);
	esperando_parada[origen]++;
	printf("%d esperando en la parada %d. \n", id_usuario, origen);

	while((n_ocupantes < MAX_USUARIOS) && (estado == EN_PARADA) 
	&& parada_actual != origen){
		pthread_cond_wait(&varParada, &mutex);
	}

	printf("%d se ha subido al bus en la parada %d \n", id_usuario, origen);
	n_ocupantes++;
	esperando_parada[origen]--;

	if(esperando_bajar[origen] == 0 && esperando_parada[origen] == 0){
		pthread_cond_signal(&varBusPreparado);
	}

	pthread_mutex_unlock(&mutex);
}

void Bajar_Autobus(int id_usuario, int destino){
	/* El usuario indicará que quiere bajar en la parada ’destino’, esperará a que
	el autobús se pare en dicha parada y bajará. El id_usuario puede utilizarse para
	proporcionar información de depuración */

	pthread_mutex_lock(&mutex);
	esperando_bajar[destino]++;
	printf("%d esperando para bajarse en la parada %d \n", id_usuario, destino);

	while(estado == EN_PARADA && parada_actual != destino)
		pthread_cond_wait(&varParada, &mutex);
	
	printf("%d se ha bajado en la parada %d \n", id_usuario, destino);
	n_ocupantes--;
	esperando_bajar[destino]--;

	if(esperando_bajar[destino] == 0 && esperando_parada[destino] == 0)
		pthread_cond_signal(&varBusPreparado);
	
	pthread_mutex_unlock(&mutex);
}

void Usuario(int id_usuario, int origen, int destino) {
	// Esperar a que el autobus esté en parada origen para subir
	Subir_Autobus(id_usuario, origen);
	// Bajarme en estación destino
	Bajar_Autobus(id_usuario, destino);
}

void * thread_autobus(void * args) {
	while (1) {
		// esperar a que los viajeros suban y bajen
		Autobus_En_Parada();
		// conducir hasta siguiente parada
		Conducir_Hasta_Siguiente_Parada();
	}
}

void * thread_usuario(void * arg) {
	int id_usuario = (unsigned long int)arg;
	int a, b;
	// obtener el id del usario
	while (1) {
		a=rand() % N_PARADAS;

		do{
			b=rand() % N_PARADAS;
		} while(a==b);

		//printf("\n--- El usuario [%d] ha decidido hacer la ruta [%d] -> [%d].\n", id_usuario, a, b);
		Usuario(id_usuario,a,b);
	}
}

// void inicializarSimulacion(pthread_mutex_t mutex, pthread_cond_t varBusPreparado, pthread_cond_t varParada){
// 	pthread_mutex_init(&mutex, NULL);
// 	pthread_cond_init(&varBusPreparado, NULL);
// 	pthread_cond_init(&varParada, NULL);
// }

int main(int argc, char *argv[]){
	int i;

	pthread_create(&bus, NULL, thread_autobus, NULL); //thread_autobus es fucnión

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&varBusPreparado, NULL);
	pthread_cond_init(&varParada, NULL);
	
	for (i = 0; i < USUARIOS; i++)
		pthread_create(&pasajeros[i], NULL, thread_usuario, (void *)i); 
	


	pthread_join(bus, NULL);

	for (i = 0; i < USUARIOS; i++)
		pthread_join(pasajeros[i], NULL);
	

	return 0;
}

