#ifndef SMSM_PBFALGORITHM_H
#define SMSM_PBFALGORITHM_H

#include <cstddef>
#include <vector>
#include <limits>

#include "../utility/util.h"

template<typename TypeSF>
class PBFAlgorithm {
public:
    size_t n_blocks;
    size_t block_size;
    size_t max_k;

    size_t algorithm_type;

    bool need_candidates;

    // holds best set and best score improvement for each set
    std::vector<std::vector<std::vector<size_t>>> set_s;
    std::vector<std::vector<TypeSF>> si_s;

    // needed for brute force
    std::vector<size_t> counter;
    std::vector<size_t> best_counter;
    std::vector<size_t> temp;

    // needed for dynamic approach
    bool dynamic_is_initialized = false;
    std::vector<std::vector<TypeSF>> dynamic_arr;
    std::vector<std::vector<size_t>> dynamic_arr_indices;

    /**
     * The constructor.
     *
     * @param n_b Number of blocks.
     * @param l_b Size of each block.
     */
    PBFAlgorithm(size_t n_b, size_t l_b, size_t t_k, size_t algo_type, bool t_need_candidates) {
        n_blocks = n_b;
        block_size = l_b;
        max_k = t_k;
        algorithm_type = algo_type;
        need_candidates = t_need_candidates;

        initialize(n_b, l_b, t_k, algo_type, t_need_candidates);
    }

    /**
     * Sets the number of blocks ans the block size. Note that this function
     * does not alter any memory and therefore values that are larger than the
     * ones used to construct the object will lead to errors.
     *
     * @param n_b Number of blocks.
     * @param l_b Size of each block.
     */
    void initialize(size_t n_b_new, size_t l_b_new, size_t max_k_new, size_t algo_type_new, bool t_need_candidates) {
        n_blocks = n_b_new;
        block_size = l_b_new;
        max_k = max_k_new;
        algorithm_type = algo_type_new;
        need_candidates = t_need_candidates;

        for (size_t i = 0; i < n_blocks; ++i) {
            if (si_s.size() <= i) {
                si_s.emplace_back();
                set_s.emplace_back();
            }

            for (size_t j = 0; j < block_size + 1; ++j) {
                if (si_s[i].size() <= j) {
                    si_s[i].push_back(0);
                    set_s[i].emplace_back(j);
                } else {
                    si_s[i][j] = 0;
                    set_s[i][j].resize(j);
                }
            }
        }

        if (algorithm_type == 1) {
            counter.resize(n_blocks);
            best_counter.resize(n_blocks);
        }

        if (algorithm_type == 2) {
            dynamic_is_initialized = false;

            for (size_t i = 0; i < n_blocks; ++i) {
                if (dynamic_arr.size() <= i) {
                    dynamic_arr.emplace_back();
                    dynamic_arr_indices.emplace_back();
                }

                size_t dynamic_table_size = std::min((i + 1) * block_size, max_k) + 1;
                for (size_t j = 0; j < dynamic_table_size; ++j) {
                    if (dynamic_arr[i].size() <= j) {
                        dynamic_arr[i].push_back(0);
                        dynamic_arr_indices[i].push_back(0);
                    } else {
                        dynamic_arr[i][j] = 0;
                        dynamic_arr_indices[i][j] = 0;
                    }
                }
            }
        }
    }

    /**
     * Sets the score improvements back to their default value.
     */
    void reinitialize(size_t n_b, size_t l_b, size_t t_k, size_t algo_type, bool t_need_candidates) {
        initialize(n_b, l_b, t_k, algo_type, t_need_candidates);
    }

    /**
     * Adds one set. Will update the best set if the score improvement is
     * greater, than the current best for this set size.
     *
     * @param b_id Id of the block.
     * @param set The set.
     * @param set_size The size of the set.
     * @param si The score improvement of the set.
     */
    void add_set(size_t b_id, std::vector<size_t> &set, size_t set_size, TypeSF si) {
        if (si >= si_s[b_id][set_size]) {
            si_s[b_id][set_size] = si;
            std::memcpy(set_s[b_id][set_size].data(), set.data(), set_size * sizeof(size_t));
        }
    }

    void add_set_1(size_t b_id, size_t e1, TypeSF si) {
        if (si >= si_s[b_id][1]) {
            si_s[b_id][1] = si;
            set_s[b_id][1][0] = e1;
        }
    }

    void add_set_2(size_t b_id, size_t e1, size_t e2, TypeSF si) {
        if (si >= si_s[b_id][2]) {
            si_s[b_id][2] = si;
            set_s[b_id][2][0] = e1;
            set_s[b_id][2][1] = e2;
        }
    }

    /**
     * Gets the solution that maximizes the sum for r elements. Will iterate
     * over all possible combinations.
     *
     * @param r Number of elements to choose.
     * @param res Vector holding the solution.
     * @param res_si The score improvement of each selected set.
     * @return The maximized sum.
     */
    TypeSF get_solution(size_t r,
                        VectorOfVectors<size_t> &res,
                        std::vector<TypeSF> &res_si) {
        if (r == 0) {
            return 0;
        }

        TypeSF bf_res, dynamic_res;
        if (algorithm_type == 1) {
            bf_res = get_bf_solution(r, res, res_si);
            return bf_res;
        }
        if (algorithm_type == 2) {
            dynamic_res = get_dynamic_solution(r, res, res_si);
            return dynamic_res;
        }

        std::cout << "Dont recognize algorithm type " << algorithm_type << " for PBF!" << std::endl;
        exit(EXIT_FAILURE);
    }

    TypeSF get_bf_solution(size_t r,
                           VectorOfVectors<size_t> &res,
                           std::vector<TypeSF> &res_si) {
        // initialize counter
        memset(counter.data(), 0, n_blocks * sizeof(counter[0]));
        TypeSF best_sum = -std::numeric_limits<TypeSF>::max();

        while (get_next_combination(counter, n_blocks, block_size + 1)) {
            if (sum(counter) != r) {
                // this combination has not the right amount of elements
                continue;
            }

            // check if the config has the best sum
            TypeSF s = 0.0;
            for (size_t i = 0; i < n_blocks; ++i) {
                s += si_s[i][counter[i]];
            }

            if (s > best_sum) {
                best_sum = s;
                std::memcpy(best_counter.data(), counter.data(), n_blocks * sizeof(counter[0]));
            }
        }

        if(need_candidates) {
            for (size_t i = 0; i < n_blocks; ++i) {
                if (best_counter[i] != 0) {
                    temp.resize(best_counter[i]);
                    for (size_t j = 0; j < best_counter[i]; ++j) {
                        temp[j] = set_s[i][best_counter[i]][j];
                    }
                    res.push_back(temp);
                    res_si.push_back(si_s[i][best_counter[i]]);
                }
            }
        }

        return best_sum;
    }

    /**
     * Gets the next possible combination.
     *
     * @param comb The vector holding the current combination.
     * @param comb_size Number of elements.
     * @param max_v Maximum value an entry can have.
     * @return True if the combination is valid.
     */
    bool get_next_combination(std::vector<size_t> &comb,
                                     size_t comb_size,
                                     size_t max_v) {
        comb[comb_size - 1] += 1;

        if (comb[comb_size - 1] != max_v) {
            return true;
        }

        for (size_t i = comb_size - 1; i > 0; --i) {
            if (comb[i] == max_v) {
                comb[i] = 0;
                comb[i - 1] += 1;
            } else {
                break;
            }
        }
        return comb[0] < max_v;
    }

    TypeSF get_dynamic_solution(size_t r,
                                VectorOfVectors<size_t> &res,
                                std::vector<TypeSF> &res_si) {
        if (!dynamic_is_initialized) {
            populate_dynamic_array();
        }

        if (need_candidates) {
            size_t remaining_set_size = r;
            size_t set_size = dynamic_arr_indices[n_blocks - 1][remaining_set_size];
            if (set_size > 0) {
                res.push_back(set_s[n_blocks - 1][set_size]);
                res_si.push_back(si_s[n_blocks - 1][set_size]);
            }

            for (int b_id = n_blocks - 2; b_id >= 0; --b_id) {
                remaining_set_size = dynamic_arr_indices[b_id][remaining_set_size - set_size];
                set_size = dynamic_arr_indices[b_id][remaining_set_size];
                if (set_size > 0) {
                    res.push_back(set_s[b_id][set_size]);
                    res_si.push_back(si_s[b_id][set_size]);
                }
            }
        }

        return dynamic_arr[n_blocks - 1][r];
    }


    void populate_dynamic_array() {
        // initialize first block
        for (size_t j = 0; j < dynamic_arr[0].size(); ++j) {
            dynamic_arr[0][j] = si_s[0][j];
            dynamic_arr_indices[0][j] = j;
        }

        // dynamically compute all other parts
        for (size_t b_id = 1; b_id < n_blocks; ++b_id) {
            for (size_t j = 0; j < dynamic_arr[b_id].size(); ++j) {
                for (size_t l = 0; l <= j; ++l) {

                    if (dynamic_arr[b_id - 1].size() > j - l && si_s[b_id].size() > l) {
                        // we can look into the previous dynamic table
                        if (dynamic_arr[b_id][j] <= si_s[b_id][l] + dynamic_arr[b_id - 1][j - l]) {
                            dynamic_arr[b_id][j] = si_s[b_id][l] + dynamic_arr[b_id - 1][j - l];
                            dynamic_arr_indices[b_id][j] = l;
                        }
                    } else {
                        // we can not look in the previous table, because it has too few elements
                        if (si_s[b_id].size() > l) {
                            // the block has enough elements, so we can check
                            if (dynamic_arr[b_id][j] <= si_s[b_id][l]) {
                                dynamic_arr[b_id][j] = si_s[b_id][l];
                                dynamic_arr_indices[b_id][j] = l;
                            }
                        }
                    }
                }
            }
        }
        dynamic_is_initialized = true;
    }

    void print_block_info() {
        for (size_t i = 0; i < n_blocks; ++i) {
            std::cout << "Block " << i << std::endl;
            for (size_t j = 0; j < block_size + 1; ++j) {
                std::cout << "Set " << j << " : " << set_s[i][j] << " " << si_s[i][j] << std::endl;
            }
        }
    }

    void print_dynamic_info() {
        for (size_t i = 0; i < n_blocks; ++i) {
            std::cout << "Dynamic Block " << i << std::endl;
            for (size_t j = 0; j < dynamic_arr[i].size(); ++j) {
                std::cout << "Set " << j << " : " << dynamic_arr_indices[i][j] << " " << dynamic_arr[i][j] << std::endl;
            }
        }
    }
};

#endif //SMSM_PBFALGORITHM_H
