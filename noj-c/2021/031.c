#include <stdio.h>
#include <stdlib.h>

int main() {
    int AH, AM, AS, BH, BM, BS;
    scanf("%d%d%d%d%d%d", &AH, &AM, &AS, &BH, &BM, &BS);
    AH = AH + BH;
    AM = AM + BM;
    AS = AS + BS;
    if (AS >= 60) {
        AM = AM + 1;
        AS = AS - 60;
    }
    if (AM >= 60) {
        AH = AH + 1;
        AM = AM - 60;
    }
    printf("%d %d %d", AH, AM, AS);
    return 0;
}
