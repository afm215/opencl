
#include "clperation.h"
#include "gpuenv.h"


//#define CPU 1;


//#define SHOW








#ifdef CPU

 // running on CPU

int main(int, char**)
{
	std::cout << "running on CPU" << std::endl;
	std::cout << "By commenting the corresponding line, undefine CPU MACRO in source code of videofilter.cpp to run on GPU \n \n" <<std::endl;
	VideoCapture camera("./bourne.mp4");
	if (!camera.isOpened())  // check if we succeeded
		return -1;

	const string NAME = "./output.avi";   // Form the new name with container
	int ex = static_cast<int>(CV_FOURCC('M', 'J', 'P', 'G'));
	Size S = Size((int)camera.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
		(int)camera.get(CV_CAP_PROP_FRAME_HEIGHT));
	cout << "SIZE:" << S << endl;

	VideoWriter outputVideo;                                        // Open the output
	outputVideo.open(NAME, ex, 25, S, true);

	if (!outputVideo.isOpened())
	{
		cout << "Could not open the output video for write: " << NAME << endl;
		return -1;
	}
	double start, end;
	timespec timer;
	double diff, tot;
	tot = 0;//passing secure flags
	int count = 0;
	const char* windowName = "filter";   // Name shown in the GUI window.
#ifdef SHOW
	namedWindow(windowName); // Resizable window, might not work on Windows.
#endif
	Mat cameraFrame, displayframe;
	Mat grayframe, edge_x, edge_y, edge, edge_inv;

	while (true) {

		count = count + 1;
		if (count > 299) {
			break;
		}
		camera >> cameraFrame;
		Mat filterframe = Mat(cameraFrame.size(), CV_8UC3);

		cvtColor(cameraFrame, grayframe, CV_BGR2GRAY);

		clock_gettime(CLOCK_REALTIME, &timer);
		start = timer.tv_nsec * pow(10, -9) + timer.tv_sec;

		GaussianBlur(grayframe, grayframe, Size(3, 3), 0, 0);
		GaussianBlur(grayframe, grayframe, Size(3, 3), 0, 0);
		GaussianBlur(grayframe, grayframe, Size(3, 3), 0, 0);
		Scharr(grayframe, edge_x, CV_8U, 0, 1, 1, 0, BORDER_DEFAULT);
		Scharr(grayframe, edge_y, CV_8U, 1, 0, 1, 0, BORDER_DEFAULT);
		//grayframe.data =(uchar*)output;
		addWeighted(edge_x, 0.5, edge_y, 0.5, 0, edge);
		threshold(edge, edge, 80, 255, THRESH_BINARY_INV);
		
		clock_gettime(CLOCK_REALTIME, &timer);
		end = timer.tv_nsec * pow(10, -9) + timer.tv_sec;

		cvtColor(edge, edge_inv, CV_GRAY2BGR);
		// Clear the output image to black, so that the cartoon line drawings will be black (ie: not drawn).
		memset((char*)displayframe.data, 0, displayframe.step * displayframe.rows);
		grayframe.copyTo(displayframe, edge);
		cvtColor(displayframe, displayframe, CV_GRAY2BGR);
		outputVideo << displayframe;
#ifdef SHOW
		imshow(windowName, displayframe);
#endif
		diff = end - start;
		tot += diff;
	}
	outputVideo.release();
	camera.release();
	printf("FPS %.2lf .\n", 299.0 / tot);
	int a;
	cout << " Enter any key to exit" << std::endl;
	cin >> a;
	return EXIT_SUCCESS;

}




#else
//Running on GPU

int main() {
	/*********************************************Context GPU INIT *****************************************/
	std::cout << "running on GPU" << std::endl;
	std::cout << "define CPU MACRO in the source code of videofilter.cpp to run on CPU" << std::endl;
	int a;
	GpuEnvironnment env;
	char char_buffer[STRING_BUFFER_LEN];
	cl_platform_id platform;
	cl_device_id device;

	unsigned char** opencl_program = read_file("operation.cl");

	int status;

	clGetPlatformIDs(1, &platform, NULL);
	clGetPlatformInfo(platform, CL_PLATFORM_NAME, STRING_BUFFER_LEN, char_buffer, NULL);
	cl_context_properties context_properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0 }; //makes it run on windows

	context_properties[1] = (cl_context_properties)platform;
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	env.context = clCreateContext(context_properties, 1, &device, NULL, NULL, NULL);
	env.queue = clCreateCommandQueue(env.context, device, 0, NULL);

	env.program = clCreateProgramWithSource(env.context, 1, (const char**)opencl_program, NULL, NULL);
	if (env.program == NULL)
	{
		printf("Program creation failed\n");
		return 1;
	}
	int success = clBuildProgram(env.program, 0, NULL, NULL, NULL, NULL);
	if (success != CL_SUCCESS) print_clbuild_errors(env.program, device);
	env.kernel = clCreateKernel(env.program, "Filter", NULL);
	/*************************************END OF GPU INIT ****************************************************/

	std::cout << "\n \ngaussian matrix could be not the same as the one use by the cpu" << std::endl;
	std::cout << "visual result could thus differ \n \n" << std::endl;
	std::cout << "A mali-renderer error can randomly occurs on the concerned devices, please rerun the app in this case \nIf the error keeps occuring please check the ouput folder to get the expecting video\n"<<std::endl;
	std::cout << "In case of error here are the performances got with an intel i5 9300H and a RTX 2060: \n CPU:27.63  \n GPU: 62.97 FPS"<<std::endl;

	float gaussian[9] = { 1.f / 16.f, 2.f / 16.f, 1.f / 16.f ,2.f / 16.f ,4.f / 16.f ,2.f / 16.f ,1.f / 16.f ,2.f / 16.f ,1.f / 16.f };

	//for the sobel matrix https://docs.opencv.org/3.4/d2/d2c/tutorial_sobel_derivatives.html
	float sobely[9] = { -3.f / 16.f, -10.f / 16.f, -3.f / 16.f, 0.f, 0.f, 0.f, 3.f / 16.f, 10.f / 16.f, 3.f / 16.f };
	float sobelx[9] = { -3.f / 16.f, 0.f, 3.f / 16.f, -10.f / 16.f, 0.f, 10.f / 16.f, -3.f / 16.f, 0,3.f / 16.f };

	VideoCapture camera("./bourne.mp4");
	if (!camera.isOpened())  // check if we succeeded
		return -1;

	const string NAME = "./output.avi";   // Form the new name with container
	int ex = static_cast<int>(CV_FOURCC('M', 'J', 'P', 'G'));
	Size S = Size((int)camera.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
		(int)camera.get(CV_CAP_PROP_FRAME_HEIGHT));
	//Size S =Size(1280,720);
	cout << "SIZE:" << S << endl;

	VideoWriter outputVideo;                                        // Open the output
	outputVideo.open(NAME, ex, 25, S, true);

	if (!outputVideo.isOpened())
	{
		cout << "Could not open the output video for write: " << NAME << endl;
		return -1;
	}
	timespec timer;
	double start, end;
	double diff, tot;
	tot = 0;//passing secure flags
	int count = 0;
	const char* windowName = "filter";   // Name shown in the GUI window.
#ifdef SHOW
	namedWindow(windowName); // Resizable window, might not work on Windows.
#endif
	Mat cameraFrame, displayframe;
	Mat grayframe, edge_x, edge_y, edge, edge_inv;
	std::cout << "entering loop while" << std::endl;
	while (true) {

		count = count + 1;
		if (count > 299) {
			break;
		}
		camera >> cameraFrame;
		Mat filterframe = Mat(cameraFrame.size(), CV_8UC3);

		cvtColor(cameraFrame, grayframe, CV_BGR2GRAY);

		clock_gettime(CLOCK_REALTIME, &timer);
		start = timer.tv_nsec * pow(10, -9) + timer.tv_sec;

		edge_x = Mat(grayframe.size[0], grayframe.size[1], grayframe.type());
		edge_y = Mat(grayframe.size[0], grayframe.size[1], grayframe.type());
		unsigned char* inputtest = (unsigned char*)grayframe.data;
		unsigned char* outputgauss = (unsigned char*)malloc(sizeof(char) * grayframe.size[0] * grayframe.size[1]);
		unsigned char* output_edgex = (unsigned char*)malloc(sizeof(char) * grayframe.size[0] * grayframe.size[1]);
		unsigned char* output_edgey = (unsigned char*)malloc(sizeof(char) * grayframe.size[0] * grayframe.size[1]);
		try {
			appliedFilter(inputtest, grayframe.size[0], grayframe.size[1], outputgauss, gaussian, env);
			appliedFilter(outputgauss, grayframe.size[0], grayframe.size[1], outputgauss, gaussian, env);
			appliedFilter(outputgauss, grayframe.size[0], grayframe.size[1], outputgauss, gaussian, env);
			appliedFilter(inputtest, grayframe.size[0], grayframe.size[1], output_edgex, sobelx, env);
			appliedFilter(inputtest, grayframe.size[0], grayframe.size[1], output_edgey, sobely, env);

		}

		catch (Exception& e) {
			//cerr << e.msg << endl;
		}
		grayframe.data = outputgauss; 
		edge_x.data = output_edgex;
		edge_y.data = output_edgey;

		addWeighted(edge_x, 0.5, edge_y, 0.5, 0, edge);
		threshold(edge, edge, 44, 255, THRESH_BINARY_INV);
		clock_gettime(CLOCK_REALTIME, &timer);
		end = timer.tv_nsec * pow(10, -9) + timer.tv_sec;
		cvtColor(edge, edge_inv, CV_GRAY2BGR);
		// Clear the output image to black, so that the cartoon line drawings will be black (ie: not drawn).
		memset((char*)displayframe.data, 0, displayframe.step * displayframe.rows);
		grayframe.copyTo(displayframe, edge);
		cvtColor(displayframe, displayframe, CV_GRAY2BGR);
		outputVideo << displayframe;
#ifdef SHOW
		imshow(windowName, displayframe);
#endif
		diff = end - start;
		tot += diff;
	}
	//appliedSobel(inputtest, unsigned int size, float *output, float *kernel_mat);	
	outputVideo.release();
	camera.release();
	printf("FPS %.2lf .\n", 299.0 / tot);

	status = clFinish(env.queue);
	checkError(status, "Queue not finished");

	status = clReleaseKernel(env.kernel);
	checkError(status, "Failed to release kernel");

	status = clReleaseProgram(env.program);
	checkError(status, "Failed to release program");
	status = clReleaseCommandQueue(env.queue);
	checkError(status, "Failed to release queue");

	status = clReleaseContext(env.context);
	checkError(status, "Failed to release context");

	//--------------------------------------------------------------------



	std::cout << "enter any letter to finish" << std::endl;
	std::cin >> a;

	return EXIT_SUCCESS;


}



#endif



