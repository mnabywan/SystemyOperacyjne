#include <stdbool.h>

void copy_lib(char *file_src ,char *file_dst, int  records, int  bytes);
void copy_sys(char *file_src ,char *file_dst, int  records, int  bytes);
void generate(char *file_name, int records, int bytes);
void lib_sort(char *file_name, int records,int bytes); //fread fwrite
void sys_sort(char *file_name, int records,int bytes); // read write
//void sort_lib(const char* filepath, int record_length, int records_number);
//generate(file, records, bytes)
//generate(file, records, bytes)