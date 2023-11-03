#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define NUMBER_N_COMP_MAJOR 1
#define NUMBER_N_COMP_EQUAL 0
#define NUMBER_N_COMP_MINOR (-1)

#define NUMBER_N_BYTE_64(n,i)   ((uint8_t)(((n) & (0xFF << ((i) << 3))) >> ((i) << 3)))

typedef struct
{
    size_t bytes;
    uint8_t * data;
}number_N_t;
typedef number_N_t* number_N_p;

number_N_p number_N_new(size_t bytes)
{
    number_N_p result = (number_N_p)malloc(sizeof(number_N_t));
    if(result == NULL)
        return NULL;
    result->data = (uint8_t*)calloc(bytes,sizeof(uint8_t));
    if(result->data == NULL)
    {
        free(result);
        return NULL;
    }
    result->bytes = bytes;
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
    if(n->bytes != bytes)
    {
        uint8_t * result = (uint8_t*)realloc(n->data,bytes);
        if(result != NULL)
        {
            n->data = result;
            n->bytes = bytes;
        }
    }
    return n->bytes;
}

number_N_p number_N_set_64(number_N_p n,uint64_t value)
{
    uint64_t select = 0xFF;
    size_t i;
    memset(n->data,0,n->bytes);
    for(i = 0; select != 0 && i < n->bytes; i++)
    {
        n->data[i] = (value & select) >> (i << 3);
        select = select << 8;
    }
    return n;
}

char* number_N_str_hex(number_N_p n)
{
    size_t i,length;
    char* result;
    for(i = 0; i < n->bytes; i++)
        if(n->data[n->bytes - 1 - i] != 0)
            break;
    length = ((n->bytes - i) << 1);
    if(length < 2)
    {
        length = 2;
        result = (char*)malloc(sizeof(char) * length);
        strcpy_s(result,length,"0");
    }
    else
    {
        result = (char*)calloc(length + 1,sizeof(char));
        uint8_t value1,value2;
        for(i = 0; i < length; i++)
        {
            value1 = n->data[(length >> 1) - 1 - (i >> 1)];
            value2 = (value1 & 0xF0) >> 4;
            value1 = value1 & 0x0F;
            if(i > 0 || value2)
            {
                result[i] = (char) (value2 < 10 ? value2 + '0' : value2 - 10 + 'a');
                i++;
            }
            else
                length--;
            result[i] = (char) (value1 < 10 ? value1 + '0' : value1 - 10 + 'a');
        }
    }
    return result;
}

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
}

int main()
{
    char* temp;
    number_N_p n1 = number_N_new(3);
    number_N_p n2 = number_N_new(1);
    number_N_set_64(n1,100);
    number_N_set_64(n2,0x1);
    /*fputs(temp = number_N_str_hex(n1),stdout);
    free(temp);
    fputs(" * ",stdout);
    fputs(temp = number_N_str_hex(n2),stdout);
    free(temp);
    fputs(" = ",stdout);*/
    uint64_t i = 0;
    do
    {
        //number_N_set_64(n1,i);
        printf("DEC: %llu\tHEX: ",number_N_convert(n1));
        puts(temp = number_N_str_hex(n1));
        free(temp);
        number_N_sub(n1,n2);
        i++;
    }
    while(i < 200);

    number_N_free(&n1);
    number_N_free(&n2);
    return 0;
}
