/*
 *
 * Written by Michael Mattioli
 *
 */

void Reduce(local double*, global double*);

/*
 * Perform numerical integration to compute an approximation of Pi using the formula
 * pi = integral from 0 to 1 of 4/(1+x^2) dx
 */
kernel void ApproximatePi(  const int stepCount,
                            const double stepWidth,
                            local double* pieces,
                            global double* chunks) {

    int start = ((get_group_id(0) * get_local_size(0)) + get_local_id(0)) * stepCount;
    int end = start + stepCount;

    double midpoint, height;
    for (int i = start; i < end; i++) {
        midpoint = (i + 0.5) * stepWidth;
        height += 4.0 / (1.0 + (midpoint * midpoint));
    }

    pieces[get_local_id(0)] = height;
    barrier(CLK_LOCAL_MEM_FENCE);
    Reduce(pieces, chunks);

}

/*
 * Determine the sum of all of the pieces (work-items) of a chunk (work-group).
 */
void Reduce(local double* pieces, global double* chunks) {

    if (get_local_id(0) != 0) {
        return;
    }

    double chunk = 0.0;
    for (size_t i = 0; i < get_local_size(0); i++) {
        chunk += pieces[i];
    }

    chunks[get_group_id(0)] = chunk;

}
