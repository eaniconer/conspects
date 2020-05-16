
#include <OpenCL/cl.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

const size_t INPUT_SIGNAL_WIDTH = 8;
const size_t INPUT_SIGNAL_HEIGHT = 8;

cl_uint inputSignal[INPUT_SIGNAL_HEIGHT][INPUT_SIGNAL_WIDTH] =
{
    {3, 1, 1, 4, 8, 2, 1, 3},
    {4, 2, 1, 1, 2, 1, 2, 3},
    {4, 4, 4, 4, 3, 2, 2, 2},
    {9, 8, 3, 8, 9, 0, 0, 0},
    {9, 3, 3, 9, 0, 0, 0, 0},
    {0, 9, 0, 8, 0, 0, 0, 0},
    {3, 0, 8, 8, 9, 4, 4, 4},
    {5, 9, 8, 1, 8, 1, 1, 1}
};

const size_t OUTPUT_SIGNAL_WIDTH = 6;
const size_t OUTPUT_SIGNAL_HEIGHT = 6;
cl_uint outputSignal[OUTPUT_SIGNAL_HEIGHT][OUTPUT_SIGNAL_WIDTH];

const size_t MASK_WIDTH = 3;
const size_t MASK_HEIGHT = 3;

cl_uint mask[MASK_HEIGHT][MASK_WIDTH] = 
{
    {1, 1, 1}, {1, 0, 1}, {1, 1, 1},
};

inline void checkError(cl_int error, const char* msg) 
{
    if (error != CL_SUCCESS) {
        cerr << "ERROR: " << error << " MSG: " << msg << endl;
        exit(EXIT_FAILURE);
    }
}

void CL_CALLBACK contextCallback(
    const char* errInfo,
    const void* privateInfo,
    size_t cb, 
    void* userData)
{
    cout << "Error occured during context use: " << errInfo << endl;
    exit(EXIT_FAILURE);
}

int main() 
{
    cl_int error = CL_SUCCESS;
    cl_uint nPlatforms = 0;

    cl_platform_id* platformIDs = NULL;

    error = clGetPlatformIDs(0, NULL, &nPlatforms); checkError(error, "clGetPlatformIDs 1");
    if (nPlatforms == 0) {
        cerr << "Platform not found." << endl;
        return 1;
    }

    platformIDs = (cl_platform_id*) alloca(sizeof(cl_platform_id) * nPlatforms);
    error = clGetPlatformIDs(nPlatforms, platformIDs, NULL); checkError(error, "clGetPlatformIDs 2");
    cl_platform_id platform = platformIDs[0];

    cl_device_id* deviceIDs = NULL;
    cl_uint nDevices = 0;
    error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &nDevices); checkError(error, "number of devices");
    if (nDevices == 0) {
        cerr << "Devices not found." << endl;
        return 1;
    }

    deviceIDs = (cl_device_id*) alloca(sizeof(cl_device_id) * nDevices);
    error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, nDevices, deviceIDs, NULL); checkError(error, "get devices");

    cl_context_properties contextProperties[] = {
        CL_CONTEXT_PLATFORM, (cl_context_properties) platform, 0
    };

    cl_context context = NULL;
    context = clCreateContext(contextProperties, nDevices, deviceIDs, &contextCallback, NULL, &error); checkError(error, "create context");

    ifstream sourceFile("convolution.cl");
    if (!sourceFile) {
        cerr << "Failed to open source file." << endl;
        return 1;
    }

    string sourceText = string(istreambuf_iterator<char>(sourceFile), (istreambuf_iterator<char>()));
    size_t length = sourceText.size();
    const char* rawSourceText = sourceText.c_str();

    cl_program program = clCreateProgramWithSource(context, 1, &rawSourceText, &length, &error); checkError(error, "create program");
    error = clBuildProgram(program, nDevices, deviceIDs, NULL, NULL, NULL); checkError(error, "build program");

    cl_kernel kernel = clCreateKernel(program, "convolve", &error); checkError(error, "create kernel");

    cl_mem inputSignalBuffer = clCreateBuffer(
        context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
        sizeof(cl_uint) * INPUT_SIGNAL_WIDTH * INPUT_SIGNAL_HEIGHT, 
        static_cast<void*>(inputSignal), &error); checkError(error, "create buffer for input signal");

    cl_mem maskBuffer = clCreateBuffer(
        context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(cl_uint) * MASK_HEIGHT * MASK_WIDTH,
        static_cast<void*>(mask), &error); checkError(error, "create buffer for mask");

    cl_mem outputSignalBuffer = clCreateBuffer(
        context, CL_MEM_WRITE_ONLY,
        sizeof(cl_uint) * OUTPUT_SIGNAL_HEIGHT * OUTPUT_SIGNAL_WIDTH, 
        NULL, &error); checkError(error, "create buffer for output signal");

    cl_command_queue queue = clCreateCommandQueue(context, deviceIDs[0], 0, &error); checkError(error, "create command queue");

    error = clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputSignalBuffer);
    error |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &maskBuffer);
    error |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &outputSignalBuffer);
    error |= clSetKernelArg(kernel, 3, sizeof(cl_uint), &INPUT_SIGNAL_WIDTH);
    error |= clSetKernelArg(kernel, 4, sizeof(cl_uint), &MASK_WIDTH);
    checkError(error, "set args");

    const size_t globalWorkSize[1] = { OUTPUT_SIGNAL_WIDTH * OUTPUT_SIGNAL_HEIGHT };
    const size_t localWorkSize[1] = { 1 };

    error = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
    checkError(error, "Enqueue kernel");

    error = clEnqueueReadBuffer(queue, outputSignalBuffer, CL_TRUE, 0,
        sizeof(cl_uint) * OUTPUT_SIGNAL_HEIGHT * OUTPUT_SIGNAL_WIDTH, outputSignal, 0, NULL, NULL);
    checkError(error, "Read output buffer");

    for (int y = 0; y < OUTPUT_SIGNAL_HEIGHT; ++y) {
        for (int x = 0; x < OUTPUT_SIGNAL_WIDTH; ++x) {
            cout << outputSignal[y][x] << " ";
        }
        cout << endl;
    }

    return 0;
}










