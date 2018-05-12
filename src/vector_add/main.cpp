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
    std::vector<float> h_a(LENGTH), h_b(LENGTH), h_c(LENGTH);

    // Fill vectors A and B with random float values.
    for (int i = 0; i < LENGTH; i++) {
        h_a[i] = rand() / (float)RAND_MAX;
        h_b[i] = rand() / (float)RAND_MAX;
    }

    try {

        // Create a context.
        cl::Context context(DEVICE);

        // Load the kernel source and create a program object for the context.
        cl::Program program(context, util::LoadProgram("vector_add.cl"), true);

        // Get the command queue for the device.
        cl::CommandQueue queue(context, CL_QUEUE_PROFILING_ENABLE);

        // Create the kernel functor.
        auto VectorAdd = cl::make_kernel<cl::Buffer, cl::Buffer, cl::Buffer> (program, "VectorAdd");

        // Create the buffers.
        cl::Buffer d_a = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(float) * h_a.size());
        cl::Buffer d_b = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(float) * h_b.size());
        cl::Buffer d_c = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * LENGTH);

        // Write the host data to the device buffers.
        queue.enqueueWriteBuffer(d_a, CL_TRUE, 0, sizeof(float) * h_a.size(), h_a.data());
        queue.enqueueWriteBuffer(d_b, CL_TRUE, 0, sizeof(float) * h_b.size(), h_b.data());

        // Enqueue the kernel for execution on the device.
        cl::Event event = VectorAdd(cl::EnqueueArgs(queue, cl::NDRange(LENGTH)), d_a, d_b, d_c);

        // Wait for the kernel to complete execution.
        queue.finish();

        // Copy the results from the device to the host.
        cl::copy(queue, d_c, begin(h_c), end(h_c));

        // Determine the kernel execution time.
        unsigned long d_timeEnd = event.getProfilingInfo<CL_PROFILING_COMMAND_END>();
        unsigned long d_timeStart = event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
        double execTimeMicroseconds = (d_timeEnd - d_timeStart) / 1000.000;

        for (int i = 0; i < LENGTH; i++) {
            std::cout << "h_a: " << h_a[i] << " h_b " << h_b[i] << " h_c " << h_c[i] << std::endl;
        }

        std::cout   << "Completed in " << execTimeMicroseconds << " microseconds using "
                    << context.getInfo<CL_CONTEXT_DEVICES>()[0].getInfo<CL_DEVICE_NAME>()
                    << "." << std::endl;

    }
    catch (cl::Error error) {
        std::cerr << "Error: " << error.what() << " " << error.err() << std::endl;
    }

}
