/******************************************************************************/
/* matrix.h                                                                   */
/******************************************************************************/

#ifndef __MATRIX_H
#define __MATRIX_H


sint16_t rotate(const sint16_t acs_data1[], const sint16_t acs_data2[], float32_t rotation[]);
void multiply(const sint16_t input_vector[], const float32_t matrix[], float32_t output_vector[]);


#endif