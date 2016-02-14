
#ifndef DEFINE_MACRO
#define DEFINE_MACRO

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <time.h>

#include <fstream>
#include "opencv2/opencv.hpp"

#include <CL/cl.h>
#include <CL/cl_ext.h>

#ifdef _WIN32
#define CLOCK_REALTIME 0
#include <Windows.h>
#endif // _WIN32

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
