#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int randomWithLimits (int min, int max) {
    srand((int)time(NULL));
    return (int) min+1+rand()%(max-min-1);
}

int main(int argc, const char * argv[]) {
    int min = 0, max = 1000, turn = 1;
    while(max - 1 != min) {
        int newValue = randomWithLimits(min, max);
        if(turn % 2 == 1) {
            printf("%d. el: %d-%d arasinda %d sayisi bulundu. Yeni ust sinir = %d\n",turn,min,max,newValue,newValue);
            max=newValue;
        }
        else {
            printf("%d. el: %d-%d arasinda %d sayisi bulundu. Yeni alt sinir = %d\n",turn,min,max,newValue,newValue);
            min=newValue;
        }
        turn++;
    }
    printf("---------------------------------------------------------------------------------\nSonuc olarak​ %d,​%d ardisik sayilari bulunmustur.\nProgram basariyla tamamlanmistir.\n",min,max);
    return 0;
}
