/*
 *
 * Written by Michael Mattioli
 *
 */

#define __CL_ENABLE_EXCEPTIONS

#include "cl.hpp"
#include "util.h"
#include <string>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <chrono>

#ifndef DEVICE
#define DEVICE CL_DEVICE_TYPE_DEFAULT
#endif

#define LENGTH 1024 // Length of vectors a, b, and c.

int main() {

    // Host data structures.
    std::vector<float> hostVectorA(LENGTH), hostVectorB(LENGTH), hostVectorC(LENGTH);

    // Device data structures.
    cl::Buffer deviceVectorA, deviceVectorB, deviceVectorC;

    // Fill vectors A and B with random float values.
    for (int i = 0; i < LENGTH; i++) {
        hostVectorA[i] = rand() / (float)RAND_MAX;
        hostVectorB[i] = rand() / (float)RAND_MAX;
    }

    try {

        // Create a context.
        cl::Context context(DEVICE);

        // Load the kernel source and create a program object for the context.
        cl::Program program(context, util::LoadProgram("vector_add.cl"), true);

        // Get the command queue for the device.
        cl::CommandQueue queue(context, CL_QUEUE_PROFILING_ENABLE);

        // Create the kernel functor.
        auto vectorAdd = cl::make_kernel<cl::Buffer, cl::Buffer, cl::Buffer> (program, "VectorAdd");

        // Create the buffers. True indicates CL_MEM_READ_ONLY, False indicates CL_MEM_READ_WRITE.
        deviceVectorA = cl::Buffer(context, begin(hostVectorA), end(hostVectorA), true);
        deviceVectorB = cl::Buffer(context, begin(hostVectorB), end(hostVectorB), true);
        deviceVectorC = cl::Buffer(context, begin(hostVectorC), end(hostVectorC), false);

        // Enqueue the kernel for execution on the device.
        cl::Event event = vectorAdd(cl::EnqueueArgs(queue, cl::NDRange(LENGTH)),
                                    deviceVectorA,
                                    deviceVectorB,
                                    deviceVectorC);

        // Wait for the kernel to complete execution.
        queue.finish();

        // Copy the results from the device to the host.
        cl::copy(queue, deviceVectorC, begin(hostVectorC), end(hostVectorC));

        // Determine the kernel execution time.
        unsigned long deviceTimeEnd = event.getProfilingInfo<CL_PROFILING_COMMAND_END>();
        unsigned long deviceTimeStart = event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
        double executionTimeMicroseconds = (deviceTimeEnd - deviceTimeStart) / 1000.000;

        for (int i = 0; i < LENGTH; i++) {
           std::cout << "hostVectorA: " << hostVectorA[i] <<
                        " hostVectorB " << hostVectorB[i] <<
                        " hostVectorC " << hostVectorC[i] << std::endl;
        }

        std::cout   << "Completed in " << executionTimeMicroseconds << " microseconds using "
                    << context.getInfo<CL_CONTEXT_DEVICES>()[0].getInfo<CL_DEVICE_NAME>()
                    << "." << std::endl;

    }
    catch (cl::Error err) {
        std::cerr << "Error: " << err.what() << " " << err.err() << std::endl;
    }

}
