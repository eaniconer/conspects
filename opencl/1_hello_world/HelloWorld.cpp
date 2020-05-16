
#include <OpenCL/cl.h>

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

cl_context CreateContext();
cl_command_queue CreateCommandQueue(cl_context, cl_device_id*);
cl_program CreateProgram(cl_context, cl_device_id, const char*);
bool CreateMemObjects(cl_context, cl_mem[3], float*, float*);
void Cleanup(cl_context, cl_command_queue, cl_program, cl_kernel, cl_mem[3]);

const int ARRAY_SIZE = 1000000;

int main() 
{
    cl_context context = 0;
    cl_command_queue commandQueue = 0;
    cl_program program = 0;
    cl_device_id device = 0;
    cl_kernel kernel = 0;
    cl_mem memObjects[3] = {0, 0, 0};
    cl_int errNum;

    context = CreateContext();
    if (context == NULL) {
        cerr << "Failed to create OpenCL context" << endl;
        return 1;
    }

    commandQueue = CreateCommandQueue(context, &device);
    if (commandQueue == NULL) {
        Cleanup(context, commandQueue, program, kernel, memObjects);
        return 1;
    }

    // Create OpenCL program from HelloWorld.cl kernel source
    program = CreateProgram(context, device, "HelloWorld.cl");
    if (program == NULL) {
        cerr << "Failed to create kernel" << endl;
        Cleanup(context, commandQueue, program, kernel, memObjects);
        return 1;
    }

    // Create OpenCL kernel
    kernel = clCreateKernel(program, "hello_kernel", NULL);
    if (kernel == NULL) {
        cerr << "Failed to create kernel. " << endl;
        Cleanup(context, commandQueue, program, kernel, memObjects);
        return 1;
    }

    // Create memory objects: they will be used  as arguments to kernel.
    float* result = (float*) malloc(sizeof(float) * ARRAY_SIZE);
    float* a = (float*) malloc(sizeof(float) * ARRAY_SIZE);
    float* b = (float*) malloc(sizeof(float) * ARRAY_SIZE);

    for (int i = 0; i < ARRAY_SIZE; ++i) {
        a[i] = i;
        b[i] = 2 * i;
    }

    if (!CreateMemObjects(context, memObjects, a, b)) {
        Cleanup(context, commandQueue, program, kernel, memObjects);
        return 1;
    }

    // Set the kernel argumetns 
    errNum = clSetKernelArg(kernel, 0, sizeof(cl_mem), &memObjects[0]);
    errNum |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &memObjects[1]);
    errNum |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &memObjects[2]);
    if (errNum != CL_SUCCESS) {
        cerr << "Error setting kernel arguments. " << endl;
        Cleanup(context, commandQueue, program, kernel, memObjects);
        return 1;
    }

    // determines how the kernel is distributed across processing units on the device
    size_t globalWorkSize[1] = { ARRAY_SIZE }; 
    size_t localWorkSize[1] = { 1 };

    errNum = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
    if (errNum != CL_SUCCESS) {
        cerr << "Error queuing kernel for execution." << endl;
        Cleanup(context, commandQueue, program, kernel, memObjects);
        return 1;
    }

    // 3rd argument is blocking_read
    errNum = clEnqueueReadBuffer(commandQueue, memObjects[2], CL_TRUE, 0, ARRAY_SIZE * sizeof(float), result, 0, NULL, NULL);
    if (errNum != CL_SUCCESS) {
        cerr << "Error reading result buffer." << endl;
        Cleanup(context, commandQueue, program, kernel, memObjects);
        return 1;
    }

    // for (int i = 0; i < ARRAY_SIZE; ++i) {
    //     cout << result[i] << " ";
    // }
    cout << endl;
    cout << "Executed program successfully." << endl;

    Cleanup(context, commandQueue, program, kernel, memObjects);

    return 0;
}

string getPlatformProperty(cl_platform_id id, cl_platform_info info)
{
    cl_int errNum;
    size_t size;
    errNum = clGetPlatformInfo(id, info, 0, NULL, &size);
    char* data[size]; // = (char*) alloca(sizeof(char) * size);
    errNum = clGetPlatformInfo(id, info, size, data, NULL);
    return std::string((const char*)data);
}

void printPlatformsInfo(size_t n, cl_platform_id* ids)
{
    for (int i = 0; i < n; ++i) {
        cl_platform_id id = ids[i];  
        cout << string(40, '-') << endl; 
        cout << "Platform name: " << getPlatformProperty(id, CL_PLATFORM_NAME) << endl;
        cout << "Platform version: " << getPlatformProperty(id, CL_PLATFORM_VERSION) << endl;
        cout << "Platform vendor: " << getPlatformProperty(id, CL_PLATFORM_VENDOR) << endl;
        cout << "Platform extensions: " << getPlatformProperty(id, CL_PLATFORM_EXTENSIONS) << endl;
        cout << "Platform profile: " << getPlatformProperty(id, CL_PLATFORM_PROFILE) << endl;
        cout << string(40, '-') << endl;
    }
}

cl_context CreateContext() 
{
    cl_int errNum;
    cl_uint numPlatforms;
    cl_platform_id firstPlatformId;
    cl_context context = NULL;

    // Get number of platforms
    errNum = clGetPlatformIDs(0, NULL, &numPlatforms);
    cout << numPlatforms << " platform(s) available\n";
    // Get all platform ids
    cl_platform_id* platformIds = (cl_platform_id*) alloca(sizeof(cl_platform_id) * numPlatforms);
    errNum = clGetPlatformIDs(numPlatforms, platformIds, NULL);
    printPlatformsInfo(numPlatforms, platformIds);

    // select the first available platform
    errNum = clGetPlatformIDs(1, &firstPlatformId, &numPlatforms);

    if (errNum != CL_SUCCESS || numPlatforms <= 0) {
        cerr << "Failed to find any OpenCL platforms" << endl;
        return NULL;
    }

    // Create OpenCL context.
    // Attempt to create GPU-based context and if that fails, try
    // to create CPU-based context

    cl_context_properties properties[] = 
    {
        CL_CONTEXT_PLATFORM, 
        (cl_context_properties)firstPlatformId,
        0
    };

    context = clCreateContextFromType(properties, CL_DEVICE_TYPE_GPU, NULL, NULL, &errNum);
    if (errNum == CL_SUCCESS) {
        return context;
    }
    cout << "Could not create GPU context, trying CPU..." << endl;
    
    context = clCreateContextFromType(properties, CL_DEVICE_TYPE_CPU, NULL, NULL, &errNum);
    if (errNum == CL_SUCCESS) {
        return context;
    }
    cout << "Could not create CPU context" << endl;
        
    return NULL; 
}

// device is the underlying compute hardware (single CPU or GPU)
// in order to communicate with device, we create command-queue
cl_command_queue CreateCommandQueue(cl_context context, cl_device_id* device) 
{
    cl_int errNum;
    cl_device_id* devices;
    cl_command_queue commandQueue = NULL;
    size_t deviceBufferSize = -1;

    // context defines OpenCL devices within itself
    // here we query the size of the buffer required to store all of the device ids
    errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);
    cout << "devices in context: " << deviceBufferSize / sizeof(cl_device_id) << endl;
    if (errNum != CL_SUCCESS) {
        cerr << "Failed to call clGetContextInfo" << endl;
        return NULL;
    }

    if (deviceBufferSize <= 0) {
        cerr << "No devices available..." << endl;
        return NULL;
    }

    devices = new cl_device_id[deviceBufferSize / sizeof(cl_device_id)];
    errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices, NULL);  // get all device ids
    if (errNum != CL_SUCCESS) {
        cerr << "Failed to get device IDs" << endl;
        return NULL;
    }

    // create command-queue for device-0
    commandQueue = clCreateCommandQueue(context, devices[0], 0, NULL);
    if (commandQueue == NULL) {
        cerr << "Failed to create commandQueue for device[0]" << endl;
        return NULL;
    }

    *device = devices[0];
    delete [] devices;

    return commandQueue;
}

cl_program CreateProgram(cl_context context, cl_device_id device, const char* filename) 
{
    cl_int errNum;
    cl_program program;

    ifstream kernelFile(filename, ios::in);
    if (!kernelFile) {
        cerr << "Failed to open file for reading: " << filename << endl;
        return NULL;
    }

    ostringstream oss;
    oss << kernelFile.rdbuf();

    const char* source = oss.str().c_str();

    // program object stores the compiled executable code for all of the devices is created on a context
    program = clCreateProgramWithSource(context, 1, (const char**) &source, NULL, NULL);
    if (program == NULL) {
        cerr << "Failed to create CL program from source. " << endl;
        return NULL;
    }

    errNum = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (errNum != CL_SUCCESS) {
        // Find out the reason of the error
        char buildLog[16384];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buildLog), buildLog, NULL);
        cerr << "Error in kernel: " << endl;
        cerr << buildLog;
        clReleaseProgram(program);
        return NULL;
    }

    return program;
}

bool CreateMemObjects(cl_context context, cl_mem memObjects[3], float* a, float* b)
{
    memObjects[0] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * ARRAY_SIZE, a, NULL);
    memObjects[1] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * ARRAY_SIZE, b, NULL);
    memObjects[2] = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * ARRAY_SIZE, NULL, NULL);

    for (int i = 0; i < 3; ++i) {
        if (memObjects[i] == NULL) {
            cerr << "Error creating memory objects." << endl;
            return false;
        }
    }
    return true;
}

void Cleanup(cl_context context, cl_command_queue commandQueue,
             cl_program program, cl_kernel kernel, cl_mem memObjects[3])
{
    for (int i = 0; i < 3; i++)
    {
        if (memObjects[i] != 0) {
            clReleaseMemObject(memObjects[i]);
        }
    }

    if (commandQueue != 0) {
        clReleaseCommandQueue(commandQueue);
    }

    if (kernel != 0) {
        clReleaseKernel(kernel);
    }

    if (program != 0) {
        clReleaseProgram(program);
    }

    if (context != 0) {
        clReleaseContext(context);
    }
}












