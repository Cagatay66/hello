#include <stdio.h>
#include <string.h>

int main(int argc, const char * argv[]) {
    char input[100],processed[100],count=0,single=1;
    printf("String giriniz: ");
    scanf("%s",input);
    for(int i=0;i<strlen(input);i++) {
        char *firstOccur=strchr(input, input[i]);
        if(firstOccur==&input[i]) {
            processed[count]=input[i];
            count++;
        }
        else
            single=0;
    }
    if(single)
        printf("\"%s\" -> hepsi tekil.\n",processed);
    else
        printf("\"%s\" -> hepsi tekil degil. Tekil hali: \"%s\"\n",input,processed);
    return 0;
}
