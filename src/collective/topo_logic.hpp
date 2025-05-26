#pragma once

#include <madrona/components.hpp>
#include <madrona/math.hpp>
#include <madrona/rand.hpp>
#include <madrona/physics.hpp>
#include <madrona/render/ecs.hpp>

#include "../sim.hpp"
#include "../consts.hpp"

#define NODES_MAXNUM_IN_RING 1000

namespace madEscape
{

    enum class Direction
    {
        Clockwise,
        Anticlockwise
    };
    enum class Dimension
    {
        Local,
        Vertical,
        Horizontal,
        NA
    };

    struct RingTopology
    {
        Map<int, int> id_to_index;
        Map<int, int> index_to_id;
        int ring_id;
        // int offset;
        int total_nodes_in_ring;
        int index_in_ring;
        Dimension dimension;
        int offset;

        // 构造函数
        RingTopology(Dimension dim = Dimension::NA,
                     int id = 0,
                     int total_nodes = 0,
                     int index = 0,
                     int offset = 1)
            : ring_id(id), total_nodes_in_ring(total_nodes), index_in_ring(index), dimension(dim), offset(offset)
        {
            // 初始化第一个节点的映射
            id_to_index[id] = index;
            index_to_id[index] = id;

            // 初始化其他节点的映射
            int tmp = id;
            for (int i = 0; i < total_nodes - 1; i++)
            {
                tmp = get_receiver_homogeneous(tmp, Direction::Clockwise, offset);
            }
        }

        // 添加 get_receiver_homogeneous 方法
        int get_receiver_homogeneous(int node_id, Direction direction, int offset)
        {
            int index = id_to_index[node_id];

            if (direction == Direction::Clockwise)
            {
                int receiver = node_id + offset;
                if (index == total_nodes_in_ring - 1)
                {
                    receiver -= (total_nodes_in_ring * offset);
                    index = 0;
                }
                else
                {
                    index++;
                }
                // assert(receiver >= 0);
                id_to_index[receiver] = index;
                index_to_id[index] = receiver;
                return receiver;
            }
            else
            {
                int receiver = node_id - offset;
                if (index == 0)
                {
                    receiver += (total_nodes_in_ring * offset);
                    index = total_nodes_in_ring - 1;
                }
                else
                {
                    index--;
                }
                // assert(receiver >= 0);
                id_to_index[receiver] = index;
                index_to_id[index] = receiver;
                return receiver;
            }
        }
        int get_receiver(int node_id, Direction direction)
        {
            int index = id_to_index[node_id];
            if (direction == Direction::Clockwise)
            {
                index++;
                if (index == total_nodes_in_ring)
                {
                    index = 0;
                }
                return index_to_id[index];
            }
            else
            {
                index--;
                if (index < 0)
                {
                    index = total_nodes_in_ring - 1;
                }
                return index_to_id[index];
            }
        }
        int get_sender(int node_id, Direction direction)
        {
            int index = id_to_index[node_id];
            if (direction == Direction::Anticlockwise)
            {
                index++;
                if (index == total_nodes_in_ring)
                {
                    index = 0;
                }
                return index_to_id[index];
            }
            else
            {
                index--;
                if (index < 0)
                {
                    index = total_nodes_in_ring - 1;
                }
                return index_to_id[index];
            }
        }
    };

}; // namespace madEscape