#include <stdio.h>
#include "clu.h"

int main(int argc, char ** argv ){
	cl_int ret = CL_SUCCESS;
	// Get a platform
	cl_platform_id platform;
	ret = clGetPlatformIDs(1, &platform, NULL);
	if(ret != CL_SUCCESS){
		fprintf(stderr, "OpenCL Error: %s\n", cluGetErrorString(ret));
		exit(1);
	}
	// Get a device
	cl_device_id device;
	ret = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 1, &device, NULL);
	if(ret != CL_SUCCESS){
		fprintf(stderr, "OpenCL Error: %s\n", cluGetErrorString(ret));
		exit(1);
	}
	// Create a context
	cl_context_properties properties[3] = {
		CL_CONTEXT_PLATFORM,
		platform,
		0
	};
	cl_context context = clCreateContext(properties, 1, &device, cluContextNotifyStdout, NULL, &ret);
	if(ret != CL_SUCCESS){
		fprintf(stderr, "OpenCL Error: %s\n", cluGetErrorString(ret));
		exit(1);
	}
	// The first argument is the filename of our program
	cl_program program = cluCreateProgramFromSource(context, argv[1], NULL, &ret);
	if(ret != CL_SUCCESS){
		fprintf(stderr, "OpenCL Error: %s\n", cluGetErrorString(ret));
		exit(1);
	}
	// Build the program
	ret = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
	if(ret != CL_SUCCESS){
		fprintf(stderr, "OpenCL Error: %s\n", cluGetErrorString(ret));
		size_t logSize;
		ret = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);
		char * buildLog = (char *)malloc(logSize);
		ret = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, buildLog, NULL);
		fprintf(stderr, "Build Log:\n%s\n", buildLog);
		exit(1);
	}
	size_t logSize;
	ret = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);
	char * buildLog = (char *)malloc(logSize);
	ret = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, buildLog, NULL);
	if(ret != CL_SUCCESS){
		fprintf(stderr, "OpenCL Error: %s\n", cluGetErrorString(ret));
		exit(1);
	}
	printf("Build log:\n%s\n", buildLog);
	free(buildLog);
	clReleaseProgram(program);
	clReleaseContext(context);
	exit(0);
}
