#include <iostream>

#include "src/utility/AlgorithmConfiguration.h"
#include "src/structures/graph_NegativeGroupFarness.h"
#include "src/structures/graph_PartialDominatingSet.h"
#include "src/structures/dataPoints_EuclidianDistance.h"
#include "src/algorithms/tree_search_iterative.h"

int main(int argc, char *argv[]) {
    AlgorithmConfiguration ac = parse_command_line(argc, argv, true);
    if (ac.invalid) {
        std::cout << ac.to_JSON() << std::endl;
        return EXIT_FAILURE;
    }

    if (ac.structure_type == "graph") {
        if (ac.score_function == "negative-group-farness") {
            GraphNegativeGroupFarness<int> g(ac.input_file_path);
            g.finalize();
            g.initialize_helping_structures(ac.k);
            if (g.get_n() < ac.k) {
                std::cout << "n (" << g.get_n() << ") is smaller than k (" << ac.k << ")!" << std::endl;
                return EXIT_FAILURE;
            }
            TreeSearchIterative<GraphNegativeGroupFarness<int>, int> ts = TreeSearchIterative<GraphNegativeGroupFarness<int>, int>(g, ac);
            ts.search();
        } else if (ac.score_function == "partial-dominating-set") {
            GraphPartialDominatingSet<int> g(ac.input_file_path);
            g.finalize();
            g.initialize_helping_structures(ac.k);
            if (g.get_n() < ac.k) {
                std::cout << "n (" << g.get_n() << ") is smaller than k (" << ac.k << ")!" << std::endl;
                return EXIT_FAILURE;
            }

            TreeSearchIterative<GraphPartialDominatingSet<int>, int> ts = TreeSearchIterative<GraphPartialDominatingSet<int>, int>(g, ac);
            ts.search();
        } else {
            std::cout << "Score function '" << ac.score_function << "' not known for structure type '" << ac.structure_type << "'!" << std::endl;
            return EXIT_FAILURE;
        }
    } else if (ac.structure_type == "k-medoid") {
        if (ac.score_function == "euclidian-distance") {
            DataPointsEuclidianDistance<double> dp(ac.input_file_path);
            dp.finalize();
            dp.initialize_helping_structures(ac.k);
            if (dp.get_n() < ac.k) {
                std::cout << "n (" << dp.get_n() << ") is smaller than k (" << ac.k << ")!" << std::endl;
                return EXIT_FAILURE;
            }

            TreeSearchIterative<DataPointsEuclidianDistance<double>, double> ts = TreeSearchIterative<DataPointsEuclidianDistance<double>, double>(dp, ac);
            ts.search();
        } else {
            std::cout << "Score function '" << ac.score_function << "' not known for structure type '" << ac.structure_type << "'!" << std::endl;
            return EXIT_FAILURE;
        }
    } else {
        std::cout << "Structure type '" << ac.structure_type << "' not known!" << std::endl;
        return EXIT_SUCCESS;
    }

    return 0;
}
