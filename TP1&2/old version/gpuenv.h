#ifndef GPUENV_H
#define GPUENV_H

#include "defineMacro.h"


class GpuEnvironnment {
public:
	cl_kernel kernel;
	cl_program program;
	cl_context context;
	cl_command_queue queue;

};




/*********DEBUG FUNCTION ***************/


const char* getErrorString(cl_int error);
void print_clbuild_errors(cl_program program, cl_device_id device);
unsigned char** read_file(const char* name);

#endif // !GPUENV_H