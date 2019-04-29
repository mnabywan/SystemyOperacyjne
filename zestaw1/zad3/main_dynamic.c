#include <dlfcn.h>

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/times.h>
#include <string.h>
#include <zconf.h>
#include <time.h>
#include <sys/times.h>
#include <math.h>

void * handle;
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

struct my_array *(* create_my_array)(int);
char * (*create_block)(struct my_array * , char *);
int (*add_block)(struct my_array *, char * );
void (*print_my_array)(struct my_array *);
void (*free_my_array)(struct my_array *);
void (*search_directory)(struct my_array *, char *,char *, char *);
void (*remove_block)(struct my_array *, int );



void load_library(){
   
    handle = dlopen("../zad1/library.so", RTLD_LAZY);

    if (handle == NULL){
        printf("Error in handle");
        exit(1);
    }

    char *error = dlerror();

    search_directory = (void (*)(struct my_array*,char * , char *, char *))dlsym(handle, "search_directory");
    create_my_array = (struct my_array * (*)(int ))dlsym(handle, "create_my_array");
    add_block = (int (*) (struct my_array*, char*))dlsym(handle,"add_block");
    create_block = (char * (*)(struct my_array *, char *))dlsym(handle,"create_block");
    print_my_array = (void (*)(struct my_array *))dlsym(handle, "print_my_array");
    free_my_array = (void (*) (struct my_array *))dlsym(handle, "free_my_array");
    remove_block = (void (*) (struct my_array *, int))dlsym(handle, "remove_block");
   
    if(error != NULL){
        printf("Error in handle");
        return;
    }


    if(!handle){
        printf("Cannot do it");
        exit(1);
    }
    
}

int main(int argc, char **argv) {
    
    load_library();
  
    FILE * f = fopen("zad3.txt", "a+");
    

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
    struct my_array * array;
    int number_of_blocks = (int) strtol(argv[i], NULL, 10);
    
    if(number_of_blocks <= 0){
        printf("Number of blocks of array must be positve");
        exit(1);
    }


    array = (*create_my_array)(number_of_blocks);

    i=2;

    while(i<argc){
    

       if((strcmp(argv[i],"create_table") == 0)){
            int number_of_blocks = (int) strtol(argv[i+1], NULL, 10);
            if(number_of_blocks <= 0){
                printf("Number of blocks of array must be posiutuve");
                exit(1);
            }
            free_my_array(array);
          
            times(&st);   
            clock_gettime(CLOCK_REALTIME, &real_st);
            array = (*create_my_array)(number_of_blocks);
            times(&en);
            clock_gettime(CLOCK_REALTIME, &real_en);

            fprintf(f,"\n");
            fprintf(f,"Creating table ");
            fprintf(f,"Real time: %f ", calculate_realtime(&real_st, &real_en));
            fprintf(f,"CPU time: %Lf ", calculate_time(st.tms_stime, en.tms_stime));
            fprintf(f, "User time: %Lf ", calculate_time(st.tms_utime, en.tms_utime));


            printf("\n");
            printf("Creating table");
            printf("Real time: %f ", calculate_realtime(&real_st, &real_en));
            printf("CPU time: %Lf", calculate_time(st.tms_stime, en.tms_stime));
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
            clock_gettime(CLOCK_REALTIME, &real_st);  
            
            
             (*search_directory)(array, directory1, file1, temp1);
            
           
         
            clock_gettime(CLOCK_REALTIME, &real_en);  
            times(&en);

           
        
            fprintf(f,"\n");
            fprintf(f,"Searching for file: %s ",argv[i+2]);
            fprintf(f,"Real time: %f ", calculate_realtime(&real_st, &real_en));
            fprintf(f,"CPU time: %Lf ", calculate_time(st.tms_stime, en.tms_stime));
            fprintf(f,"User time: %Lf ", calculate_time(st.tms_utime, en.tms_utime));


            printf("\n");
            printf("Searching for file: %s ",argv[i+2]);
            printf("Real time: %f ", calculate_realtime(&real_st, &real_en));
            printf("CPU time: %Lf ", calculate_time(st.tms_stime, en.tms_stime));
            printf("User time: %Lf ", calculate_time(st.tms_utime, en.tms_utime));

            times(&st); 
            clock_gettime(CLOCK_REALTIME, &real_st);

            (*add_block)(array,temp1);
           
            clock_gettime(CLOCK_REALTIME, &real_en);                  
            times(&en);
            fprintf(f,"\n");
            fprintf(f,"Adding to arrray:\n");
            fprintf(f,"Real time: %f ", calculate_realtime(&real_st, &real_en));
            fprintf(f,"CPU time: %Lf ", calculate_time(st.tms_stime, en.tms_stime));
            fprintf(f,"User time: %Lf ", calculate_time(st.tms_utime, en.tms_utime));


            printf("Adding to arrray:\n");
            printf("Real time: %f ", calculate_realtime(&real_st, &real_en));
            printf("CPU time: %Lf ", calculate_time(st.tms_stime, en.tms_stime));
            printf("User time: %Lf ", calculate_time(st.tms_utime, en.tms_utime));


            i=i+4;
       }
         

       else if(strcmp(argv[i],"remove_block") == 0){
            int block_to_remove = (int) strtol(argv[i+1], NULL, 10);
           

            times(&st); 
            clock_gettime(CLOCK_REALTIME, &real_st); 
            
            remove_block(array, block_to_remove);

            clock_gettime(CLOCK_REALTIME, &real_en);                  
            times(&en);
            fprintf(f,"\n");
            fprintf(f,"Remove %s block: ",argv[i+1]);
            fprintf(f,"Real time: %f ", calculate_realtime(&real_st, &real_en));
            fprintf(f,"CPU time: %Lf ", calculate_time(st.tms_stime, en.tms_stime));
            fprintf(f,"User time: %Lf ", calculate_time(st.tms_utime, en.tms_utime));

            printf("Remove %d block:\n",i);   
            printf("Real time: %f ", calculate_realtime(&real_st, &real_en));
            printf("CPU time: %Lf ", calculate_time(st.tms_stime, en.tms_stime));
            printf("User time: %Lf ", calculate_time(st.tms_utime, en.tms_utime));
           
          
             i+=2;
       }

       else{
           printf("Unknown command");
           exit(1);
       }
       
   }
   
    fclose(f);
    //print_my_array(array);
    dlclose(handle);
    
}

