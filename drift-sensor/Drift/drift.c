/******************************************************************************/
/* drift.c                                                                    */
/******************************************************************************/

#include "data_types.h"
#include "Core\core.h"
#include "Uart\v_printf.h"
#include "matrix\matrix.h"
#include "Sett\settings.h"
#include "Drift\drift.h"


static drift_threshold x_acc_thr = {0, 2500, 2500, 350, 10, 1};
static drift_threshold y_acc_thr = {0, 2500, 2500, 350, 10, 1};
static drift_threshold z_acc_thr = {8000, 2500, 2500, 350, 10, 1};
static drift_threshold pit_acc_thr = {8000, 1800, 1800, 150, 6, 0};
static drift_threshold mod_acc_thr = {8000, 2000, 2000, 150, 13, 2};

static drift_stats x_acc_stats;
static drift_stats y_acc_stats;
static drift_stats z_acc_stats;
static drift_stats pit_acc_stats;
static drift_stats mod_acc_stats;

static average_buff x_acc_filter;
static average_buff y_acc_filter;
static average_buff z_acc_filter;
static average_buff pit_acc_filter;
static average_buff mod_acc_filter;



/*


    key
     *  ^ x
        |
  y     |
  <-----. z

 */



/*
 * Acc X decision
 *
 */
static void acc_x_decision_reco_drift() {
    static uint8_t blank = 0;
    static sint16_t gisteresis = 0;
    const sint16_t average = x_acc_filter.average_type == AVERAGE_SIMPLE
                                ? get_average(&x_acc_filter) : get_weighted_average(&x_acc_filter);
    const sint16_t last_sample = get_last_sample(&x_acc_filter);

    if ((blank == 0 || blank == 1) && (average > (x_acc_thr.med + x_acc_thr.up_front - gisteresis))) {
        if (blank != 1) {
            blank = 1;

            x_acc_stats.average = last_sample;
            x_acc_stats.max = last_sample;
            x_acc_stats.min = last_sample;
            x_acc_stats.count = 1;
            gisteresis = x_acc_thr.gist;
        } else {
            x_acc_stats.average += last_sample;
            x_acc_stats.max = x_acc_stats.max > last_sample ? x_acc_stats.max : last_sample;
            x_acc_stats.min = x_acc_stats.min < last_sample ? x_acc_stats.min : last_sample;
            x_acc_stats.count++;
        }
    }
    else if ((blank == 0 || blank == 2) && (average < (x_acc_thr.med - x_acc_thr.down_front + gisteresis))) {
        if (blank != 2) {
            blank = 2;

            x_acc_stats.average = last_sample;
            x_acc_stats.max = last_sample;
            x_acc_stats.min = last_sample;
            x_acc_stats.count = 1;
            gisteresis = x_acc_thr.gist;
        } else {
            x_acc_stats.average += last_sample;
            x_acc_stats.max = x_acc_stats.max > last_sample ? x_acc_stats.max : last_sample;
            x_acc_stats.min = x_acc_stats.min < last_sample ? x_acc_stats.min : last_sample;
            x_acc_stats.count++;
        }
    }
    else {
        switch (blank) {
            case 1:
                serprintf("$DRIFT,XA,%4x,%4x,%4x,%u\r\n",
                    (int16_t)(x_acc_stats.average / x_acc_stats.count), x_acc_stats.max, x_acc_stats.min, x_acc_stats.count);
                break;

            case 2:
                serprintf("$DRIFT,XB,%4x,%4x,%4x,%u\r\n",
                    (int16_t)(x_acc_stats.average / x_acc_stats.count), x_acc_stats.max, x_acc_stats.min, x_acc_stats.count);
                break;

            default:
                blank = 0;
                gisteresis = 0;
                return;
        }
        blank = 0;
        gisteresis = 0;
    }
}


/*
 * Acc Y decision
 *
 */
static void acc_y_decision_reco_drift() {
    static uint8_t blank = 0;
    static sint16_t gisteresis = 0;
    const sint16_t average = y_acc_filter.average_type == AVERAGE_SIMPLE
                                ? get_average(&y_acc_filter) : get_weighted_average(&y_acc_filter);
    const sint16_t last_sample = get_last_sample(&y_acc_filter);

    if ((blank == 0 || blank == 1) && (average > (y_acc_thr.med + y_acc_thr.up_front - gisteresis))) {
        if (blank != 1) {
            blank = 1;

            y_acc_stats.average = last_sample;
            y_acc_stats.max = last_sample;
            y_acc_stats.min = last_sample;
            y_acc_stats.count = 1;
            gisteresis = y_acc_thr.gist;
        } else {
            y_acc_stats.average += last_sample;
            y_acc_stats.max = y_acc_stats.max > last_sample ? y_acc_stats.max : last_sample;
            y_acc_stats.min = y_acc_stats.min < last_sample ? y_acc_stats.min : last_sample;
            y_acc_stats.count++;
        }
    }
    else if ((blank == 0 || blank == 2) && (average < (y_acc_thr.med - y_acc_thr.down_front + gisteresis))) {
        if (blank != 2) {
            blank = 2;

            y_acc_stats.average = last_sample;
            y_acc_stats.max = last_sample;
            y_acc_stats.min = last_sample;
            y_acc_stats.count = 1;
            gisteresis = y_acc_thr.gist;
        } else {
            y_acc_stats.average += last_sample;
            y_acc_stats.max = y_acc_stats.max > last_sample ? y_acc_stats.max : last_sample;
            y_acc_stats.min = y_acc_stats.min < last_sample ? y_acc_stats.min : last_sample;
            y_acc_stats.count++;
        }
    }
    else {
        switch (blank) {
            case 1:
                serprintf("$DRIFT,YL,%4x,%4x,%4x,%u\r\n",
                    (int16_t)(y_acc_stats.average / y_acc_stats.count), y_acc_stats.max, y_acc_stats.min, y_acc_stats.count);
                break;

            case 2:
                serprintf("$DRIFT,YR,%4x,%4x,%4x,%u\r\n",
                    (int16_t)(y_acc_stats.average / y_acc_stats.count), y_acc_stats.max, y_acc_stats.min, y_acc_stats.count);
                break;

            default:
                blank = 0;
                gisteresis = 0;
                return;
        }
        blank = 0;
        gisteresis = 0;
    }
}


/*
 * Acc Z decision
 *
 */
static void acc_z_decision_reco_drift() {
    static uint8_t blank = 0;
    static sint16_t gisteresis = 0;
    const sint16_t average = z_acc_filter.average_type == AVERAGE_SIMPLE
                                ? get_average(&z_acc_filter) : get_weighted_average(&z_acc_filter);
    const sint16_t last_sample = get_last_sample(&z_acc_filter);

    if ((blank == 0 || blank == 1) && (average > (z_acc_thr.med + z_acc_thr.up_front - gisteresis))) {
        if (blank != 1) {
            blank = 1;

            z_acc_stats.average = last_sample;
            z_acc_stats.max = last_sample;
            z_acc_stats.min = last_sample;
            z_acc_stats.count = 1;
            gisteresis = z_acc_thr.gist;
        } else {
            z_acc_stats.average += last_sample;
            z_acc_stats.max = z_acc_stats.max > last_sample ? z_acc_stats.max : last_sample;
            z_acc_stats.min = z_acc_stats.min < last_sample ? z_acc_stats.min : last_sample;
            z_acc_stats.count++;
        }
    }
    else if ((blank == 0 || blank == 2) && (average < (z_acc_thr.med - z_acc_thr.down_front + gisteresis))) {
        if (blank != 2) {
            blank = 2;

            z_acc_stats.average = last_sample;
            z_acc_stats.max = last_sample;
            z_acc_stats.min = last_sample;
            z_acc_stats.count = 1;
            gisteresis = z_acc_thr.gist;
        } else {
            z_acc_stats.average += last_sample;
            z_acc_stats.max = z_acc_stats.max > last_sample ? z_acc_stats.max : last_sample;
            z_acc_stats.min = z_acc_stats.min < last_sample ? z_acc_stats.min : last_sample;
            z_acc_stats.count++;
        }
    }
    else {
        switch (blank) {
            case 1:
                serprintf("$DRIFT,Z1,%4x,%4x,%4x,%u\r\n",
                    (int16_t)(z_acc_stats.average / z_acc_stats.count), z_acc_stats.max, z_acc_stats.min, z_acc_stats.count);
                break;

            case 2:
                serprintf("$DRIFT,Z2,%4x,%4x,%4x,%u\r\n",
                    (int16_t)(z_acc_stats.average / z_acc_stats.count), z_acc_stats.max, z_acc_stats.min, z_acc_stats.count);
                break;

            default:
                blank = 0;
                gisteresis = 0;
                return;
        }
        blank = 0;
        gisteresis = 0;
    }
}


/*
 * Acc Z_PIT decision
 *
 */
static void acc_pit_decision_reco_drift() {
    static uint16_t pit_count = 0;
    static uint8_t blank = 0;
    static sint16_t gisteresis = 0;
    const sint16_t average = pit_acc_filter.average_type == AVERAGE_SIMPLE
                                ? get_average(&pit_acc_filter) : get_weighted_average(&pit_acc_filter);
    const sint16_t last_sample = get_last_sample(&pit_acc_filter);

    if (pit_count)
        pit_count--;

    if (average > (pit_acc_thr.med + pit_acc_thr.up_front - gisteresis)) {
        if (blank != 1) {
            blank = 1;

            if (pit_count) {
                serprintf("$DRIFT,ZA,%4x,%4x,%4x,%u\r\n",
                    (int16_t)(pit_acc_stats.average / pit_acc_stats.count), pit_acc_stats.max, pit_acc_stats.min, pit_acc_stats.count);
            }
            pit_count = 13;

            pit_acc_stats.average = last_sample;
            pit_acc_stats.max = last_sample;
            pit_acc_stats.min = last_sample;
            pit_acc_stats.count = 1;
            gisteresis = pit_acc_thr.gist;
        } else {
            pit_acc_stats.average += last_sample;
            pit_acc_stats.max = pit_acc_stats.max > last_sample ? pit_acc_stats.max : last_sample;
            pit_acc_stats.min = pit_acc_stats.min < last_sample ? pit_acc_stats.min : last_sample;
            pit_acc_stats.count++;
        }
    }
    else if (average < (pit_acc_thr.med - pit_acc_thr.down_front + gisteresis)) {
        if (blank != 2) {
            blank = 2;

            if (pit_count) {
                serprintf("$DRIFT,ZB,%4x,%4x,%4x,%u\r\n",
                    (int16_t)(pit_acc_stats.average / pit_acc_stats.count), pit_acc_stats.max, pit_acc_stats.min, pit_acc_stats.count);
            }
            pit_count = 13;

            pit_acc_stats.average = last_sample;
            pit_acc_stats.max = last_sample;
            pit_acc_stats.min = last_sample;
            pit_acc_stats.count = 1;
            gisteresis = pit_acc_thr.gist;
        } else {
            pit_acc_stats.average += last_sample;
            pit_acc_stats.max = pit_acc_stats.max > last_sample ? pit_acc_stats.max : last_sample;
            pit_acc_stats.min = pit_acc_stats.min < last_sample ? pit_acc_stats.min : last_sample;
            pit_acc_stats.count++;
        }
    }
    else {
        blank = 0;
        gisteresis = 0;
    }
}


/*
 * Acc PIT_MOD decision
 *
 */
static void acc_pit_mod_decision_reco_drift() {
    static sint32_t av_up = 0;
    static sint32_t av_down = 0;
    static uint16_t count_up = 0;

    static uint8_t blank = 0;
    static sint16_t gisteresis = 0;
    const sint16_t average = get_pit_average(&mod_acc_filter, mod_acc_thr.med);
    const sint16_t last_sample = get_last_sample(&mod_acc_filter);

    if (average > (mod_acc_thr.up_front - gisteresis)) {
        sint16_t lls = 0;
        uint32_t ccc = 0;

        if (last_sample < 0) {
             lls = (last_sample * -1);
             ccc = 0;
        } else {
             lls = last_sample;
             ccc = 1;
        }

        if (lls > mod_acc_thr.med) {
            lls -= mod_acc_thr.med;
        } else {
            lls = mod_acc_thr.med - lls;
        }

        if (blank != 1) {
            blank = 1;

            if (ccc) {
                av_up = lls;
                count_up = 1;
            } else {
                av_down = lls;
            }

            count_up = ccc;

            mod_acc_stats.average = last_sample;
            mod_acc_stats.max = last_sample;
            mod_acc_stats.min = last_sample;
            mod_acc_stats.count = 1;
            gisteresis = mod_acc_thr.gist;
        } else {
            mod_acc_stats.average += last_sample;
            mod_acc_stats.max = mod_acc_stats.max > last_sample ? mod_acc_stats.max : last_sample;
            mod_acc_stats.min = mod_acc_stats.min < last_sample ? mod_acc_stats.min : last_sample;
            mod_acc_stats.count++;

            if (ccc) {
                av_up += lls;
                count_up = 1;
            } else {
                av_down += lls;
            }

            count_up += ccc;
        }
    }
    else {
        if (blank == 1) {
            serprintf("$DRIFT,ZM,%4x,%4x,%4x,%u\r\n",
                (int16_t)(mod_acc_stats.average / mod_acc_stats.count), mod_acc_stats.max, mod_acc_stats.min, mod_acc_stats.count);
        }

        blank = 0;
        gisteresis = 0;
    }
}


/*
 * Init drift recognition
 *
 */
void init_reco_drift() {
    __FMEM_DATA data;

    // read drift sett
    data.addr = ACC_FILTR_DATA_ADDR;
    data.pBuff = (uint8_t *) &x_acc_thr;
    data.len = sizeof(drift_threshold);
    read_app_settings(&data);

    data.addr += sizeof(drift_threshold);
    data.pBuff = (uint8_t *) &y_acc_thr;
    read_app_settings(&data);

    data.addr += sizeof(drift_threshold);
    data.pBuff = (uint8_t *) &z_acc_thr;
    read_app_settings(&data);

    data.addr += sizeof(drift_threshold);
    data.pBuff = (uint8_t *) &pit_acc_thr;
    read_app_settings(&data);

    data.addr += sizeof(drift_threshold);
    data.pBuff = (uint8_t *) &mod_acc_thr;
    read_app_settings(&data);

    // Init acc filters
    init_average_filter(&x_acc_filter);
    set_wind_size_average_filter(&x_acc_filter, x_acc_thr.wind_size);
    set_average_type_filter(&x_acc_filter, x_acc_thr.average_type);

    init_average_filter(&y_acc_filter);
    set_wind_size_average_filter(&y_acc_filter, y_acc_thr.wind_size);
    set_average_type_filter(&y_acc_filter, y_acc_thr.average_type);

    init_average_filter(&z_acc_filter);
    set_wind_size_average_filter(&z_acc_filter, z_acc_thr.wind_size);
    set_average_type_filter(&z_acc_filter, z_acc_thr.average_type);

    init_average_filter(&pit_acc_filter);
    set_wind_size_average_filter(&pit_acc_filter, pit_acc_thr.wind_size);
    set_average_type_filter(&pit_acc_filter, pit_acc_thr.average_type);

    init_average_filter(&mod_acc_filter);
    set_wind_size_average_filter(&mod_acc_filter, mod_acc_thr.wind_size);
    set_average_type_filter(&mod_acc_filter, mod_acc_thr.average_type);

    // init calb sett
    data.addr = ACC_MATRIX_KOEFF_ADDR;
    data.pBuff = (uint8_t *) get_rotation_matrix();
    data.len = 36;
    read_app_settings(&data);

    // init axis indexes
    data.addr = ACC_AXIS_MAP_ADDR;
    data.pBuff = (uint8_t *) get_axis_data();
    data.len = 6;
    read_app_settings(&data);

    if (get_trace()) {
        print_calibr_results();
    }
}


/*
 * Add acc samples
 *
 */
void add_acc_samples_in_reco_drift(const uint16_t x_acc, const uint16_t y_acc, const uint16_t z_acc) {
    add_sample_in_filter(&x_acc_filter, x_acc);
    add_sample_in_filter(&y_acc_filter, y_acc);
    //add_sample_in_filter(&z_acc_filter, z_acc);
    //add_sample_in_filter(&pit_acc_filter, z_acc);
    add_sample_in_filter(&mod_acc_filter, z_acc);

    acc_x_decision_reco_drift();
    acc_y_decision_reco_drift();
    //acc_z_decision_reco_drift();
    //acc_pit_decision_reco_drift();
    acc_pit_mod_decision_reco_drift();
}


/*
 * Add acc matrix samples
 *
 */
void add_acc_matrix_samples_in_reco_drift(const uint16_t x_acc, const uint16_t y_acc, const uint16_t z_acc) {
    sint16_t rot_x, rot_y, rot_z;
    uint16_t * pAxis = get_axis_data();

    // rotate
    float32_t output_vector[3];
    const sint16_t input_vector[3] = {x_acc, y_acc, z_acc};

    multiply(input_vector, get_rotation_matrix(), output_vector);
    rot_x = (sint16_t)output_vector[pAxis[0] < 3 ? pAxis[0] : 0];
    rot_y = (sint16_t)output_vector[pAxis[1] < 3 ? pAxis[1] : 1];
    rot_z = (sint16_t)output_vector[pAxis[2] < 3 ? pAxis[2] : 2];

    add_sample_in_filter(&x_acc_filter, rot_x);
    add_sample_in_filter(&y_acc_filter, rot_y);
    //add_sample_in_filter(&z_acc_filter, (sint16_t)output_vector[2]);
    //add_sample_in_filter(&pit_acc_filter, (sint16_t)output_vector[2]);
    add_sample_in_filter(&mod_acc_filter, rot_z);

    acc_x_decision_reco_drift();
    acc_y_decision_reco_drift();
    //acc_z_decision_reco_drift();
    //acc_pit_decision_reco_drift();
    acc_pit_mod_decision_reco_drift();
}


/*
 * Set threshold
 *
 */
void set_acc_threshold(drift_threshold * const pTr, const drift_threshold * const pEtalonTr) {
    pTr->med = pEtalonTr->med;
    pTr->up_front = (pEtalonTr->up_front < 0) ? (-1 * pEtalonTr->up_front) : pEtalonTr->up_front;
    pTr->down_front = (pEtalonTr->down_front < 0) ? (-1 * pEtalonTr->down_front) : pEtalonTr->down_front;
    pTr->gist = (pEtalonTr->gist < 0) ? (-1 * pEtalonTr->gist) : pEtalonTr->gist;
    pTr->wind_size = pEtalonTr->wind_size;
    pTr->average_type = pEtalonTr->average_type;
}


/*
 * Get x_acc_thr
 *
 */
drift_threshold * get_x_acc_threshold() {
    return &x_acc_thr;
}

/*
 * Get y_acc_thr
 *
 */
drift_threshold * get_y_acc_threshold() {
    return &y_acc_thr;
}

/*
 * Get z_acc_thr
 *
 */
drift_threshold * get_z_acc_threshold() {
    return &z_acc_thr;
}


/*
 * Get pit_acc_thr
 *
 */
drift_threshold * get_pit_acc_threshold() {
    return &pit_acc_thr;
}


/*
 * Get pit_acc_thr
 *
 */
drift_threshold * get_mod_acc_threshold() {
    return &mod_acc_thr;
}


/*
 * Get x filter
 *
 */
average_buff * get_x_drift_filter() {
    return &x_acc_filter;
}


/*
 * Get y filter
 *
 */
average_buff * get_y_drift_filter() {
    return &y_acc_filter;
}


/*
 * Get z filter
 *
 */
average_buff * get_z_drift_filter() {
    return &z_acc_filter;
}


/*
 * Get pit filter
 *
 */
average_buff * get_pit_drift_filter() {
    return &pit_acc_filter;
}


/*
 * Get mod filter
 *
 */
average_buff * get_mod_drift_filter() {
    return &mod_acc_filter;
}

