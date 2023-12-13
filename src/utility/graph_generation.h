#ifndef SMSM_GRAPH_GENERATION_H
#define SMSM_GRAPH_GENERATION_H

#include <cstdint>
#include <string>
#include <vector>
#include <filesystem>

#include "util.h"
#include "../structures/graph.h"
#include "../structures/graph_NegativeGroupFarness.h"
#include "../structures/graph_PartialDominatingSet.h"

/**
 * Generates a random connected graph. This function is specialized to generate small
 * graphs.
 *
 * @param n_nodes Number of nodes.
 * @param n_edges Number of edges (at least n_nodes - 1).
 * @return The graph.
 */
Graph<int> random_graph(size_t n_nodes,
                        size_t n_edges);

/**
 * Generates a random graph dataset. It will start with 5 nodes and increment
 * it every time by one. For every number of nodes n_graphs are generated.
 *
 * @param max_n_nodes Maximal number of nodes in the graph.
 * @param n_graphs Number of graphs for each n_nodes.
 */
void random_graph_dataset(size_t max_n_nodes,
                          size_t n_graphs,
                          std::string &dataset_name);

GraphNegativeGroupFarness<int> random_GraphNegativeGroupFarness(size_t n_nodes, size_t n_edges);

std::vector<GraphNegativeGroupFarness<int>> random_GraphNegativeGroupFarness_dataset(size_t n_nodes, size_t n_graphs);

GraphPartialDominatingSet<int> random_GraphPartialDominatingSet(size_t n_nodes, size_t n_edges);

std::vector<GraphPartialDominatingSet<int>> random_GraphPartialDominatingSet_dataset(size_t n_nodes, size_t n_graphs);

#endif //SMSM_GRAPH_GENERATION_H
