/*
 *
 * Written by Michael Mattioli
 *
 */


/*
 * Simple vector addition.
 * C = A + B
 */
kernel void VectorAdd(global float* a, global float* b, global float* c) {

    // Get the current global work-item ID.
    size_t id = get_global_id(0);

    // Perform addition.
    c[id] = a[id] + b[id];

}
