#include <bitset>
#include <iostream>
#include <map>
#include <random>

#include "gen_graph_functions.hpp"
#include "utils.hpp"

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