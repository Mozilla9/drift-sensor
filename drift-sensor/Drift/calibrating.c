/******************************************************************************/
/* calibrating.c                                                              */
/******************************************************************************/

#include "data_types.h"
#include "Core\core.h"
#include "Matrix\matrix.h"
#include "Uart\v_printf.h"
#include "Sett\settings.h"
#include "Drift\drift.h"


static calibr_data quiet_data;
static calibr_data motion_data;
static sint16_t motion_max_d;

static average_buff x_acc_filter;
static average_buff y_acc_filter;
static average_buff z_acc_filter;

static float32_t rotation[9] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
static uint16_t axis_ind[3] = {0, 1, 2};

/*
 * Init calibr data
 *
 */
void init_calibrating_data() {
    init_average_filter(&x_acc_filter);
    init_average_filter(&y_acc_filter);
    init_average_filter(&z_acc_filter);

    set_wind_size_average_filter(&x_acc_filter, AVERAGE_MAX_WINDOW_SIZE);
    set_wind_size_average_filter(&y_acc_filter, AVERAGE_MAX_WINDOW_SIZE);
    set_wind_size_average_filter(&z_acc_filter, AVERAGE_MAX_WINDOW_SIZE);

    quiet_data.x = 0;
    quiet_data.y = 0;
    quiet_data.z = 0;

    motion_data.x = 0;
    motion_data.y = 0;
    motion_data.z = 0;

    motion_max_d = 0;

    rotation[0] = rotation[1] = rotation[2] = 0.0;
    rotation[3] = rotation[4] = rotation[5] = 0.0;
    rotation[6] = rotation[7] = rotation[8] = 0.0;
}


/*
 * Add acc samples in calibr
 *
 */
void add_acc_samples_in_calibr(const uint16_t x_acc, const uint16_t y_acc, const uint16_t z_acc) {
    add_sample_in_filter(&x_acc_filter, x_acc);
    add_sample_in_filter(&y_acc_filter, y_acc);
    add_sample_in_filter(&z_acc_filter, z_acc);
}


/*
 * Handle accel data calibr
 *
 */
void handle_acc_samples_in_calibr() {
    const sint16_t x_sample = get_last_sample(&x_acc_filter);
    const sint16_t y_sample = get_last_sample(&y_acc_filter);
    const sint16_t z_sample = get_last_sample(&z_acc_filter);

    // get delta
    sint16_t x_d = get_last_sample(&x_acc_filter) - quiet_data.x;
    sint16_t y_d = get_last_sample(&y_acc_filter) - quiet_data.y;
    sint16_t z_d = get_last_sample(&z_acc_filter) - quiet_data.z;

    // set module val
    x_d = x_d < 0 ? (-1 * x_d) : x_d;
    y_d = y_d < 0 ? (-1 * y_d) : y_d;
    z_d = z_d < 0 ? (-1 * z_d) : z_d;

    // need find max delta
    bool_t isMaxFound = FALSE_T;
    if (x_d > motion_max_d || y_d > motion_max_d || z_d > motion_max_d) {
        isMaxFound = TRUE_T;

        if (x_d > y_d && x_d > z_d) {
            motion_max_d = x_d;
        } else if (y_d > x_d && y_d > z_d) {
            motion_max_d = y_d;
        } else if (z_d > x_d && z_d > y_d) {
            motion_max_d = z_d;
        } else {
            motion_max_d = MAX(x_d, y_d);
            motion_max_d = MAX(motion_max_d, z_d);
        }
    }

    if (isMaxFound) {
        motion_data.x = x_sample;
        motion_data.y = y_sample;
        motion_data.z = z_sample;
    }
}


/*
 * Commit quiet data calibr
 *
 */
void commit_quiet_calibr_data() {
    quiet_data.x = get_average(&x_acc_filter);
    quiet_data.y = get_average(&y_acc_filter);
    quiet_data.z = get_average(&z_acc_filter);
}


/*
 * Commit motion data calibr
 *
 */
sint16_t commit_motion_calibr_data() {
    __FMEM_DATA data;

    const sint16_t quiet[3] = {quiet_data.x, quiet_data.y, quiet_data.z};
    const sint16_t motion[3] = {motion_data.x, motion_data.y, motion_data.z};
    const sint16_t res = rotate(quiet, motion, rotation);

    data.addr = ACC_MATRIX_KOEFF_ADDR;
    data.pBuff = (uint8_t *) rotation;
    data.len = sizeof(rotation);

    if (res == 0) {
        write_app_settings(&data);
    } else {
        read_app_settings(&data);
    }
    return res;
}


/*
 * Commit axis calb data
 *
 */
void commit_axis_calibr_data() {
    __FMEM_DATA data;
    sint16_t rot_x, rot_y, rot_z;

    // search and set x-axis
    if (motion_data.x >= motion_data.y && motion_data.x >= motion_data.z) {
        axis_ind[0] = 0;
    } else if (motion_data.y >= motion_data.x && motion_data.y >= motion_data.z) {
        axis_ind[0] = 1;
    } else if (motion_data.z >= motion_data.y && motion_data.z >= motion_data.x) {
        axis_ind[0] = 2;
    }

    // rotate
    float32_t output_vector[3];
    const sint16_t input_vector[3] = {quiet_data.x, quiet_data.y, quiet_data.z};
    multiply(input_vector, rotation, output_vector);

    rot_x = (sint16_t)output_vector[0];
    rot_y = (sint16_t)output_vector[1];
    rot_z = (sint16_t)output_vector[2];

    // search and set z-axis
    if (rot_x >= rot_y && rot_x >= rot_z) {
        axis_ind[2] = 0;
    } else if (rot_y >= rot_x && rot_y >= rot_z) {
        axis_ind[2] = 1;
    } else if (rot_z >= rot_y && rot_z >= rot_x) {
        axis_ind[2] = 2;
    }

    // set y-axis
    axis_ind[1] = 3 - (axis_ind[0] + axis_ind[2]);

    // validating
    if (axis_ind[0] > 2 || axis_ind[1] > 2 || axis_ind[2] > 2) {
        axis_ind[0] = 0;
        axis_ind[1] = 1;
        axis_ind[2] = 2;
    }

    data.addr = ACC_AXIS_MAP_ADDR;
    data.pBuff = (uint8_t *) axis_ind;
    data.len = sizeof(axis_ind);
    write_app_settings(&data);
}


/*
 * Print calibr results
 *
 */
void print_calibr_results() {
    DEBUG_PRINTF("rotation matrix koeff:\r\n");

    DEBUG_PRINTF("rotation[0] = %f\r\n", rotation[0]);
    DEBUG_PRINTF("rotation[0] = %u\r\n", (uint32_t)rotation[0]);
    
    DEBUG_PRINTF("rotation[1] = %f\r\n", rotation[1]);
    DEBUG_PRINTF("rotation[1] = %u\r\n", (uint32_t)rotation[1]);
    
    DEBUG_PRINTF("rotation[2] = %f\r\n", rotation[2]);
    DEBUG_PRINTF("rotation[2] = %u\r\n", (uint32_t)rotation[2]);
    
    DEBUG_PRINTF("rotation[3] = %f\r\n", rotation[3]);
    DEBUG_PRINTF("rotation[3] = %u\r\n", (uint32_t)rotation[3]);
    
    DEBUG_PRINTF("rotation[4] = %f\r\n", rotation[4]);
    DEBUG_PRINTF("rotation[4] = %u\r\n", (uint32_t)rotation[4]);
    
    DEBUG_PRINTF("rotation[5] = %f\r\n", rotation[5]);
    DEBUG_PRINTF("rotation[5] = %f\r\n", rotation[5]);
    
    DEBUG_PRINTF("rotation[6] = %f\r\n", rotation[6]);
    DEBUG_PRINTF("rotation[6] = %u\r\n", (uint32_t)rotation[6]);
    
    DEBUG_PRINTF("rotation[7] = %f\r\n", rotation[7]);
    DEBUG_PRINTF("rotation[7] = %u\r\n", (uint32_t)rotation[7]);
    
    DEBUG_PRINTF("rotation[8] = %f\r\n", rotation[8]);
    DEBUG_PRINTF("rotation[8] = %u\r\n", (uint32_t)rotation[8]);

    DEBUG_PRINTF("axis matrix indx:\r\n");

    DEBUG_PRINTF("x_idx = %u\r\n", axis_ind[0]);
    DEBUG_PRINTF("y_idx = %u\r\n", axis_ind[1]);
    DEBUG_PRINTF("z_idx = %u\r\n", axis_ind[2]);
}


/*
 * Get calibr quiet data
 *
 */
calibr_data * get_calibr_quiet_data() {
    return &quiet_data;
}


/*
 * Get calibr motion data
 *
 */
calibr_data * get_calibr_motion_data() {
    return &motion_data;
}


/*
 * Get calibr motion data
 *
 */
float32_t * get_rotation_matrix() {
    return rotation;
}


/*
 * Get calibr axis data
 *
 */
uint16_t * get_axis_data() {
    return axis_ind;
}



