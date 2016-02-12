#include <stdio.h>
#include <stdlib.h>
#include <iostream> // for standard I/O
#include <math.h>
#include <time.h>
#include <CL/cl.h>
#include <CL/cl_ext.h>
#include <time.h>
#define STRING_BUFFER_LEN 1024
using namespace std;




void print_clbuild_errors(cl_program program,cl_device_id device)
	{
		cout<<"Program Build failed\n";
		size_t length;
		char buffer[2048];
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &length);
		cout<<"--- Build log ---\n "<<buffer<<endl;
		exit(1);
	}

unsigned char ** read_file(const char *name) {
  size_t size;
  unsigned char **output=(unsigned char **)malloc(sizeof(unsigned char *));
  FILE* fp = fopen(name, "rb");
  if (!fp) {
    printf("no such file:%s",name);
    exit(-1);
  }

  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  *output = (unsigned char *)malloc(size);
  unsigned char **outputstr=(unsigned char **)malloc(sizeof(unsigned char *));
  *outputstr= (unsigned char *)malloc(size);
  if (!*output) {
    fclose(fp);
    printf("mem allocate failure:%s",name);
    exit(-1);
  }

  if(!fread(*output, size, 1, fp)) printf("failed to read file\n");
  fclose(fp);
  printf("file size %d\n",size);
  printf("-------------------------------------------\n");
  snprintf((char *)*outputstr,size,"%s\n",*output);
  printf("%s\n",*outputstr);
  printf("-------------------------------------------\n");
  return outputstr;
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

int main(int argc, char *argv[])
{
     char char_buffer[STRING_BUFFER_LEN];
     cl_platform_id platform;
     cl_device_id device;
     cl_context context;
     cl_context_properties context_properties[] =
     { 
          CL_CONTEXT_PLATFORM, 0,
          CL_PRINTF_CALLBACK_ARM, (cl_context_properties)callback,
          CL_PRINTF_BUFFERSIZE_ARM, 0x1000,
          0
     };
     cl_command_queue queue;
     cl_program program;
     cl_kernel kernel;



//--------------------------------------------------------------------
unsigned int width = std::stoi(argv[1]);
const unsigned N = width * width;
float *input_a;
float *input_b;
float *output;
float *ref_output=(float *) malloc(sizeof(float)*N);
cl_mem input_a_buf; // num_devices elements
cl_mem input_b_buf; // num_devices elements
cl_mem output_buf; // num_devices elements
int status;
int errcode;

	//time_t start,end;
  timespec timer;
  double beg ;
  double finish ;
  double nanodiff;
  float flops;


    
     clGetPlatformIDs(1, &platform, NULL);
     clGetPlatformInfo(platform, CL_PLATFORM_NAME, STRING_BUFFER_LEN, char_buffer, NULL);
     printf("%-40s = %s\n", "CL_PLATFORM_NAME", char_buffer);
     clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, STRING_BUFFER_LEN, char_buffer, NULL);
     printf("%-40s = %s\n", "CL_PLATFORM_VENDOR ", char_buffer);
     clGetPlatformInfo(platform, CL_PLATFORM_VERSION, STRING_BUFFER_LEN, char_buffer, NULL);
     printf("%-40s = %s\n\n", "CL_PLATFORM_VERSION ", char_buffer);

     context_properties[1] = (cl_context_properties)platform;
     clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
     context = clCreateContext(context_properties, 1, &device, NULL, NULL, NULL);
     queue = clCreateCommandQueue(context, device, 0, NULL);

     unsigned char **opencl_program=read_file("vector_add.cl");
     program = clCreateProgramWithSource(context, 1, (const char **)opencl_program, NULL, NULL);
     if (program == NULL)
	{
         printf("Program creation failed\n");
         return 1;
	}	
     int success=clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	 if(success!=CL_SUCCESS) print_clbuild_errors(program,device);
     kernel = clCreateKernel(program, "cross_matrix", NULL);

  printf("\n");
   printf("\n");
   printf("_______________Buffer stats_____________ \n");
    clock_gettime(CLOCK_REALTIME, &timer);
    beg = timer.tv_nsec * pow(10,-9) +timer.tv_sec;
    //shared memory allows shared buffer (sort of)
    input_a_buf = clCreateBuffer(context, CL_MEM_ALLOC_HOST_PTR, N*sizeof(float), NULL, &status);
    checkError(status, "Fail to create buffer for input A");

    input_b_buf = clCreateBuffer(context, CL_MEM_ALLOC_HOST_PTR, N*sizeof(float), NULL, &status);
    checkError(status, "Fail to create buffer for input B");

    output_buf = clCreateBuffer(context, CL_MEM_ALLOC_HOST_PTR, N*sizeof(float), NULL, &status);
    checkError(status, "Fail to create buffer for output");
    clock_gettime(CLOCK_REALTIME, &timer);
    finish = timer.tv_nsec * pow(10,-9) +timer.tv_sec;
    nanodiff = finish - beg;
    printf("GPU buffer creation took %lf seconds to run.\n", nanodiff);

    // for(unsigned j = 0; j < N; ++j) {
	  //     input_a[j] = rand_float();
	  //     input_b[j] = rand_float();
	  //     //printf("ref %f\n",ref_output[j]);
	  //   }

    // Transfer inputs to each device. Each of the host buffers supplied to
    // clEnqueueWriteBuffer here is already aligned to ensure that DMA is used
    // for the host-to-device transfer.
    cl_event write_event[2];
    
    clock_gettime(CLOCK_REALTIME, &timer);
    beg = timer.tv_nsec * pow(10, -9) + timer.tv_sec;

    input_a = (float* ) clEnqueueMapBuffer(queue, input_a_buf, CL_TRUE, CL_MAP_WRITE,0,N* sizeof(float),0,NULL,&write_event[0],&errcode);
    checkError(errcode, "Failed to map input A");

    input_b = (float* ) clEnqueueMapBuffer(queue, input_b_buf, CL_TRUE, CL_MAP_WRITE,0,N* sizeof(float),0,NULL,&write_event[1],&errcode);
    checkError(errcode, "Failed to map input B");

    output = (float* ) clEnqueueMapBuffer(queue, output_buf, CL_TRUE, CL_MAP_READ,0,N* sizeof(float),0,NULL,NULL,&errcode);
    checkError(errcode, "Failed to map output");

    clock_gettime(CLOCK_REALTIME, &timer);
    finish = timer.tv_nsec * pow(10,-9) +timer.tv_sec;
    nanodiff = finish - beg;
    printf("CPU buffer mapping took %lf seconds to run.\n", nanodiff);

  clWaitForEvents(2, write_event);

  clock_gettime(CLOCK_REALTIME, &timer);
    beg = timer.tv_nsec * pow(10,-9) +timer.tv_sec;

  for(unsigned j = 0; j < N; ++j) {
      input_a[j] = rand_float();
      input_b[j] = rand_float();
      //printf("ref %f\n",ref_output[j]);
    }
    clock_gettime(CLOCK_REALTIME, &timer);
    finish = timer.tv_nsec * pow(10,-9) +timer.tv_sec;
    nanodiff = finish - beg;
    printf("CPU AND GPU buffer initialization took %lf seconds to run.\n", nanodiff);
  clock_gettime(CLOCK_REALTIME, &timer);
  beg = timer.tv_nsec * pow(10,-9) +timer.tv_sec;
  float elt;
  for(unsigned i = 0; i < width; i++) {
      for (unsigned j = 0; j < width; j++){
          elt = 0.f;
          for (unsigned k = 0; k < width; k++){
              elt += input_a[i * width + k] * input_b[k * width + j];
          }
          ref_output[j + width * i] = elt;
      }
    //printf("ref %f\n",ref_output[j]);
  }
  clock_gettime(CLOCK_REALTIME, &timer);
  finish = timer.tv_nsec * pow(10,-9) +timer.tv_sec;
  nanodiff = finish - beg;
  //printf("Time get %ld \n",finish);
  printf("\n");
   printf("\n");
   printf("_______________CPU stats_____________ \n");
  printf("CPU took %lf seconds to run with map.\n", nanodiff);
  flops = width * 6 * N/nanodiff;
   printf("CPU has %0.6f FLOPS \n", flops);
   flops *= pow(10,-6);
  printf("CPU has %.6f MFLOPs \n", flops);

	cl_event kernel_event; //, finish_event;
  
    unsigned argi = 0;

    status = clSetKernelArg(kernel, argi++, sizeof(cl_mem), &input_a_buf);
    checkError(status, "Failed to set argument 1");

    status = clSetKernelArg(kernel, argi++, sizeof(cl_mem), &input_b_buf);
    checkError(status, "Failed to set argument 2");

    status = clSetKernelArg(kernel, argi++, sizeof(cl_mem), &output_buf);
    checkError(status, "Failed to set argument 3");

    status = clSetKernelArg(kernel, argi++, sizeof(cl_mem), &width);
    checkError(status, "Failed to set argument 4");
    
    const size_t global_work_size[2] = {width, width};
    
    
    //unmap buffer
    clEnqueueUnmapMemObject(queue,input_a_buf,input_a,0,NULL,NULL);
    clEnqueueUnmapMemObject(queue,input_b_buf,input_b,0,NULL,NULL);

    clock_gettime(CLOCK_REALTIME, &timer);
    beg = timer.tv_nsec * pow(10,-9) +timer.tv_sec;
    status = clEnqueueNDRangeKernel(queue, kernel, 2, NULL,
        global_work_size, NULL, 2, write_event, &kernel_event);
    checkError(status, "Failed to launch kernel");

    
    clWaitForEvents(1, &kernel_event);
    
    // Read the result. This the final operation.
    //status = clEnqueueReadBuffer(queue, output_buf, CL_TRUE,
    //     0, N* sizeof(float), output, 1, &kernel_event, &finish_event);
   clock_gettime(CLOCK_REALTIME, &timer);
   finish = timer.tv_nsec * pow(10,-9) +timer.tv_sec;;
   //diff = difftime (end,start);
   nanodiff = finish - beg ;
   flops = (long) N * 6 * width;
   flops /= (nanodiff);
   printf("\n");
   printf("\n");
   printf("_______________GPU stats_____________ \n");
   printf("GPU took %lf seconds to run. \n", nanodiff);
   printf("GPU has %0.6f FLOPS \n", flops);
   flops *= pow(10,-6);
   printf("GPU has %0.6f MFLOPS \n", flops);
// Verify results.
bool pass = true;

for(unsigned j = 0; j < N && pass; ++j) {
      if(fabsf(output[j] - ref_output[j]) > 1.0e-5f) {
        printf("Failed verification @ index %d\nOutput: %f\nReference: %f\n",
            j, output[j], ref_output[j]);
        pass = false;
      }
}
    // Release local events.
    clReleaseEvent(write_event[0]);
    clReleaseEvent(write_event[1]);
clReleaseKernel(kernel);
clReleaseCommandQueue(queue);
clReleaseMemObject(input_a_buf);
clReleaseMemObject(input_b_buf);
clReleaseMemObject(output_buf);
clReleaseProgram(program);
clReleaseContext(context);


//--------------------------------------------------------------------






     clFinish(queue);

     return 0;
}
