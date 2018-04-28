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

    // Device data structures.
    cl::Buffer d_a, d_b, d_c;

    // Fill vectors A and B with random float values.
    for (int i = 0; i < LENGTH; i++) {
        h_a[i] = rand() / (float)RAND_MAX;
        h_b[i] = rand() / (float)RAND_MAX;
    }

    try {

        // Create a context.
        cl::Context context(DEVICE);

        // Load the kernel source and create a program object for the context.
        cl::Program program(context, util::loadProgram("vadd.cl"), true);

        // Get the command queue for the device.
        cl::CommandQueue queue(context);

        // Create the kernel functor.
        auto vadd = cl::make_kernel<cl::Buffer, cl::Buffer, cl::Buffer> (program, "vadd");

        // Create the buffers. True indicates CL_MEM_READ_ONLY, False indicates CL_MEM_READ_WRITE.
        d_a = cl::Buffer(context, begin(h_a), end(h_a), true);
        d_b = cl::Buffer(context, begin(h_b), end(h_b), true);
        d_c = cl::Buffer(context, begin(h_c), end(h_c), false);

        auto start = std::chrono::high_resolution_clock::now();

        // Enqueue the kernel for execution on the device.
        vadd(cl::EnqueueArgs(queue, cl::NDRange(LENGTH)), d_a, d_b, d_c);

        // Wait for the kernel to complete execution.
        queue.finish();

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

        // Copy the results from the device to the host.
        cl::copy(queue, d_c, begin(h_c), end(h_c));

        for (int i = 0; i < LENGTH; i++) {
            std::cout << "h_a: " << h_a[i] << " h_b " << h_b[i] << " h_c " << h_c[i] << std::endl;
        }
        std::cout   << "Completed in " << duration.count() << " microseconds using "
                    << context.getInfo<CL_CONTEXT_DEVICES>()[0].getInfo<CL_DEVICE_NAME>()
                    << "." << std::endl;

    }
    catch (cl::Error err) {
        std::cerr << "Error: " << err.what() << " " << err.err() << std::endl;
    }

}
