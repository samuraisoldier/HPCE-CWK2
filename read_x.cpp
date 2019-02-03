#include <cstdlib>
#include <cstdio>
int main()
{
    char *v=getenv("HPCE_DIRECT_INNER_K");
    if(v==NULL){
        printf("HPCE_DIRECT_INNER_K is not set.\n");
    }else{
        printf("HPCE_DIRECT_INNER_K = %s\n", v);
    }
    return 0;
}