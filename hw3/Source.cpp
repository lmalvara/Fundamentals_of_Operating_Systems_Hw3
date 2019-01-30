#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#define Tsleep 5
#define NCars 128
using namespace std;

struct carData {
	int id;
	int arrive;
	int cross;
	string dir;

};

void *tunnel(void * arg); 
void *car(void * arg); 

static pthread_mutex_t direction_lock;
static pthread_mutex_t myMutex;
static pthread_cond_t car_can; 

static string direction = "";
static int TunnelMax = 0;
static int NumAllCars = 0;
static int NumCurrentCars = 0;
static int NumBBCars = 0;
static int NumWBCars = 0;
static int NumWaitCars = 0;



int main() 
{
	carData cars[NCars];

	pthread_mutex_init(&direction_lock, NULL);
	pthread_mutex_init(&myMutex, NULL);
	car_can = PTHREAD_COND_INITIALIZER;

	pthread_t car_tids[NCars];
	pthread_t tunnel_tid;

	cin >> TunnelMax;

	pthread_create(&tunnel_tid, NULL, tunnel, NULL);

	while (cin >> cars[NumAllCars].arrive >> cars[NumAllCars].dir >> cars[NumAllCars].cross)
	{
		sleep(cars[NumAllCars].arrive);
		cars[NumAllCars].id = NumAllCars + 1;
		pthread_create(&car_tids[NumAllCars], NULL, car, (void*)&cars[NumAllCars]);
		NumAllCars++;
	}

	for (int i = 0; i < NumAllCars; i++)
		pthread_join(car_tids[i], NULL);


	cout << NumBBCars << " car(s) going to Bear Valley arrived at the tunnel." << endl;
	cout << NumWBCars << " car(s) going to Whittier arrived at the tunnel." << endl;
	cout << NumWaitCars << " car(s) were delayed." << endl;
	return 0;
}
void * tunnel(void * arg)
{
	while (1)
	{
		pthread_mutex_lock(&direction_lock);
		direction = "WB";
		cout << "The tunnel is now open for Whittier-bound traffic." << endl;
		pthread_cond_broadcast(&car_can);
		pthread_mutex_unlock(&direction_lock);
		sleep(Tsleep);

		pthread_mutex_lock(&direction_lock);
		direction = "No";
		cout << "The tunnel is now closed to ALL traffic." << endl;
		pthread_mutex_unlock(&direction_lock);
		sleep(Tsleep);

		pthread_mutex_lock(&direction_lock);
		direction = "BB";
		cout << "The tunnel is now open for Big Bear-bound traffic." << endl;
		pthread_cond_broadcast(&car_can);
		pthread_mutex_unlock(&direction_lock);
		sleep(Tsleep);

		pthread_mutex_lock(&direction_lock);
		direction = "No";
		cout << "The tunnel is now closed to ALL traffic." << endl;
		pthread_mutex_unlock(&direction_lock);
		sleep(Tsleep);
	}
}
void *car(void * arg) 
{
	carData * currentCar = (struct carData *) arg;
	bool delayed = false;

	pthread_mutex_lock(&myMutex);
	if (currentCar->dir == "WB")
		cout << "Car # " << currentCar->id << " going to Whittier arrives at the tunnel" << endl;
	else
		cout << "Car # " << currentCar->id << " going to Bear Valley arrives at the tunnel" << endl;
	pthread_mutex_unlock(&myMutex);

	pthread_mutex_lock(&myMutex);
	while (currentCar->dir != direction || NumCurrentCars == TunnelMax)
	{
		if (currentCar->dir == direction && NumCurrentCars == TunnelMax)
			delayed = true;
		pthread_cond_wait(&car_can, &myMutex);
	}
	if (delayed == true)
		NumWaitCars++;
	NumCurrentCars++;
	if (currentCar->dir == "WB")
		cout << "Car # " << currentCar->id << " going to Whittier enters at the tunnel" << endl;
	else
		cout << "Car # " << currentCar->id << " going to Bear Valley enters at the tunnel" << endl;
	pthread_mutex_unlock(&myMutex);

	sleep(currentCar->cross);

	pthread_mutex_lock(&myMutex);
	NumCurrentCars--;
	if (direction == "WB")
		pthread_cond_broadcast(&car_can);
	else if (direction == "BB")
		pthread_cond_broadcast(&car_can);
	if (currentCar->dir == "WB")
	{
		cout << "Car # " << currentCar->id << " going to Whittier exits at the tunnel" << endl;
		NumWBCars++;
	}
	else
	{
		cout << "Car # " << currentCar->id << " going to Bear Valley exits at the tunnel" << endl;
		NumBBCars++;
	}
	pthread_mutex_unlock(&myMutex);

	pthread_exit(NULL);
}

