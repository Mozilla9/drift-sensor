/*
 * $Id$
 */

#include "data_types.h"
#include <math.h>
#include "matrix\matrix.h"



#define DIM_SIZE    3
#define EE          0.00001f



/*
 * rotate Z
 *
 * input: asc_data[]
 * output: rotation_z[]
 *
 */
static sint16_t rotate_z(const sint16_t acs_data[], float32_t rotation_z[]) {
    float32_t axis_len = 0;
    float32_t data_len = 0;
    float32_t turn_cos = 0;
    float32_t turn_sin = 0;
    float32_t axis[DIM_SIZE] = {0, 0, 0};

    /* axis vector length */
    axis_len = sqrt((float32_t)(acs_data[0] * acs_data[0] + acs_data[1] * acs_data[1]));

    if (axis_len < EE)
        return -1;

    /* axis vector coordinates */
    axis[0] = acs_data[1] / axis_len;
    axis[1] = - acs_data[0] / axis_len;

    /* turn angle cos and sin */
    data_len = sqrt((float32_t)(acs_data[0] * acs_data[0] + acs_data[1] * acs_data[1] + acs_data[2] * acs_data[2]));

    if (data_len < EE)
        return -2;

    turn_cos = acs_data[2] / data_len;
    turn_sin = sqrt(1 - turn_cos * turn_cos);

    /* rotation matrix components */
    rotation_z[0] = turn_cos + (1 - turn_cos) * axis[0] * axis[0];
    rotation_z[1] = (1 - turn_cos) * axis[0] * axis[1];
    rotation_z[2] = turn_sin * axis[1];

    rotation_z[3] = (1 - turn_cos) * axis[0] * axis[1];
    rotation_z[4] = turn_cos + (1 - turn_cos) * axis[1] * axis[1];
    rotation_z[5] = - turn_sin * axis[0];

    rotation_z[6] = - turn_sin * axis[1];
    rotation_z[7] = turn_sin * axis[0];
    rotation_z[8] = turn_cos;

    return 0;
}


/*
 * rotate X
 *
 * input:  asc_data[]
 * input:  rotation_z[]
 * output: rotation_x[]
 */
static sint16_t rotate_x(const sint16_t acs_data[], const float32_t rotation_z[], float32_t rotation_x[]) {
    float32_t turn_data[DIM_SIZE];
    float32_t data_len = 0;

    /* rotation input vector */
    multiply(acs_data, rotation_z, turn_data);

    data_len = sqrt(turn_data[0] * turn_data[0] + turn_data[1] * turn_data[1]);

    if (data_len < EE)
        return -1;

    /* rotation matrix components */
    rotation_x[0] = turn_data[0] / data_len;
    rotation_x[1] = turn_data[1] / data_len;
    rotation_x[2] = 0;

    rotation_x[3] = - turn_data[1] / data_len;
    rotation_x[4] = turn_data[0] / data_len;
    rotation_x[5] = 0;

    rotation_x[6] = 0;
    rotation_x[7] = 0;
    rotation_x[8] = 1;

    return 0;
}


/*
 * matrix multiply
 * input: matrix1[], matrix2[]
 * output: output_matrix[]
 *
 */
static void matrix_multiply(const float32_t matrix1[], const float32_t matrix2[], float32_t output_matrix[]) {
    output_matrix[0] = matrix1[0] * matrix2[0] + matrix1[1] * matrix2[3] + matrix1[2] * matrix2[6];
    output_matrix[1] = matrix1[0] * matrix2[1] + matrix1[1] * matrix2[4] + matrix1[2] * matrix2[7];
    output_matrix[2] = matrix1[0] * matrix2[2] + matrix1[1] * matrix2[5] + matrix1[2] * matrix2[8];

    output_matrix[3] = matrix1[3] * matrix2[0] + matrix1[4] * matrix2[3] + matrix1[5] * matrix2[6];
    output_matrix[4] = matrix1[3] * matrix2[1] + matrix1[4] * matrix2[4] + matrix1[5] * matrix2[7];
    output_matrix[5] = matrix1[3] * matrix2[2] + matrix1[4] * matrix2[5] + matrix1[5] * matrix2[8];

    output_matrix[6] = matrix1[6] * matrix2[0] + matrix1[7] * matrix2[3] + matrix1[8] * matrix2[6];
    output_matrix[7] = matrix1[6] * matrix2[1] + matrix1[7] * matrix2[4] + matrix1[8] * matrix2[7];
    output_matrix[8] = matrix1[6] * matrix2[2] + matrix1[7] * matrix2[5] + matrix1[8] * matrix2[8];
}


/*
 * multiply
 * input: input_vector[], matrix[]
 * output: output_vector[]
 *
 */
void multiply(const sint16_t input_vector[], const float32_t matrix[], float32_t output_vector[]) {
    output_vector[0] = input_vector[0] * matrix[0] + input_vector[1] * matrix[1] + input_vector[2] * matrix[2];
    output_vector[1] = input_vector[0] * matrix[3] + input_vector[1] * matrix[4] + input_vector[2] * matrix[5];
    output_vector[2] = input_vector[0] * matrix[6] + input_vector[1] * matrix[7] + input_vector[2] * matrix[8];
}


/*
 * rotate
 * input: asc_data1[], acs_data2[]
 * output: rotation[]
 *
 */
sint16_t rotate(const sint16_t acs_data1[], const sint16_t acs_data2[], float32_t rotation[]) {
    float32_t rotation_z[9];
    float32_t rotation_x[9];
    sint16_t rezult;
    rezult = rotate_z(acs_data1, rotation_z);

    if (rezult)
        return rezult;

    rezult = rotate_x(acs_data2, rotation_z, rotation_x);

    if (rezult)
        return rezult;

    matrix_multiply(rotation_x, rotation_z, rotation);
    return 0;
}