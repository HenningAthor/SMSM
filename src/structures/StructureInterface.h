#ifndef SMSM_STRUCTUREINTERFACE_H
#define SMSM_STRUCTUREINTERFACE_H

#include <vector>
#include <cstddef>
#include <cstdint>

/**
 * Interface for structures, so that all score functions can be called in the
 * same way.
 *
 * @tparam TypeSF Datatype of the score function.
 */
template<typename TypeSF>
class StructureInterface {
public:
    TypeSF max_reachable_score = std::numeric_limits<TypeSF>::max(); // maximum reachable score

    /**
     * Gives the score for the empty set.
     *
     * @return Score of empty set.
     */
    virtual inline TypeSF evaluate_empty_set() = 0;

    /**
     * Gives the score for the set that has only added one element.
     *
     * @return Score of the set.
     */
    virtual inline TypeSF evaluate_1D(const std::vector<uint32_t> &s, size_t s_size) = 0;

    /**
     * Gives the score for the set that has only added two elements.
     *
     * @return Score of the set.
     */
    virtual inline TypeSF evaluate_2D(const std::vector<uint32_t> &s, size_t s_size) = 0;

    /**
     * Evaluates the score function, for sets that have added more than two
     * elements.
     *
     * @param s The set S.
     * @param s_size The size of set S.
     */
    virtual inline TypeSF evaluate_XD(const std::vector<uint32_t> &s, size_t s_size) = 0;

    /**
     * Evaluates the score function in the most general way. The function
     * should be able to accept any set of any size.
     *
     * @param s The set S.
     * @param s_size The size of set S.
     */
    virtual inline TypeSF evaluate_general(const std::vector<uint32_t> &s, size_t s_size) = 0;

    /**
    * Processes the graph after all edges have been added.
    */
    virtual inline void finalize() = 0;

    /**
     * Will initialize helping structures, that can help with score function
     * evaluation.
     *
     * @param k Number of depths, the search algorithm can explore.
     */
    virtual inline void initialize_helping_structures(size_t k) = 0;

    /**
     * Signals to the structure, that a new depth will be explored. Use this
     * function to keep the helping structures up to date.
     *
     * @param s The set S, that is present at the new depth.
     * @param s_size Size of the set S.
     */
    virtual inline void visit_new_depth(const std::vector<uint32_t> &s, size_t s_size) = 0;

    /**
     * Signals to the structure, that we returned from the last depth. Use this
     * function to keep the helping structures up to date.
     */
    virtual inline void return_from_last_depth() = 0;
};

#endif //SMSM_STRUCTUREINTERFACE_H
