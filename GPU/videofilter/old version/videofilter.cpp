
#include "clperation.h"
#include "gpuenv.h"


//#define CPU 1;


//#define SHOW








#ifdef CPU

 // CPU

int main(int, char**)
{
	
	//for the sobel matrix https://docs.opencv.org/3.4/d2/d2c/tutorial_sobel_derivatives.html
	float gaussian[9] = { 1/16, 2/16, 1/16 ,2/16 ,4/16 ,2/16 ,1/16 ,2/16 ,1/16 };

	float sobely[9] = { -3 / 16, -10 / 16, -3 / 16, 0, 0, 0, 3 / 16, 10 / 16, 3/16 };
	float sobelx[9] = { -3 / 16, 0, 3 / 16, -10 / 16, 0, 10 / 16, -3 / 16, 0,3 / 16 };
    VideoCapture camera("./bourne.mp4");
    if(!camera.isOpened())  // check if we succeeded
        return -1;

    const string NAME = "./output.avi";   // Form the new name with container
    int ex = static_cast<int>(CV_FOURCC('M','J','P','G'));
    Size S = Size((int) camera.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
                  (int) camera.get(CV_CAP_PROP_FRAME_HEIGHT));
	//Size S =Size(1280,720);
	cout << "SIZE:" << S << endl;
	
    VideoWriter outputVideo;                                        // Open the output
        outputVideo.open(NAME, ex, 25, S, true);

    if (!outputVideo.isOpened())
    {
        cout  << "Could not open the output video for write: " << NAME << endl;
        return -1;
    }
	time_t start,end;
	double diff,tot;
	tot = 0;//passing secure flags
	int count=0;
	const char *windowName = "filter";   // Name shown in the GUI window.
    #ifdef SHOW
    namedWindow(windowName); // Resizable window, might not work on Windows.
    #endif
	Mat cameraFrame,displayframe;
	Mat grayframe, edge_x, edge_y, edge, edge_inv;
	
    while (true) {
		
		count=count+1;
		if(count > 299) {
			break;
		}
        camera >> cameraFrame;
        Mat filterframe = Mat(cameraFrame.size(), CV_8UC3);
       
    	cvtColor(cameraFrame, grayframe,CV_BGR2GRAY);
		//float *output = (float *) malloc(sizeof(float) * )



		time (&start);
    	GaussianBlur(grayframe, grayframe, Size(3,3),0,0);
    	GaussianBlur(grayframe, grayframe, Size(3,3),0,0);
    	GaussianBlur(grayframe, grayframe, Size(3,3),0,0);
		Scharr(grayframe, edge_x, CV_8U, 0, 1, 1, 0, BORDER_DEFAULT );
		Scharr(grayframe, edge_y, CV_8U, 1, 0, 1, 0, BORDER_DEFAULT );
		//grayframe.data =(uchar*)output;
		addWeighted( edge_x, 0.5, edge_y, 0.5, 0, edge );
        threshold(edge, edge, 80, 255, THRESH_BINARY_INV);
		time (&end);
        cvtColor(edge, edge_inv, CV_GRAY2BGR);
    	// Clear the output image to black, so that the cartoon line drawings will be black (ie: not drawn).
    	memset((char*)displayframe.data, 0, displayframe.step * displayframe.rows);
		grayframe.copyTo(displayframe,edge);
        cvtColor(displayframe, displayframe, CV_GRAY2BGR);
		outputVideo << displayframe;
	#ifdef SHOW
        imshow(windowName, displayframe);
	#endif
		diff = difftime (end,start);
		tot+=diff;
	}
	//appliedSobel(inputtest, unsigned int size, float *output, float *kernel_mat);	
	outputVideo.release();
	camera.release();
  	printf ("FPS %.2lf .\n", 299.0/tot );
	int a;
	cout << " Enter any hey to exit" << std::endl;
	cin >> a;
    return EXIT_SUCCESS;

}




#else
int main() {
	/*********************************************Context GPU INIT *****************************************/
	int a;
	GpuEnvironnment env;
	char char_buffer[STRING_BUFFER_LEN];
	cl_platform_id platform;
	cl_device_id device;


	
	/*for (int i = 0; i < size; i++) {
		std::cout << "index " << i << " value" << inpu_picture[i] << std::endl;
	}*/

	//--------------------------------------------------------------------
	unsigned char** opencl_program = read_file("operation.cl");

	int status;

	clGetPlatformIDs(1, &platform, NULL);
	clGetPlatformInfo(platform, CL_PLATFORM_NAME, STRING_BUFFER_LEN, char_buffer, NULL);
	cl_context_properties context_properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0 }; //make it run on windows



	context_properties[1] = (cl_context_properties)platform;
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	//clGetPlatformInfo(platform, CL_PLATFORM_NAME, STRING_BUFFER_LEN, char_buffer, NULL);
	//printf("%-40s = %s\n", "CL_PLATFORM_NAME", char_buffer);
	//clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, STRING_BUFFER_LEN, char_buffer, NULL);
	//printf("%-40s = %s\n", "CL_PLATFORM_VENDOR ", char_buffer);
	//clGetPlatformInfo(platform, CL_PLATFORM_VERSION, STRING_BUFFER_LEN, char_buffer, NULL);
	//printf("%-40s = %s\n\n", "CL_PLATFORM_VERSION ", char_buffer);
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

	//for the sobel matrix https://docs.opencv.org/3.4/d2/d2c/tutorial_sobel_derivatives.html
	float gaussian[9] = { 1.f / 16.f, 2.f / 16.f, 1.f / 16.f ,2.f / 16.f ,4.f / 16.f ,2.f / 16.f ,1.f / 16.f ,2.f / 16.f ,1.f / 16.f };
	printf("gaussian %f \n", gaussian[4]);
	std::cin >> a;

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
	time_t start, end;
	double diff, tot;
	tot = 0;//passing secure flags
	int count = 0;
	const char* windowName = "filter";   // Name shown in the GUI window.
#ifdef SHOW
	namedWindow(windowName); // Resizable window, might not work on Windows.
#endif
	Mat cameraFrame, displayframe;
	Mat grayframe, edge_x, edge_y, edge, edge_inv;
	std::cout<<"entering loop while"<<std::endl;
	while (true) {

		count = count + 1;
		if (count > 299) {
			std::cout << "size 0" << grayframe.size[1] << std::endl;
			//Mat transion; In case we can convert to float
			//grayframe.convertTo(transion, CV_32FC1);
			/*char* inputtest = (char* ) grayframe.data;
			char* output = (char*)malloc(sizeof(char) * cameraFrame.size[0] * cameraFrame.size[1]);
			try {
				appliedFilter(inputtest,grayframe.size[0], grayframe.size[1] , output, gaussian, opencl_program);

			}
			catch (Exception &e) {
				cerr << e.msg << endl;
			}
			int a;
			std::cin >> a;
			printf("count\n");*/
			break;
		}
		camera >> cameraFrame;
		Mat filterframe = Mat(cameraFrame.size(), CV_8UC3);

		cvtColor(cameraFrame, grayframe, CV_BGR2GRAY);
		time(&start);
		edge_x = Mat(grayframe.size[0], grayframe.size[1], grayframe.type());
		unsigned char* inputtest = (unsigned char*)grayframe.data;
		unsigned char* output = (unsigned char*)malloc(sizeof(char) * grayframe.size[0] * grayframe.size[1]);
		//std::cout<<"applying filter "<< count<<"\n \n \n"<<std::endl;
		try {
			appliedFilter(inputtest, grayframe.size[0], grayframe.size[1], output, gaussian, env);

		}
		
		catch (Exception& e) {
			cerr << e.msg << endl;
		}
		//printf("\n \n\n");
		//std::cout<<"leaving filter "<<std::endl;

		//std::cout << "give number" << std::endl;
		//std::cin >> a;
		
		//Scharr(grayframe, edge_x, CV_8U, 0, 1, 1, 0, BORDER_DEFAULT);
		//Scharr(grayframe, edge_y, CV_8U, 1, 0, 1, 0, BORDER_DEFAULT);
		grayframe.data = output;
		//grayframe.data =(uchar*)output;
		//addWeighted(edge_x, 0.5, edge_y, 0.5, 0, edge);
		//threshold(edge, edge, 80, 255, THRESH_BINARY_INV);
		time(&end);
		//cvtColor(edge, edge_inv, CV_GRAY2BGR);
		// Clear the output image to black, so that the cartoon line drawings will be black (ie: not drawn).
		memset((char*)displayframe.data, 0, displayframe.step * displayframe.rows);
		grayframe.copyTo(displayframe);
		cvtColor(displayframe, displayframe, CV_GRAY2BGR);
		outputVideo << displayframe;
#ifdef SHOW
		imshow(windowName, displayframe);
#endif
		diff = difftime(end, start);
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



