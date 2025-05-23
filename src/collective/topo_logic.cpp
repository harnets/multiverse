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

    struct RingTopology
    {
        Map<int, int> id_to_index;
        Map<int, int> index_to_id;
        int ring_id;
        // int offset;
        int total_nodes_in_ring;
        int index_in_ring;
        Dimension dimension;

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