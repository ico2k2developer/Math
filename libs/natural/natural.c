//
// Created by ico2k2 on 05/11/2023.
//

#include "natural.h"

number_N_p number_N_new(size_t bytes)
{
    if(bytes == 0)
        return NULL;
    number_N_p result = (number_N_p)malloc(sizeof(number_N_t));
    if(result == NULL)
        return NULL;
    result->length = bytes / sizeof(NUMBER_N_TYPE_DATA);
    if(number_N_length_bytes(result->length) < bytes)
        result->length++;
    result->data = (NUMBER_N_TYPE_DATA*)calloc(result->length,sizeof(NUMBER_N_TYPE_DATA));
    if(result->data == NULL)
    {
        free(result);
        return NULL;
    }
    return result;
}

void number_N_free(number_N_p* n)
{
    free((*n)->data);
    free(*n);
    *n = NULL;
}

size_t number_N_resize(number_N_p n,size_t bytes)
{
    if(bytes == 0)
        return n->length;
    size_t new = bytes / sizeof(NUMBER_N_TYPE_DATA);
    if(number_N_length_bytes(new) < bytes)
        new++;
    if(n->length != new)
    {
        NUMBER_N_TYPE_DATA* result = (NUMBER_N_TYPE_DATA*)realloc(n->data,number_N_length_bytes(new));
        if(result != NULL)
        {
            n->data = result;
            n->length = new;
        }
    }
    return n->length;
}

number_N_p number_N_set(number_N_p n,size_t pos,NUMBER_N_TYPE_DATA value)
{
    if(pos < n->length)
        n->data[pos] = value;
    return n;
}

number_N_p number_N_set_64(number_N_p n,uint64_t value)
{
    if(number_N_size_bytes(n) >= sizeof(value))
    {
        ((uint64_t*)n->data)[0] = value;
        memset(((uint8_t*)n->data) + sizeof(value),0,number_N_size_bytes(n) - sizeof(value));
    }
    else
    {
        uint8_t i;
        for(i = 0; i < n->length; i++)
            n->data[i] = (value >> ((i * sizeof(n->data[0]) << 3)) & ((1ll << (sizeof(n->data[0]) << 3)) - 1ll));
    }
    return n;
}

number_N_p number_N_set_byte(number_N_p n,size_t byte,uint8_t value)
{
    if(byte < number_N_size_bytes(n))
        number_N_get_byte(n,byte) = value;
    return n;
}

char* number_N_str_hex(number_N_p n,char** target,NULLABLE size_t* length,uint8_t zero_skip)
{
    if(target == NULL)
        return NULL;
    size_t i,j;
    uint8_t byte;
    if(zero_skip)
    {
        for(i = number_N_size_bytes(n); i > 0; i--)
        {
            if(number_N_get_byte(n,i - 1) != 0)
                break;
        }
    }
    else
    {
        i = number_N_size_bytes(n);
    }
    j = sizeof(char) * ((i << 1) + 1);
    if(j > *length)
        *target = (char*)realloc(*target,j);
    if(length != NULL)
        *length = j;
    j = 0;
    for(; i > 0; i--)
    {
        byte = number_N_get_byte(n,i - 1);
        if(!zero_skip || byte > 0x0F)
            (*target)[j++] = byte < 0xA0 ? (byte >> 4) + '0' : (byte >> 4) + 'a' - 10;
        (*target)[j++] = (byte & 0x0F) < 0x0A ? (byte & 0x0F) + '0' : (byte & 0x0F) + 'a' - 10;
        zero_skip = 0;
    }
    (*target)[j] = '\0';
    return *target;
}

number_N_p number_N_dup(number_N_p n)
{
    number_N_p result = (number_N_p)malloc(sizeof(number_N_t));
    if(result == NULL)
        return NULL;
    result->data = (NUMBER_N_TYPE_DATA*)malloc(number_N_size_bytes(n));
    if(result->data == NULL)
    {
        free(result);
        return NULL;
    }
    result->length = n->length;
    memcpy_s(result->data,number_N_size_bytes(result),n->data,number_N_size_bytes(n));
    return result;
}

int8_t number_N_comp(number_N_p source,number_N_p comp)
{
    size_t i = source->length;
    if(source->length > comp->length)
    {
        for(; i > comp->length; i--)
        {
            if(source->data[i - 1])
                return NUMBER_N_COMP_MAJOR;
        }
    }
    else if(source->length < comp->length)
    {
        for(i = comp->length; i > source->length; i--)
        {
            if(comp->data[i - 1])
                return NUMBER_N_COMP_MINOR;
        }
    }
    for(; i > 0; i--)
    {
        if(source->data[i - 1] > comp->data[i - 1])
            return NUMBER_N_COMP_MAJOR;
        else if(source->data[i - 1] < comp->data[i - 1])
            return NUMBER_N_COMP_MINOR;
    }
    return NUMBER_N_COMP_EQUAL;
}

int8_t number_N_comp_64(number_N_p source,uint64_t comp)
{
    size_t i = number_N_size_bytes(source);
    if(number_N_size_bytes(source) > sizeof(comp))
    {
        for(; i > sizeof(comp); i--)
        {
            if(number_N_get_byte(source,i - 1))
                return NUMBER_N_COMP_MAJOR;
        }
    }
    else if(number_N_size_bytes(source) < sizeof(comp))
    {
        for(i = sizeof(comp); i > number_N_size_bytes(source); i--)
        {
            if(NUMBER_N_HWORD_64(comp,i - 1))
                return NUMBER_N_COMP_MINOR;
        }
    }
    for(; i > 0; i--)
    {
        if(number_N_get_byte(source,i - 1) > NUMBER_N_HWORD_64(comp,i - 1))
            return NUMBER_N_COMP_MAJOR;
        else if(number_N_get_byte(source,i - 1) < NUMBER_N_HWORD_64(comp,i - 1))
            return NUMBER_N_COMP_MINOR;
    }
    return NUMBER_N_COMP_EQUAL;
}

number_N_p number_N_add(number_N_p target,number_N_p add)
{
    size_t target_l = number_N_size_bytes(target) / sizeof(uint32_t);
    size_t add_l = number_N_size_bytes(add) / sizeof(uint32_t);
    size_t i;
    if(target_l > 0 && add_l > 0)
    {
        uint64_t temp = 0;
        for(i = 0; i < target_l; i++)
        {
            temp += ((uint32_t*)target->data)[i] + (i < add_l ? ((uint32_t*)add->data)[i] : 0);
            ((uint32_t*)target->data)[i] = temp & UINT32_MAX;
            temp = temp >> (8 * sizeof(uint32_t));
            if(temp == 0 && (number_N_size_bytes(add) >> 3) <= i)
                break;
        }
    }
    else
    {
        uint16_t temp = 0;
        target_l = number_N_size_bytes(target);
        add_l = number_N_size_bytes(add);
        for(i = 0; i < target_l; i++)
        {
            temp += number_N_get_byte(target,i) + (i < add_l ? number_N_get_byte(add,i) : 0);
            number_N_get_byte(target,i) = temp & UINT8_MAX;
            temp = temp >> (8 * sizeof(uint8_t));
            if(temp == 0 && (add_l >> 3) <= i)
                break;
        }
    }
    return target;
}

number_N_p number_N_add_64(number_N_p target,uint64_t add)
{
    size_t target_l = number_N_size_bytes(target) / sizeof(uint32_t);
    size_t add_l = sizeof(add) / sizeof(uint32_t);
    size_t i;
    if(target_l > 0)
    {
        uint64_t temp = 0;
        for(i = 0; i < target_l; i++)
        {
            temp += ((uint32_t*)target->data)[i] + (i < add_l ? NUMBER_N_DWORD_64(add,i) : 0);
            ((uint32_t*)target->data)[i] = temp & UINT32_MAX;
            temp = temp >> (8 * sizeof(uint32_t));
            if(temp == 0 && (sizeof(add) >> 3) <= i)
                break;
        }
    }
    else
    {
        uint16_t temp = 0;
        target_l = number_N_size_bytes(target);
        add_l = sizeof(add);
        for(i = 0; i < target_l; i++)
        {
            temp += number_N_get_byte(target,i) + (i < add_l ? NUMBER_N_HWORD_64(add,i) : 0);
            number_N_get_byte(target,i) = temp & UINT8_MAX;
            temp = temp >> (8 * sizeof(uint8_t));
            if(temp == 0 && (add_l >> 3) <= i)
                break;
        }
    }
    return target;
}

number_N_p number_N_sub(number_N_p target,number_N_p sub)
{
    size_t i;
    NUMBER_N_TYPE_DATA value,temp = 0;
    for(i = 0; i < target->length; i++)
    {
        value = (i < sub->length ? sub->data[i] : 0) + temp;
        if(target->data[i] < value)
            temp = ((value - target->data[i]) >> 4) + 1;
        else
            temp = 0;
        target->data[i] -= value;
    }
    return target;
}

number_N_p number_N_sub_64(number_N_p target,uint64_t sub)
{
    size_t i;
    uint8_t value,temp = 0;
    for(i = 0; i < number_N_size_bytes(target); i++)
    {
        value = NUMBER_N_HWORD_64(sub,i) + temp;
        if(number_N_get_byte(target,i) < value)
            temp = ((value - number_N_get_byte(target,i)) >> 4) + 1;
        else
            temp = 0;
        number_N_get_byte(target,i) -= value;
    }
    return target;
}

number_N_p number_N_and(number_N_p target,number_N_p and)
{
    size_t i,min;
    min = and->length < target->length ? and->length : target->length;
    for(i = 0; i < min; i++)
        target->data[i] &= and->data[i];
    if(target->length > min)
        memset(target->data + min,0,number_N_length_bytes(target->length - min));
    return target;
}

number_N_p number_N_and_64(number_N_p target,uint64_t and)
{
    size_t i,min;
    min = sizeof(and) < number_N_size_bytes(target) ? sizeof(and) : number_N_size_bytes(target);
    if(min == sizeof(and))
        ((uint64_t*)target->data)[0] &= and;
    else
    {
        for(i = 0; i < min; i++)
            number_N_get_byte(target,i) &= NUMBER_N_HWORD_64(and,i);
    }
    if(number_N_size_bytes(target) > min)
        memset(((uint8_t*)target->data) + min,0,number_N_length_bytes(target->length) - min);
    return target;
}

number_N_p number_N_or(number_N_p target,number_N_p or)
{
    size_t i,min;
    min = or->length < target->length ? or->length : target->length;
    for(i = 0; i < min; i++)
        target->data[i] |= or->data[i];
    return target;
}

number_N_p number_N_or_64(number_N_p target,uint64_t or)
{
    size_t i,min;
    min = sizeof(or) < number_N_size_bytes(target) ? sizeof(or) : number_N_size_bytes(target);
    for(i = 0; i < min; i++)
        number_N_get_byte(target,i) |= NUMBER_N_HWORD_64(or,i);
    return target;
}

/*
number_N_p number_N_mul(number_N_p target,number_N_p mul)
{
    number_N_p i = number_N_dup(mul);
    number_N_p add = number_N_dup(target);
    while(number_N_comp_64(i,0) == NUMBER_N_COMP_MAJOR)
    {
        number_N_add(target,add);
        number_N_sub_64(i,1);
    }
    number_N_free(&add);
    number_N_free(&i);
    return target;
}

number_N_p number_N_mul_64(number_N_p target,uint64_t mul)
{
    size_t i;
    number_N_p add = number_N_dup(target);
    for(i = 1; i < mul; i++)
    {
        number_N_add(target,add);
    }
    number_N_free(&add);
    return target;
}*/


uint64_t number_N_convert(number_N_p n)
{
    if(number_N_size_bytes(n) >= sizeof(uint64_t))
        return ((uint64_t*)n->data)[0];
    else
    {
        uint64_t result = 0,temp;
        uint8_t i;
        for(i = 0; i < n->length; i++)
        {
            temp = (sizeof(n->data[0]) << 3) * i;
            temp = ((uint64_t)(n->data[i])) << temp;
            result |= temp;
        }
        return result;
    }
}