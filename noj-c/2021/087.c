#include <stdio.h>
#include <math.h>

int main() {
    int a[9], n, b[3], s;
    for (a[0] = 1; a[0] < 4; a[0]++)
        for (a[1] = 1; a[1] < 10; a[1]++)
            for (a[2] = 2; a[2] < 10; a[2]++)
                for (a[3] = 3; a[3] < 7; a[3]++)
                    for (a[4] = 3; a[4] < 9; a[4]++)
                        for (a[5] = 4; a[5] < 9; a[5]++)
                            for (a[6] = 5; a[6] < 10; a[6]++)
                                for (a[7] = 1; a[7] < 9; a[7]++)
                                    for (a[8] = 1; a[8] < 10; a[8]++) {
                                        s = 0;
                                        for (n = 1; n < 10; n++)
                                            if (a[0] == a[n]) {
                                                s = s + 1;
                                                break;
                                            };
                                        for (n = 2; n < 10; n++) {
                                            if (s > 0) break;
                                            if (a[1] == a[n]) {
                                                s = s + 1;
                                                break;
                                            };
                                        };
                                        for (n = 3; n < 10; n++) {
                                            if (s > 0) break;
                                            if (a[2] == a[n]) {
                                                s = s + 1;
                                                break;
                                            };
                                        };
                                        for (n = 4; n < 10; n++) {
                                            if (s > 0) break;
                                            if (a[3] == a[n]) {
                                                s = s + 1;
                                                break;
                                            };
                                        };
                                        for (n = 5; n < 10; n++) {
                                            if (s > 0) break;
                                            if (a[4] == a[n]) {
                                                s = s + 1;
                                                break;
                                            };
                                        };
                                        for (n = 6; n < 10; n++) {
                                            if (s > 0) break;
                                            if (a[5] == a[n]) {
                                                s = s + 1;
                                                break;
                                            };
                                        };
                                        for (n = 7; n < 10; n++) {
                                            if (s > 0) break;
                                            if (a[6] == a[n]) {
                                                s = s + 1;
                                                break;
                                            };
                                        };
                                        for (n = 8; n < 10; n++) {
                                            if (s > 0) break;
                                            if (a[7] == a[n]) {
                                                s = s + 1;
                                                break;
                                            };
                                        };
                                        if (s == 0) {
                                            b[0] =
                                                100 * a[0] + 10 * a[1] + a[2];
                                            b[1] =
                                                100 * a[3] + 10 * a[4] + a[5];
                                            b[2] =
                                                100 * a[6] + 10 * a[7] + a[8];
                                            if ((b[2] == 3 * b[0])
                                                && (b[1] == 2 * b[0]))
                                                printf(
                                                    "%d %d %d\n",
                                                    b[0],
                                                    b[1],
                                                    b[2]);
                                        };
                                    };
    return 0;
}
