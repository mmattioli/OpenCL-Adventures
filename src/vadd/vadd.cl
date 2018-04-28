/*
 *
 * Written by Michael Mattioli
 *
 */


/*
 * Simple vector addition.
 * C = A + B
 */
__kernel void vadd( __global float* a, __global float* b, __global float* c) {

    // Get the current global work-item ID.
    size_t id = get_global_id(0);

    // Perform addition.
    c[id] = a[id] + b[id];

}
