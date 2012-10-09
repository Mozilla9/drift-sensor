/******************************************************************************/
/* drift.c                                                                    */
/******************************************************************************/

#include "data_types.h"
#include "Core\core.h"
#include "Uart\v_printf.h"
#include "matrix\matrix.h"
#include "Drift\drift.h"


static drift_threshold x_acc_thr;
static drift_threshold y_acc_thr;
static drift_threshold z_acc_thr;
static drift_threshold pit_acc_thr;
static drift_threshold mod_acc_thr;

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

        DEBUG_PRINTF("x_res: %d, %d, %d, %u\n\r",
            (int16_t)(x_acc_stats.average / x_acc_stats.count), x_acc_stats.max, x_acc_stats.min, x_acc_stats.count);
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

        DEBUG_PRINTF("y_res: %d, %d, %d, %u\n\r",
            (int16_t)(y_acc_stats.average / y_acc_stats.count), y_acc_stats.max, y_acc_stats.min, y_acc_stats.count);
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
                break;

            case 2:
                break;

            default:
                blank = 0;
                gisteresis = 0;
                return;
        }
        blank = 0;
        gisteresis = 0;

        DEBUG_PRINTF("z_res: %d, %d, %d, %u\n\r",
            (int16_t)(z_acc_stats.average / z_acc_stats.count), z_acc_stats.max, z_acc_stats.min, z_acc_stats.count);
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
                serprintf("$DRIFT,ZP1,%4x,%4x,%4x,%u\r\n",
                    (int16_t)(pit_acc_stats.average / pit_acc_stats.count), pit_acc_stats.max, pit_acc_stats.min, pit_acc_stats.count);

                DEBUG_PRINTF("Z_PIT_1: %d, %d, %d, %u\n\r",
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
                serprintf("$DRIFT,ZP2,%4x,%4x,%4x,%u\r\n",
                    (int16_t)(pit_acc_stats.average / pit_acc_stats.count), pit_acc_stats.max, pit_acc_stats.min, pit_acc_stats.count);

                DEBUG_PRINTF("Z_PIT_2: %d, %d, %d, %u\n\r",
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
            DEBUG_PRINTF("PIT_MOD_START: %d\n\r", average);
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

            DEBUG_PRINTF("PIT_MOD_END: %d, %d, %d, %u, av_up=%d, av_dw=%d\n\r",
                (int16_t)(mod_acc_stats.average / mod_acc_stats.count), mod_acc_stats.max, mod_acc_stats.min, mod_acc_stats.count,
                (av_up / count_up), (av_down / (mod_acc_stats.count - count_up)));
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
    // Init acc filters
    init_average_filter(&x_acc_filter);
    init_average_filter(&y_acc_filter);
    init_average_filter(&z_acc_filter);

    init_average_filter(&pit_acc_filter);
    set_wind_size_average_filter(&pit_acc_filter, 6);
    set_average_type_filter(&pit_acc_filter, AVERAGE_SIMPLE);

    init_average_filter(&mod_acc_filter);
    set_wind_size_average_filter(&mod_acc_filter, 13);
    set_average_type_filter(&mod_acc_filter, AVERAGE_MOD_PIT);

    x_acc_thr.med = 0;
    x_acc_thr.up_front = 2500;
    x_acc_thr.down_front = 2500;
    x_acc_thr.gist = 350;

    y_acc_thr.med = 0;
    y_acc_thr.up_front = 2500;
    y_acc_thr.down_front = 2500;
    y_acc_thr.gist = 350;

    z_acc_thr.med = 8000;
    z_acc_thr.up_front = 2500;
    z_acc_thr.down_front = 2500;
    z_acc_thr.gist = 350;

    pit_acc_thr.med = 8000;
    pit_acc_thr.up_front = 1800;
    pit_acc_thr.down_front = 1800;
    pit_acc_thr.gist = 150;

    mod_acc_thr.med = 8000;
    mod_acc_thr.up_front = 2000;
    mod_acc_thr.down_front = 2000;
    mod_acc_thr.gist = 150;
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
    // rotate
    float32_t output_vector[3];
    const sint16_t input_vector[3] = {x_acc, y_acc, z_acc};

    multiply(input_vector, get_rotation_matrix(), output_vector);

    add_sample_in_filter(&x_acc_filter, (sint16_t)output_vector[0]);
    add_sample_in_filter(&y_acc_filter, (sint16_t)output_vector[1]);
    //add_sample_in_filter(&z_acc_filter, (sint16_t)output_vector[2]);
    //add_sample_in_filter(&pit_acc_filter, (sint16_t)output_vector[2]);
    add_sample_in_filter(&mod_acc_filter, (sint16_t)output_vector[2]);

    acc_x_decision_reco_drift();
    acc_y_decision_reco_drift();
    //acc_z_decision_reco_drift();
    //acc_pit_decision_reco_drift();
    acc_pit_mod_decision_reco_drift();
}


/*
 * Set x threshold
 *
 */
void set_x_acc_threshold(const sint16_t _med, const sint16_t _up, const sint16_t _down, const sint16_t _gist) {
    x_acc_thr.med = _med;
    x_acc_thr.up_front = (_up < 0) ? (-1 * _up) : _up;
    x_acc_thr.down_front = (_down < 0) ? (-1 * _down) : _down;
    x_acc_thr.gist= (_gist < 0) ? (-1 * _gist) : _gist;
}


/*
 * Get x_acc_thr
 *
 */
drift_threshold * get_x_acc_threshold() {
    return &x_acc_thr;
}


/*
 * Set y threshold
 *
 */
void set_y_acc_threshold(const sint16_t _med, const sint16_t _up, const sint16_t _down, const sint16_t _gist) {
    y_acc_thr.med = _med;
    y_acc_thr.up_front = (_up < 0) ? (-1 * _up) : _up;
    y_acc_thr.down_front = (_down < 0) ? (-1 * _down) : _down;
    y_acc_thr.gist= (_gist < 0) ? (-1 * _gist) : _gist;
}


/*
 * Get y_acc_thr
 *
 */
drift_threshold * get_y_acc_threshold() {
    return &y_acc_thr;
}


/*
 * Set z threshold
 *
 */
void set_z_acc_threshold(const sint16_t _med, const sint16_t _up, const sint16_t _down, const sint16_t _gist) {
    z_acc_thr.med = _med;
    z_acc_thr.up_front = (_up < 0) ? (-1 * _up) : _up;
    z_acc_thr.down_front = (_down < 0) ? (-1 * _down) : _down;
    z_acc_thr.gist= (_gist < 0) ? (-1 * _gist) : _gist;
}


/*
 * Get z_acc_thr
 *
 */
drift_threshold * get_z_acc_threshold() {
    return &z_acc_thr;
}


/*
 * Set pit threshold
 *
 */
void set_pit_acc_threshold(const sint16_t _med, const sint16_t _up, const sint16_t _down, const sint16_t _gist) {
    pit_acc_thr.med = _med;
    pit_acc_thr.up_front = (_up < 0) ? (-1 * _up) : _up;
    pit_acc_thr.down_front = (_down < 0) ? (-1 * _down) : _down;
    pit_acc_thr.gist= (_gist < 0) ? (-1 * _gist) : _gist;
}


/*
 * Get pit_acc_thr
 *
 */
drift_threshold * get_pit_acc_threshold() {
    return &pit_acc_thr;
}


/*
 * Set mod threshold
 *
 */
void set_mod_acc_threshold(const sint16_t _med, const sint16_t _up, const sint16_t _down, const sint16_t _gist) {
    mod_acc_thr.med = _med;
    mod_acc_thr.up_front = (_up < 0) ? (-1 * _up) : _up;
    mod_acc_thr.down_front = (_down < 0) ? (-1 * _down) : _down;
    mod_acc_thr.gist= (_gist < 0) ? (-1 * _gist) : _gist;
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

