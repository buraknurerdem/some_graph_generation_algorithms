// Author: Burak Nur Erdem

#include <iostream>
#include <map>
#include <random>
#include <fstream>

#include "gen_graph_functions.hpp"
#include "utils.hpp"

std::vector<std::vector<bool>> gen_c5_free(size_t n, double d)
{
    std::random_device rd;
    std::mt19937 generator(rd());

    std::vector<std::vector<bool>> adj_mat(n, std::vector<bool>(n, false));

    std::map<size_t, std::vector<size_t>> adj_list;
    for (size_t i = 0; i < n; i++)
    {
        adj_list.emplace(i, std::vector<size_t>());
        adj_list[i].reserve(n);
    }

    // init valid pairs
    std::vector<size_t> valid_pairs;
    std::vector<size_t> invalid_pairs;
    valid_pairs.reserve(n * n / 2);
    invalid_pairs.reserve(n * n / 2);

    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = i + 1; j < n; j++)
        {
            valid_pairs.push_back(pair_index_to_upper_adj_matrix_flat_index(i, j));
        }
    }

    // if d > 0.5, generate and take the complement
    bool working_complement = false;
    if (d > 0.5) 
    {
        working_complement = true;
        d = 1-d;
    }

    size_t n_added_edges = 0;
    size_t n_target_edges = n * (n - 1) * d / 2;

    while (n_added_edges < n_target_edges)
    {
        // hard reset if valid pairs is empty
        if (valid_pairs.empty())
        {
           adj_mat = std::vector<std::vector<bool>>(n, std::vector<bool>(n, false));
           for (size_t i = 0; i < n; i++)
            {
                adj_list[i].clear();
            }
            invalid_pairs.clear();
            valid_pairs.clear();       
            for (size_t i = 0; i < n; i++)
            {
                for (size_t j = i + 1; j < n; j++)
                {
                    valid_pairs.push_back(pair_index_to_upper_adj_matrix_flat_index(i, j));
                }
            }
            n_added_edges = 0;
        }

        // random choose pair index
        size_t valid_pairs_size = valid_pairs.size();
        unsigned long long max_index = static_cast<unsigned long long>(valid_pairs_size - 1);
        std::uniform_int_distribution distribution(static_cast<unsigned long long>(0), max_index);

        unsigned long long random_vp_index = distribution(generator);
        size_t random_vp = valid_pairs[random_vp_index];
        auto [v1, v2] = upper_adj_matrix_flat_index_to_pair_index(random_vp);

        // remove flat_vp from valid_pairs
        valid_pairs[random_vp_index] = valid_pairs.back();
        valid_pairs.pop_back();

        // try to add the edge
        bool will_create_c5 = false;

        // check c5
        for (size_t u1 : adj_list[v1])
        {
            if (adj_mat[v2][u1]) continue;
            for (size_t u2 : adj_list[v2])
            {
                if (adj_mat[v1][u2] || adj_mat[u1][u2]) continue;
                for (size_t x: adj_list[u1])
                {
                    if (!adj_mat[x][u2] || adj_mat[x][v1] || adj_mat[x][v2]) continue;
                    will_create_c5 = true;
                    break;
                }
                if (will_create_c5) break;
            }
            if (will_create_c5) break;
        }

        if (!will_create_c5)
        {
            // add the edge
            adj_list[v1].push_back(v2);
            adj_list[v2].push_back(v1);
            adj_mat[v1][v2] = true;
            adj_mat[v2][v1] = true;
            n_added_edges++;

            // dump invalid pairs into valid pairs
            valid_pairs.insert(valid_pairs.end(), invalid_pairs.begin(), invalid_pairs.end());
            invalid_pairs.clear();
        }
        else
        {
            invalid_pairs.push_back(random_vp);
        }
    }

    if (working_complement) adj_mat = get_complement_of_graph(adj_mat);

    return adj_mat;
}

std::vector<std::vector<bool>> gen_perfect_via_operations(int n, double d, double eps, std::string folder_path)
{

    std::random_device rd;
    std::mt19937 gen(rd());

    // pick a random operation.
    // IF NEW OPERATION IS ADDED, UPDATE THIS DISTRIBUTION
    std::uniform_int_distribution<> dist_operations(0, 5);

    // iterate through the folder of g6 files and collect all graphs.
    // The logic can be modified if memory overflows
    std::vector<std::string> graph_pool;
    std::vector<std::filesystem::directory_entry> files;
    for (const auto &entry : std::filesystem::directory_iterator(folder_path))
    {
        auto ext = entry.path().extension();
        if (entry.is_regular_file() && (ext == ".g6" || ext == ".G6"))
        {
            std::ifstream file(entry.path());
            std::string line;

            while (std::getline(file, line))
            {
                if (line.empty()) continue;

                graph_pool.push_back(line);
            }
        }
    }
    std::uniform_int_distribution<> dist_pool(0, graph_pool.size() - 1);

    // choose a graph randomly (g6 string)
    auto graph = g6_to_adj_matrix(graph_pool[dist_pool(gen)]);

    int counter = 0;
    while (true)
    {
        operation_type random_operation = static_cast<operation_type>(dist_operations(gen));

        switch (random_operation)
        {
        case COMPLETE_JOIN:
        {
            auto graph_ext = g6_to_adj_matrix(graph_pool[dist_pool(gen)]);
            complete_join(graph, graph_ext);
            break;
        }
        case DISJOINT_UNION:
        {
            auto graph_ext = g6_to_adj_matrix(graph_pool[dist_pool(gen)]);
            disjoint_union(graph, graph_ext);
            break;
        }
        case OPERATION_SUBSTITUTION:
        {
            auto graph_ext = g6_to_adj_matrix(graph_pool[dist_pool(gen)]);
            operation_substitution(graph, graph_ext, gen);
            break;
        }
        case OPERATION_COMPOSITION:
        {
            auto graph_ext = g6_to_adj_matrix(graph_pool[dist_pool(gen)]);
            operation_composition(graph, graph_ext, gen);
            break;
        }
        case OPERATION_CLIQUE_ID:
        {
            auto graph_ext = g6_to_adj_matrix(graph_pool[dist_pool(gen)]);
            operation_clique_identification(graph, graph_ext, gen);
            break;
        }
        case COMPLEMENT:
        {
            graph = get_complement_of_graph(graph);
            break;
        }
        }

        if (graph.size() >= n)
        {

            // It is safe to remove any vertex, since perfectness is hereditary
            // Remove a random vertex until graph size is n
            while (graph.size() > n)
            {
                int n_current = graph.size();
                std::uniform_int_distribution<> dist_remove(0, n_current - 1);

                int vertex_to_removed = dist_remove(gen);

                for (int i = 0; i < n_current; i++)
                {
                    graph[i].erase(graph[i].begin() + vertex_to_removed);
                }
                graph.erase(graph.begin() + vertex_to_removed);
            }

            // Check if the result is close to wanted density
            double result_density = calculate_density(graph);
            double complement_density = 1 - result_density;
            if ((d - eps <= result_density) && (result_density <= d + eps))
            {
                break;
            }
            else if ((d - eps <= complement_density) && (complement_density <= d + eps))
            {
                graph = get_complement_of_graph(graph);
                break;
            }
            else
            {
                // if density is different, start again
                graph = g6_to_adj_matrix(graph_pool[dist_pool(gen)]);
            }
        }
        counter++;
    }

    return graph;
}

std::vector<std::vector<bool>> gen_perturb_vertex_pairs(const std::vector<std::vector<bool>> &base_graph, double vertex_pair_ratio)
{
    int n = base_graph.size();
    std::vector<uint64_t> vertex_pairs;
    for (int i = 0; i < (n - 1); i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            auto key = pack_pair(i, j);
            vertex_pairs.push_back(key);
        }
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(vertex_pairs.begin(), vertex_pairs.end(), g);

    int different_vp_count = static_cast<int>(vertex_pair_ratio * n * (n - 1) / 2);

    std::vector<uint64_t> picked(vertex_pairs.begin(), vertex_pairs.begin() + different_vp_count);

    auto output_graph = base_graph;
    for (const auto &vp : picked)
    {
        auto [i, j] = unpack_pair(vp);
        output_graph[i][j] = !output_graph[i][j];
        output_graph[j][i] = !output_graph[j][i];
    }

    return output_graph;
}

std::vector<std::vector<bool>> gen_threshold_graph(int n, double p)
// n is graph order, p is the probability that a vertex is universal to the vertices before
// amazingly expected density of the graph is p.
{
    std::vector<std::vector<bool>> adjacency_matrix(n, std::vector<bool>(n, false));
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    // A vector to hold if the added vertex is universal or not, at its addition.
    // A vertex is either universal or isolated to vertices with smaller index
    for (int i = 1; i < n; i++)
    {
        double random_value = distribution(generator);
        if (random_value <= p)
        {
            // vertex is universal
            for (int j = 0; j < i; j++)
            {
                adjacency_matrix[i][j] = true;
                adjacency_matrix[j][i] = true;
            }
        }
    }

    return adjacency_matrix;
}

std::vector<std::vector<bool>> gen_erdos_renyi(int n, double p)
{
    std::vector<std::vector<bool>> adjacency_matrix(n, std::vector<bool>(n, false));

    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            double random_value = distribution(generator);
            if (random_value <= p)
            {
                adjacency_matrix[i][j] = true;
                adjacency_matrix[j][i] = true;
            }
        }
    }

    return adjacency_matrix;
}

std::vector<std::vector<bool>> gen_graph_outer_sandwich(const std::vector<std::vector<bool>> &base_graph, double optional_edge_density)
{
    int order = base_graph.size();
    auto output_graph = base_graph;

    std::vector<uint64_t> non_edge_indices;
    // count nonedges in the input graph, also store their indices
    int n_non_edges = 0;
    for (int i = 0; i < order; i++)
    {
        for (int j = i + 1; j < order; j++)
        {
            if (!base_graph[i][j])
            {
                n_non_edges++;
                auto vp = pack_pair(i, j);
                non_edge_indices.push_back(vp);
            }
        }
    }
    // calculate number of edges to be added in the optional edges according to the completion dens.
    int n_edges_to_be_added = static_cast<int>(n_non_edges * optional_edge_density);

    // Select randomly n_edges_to_be_added many pairs among n_non_edges
    std::vector<int> indices_to_be_edges = select_k_without_replacement(n_non_edges, n_edges_to_be_added);

    // modify output graph
    for (const auto &pair_index : indices_to_be_edges)
    {
        auto vp = non_edge_indices[pair_index];
        auto [u, v] = unpack_pair(vp);
        output_graph[v][u] = true;
        output_graph[u][v] = true;
    }

    return output_graph;
}

std::vector<std::vector<bool>> gen_BA_with_given_order_and_density(int n, double d)
{
    // calculate corresponding BA parameter k
    double inside_sqrt = (2 * n - 1) * (2 * n - 1) - 4 * d * n * (n - 1);
    double k_double = ((2 * n - 1) - std::sqrt(inside_sqrt)) / 2;
    int k = static_cast<int>(std::round(k_double));

    std::vector<std::vector<bool>> adjacency_matrix(n, std::vector<bool>(n, false));

    // Random Number Generator
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    // Degrees init
    std::vector<int> degrees(n, 0);

    // Generation
    for (int i = 1; i < n; i++)
    {
        if (i <= k)
        {
            for (int j = 0; j < i; j++)
            {
                adjacency_matrix[i][j] = true;
                adjacency_matrix[j][i] = true;
                degrees[i]++;
                degrees[j]++;
            }
        }
        else
        {
            auto temp_degrees = degrees;
            for (int a = 0; a < k; a++)
            {
                double random_value = distribution(generator);
                int j = random_weighted_choice(temp_degrees, random_value);

                // Set degree of vertex j to 0 in temp_degrees s.t. it is not selected further.
                temp_degrees[j] = 0;

                adjacency_matrix[i][j] = true;
                adjacency_matrix[j][i] = true;
                degrees[i]++;
                degrees[j]++;
            }
        }
    }

    return adjacency_matrix;
}

std::vector<std::vector<bool>> embed_hole(const std::vector<std::vector<bool>> &input_graph, int k)
{
    auto output_graph = input_graph;
    int n = output_graph.size();

    // randomly choose k vertices
    auto indices = select_k_without_replacement(n, k);

    for (int i = 0; i < k; i++)
    {
        for (int j = i + 1; j < k; j++)
        {
            // modify to edge
            if (j == i + 1 || (i == 0 && j == k - 1))
            {
                output_graph[indices[i]][indices[j]] = true;
                output_graph[indices[j]][indices[i]] = true;
            }
            // modify to non-edge
            else
            {
                output_graph[indices[i]][indices[j]] = false;
                output_graph[indices[j]][indices[i]] = false;
            }
        }
    }

    return output_graph;
}
