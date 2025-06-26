#!/bin/bash

# 循环 K 从 8 到 16，步长为 2
for K in $(seq 8 2 8); do
    echo "Running gen_fattree_topo.py with K = $K"
    python gen_fattree_topo.py --K $K
done

# python gen_fattree_topo.py --K 8

echo "All runs completed!"