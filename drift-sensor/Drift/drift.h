/******************************************************************************/
/* drift.h                                                                    */
/******************************************************************************/

#ifndef __DRIFT_H
#define __DRIFT_H


#include "Filter\average.h"


typedef struct drift_threshold {
    sint16_t med;
    sint16_t up_front;
    sint16_t down_front;
    sint16_t gist;
} drift_threshold;


typedef struct drift_stats {
    sint32_t average;
    sint16_t min;
    sint16_t max;
    uint32_t count;

} drift_stats;


typedef struct calibr_data {
    sint16_t x;
    sint16_t y;
    sint16_t z;

} calibr_data;



// calibrating func
void init_calibrating_data();
void add_acc_samples_in_calibr(const uint16_t x_acc, const uint16_t y_acc, const uint16_t z_acc);
void commit_quiet_calibr_data();
void handle_acc_samples_in_calibr();
sint16_t commit_motion_calibr_data();
calibr_data * get_calibr_quiet_data();
calibr_data * get_calibr_motion_data();
float32_t * get_rotation_matrix();
void print_calibr_results();

// drift func
void init_reco_drift();
void add_acc_samples_in_reco_drift(const uint16_t x_acc, const uint16_t y_acc, const uint16_t z_acc);
void add_acc_matrix_samples_in_reco_drift(const uint16_t x_acc, const uint16_t y_acc, const uint16_t z_acc);

void set_x_acc_threshold(const sint16_t _med, const sint16_t _up, const sint16_t _down, const sint16_t _gist);
drift_threshold * get_x_acc_threshold();

void set_y_acc_threshold(const sint16_t _med, const sint16_t _up, const sint16_t _down, const sint16_t _gist);
drift_threshold * get_y_acc_threshold();

void set_z_acc_threshold(const sint16_t _med, const sint16_t _up, const sint16_t _down, const sint16_t _gist);
drift_threshold * get_z_acc_threshold();

void set_pit_acc_threshold(const sint16_t _med, const sint16_t _up, const sint16_t _down, const sint16_t _gist);
drift_threshold * get_pit_acc_threshold();

void set_mod_acc_threshold(const sint16_t _med, const sint16_t _up, const sint16_t _down, const sint16_t _gist);
drift_threshold * get_mod_acc_threshold();

average_buff * get_x_drift_filter();
average_buff * get_y_drift_filter();
average_buff * get_z_drift_filter();
average_buff * get_pit_drift_filter();
average_buff * get_mod_drift_filter();

#endif