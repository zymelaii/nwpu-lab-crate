#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <functional>
#include <assert.h>

using ivec_t = std::vector<int>;

//! atomics
std::atomic_flag lock{false};
std::atomic_flag done{false};
std::atomic_int  completed{0};

//! options
std::string file{};
int         total_threads = 1;

auto timeit(std::function<void()> fn) {
    using clock = std::chrono::system_clock;
    using std::chrono::duration_cast;
    using std::chrono::microseconds;
    auto t1 = clock::now();
    fn();
    auto t2 = clock::now();
    return duration_cast<microseconds>(t2 - t1).count();
}

ivec_t readall(const std::string &file, int *size) {
    std::ifstream ifs(file);
    assert(ifs.is_open());

    int total = -1;
    ifs >> total;
    assert(total >= 0);
    if (size != nullptr) { *size = total; }

    ivec_t v(total);
    for (auto &e : v) {
        ifs >> e;
        assert(!ifs.eof());
    }

    return v;
}

void writeall(const std::string &file, ivec_t &v) {
    std::ofstream ofs(file);
    assert(ofs.is_open());
    ofs << v.size() << "\n";
    for (auto &e : v) { ofs << e << " "; }
}

void job(int *v, int len) {
    std::sort(v, v + len);
    ++completed;
    if (completed == total_threads) {
        while (done.test_and_set()) {}
        done.notify_one();
    }
}

int main(int argc, char *argv[]) {
    //! parse options
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-n") == 0) {
            if (++i >= argc) { exit(-1); }
            total_threads = atoi(argv[i]);
        } else if (file.empty()) {
            file = argv[i];
        }
    }

    assert(!file.empty());
    assert(total_threads > 0);

    //! load data
    int    total = -1;
    ivec_t vec   = readall(file, &total);
    assert(total >= 0);
    assert(total == vec.size());

    //! assign task
    int    offset = 0;
    ivec_t index(total_threads + 1);
    index[0] = 0;
    for (int i = 0; i < total_threads; ++i) {
        int n = total * 1.0 / total_threads * (i + 1) - offset;
        std::thread(job, vec.data() + offset, n).detach();
        offset       += n;
        index[i + 1] = offset;
    }

    auto used = timeit([&] {
        //! write back
        done.wait(false);
        std::sort(vec.begin(), vec.end());
    });
    std::cout << used << "us";

    writeall("out_" + file, vec);

    return 0;
}
