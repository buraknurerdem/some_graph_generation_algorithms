#!/bin/bash
# Author: Burak Nur Erdem

# Possible Types: erdos-renyi, barabasi-albert, threshold, perfect-operations, perturb-vertex-pairs, IMH, sandwich_outer, embed-hole, IMH-near-perfect, c5-free
# Check the script below for additional parameters for specific generation types


# You may define input/output directories as full paths or relative to this script's location
INPUTS_DIR="graphs/perfect"
OUTPUTS_DIR="graphs/perturbed"

TYPE="c5-free" # Graph type to be generated
ORDERS=(65 70 75 80 85 90) # Graph order list
DENSITIES=(25 50 75) # Density list (after decimal point)
REPETITION=5 # For each order and density how many graphs are generated

mkdir -p "$OUTPUTS_DIR" # Ensure output directory exist

if [ "$TYPE" == "perfect-operations" ] || [ "$TYPE" == "erdos-renyi" ] || [ "$TYPE" == "threshold" ] || [ "$TYPE" == "barabasi-albert" ] || [ "$TYPE" == "c5-free" ] ; then
    for n in "${ORDERS[@]}"; do
        for d in "${DENSITIES[@]}"; do
            for ((id=1; id<=REPETITION; id++ )); do
                if [ "$TYPE" == "perfect-operations" ]; then
                    epsilon=0.03
                    input_pool_path="/Users/burakerdem/showg/perfect_mckay" # Perfect graphs to be used in the operations. 
                    ./bin/gen_graph "$TYPE" "$n" "$d" "$id" "$epsilon" "$input_pool_path" "$OUTPUTS_DIR"
                elif [ "$TYPE" == "erdos-renyi" ] || [ "$TYPE" == "threshold" ] || [ "$TYPE" == "barabasi-albert" ] || [ "$TYPE" == "c5-free" ]; then
                    FORCE_NONPERFECT=TRUE
                    ./bin/gen_graph "$TYPE" "$n" "$d" "$id" "$OUTPUTS_DIR" "$FORCE_NONPERFECT"
                else
                    echo "Error: Unknown graph type specified."
                    exit 1
                fi
            done
        done
    done
elif [ "$TYPE" == "perturb-vertex-pairs" ]; then
    for input_file in "$INPUTS_DIR"/*; do

        perturb_ratios=(0.1 0.3 0.5 0.7 0.9)
        for pr in "${perturb_ratios[@]}"; do        
            # output file name is defined here. can be changed
            filename="${input_file##*/}"
            typename="perturb-from-perfect-op" 
            parts=(${filename//_/ })
            output_filename="${parts[0]}_${typename}_${parts[2]}_${parts[3]}-${pr}_${parts[4]}"
            output_file="$OUTPUTS_DIR/$output_filename"
            ./bin/gen_graph "$TYPE" "$input_file" "$pr" "$output_file"
        done
    done
elif [ "$TYPE" == "IMH" ]; then
    for input_file in "$INPUTS_DIR"/*; do
        filename="${input_file##*/}" 
        parts=(${filename//_/ })
        output_filename="${parts[0]}_${TYPE}_${parts[2]}_${parts[3]}_${parts[4]}"
        output_file="$OUTPUTS_DIR/$output_filename"
        ./bin/gen_graph "$TYPE" "$input_file" "$output_file"
    done
elif [ "$TYPE" == "IMH-near-perfect" ]; then
    max_allowed_hole_count=20
    for input_file in "$INPUTS_DIR"/*; do
        filename="${input_file##*/}" 
        parts=(${filename//_/ })
        output_filename="${parts[0]}_${TYPE}_${parts[2]}_${parts[3]}_${parts[4]}"
        output_file="$OUTPUTS_DIR/$output_filename"
        ./bin/gen_graph "$TYPE" "$input_file" "$max_allowed_hole_count" "$output_file"
    done
elif [ "$TYPE" == "sandwich_outer" ]; then
    for input_file in "$INPUTS_DIR"/*; do
        # output file name is defined here. can be changed
        graph_type="sandwich"
        filename="${input_file##*/}" 
        parts=(${filename//_/ })
        output_filename="${parts[0]}_${graph_type}_${parts[2]}_${parts[3]}_${parts[4]}"
        output_file="$OUTPUTS_DIR/$output_filename"
        # output_file="$OUTPUTS_DIR/${input_file##*/}"
        optional_edge_density=1
        ./bin/gen_graph "$TYPE" "$input_file" "$optional_edge_density" "$output_file"
    done
elif [ "$TYPE" == "embed-hole" ]; then
    for input_file in "$INPUTS_DIR"/*; do
        hole_length=5
        graph_type="embedded-hole-5-from"
        filename="${input_file##*/}" 
        parts=(${filename//_/ })
        output_filename="${parts[0]}_${graph_type}_${parts[2]}_${parts[3]}_${parts[4]}"
        output_file="$OUTPUTS_DIR/$output_filename"
        ./bin/gen_graph "$TYPE" "$input_file" "$hole_length" "$output_file"
    done
else
    echo "Error: Unknown graph type specified."
    exit 1
fi