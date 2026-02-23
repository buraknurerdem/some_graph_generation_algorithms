#!/bin/bash
# Author: Burak Nur Erdem

###########################################
### Parameters for all generation types ###
###########################################

OUTPUT_FORMAT="g6" # Possible Values: "g6", "adj_matrices", "g6_and_adj_matrices"
OUTPUTS_ADJ_MAT_DIR="graphs/perfect-operations" # Folder path to write the results
OUTPUTS_G6_FILE_PATH="graphs/perfect-operations/graphs.g6" # Folder path to write the results
TYPE="perfect-operations" # Graph type to be generated. Possible Types: erdos-renyi, barabasi-albert, threshold,
#                           perfect-operations, perturb-vertex-pairs, IMH, sandwich-outer, embed-hole, IMH-near-perfect, c5-free

############################################
### Parameters for some generation types ###
############################################

# For some generation types the main loop is over (ORDERS X DENSITIES X REPETITON)
ORDERS=(5) # Graph order list
DENSITIES=(25) # Density list (after decimal point)
REPETITION=4 # For each order and density how many graphs are generated

# For the remaining graph generation types the main loop is over the graphs in the INPUT_PATH
INPUT_PATH="graphs/perfect-operations"

PERFECT_POOL="/Users/burakerdem/showg" # Relevant in << perfect-operations >>. The folder of the perfect graphs that will be used.
EPSILON=0.1 # Relevant in << perfect-operations >>. Tolerance from the target density.
FORCE_NONPERFECT=TRUE # Relevant in << c5-free >>. If set to "TRUE". The algorithm is repeated until it generates a graph of $TYPE that is not perfect.
VERTEX_PAIR_RATIO=0.5 # Relevant in << perturb-vertex-pairs: ratio of vertex pairs whose state will be flipped,
#                                      sandwich-outer: ratio of non-edges to be changed to edges >>.
INT_PARAM1=5 # Relevant in << IMH-near-perfect: The upper bound on the allowed hole count,
#                              embed-hole: size of the hole to be embedded into the input graph >>


###############################################################
##                      Program Call                      #####
## A regular user does not need to change anything below. #####
###############################################################

mkdir -p "$OUTPUTS_ADJ_MAT_DIR" # Ensuring output directory exists
mkdir -p "$(dirname "$OUTPUTS_G6_FILE_PATH")"

# If looping over orders densities and repetitions
if [ "$TYPE" == "perfect-operations" ] || [ "$TYPE" == "erdos-renyi" ] || [ "$TYPE" == "threshold" ] || [ "$TYPE" == "barabasi-albert" ] || [ "$TYPE" == "c5-free" ] ; then
    for n in "${ORDERS[@]}"; do
        for d in "${DENSITIES[@]}"; do
            for ((id=1; id<=REPETITION; id++ )); do
                output_adj_mat_file_path=$(printf "%s/graph_%s_%05d_%-05s_%05d.txt" "$OUTPUTS_ADJ_MAT_DIR" "$TYPE" "$n" "$d" "$id" | tr ' ' '0')
                ./bin/gen_graph -t $TYPE -n $n -d $d -i $id -o $output_adj_mat_file_path -f $OUTPUT_FORMAT -g $OUTPUTS_G6_FILE_PATH \
                    -e $EPSILON --perfectpool $PERFECT_POOL \
                     --forcenonperfect $FORCE_NONPERFECT --vertexpairratio $VERTEX_PAIR_RATIO
            done
        done
    done
# If looping over an input
else
    # If input is a single .g6 file
    if [[ "$INPUT_PATH" == *.g6 ]]; then
    line_num=1
    while IFS= read -r g6_string; do
        
        # Skip empty lines
        [[ -z "$g6_string" ]] && continue

        output_file_name="graph_${TYPE}_line_${line_num}.txt"
        output_adj_mat_file_path="$OUTPUTS_ADJ_MAT_DIR/$output_file_name"

        ./bin/gen_graph -t $TYPE -o "$output_adj_mat_file_path" -e "$EPSILON" -f "$OUTPUT_FORMAT" \
            -g "$OUTPUTS_G6_FILE_PATH" --perfectpool "$PERFECT_POOL" --intparam1 "$INT_PARAM1" \
            --inputg6 "$g6_string" --forcenonperfect "$FORCE_NONPERFECT" \
            --vertexpairratio "$VERTEX_PAIR_RATIO"

        ((line_num++))
    done < "$INPUT_PATH"
    # If input is a folder of adjacency matrices
    else
        for input_file_path in "$INPUT_PATH"/*; do
            input_file_name="${input_file_path##*/}"
            parts=(${input_file_name//_/ })
            output_file_name="${parts[0]}_${TYPE}_${parts[2]}_${parts[3]}_${parts[4]}"
            output_adj_mat_file_path="$OUTPUTS_ADJ_MAT_DIR/$output_file_name"
            ./bin/gen_graph -t $TYPE -o $output_adj_mat_file_path -e $EPSILON -f $OUTPUT_FORMAT -g $OUTPUTS_G6_FILE_PATH \
                --perfectpool $PERFECT_POOL --intparam1 $INT_PARAM1 \
                --inputfilepath $input_file_path --forcenonperfect $FORCE_NONPERFECT --vertexpairratio $VERTEX_PAIR_RATIO
        done
    fi
fi
