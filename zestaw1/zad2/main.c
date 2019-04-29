
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/times.h>
#include <string.h>
#include <zconf.h>
#include <time.h>
#include <sys/times.h>
#include "../zad1/library.h"
#include <math.h>


char * temp1;
char * directory1;
char * file1;


double calculate_realtime(struct timespec * start, struct  timespec * stop){
    
    double res = (double) (stop->tv_sec - start->tv_sec);
    res += (double) (stop->tv_nsec - start->tv_nsec) * pow(10,-9);
    return res;
}

long double calculate_time(clock_t start, clock_t end) {
    long double time = (long double) (end - start) / sysconf(_SC_CLK_TCK);
    return time;
}

 


int main(int argc, char **argv) {

    FILE * f = fopen("raport2.txt", "a+");
    
    int i;

    struct tms st;
    struct tms en;
   
    struct timespec real_st;
    struct timespec real_en;
    
    if (argc <= 1){
        printf("You have to give at least 1 argument");
        return -1;
    }
     
    i = 1;
    struct my_array * array = NULL;
    int number_of_blocks = (int) strtol(argv[i], NULL, 10);
    
    if(number_of_blocks <= 0){
        printf("Number of blocks of array must be positve");
        exit(1);
    }

    times(&st);   
    clock_gettime(CLOCK_REALTIME, &real_st);

    array = create_my_array(number_of_blocks); 
    times(&en);
    clock_gettime(CLOCK_REALTIME, &real_en);

    fprintf(f,"\n");
    fprintf(f,"Creating table");

    fprintf(f,"Real time: %f: ", calculate_realtime(&real_st, &real_en));
    fprintf(f,"CPU time: %Lf: ", calculate_time(st.tms_stime, en.tms_stime));
    fprintf(f, "User time: %Lf: ", calculate_time(st.tms_utime, en.tms_utime));


    printf("\n");
    printf("Creating table");
    printf("Real time: %f ", calculate_realtime(&real_st, &real_en));
    printf("CPU time: %Lf ", calculate_time(st.tms_stime, en.tms_stime));
    printf( "User time: %Lf ", calculate_time(st.tms_utime, en.tms_utime));

    i=2;
    while(i<argc){
       if((strcmp(argv[i],"create_table") == 0)){
            int number_of_blocks = (int) strtol(argv[i+1], NULL, 10);
            if(number_of_blocks <= 0){
                printf("Number of blocks of array must be posituve");
                exit(1);
            }
            free_my_array(array);
          
            times(&st);   
            clock_gettime(CLOCK_REALTIME, &real_st);
            array = create_my_array(number_of_blocks);
            times(&en);
            clock_gettime(CLOCK_REALTIME, &real_en);

            fprintf(f,"\n");
            fprintf(f,"Creating table");
            fprintf(f,"Real time: %f ", calculate_realtime(&real_st, &real_en));
            fprintf(f,"CPU time: %Lf ", calculate_time(st.tms_stime, en.tms_stime));
            fprintf(f, "User time: %Lf ", calculate_time(st.tms_utime, en.tms_utime));



            printf("\n");
            printf("Creating table");
            printf("Real time: %f ", calculate_realtime(&real_st, &real_en));
            printf("CPU time: %Lf ", calculate_time(st.tms_stime, en.tms_stime));
            printf( "User time: %Lf ", calculate_time(st.tms_utime, en.tms_utime));


            i+=2;
       }




       else if(strcmp(argv[i],"search_directory") == 0){
            char * directory = argv[i+1];
            char * file = argv[i+2];
            char * t1 = argv[i+3];

            directory1 = directory;
            file1 = file;
            temp1 = t1;
            
            times(&st);  
            
            search_directory(array,directory1,file1, temp1); //UWAGA

            times(&en);
            clock_gettime(CLOCK_REALTIME, &real_en);
            
            fprintf(f,"\n");
            fprintf(f,"Searching directory %s for file: %s ",argv[i+1] , argv[i+2] ) ;
            fprintf(f,"Real time: %f ", calculate_realtime(&real_st, &real_en));
            fprintf(f,"CPU time: %Lf ", calculate_time(st.tms_stime, en.tms_stime));
            fprintf(f,"User time: %Lf ", calculate_time(st.tms_utime, en.tms_utime));


            printf("\n");
            printf("Searching directory %s for file: %s ",argv[i+1] , argv[i+2] ) ;  
            printf("Real time: %f ", calculate_realtime(&real_st, &real_en));
            printf("CPU time: %Lf ", calculate_time(st.tms_stime, en.tms_stime));
            printf("User time: %Lf ", calculate_time(st.tms_utime, en.tms_utime));

            times(&st); 
            clock_gettime(CLOCK_REALTIME, &real_st); 
            
            add_block(array,temp1);

            clock_gettime(CLOCK_REALTIME, &real_en);                  
            times(&en);

            fprintf(f,"\n");
            fprintf(f,"Adding to arrray: ");
            fprintf(f,"Real time: %f ", calculate_realtime(&real_st, &real_en));
            fprintf(f,"CPU time: %Lf ", calculate_time(st.tms_stime, en.tms_stime));
            fprintf(f,"User time: %Lf ", calculate_time(st.tms_utime, en.tms_utime));

            printf("\n");    
            printf("Adding to arrray: ");
            printf("Real time: %f ", calculate_realtime(&real_st, &real_en));
            printf("CPU time: %Lf ", calculate_time(st.tms_stime, en.tms_stime));
            printf("User time: %Lf", calculate_time(st.tms_utime, en.tms_utime));


            i=i+4;
       }
         
       

       else if(strcmp(argv[i],"remove_block") == 0){
            int block_to_remove = (int) strtol(argv[i+1], NULL, 10);
            i+=2;

            times(&st); 
            clock_gettime(CLOCK_REALTIME, &real_st); 
            remove_block(array,block_to_remove);
            clock_gettime(CLOCK_REALTIME, &real_en);                  
            times(&en);
            fprintf(f,"\n");
            fprintf(f,"Remove %s block: ",argv[i+1]);
            fprintf(f,"Real time: %f ", calculate_realtime(&real_st, &real_en));
            fprintf(f,"CPU time: %Lf ", calculate_time(st.tms_stime, en.tms_stime));
            fprintf(f,"User time: %Lf ", calculate_time(st.tms_utime, en.tms_utime));


            printf("\n");
            printf("Remove %d block: ",i);   
            printf("Real time: %f ", calculate_realtime(&real_st, &real_en));
            printf("CPU time: %Lf ", calculate_time(st.tms_stime, en.tms_stime));
            printf("User time: %Lf ", calculate_time(st.tms_utime, en.tms_utime));
            
        
       }

       else{
           printf("Unknown command");
           exit(1);
       }
       
   }
   fclose(f);




}
    

    