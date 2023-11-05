#include <stdio.h>
#include <natural.h>

int main()
{
    char* temp;
    number_N_p n1 = number_N_new(3);
    //number_N_p n2 = number_N_new(1);
    number_N_set_byte(n1, 0,12);
    number_N_set_byte(n1, 7,33);
    number_N_set_byte(n1, 8,22);
    number_N_set_byte(n1, 15,100);
    number_N_set_byte(n1, 16,255);
    number_N_set_byte(n1, 22,11);
    //number_N_set(n2, 0,0x1);
    temp = number_N_str_hex(n1,0);
    puts(temp);
    free(temp);
    temp = number_N_str_hex(n1,1);
    puts(temp);
    free(temp);


    number_N_free(&n1);
    //number_N_free(&n2);
    return 0;
}
