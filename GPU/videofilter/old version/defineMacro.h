#ifndef DEFINE_MACRO
#define DEFINE_MACRO

#include <stdio.h>
#include <stdlib.h>
#include <iostream> // for standard I/O
#include <math.h>
#include <time.h>


// for standard I/O
#include <fstream>
#include "opencv2/opencv.hpp"
//#include "opencv2/core/core.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//#include <opencv2/videoio.hpp>

#include <CL/cl.h>
#include <CL/cl_ext.h>


#define STRING_BUFFER_LEN 1024

using namespace cv;
using namespace std;

#if CV_MAJOR_VERSION > 3

#ifndef CV_FOURCC
#define CV_FOURCC VideoWriter::fourcc
#endif // !CV_FOURCC

#ifndef CV_BGR2GRAY
#define CV_BGR2GRAY COLOR_BGR2GRAY
#endif // !CV_BGR2GRAY

#ifndef CV_GRAY2BGR
#define CV_GRAY2BGR COLOR_GRAY2BGR
#endif // !CV_GRAY2BGR

#ifndef CV_CAP_PROP_FRAME_WIDTH
#define CV_CAP_PROP_FRAME_WIDTH CAP_PROP_FRAME_WIDTH
#endif

#ifndef CV_CAP_PROP_FRAME_HEIGHT
#define CV_CAP_PROP_FRAME_HEIGHT CAP_PROP_FRAME_HEIGHT
#endif // !CV_CAP_PROP_FRAME_HEIGHT

#endif


#endif