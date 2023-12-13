#ifndef SMSM_DATA_POINT_GENERATION_H
#define SMSM_DATA_POINT_GENERATION_H

#include <cstddef>
#include <random>

#include "util.h"
#include "../structures/data_points.h"
#include "../structures/dataPoints_EuclidianDistance.h"

/**
 * Creates n datapoints of dimensionality d. Each entry is uniformly distributed
 * in the interval [0, 1).
 *
 * @param n Number of data points.
 * @param d Dimensionality.
 * @return The data points.
 */
DataPoints<double> random_data_points(size_t n, size_t d);

DataPointsEuclidianDistance<double> random_DataPointsEuclidianDistance(size_t n, size_t d);

std::vector<DataPointsEuclidianDistance<double>> random_DataPointsEuclidianDistance_dataset(size_t n, size_t d, size_t n_sets);

#endif //SMSM_DATA_POINT_GENERATION_H
