#include <stdio.h>
#include <math.h>

int main(int argc, const char * argv[]) {
    int input, curPower;
    printf("Sayı giriniz: ");
    scanf("%d",&input);
    if(input==0) {
        printf("0\n");
        return 0;
    }
    curPower = pow(2,floor(log2(input)));
    while(curPower!=0) {
        if(input>=curPower) {
            printf("1");
            input-=curPower;
        }
        else {
            printf("0");
        }
        curPower/=2;
    }
    printf("\n");
    return 0;
}
