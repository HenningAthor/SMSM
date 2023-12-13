#include "data_point_generation.h"

DataPoints<double> random_data_points(size_t n, size_t d) {
    DataPoints<double> data_points(n, d);

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<> dist(0.0, 1.0);

    dist(rng);

    std::vector<double> point(d, 0.0);

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < d; ++j) {
            point[j] = dist(rng);
        }
        data_points.add_entry(point);
    }
    data_points.determine_n_and_d();

    data_points.finalize();
    return data_points;
}

DataPointsEuclidianDistance<double> random_DataPointsEuclidianDistance(size_t n, size_t d) {
    DataPointsEuclidianDistance<double> data_points(n, d);

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<> dist(0.0, 1.0);

    dist(rng);

    std::vector<double> point(d, 0.0);

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < d; ++j) {
            point[j] = dist(rng);
        }
        data_points.add_entry(point);
    }
    data_points.determine_n_and_d();

    data_points.finalize();
    return data_points;
}

std::vector<DataPointsEuclidianDistance<double>> random_DataPointsEuclidianDistance_dataset(size_t n, size_t d, size_t n_sets) {
    std::vector<DataPointsEuclidianDistance<double>> vec;

    for (uint32_t i = 0; i < n_sets; ++i) {
        vec.emplace_back(random_DataPointsEuclidianDistance(n, d));
    }

    return vec;
}
