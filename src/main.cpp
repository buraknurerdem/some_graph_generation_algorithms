// Author: Burak Nur Erdem

#include <cmath>
#include <format>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>
#include <filesystem>
#include <getopt.h>

#include "iterative_modification_heuristic.hpp"
#include "utils.hpp"
#include "gen_graph_functions.hpp"
#include "find_odd_holes.hpp"

int main(int argc, char *argv[])
{
    
    // Initialize all program parameters
    std::string type = "";
    std::string output_format = "";
    int order = 0;
    std::string density_dec_str = "";
    double density = 0.0;
    int id = 0;
    std::string output_adj_mat_file_path = "";
    std::string output_g6_file_path = "";
    std::string input_file_path = "";
    std::string input_g6 = "";
    std::string perfect_pool_folder_path = "";
    double epsilon = 0.0;
    std::string force_nonperfect = "";
    double vertex_pair_ratio = 0.0;
    int int_param1 = 0; 


    // Options table
    static struct option long_options[] = {
        {"type",                 required_argument, 0, 't'},
        {"format",               required_argument, 0, 'f'},
        {"order",                required_argument, 0, 'n'},
        {"density",              required_argument, 0, 'd'},
        {"id",                   required_argument, 0, 'i'},
        {"outputadjfilepath",    required_argument, 0, 'o'},
        {"outputg6filepath",     required_argument, 0, 'g'},
        {"epsilon",              required_argument, 0, 'e'},
        {"inputfilepath",        required_argument, 0, 1001},
        {"perfectpool",          required_argument, 0, 1002},
        {"forcenonperfect",      required_argument, 0, 1003},
        {"vertexpairratio",      required_argument, 0, 1004},
        {"intparam1",            required_argument, 0, 1005},
        {"inputg6",              required_argument, 0, 1006},
        {0,0,0,0}
    };

    int opt;
    int option_index = 0;

    while(true)
    {
        opt = getopt_long(argc, argv, "t:f:n:d:i:o:g:e:", long_options, &option_index);

        if(opt == -1) break;

        switch (opt)
        {
        case 't':
            type = optarg;
            break;
        case 'f':
            output_format = optarg;
            break;
        case 'n':
            order = std::stoi(optarg);
            break;
        case 'd':
            density_dec_str = optarg;
            density = std::stod("0." + std::string(density_dec_str));
            break;
        case 'i':
            id = std::stoi(optarg);
            break;
        case 'o':
            output_adj_mat_file_path = optarg;
            break;
        case 'g':
            output_g6_file_path = optarg;
            break;
        case 'e':
            epsilon = std::stod(optarg);
            break;
        case 1001:
            input_file_path = optarg;
            break;
        case 1002:
            perfect_pool_folder_path = optarg;
            break;
        case 1003:
            force_nonperfect = optarg;
            break;
        case 1004:
            vertex_pair_ratio = std::stod(optarg);
            break;
        case 1005:
            int_param1 = std::stoi(optarg);
            break;
        case 1006:
            input_g6 = optarg;
            break;

        default:
            break;
        }
    }

    // Declare graph
    std::vector<std::vector<bool>> input_graph;
    std::vector<std::vector<bool>> output_graph;

    // Read input graph if relevant
    if (input_g6 != "")
    {
        input_graph = g6_to_adj_matrix(input_g6);
    }
    else if (input_file_path != "")
    {
        input_graph = read_graph_adj_matrix_from_txt_file(input_file_path);
    }
    // Control input graph for relevant types
    std::vector<std::string> types_that_need_input = {"perturb-vertex-pairs", "IMH",
        "sandwich-outer", "embed-hole", "IMH-near-perfect", "c5-free"};
    if (std::find(types_that_need_input.begin(), types_that_need_input.end(), type) != types_that_need_input.end())
    {
        if (input_graph.empty())
        {
            std::cerr << "Error: Couldn't read input graph, exiting." << std::endl;
            return 1;
        }
    }

    // Generation based on type
    if (type == "erdos-renyi")
    {
        output_graph = gen_erdos_renyi(order, density);
    }
    else if (type == "barabasi-albert")
    {
        output_graph = gen_BA_with_given_order_and_density(order, density);
    }
    else if (type == "threshold")
    {
        output_graph = gen_threshold_graph(order, density);
    }
    else if (type == "perfect-operations")
    {
        output_graph = gen_perfect_via_operations(order, density, epsilon, perfect_pool_folder_path);
    }
    else if (type == "c5-free")
    {
        if (force_nonperfect == "")
        {
            std::cerr << "Warning: Parameter FORCE_NONPERFECT is empty." << std::endl;
        }

        output_graph = gen_c5_free(order, density);
        // If forcing non-perfect, we ensure that graph contains some odd holes or odd antiholes
        while (force_nonperfect == "TRUE")
        {
            auto oh = find_odd_holes(output_graph, false, 0);
            auto complement = get_complement_of_graph(output_graph);
            auto oah = find_odd_holes(complement, true, 0);
            if (oh.size() + oah.size() > 0) break;
            output_graph = gen_c5_free(order, density);
        }
    }
    else if (type == "perturb-vertex-pairs")
    {
        output_graph = gen_perturb_vertex_pairs(input_graph, vertex_pair_ratio);
    }
    else if (type == "IMH")
    {
        heuristic_stats heuristic_stats_obj = iterative_modification_heuristic(input_graph);
        if (heuristic_stats_obj.is_successful)
        {
            output_graph = heuristic_stats_obj.output_graph;
        }
        else
        {
            std::cerr << "IMH failed." << std::endl;
            return 1;
        }
    }
    else if (type == "IMH-near-perfect")
    {
        if(int_param1 <= 0)
        {
            std::cerr << "Error: INT_PARAM1 should be at least 0 when TYPE=IMH-near-perfect." << std::endl;
            return 1;
        }

        heuristic_stats heuristic_stats_obj = iterative_modification_heuristic_near_perfect(input_graph, int_param1);
        if (heuristic_stats_obj.is_successful)
        {
            output_graph = heuristic_stats_obj.output_graph;
        }
        else
        {
            std::cerr << "IMH_near_perfect failed." << std::endl;
            return 1;
        }
    }
    else if (type == "sandwich-outer")
    {
        
        output_graph = gen_graph_outer_sandwich(input_graph, vertex_pair_ratio);
    }
    else if (type == "embed-hole")
    {
        if(int_param1 <= 3)
        {
            std::cerr << "Error: INT_PARAM1 should be at least 4 when TYPE=embed-hole." << std::endl;
            return 1;
        }
        output_graph = embed_hole(input_graph, int_param1);
    }
    else
    {
        std::cerr << "The type: << " << type 
            << " >> does not exist among defined graph generation types." << std::endl;
        return 1;
    }

    // Control output_graph
    if (output_graph.empty())
    {
        std::cerr << "Warning: Generated graph is empty." << std::endl;
    }
    else if (calculate_density(output_graph) < 1e-6)
    {
        std::cerr << "Warning: Generated graph has density less than 1e-6." << std::endl;
    }

    // Writing Output
    // Possible formats: "g6", "adj_matrices", "g6_and_adj_matrices"
    bool output_adj_mat = false;
    bool output_g6 = false;

    if (output_format == "g6" || output_format == "g6_and_adj_matrices")
    {
        output_g6 = true;
    }
    if (output_format == "adj_matrices" || output_format == "g6_and_adj_matrices")
    {
        output_adj_mat = true;
    }

    // Write the output_graph
    if (output_adj_mat)
    {
        if (output_adj_mat_file_path == "")
        {
            std::cerr << "Warning: output_adj_mat_file_path is empty. Could not write adjacency matrix." << std::endl;
        }
        else
        {
            write_adj_matrix(output_graph, output_adj_mat_file_path);
        }
    }
    if (output_g6)
    {
        if (output_g6_file_path == "")
        {
            std::cerr << "Warning: output_g6_file_path is empty. Could not write g6." << std::endl;
        }
        else
        {
            std::vector<uint8_t> g6_vector = adj_matrix_to_g6(output_graph);
            write_g6(g6_vector, output_g6_file_path);
        }
    }

    return 0;
}