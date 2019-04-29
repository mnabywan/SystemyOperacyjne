#include <stdio.h>
#ifndef SYSOPY_LIBRARY_H
#define SYSOPY_LIBRARY_H

struct my_array{
    int number_of_blocks;
    char **arr;
};

struct my_array * create_my_array (int number_of_blocks);
void remove_block(struct my_array *array, int block_to_delete);

void actualise_searched_directory(char *directory);
char *create_block(struct my_array * array, char *file);
int add_block(struct my_array * array, char * file);
//void * search_for_file()
void actualise_searched_file(char * file);
void actualise_temp_file( char *file);
void search_directory(struct my_array * array,char *dir1,char * file1, char* temp); //UWAGA
void print_my_array(struct my_array * array);
void free_my_array(struct my_array * array);

#endif



