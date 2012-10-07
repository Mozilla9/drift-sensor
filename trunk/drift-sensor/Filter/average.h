/******************************************************************************/
/* average.h                                                                  */
/******************************************************************************/

#ifndef __AVERAGE_H
#define __AVERAGE_H



#define AVERAGE_MAX_WINDOW_SIZE    100
#define AVERAGE_DEF_WINDOW_SIZE    10

#define AVERAGE_SIMPLE             0
#define AVERAGE_WEIGHT             1
#define AVERAGE_MOD_PIT            2
#define AVERAGE_DEF_TYPE           AVERAGE_WEIGHT



typedef struct average_buff {
    sint16_t buff[AVERAGE_MAX_WINDOW_SIZE];
    uint16_t fill_index;
    uint16_t wind_size;
    uint16_t average_type;
} average_buff;



void init_average_filter(average_buff * const _pData);
void set_wind_size_average_filter(average_buff * const _pData, const uint16_t _wind_size);
void set_average_type_filter(average_buff * const _pData, const uint16_t _type);
void add_sample_in_filter(average_buff * const _pData, const sint16_t _sample);
sint16_t get_last_sample(average_buff * const _pData);
sint16_t get_average(average_buff * const _pData);
sint16_t get_weighted_average(average_buff * const _pData);
sint16_t get_pit_average(average_buff * const _pData, const sint16_t _med);


#endif