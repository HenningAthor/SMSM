#ifndef SMSM_BF_TREE_SEARCH_H
#define SMSM_BF_TREE_SEARCH_H

#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <limits>
#include <vector>
#include <cstring>

#include "../utility/util.h"

#define BF_ASSERT_ENABLED 0

template<typename T, typename TypeSF>
class BfTreeSearch {
public:

    T &t;
    size_t n;
    size_t k;
    size_t max_tree_depth;
    size_t sf_evaluated = 0;

    std::vector<uint32_t> candidates;
    std::vector<size_t> indices;

    std::vector<uint32_t> best_s;
    std::vector<uint32_t> s;
    size_t s_size;

    TypeSF best_score;

#if BF_ASSERT_ENABLED
    std::vector<std::vector<uint32_t>> all_tested_sets;
#endif

    BfTreeSearch(T &t,
                 size_t k,
                 const std::vector<uint32_t> &s_part = {},
                 const std::vector<uint32_t> &init_candidates = {}) : t(t), k(k) {
        n = t.get_n();

        best_s.resize(k);
        s.resize(k);

        for (size_t i = 0; i < s_part.size(); ++i) {
            best_s[i] = s_part[i];
            s[i] = s_part[i];
        }
        s_size = s_part.size();

        max_tree_depth = k - s_size;
        indices.resize(max_tree_depth);

        candidates.clear();
        candidates.reserve(n);
        if (init_candidates.empty()) {
            for (uint32_t c = 0; c < n; ++c) {
                if (!contains(s_part, c)) {
                    candidates.push_back(c);
                }
            }
        } else {
            for (auto c: init_candidates) {
                if (!contains(s_part, c)) {
                    candidates.push_back(c);
                }
            }
        }

        s_size = s_part.size();
        best_score = -std::numeric_limits<TypeSF>::max();

#if BF_ASSERT_ENABLED
        all_tested_sets.clear();
#endif
    }

    void reinitialize(T &t_new,
                      size_t k_new,
                      const std::vector<uint32_t> &s_part = {},
                      const std::vector<uint32_t> &init_candidates = {}) {
        t = t_new;
        n = t.get_n();
        k = k_new;

        best_s.resize(k);
        s.resize(k);

        for (size_t i = 0; i < s_part.size(); ++i) {
            best_s[i] = s_part[i];
            s[i] = s_part[i];
        }
        s_size = s_part.size();

        max_tree_depth = k - s_size;
        indices.resize(max_tree_depth);

        candidates.clear();
        candidates.reserve(n);
        if (init_candidates.empty()) {
            for (uint32_t c = 0; c < n; ++c) {
                if (!contains(s_part, c)) {
                    candidates.push_back(c);
                }
            }
        } else {
            for (auto c: init_candidates) {
                if (!contains(s_part, c)) {
                    candidates.push_back(c);
                }
            }
        }

        best_score = -std::numeric_limits<TypeSF>::max();

#if BF_ASSERT_ENABLED
        all_tested_sets.clear();
#endif
    }

    std::vector<uint32_t> search() {
        // initial greedy solution
        simple_greedy();
        best_score = -std::numeric_limits<TypeSF>::max(); // t.evaluate_general(best_s, k);
#if BF_ASSERT_ENABLED
        if (n <= 20) {
        all_tested_sets.reserve(nCk(n, k));
    }
#endif

        recursive_search(0);

#if BF_ASSERT_ENABLED
        if (n <= 20) {
        ASSERT(all_tested_sets.size() == nCk(n, k));
        ASSERT(!contains_duplicate(all_tested_sets, all_tested_sets.size()));
        all_tested_sets.clear();
    }
#endif
        return best_s;
    };

    void recursive_search(size_t depth) {
        if (s_size == k) {
            // if the size of S is k then evaluate_XD

#if BF_ASSERT_ENABLED
            if (n <= 20) {
            std::sort(s.begin(), s.end());
            all_tested_sets.push_back(s);
        }
#endif
            sf_evaluated += 1;
            TypeSF score = t.evaluate_general(s, s_size);
            if (score > best_score) {
                best_score = score;
                for (size_t i = 0; i < k; ++i) {
                    best_s[i] = s[i];
                }
            }
            return;
        } else if (depth == 0) {
            // special case at the top of the tree
            for (size_t idx = 0; idx < candidates.size(); ++idx) {
                indices[depth] = idx;

                uint32_t c = candidates[idx];

                s[s_size] = c;
                s_size += 1;
                recursive_search(depth + 1);
                s_size -= 1;
            }
            return;
        } else {
            // determine which element to add to the set
            size_t max_idx = indices[depth - 1];
            for (size_t idx = max_idx + 1; idx < candidates.size(); ++idx) {
                indices[depth] = idx;

                uint32_t c = candidates[idx];
                s[s_size] = c;
                s_size += 1;
                recursive_search(depth + 1);
                s_size -= 1;
            }
        }

    }

    void simple_greedy() {
        TypeSF score = -std::numeric_limits<TypeSF>::max();

        for (size_t i = s_size; i < k; ++i) {
            uint32_t best_c = 0;

            for (unsigned int c: candidates) {
                if (!contains(best_s, i, c)) {
                    best_s[i] = c;
                    TypeSF new_score = t.evaluate_general(best_s, i);

                    if (new_score >= score) {
                        score = new_score;
                        best_c = c;
                    }
                }
            }
            s[i] = best_c;
        }
    }
};

#endif //SMSM_BF_TREE_SEARCH_H
