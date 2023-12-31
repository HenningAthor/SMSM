#ifndef SMSM_FACILITY_LOCATION_H
#define SMSM_FACILITY_LOCATION_H

#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <iterator>
#include <optional>
#include <vector>
#include <fstream>
#include <cstring>

#include "../utility/VectorOfVectors.h"
#include "../utility/util.h"
#include "StructureInterface.h"

/**
 * Class for defining a graph.
 * */
template<typename TypeSF>
class FacilityLocation : public StructureInterface<TypeSF> {
public:
    // normal variables to model the graph
    size_t n_facilities = 0;
    size_t n_customers = 0;
    std::vector<std::vector<double>> benefits;

    // structures to speed up score function evaluation
    size_t depth = 0;
    std::vector<std::vector<TypeSF>> min_benefits;
    std::vector<TypeSF> temp_min;

public:
    /**
     * Initializes a graph.
     *
     * @return A graph object.
     * */
    FacilityLocation() = default;

    /**
    * Reads a graph from the specified file. File should have the following format:
    * idx1 idx2\n
    * idx3 idx4\n
    * ...
    *
    * Vertices have values from 0 to n-1.
    * Lines with '%' are ignored.
    *
    * @param file_path Path to the file.
    * @return The Graph.
    */
    explicit FacilityLocation(const std::string &file_path) {
        if (!file_exists(file_path)) {
            std::cout << "File " << file_path << " was not found!\n";
            std::cout << "Current working directory is " << std::filesystem::current_path() << "!" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::ifstream file_in(file_path);

        std::string line;
        while (std::getline(file_in, line)) {
            std::istringstream ss(line);
            benefits.emplace_back(std::istream_iterator<double>(ss), std::istream_iterator<double>());
        }
        n_facilities = benefits.size();
        n_customers = benefits[0].size();

        for (size_t i = 0; i < n_facilities; ++i) {
            for (size_t j = 0; j < n_customers; ++j) {
                benefits[i][j] *= -1.0;
            }
        }

    };

    /**
     * Returns the number of nodes.
     *
     * @return Number of nodes.
     */
    inline size_t get_n() const {
        return n_facilities;
    };

    inline TypeSF evaluate_empty_set() override {
        return 0.0;
    };

    inline TypeSF evaluate_1D(const std::vector<uint32_t> &s, const size_t s_size) override {
        TypeSF score = sum_of_min(min_benefits[depth], benefits[s[s_size - 1]]);
        ASSERT(evaluate_general(s, s_size) == -score);
        return -score;
    };

    inline TypeSF evaluate_2D(const std::vector<uint32_t> &s, const size_t s_size) override {
        TypeSF score = sum_of_min(min_benefits[depth], benefits[s[s_size - 2]], benefits[s[s_size - 1]]);
        ASSERT(evaluate_general(s, s_size) == -score);
        return -score;
    };

    inline TypeSF evaluate_XD(const std::vector<uint32_t> &s, size_t s_size) override {
        size_t n_new_elements = s_size - depth;

        min(temp_min, min_benefits[depth], benefits[s[depth]]);
        for (size_t j = 1; j < n_new_elements - 1; ++j) {
            min_in_place(temp_min, benefits[s[depth + j]]);
        }
        TypeSF score = sum_of_min(temp_min, benefits[s[depth + n_new_elements - 1]]);
        ASSERT(evaluate_general(s, s_size) == -score);
        return -score;
    };

    inline TypeSF evaluate_general(const std::vector<uint32_t> &s, size_t s_size) override {
        if (s_size == 0) {
            return evaluate_empty_set();
        } else if (s_size == 1) {
            return -sum(benefits[s[0]]);
        } else if (s_size == 2) {
            return -sum_of_min(benefits[s[0]], benefits[s[1]]);
        }

        min(temp_min, benefits[s[0]], benefits[s[1]]);
        for (size_t j = 2; j < s_size - 1; ++j) {
            min_in_place(temp_min, benefits[s[j]]);
        }
        TypeSF score = sum_of_min(temp_min, benefits[s[s_size - 1]]);
        return -score;
    };

    inline void finalize() override {
        temp_min.resize(n_customers);
    };

    inline void initialize_helping_structures(size_t k) override {
        min_benefits.clear();
        min_benefits.resize((k + 1), std::vector<double>(n_customers, std::numeric_limits<TypeSF>::max()));

        depth = 0;
    };

    inline void visit_new_depth(const std::vector<uint32_t> &s, size_t s_size) override {
        depth += 1;

        min(min_benefits[depth], min_benefits[depth - 1], benefits[s[s_size - 1]]);

    };

    inline void return_from_last_depth() override {
        depth -= 1;
    };
};

#endif //SMSM_FACILITY_LOCATION_H
