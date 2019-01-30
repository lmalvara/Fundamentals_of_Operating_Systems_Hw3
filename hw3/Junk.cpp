#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include<string>
#include<sstream>

using namespace std;

static int carMax;
static int done = 0;
static int currentCars = 0;
static int delayed = 0;
static pthread_mutex_t mutex;
static pthread_mutex_t mutex2;
static pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
string t_bound;

//struct to contain input values
struct vehicle {
	int ID;
	int arrive;
	string bound;
	int estimate;
};

// Methods to be used
void enter(vehicle *threadarg);
void leave(vehicle * threadarg);

void *thread_Run(void *threadarg);
void *tunnelThread(void *threadarg);

int main(int argc, char *argv[]) {
	int tun;
	int nVehicles = 0;
	int rc;
	int tunnel = 0;
	string tbound;
	struct vehicle car[64];
	cin >> carMax;
	
	cout << "-- Maximum number of cars in the tunnel: " << carMax << endl;

	pthread_mutex_init(&mutex, NULL);

	pthread_t threads[64];
	pthread_create(&threads[tunnel], NULL, tunnelThread, NULL);


	while (cin >> car[nVehicles].arrive >> car[nVehicles].bound >> car[nVehicles].estimate) {
	
		car[nVehicles].ID = nVehicles + 1;
		sleep(car[nVehicles].arrive);
		


		rc = pthread_create(&threads[nVehicles], NULL, thread_Run, (void *)&car[nVehicles]);
		nVehicles++;

		if (rc) {
			cout << "Error:unable to create thread," << rc << endl;
			exit(-1);
		}
	}
	
	int i;
	for (i = 0; i < nVehicles; i++) {
		pthread_join(threads[i], NULL);
	}
	done = 1;
	cout << delayed << " car(s) were delayed." << endl;

	return 0;
}

void enter(vehicle *threadarg) {
	
	if (currentCars + 1 < carMax && t_bound == threadarg->bound) {		//-------UPDATE BOUND-------
		currentCars += 1;
		cout << "Car # " << threadarg->ID << " going to " << threadarg->bound << " enters the tunnel." << endl;
		pthread_cond_signal(&empty);
	}
	else {
		delayed++;
		while ((currentCars + 1) > carMax) {
			pthread_cond_wait(&empty, &mutex);
		}
	}
}

void leave(vehicle * threadarg) {
	currentCars = currentCars - 1;
	pthread_cond_signal(&empty);
	cout << "Car # " << threadarg->ID << " going to " << threadarg->bound << " exits the tunnel." << endl;
}

void *thread_Run(void *threadarg) {

	struct vehicle *my_data;

	my_data = (struct vehicle *) threadarg;
	pthread_mutex_lock(&mutex);

	cout << "Car # " << my_data->ID << " going to " << my_data->bound << " arrives at the tunnel." << endl;


	enter(my_data);
	pthread_mutex_unlock(&mutex);
	
	sleep(my_data->arrive);		//estimate or arrive?------
	pthread_mutex_lock(&mutex);
	
	leave(my_data);
	pthread_mutex_unlock(&mutex);
	
	pthread_exit(NULL);
}

void *tunnelThread(void *threadarg)
{

	while (1)
	{
		pthread_mutex_lock(&mutex);
		t_bound = "WB";
		cout << "Tunnel bound for Whittier only\n";
		pthread_cond_broadcast(&empty);
		pthread_mutex_unlock(&mutex);
		sleep(5);

		pthread_mutex_lock(&mutex);
		t_bound = "NONE";
		cout << "Tunnel is now closed to all Traffic!\n";
		pthread_cond_broadcast(&empty);
		pthread_mutex_unlock(&mutex);
		sleep(5);

		pthread_mutex_lock(&mutex);
		t_bound = "BB";
		cout << "Tunnel bound for Bear Valley only\n";
		pthread_cond_broadcast(&empty);
		pthread_mutex_unlock(&mutex);
		sleep(5);

		pthread_mutex_lock(&mutex);
		t_bound = "NONE";
		cout << "Tunnel is closed to all Traffic!\n";
		pthread_cond_broadcast(&empty);
		pthread_mutex_unlock(&mutex);
		sleep(5);

	}
}