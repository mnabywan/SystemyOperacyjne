

#include "files.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <zconf.h>
#include <unistd.h>
#include <string.h>


char *random_str(int bytes){
   
    char *rand_str = calloc(bytes+1 , sizeof(char));
    
    
    for (int i =0; i< bytes; i++){
       
        rand_str[i] = 'a' + rand()%26;
    }

    rand_str[bytes] = 10;
    return rand_str;


}


void generate(char *file_name, int records, int bytes){
    FILE * file = fopen(file_name, "w+");
    if(file==NULL){
        printf("File is empty, cannot be open");
        return;
    }

    for (int i =0; i<records; i++){
        char *buff = random_str(bytes); 
        fwrite(buff, sizeof(char), (size_t) bytes + 1, file);
    }
    fclose(file);
    
}

void lib_sort(char *file_name, int records,int bytes){
    
    char* buff = (char*)calloc((bytes+1), sizeof(char));
    char* buff1 = (char*)calloc((bytes+1), sizeof(char));

    FILE * file;
    int offset = (int) ((bytes+1)*sizeof(char));
    file = fopen(file_name, "r+");
    if (file == NULL){
        printf("Cannot open");
        return;
    }
    int i;
    
    for (i =0; i<records; i++){
        
        fseek(file, i* offset, 0);
        int j;
        int k = i;
        char min_char = 'z';
        for (j =i; j<records; j++){
            fseek(file, j * offset, 0);
            fread(buff, sizeof(char), (bytes+1), file);
            
            if (buff[0]<= min_char){
                min_char = buff[0];
                k = j;
            }
        }
        if (k !=i){
            fseek(file,k*offset,0);
            fread(buff,sizeof(char),(bytes+1),file);
            fseek(file,i*offset,0);
            fread(buff1,sizeof(char),(bytes+1),file);
            fseek(file,k*offset,0);
            fwrite(buff1,sizeof(char),(bytes+1),file);
            fseek(file,i*offset,0);
            fwrite(buff,sizeof(char),(bytes+1),file);
        }
    }
    free(buff1);
    free(buff);
    fclose(file);
}


void sys_sort(char *file_name, int records,int bytes){
    int file = open(file_name, O_RDWR);

    char* buff = (char*)calloc((bytes+1), sizeof(char));
    char* buff1 = (char*)calloc((bytes+1), sizeof(char));
    int offset = (int) ((bytes+1)*sizeof(char));

    for(int i =0; i< records; i++){
        lseek(file, i*offset, 0);
        int j;
        int k = i;
        char min_char = 'z';
        
        for (j=i; j<records; j++){
            lseek(file, j * offset, 0);
            read(file, buff, (bytes+1));
            

            if (buff[0]<= min_char){
                min_char = buff[0];
                k = j;
            }
        }
        if (k !=i){
            lseek(file,k*offset,0);
            read(file, buff,sizeof(char)*(bytes+1));
            lseek(file,i*offset,0);
            read(file, buff1,sizeof(char) *(bytes+1));
            lseek(file,k*offset,0);
            write(file, buff1,sizeof(char)*(bytes+1));
            lseek(file,i*offset,0);
            write(file, buff,sizeof(char)*(bytes+1));
            
        }
    }
close(file);
}      
    

void copy_lib(char *file_src ,char *file_dst, int  records, int  bytes){
    FILE * src = fopen(file_src, "r+");
    FILE * dst = fopen(file_dst, "w+");
     if (src == NULL){
        printf("Cannot open src file");
        return;
    }
    

    if (dst == NULL){
        printf("Cannot open dst file");
        return;
    }
    
    char * buff1 = (char*)calloc(bytes, sizeof(char));
    for (int i = 0; i<=records; i++){
        fread(buff1,sizeof(char),bytes,src);
        fwrite(buff1,sizeof(char), bytes, dst);
    }
    fclose(src);
    fclose(dst);
    free(buff1);
    
    
}





void copy_sys(char *file_src ,char *file_dst, int  records, int  bytes){
    int src = open(file_src, O_RDWR);
    int dst = open(file_dst, O_RDWR | O_CREAT);
    
    char * buff1 = (char*)calloc(bytes, sizeof(char));
    for (int i = 0; i<records; i++){
        read(src, buff1,sizeof(char)*bytes);
        write(dst,buff1,sizeof(char)* bytes);
    }

    close(src);
    close(dst);
    free(buff1);
}
//void generate(char *file_name, int records, int bytes){return;}



    
