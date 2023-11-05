//
// Created by ico2k2 on 05/11/2023.
//

#include "natural.h"

number_N_p number_N_new(size_t length)
{
    number_N_p result = (number_N_p)malloc(sizeof(number_N_t));
    if(result == NULL)
        return NULL;
    result->data = (NUMBER_N_TYPE_DATA*)calloc(length,sizeof(NUMBER_N_TYPE_DATA));
    if(result->data == NULL)
    {
        free(result);
        return NULL;
    }
    result->length = length;
    return result;
}

void number_N_free(number_N_p* n)
{
    free((*n)->data);
    free(*n);
    *n = NULL;
}

size_t number_N_resize(number_N_p n,size_t length)
{
    if(n->length != length)
    {
        NUMBER_N_TYPE_DATA* result = (NUMBER_N_TYPE_DATA*)realloc(n->data,number_N_length_bytes(length));
        if(result != NULL)
        {
            n->data = result;
            n->length = length;
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

number_N_p number_N_set_byte(number_N_p n,size_t byte,uint8_t value)
{
    if(byte < number_N_size_bytes(n))
        number_N_get_byte(n,byte) = value;
    return n;
}

char* number_N_str_hex(number_N_p n,uint8_t zero_skip)
{
    size_t i,j,bytes;
    uint8_t byte;
    char* result;
    if(zero_skip)
    {
        for(i = number_N_size_bytes(n); i > 0; i--)
        {
            if(number_N_get_byte(n,i - 1) != 0)
                break;
        }
        bytes = i;
    }
    else
    {
        i = bytes = number_N_size_bytes(n);
    }
    result = (char*)malloc(sizeof(char) * ((bytes << 1) + 1));
    j = 0;
    for(; i > 0; i--)
    {
        byte = number_N_get_byte(n,i - 1);
        if(!zero_skip || byte > 0x0F)
            result[j++] = byte < 0xA0 ? (byte >> 4) + '0' : (byte >> 4) + 'a' - 10;
        result[j++] = (byte & 0x0F) < 0x0A ? (byte & 0x0F) + '0' : (byte & 0x0F) + 'a' - 10;
        zero_skip = 0;
    }
    result[j] = '\0';
    return result;
}

/*
number_N_p number_N_dup(number_N_p n)
{
    number_N_p result = (number_N_p)malloc(sizeof(number_N_t));
    if(result == NULL)
        return NULL;
    result->data = (uint8_t*)malloc(n->bytes);
    if(result->data == NULL)
    {
        free(result);
        return NULL;
    }
    result->bytes = n->bytes;
    memcpy_s(result->data,result->bytes,n->data,n->bytes);
    return result;
}

int8_t number_N_comp(number_N_p source,number_N_p comp)
{
    size_t i;
    if(source->bytes > comp->bytes)
    {
        for(i = source->bytes; i > comp->bytes; i--)
        {
            if(source->data[i - 1])
                return NUMBER_N_COMP_MAJOR;
        }
    }
    else if(source->bytes < comp->bytes)
    {
        for(i = comp->bytes; i > source->bytes; i--)
        {
            if(comp->data[i - 1])
                return NUMBER_N_COMP_MINOR;
        }
    }
    for(i = source->bytes; i > 0; i--)
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
    size_t i;
    if(source->bytes > sizeof(comp))
    {
        for(i = source->bytes; i > sizeof(comp); i--)
        {
            if(source->data[i - 1])
                return NUMBER_N_COMP_MAJOR;
        }
    }
    else if(source->bytes < sizeof(comp))
    {
        for(i = sizeof(comp); i > source->bytes; i--)
        {
            if(NUMBER_N_BYTE_64(comp,i - 1))
                return NUMBER_N_COMP_MINOR;
        }
    }
    for(i = source->bytes; i > 0; i--)
    {
        if(source->data[i - 1] > NUMBER_N_BYTE_64(comp,i - 1))
            return NUMBER_N_COMP_MAJOR;
        else if(source->data[i - 1] < NUMBER_N_BYTE_64(comp,i - 1))
            return NUMBER_N_COMP_MINOR;
    }
    return NUMBER_N_COMP_EQUAL;
}

number_N_p number_N_add(number_N_p target,number_N_p add)
{
    size_t i;
    uint16_t temp = 0;
    for(i = 0; i < target->bytes; i++)
    {
        temp += target->data[i] + (i < add->bytes ? add->data[i] : 0);
        target->data[i] = temp & 0xFF;
        temp = temp >> 8;
        if(temp == 0 && add->bytes <= i)
            break;
    }
    return target;
}

number_N_p number_N_add_64(number_N_p target,uint64_t add)
{
    size_t i;
    uint16_t temp = 0;
    uint64_t value;
    for(i = 0; i < target->bytes; i++)
    {
        value = NUMBER_N_BYTE_64(add,i);
        temp += target->data[i] + value;
        target->data[i] = temp & 0xFF;
        temp = temp >> 8;
        if(temp == 0 && sizeof(add) <= i)
            break;
    }
    return target;
}

number_N_p number_N_sub(number_N_p target,number_N_p sub)
{
    size_t i;
    uint8_t value;
    uint16_t temp = 0;
    for(i = 0; i < target->bytes; i++)
    {
        value = (i < sub->bytes ? sub->data[i] : 0) + temp;
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
    uint8_t value;
    uint16_t temp = 0;
    for(i = 0; i < target->bytes; i++)
    {
        value = NUMBER_N_BYTE_64(sub,i) + temp;
        if(target->data[i] < value)
            temp = ((value - target->data[i]) >> 4) + 1;
        else
            temp = 0;
        target->data[i] -= value;
    }
    return target;
}

number_N_p number_N_and(number_N_p target,number_N_p and)
{
    size_t i,min;
    min = and->bytes < target->bytes ? and->bytes : target->bytes;
    for(i = 0; i < min; i++)
        target->data[i] = target->data[i] & and->data[i];
    return target;
}

number_N_p number_N_and_64(number_N_p target,uint64_t and)
{
    size_t i,min;
    min = sizeof(and) < target->bytes ? sizeof(and) : target->bytes;
    for(i = 0; i < min; i++)
        target->data[i] = target->data[i] & NUMBER_N_BYTE_64(and,i);
    return target;
}

number_N_p number_N_or(number_N_p target,number_N_p or)
{
    size_t i,min;
    min = or->bytes < target->bytes ? or->bytes : target->bytes;
    for(i = 0; i < min; i++)
        target->data[i] = target->data[i] | or->data[i];
    return target;
}

number_N_p number_N_or_64(number_N_p target,uint64_t or)
{
    size_t i,min;
    min = sizeof(or) < target->bytes ? sizeof(or) : target->bytes;
    for(i = 0; i < min; i++)
        target->data[i] = target->data[i] | NUMBER_N_BYTE_64(or,i);
    return target;
}

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
}


uint64_t number_N_convert(number_N_p n)
{
    if(n->bytes >= sizeof(uint64_t))
        return ((uint64_t*)n->data)[0];
    size_t i;
    uint64_t result = 0;
    for(i = 0; i < n->bytes; i++)
    {
        result = result | (n->data[i] << (i << 3));
    }
    return result;
}*/