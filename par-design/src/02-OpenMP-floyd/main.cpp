#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>
#include <omp.h>
#include <random>

#ifndef NR_THREADS
#define NR_THREADS 4
#endif

#ifndef ARG_MULTIPLIER_N
#define ARG_MULTIPLIER_N 1
#endif

#ifndef ARG_DENSITY
#define ARG_DENSITY 0.1
#endif

#define MAX_WEIGHT 100

#define DATOUT_FILE "floyd-result.dat"
#define DATSTD_FILE "floyd-expect.dat"

#define INF std::numeric_limits<int>::max()
#define N ((NR_THREADS) * (ARG_MULTIPLIER_N))

using graph_t = std::array<std::array<int, N>, N>;
using hres_clock = std::chrono::high_resolution_clock;

double random() {
  static std::mt19937 rng(std::random_device{}());
  static std::uniform_real_distribution<double> dist(0.0, 1.0);
  return dist(rng);
}

int random_weight() { return std::max<int>(1, random() * MAX_WEIGHT); }

void ser_floyd_impl(graph_t &m, int timeout_ms) {
  const auto beg = hres_clock::now();
  for (int k = 0; k < N; ++k) {
    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j) {
        if ((m[k][j] != INF && m[i][k] != INF) &&
            m[i][j] > (m[i][k] + m[k][j])) {
          m[i][j] = m[i][k] + m[k][j];
        }
      }
#ifdef ENABLE_TIMEOUT
      {
        const auto end = hres_clock::now();
        const auto elapsed =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - beg);
        if (elapsed.count() > timeout_ms) {
          throw std::runtime_error("timeout");
        }
      }
#endif
    }
  }
}

bool ser_floyd(graph_t &m, int timeout_ms) {
  bool timeout = false;
  try {
    ser_floyd_impl(m, timeout_ms);
  } catch (...) {
    timeout = true;
  }
  return !timeout;
}

void init_graph(graph_t &m, double density) {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      if (i == j) {
        m[i][j] = 0;
      } else {
        m[i][j] = random() < density ? random_weight() : INF;
      }
    }
  }
}

void read_and_distribute_graph(graph_t &m) {
  //! already inited, nothing to do here
}

void dump_rows(std::ofstream &of, const graph_t &m, int beg_row, int end_row) {
  for (int i = beg_row; i < end_row; ++i) {
    for (int j = 0; j < N; ++j) {
      if (m[i][j] == INF) {
        of << "INF ";
      } else {
        of << m[i][j] << " ";
      }
    }
    of << "\n";
  }
}

int main(int argc, char *argv[]) {
  auto mat = new graph_t;
  { std::ofstream of(DATOUT_FILE, std::ios::trunc); }

#ifndef NOCPU
  {
    init_graph(*mat, ARG_DENSITY);
    bool timeout = false;
    const auto beg = hres_clock::now();
    {
      auto cloned = std::make_unique<graph_t>(*mat);
      timeout = !ser_floyd(*cloned, 2000);
#ifndef NOIO
      std::ofstream of(DATSTD_FILE, std::ios::trunc);
      dump_rows(of, *cloned, 0, N);
#endif
    }
    const auto end = hres_clock::now();
    std::cout << "serial floyd time, io included: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end -
                                                                       beg)
                     .count()
              << " us" << (timeout ? " (timeout)" : "") << std::endl;
  }
#endif

  const auto beg = hres_clock::now();
  {
#pragma omp parallel num_threads(NR_THREADS)
    {
      const int tid = omp_get_thread_num();
      if (tid == NR_THREADS - 1) {
        read_and_distribute_graph(*mat);
      }
#pragma omp barrier

      const int beg_row = tid * N / NR_THREADS;
      const int end_row = (tid + 1) * N / NR_THREADS;

      for (int k = 0; k < N; ++k) {
#pragma omp barrier
        for (int i = beg_row; i < end_row; i++) {
          for (int j = 0; j < N; ++j) {
            if ((*mat)[i][k] != INF && (*mat)[k][j] != INF &&
                (*mat)[i][j] > (*mat)[i][k] + (*mat)[k][j]) {
              (*mat)[i][j] = (*mat)[i][k] + (*mat)[k][j];
            }
          }
        }
      }
#ifndef NOIO
      for (int id = 0; id < NR_THREADS; ++id) {
        if (tid == id) {
          std::ofstream of(DATOUT_FILE, std::ios::app);
          dump_rows(of, *mat, beg_row, end_row);
        }
#pragma omp barrier
#endif
      }
    }
  }
  const auto end = hres_clock::now();
  std::cout << "OpenMP floyd time, io included: "
            << std::chrono::duration_cast<std::chrono::microseconds>(end - beg)
                   .count()
            << " us" << std::endl;

  delete mat;

  return 0;
}
