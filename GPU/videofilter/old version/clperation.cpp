
#include "clperation.h"
using namespace cv;
using namespace std;


/*
*I will not use MAP here to preserve compatibility although MAP woulf be more efficient with the device used.
*/




void callback(const char *buffer, size_t length, size_t final, void *user_data)
{
     fwrite(buffer, 1, length, stdout);
}


void checkError(int status, const char* msg) {
    if (status != CL_SUCCESS)
        printf("%s: %s\n", msg, getErrorString(status));
}

// Randomly generate a floating-point number between -10 and 10.
float rand_float() {
  return float(rand()) / float(RAND_MAX) * 20.0f - 10.0f;
}

void hello(){
    printf("Hello!! \n \n");
}

int appliedFilter(unsigned char *input_picture, unsigned int line, unsigned int colonne, unsigned char *output, float *kernel_mat, GpuEnvironnment &env)
{
    int a;
    int  size = line * colonne;
    //std::cout << "one entering in the filter function size of the ouput :"<< size <<std::endl;
   
    cl_mem picture_buf;
    cl_mem kernel_buff;
    cl_mem output_buf;



    int status;
   
   
    // Input buffers.

/*

Thread 1 "videofilter" received signal SIGILL, Illegal instruction.
0x00011d78 in clCreateBuffer@plt ()

*/
    picture_buf = clCreateBuffer(env.context, CL_MEM_READ_ONLY,
        size * sizeof(char), NULL, &status);
    checkError(status, "Failed to create buffer for input A");

    kernel_buff = clCreateBuffer(env.context, CL_MEM_READ_ONLY, 9 * sizeof(float), NULL, &status); //WARNING using kernel of size 3*3
    checkError(status, "Failed to create buffer for kernelmat");
    // Output buffer.
    output_buf = clCreateBuffer(env.context, CL_MEM_WRITE_ONLY,
       size * sizeof(char), NULL, &status);
    checkError(status, "Failed to create buffer for output");



    // Transfer inputs to each device. Each of the host buffers supplied to
    // clEnqueueWriteBuffer here is already aligned to ensure that DMA is used
    // for the host-to-device transfer.
    cl_event write_event[2];
    cl_event kernel_event, finish_event;
    /*for (int i = 0; i < size; i++) {
        std::cout << (int) input_picture[i];
    }*/
    status = clEnqueueWriteBuffer(env.queue, picture_buf, CL_FALSE,
        0, size * sizeof(char), input_picture, 0, NULL, &write_event[0]);
    checkError(status, "Failed to transfer input picture");
    status = clEnqueueWriteBuffer(env.queue, kernel_buff, CL_FALSE,0, 9 * sizeof(float), kernel_mat,0,NULL, &write_event[1]);
    checkError(status, "Failed to transfer kernel_mat");

    clWaitForEvents(2, write_event);
    // Set kernel arguments.
    unsigned argi = 0;
    status = clSetKernelArg(env.kernel, argi++, sizeof(cl_mem), &picture_buf);
    checkError(status, "Failed to set argument 1");

     status = clSetKernelArg(env.kernel, argi++, sizeof(cl_mem), &kernel_buff);
     checkError(status, "Failed to set argument 2");

    status = clSetKernelArg(env.kernel, argi++, sizeof(cl_mem), &output_buf);
    checkError(status, "Failed to set argument 3");

    status = clSetKernelArg(env.kernel, argi++, sizeof(int), &line);
    checkError(status, "Failed to set argument 4");

    status = clSetKernelArg(env.kernel, argi++, sizeof(int), &colonne);
    checkError(status, "Failed to set argument 5");


    const size_t global_work_size[2] = {line, colonne};
    //std::cout<<"launching threads ..."<<std::endl;
    status = clEnqueueNDRangeKernel(env.queue, env.kernel, 2, NULL,
        global_work_size, NULL, 2, write_event, &kernel_event);
    checkError(status, "Failed to launch kernel");
    

    clWaitForEvents(1, &kernel_event);
    //std::cout<<"threads finished"<<std::endl;
    // Read the result. This the final operation.
    
    
    //std::cout<<"reading ouput"<<std::endl;
        status = clEnqueueReadBuffer(env.queue, output_buf, CL_TRUE,
            0, size * sizeof(char), output, 1, &kernel_event, &finish_event);
        checkError(status, "Failed to read output buffer");

    /*for (int i = 0; i < size; i++) {
        printf("adrres %c \n", output[i]);
    }*/
    
    

    clWaitForEvents(1, &finish_event);
    
    //status = clFinish(env.queue);
    //checkError(status, "Queue not finished");
    //std::cout<<"output read"<<std::endl;
    
    // Release local events.
    //std::cout <<"Release"<<std::endl;
    clReleaseEvent(write_event[0]);
    clReleaseEvent(write_event[1]);
    
    status = clReleaseEvent(kernel_event);
    checkError(status, "Failed to clean kernel");

    clReleaseEvent(finish_event);
    status = clReleaseMemObject(picture_buf);
    checkError(status, "Failed to clean picture buff");

    clReleaseMemObject(kernel_buff);
    clReleaseMemObject(output_buf);

    
    //std::cout << "releasing" << std::endl;
    
    //std::cout <<"finished"<<std::endl;
    return 0;
}

