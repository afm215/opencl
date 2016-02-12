#include <stdio.h>
#include <stdlib.h>
#include <iostream> // for standard I/O
#include <math.h>
#include <time.h>
#include <CL/cl.h>
#include <CL/cl_ext.h>
#include "header/clperation.h"
#define STRING_BUFFER_LEN 1024
using namespace std;



/*
*I will not use MAP here to preserve compatibility although MAP woulf be more efficient with the device used.
*/
void print_clbuild_errors(cl_program program,cl_device_id device)
	{
		cout<<"Program Build failed\n";
		size_t length;
		char buffer[2048];
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &length);
		cout<<"--- Build log ---\n "<<buffer<<endl;
		exit(1);
	}

unsigned char** read_file(const char* name) {
    size_t size;
    unsigned char** output = (unsigned char**)malloc(sizeof(unsigned char*));
    FILE* fp = fopen(name, "rb");
    if (!fp) {
        printf("no such file:%s", name);
        exit(-1);
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    *output = (unsigned char*)malloc(size + 1);

    if (!*output) {
        fclose(fp);
        printf("mem allocate failure:%s", name);
        exit(-1);
    }

    if (!fread(*output, size, 1, fp)) printf("failed to read file\n");
    fclose(fp);
#ifdef DEBUG
    printf("%s", *output);
#endif
    (*output)[size] = 0;//just in case, but without this line the function will output weird characters on Windows
    return output;
}
void callback(const char *buffer, size_t length, size_t final, void *user_data)
{
     fwrite(buffer, 1, length, stdout);
}


void checkError(int status, const char *msg) {
	if(status!=CL_SUCCESS)	
		printf("%s\n",msg);
}

// Randomly generate a floating-point number between -10 and 10.
float rand_float() {
  return float(rand()) / float(RAND_MAX) * 20.0f - 10.0f;
}

void hello(){
    printf("Hello!! \n \n");
}

int appliedSobel(float *input_picture, unsigned int size, float *output, float *kernel_mat)
{
    char char_buffer[STRING_BUFFER_LEN];
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;



    //--------------------------------------------------------------------

    cl_mem picture_buf; 
    cl_mem kernel_buff;
    cl_mem output_buf; 
    int status;

    clGetPlatformIDs(1, &platform, NULL);
    clGetPlatformInfo(platform, CL_PLATFORM_NAME, STRING_BUFFER_LEN, char_buffer, NULL);
    cl_context_properties context_properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0 }; //make it run on windows
   


    context_properties[1] = (cl_context_properties)platform;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    context = clCreateContext(context_properties, 1, &device, NULL, NULL, NULL);
    queue = clCreateCommandQueue(context, device, 0, NULL);

    unsigned char** opencl_program = read_file("operation.cl");
    program = clCreateProgramWithSource(context, 1, (const char**)opencl_program, NULL, NULL);
    if (program == NULL)
    {
        printf("Program creation failed\n");
        return 1;
    }
    int success = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (success != CL_SUCCESS) print_clbuild_errors(program, device);
    kernel = clCreateKernel(program, "sobelFilter", NULL);
    // Input buffers.

    picture_buf = clCreateBuffer(context, CL_MEM_READ_ONLY,
        size * sizeof(float), NULL, &status);
    checkError(status, "Failed to create buffer for input A");

    kernel_buff = clCreateBuffer(context, CL_MEM_READ_ONLY, 9 * sizeof(float), NULL, &status); //WARNING using kernel of size 3*3
    // Output buffer.
    output_buf = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
        size * sizeof(float), NULL, &status);
    checkError(status, "Failed to create buffer for output");



    // Transfer inputs to each device. Each of the host buffers supplied to
    // clEnqueueWriteBuffer here is already aligned to ensure that DMA is used
    // for the host-to-device transfer.
    cl_event write_event[2];
    cl_event kernel_event, finish_event;

    status = clEnqueueWriteBuffer(queue, picture_buf, CL_FALSE,
        0, size * sizeof(float), input_picture, 0, NULL, &write_event[0]);
    checkError(status, "Failed to transfer input A");
    status = clEnqueueWriteBuffer(queue, kernel_buff, CL_FALSE,0, 9 * sizeof(float), kernel_mat,0,NULL, &write_event[1]);
    clWaitForEvents(2, write_event);
    // Set kernel arguments.
    unsigned argi = 0;

    status = clSetKernelArg(kernel, argi++, sizeof(cl_mem), &picture_buf);
    checkError(status, "Failed to set argument 1");

     status = clSetKernelArg(kernel, argi++, sizeof(cl_mem), &kernel_buff);
     checkError(status, "Failed to set argument 2");

    status = clSetKernelArg(kernel, argi++, sizeof(cl_mem), &output_buf);
    checkError(status, "Failed to set argument 3");

    const size_t global_work_size = size;
    status = clEnqueueNDRangeKernel(queue, kernel, 1, NULL,
        &global_work_size, NULL, 2, write_event, &kernel_event);
    checkError(status, "Failed to launch kernel");


    clWaitForEvents(1, &kernel_event);
    // Read the result. This the final operation.
    status = clEnqueueReadBuffer(queue, output_buf, CL_TRUE,
        0, size * sizeof(float), output, 1, &kernel_event, &finish_event);
    // Release local events.
    clReleaseEvent(write_event[0]);
    clReleaseEvent(write_event[1]);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseMemObject(picture_buf);
    clReleaseMemObject(kernel_buff);
    clReleaseMemObject(output_buf);
    clReleaseProgram(program);
    clReleaseContext(context);


    //--------------------------------------------------------------------





    clFinish(queue);

    return 0;
}
