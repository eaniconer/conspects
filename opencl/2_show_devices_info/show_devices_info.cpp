
#include <OpenCL/cl.h>

#include <iostream>

using namespace std;

void checkError(cl_int err, const string& msg = "") {
    if (err != CL_SUCCESS) {
        cerr << "Error: " << msg << endl;
    }
}

void displayDevicesInfo(cl_platform_id platform, cl_device_type deviceType) 
{
    cl_int err;
    cl_uint nDevices;
    err = clGetDeviceIDs(platform, deviceType, 0, NULL, &nDevices);
    checkError(err);
    cout << "nDevices: " << nDevices << endl;

    cl_device_id* devices = (cl_device_id*) alloca(sizeof(cl_device_id) * nDevices);
    err = clGetDeviceIDs(platform, deviceType, nDevices, (cl_device_id*) devices, NULL);
    checkError(err);

    for (int i = 0; i < nDevices; ++i) {
        cl_device_id id = devices[i];
        cout << id << endl;
        
        cl_uint value;
        size_t size;
        clGetDeviceInfo(id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &value, &size);
        std::cout << "\t CL_DEVICE_MAX_COMPUTE_UNITS: " << value << endl;

        clGetDeviceInfo(id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &value, &size);
        std::cout << "\t CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: " << value << endl;
        const cl_uint dim = value;

        size_t data[dim];
        clGetDeviceInfo(id, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * dim, &data, &size);
        
        std::cout << "\t CL_DEVICE_MAX_WORK_ITEM_SIZES: ";
        for (int i = 0; i < dim; ++i) {
            cout << data[i] << " ";
        }
        cout << endl;

        size_t tvalue;
        clGetDeviceInfo(id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &tvalue, &size);
        std::cout << "\t CL_DEVICE_MAX_WORK_GROUP_SIZE: " << tvalue << endl;

        clGetDeviceInfo(id, CL_DEVICE_ADDRESS_BITS, sizeof(cl_uint), &value, &size);
        std::cout << "\t CL_DEVICE_ADDRESS_BITS: " << value << endl;
    }
}

int main() 
{
    cl_platform_id platform;
    cl_int err;
    err = clGetPlatformIDs(1, &platform, NULL);
    checkError(err, "1");

    cout << "GPU" << endl;
    displayDevicesInfo(platform, CL_DEVICE_TYPE_GPU);
    cout << string(40, '-') << endl;

    cout << "CPU" << endl;
    displayDevicesInfo(platform, CL_DEVICE_TYPE_CPU);
    cout << string(40, '-') << endl;

    cout << "ACCELERATOR" << endl; // IBM's Cell Broadband
    displayDevicesInfo(platform, CL_DEVICE_TYPE_ACCELERATOR);
    cout << string(40, '-') << endl;

    cout << "DEFAULT" << endl;
    displayDevicesInfo(platform, CL_DEVICE_TYPE_DEFAULT);
    cout << string(40, '-') << endl;

    cout << "ALL" << endl;
    displayDevicesInfo(platform, CL_DEVICE_TYPE_ALL);
    cout << string(40, '-') << endl;

    cout << "Done";

    return 0;
}