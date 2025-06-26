#!/bin/bash

# GPU Memory and Debug Environment Variables
# export MADRONA_MWGPU_DEVICE_HEAP_SIZE=17179869184  # 16GB GPU heap
# export MADRONA_MWGPU_FORCE_DEBUG=0                 # Disable debug mode for better performance  
# export CUDA_VISIBLE_DEVICES=0                      # Use only GPU 0

# Debug options (uncomment if needed)
# export MADRONA_MWGPU_VERBOSE_COMPILE=1
# export CUDA_LAUNCH_BLOCKING=1

# export MADRONA_MWGPU_DEVICE_HEAP_SIZE=4679869184

# echo "=== GPU Memory Settings ==="
# echo "MADRONA_MWGPU_DEVICE_HEAP_SIZE: $MADRONA_MWGPU_DEVICE_HEAP_SIZE bytes"
# echo "CUDA_VISIBLE_DEVICES: $CUDA_VISIBLE_DEVICES"
# echo "=========================="

num_env=1
num_updates=100
gpu_id=1
fattree_K=4
cc_method=1 #0 = dcqcn, 1 = hpcc(TBD)


# # show link log
# export MADRONA_MWGPU_LINK_VERBOSE=1
# # detailed compile output
# export MADRONA_MWGPU_VERBOSE_COMPILE=1

cd ./build
cmake ..
# cmake -DCMAKE_BUILD_TYPE=Debug \
#       -DCUDA_VERBOSE=ON \
#       -DMADRONA_ENABLE_TRACING=ON \
#       ..

make -j 60
cd ../
cd ./scripts/

#compute-sanitizer --tool=memcheck 
# gdb --args 
python train_with_topo_fib_transfer.py --num-worlds $num_env --num-updates $num_updates --ckpt-dir ../build/ckpts --gpu_id $gpu_id --fattree_K $fattree_K --cc_method $cc_method  --gpu-sim   > ../zlog/cc_0625_1024.log 
#

# for k in $(seq 1 20); 
# do
#     python train_with_topo_fib_transfer.py --num-worlds $num_env --num-updates $num_updates --ckpt-dir ../build/ckpts --gpu_id $gpu_id --fattree_K $fattree_K --cc_method $cc_method  --gpu-sim  > ../zlog/cc_0624_10000_${k}.log  
#     # Check if the run was successful
#     if [ $? -eq 0 ]; then
#         echo "pkt_buf_size = 1000, K=$k completed successfully"
#     else
#         echo "pkt_buf_size = 1000, K=$k failed with exit code $?"
#     fi
#     sleep 10
# done



# for k in $(seq 1 128); do
#     echo "=== Running fattree_K=$k ==="
#     fattree_K=$k
#     python train_with_topo_fib_transfer.py --num-worlds $num_env --num-updates $num_updates --ckpt-dir ../build/ckpts --gpu_id $gpu_id --fattree_K $fattree_K --cc_method $cc_method --gpu-sim > "../log/cc_0612_fattree_K_${k}.log" 2>&1
    
#     # Check if the run was successful
#     if [ $? -eq 0 ]; then
#         echo "fattree_K=$k completed successfully"
#     else
#         echo "fattree_K=$k failed with exit code $?"
#     fi
    
#     sleep 2  # Brief pause between runs
# done

# echo "=== All flow num iterations completed ==="




# for i in $(seq 31 60); do
#     echo "Running iteration $i..."
#     python train.py --num-worlds $num_env --num-updates $num_updates --ckpt-dir ../build/ckpts --gpu_id $gpu_id --fattree_K $fattree_K --cc_method $cc_method --gpu-sim > "../log/log_f_10_a2a_f_14KB_${i}_finished" 2>&1
#     sleep 5
# done
