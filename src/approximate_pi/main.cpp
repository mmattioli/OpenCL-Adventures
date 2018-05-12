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

#define INTEGRATION_STEPS (512*512*512)
#define ITERATIONS (262144)

int main() {

    try {

        // Create a context.
        cl::Context context(DEVICE);

        // Create a device object from the context.
        cl::Device device = context.getInfo<CL_CONTEXT_DEVICES>()[0];

        // Load the kernel source and create a program object for the context.
        cl::Program program(context, util::LoadProgram("approximate_pi.cl"), true);

        // Get the command queue for the device.
        cl::CommandQueue queue(context, CL_QUEUE_PROFILING_ENABLE);

        // Create a kernel object so we can query information.
        cl::Kernel koApproximatePi(program, "ApproximatePi");

        // Get the chunk (work-group) size.
        size_t chunkSize = koApproximatePi.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(device);

        // Create the kernel functor.
        auto ApproximatePi = cl::make_kernel<int, double, cl::LocalSpaceArg, cl::Buffer>(program, "ApproximatePi");

        /*
         * Now that we know the chunk (work-group) size, we can determine the number of chunks, the
         * actual number of steps, and the step width.
         */
        size_t chunkCount = INTEGRATION_STEPS / (chunkSize * ITERATIONS);
        int stepCount = chunkSize * ITERATIONS * chunkCount;
        double stepWidth = 1.0 / (double)stepCount;
        std::vector<double> h_chunks(chunkCount);

        // Create the memory buffer on the device.
        cl::Buffer d_chunks = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(double) * chunkCount);

        // Enqueue the kernel for execution on the device.
        cl::Event evt = ApproximatePi(  cl::EnqueueArgs(queue,
                                                        cl::NDRange(stepCount / ITERATIONS), cl::NDRange(chunkSize)),
                                        ITERATIONS,
                                        stepWidth,
                                        cl::Local(sizeof(double) * chunkSize),
                                        d_chunks);

        // Wait for the kernel to complete execution.
        queue.finish();

        // Copy the results from the device to the host.
        cl::copy(queue, d_chunks, h_chunks.begin(), h_chunks.end());

        // Determine the kernel execution time.
        unsigned long d_timeEnd = evt.getProfilingInfo<CL_PROFILING_COMMAND_END>();
        unsigned long d_timeStart = evt.getProfilingInfo<CL_PROFILING_COMMAND_START>();
        double execTimeMicroseconds = (d_timeEnd - d_timeStart) / 1000.000;

        // The sum of all chunks is the approximation of Pi.
        double pi = 0.0;
        for (size_t i = 0; i < chunkCount; i++) {
                pi += h_chunks[i];
        }
        pi = pi * stepWidth;

        std::cout   << "Pi is approximately " << pi << "." << std::endl
                    << "Completed in " << executionTimeMicroseconds << " microseconds using "
                    << device.getInfo<CL_DEVICE_NAME>()
                    << "." << std::endl;

    }
    catch (cl::Error err) {
        std::cerr << "Error: " << err.what() << " " << err.err() << std::endl;
    }

}
