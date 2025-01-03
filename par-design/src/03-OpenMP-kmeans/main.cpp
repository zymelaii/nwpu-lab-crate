#include <assert.h>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <map>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#ifndef NR_THREADS
#define NR_THREADS 4
#endif

#ifndef ARG_NR_CLUS
#define ARG_NR_CLUS 3
#endif

#ifndef ARG_MAX_ITERS
#define ARG_MAX_ITERS 1024
#endif

#define IRIS_DATA_FILE "iris.data"

typedef struct irisent {
  double sepal_len;
  double sepal_wid;
  double petal_len;
  double petal_wid;
  std::string type;
} irisent_t;

typedef struct clus {
  irisent_t centroid;
  std::vector<int> clus_ids;
} clus_t;

static std::vector<irisent_t> data;

double calc_iris_dist(const irisent_t &lhs, const irisent_t &rhs) {
  return sqrt(pow(lhs.sepal_len - rhs.sepal_len, 2) +
              pow(lhs.sepal_wid - rhs.sepal_wid, 2) +
              pow(lhs.petal_len - rhs.petal_len, 2) +
              pow(lhs.petal_wid - rhs.petal_wid, 2));
}

int find_nearest_clus(int entid, std::vector<clus_t> &clusters) {
  assert(!clusters.empty());
  int clus_id = 0;
  double dist = calc_iris_dist(data[entid], clusters[0].centroid);
  for (int i = 1; i < clusters.size(); ++i) {
    const double new_dist = calc_iris_dist(data[entid], clusters[i].centroid);
    if (new_dist < dist) {
      clus_id = i;
      dist = new_dist;
    }
  }
  return clus_id;
}

void update_centroid(clus_t &cluster) {
  double sum_sepal_len = 0.0;
  double sum_sepal_wid = 0.0;
  double sum_petal_len = 0.0;
  double sum_petal_wid = 0.0;
  for (const int i : cluster.clus_ids) {
    sum_sepal_len += data[i].sepal_len;
    sum_sepal_wid += data[i].sepal_wid;
    sum_petal_len += data[i].petal_len;
    sum_petal_wid += data[i].petal_wid;
  }
  cluster.centroid.sepal_len = sum_sepal_len / cluster.clus_ids.size();
  cluster.centroid.sepal_wid = sum_sepal_wid / cluster.clus_ids.size();
  cluster.centroid.petal_len = sum_petal_len / cluster.clus_ids.size();
  cluster.centroid.petal_wid = sum_petal_wid / cluster.clus_ids.size();
  //! class of centroid inherits the original iris data point
}

void kmeans(int iters, std::vector<clus_t> &clusters) {
#pragma omp parallel num_threads(NR_THREADS)
  {
    for (int iter = 0; iter < iters; ++iter) {
#pragma omp for
      for (int i = 0; i < data.size(); ++i) {
        const int nearest_clus = find_nearest_clus(i, clusters);
#pragma omp critical
        { clusters[nearest_clus].clus_ids.push_back(i); }
      }
#pragma omp for
      for (int i = 0; i < clusters.size(); ++i) {
        if (!clusters[i].clus_ids.empty()) {
          update_centroid(clusters[i]);
        }
      }
#pragma omp for
      for (int i = 0; i < clusters.size(); ++i) {
        clusters[i].clus_ids.clear();
      }
    }
#pragma omp barrier
  }
}

void ser_kmeans(int iters, std::vector<clus_t> &clusters) {
  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < data.size(); ++i) {
      const int nearest_clus = find_nearest_clus(i, clusters);
      { clusters[nearest_clus].clus_ids.push_back(i); }
    }
    for (int i = 0; i < clusters.size(); ++i) {
      if (!clusters[i].clus_ids.empty()) {
        update_centroid(clusters[i]);
      }
    }
    for (int i = 0; i < clusters.size(); ++i) {
      clusters[i].clus_ids.clear();
    }
  }
}

void predict_clus_class(clus_t &clus) {
  std::map<std::string, int> class_cnt;
  for (const int i : clus.clus_ids) {
    if (class_cnt.count(data[i].type)) {
      ++class_cnt[data[i].type];
    } else {
      class_cnt[data[i].type] = 1;
    }
  }
  clus.centroid.type.clear();
  int max_cnt = 0;
  for (const auto &[class_name, count] : class_cnt) {
    if (count > max_cnt) {
      clus.centroid.type = class_name;
      max_cnt = count;
    }
  }
}

void predict(std::vector<clus_t> &clusters) {
#pragma omp parallel num_threads(NR_THREADS)
  {
#pragma omp for
    for (int i = 0; i < data.size(); ++i) {
      const int nearest_clus = find_nearest_clus(i, clusters);
#pragma omp critical
      { clusters[nearest_clus].clus_ids.push_back(i); }
    }
#pragma omp for
    for (int i = 0; i < clusters.size(); ++i) {
      predict_clus_class(clusters[i]);
    }
  }
}

void init_iris() {
  FILE *fp = fopen(IRIS_DATA_FILE, "r");
  if (!fp) {
    std::cerr << "failed to open iris data file" << std::endl;
    exit(-1);
  }
  char buf[64] = {};
  while (!feof(fp)) {
    irisent_t ent;
    fscanf(fp, "%lf,%lf,%lf,%lf,%s", &ent.sepal_len, &ent.sepal_wid,
           &ent.petal_len, &ent.petal_wid, buf);
    ent.type = std::string(buf);
    data.push_back(ent);
  }
  fclose(fp);
}

int main(int argc, char *argv[]) {
  using clock = std::chrono::high_resolution_clock;

  init_iris();

  std::vector<clus_t> clusters(ARG_NR_CLUS);
  for (int i = 0; i < clusters.size(); ++i) {
    clusters[i].centroid = data[i % data.size()];
  }

  const auto beg = clock::now();
  {
#ifndef NOMT
    kmeans(ARG_MAX_ITERS, clusters);
#else
    ser_kmeans(ARG_MAX_ITERS, clusters);
#endif
  }
  const auto end = clock::now();

  predict(clusters);

  size_t errors = 0;
  for (const auto &c : clusters) {
    std::cout << "cluster \"" << c.centroid.type << "\" has "
              << c.clus_ids.size() << " data points" << std::endl;
    for (const int i : c.clus_ids) {
      if (data[i].type != c.centroid.type) {
        ++errors;
      }
    }
  }
  std::cout << "purity: " << std::fixed << std::setprecision(2)
            << (data.size() - errors) * 1.0 / data.size() * 100.0 << "%, "
            << "cost time: "
            << std::chrono::duration_cast<std::chrono::microseconds>(end - beg)
                   .count()
            << " us" << std::endl;

  return 0;
}
