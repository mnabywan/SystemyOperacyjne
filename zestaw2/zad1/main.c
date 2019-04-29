#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "files.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <time.h>
#include <sys/times.h>

//#define 

double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}


int main(int argc, char ** argv){


    clock_t operation_real_time = 0;
    clock_t operation_system_time = 0;
    clock_t operation_user_time = 0;
    struct tms time_start;
    struct tms time_stop;
    clock_t real_time_start;
    clock_t real_time_stop;

    real_time_start = times(&time_start);
    //struct tms start_tms, end_tms;
    //struct timespec start_ts, end_ts;

    if (strcmp(argv[1],"generate") == 0){
        if (argc < 5){
            printf("Wrong number of arguments");
            return 1;
        }
        char *file = argv[2];
        int records = (int) strtol(argv[3], NULL, 10);
        if (records < 0){
            printf("Number of records must be positive");
            return 2;   
        }

        int bytes = (int) strtol(argv[4], NULL, 10);
        if (bytes < 0){
            printf("Bytes must be positive");
            return 3;
        }
        generate(file, records, bytes);

    }
     else if (strcmp(argv[1], "sort") == 0){
        if (argc < 6){
            printf("Wrong number of arguments");
            return 1;
        }
        char *file = argv[2];
        int records = (int) strtol(argv[3], NULL, 10);
        if (records < 0){
            printf("Number of records must be positive");
            return 2;   
        }

        int bytes = (int) strtol(argv[4], NULL, 10);
        if (bytes < 0){
            printf("Bytes must be positive");
            return 3;
        }

        

        if(strcmp(argv[5], "lib") == 0){
            lib_sort(file, records, bytes);
        }

        else if (strcmp(argv[5], "sys") == 0){
            sys_sort(file, records, bytes);
        }

        else {
            printf("Bad mode");
            return 5;
        }
     }

     else if (strcmp(argv[1], "copy") == 0){
        if (argc < 7){
            printf("Wrong number of arguments");
            return 1;
        }

        char * file_src = argv[2];
        char * file_dst = argv[3];

        int records = (int) strtol(argv[4], NULL, 10);
        if (records < 0){
            printf("Number of records must be positive");
            return 2;   
        }

        int bytes = (int) strtol(argv[5], NULL, 10);
        if (bytes < 0){
            printf("Bytes must be positive");
            return 3;
        }

        

        if(strcmp(argv[6], "lib") == 0){
            copy_lib(file_src, file_dst, records,bytes);
        }

        else if (strcmp(argv[6], "sys") == 0){
            copy_sys(file_src , file_dst, records, bytes);
        }

        else {
            printf("BAd mode");
            return 5;
        }
     }

     else {
         printf("BAd command");
         return -1;
     }


    double clock_ticks_per_sec = sysconf(_SC_CLK_TCK); 
    real_time_stop = times(&time_stop);
    operation_system_time = (time_stop.tms_stime - time_start.tms_stime);
    operation_user_time = (time_stop.tms_utime - time_start.tms_utime);
    operation_real_time = (real_time_stop - real_time_start);

    printf("\n%s","Wywowałanie programu: ");
    for( int i=0; i<argc; i++ )
    {
        printf("%s%s",argv[i]," ");
    }
    printf("%s\n"," ");
    printf("Czas rzeczywisty: %lu %s %f %s\n", 
        operation_real_time,"      ",(operation_real_time / clock_ticks_per_sec)," [s]");
    printf("Czas w trybie systemowym: %lu %s %f %s\n", operation_system_time,
    "      ",(operation_system_time / clock_ticks_per_sec)," [s]");
    printf("Czas w trybie uzytkownika: %lu %s %f %s\n", operation_user_time,
    "      ",(operation_user_time / clock_ticks_per_sec)," [s]");
    


}






