#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct student {
    int  id;
    char name[12];
    int  sco;
};

int main() {
    int            n;
    struct student stu, max;
    FILE          *fp;
    scanf("%d", &n);
    fp = fopen("DATA5614.DB", "wb+");
    if (!fp) printf("No!!!");
    while (n--) {
        scanf("%d%s%d", &stu.id, stu.name, &stu.sco);
        fwrite(&stu, sizeof(struct student), 1, fp);
    }
    rewind(fp);
    max.sco = 0;
    while (!feof(fp)) {
        fread(&stu, sizeof(struct student), 1, fp);
        if (stu.sco > max.sco) {
            max.id  = stu.id;
            max.sco = stu.sco;
            strcpy(max.name, stu.name);
        }
    }
    fclose(fp);
    printf("%d %s %d", max.id, max.name, max.sco);
}
