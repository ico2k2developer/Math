#include <stdio.h>
#include <natural.h>

int main()
{
    char* temp = NULL;
    size_t temp_len = 0;
    const char* comp[] =
    {
        "minor than",
        "equal to",
        "major than"
    };
    number_N_p n1 = number_N_new(6);
    number_N_p n2 = number_N_new(1);
    number_N_or_64(n1,50);
    number_N_or_64(n1,5);
    number_N_and_64(n1,0x0f);
    number_N_set_byte(n2,0,255);
    //number_N_set(n2, 0,0x1);
    do
    {
        fputs(number_N_str_hex(n1,&temp,&temp_len,1),stdout);
        fputs(" is ",stdout);
        fputs(comp[number_N_comp(n1,n2) + 1],stdout);
        fputc(' ',stdout);
        puts(number_N_str_hex(n2,&temp,&temp_len,1));
        number_N_or(n1,n2);
        number_N_sub_64(n2,1);
    }
    while(number_N_comp_64(n2,150) != NUMBER_N_COMP_MINOR);
    free(temp);
    number_N_free(&n1);
    number_N_free(&n2);
    return 0;
}
