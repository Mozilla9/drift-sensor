/******************************************************************************/
/* calibrating.c                                                              */
/******************************************************************************/

#include "data_types.h"
#include "Core\core.h"
#include "Lib\float_to_txt.h"
#include "Matrix\matrix.h"
#include "Uart\v_printf.h"
#include "Drift\drift.h"


static calibr_data quiet_data;
static calibr_data motion_data;
static sint16_t motion_max_d;

static average_buff x_acc_filter;
static average_buff y_acc_filter;
static average_buff z_acc_filter;

static float32_t rotation[9] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};

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
    const sint16_t quiet[3] = {quiet_data.x, quiet_data.y, quiet_data.z};
    const sint16_t motion[3] = {motion_data.x, motion_data.y, motion_data.z};

    return rotate(quiet, motion, rotation);
}


/*
 * Print calibr results
 *
 */
void print_calibr_results() {
    uint32_t ff_comp[3];
    int8_t add_null[4];

    DEBUG_PRINTF("*********** Results ***************\r\n");
    DEBUG_PRINTF("quiet values:\r\n");
    DEBUG_PRINTF(" x = %d\r\n", quiet_data.x);
    DEBUG_PRINTF(" y = %d\r\n", quiet_data.y);
    DEBUG_PRINTF(" z = %d\r\n", quiet_data.z);

    DEBUG_PRINTF("motion max values:\r\n");
    DEBUG_PRINTF(" x = %d\r\n", motion_data.x);
    DEBUG_PRINTF(" y = %d\r\n", motion_data.y);
    DEBUG_PRINTF(" z = %d\r\n", motion_data.z);
    DEBUG_PRINTF("rotation matrix values:\r\n");

    split_float_to_comp(rotation[0], ff_comp);
    if (ff_comp[2] >= 1000 || !ff_comp[2]) {
        add_null[0] = 0;
    } else if (ff_comp[2] >= 100) {
        add_null[0] = '0';
        add_null[1] = 0;
    } else if (ff_comp[2] >= 10)  {
        add_null[0] = '0';
        add_null[1] = '0';
        add_null[2] = 0;
    } else {
        add_null[0] = '0';
        add_null[1] = '0';
        add_null[2] = '0';
        add_null[3] = 0;
    }
    DEBUG_PRINTF("rotation[0] = %c%d.%s%d\r\n", ff_comp[0] ? '-' : '+', ff_comp[1], add_null, ff_comp[2]);

    split_float_to_comp(rotation[1], ff_comp);
    if (ff_comp[2] >= 1000 || !ff_comp[2]) {
        add_null[0] = 0;
    } else if (ff_comp[2] >= 100) {
        add_null[0] = '0';
        add_null[1] = 0;
    } else if (ff_comp[2] >= 10)  {
        add_null[0] = '0';
        add_null[1] = '0';
        add_null[2] = 0;
    } else {
        add_null[0] = '0';
        add_null[1] = '0';
        add_null[2] = '0';
        add_null[3] = 0;
    }
    DEBUG_PRINTF("rotation[1] = %c%d.%s%d\r\n", ff_comp[0] ? '-' : '+', ff_comp[1], add_null, ff_comp[2]);

    split_float_to_comp(rotation[2], ff_comp);
    if (ff_comp[2] >= 1000 || !ff_comp[2]) {
        add_null[0] = 0;
    } else if (ff_comp[2] >= 100) {
        add_null[0] = '0';
        add_null[1] = 0;
    } else if (ff_comp[2] >= 10)  {
        add_null[0] = '0';
        add_null[1] = '0';
        add_null[2] = 0;
    } else {
        add_null[0] = '0';
        add_null[1] = '0';
        add_null[2] = '0';
        add_null[3] = 0;
    }
    DEBUG_PRINTF("rotation[2] = %c%d.%s%d\r\n", ff_comp[0] ? '-' : '+', ff_comp[1], add_null, ff_comp[2]);

    split_float_to_comp(rotation[3], ff_comp);
    if (ff_comp[2] >= 1000 || !ff_comp[2]) {
        add_null[0] = 0;
    } else if (ff_comp[2] >= 100) {
        add_null[0] = '0';
        add_null[1] = 0;
    } else if (ff_comp[2] >= 10)  {
        add_null[0] = '0';
        add_null[1] = '0';
        add_null[2] = 0;
    } else {
        add_null[0] = '0';
        add_null[1] = '0';
        add_null[2] = '0';
        add_null[3] = 0;
    }
    DEBUG_PRINTF("rotation[3] = %c%d.%s%d\r\n", ff_comp[0] ? '-' : '+', ff_comp[1], add_null, ff_comp[2]);

    split_float_to_comp(rotation[4], ff_comp);
    if (ff_comp[2] >= 1000 || !ff_comp[2]) {
        add_null[0] = 0;
    } else if (ff_comp[2] >= 100) {
        add_null[0] = '0';
        add_null[1] = 0;
    } else if (ff_comp[2] >= 10)  {
        add_null[0] = '0';
        add_null[1] = '0';
        add_null[2] = 0;
    } else {
        add_null[0] = '0';
        add_null[1] = '0';
        add_null[2] = '0';
        add_null[3] = 0;
    }
    DEBUG_PRINTF("rotation[4] = %c%d.%s%d\r\n", ff_comp[0] ? '-' : '+', ff_comp[1], add_null, ff_comp[2]);

    split_float_to_comp(rotation[5], ff_comp);
    if (ff_comp[2] >= 1000 || !ff_comp[2]) {
        add_null[0] = 0;
    } else if (ff_comp[2] >= 100) {
        add_null[0] = '0';
        add_null[1] = 0;
    } else if (ff_comp[2] >= 10)  {
        add_null[0] = '0';
        add_null[1] = '0';
        add_null[2] = 0;
    } else {
        add_null[0] = '0';
        add_null[1] = '0';
        add_null[2] = '0';
        add_null[3] = 0;
    }
    DEBUG_PRINTF("rotation[5] = %c%d.%s%d\r\n", ff_comp[0] ? '-' : '+', ff_comp[1], add_null, ff_comp[2]);

    split_float_to_comp(rotation[6], ff_comp);
    if (ff_comp[2] >= 1000 || !ff_comp[2]) {
        add_null[0] = 0;
    } else if (ff_comp[2] >= 100) {
        add_null[0] = '0';
        add_null[1] = 0;
    } else if (ff_comp[2] >= 10)  {
        add_null[0] = '0';
        add_null[1] = '0';
        add_null[2] = 0;
    } else {
        add_null[0] = '0';
        add_null[1] = '0';
        add_null[2] = '0';
        add_null[3] = 0;
    }
    DEBUG_PRINTF("rotation[6] = %c%d.%s%d\r\n", ff_comp[0] ? '-' : '+', ff_comp[1], add_null, ff_comp[2]);

    split_float_to_comp(rotation[7], ff_comp);
    if (ff_comp[2] >= 1000 || !ff_comp[2]) {
        add_null[0] = 0;
    } else if (ff_comp[2] >= 100) {
        add_null[0] = '0';
        add_null[1] = 0;
    } else if (ff_comp[2] >= 10)  {
        add_null[0] = '0';
        add_null[1] = '0';
        add_null[2] = 0;
    } else {
        add_null[0] = '0';
        add_null[1] = '0';
        add_null[2] = '0';
        add_null[3] = 0;
    }
    DEBUG_PRINTF("rotation[7] = %c%d.%s%d\r\n", ff_comp[0] ? '-' : '+', ff_comp[1], add_null, ff_comp[2]);

    split_float_to_comp(rotation[8], ff_comp);
    if (ff_comp[2] >= 1000 || !ff_comp[2]) {
        add_null[0] = 0;
    } else if (ff_comp[2] >= 100) {
        add_null[0] = '0';
        add_null[1] = 0;
    } else if (ff_comp[2] >= 10)  {
        add_null[0] = '0';
        add_null[1] = '0';
        add_null[2] = 0;
    } else {
        add_null[0] = '0';
        add_null[1] = '0';
        add_null[2] = '0';
        add_null[3] = 0;
    }
    DEBUG_PRINTF("rotation[8] = %c%d.%s%d\r\n", ff_comp[0] ? '-' : '+', ff_comp[1], add_null, ff_comp[2]);
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



