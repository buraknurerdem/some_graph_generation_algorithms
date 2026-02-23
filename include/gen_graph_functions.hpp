// Author: Burak Nur Erdem

#pragma once

#include <vector>
#include <string>

std::vector<std::vector<bool>> gen_c5_free(size_t n, double d);

std::vector<std::vector<bool>> gen_perfect_via_operations(int n, double d, double eps, std::string folder_path);

std::vector<std::vector<bool>> gen_perturb_vertex_pairs(const std::vector<std::vector<bool>> &base_graph, double vertex_pair_ratio);

std::vector<std::vector<bool>> gen_threshold_graph(int n, double p);

std::vector<std::vector<bool>> gen_erdos_renyi(int n, double p);

std::vector<std::vector<bool>> gen_graph_outer_sandwich(const std::vector<std::vector<bool>> &base_graph, double optional_edge_density);

std::vector<std::vector<bool>> gen_BA_with_given_order_and_density(int n, double d);

std::vector<std::vector<bool>> embed_hole(const std::vector<std::vector<bool>> &input_graph, int k);