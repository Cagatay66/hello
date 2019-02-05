#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main(int argc, const char * argv[]) {
    int input, count = 1, prime = 2;
    printf("Hesaplanmasini istediginiz asal sayı: ");
    scanf("%d",&input);
    while(count<input) {
        prime++;
        int i = 2;
        while(i <= sqrt(prime) && prime % i != 0) {
            i++;
        }
        if(prime % i != 0) {
            count++;
        }
    }
    printf("%d\n",prime);
    return 0;
}
