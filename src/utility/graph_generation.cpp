#include "graph_generation.h"

Graph<int> random_graph(size_t n_nodes, size_t n_edges) {
    Graph<int> g(n_nodes, n_edges);

    size_t curr_n_edges = 0;
    std::vector<std::vector<uint32_t>> forrest;

    // forrest has n_nodes trees.
    for (uint32_t node = 0; node < n_nodes; ++node) {
        forrest.push_back({node});
    }

    // connect two trees (n_node-1) times
    for (uint32_t i = 0; i < n_nodes - 1; ++i) {
        size_t tree1_idx = random_int() % forrest.size();
        size_t tree2_idx = random_int() % forrest.size();

        while (tree1_idx == tree2_idx) {
            tree2_idx = random_int() % forrest.size();
        }

        uint32_t node1 = forrest[tree1_idx][random_int() % forrest[tree1_idx].size()];
        uint32_t node2 = forrest[tree2_idx][random_int() % forrest[tree2_idx].size()];
        g.add_edge(node1, node2);
        curr_n_edges += 1;

        forrest[tree1_idx].insert(forrest[tree1_idx].end(), forrest[tree2_idx].begin(), forrest[tree2_idx].end());
        forrest.erase(forrest.begin() + tree2_idx);
    }
    // one large tree

    std::vector<uint32_t> available_n1;
    std::vector<uint32_t> available_n2;
    for (uint32_t n1 = 0; n1 < n_nodes; ++n1) {
        for (uint32_t n2 = n1 + 1; n2 < n_nodes; ++n2) {
            if (!g.edge_exists(n1, n2)) {
                available_n1.push_back(n1);
                available_n2.push_back(n2);
            }
        }
    }

    // connect random graphs
    for (; curr_n_edges < n_edges; ++curr_n_edges) {
        size_t idx = random_int() % available_n1.size();
        g.add_edge(available_n1[idx], available_n2[idx]);

        std::swap(available_n1[idx], available_n1.back());
        std::swap(available_n2[idx], available_n2.back());
        available_n1.pop_back();
        available_n2.pop_back();
    }

    g.finalize();
    return g;
}

void random_graph_dataset(size_t n_nodes,
                          size_t n_graphs,
                          std::string &dataset_name) {
    std::string dir_path = "../data/random/" + dataset_name + "/";
    if (std::filesystem::is_directory(dir_path)) {
        return;
    }

    std::filesystem::create_directories(dir_path);

    for (uint32_t n = 0; n < n_graphs; ++n) {
        uint32_t n_edges = (n_nodes - 1) + (random_int() % ((n_nodes * (n_nodes - 1) / 2) - (n_nodes - 1)));
        Graph g = random_graph(n_nodes, n_edges);

        std::string file_path = dir_path + "g_";
        file_path.append(std::to_string(n_nodes));
        file_path.append("_");
        file_path.append(std::to_string(n_edges));
        file_path.append("_");
        file_path.append(std::to_string(n));
        file_path.append(".edges");
        g.write_to_file(file_path);
    }
}

GraphNegativeGroupFarness<int> random_GraphNegativeGroupFarness(size_t n_nodes, size_t n_edges) {
    GraphNegativeGroupFarness<int> g(n_nodes, n_edges);

    size_t curr_n_edges = 0;
    std::vector<std::vector<uint32_t>> forrest;

    // forrest has n_nodes trees.
    for (uint32_t node = 0; node < n_nodes; ++node) {
        forrest.push_back({node});
    }

    // connect two trees (n_node-1) times
    for (uint32_t i = 0; i < n_nodes - 1; ++i) {
        size_t tree1_idx = random_int() % forrest.size();
        size_t tree2_idx = random_int() % forrest.size();

        while (tree1_idx == tree2_idx) {
            tree2_idx = random_int() % forrest.size();
        }

        uint32_t node1 = forrest[tree1_idx][random_int() % forrest[tree1_idx].size()];
        uint32_t node2 = forrest[tree2_idx][random_int() % forrest[tree2_idx].size()];
        g.add_edge(node1, node2);
        curr_n_edges += 1;

        forrest[tree1_idx].insert(forrest[tree1_idx].end(), forrest[tree2_idx].begin(), forrest[tree2_idx].end());
        forrest.erase(forrest.begin() + tree2_idx);
    }
    // one large tree

    std::vector<uint32_t> available_n1;
    std::vector<uint32_t> available_n2;
    for (uint32_t n1 = 0; n1 < n_nodes; ++n1) {
        for (uint32_t n2 = n1 + 1; n2 < n_nodes; ++n2) {
            if (!g.edge_exists(n1, n2)) {
                available_n1.push_back(n1);
                available_n2.push_back(n2);
            }
        }
    }

    // connect random graphs
    for (; curr_n_edges < n_edges; ++curr_n_edges) {
        size_t idx = random_int() % available_n1.size();
        g.add_edge(available_n1[idx], available_n2[idx]);

        std::swap(available_n1[idx], available_n1.back());
        std::swap(available_n2[idx], available_n2.back());
        available_n1.pop_back();
        available_n2.pop_back();
    }

    g.finalize();
    return g;
}

std::vector<GraphNegativeGroupFarness<int>> random_GraphNegativeGroupFarness_dataset(size_t n_nodes, size_t n_graphs) {
    std::vector<GraphNegativeGroupFarness<int>> vec;

    for (uint32_t n = 0; n < n_graphs; ++n) {
        uint32_t n_edges = (n_nodes - 1) + (random_int() % ((n_nodes * (n_nodes - 1) / 2) - (n_nodes - 1)));
        vec.emplace_back(random_GraphNegativeGroupFarness(n_nodes, n_edges));
    }

    return vec;
}

GraphPartialDominatingSet<int> random_GraphPartialDominatingSet(size_t n_nodes, size_t n_edges) {
    GraphPartialDominatingSet<int> g(n_nodes, n_edges);

    size_t curr_n_edges = 0;
    std::vector<std::vector<uint32_t>> forrest;

    // forrest has n_nodes trees.
    for (uint32_t node = 0; node < n_nodes; ++node) {
        forrest.push_back({node});
    }

    // connect two trees (n_node-1) times
    for (uint32_t i = 0; i < n_nodes - 1; ++i) {
        size_t tree1_idx = random_int() % forrest.size();
        size_t tree2_idx = random_int() % forrest.size();

        while (tree1_idx == tree2_idx) {
            tree2_idx = random_int() % forrest.size();
        }

        uint32_t node1 = forrest[tree1_idx][random_int() % forrest[tree1_idx].size()];
        uint32_t node2 = forrest[tree2_idx][random_int() % forrest[tree2_idx].size()];
        g.add_edge(node1, node2);
        curr_n_edges += 1;

        forrest[tree1_idx].insert(forrest[tree1_idx].end(), forrest[tree2_idx].begin(), forrest[tree2_idx].end());
        forrest.erase(forrest.begin() + tree2_idx);
    }
    // one large tree

    std::vector<uint32_t> available_n1;
    std::vector<uint32_t> available_n2;
    for (uint32_t n1 = 0; n1 < n_nodes; ++n1) {
        for (uint32_t n2 = n1 + 1; n2 < n_nodes; ++n2) {
            if (!g.edge_exists(n1, n2)) {
                available_n1.push_back(n1);
                available_n2.push_back(n2);
            }
        }
    }

    // connect random graphs
    for (; curr_n_edges < n_edges; ++curr_n_edges) {
        size_t idx = random_int() % available_n1.size();
        g.add_edge(available_n1[idx], available_n2[idx]);

        std::swap(available_n1[idx], available_n1.back());
        std::swap(available_n2[idx], available_n2.back());
        available_n1.pop_back();
        available_n2.pop_back();
    }

    g.finalize();
    return g;
}

std::vector<GraphPartialDominatingSet<int>> random_GraphPartialDominatingSet_dataset(size_t n_nodes, size_t n_graphs) {
    std::vector<GraphPartialDominatingSet<int>> vec;

    for (uint32_t n = 0; n < n_graphs; ++n) {
        uint32_t n_edges = (n_nodes - 1) + (random_int() % ((n_nodes * (n_nodes - 1) / 2) - (n_nodes - 1)));
        vec.emplace_back(random_GraphPartialDominatingSet(n_nodes, n_edges));
    }

    return vec;
}
