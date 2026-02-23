// Author: Burak Nur Erdem

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "utils.hpp"

void print_graph(const std::vector<std::vector<bool>> &graph)
{
    for (const auto &row : graph)
    {
        for (const auto &entry : row)
        {
            std::cout << entry;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    return;
}

double expected_value_of_odd_holes(int n, double p)
{

    double expected_value = 0;

    for (int cycle_length = 5; cycle_length <= n; cycle_length += 2)
    {

        long double power_of_term_2 = (cycle_length * (((cycle_length - 1) / 2) - 1));
        // The code below is designed to do the term calculations without ending up with very big or
        // very small numbers
        long double term_total = 1;
        int t1 = n - cycle_length + 1; // partial factorial
        int t2 = 1;                    // power 1
        int t3 = 1;                    // power 2
        while (true)
        { // multiplying of the terms individually
            // term1: partial_factorial
            bool break_cond = true;
            if (t1 <= n)
            {
                term_total *= t1;
                t1++;
                break_cond = false;
            }
            if (t2 <= cycle_length)
            {
                term_total *= p;
                t2++;
                break_cond = false;
            }
            if (t3 <= power_of_term_2)
            {
                term_total *= (1 - p);
                t3++;
                break_cond = false;
            }
            if (break_cond)
            {
                break;
            }
        }
        expected_value += ((term_total / 2) / cycle_length);
    }
    return expected_value;
}

double expected_value_of_odd_anti_holes(int n, double p)
{

    double expected_value = 0;

    for (int cycle_length = 7; cycle_length <= n; cycle_length += 2)
    {

        long double power_of_term_2 = (cycle_length * (((cycle_length - 1) / 2) - 1));
        // The code below is designed to do the term calculations without ending up with very big or
        // very small numbers
        long double term_total = 1;
        int t1 = n - cycle_length + 1; // partial factorial
        int t2 = 1;                    // power 1
        int t3 = 1;                    // power 2
        while (true)
        { // multiplying of the terms individually
            // term1: partial_factorial
            bool break_cond = true;
            if (t1 <= n)
            {
                term_total *= t1;
                t1++;
                break_cond = false;
            }
            if (t2 <= cycle_length)
            {
                term_total *= (1 - p);
                t2++;
                break_cond = false;
            }
            if (t3 <= power_of_term_2)
            {
                term_total *= p;
                t3++;
                break_cond = false;
            }
            if (break_cond)
            {
                break;
            }
        }
        expected_value += ((term_total / 2) / cycle_length);
    }
    return expected_value;
}

bool is_graph_empty_or_complete(std::vector<std::vector<bool>> &graph)
{
    int graph_size = graph.size();

    bool is_complete = true;
    bool is_empty = true;
    // Checking only the upper triangular of adj. matrix
    for (int i = 0; i < graph_size - 1; i++)
    {
        for (int j = i + 1; j < graph_size; j++)
        {
            if (is_complete && !graph[i][j])
            {
                is_complete = false;
            }
            if (is_empty && graph[i][j])
            {
                is_empty = false;
            }
        }
    }
    if (is_complete || is_empty)
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::string cycle_vector_to_string(std::vector<int> vec)
{

    std::sort(vec.begin(), vec.end());

    std::string cycle_string;

    for (const auto &v : vec)
    {
        cycle_string.append(std::to_string(v) + "_");
    }

    return cycle_string;
}

std::string get_string_rep_of_vp(int v1, int v2)
{
    std::string vp_str;
    if (v1 < v2)
    {
        vp_str = std::to_string(v1) + "_" + std::to_string(v2);
    }
    else
    {
        vp_str = std::to_string(v2) + "_" + std::to_string(v1);
    }
    return vp_str;
}

std::pair<int, int> get_two_integers_in_vp_string(std::string input)
{

    size_t underscorePos = input.find("_");

    std::string s1 = input.substr(0, underscorePos);
    std::string s2 = input.substr(underscorePos + 1);

    int v1 = std::stoi(s1);
    int v2 = std::stoi(s2);

    if (v1 < v2)
    {
        return std::pair<int, int>(v1, v2);
    }
    else
    {
        return std::pair<int, int>(v2, v1);
    }
}

std::vector<std::vector<bool>> get_complement_of_graph(const std::vector<std::vector<bool>> &adj_mat)
{

    int n = adj_mat.size();
    std::vector<std::vector<bool>> complement_graph(n, std::vector<bool>(n, false));

    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (!adj_mat[i][j] && i != j)
            {
                complement_graph[i][j] = 1;
                complement_graph[j][i] = 1;
            }
        }
    }

    return complement_graph;
}

std::vector<std::vector<bool>> read_graph_adj_matrix_from_txt_file(std::string graph_file_name)
{
    std::ifstream graph_file(graph_file_name);
    std::vector<std::vector<bool>> graph;

    if (!graph_file)
    {
        std::cerr << "Unable to open graph file!" << std::endl;
        return graph;
    }

    std::string line;
    while (std::getline(graph_file, line))
    {
        std::vector<bool> row;
        for (const char &c : line)
        {
            if (c == '0' || c == '1')
            {
                row.push_back(c == '1');
            }
        }
        if(!row.empty())
        {
            graph.push_back(row);
        }
    }

    graph_file.close();

    return graph;
}

double calculate_density(const std::vector<std::vector<bool>> &graph)
{

    if (graph.empty())
    {
        std::cerr << "Warning: Graph empty in calculate_density()" << std::endl;
    }

    int edge_count = 0;
    int n = graph.size();
    int vp = n * (n - 1) / 2;

    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (graph[i][j])
            {
                edge_count++;
            }
        }
    }

    double density = static_cast<double>(edge_count) / vp;
    return density;
}

std::vector<std::vector<int>> get_adj_list_from_adj_matrix(const std::vector<std::vector<bool>> &graph)
{
    int n = graph.size();
    std::vector<std::vector<int>> graph_adj_list(n);

    for (int i = 0; i < n; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            if (graph[i][j])
            {
                graph_adj_list[i].push_back(j);
                graph_adj_list[j].push_back(i);
            }
        }
    }
    return graph_adj_list;
}

void print_odd_holes(const std::unordered_map<std::string, std::vector<int>> &odd_holes)
{
    std::cout << "Number of odd holes: " << odd_holes.size() << std::endl;
    if (!odd_holes.empty())
    {
        std::cout << "Printing odd-holes: " << std::endl;
        for (const auto &[key, cycle] : odd_holes)
        {
            for (const auto &v : cycle)
            {
                std::cout << v << ", ";
            }
            std::cout << std::endl;
        }
    }
    return;
}

int calculate_number_of_changes(
    const std::vector<std::vector<bool>> &g1, const std::vector<std::vector<bool>> &g2
)
{
    int n1 = g1.size();
    int n2 = g2.size();
    if (n1 != n2)
    {
        std::cerr << "Two graphs are not the same order in calculate_number_of_changes()!" << std::endl;
    }

    int changes = 0;
    for (int i = 0; i < n1; i++)
    {
        for (int j = i + 1; j < n1; j++)
        {
            if (g1[i][j] != g2[i][j])
            {
                changes++;
            }
        }
    }
    return changes;
}

void remove_entries_containing_vp_from_odd_holes(
    std::unordered_map<std::string, std::vector<int>> &odd_holes, int v1, int v2
)
{
    for (auto it = odd_holes.begin(); it != odd_holes.end();)
    {
        const std::vector<int> &values = it->second;
        if (std::find(values.begin(), values.end(), v1) != values.end() &&
            std::find(values.begin(), values.end(), v2) != values.end())
        {
            it = odd_holes.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

bool parse_graph_filename(const std::string &filepath, graph_file_info &graph_file_info_obj)
{

    size_t last_slash = filepath.find_last_of("/\\");
    std::string filename = (last_slash == std::string::npos) ? filepath : filepath.substr(last_slash + 1);

    if (filename.size() < 4 || filename.substr(filename.size() - 4) != ".txt")
        return false;
    std::string base = filename.substr(0, filename.size() - 4);

    size_t last_underscore = base.rfind('_');
    if (last_underscore == std::string::npos)
        return false;
    graph_file_info_obj.id = base.substr(last_underscore + 1);

    base = base.substr(0, last_underscore);
    last_underscore = base.rfind('_');
    if (last_underscore == std::string::npos)
        return false;
    graph_file_info_obj.density = base.substr(last_underscore + 1);

    base = base.substr(0, last_underscore);
    last_underscore = base.rfind('_');
    if (last_underscore == std::string::npos)
        return false;
    graph_file_info_obj.order = base.substr(last_underscore + 1);

    base = base.substr(0, last_underscore);
    last_underscore = base.rfind('_');
    if (last_underscore == std::string::npos)
        return false;
    graph_file_info_obj.type = base.substr(last_underscore + 1);

    return true;
}

bool is_matrix_symmetric(const std::vector<std::vector<bool>> &mat)
{
    int matrix_len = mat.size();
    for (const auto &row : mat)
    {
        int row_len = row.size();
        if (row_len != matrix_len)
        {
            return false;
        }
    }

    for (int i = 0; i < matrix_len; i++)
    {
        for (int j = 0; j < matrix_len; j++)
        {
            if (mat[i][j] != mat[j][i])
            {
                return false;
            }
        }
    }

    return true;
}

std::vector<int>
find_a_maximal_clique_including_a_given_vertex(const std::vector<std::vector<bool>> &graph, int v)
{
    std::vector<int> clique = {v};
    int n = graph.size();

    bool added = true;
    while (added)
    {
        added = false;
        for (int i = 0; i < n; i++)
        {
            // if i in clique, pass i
            if (std::find(clique.begin(), clique.end(), i) != clique.end())
            {
                continue;
            }
            // check if i adjacent to all vertices in the clique
            bool adjacent_to_all = true;
            for (int u : clique)
            {
                if (!graph[i][u])
                {
                    adjacent_to_all = false;
                    break;
                }
            }
            // if i adjacent to all and i not in clique, add i
            if (adjacent_to_all && std::find(clique.begin(), clique.end(), i) == clique.end())
            {
                clique.push_back(i);
                added = true;
                break;
            }
        }
    }

    return clique;
}

std::string create_output_adj_matrix_file_name(
    std::vector<std::vector<bool>> &graph,
    std::string graph_folder,
    std::string type,
    int order,
    std::string density_dec_str,
    int id
)
{
    int target_length = 5;

    std::ostringstream oss_n, oss_id;
    oss_n << std::setw(5) << std::setfill('0') << order;
    oss_id << std::setw(5) << std::setfill('0') << id;

    std::string n_str = oss_n.str();
    std::string id_str = oss_id.str();
    if (density_dec_str.length() < target_length)
    {
        density_dec_str.append(target_length - density_dec_str.length(), '0');
    }

    std::string name = graph_folder + "/graph_" + type + "_" + n_str + "_" + density_dec_str + "_" + id_str + ".txt";
    
    return name;
}

void write_adj_matrix(std::vector<std::vector<bool>> &graph, std::string output_file_name)
{
    int order = graph.size();
    std::ofstream graph_file(output_file_name);
    for (int i = 0; i < order; ++i)
    {
        for (int j = 0; j < order; ++j)
        {
            graph_file << graph[i][j];
            if (j == order - 1)
            {
                graph_file << std::endl;
            }
        }
    }
    graph_file.close();
    return;
}

void remove_vertex_from_graph(std::vector<std::vector<bool>> &graph, int v)
{
    int n = graph.size();
    for (int i = 0; i < n; i++)
    {
        graph[i].erase(graph[i].begin() + v);
    }
    graph.erase(graph.begin() + v);
    return;
}

std::vector<int> select_k_without_replacement(int n, int k)
{

    if (k > n)
    {
        throw std::invalid_argument("k cannot be greater than the array size");
    }

    // Create a vector of integers from 0 to n
    std::vector<int> indices(n, 0);
    for (int i = 0; i < n; i++)
    {
        indices[i] = i;
    }

    // Shuffle the vector
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(indices.begin(), indices.end(), gen);

    // Return the first k elements
    return std::vector<int>(indices.begin(), indices.begin() + k);
}

int random_weighted_choice(const std::vector<int> &weights, double random_value)
{
    int sum = 0;
    int len = weights.size();

    for (int i = 0; i < len; i++)
    {
        sum += weights[i];
        if (weights[i] < 0)
        {
            std::cerr << "Error: Negative weight in random_weighted_choice." << std::endl;
        }
    }

    // Scale random_value
    double scaled_random = random_value * sum;

    for (int i = 0; i < len; i++)
    {
        if (scaled_random < weights[i])
        {
            return i;
        }
        else
        {
            scaled_random -= weights[i];
        }
    }

    std::cerr << "Error: Problem in random_weighted_choice." << std::endl;
    return -1;
}

int select_random_integer(int n)
{
    // Return an integer choosen uniformly random in the interval [0,n-1]
    
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, n - 1);
    int random_int = distribution(generator);
    return random_int;
}

uint64_t select_random_vertex_pair_packed(int n)
{
    // n is the graph order. choose u and v with rejection sampling
    
    if(n < 2)
    {
        std::cerr << "Error: n should be >= 2 in select_random_vertex_pair.\n";
    }

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, n - 1);
    int u = distribution(generator);
    int v;
    do {
        v = distribution(generator);
    } while (u == v);

    // make pair in data type uint64_t
    auto vp = pack_pair(u, v);
    return vp;
}

std::pair<int, int> select_random_vertex_pair_unpacked(int n)
{
    // n is the graph order. choose u and v with rejection sampling
    
    if(n < 2)
    {
        std::cerr << "Error: n should be >= 2 in select_random_vertex_pair.\n";
    }

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, n - 1);
    int u = distribution(generator);
    int v;
    do {
        v = distribution(generator);
    } while (u == v);

    return {u, v};
}


void complete_join(std::vector<std::vector<bool>> &g1, const std::vector<std::vector<bool>> &g2)
{
    int n1 = g1.size();
    int n2 = g2.size();

    for (int i = 0; i < n1; i++)
    {
        for (int j = 0; j < n2; j++)
        {
            g1[i].push_back(true);
        }
    }

    for (int j = 0; j < n2; j++)
    {
        std::vector<bool> temp_vect(n1 + n2, true);
        for (int i = 0; i < n2; i++)
        {
            if (!g2[i][j])
            {
                temp_vect[n1 + i] = false;
            }
        }
        g1.push_back(temp_vect);
    }

    return;
}

void disjoint_union(std::vector<std::vector<bool>> &g1, const std::vector<std::vector<bool>> &g2)
{
    int n1 = g1.size();
    int n2 = g2.size();

    for (int i = 0; i < n1; i++)
    {
        for (int j = 0; j < n2; j++)
        {
            g1[i].push_back(false);
        }
    }

    for (int j = 0; j < n2; j++)
    {
        std::vector<bool> temp_vect(n1 + n2, false);
        for (int i = 0; i < n2; i++)
        {
            if (g2[i][j])
            {
                temp_vect[n1 + i] = true;
            }
        }
        g1.push_back(temp_vect);
    }

    return;
}

void operation_substitution(
    std::vector<std::vector<bool>> &g1, const std::vector<std::vector<bool>> &g2, std::mt19937 &gen
)
{
    int n1 = g1.size();
    int n2 = g2.size();

    std::uniform_int_distribution<> distrib(0, n1 - 1);

    // random vertex of v
    int v = distrib(gen);

    // neighbors of v
    std::vector<int> neighbors_v;
    for (int i = 0; i < n1; i++)
    {
        if (g1[v][i])
        {
            neighbors_v.push_back(i);
        }
    }

    // firstly, use disjoint union to merge adjacency matrices:
    disjoint_union(g1, g2);

    // create adjacencies between vertices of g2 and neighbors_v
    for (int i = n1; i < (n1 + n2); i++)
    {
        for (int u : neighbors_v)
        {
            g1[i][u] = true;
            g1[u][i] = true;
        }
    }

    // remove v from g1
    for (int i = 0; i < (n1 + n2); i++)
    {
        g1[i].erase(g1[i].begin() + v);
    }
    g1.erase(g1.begin() + v);

    return;
}

void operation_composition(
    std::vector<std::vector<bool>> &g1, const std::vector<std::vector<bool>> &g2, std::mt19937 &gen
)
{
    int n1 = g1.size();
    int n2 = g2.size();

    if (n1 < 3 || n2 < 3)
    {
        return;
    }

    std::uniform_int_distribution<> distrib1(0, n1 - 1);
    std::uniform_int_distribution<> distrib2(0, n2 - 1);

    // random vertices from g1 and g2
    int v1 = distrib1(gen);
    int v2 = distrib2(gen);

    // neighbors of v1 and v2
    std::vector<int> neighbors_v1;
    std::vector<int> neighbors_v2;
    for (int i = 0; i < n1; i++)
    {
        if (g1[v1][i])
        {
            neighbors_v1.push_back(i);
        }
    }
    for (int i = 0; i < n2; i++)
    {
        if (g2[v2][i])
        {
            // here we add n1 to the vertex label since the two graphs will be
            // merged later and vertex labels (indices) of g2 will increase as much as n1
            neighbors_v2.push_back(i + n1);
        }
    }

    // first use disjoint union to merge adjacency matrices:
    disjoint_union(g1, g2);

    // create adjacencies between vertices of neighbors_v1 and neighbors_v2
    for (int u1 : neighbors_v1)
    {
        for (int u2 : neighbors_v2)
        {
            g1[u1][u2] = true;
            g1[u2][u1] = true;
        }
    }

    // remove v1 and v2 from g2. new label of v2 in g1 is (v2+n1)
    //  first remove v2 since (v2+n1) > v1
    for (int i = 0; i < (n1 + n2); i++)
    {
        g1[i].erase(g1[i].begin() + v2 + n1);
    }
    g1.erase(g1.begin() + v2 + n1);
    for (int i = 0; i < (n1 + n2 - 1); i++)
    {
        g1[i].erase(g1[i].begin() + v1);
    }
    g1.erase(g1.begin() + v1);
}

void operation_clique_identification(
    std::vector<std::vector<bool>> &g1, const std::vector<std::vector<bool>> &g2, std::mt19937 &gen
)
{
    int n1 = g1.size();
    int n2 = g2.size();

    std::uniform_int_distribution<> distrib1(0, n1 - 1);
    std::uniform_int_distribution<> distrib2(0, n2 - 1);

    // random vertices from g1 and g2
    int v1 = distrib1(gen);
    int v2 = distrib2(gen);

    // find maximal cliques including the selected vertices
    auto clique1 = find_a_maximal_clique_including_a_given_vertex(g1, v1);
    auto clique2 = find_a_maximal_clique_including_a_given_vertex(g2, v2);

    // find the bigger clique, shuffle it, downsize it to the clique of the lesser size
    int c1_size = clique1.size();
    int c2_size = clique2.size();

    if (c1_size == 0 || c2_size == 0)
    {
        std::cerr << "Clique size 0!" << std::endl;
    }

    int new_clique_size;
    if (c1_size >= c2_size)
    {
        std::shuffle(clique1.begin(), clique1.end(), gen);
        clique1.resize(c2_size);
        new_clique_size = c2_size;
    }
    else
    {
        std::shuffle(clique2.begin(), clique2.end(), gen);
        clique2.resize(c1_size);
        new_clique_size = c1_size;
    }

    // disjoint union to merge adjacency matrices:
    disjoint_union(g1, g2);

    // readjust the labels of clique2
    for (int i = 0; i < new_clique_size; i++)
    {
        clique2[i] += n1;
    }

    // a one to one mapping is carried out on indices.
    // since a clique is shuffled, there is no bias towards smaller indices
    // connect a clique1's vertex to neighbors of corresponding vertex in clique2
    for (int i = 0; i < new_clique_size; i++)
    {
        int u1 = clique1[i];
        int u2 = clique2[i];

        // find neighbors of u2, connect them to u1
        for (int j = n1; j < (n1 + n2); j++)
        {
            if (g1[j][u2])
            {
                g1[j][u1] = true;
                g1[u1][j] = true;
            }
        }
    }

    // remove clique2 from g1. first order decreasing
    // then iteratively remove the last vertex in clique2
    std::sort(clique2.begin(), clique2.end());
    for (int j = new_clique_size - 1; j >= 0; j--)
    {
        int vertex_to_remove = clique2[j];
        clique2.pop_back();

        // remove vertex_to_remove from g1
        for (int i = 0; i < (n1 + n2 - new_clique_size + j + 1); i++)
        {
            g1[i].erase(g1[i].begin() + vertex_to_remove);
        }
        g1.erase(g1.begin() + vertex_to_remove);
    }

    return;
}

std::vector<uint8_t> adj_matrix_to_g6(const std::vector<std::vector<bool>> &graph)
{
    // The algorithm is explained in:
    // https://users.cecs.anu.edu.au/~bdm/data/formats.txt

    unsigned long n = graph.size();
    if (n > 68719476735)
    {
        std::cerr << "Graph order above 68719476735.\n";
        std::exit(1);
    }

    std::vector<uint8_t> g6;

    // Reserving space for the vector
    g6.reserve((n * (n - 1) / 12) + 5);

    // Pushing the head chars that holds the graph order
    if (n <= 62)
    {
        g6.push_back(n + 63);
    }
    else if (n <= 258047)
    {
        g6.push_back(126);
        g6.push_back((0b111111 & (n >> 12)) + 63);
        g6.push_back((0b111111 & (n >> 6)) + 63);
        g6.push_back((0b111111 & n) + 63);
    }
    else if (n <= 68719476735)
    {
        g6.push_back(126);
        g6.push_back(126);
        g6.push_back((0b111111 & (n >> 30)) + 63);
        g6.push_back((0b111111 & (n >> 24)) + 63);
        g6.push_back((0b111111 & (n >> 18)) + 63);
        g6.push_back((0b111111 & (n >> 12)) + 63);
        g6.push_back((0b111111 & (n >> 6)) + 63);
        g6.push_back((0b111111 & n) + 63);
    }

    // adj mat to chars
    uint8_t counter_6bit = 0;
    uint8_t one_char = 0;
    for (unsigned long j = 1; j < n; j++)
    {
        for (unsigned long i = 0; i < j; i++)
        {
            if (graph[i][j])
                one_char |= 1;
            counter_6bit++;

            if (counter_6bit == 6)
            {
                // std::cout << std::bitset<6>(one_char) << '\n';
                g6.push_back(one_char + 63);
                one_char = 0;
                counter_6bit = 0;
            }
            else
            {
                one_char <<= 1;
            }
        }
    }
    // Last char
    if (counter_6bit != 0)
    {
        one_char <<= (6 - counter_6bit - 1);
        g6.push_back(one_char + 63);
    }

    return g6;
}

void write_g6(std::vector<uint8_t>& g6_vector, std::string file_path)
{
    std::ofstream output_g6_file;
    
    try
    {
        output_g6_file.open(file_path, std::ios::app);
    }
    catch(...)
    {
        std::cerr << "Error when opening output g6 file." << std::endl;
        return;
    }

    output_g6_file.write(reinterpret_cast<const char *>(g6_vector.data()), g6_vector.size());
    output_g6_file.put('\n');


    output_g6_file.close();

    return;
}

std::vector<std::vector<bool>> g6_to_adj_matrix(std::string &g6_string)
{
    if (g6_string.empty())
    {
        std::cerr << "Warning: g6_string is empty." << std::endl;
        return std::vector<std::vector<bool>>();
    }

    size_t g6_str_len = g6_string.length();
    size_t n = 0; // graph order
    size_t current_char_index = 1;


    // Extract the order
    // if n <= 62
    if (g6_string[0] < 126)
    {
        n = g6_string[0] - 63;
    }
    // if 62 < n <= 258047
    else if (g6_string[1] < 126)
    {
        n = ((size_t)(g6_string[1] - 63) << 12) |
            ((size_t)(g6_string[2] - 63) << 6) |
            ((size_t)(g6_string[3] - 63));

        current_char_index = 4;
    }
    // if 258047 < n <= 68719476735
    else
    {
        n = ((size_t)(g6_string[2] - 63) << 30) |
            ((size_t)(g6_string[3] - 63) << 24) |
            ((size_t)(g6_string[4] - 63) << 18) |
            ((size_t)(g6_string[5] - 63) << 12) |
            ((size_t)(g6_string[6] - 63) << 6) |
            (size_t)(g6_string[7] - 63);

        current_char_index = 8;
    }

    // initialize empty graph
    std::vector<std::vector<bool>> graph = std::vector<std::vector<bool>>(n, std::vector<bool>(n, false));

    // vertex indices 
    size_t i = 0;
    size_t j = 1;
    while (current_char_index < g6_str_len)
    {
        u_int8_t curr_bin = g6_string[current_char_index] - 63;
        for (int k = 5; k >= 0; k--)
        {
            bool v = 1 & (curr_bin >> k);
            // Add Edge
            if (v)
            {
                graph[i][j] = true;
                graph[j][i] = true;
            }
            // increment i,j
            i++;
            if (i == j) // move to new column if on diagonal
            {
                i = 0;
                j++;
                if(j >= n) break; // termination for last char
            }
        }
        if (j >= n) break; // termination for last char

        current_char_index++;
    }
    return graph;
}

std::string get_random_line_from_g6_file(std::string path, std::mt19937 &gen)
{
    std::ifstream file(path);
    std::string line;
    std::string result;
    size_t count = 0;

    while (std::getline(file, line))
    {
        if (line.empty()) continue;
        std::uniform_int_distribution<> dist(0, count);
        if (dist(gen) == 0) result = line;
        count++;
    }

    if (count == 0) std::cerr << "Error: g6 file empty: " << path << std::endl;

    return result;
}