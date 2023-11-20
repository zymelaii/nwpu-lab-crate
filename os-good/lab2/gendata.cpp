#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <random>

#define BOUNDARY_SIZE 100000000

void GenRandData4SmallScale(int count, FILE *fp) {
    fprintf(fp, "%d\n", count);
    std::default_random_engine rng{};

    std::set<int> data{};
    for (int i = 0; i < count; i++) {
        while (true) {
            auto num = rng();
            if (data.count(num) == 0) {
                data.insert(num);
                fprintf(fp, "%-10d ", num);
                break;
            }
        }
        if ((i + 1) % 100 == 0) { fprintf(fp, "\n"); }
    }
}

void GenRandData4LargeScale(int count, FILE *fp) {
    fprintf(fp, "%d\n", count);
    std::default_random_engine rng{};

    const auto       shuffle_times = count / BOUNDARY_SIZE;
    std::vector<int> shuffle_array(shuffle_times);
    for (int i = 0; i < shuffle_array.size(); ++i) { shuffle_array[i] = i; }
    std::shuffle(shuffle_array.begin(), shuffle_array.end(), rng);

    for (int i = 0; i < shuffle_array.size(); ++i) {
        int              start = shuffle_array[i] * BOUNDARY_SIZE;
        std::vector<int> temp_array(BOUNDARY_SIZE);
        for (int i = 0; i < temp_array.size(); ++i) {
            temp_array[i] = start + i;
        };
        std::shuffle(temp_array.begin(), temp_array.end(), rng);
        for (int i = 0; i < temp_array.size(); i++) {
            if ((fprintf(fp, "%-10d ", temp_array[i]) < 0)) {
                printf("Error : Please Check whether you have enough disk "
                       "space!!\n");
                temp_array.clear();
                temp_array.shrink_to_fit();
                return;
            }
            if ((i + 1) % 100 == 0) { fprintf(fp, "\n"); }
        }
        temp_array.clear();
        temp_array.shrink_to_fit();
    }
}

int main(int argc, char *argv[]) {
    int count;

    if (argv[1] == NULL) {
        printf("Please enter the num\n");
        return -1;
    }

    if (argv[2] == NULL) {
        printf("Please enter the filename\n");
        return -1;
    }

    count    = atoi(argv[1]);
    FILE *fp = fopen(argv[2], "w");
    do {
        if (count <= 0) {
            printf("Unvalidated Input NUM\n");
            break;
        }
        if (count < BOUNDARY_SIZE) {
            GenRandData4SmallScale(count, fp);
        } else if (count % BOUNDARY_SIZE != 0) {
            printf("NUM must be a multiple of %d\n", BOUNDARY_SIZE);
        } else {
            GenRandData4LargeScale(count, fp);
        }
    } while (0);

    fclose(fp);
    return 0;
}
