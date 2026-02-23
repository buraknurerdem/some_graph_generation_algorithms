// Author: Burak Nur Erdem

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <random>

enum operation_type
{
    COMPLETE_JOIN,
    DISJOINT_UNION,
    OPERATION_SUBSTITUTION,
    OPERATION_COMPOSITION,
    OPERATION_CLIQUE_ID,
    COMPLEMENT
};

struct graph_file_info
{
    std::string type, order, density, id;
};

void print_graph(const std::vector<std::vector<bool>> &graph);

double expected_value_of_odd_holes(int n, double p);

double expected_value_of_odd_anti_holes(int n, double p);

bool is_graph_empty_or_complete(std::vector<std::vector<bool>> &graph);

std::string cycle_vector_to_string(std::vector<int> vec);

// std::string get_string_rep_of_vp(int v1, int v2);
// std::pair<int, int> get_two_integers_in_vp_string(std::string input);

std::vector<std::vector<bool>> get_complement_of_graph(const std::vector<std::vector<bool>> &graph);

// Read graph from a txt file containing adjacency matrix
std::vector<std::vector<bool>> read_graph_adj_matrix_from_txt_file(std::string graph_file_name);

double calculate_density(const std::vector<std::vector<bool>> &graph);

std::vector<std::vector<int>> get_adj_list_from_adj_matrix(const std::vector<std::vector<bool>> &graph);

void print_odd_holes(const std::unordered_map<std::string, std::vector<int>> &odd_holes);

int calculate_number_of_changes(
    const std::vector<std::vector<bool>> &g1, const std::vector<std::vector<bool>> &g2
);

void remove_entries_containing_vp_from_odd_holes(
    std::unordered_map<std::string, std::vector<int>> &odd_holes, int v1, int v2
);

inline std::pair<int, int> unpack_pair(uint64_t key)
{
    int u = static_cast<int>(key >> 32);        // upper 32 bits
    int v = static_cast<int>(key & 0xFFFFFFFF); // lower 32 bits
    return {u, v};
}

inline uint64_t pack_pair(int u, int v)
{
    if (u > v)
        std::swap(u, v);

    return (static_cast<uint64_t>(u) << 32) | static_cast<uint64_t>(v);
}

bool parse_graph_filename(const std::string &filepath, graph_file_info &graph_file_info_obj);

bool is_matrix_symmetric(const std::vector<std::vector<bool>> &mat);

std::vector<int> find_a_maximal_clique_including_a_given_vertex(const std::vector<std::vector<bool>> &graph, int v);

std::string create_output_adj_matrix_file_name(std::vector<std::vector<bool>> &graph, std::string graph_folder, std::string type, int order, std::string density_dec_str, int id);

void write_adj_matrix(std::vector<std::vector<bool>> &graph, std::string output_file_name);

void remove_vertex_from_graph(std::vector<std::vector<bool>> &graph, int v);

std::vector<int> select_k_without_replacement(int n, int k);

int random_weighted_choice(const std::vector<int> &weights, double random_value);

int select_random_integer(int n);

uint64_t select_random_vertex_pair_packed(int n);

std::pair<int, int> select_random_vertex_pair_unpacked(int n);

void complete_join(std::vector<std::vector<bool>> &g1, const std::vector<std::vector<bool>> &g2);

void disjoint_union(std::vector<std::vector<bool>> &g1, const std::vector<std::vector<bool>> &g2);

void operation_substitution(std::vector<std::vector<bool>> &g1, const std::vector<std::vector<bool>> &g2, std::mt19937 &gen);

void operation_composition(std::vector<std::vector<bool>> &g1, const std::vector<std::vector<bool>> &g2, std::mt19937 &gen);

void operation_clique_identification(std::vector<std::vector<bool>> &g1, const std::vector<std::vector<bool>> &g2, std::mt19937 &gen);

std::vector<uint8_t> adj_matrix_to_g6(const std::vector<std::vector<bool>> &graph);

inline std::pair<size_t, size_t> upper_adj_matrix_flat_index_to_pair_index(size_t x)
{
    size_t j = (1 + std::sqrt(8.0 * x + 1)) / 2;
    size_t i = x - (j * (j - 1) / 2);
    return {i, j};
}

inline size_t pair_index_to_upper_adj_matrix_flat_index(size_t i, size_t j)
{
    if (i > j)
    {
        size_t temp = i;
        i = j;
        j = temp;
    }

    size_t x = j * (j - 1) / 2 + i;
    return x;
}

inline size_t pair_index_to_flat_adj_mat_index(size_t n, size_t i, size_t j)
{
    size_t mat_index = j * n + i;
    return mat_index;
}

inline std::pair<size_t, size_t> flat_adj_matrix_index_to_pair_index(size_t n, size_t matrix_index)
{
    size_t j = matrix_index / n;       // floor gives the j'th index.
    size_t i = matrix_index - (j * n); // remainder gives the i'th index
    return std::make_pair(i, j);
}

void write_g6(std::vector<uint8_t>& g6_vector, std::string file_path);

std::vector<std::vector<bool>> g6_to_adj_matrix(std::string &g6_string);

std::string get_random_line_from_g6_file(std::string path, std::mt19937 &gen);