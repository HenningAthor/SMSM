#ifndef SMSM_CANDIDATEMANAGER_H
#define SMSM_CANDIDATEMANAGER_H

#include <cstdint>
#include <algorithm>

#include "../utility/util.h"

/**
 * Represents a candidate with score improvement and accuracy information.
 *
 * @tparam TypeSF The type of the 'score_improvement' attribute (e.g., int, float, double).
 */
template<typename TypeSF>
struct CandidateEntry {
    TypeSF score_improvement;
    uint32_t candidate;
    uint8_t accurate;
};

/**
 * Manages a collection of candidate entries for optimization.
 *
 * @tparam TypeSF The numeric type used for score improvement.
 */
template<typename TypeSF>
class CandidateManager {
public:
    std::vector<CandidateEntry<TypeSF>> candidates;
    std::vector<TypeSF> csum_si;

    size_t offset; // current offset into the candidates
    size_t size; // current size of the candidates
    size_t max_size; // maximum size of the candidates

    std::vector<CandidateEntry<TypeSF>> SUB_heap;
    size_t SUB_heap_size;
    size_t SUB_heap_max_size;
    TypeSF SUB_heap_sum;

    size_t candidate_heap_size;
    size_t n_sorted_candidates;

    /**
     * Constructor.
     *
     * @param n Number of candidates.
     */
    explicit CandidateManager(size_t n) {
        candidates.resize(n);
        csum_si.resize(n + 1);

        offset = 0;
        size = 0;
        max_size = n;

        SUB_heap_size = 0;
        SUB_heap_max_size = 0;
        SUB_heap_sum = 0;

        candidate_heap_size = 0;
        n_sorted_candidates = 0;
    };

    void reinitialize(size_t n){
        candidates.resize(n);
        csum_si.resize(n + 1);

        offset = 0;
        size = 0;
        max_size = n;

        SUB_heap_size = 0;
        SUB_heap_max_size = 0;
        SUB_heap_sum = 0;

        candidate_heap_size = 0;
        n_sorted_candidates = 0;
    }

    /**
     * Fills the candidate array with values 0, ..., n-1.
     *
     * @param n Number of elements.
     */
    void fill_candidates(size_t n) {
        size_t idx = 0;
        for (size_t i = 0; i < n; ++i) {
            candidates[idx].candidate = i;
            candidates[idx].score_improvement = 0;
            candidates[idx].accurate = 0;
            idx += 1;
        }
        offset = 0;
        size = idx;
    };

    void fill_from_candidate_manager(const CandidateManager<TypeSF> &ref_manager) {
        for (size_t i = ref_manager.offset; i < ref_manager.size; ++i) {
            candidates[i - ref_manager.offset].candidate = ref_manager.get_c(i);
        }
        offset = 0;
        size = ref_manager.size - ref_manager.offset;
    }

    /**
    * Add a candidate entry to the manager.
    *
    * @param candidate_id An identifier or index for the candidate.
    * @param score_improvement The score improvement.
    * @param is_accurate A flag indicating the accuracy.
    */
    void add_entry(uint32_t candidate_id, TypeSF score_improvement, uint8_t is_accurate) {
        candidates[size].candidate = candidate_id;
        candidates[size].score_improvement = score_improvement;
        candidates[size].accurate = is_accurate;
        size += 1;
    }

    void set_entry(size_t idx, uint32_t candidate_id, TypeSF score_improvement, uint8_t is_accurate) {
        candidates[idx].candidate = candidate_id;
        candidates[idx].score_improvement = score_improvement;
        candidates[idx].accurate = is_accurate;
    }

    /**
    * Sort the candidates by score improvement in descending order.
    */
    void sort() {
        auto first = candidates.begin();

        auto last = candidates.begin();
        std::advance(last, size);

        std::sort(first, last, [](const CandidateEntry<TypeSF> &a, const CandidateEntry<TypeSF> &b) {
            return a.score_improvement > b.score_improvement;
        });
    }

    /**
    * Calculate the cumulative sum of score improvements for candidates.
    */
    void calc_csum() {
        TypeSF cumulative_sum = 0;
        csum_si[0] = 0; // Initialize the first element.

        for (size_t i = 0; i < size; ++i) {
            cumulative_sum += candidates[i].score_improvement;
            csum_si[i + 1] = cumulative_sum;
        }
    }

    /**
     * Clears the manager.
     */
    void clear(size_t r) {
        SUB_heap.resize(r);
        SUB_heap_size = 0;
        SUB_heap_max_size = r;
        SUB_heap_sum = 0;

        size = 0;
    }

    void SUB_heap_add(uint32_t candidate, TypeSF score_improvement){
        if (SUB_heap_size < SUB_heap_max_size) {
            // add the element to the heap, if it has less than r elements
            SUB_heap_sum += score_improvement;
            SUB_heap[SUB_heap_size].candidate = candidate;
            SUB_heap[SUB_heap_size].score_improvement = score_improvement;
            SUB_heap_size += 1;

            if (SUB_heap_size == SUB_heap_max_size) {
                // if the heap is full, then heapify
                std::make_heap(SUB_heap.begin(), SUB_heap.end(), [](const CandidateEntry<TypeSF> &a, const CandidateEntry<TypeSF> &b) { return a.score_improvement > b.score_improvement; });
            }
        } else {
            if (score_improvement > SUB_heap[0].score_improvement) {
                // we have to push the score improvement on the heap
                SUB_heap_sum = SUB_heap_sum - SUB_heap[0].score_improvement + score_improvement;
                SUB_heap[0].candidate = candidate;
                SUB_heap[0].score_improvement = score_improvement;

                std::make_heap(SUB_heap.begin(), SUB_heap.end(), [](const CandidateEntry<TypeSF> &a, const CandidateEntry<TypeSF> &b) { return a.score_improvement > b.score_improvement; });
            }
        }
    }

    size_t SUB_heap_find(uint32_t candidate){
        for(size_t i = 0; i < SUB_heap_size; ++i){
            if (SUB_heap[i].candidate == candidate){
                return i;
            }
        }
        std::cout << "Error: Could not find candidate " << candidate << std::endl;
        abort();
    }

    void SUB_heap_update(size_t idx, TypeSF score_improvement){
        SUB_heap_sum = SUB_heap_sum - SUB_heap[idx].score_improvement + score_improvement;

        SUB_heap[idx].score_improvement = score_improvement;
        std::make_heap(SUB_heap.begin(), SUB_heap.end(), [](const CandidateEntry<TypeSF> &a, const CandidateEntry<TypeSF> &b) { return a.score_improvement > b.score_improvement; });
    }

    TypeSF SUB_heap_min() {
        return SUB_heap[0].score_improvement;
    }

    TypeSF get_SUB_heap_sum() {
        return SUB_heap_sum;
    }

    /**
    * Calculate the sum of score improvements for a range of candidates.
    *
    * @param start_idx The index of the first candidate in the range.
    * @param count The number of candidates to include in the sum.
    * @return The sum of score improvements for the specified range of candidates.
    */
    TypeSF get_partial_sum(size_t start_idx, size_t count) const {
        // Calculate the sum of score improvements for the specified range.
        return csum_si[start_idx + count] - csum_si[start_idx];
    }

    /**
     * Returns the score improvement of the candidate at the specified index.
     *
     * @param idx The index.
     * @return The score improvement.
     */
    TypeSF get_si(size_t idx) const {
        return candidates[idx].score_improvement;
    }

    /**
     * Returns the candidate at the specified index.
     *
     * @param idx The index.
     * @return The candidate.
     */
    uint32_t get_c(size_t idx) const {
        return candidates[idx].candidate;
    }

    /**
     * Returns the accuracy of score improvement of the candidate at the specified index.
     *
     * @param idx The index.
     * @return The accuracy.
     */
    uint8_t get_acc(size_t idx) const {
        return candidates[idx].accurate;
    }

    /**
     * Prints the elements.
     *
     * @param n Number of elements.
     */
    void print(size_t n) const {
        std::string s0 = "idx [", s1 = "c   [", s2 = "si  [", s3 = "csi [", s4 = "acc [";
        for (size_t i = 0; i < n; ++i) {
            std::string t0, t1, t2, t3, t4;

            t0 = std::to_string(i);
            t1 = std::to_string(candidates[i].candidate);
            t2 = double_to_string(candidates[i].score_improvement, 2);
            t3 = double_to_string(csum_si[i], 2);
            t4 = std::to_string(candidates[i].accurate);

            size_t max_len = std::max({(size_t) t0.size(), (size_t) t1.size(), (size_t) t2.size(), (size_t) t3.size(), (size_t) t4.size()});
            while (t0.size() != max_len) {
                t0.push_back(' ');
            }
            while (t1.size() != max_len) {
                t1.push_back(' ');
            }
            while (t2.size() != max_len) {
                t2.push_back(' ');
            }
            while (t3.size() != max_len) {
                t3.push_back(' ');
            }
            while (t4.size() != max_len) {
                t4.push_back(' ');
            }

            s0 += t0 + ", ";
            s1 += t1 + ", ";
            s2 += t2 + ", ";
            s3 += t3 + ", ";
            s4 += t4 + ", ";
        }
        if (s0.size() > 2) {
            s0.pop_back();
            s0.pop_back();
            s1.pop_back();
            s1.pop_back();
            s2.pop_back();
            s2.pop_back();
            s3.pop_back();
            s3.pop_back();
            s4.pop_back();
            s4.pop_back();
        }

        s0 += "]";
        s1 += "]";
        s2 += "]";
        s3 += ", " + double_to_string(csum_si[n], 2) + "]";
        s4 += "]";

        std::cout << s0 << "\n" << s1 << "\n" << s2 << "\n" << s3 << "\n" << s4 << std::endl;
    }

    /**
     * Simply prints all candidates.
     */
    void print_c() const {
        std::cout << "candidate : ";
        for (size_t i = offset; i < size; ++i) {
            std::cout << get_c(i) << " ";
        }
        std::cout << std::endl;
    }

    /**
     * Simply prints all score improvements.
     */
    void print_si() const {
        std::cout << "score_improvement: ";
        for (size_t i = offset; i < size; ++i) {
            std::cout << get_si(i) << " ";
        }
        std::cout << std::endl;
    }

    void prepare_candidate_heap(size_t n) {
        size = n;
        offset = 0;
        candidate_heap_size = 0;
        n_sorted_candidates = 0;
        csum_si[0] = 0;
    }

    void add_to_candidate_heap(uint32_t c, TypeSF si) {
        candidates[size - 1 - candidate_heap_size].candidate = c;
        candidates[size - 1 - candidate_heap_size].score_improvement = si;
        candidates[size - 1 - candidate_heap_size].accurate = 0;
        candidate_heap_size += 1;
    }

    bool candidate_heap_is_empty(){
        return candidate_heap_size == 0;
    }

    bool top_candidate_heap_is_accurate() {
        return candidates[size - 1].accurate;
    }

    uint32_t top_candidate_heap_get_c() {
        return candidates[size - 1].candidate;
    }

    TypeSF top_candidate_heap_get_si() {
        return candidates[size - 1].score_improvement;
    }

#define get_idx_child_1(idx) (((idx)*2) + 1);
#define get_idx_child_2(idx) (((idx)*2) + 2);
#define get_idx_parent(idx ) (((idx) - 1)/2);

    void top_candidate_heap_update(TypeSF si) {
        candidates[size - 1].score_improvement = si;
        candidates[size - 1].accurate = 1;
        drop_down(0);
    }

    void top_candidate_heap_remove() {
        n_sorted_candidates += 1;
        if(candidate_heap_size == 1){
            candidate_heap_size -= 1;
            csum_si[size] = csum_si[size - 1] + candidates[size - 1].score_improvement;
            return;
        }

        TypeSF si_temp = candidates[size - 1].score_improvement;
        candidates[size - 1].score_improvement = -1;

        size_t idx = drop_down(0);

        std::swap(candidates[size - candidate_heap_size], candidates[size - 1 - idx]);
        candidates[size - candidate_heap_size].score_improvement = si_temp;

        candidate_heap_size -= 1;

        csum_si[size - candidate_heap_size] = csum_si[size - candidate_heap_size - 1] + si_temp;

        if (idx < candidate_heap_size){
            bubble_up(idx);
        }
    }

    void place_top_candidate_heap_away(){
        candidates[n_sorted_candidates] = candidates[size - 1];
        n_sorted_candidates += 1;

        std::swap(candidates[size - candidate_heap_size], candidates[size - 1]);
        candidate_heap_size -= 1;
        drop_down(0);
    }

    void push_to_candidate_heap(uint32_t c, TypeSF si){
        candidates[size - 1 - candidate_heap_size].candidate = c;
        candidates[size - 1 - candidate_heap_size].score_improvement = si;
        candidates[size - 1 - candidate_heap_size].accurate = 0;
        candidate_heap_size += 1;
        bubble_up(candidate_heap_size-1);
    }

    void candidate_heap_sort_remaining() {
        if (candidate_heap_size == 0){
            return;
        }

        auto first = candidates.begin();
        std::advance(first, size - candidate_heap_size);

        auto last = candidates.begin();
        std::advance(last, size);

        std::sort(first, last, [](const CandidateEntry<TypeSF> &a, const CandidateEntry<TypeSF> &b) {
            return a.score_improvement > b.score_improvement;
        });

        for (size_t i = size - candidate_heap_size; i < size; ++i) {
            csum_si[i + 1] = csum_si[i] + candidates[i].score_improvement;
        }
    }

    size_t drop_down(size_t idx) {
        // make sure heap is correct
        size_t idx_to_swap;

        size_t idx_child_1 = get_idx_child_1(idx);
        size_t idx_child_2 = get_idx_child_2(idx);

        bool child_1_valid = idx_child_1 < candidate_heap_size && candidates[size - 1 - idx_child_1].score_improvement > candidates[size - 1 - idx].score_improvement;
        bool child_2_valid = idx_child_2 < candidate_heap_size && candidates[size - 1 - idx_child_2].score_improvement > candidates[size - 1 - idx].score_improvement;

        while (child_1_valid || child_2_valid) {
            // at least one child is larger
            if (child_1_valid && child_2_valid) {
                // both children are valid
                if (candidates[size - 1 - idx_child_1].score_improvement > candidates[size - 1 - idx_child_2].score_improvement) {
                    // swap with child 1
                    idx_to_swap = idx_child_1;
                } else {
                    // swap with child 2
                    idx_to_swap = idx_child_2;
                }
            } else if (child_1_valid){
                // swap with child 1
                idx_to_swap = idx_child_1;
            } else{
                // swap with child 2
                idx_to_swap = idx_child_2;
            }

            std::swap(candidates[size - 1 - idx], candidates[size - 1 - idx_to_swap]);
            idx = idx_to_swap;

            idx_child_1 = get_idx_child_1(idx);
            idx_child_2 = get_idx_child_2(idx);

            child_1_valid = idx_child_1 < candidate_heap_size && candidates[size - 1 - idx_child_1].score_improvement > candidates[size - 1 - idx].score_improvement;
            child_2_valid = idx_child_2 < candidate_heap_size && candidates[size - 1 - idx_child_2].score_improvement > candidates[size - 1 - idx].score_improvement;
        }

        return idx;
    }

    size_t bubble_up(size_t idx) {
        if(idx == 0){
            return 0;
        }

        size_t parent_idx = get_idx_parent(idx);

        while(candidates[size - 1 - idx].score_improvement > candidates[size - 1 - parent_idx].score_improvement){
            std::swap(candidates[size - 1 - idx], candidates[size - 1 - parent_idx]);
            idx = parent_idx;

            if(idx == 0){
                return 0;
            }

            parent_idx = get_idx_parent(idx);
        }
        return idx;
    }

    bool is_valid_heap(size_t idx) {
        if (candidate_heap_size == 0) {
            return true;
        }

        TypeSF si = candidates[size - 1 - idx].score_improvement;
        bool valid = true;

        size_t child_idx_1 = get_idx_child_1(idx);
        if (child_idx_1 < candidate_heap_size) {
            TypeSF si_child = candidates[size - 1 - child_idx_1].score_improvement;
            valid &= (si >= si_child) && is_valid_heap(child_idx_1);
        }

        size_t child_idx_2 = get_idx_child_2(idx);
        if (child_idx_2 < candidate_heap_size) {
            TypeSF si_child = candidates[size - 1 - child_idx_2].score_improvement;
            valid &= (si >= si_child) && is_valid_heap(child_idx_2);
        }
        return valid;
    }

    TypeSF get_sum_last_r_sorted(size_t r){
        r = std::min(r, n_sorted_candidates);

        return get_partial_sum(n_sorted_candidates - r, r);
    }

    std::string return_spaces(size_t depth, size_t max_depth, size_t max_len){
        std::string s = "";
        for(size_t i = 0; i < max_len * (max_depth - depth); ++i){
            s.push_back(' ');
        }
        return s;
    }

    void collect_to_vec(size_t depth, size_t idx, std::vector<std::vector<std::string>> &depths){
        std::string content = std::to_string(candidates[size - 1 - idx].candidate) + "/" + std::to_string(candidates[size - 1 - idx].score_improvement) + "/" + std::to_string(idx);
        depths[depth].push_back(content);

        size_t idx_child_1 = get_idx_child_1(idx);
        if(idx_child_1 < candidate_heap_size){
            collect_to_vec(depth+1, idx_child_1, depths);
        }

        size_t idx_child_2 = get_idx_child_2(idx);
        if(idx_child_2 < candidate_heap_size){
            collect_to_vec(depth+1, idx_child_2, depths);
        }
    }

    void print_heap(){
        if (candidate_heap_size == 0){
            return;
        }

        size_t max_depth = log2(candidate_heap_size) + 1;
        std::vector<std::vector<std::string>> depths;
        depths.resize(max_depth);

        collect_to_vec(0, 0, depths);

        size_t max_len = 0;
        for(auto &v : depths){
            for(auto &s : v){
                max_len = std::max(max_len, s.size());
            }
        }
        for(auto &v : depths){
            for(auto &s : v){
                while(s.size() < max_len){
                    s.push_back(' ');
                }
            }
        }

        for(size_t depth = 0; depth < max_depth; ++depth){
            std::string spaces = return_spaces(depth, max_depth, max_len);
            for(size_t i = 0; i < depths[depth].size(); ++i){
                std::cout << "   " << depths[depth][i];
            }
            std::cout << std::endl;
        }
    }
};


#endif //SMSM_CANDIDATEMANAGER_H
