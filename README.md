# Introduction

This `c++` program provides a framework for generating graphs of various types, designed to produce instances and datasets for algorithmic benchmarking and experimenting.

The available [graph types](#graph-types) and their details are presented below.

The generation script is originally designed to work on Linux and MacOS. It requires some tweaks to run on Windows, especially with the file paths.

# How to use the program

## 1. Downloading the program

Download or clone this repository to your machine. You may download directly from Github webpage: Code > Code > Download ZIP. Then extract the archived file. Or, you may clone the repository using git.

```
git clone https://github.com/buraknurerdem/some_graph_generation_algorithms
```

## 2. Compiling

The program needs to be compiled. Any `c++` compiler should work. I use `clang++` with the following command. Make sure the working directory is the project folder before running the command.

```
mkdir -p bin && clang++ -std=c++20 -O3 -o bin/gen_graph -Iinclude src/*.cpp
```

## 3. Running the program

After the program is compiled, it is ready to be used. The most convenient way is to customize and run the bash script `./scripts/run_gen_graph.sh`. You may set all the necessary parameters for the program is the script and run it using the following command.

```
./scripts/run_gen_graph.sh
````

There are two options for the format of the graphs.

- Adjacency matrix in a text file (.txt)
- g6 format (.g6) For details regarding the g6 format, see [this page](https://users.cecs.anu.edu.au/~bdm/data/formats.txt).

# Program Parameters

The program requires various parameters as input. These are defined in `./scripts/run_gen_graph.sh`. You need to change the variables according your intentions. Some of these parameters are specific to the graph type while some are generic. Generic parameters are explained in this section, while the specific parameters are explained under [Graph Types](#graph-types). Beware that invalid parameter inputs may result in unexpected behavior.

### `OUTPUT_FORMAT`

Defines the format of the output graphs. Possible values are "g6", "adj_matrices", "g6_and_adj_matrices".

### `OUTPUTS_ADJ_MAT_DIR`

Defines the folder path which the adjacency matrices will be created in. Relevant only when adjacency matrix is set in OUTPUT_FORMAT.

### `OUTPUTS_G6_FILE_PATH`

Defines the g6 file path which the graphs will be written to. The path should end with ".g6" indicating the g6 format. Each line in a ".g6" file corresponds to a graph. The parameter is relevant only when g6 is set in OUTPUT_FORMAT.

### `TYPE`

This is the graph type to be generated. Possible values are "erdos-renyi", "barabasi-albert", "threshold", "perfect-operations", "perturb-vertex-pairs", "IMH", "sandwich-outer", "embed-hole", "IMH-near-perfect", "c5-free". Each type is explained below.

# Graph Types

The main difference between graph types is that some graph types are generated from scratch (e.g. erdos-renyi) while others require existing graphs to build from, e.g. perturb-vertex-pairs. For the graphs that are created from scratch, three loops are carried over `ORDERS`, `DENSITIES` and `REPETITION`. For each order and density pair `REPETITION` many graphs are generated. 

For graph types that require an input, the loop is carried over some inputs graphs. This is either a directory of adjacency matrices or a ".g6" file.


## erdos-renyi

An Erdös-Renyi graph with order $n$ and a probability $p$ of having an edge between two vertex pairs. The graph density is equal to $p$ on average. Therefore, a density given as parameter is tak

Parameters:
- `ORDERS`: The vector of graph orders. E.g. `ORDERS=(10 20 30)`.
- `DENSITIES`: The vector of densities after the decimal point. E.g. `DENSITIES=(25 50 75)`.
- `REPETITION`: The number of graphs generated for each order and density value. E.g. `REPETITION=5`

## barabasi-albert

A Barabasi-Albert graph. Usually, this model takes a parameter $k$ stating how many neighbors a vertex will have at the time it is introduced. Here, graph density is a given as a parameter instead. The value $k$ is then calculated according to the wanted density. Until there are $k+1$ vertices in the graph, vertex added at an iteration is adjacent to all previous vertices. In other words, the initial graph is a complete graph with $k+1$ vertices.

Parameters:
- `ORDERS`: The vector of graph orders. E.g. `ORDERS=(10 20 30)`.
- `DENSITIES`: The vector of densities after the decimal point. E.g. `DENSITIES=(25 50 75)`.
- `REPETITION`: The number of graphs generated for each order and density value. E.g. `REPETITION=5`

## threshold

A threshold graph. Starting with a 1 vertex graph, at each iteration either a universal (dominating) vertex or an isolated vertex is added. This is repeated until the input graph order is achieved. On average, the graph density equals to the probability of the new vertex being a universal vertex.

Parameters:
- `ORDERS`: The vector of graph orders. E.g. `ORDERS=(10 20 30)`.
- `DENSITIES`: The vector of densities after the decimal point. E.g. `DENSITIES=(25 50 75)`.
- `REPETITION`: The number of graphs generated for each order and density value. E.g. `REPETITION=5`

## perfect-operations

This algorithm generates perfect graphs with the method explained in [[1]](https://doi.org/10.1016/j.ejor.2020.09.017). The idea is based on utilizing some graph combination operations where the perfectness are preserved. Therefore, this method expects an initial pool of perfect graphs. A very good candidate is the all perfect graphs up to order 10 [provided here by Brendan McKay](https://users.cecs.anu.edu.au/~bdm/data/graphs.html). To use these, you need to download the .g6 files into a folder and provide the path of the folder to the `PERFECT_POOL` parameter.

Parameters:
- `ORDERS`: The vector of graph orders. E.g. `ORDERS=(10 20 30)`.
- `DENSITIES`: The vector of densities after the decimal point. E.g. `DENSITIES=(25 50 75)`.
- `REPETITION`: The number of graphs generated for each order and density value. E.g. `REPETITION=5`
- `PERFECT_POOL`: The path of the folder containing the perfect graphs in .g6 format. At least one file must exist in the folder. E.g. `PERFECT_POOL="/Users/burakerdem/showg"`
- `EPSILON`: The accepted tolerance around the target graph density. E.g.`EPSILON=0.1`.

## c5-free

This algorithm generates $c_5$-free graphs, starting with an empty graph and adding edges randomly until target density is reached, while ensuring that added edges do not create an induced $c_5$.

Parameters:
- `ORDERS`: The vector of graph orders. E.g. `ORDERS=(10 20 30)`.
- `DENSITIES`: The vector of densities after the decimal point. E.g. `DENSITIES=(25 50 75)`.
- `REPETITION`: The number of graphs generated for each order and density value. E.g. `REPETITION=5`
- `FORCE_NONPERFECT`: If set to `"TRUE"`. The program ensures that the output is not a perfect graph.

## perturb-vertex-pairs

This algorithm takes a graph as input and randomly flips the state of some vertex pairs. If an edge exists on a chosen vertex pair, it is removed. If no edges exists on a chosen vertex pair, it is added. The number of vertex pairs whose state will be flipped equals to the floor of ${|V| \choose 2} * $`VERTEX_PAIR_RATIO`.

Parameters:
- `INPUT_PATH`: The path of the folder of adjacency matrices or path of a single .g6 file. E.g. `INPUT_PATH="graphs/perfect-operations/graphs.g6"`.
- `VERTEX_PAIR_RATIO`: The ratio of vertex pairs whose state will be flipped. E.g. `VERTEX_PAIR_RATIO=0.1`.

## IMH

This algorithm, called "Iterative Modification Heuristic", takes a graph and iteratively modifies its edges, decreasing the number of odd holes and odd antiholes throughout the iterations (See [[2]](https://arxiv.org/abs/2507.21987) for details). At the end, either it obtains a perfect graph or it fails. It has chance to fail in some cases where at some iteration, no modification reduces the number of odd holes and odd antiholes.

Parameters:
- `INPUT_PATH`: The path of the folder of adjacency matrices or path of a single .g6 file. E.g. `INPUT_PATH="graphs/erdos-renyi/graphs.g6"`.


## IMH-near-perfect
This algorithm is based on [IMH](#imh) and is used to obtain "near perfect" graphs. The difference with [IMH](#imh) is that this one stops just before the last odd holes and/or odd antiholes are eliminated. The output graph has at least one, at most `INT_PARAM1` odd holes and odd antiholes.

Parameters:
- `INPUT_PATH`: The path of the folder of adjacency matrices or the path of a single .g6 file. E.g. `INPUT_PATH="graphs/erdos-renyi/graphs.g6"`.
- `INT_PARAM1`: The maximum number of allowed of odd holes and odd antiholes in E.g. `INT_PARAM1=20`.

## sandwich-outer

This algorithm copies a graph and turns a percentage of its non-edges to edges randomly. It is particularly useful for generating instances for the graph sandwich problem. The parameter `VERTEX_PAIR_RATIO` decides on The number of non-edges that will be turned to edges are determined as the floor of $({|V| \choose 2} - |E|) * $`VERTEX_PAIR_RATIO`.

Parameters:
- `INPUT_PATH`: The path of the folder of adjacency matrices or the path of a single .g6 file. E.g. `INPUT_PATH="graphs/erdos-renyi/graphs.g6"`.
- `VERTEX_PAIR_RATIO`: The ratio of non-edges which will be turned to edges. E.g. `VERTEX_PAIR_RATIO=0.1`.

## embed-hole

This algorithm copies a graph, chooses `INT_PARAM1` many vertices randomly and modifies vertex pairs (edges/non-edges) so that the selected vertices induce a hole of size `INT_PARAM1` (A hole is a chordless cycle of length at least 4). This method is useful to obtain "near perfect" graphs, when the input graphs are perfect. But, note that this procedure may create more than one hole. For example, consider the input is a $C_8$ and `INT_PARAM1=5`, and assume the five randomly selected vertices are consecutive on the $C_8$. The result will be two $C_5$'s sharing an edge.

Parameters:
- `INPUT_PATH`: The path of the folder of adjacency matrices or the path of a single .g6 file. E.g. `INPUT_PATH="graphs/erdos-renyi/graphs.g6"`.
- `INT_PARAM1`: The size of the hole. At least 4. E.g. `INT_PARAM1=5`.

# References

1. [Şeker, O., Ekim, T. and Taşkın, Z. C., "An Exact Cutting Plane Algorithm to Solve the Selective Graph Coloring Problem in Perfect Graphs", *European Journal of Operational Research*, Vol. 291, No. 1, pp. 67-83, 2021.](https://doi.org/10.1016/j.ejor.2020.09.017)

2. [Erdem B. N., Ekim T., Taşkın, Z. C., "Perfect Graph Modification Problems: An Integer Programming Approach", arXiv:2507.21987, 2025.](https://arxiv.org/abs/2507.21987)
