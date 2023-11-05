#include <stdio.h>

#define STR(x)  STR_(x)
#define STR_(x)  #x

#define ERR err_code

#define error() ERR = 1

#define cteststart() int ctest() { int ERR = 0; do {
#define ctestend } while(0); return ERR; } int main() { return ctest(); }

#define assert(...) do { if(!(__VA_ARGS__)) { fputs("Assertion \"" #__VA_ARGS__ "\" failed at line " \
        STR(__LINE__)"\n",stderr); error(); } } while(0)

#define zero(...) assert(__VA_ARGS__ == 0)
#define one(...) assert(__VA_ARGS__ == 1)
#define nonzero(...) assert(__VA_ARGS__ != 0)
#define null(...) assert(__VA_ARGS__ == NULL)
#define nonnull(...) assert(__VA_ARGS__ != NULL)