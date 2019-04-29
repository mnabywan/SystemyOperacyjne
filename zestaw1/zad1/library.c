#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


//char *actual_directory;
//char *actual_file;
//char *temporary_file = "tmp_file";

struct my_array *create_my_array (int number_of_blocks){
    if (number_of_blocks < 0){
        return NULL;
    }

    struct my_array *array = malloc(sizeof(struct my_array));
    array->number_of_blocks = number_of_blocks;
   // array->current_number_of_elements = 0;
 
    char **res = calloc(number_of_blocks, sizeof(char *));
    array-> arr = res;
    return array;
}


void remove_block(struct my_array *array, int block_to_delete){
    if (array->number_of_blocks < block_to_delete || array->arr[block_to_delete] == NULL ){
        return;
    }

    free(array -> arr[block_to_delete]);
    array->arr[block_to_delete] = NULL;
}




void  search_directory(struct my_array * array, char * dir1, char *file1, char* temp){
    char find[255] = "find ";
    char *name = " -name ";
    strcat(find, dir1);
    strcat(find, name);
    strcat(find,"\"");
    strcat(find, file1);
    strcat(find, "\"");
    strcat(find, " > ");
    strcat(find,temp);
   
    system(find);
 
    printf("\n");

}  



//void
int add_block(struct my_array *array, char *file) {//zmiana
    FILE *f = fopen(file,"r+");   // read
    if(f==NULL){
        printf("File is empty, cannot be open");
        
    }
    //int len = file_size(file);
    fseek(f, 0, SEEK_END);
    int file_size = ftell(f);
    printf("%d", file_size);
    fseek(f, 0, SEEK_SET);

    char *block = calloc(file_size, sizeof(char));
    char c;
    int j=0;
    while((c = fgetc(f)) != EOF){
        block[j] = c;
        j++;
    }
    

    int i = 0;
    for (i = 0; i <= array->number_of_blocks; i++){
        if(array -> arr[i] == NULL){
            array->arr[i] = calloc(file_size, sizeof(char));
            strcpy(array->arr[i],block);
            return i;       
        }
    }
    return -1;
}



void free_my_array(struct my_array * array){
    if(array==NULL) return;
 
    if(array->arr!=NULL){
      for(int i = 0 ; i<array-> number_of_blocks ; i++){
        if(array->arr[i]!=NULL){
          free(array->arr[i]);
        }
      }
        free(array->arr);
    }
    free(array);
    array =NULL;
}

void print_my_array(struct my_array * array){
     for(int i=0; i<array->number_of_blocks; i++){
        printf("\n");
        printf("%i            %s",i,array->arr[i]);
   }
}






