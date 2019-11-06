#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <mqueue.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

typedef struct Passenger{
    int id;
    int cart;
} Passenger;

typedef struct Car{
    int id;
    int size;
    int raids;
    Passenger* passIds;
} Car;

int carsCount;
int carSize;
pthread_cond_t* carsCond;
pthread_mutex_t* carsMutex;

Car* cars;
int stationCartID;

#endif //UTILS_H