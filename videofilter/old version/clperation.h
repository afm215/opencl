#ifndef CLPERATION_H
#define CLPERATION_H 
#include "gpuenv.h"
void checkError(int status, const char* msg);
int appliedFilter(unsigned char *input_picture, unsigned int line, unsigned int colonne,unsigned char *output, float *kernel_matn, GpuEnvironnment &env);
void hello();//test that .cpp are indeed linked
#endif