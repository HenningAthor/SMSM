#ifndef SMSM_DATA_POINTS_H
#define SMSM_DATA_POINTS_H

#include <cstddef>
#include <filesystem>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include "../utility/util.h"
#include "StructureInterface.h"

/**
 * Class to hold data for kMeans.
 */
template<typename TypeSF>
class DataPoints : public StructureInterface<TypeSF> {
public:
    size_t n_data_points = 0;
    size_t dimensionality = 0;
    std::vector<std::vector<TypeSF>> data_points;

    DataPoints() = default;

    DataPoints(size_t n, size_t d) {
        n_data_points = n;
        dimensionality = d;
    }

    explicit DataPoints(const std::string &file_path) {
        if (!file_exists(file_path)) {
            std::cout << "File " << file_path << " was not found!\n";
            std::cout << "Current working directory is " << std::filesystem::current_path() << "!" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::ifstream file(file_path);
        std::string line;

        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '%') {
                continue;
            }

            std::vector<std::string> str_entries = split(line, ' ');
            std::vector<double> entries;
            for (auto &s: str_entries) {
                entries.push_back(std::stod(s));
            }
            add_entry(entries);
        }
        file.close();
        determine_n_and_d();
    }

    void add_entry(const std::vector<TypeSF> &vec) {
        data_points.push_back(vec);
    }

    void determine_n_and_d() {
        n_data_points = data_points.size();
        dimensionality = data_points[0].size();
    }

    [[nodiscard]] size_t get_n() const {
        return n_data_points;
    }

    [[nodiscard]]std::string get_as_string() const {
        std::stringstream output;
        output << "%" << n_data_points << " " << dimensionality << "\n";

        for (size_t i = 0; i < n_data_points; ++i) {
            for (size_t d = 0; d < dimensionality; ++d) {
                output << data_points[i][d] << " ";
            }
            output << "\n";
        }
        return output.str();
    }

    /**
     * Writes the datapoints to a file.
     *
     * @param file_path The path to the file.
     */
    inline void write_to_file(std::string &file_path) const {
        std::stringstream output;
        output << get_as_string() << "\n";
        std::ofstream file;
        file.open(file_path);
        file << output.rdbuf();
        file.close();
        std::cout << "write to " << file_path << std::endl;
    };

    inline void finalize() override {
        for (size_t i = 0; i < n_data_points; ++i) {
            auto &vec = data_points[i];
            if (vec.size() != dimensionality) {
                std::cout << "Datapoint " << i << " does not have dimensionality " << dimensionality << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    };

    inline TypeSF evaluate_empty_set() override { return 0.0; };

    inline TypeSF evaluate_1D([[maybe_unused]] const std::vector<uint32_t> &s, [[maybe_unused]] const size_t s_size) override { return 0.0; };

    inline TypeSF evaluate_2D([[maybe_unused]] const std::vector<uint32_t> &s, [[maybe_unused]] const size_t s_size) override { return 0.0; };

    /**
     * Evaluates the score function.
     *
     * @param s The set S.
     * @param s_size The size of set S.
     */
    inline TypeSF evaluate_XD([[maybe_unused]] const std::vector<uint32_t> &s, [[maybe_unused]] size_t s_size) override { return 0.0; };

    /**
     * Evaluates the score function.
     *
     * @param s The set S.
     * @param s_size The size of set S.
     */
    inline TypeSF evaluate_general([[maybe_unused]] const std::vector<uint32_t> &s, [[maybe_unused]] size_t s_size) override { return 0.0; };

    /**
     * Will initialize helping structures, that can help with score function
     * evaluation.
     *
     * @param max_depth Maximum depth the searching algorithm will explore.
     */
    inline void initialize_helping_structures([[maybe_unused]] size_t max_depth) override {};

    /**
     * Signals to the structure, that a new depth will be explored.
     *
     * @param s The set S, that is present at the new depth.
     * @param s_size Size of the set S.
     */
    inline void visit_new_depth([[maybe_unused]] const std::vector<uint32_t> &s, [[maybe_unused]] size_t s_size) override {};

    /**
     * Signals to the structure, that we returned from the last depth.
     */

    inline void return_from_last_depth() override {};
};

#endif //SMSM_DATA_POINTS_H
