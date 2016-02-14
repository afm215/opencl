#ifndef CLPERATION_H
#define CLPERATION_H 
#include "gpuenv.h"
int appliedFilter(unsigned char *input_picture, unsigned int line, unsigned int colonne,unsigned char *output, float *kernel_matn, GpuEnvironnment &env);
void checkError(int status, const char* msg);
void hello();//test that .cpp are indeed linked
#endif