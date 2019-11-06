#include "utils.h"


pthread_mutex_t stationMutex;
pthread_mutex_t passengerMutex;
pthread_mutex_t emptyCarMutex;
pthread_mutex_t fullCarMutex;
pthread_cond_t emptyCondition;
pthread_cond_t fullCondition;

struct timeval getCurrentTime() {
    struct timeval curr;
    gettimeofday(&curr, NULL);
    return curr;
}

void *passengerFunction(void *data) {
    Passenger *passenger = (Passenger *) data;
    while (1) {
        pthread_mutex_lock(&passengerMutex);

        passenger->cart = stationCartID;
        cars[stationCartID].passIds[cars[stationCartID].size] = *passenger;
        cars[stationCartID].size = cars[stationCartID].size + 1;
        struct timeval curr = getCurrentTime();
        printf("Passenger in -- ID: %d People in car: %d TIME: %ld.%06ld \n", passenger->id,cars[stationCartID].size, curr.tv_sec, curr.tv_usec);

        if(cars[stationCartID].size == carSize){
            srand(time(NULL));
            curr = getCurrentTime();
            printf("Passenger pressed START -- ID: %d People in car: %d TIME: %ld.%06ld\n", cars[stationCartID].passIds[rand()%cars->size].id,cars[stationCartID].size, curr.tv_sec, curr.tv_usec);
            pthread_cond_signal(&fullCondition);
            pthread_mutex_unlock(&fullCarMutex);
        } else {
            pthread_mutex_unlock(&passengerMutex);
        }

        pthread_mutex_lock(&carsMutex[passenger->cart]);
        curr = getCurrentTime();
        cars[stationCartID].size--;
        printf("Passenger left -- ID: %d People in car: %d TIME: %ld.%06ld \n", passenger->id, cars[stationCartID].size,curr.tv_sec, curr.tv_usec);
        if(cars[stationCartID].size == 0){
            pthread_cond_signal(&emptyCondition);
            pthread_mutex_unlock(&emptyCarMutex);
        }
        pthread_mutex_unlock(&carsMutex[passenger->cart]);
        passenger->cart = -1;
    }
}

void *carFunction(void *data) {
    Car *car = (Car *) data;
    if (car->id == 0)
        pthread_mutex_lock(&passengerMutex);

    for (int i = 0; i < car->raids; i++) {
        pthread_mutex_lock(&stationMutex);
        if (car->id != stationCartID) {
            pthread_cond_wait(&carsCond[car->id], &stationMutex);
        }
        struct timeval curr = getCurrentTime();
        printf("Car come --  ID: %d TIME: %ld.%06ld\n", car->id, curr.tv_sec, curr.tv_usec);

        if (i != 0) {
            pthread_mutex_unlock(&carsMutex[car->id]);
            pthread_cond_wait(&emptyCondition, &emptyCarMutex);
        }

        pthread_mutex_lock(&carsMutex[car->id]);
        pthread_mutex_unlock(&passengerMutex);
        pthread_cond_wait(&fullCondition, &fullCarMutex);

        curr = getCurrentTime();
        printf("Car full -- ID: %d TIME: %ld.%06ld\n", car->id, curr.tv_sec, curr.tv_usec);
        stationCartID = (stationCartID + 1) % carsCount;

        pthread_cond_signal(&carsCond[stationCartID]);
        pthread_mutex_unlock(&stationMutex);
    }

    pthread_mutex_lock(&stationMutex);

    if(car->id != stationCartID) {
        pthread_cond_wait(&carsCond[car->id], &stationMutex);
    }

    struct timeval curr = getCurrentTime();
    printf("Car come -- ID: %d TIME: %ld.%06ld\n", car->id, curr.tv_sec, curr.tv_usec);

    stationCartID = car->id;

    pthread_mutex_unlock(&carsMutex[car->id]);
    pthread_cond_wait(&emptyCondition,&emptyCarMutex);

    stationCartID = (stationCartID + 1)%carsCount;

    curr = getCurrentTime();
    printf("Car end run --  ID: %d TIME: %ld.%06ld\n", car->id, curr.tv_sec, curr.tv_usec);

    pthread_cond_signal(&carsCond[stationCartID]);
    pthread_mutex_unlock(&stationMutex);

    pthread_exit(NULL);
}




int main(int argc, char **argv) {


    if (argc != 5){
        printf("Wrong num of arguments");
        exit(1);
    }


    int passengersCount = atoi(argv[1]);
    carsCount = atoi(argv[2]);
    carSize = atoi(argv[3]);
    int raidCount = atoi(argv[4]);

    if (passengersCount <= 0 || carsCount <= 0 || carSize <= 0 || raidCount <= 0){
        printf("Bad args");
        exit(1);
    }

    stationCartID = 0;
    pthread_t passThr[passengersCount];
    pthread_t cartThr[carsCount];
    Passenger passengers[passengersCount];
    cars = malloc(sizeof(Car) * carsCount + sizeof(int) * passengersCount);
    carsMutex = malloc(sizeof(pthread_mutex_t) * carsCount);
    carsCond = malloc(sizeof(pthread_cond_t) * carsCount);

    pthread_mutex_init(&stationMutex, NULL);
    pthread_mutex_init(&emptyCarMutex, NULL);
    pthread_mutex_init(&passengerMutex, NULL);
    pthread_mutex_init(&fullCarMutex, NULL);
    pthread_cond_init(&emptyCondition, NULL);
    pthread_cond_init(&fullCondition, NULL);

    for (int i = 0; i < passengersCount; i++) {
        passengers[i].id = i;
        passengers[i].cart = -1;
    }

    for (int i = 0; i < carsCount; i++) {
        cars[i].id = i;
        cars[i].size = 0;
        cars[i].raids = raidCount;
        cars[i].passIds = malloc(sizeof(Passenger) * carSize);
        pthread_mutex_init(&carsMutex[i], NULL);
        pthread_cond_init(&carsCond[i], NULL);
    }

    for (int i = 0; i < carsCount; i++) {
        pthread_create(&cartThr[i], NULL, carFunction, &cars[i]);
    }

    for (int i = 0; i < passengersCount; i++) {
        pthread_create(&passThr[i], NULL, passengerFunction, &passengers[i]);
    }

    for (int i = 0; i < carsCount; i++) {
        pthread_join(cartThr[i], NULL);
    }

    for (int i = 0; i < carsCount; i++) {
        pthread_mutex_destroy(&carsMutex[i]);
    }
    for(int i = 0; i < carsCount; i++){
        free(cars[i].passIds);
    }
    free(cars);
    free(carsMutex);
    free(carsCond);
}

