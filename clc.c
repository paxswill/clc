#include <stdio.h>
#include <string.h>
#include "clu.h"

int main(int argc, char ** argv ){
	// Check number of arguments
	if(argc < 2){
		printf("Usage: clc program.cl [compiler options]\n");
		exit(1);
	}
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
	// String any options together
	char *options = NULL;
	if(argc > 2){
		int totalSize = 0;
		for(int i = 0; i < (argc - 2); i++){
			totalSize += strlen(argv[2 + i]) + 1;
		}
		totalSize++;
		options = malloc(totalSize);
		char *current = options;
		for(int i = 0; i < (argc - 2); i++){
			int size = strlen(argv[2 + i]);
			strcpy(current, argv[2 + i]);
			current[size] = ' ';
			current += size + 1;	
		}
		options[totalSize] = '\0';
	}
	// Enable verbose mode on Nvidia platforms that support it
	char *extensions;
	size_t extensionsSize;
	ret = clGetPlatformInfo(platform, CL_PLATFORM_EXTENSIONS, 0, NULL, &extensionsSize);
	extensions = malloc(extensionsSize + 1);
	ret = clGetPlatformInfo(platform, CL_PLATFORM_EXTENSIONS, extensionsSize, extensions, NULL);
	extensions[extensionsSize] = '\0';
	if(strstr(extensions, "cl_nv_compiler_options") != NULL){
		int optionsSize = 0;
		if(options != NULL){
			optionsSize = strlen(options);
		}
		char *newOptions = malloc(optionsSize + 14 + 2);
		if(options != NULL){
			strcpy(newOptions, options);
		}
		strcpy(newOptions + optionsSize + 1, "-cl-nv-verbose");
		newOptions[optionsSize] = ' ';
		options = newOptions;
	}
	// Actually compile
	printf("Compile options:\n%s\n", options);
	ret = clBuildProgram(program, 1, &device, options, NULL, NULL);
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
