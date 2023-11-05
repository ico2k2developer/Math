//
// Created by ico2k2 on 05/11/2023.
//

#ifndef MATH_NATURAL_H
#define MATH_NATURAL_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_N_COMP_MAJOR 1
#define NUMBER_N_COMP_EQUAL 0
#define NUMBER_N_COMP_MINOR (-1)

//#define NUMBER_N_BYTE_64(n,i)   ((uint8_t)(((n) & (0xFF << ((i) << 3))) >> ((i) << 3)))

#define NUMBER_N_TYPE_LENGTH    size_t
#define NUMBER_N_TYPE_DATA      uint64_t

typedef struct
{
    NUMBER_N_TYPE_LENGTH length;
    NUMBER_N_TYPE_DATA* data;
}number_N_t;
typedef number_N_t* number_N_p;

#define number_N_length_bytes(length)   ((length) * sizeof(NUMBER_N_TYPE_DATA))
#define number_N_size_bytes(n)          ((n)->length * sizeof(NUMBER_N_TYPE_DATA))
#define number_N_get(n,pos)             (((n)->data)[byte])
#define number_N_get_byte(n,byte)       (((uint8_t*)((n)->data))[byte])

number_N_p number_N_new(size_t length);
void number_N_free(number_N_p* n);
size_t number_N_resize(number_N_p n,size_t length);
number_N_p number_N_set(number_N_p n,size_t pos,NUMBER_N_TYPE_DATA value);
number_N_p number_N_set_byte(number_N_p n,size_t byte,uint8_t value);
char* number_N_str_hex(number_N_p n,uint8_t zero_skip);
number_N_p number_N_dup(number_N_p n);
int8_t number_N_comp(number_N_p source,number_N_p comp);
int8_t number_N_comp_64(number_N_p source,uint64_t comp);
number_N_p number_N_add(number_N_p target,number_N_p add);
number_N_p number_N_add_64(number_N_p target,uint64_t add);
number_N_p number_N_sub(number_N_p target,number_N_p sub);
number_N_p number_N_sub_64(number_N_p target,uint64_t sub);
number_N_p number_N_and(number_N_p target,number_N_p and);
number_N_p number_N_and_64(number_N_p target,uint64_t and);
number_N_p number_N_or(number_N_p target,number_N_p or);
number_N_p number_N_or_64(number_N_p target,uint64_t or);
number_N_p number_N_mul(number_N_p target,number_N_p mul);
number_N_p number_N_mul_64(number_N_p target,uint64_t mul);
uint64_t number_N_convert(number_N_p n);

#endif //MATH_NATURAL_H
