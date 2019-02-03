#include <cstdlib>
#include <cstdio>
int main()
{
    char *h=getenv("HPCE_FFT_RECURSION_K");
    if(h==NULL){
        printf("HPCE_FFT_RECURSION_K is not set.\n");
    }else{
        printf("HPCE_FFT_RECURSION_K = %s\n", h);
    }
    return 0;
}