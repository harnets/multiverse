#pragma once

#include <madrona/components.hpp>
#include <madrona/math.hpp>
#include <madrona/rand.hpp>
#include <madrona/physics.hpp>
#include <madrona/render/ecs.hpp>

#include "consts.hpp"





#define K_ARY 4 // 8 //4 //16
#define INPORT_NUM  K_ARY //40

#define NIC_RATE 1000LL*1000*1000 * 100 // 100 Gbps
#define BW 100 // 100 Gbps
// egress port
#define QUEUE_NUM 1


// packet buffer
#define PKT_BUF_LEN 600 //300 // 1024*1
#define TX_BUF_LEN PKT_BUF_LEN

#define BUF_SIZE PKT_BUF_LEN*1000  // 200*1460 //byte
#define PFC_THR 80

// ECN configuration
#define K_MAX 1000 * 1024   // 400 * 4 * 1024
#define K_MIN 200 * 1024   // 100 * 4 * 1024
#define P_MAX 0.2

//link
#define SS_LINK_DELAY 1000 // 1000 //ns switch --> switch
#define HS_LINK_DELAY 1000  // 1000  //ns host --> switch
#define MTU 1500

#define LOOKAHEAD_TIME SS_LINK_DELAY

#define PACKET_PROCESS_TIME 100 //100 ns
#define TMP_BUF_LEN SS_LINK_DELAY/PACKET_PROCESS_TIME

//NET_NPU
#define MAX_NET_NPU_FLOW_NUM 2000 //max number of flows concurrent exist in 1 net_npu at most 

// // NIC
// #define MAX_NIC_FLOW_NUM 100

// number of dst host
#define NUM_HOST K_ARY*K_ARY*K_ARY/4
#define MAX_NUM_NEXT_HOP K_ARY
//

#define MAX_PATH_LEN 40 //16 //8 hops (each hop includes device id and port id)

//Retransmit timer
#define RETRANSMIT_TIMER 100*1000 // 100us

// DCQCN parameter
#define DCQCN_Alpha 1.0
#define DCQCN_CurrentRate  NIC_RATE // 100Gbps 
#define DCQCN_TargetRate  NIC_RATE // 100Gbps
#define DCQCN_G 0.00390625
#define DCQCN_AIRate 20*1000*1000 // 20M bps  //50M bps
#define DCQCN_HAIRate 200*1000*1000 // 200 Mbps  //500M bps
#define DCQCN_AlphaUpdateInterval 1 * 1000 // 1us,  nanoseconds
#define DCQCN_RateDecreaseInterval 4 * 1000 // 4us,  nanoseconds
#define DCQCN_RateIncreaseInterval 300 * 1000 // 300us,  nanoseconds

#define DCQCN_IncreaseStageCount 0 // runtime variables, should set at runtime
#define DCQCN_RecoveryStageThreshhold 1 // 5

#define DCQCN_RateDecreaseNextTime 0 // runtime variables, should set at runtime
#define DCQCN_RateIncreaseNextTime 0 // runtime variables, should set at runtime
#define DCQCN_AlphaUpdateNextTime 0 // runtime variables, should set at runtime
#define DCQCN_RateIsDecreased false

#define CNP_DURATION 50*1000 // 50*1000 //50us

//Map structure size
#define MAP_SIZE 1000

#define MESSAGE_BUFFER_LENGTH (1000*10)






namespace madEscape {

// Include several madrona types into the simulator namespace for convenience
using madrona::Entity;
using madrona::RandKey;
using madrona::CountT;
using madrona::base::Position;
using madrona::base::Rotation;
using madrona::base::Scale;
using madrona::base::ObjectID;
using madrona::phys::Velocity;
using madrona::phys::ResponseType;
using madrona::phys::ExternalForce;
using madrona::phys::ExternalTorque;
using madrona::phys::RigidBody;

// WorldReset is a per-world singleton component that causes the current
// episode to be terminated and the world regenerated
// (Singleton components like WorldReset can be accessed via Context::singleton
// (eg ctx.singleton<WorldReset>().reset = 1)
struct WorldReset {
    int32_t reset;
};

// Discrete action component. Ranges are defined by consts::numMoveBuckets (5),
// repeated here for clarity
struct Action {
    int32_t moveAmount; // [0, 3]
    int32_t moveAngle; // [0, 7]
    int32_t rotate; // [-2, 2]
    int32_t grab; // 0 = do nothing, 1 = grab / release
};

// Per-agent reward
// Exported as an [N * A, 1] float tensor to training code
struct Reward {
    float v;
};

// Per-agent component that indicates that the agent's episode is finished
// This is exported per-agent for simplicity in the training code
struct Done {
    // Currently bool components are not supported due to
    // padding issues, so Done is an int32_t
    int32_t v;
};

// Observation state for the current agent.
// Positions are rescaled to the bounds of the play area to assist training.
struct SelfObservation {
    float roomX;
    float roomY;
    float globalX;
    float globalY;
    float globalZ;
    float maxY;
    float theta;
    float isGrabbing;
};

// The state of the world is passed to each agent in terms of egocentric
// polar coordinates. theta is degrees off agent forward.
struct PolarObservation {
    float r;
    float theta;
};

struct PartnerObservation {
    PolarObservation polar;
    float isGrabbing;
};

// Egocentric observations of other agents
struct PartnerObservations {
    PartnerObservation obs[consts::numAgents - 1];
};

// PartnerObservations is exported as a
// [N, A, consts::numAgents - 1, 3] // tensor to pytorch
static_assert(sizeof(PartnerObservations) == sizeof(float) *
    (consts::numAgents - 1) * 3);

// Per-agent egocentric observations for the interactable entities
// in the current room.
struct EntityObservation {
    PolarObservation polar;
    float encodedType;
};

struct RoomEntityObservations {
    EntityObservation obs[consts::maxEntitiesPerRoom];
};

// RoomEntityObservations is exported as a
// [N, A, maxEntitiesPerRoom, 3] tensor to pytorch
static_assert(sizeof(RoomEntityObservations) == sizeof(float) *
    consts::maxEntitiesPerRoom * 3);

// Observation of the current room's door. It's relative position and
// whether or not it is ope
struct DoorObservation {
    PolarObservation polar;
    float isOpen; // 1.0 when open, 0.0 when closed.
};

struct LidarSample {
    float depth;
    float encodedType;
};

// Linear depth values and entity type in a circle around the agent
struct Lidar {
    LidarSample samples[consts::numLidarSamples];
};

// Number of steps remaining in the episode. Allows non-recurrent policies
// to track the progression of time.
struct StepsRemaining {
    uint32_t t;
};

// Tracks progress the agent has made through the challenge, used to add
// reward when more progress has been made
struct Progress {
    float maxY;
};

// Per-agent component storing Entity IDs of the other agents. Used to
// build the egocentric observations of their state.
struct OtherAgents {
    madrona::Entity e[consts::numAgents - 1];
};

// Tracks if an agent is currently grabbing another entity
struct GrabState {
    Entity constraintEntity;
};

// This enum is used to track the type of each entity for the purposes of
// classifying the objects hit by each lidar sample.
enum class EntityType : uint32_t {
    None,
    Button,
    Cube,
    Wall,
    Agent,
    Door,
    NumTypes,
};

// A per-door component that tracks whether or not the door should be open.
struct OpenState {
    bool isOpen;
};

// Linked buttons that control the door opening and whether or not the door
// should remain open after the buttons are pressed once.
struct DoorProperties {
    Entity buttons[consts::maxEntitiesPerRoom];
    int32_t numButtons;
    bool isPersistent;
};

// Similar to OpenState, true during frames where a button is pressed
struct ButtonState {
    bool isPressed;
};

// Room itself is not a component but is used by the singleton
// component "LevelState" (below) to represent the state of the full level
struct Room {
    // These are entities the agent will interact with
    Entity entities[consts::maxEntitiesPerRoom];

    // The walls that separate this room from the next
    Entity walls[2];

    // The door the agents need to figure out how to lower
    Entity door;
};

// A singleton component storing the state of all the rooms in the current
// randomly generated level
struct LevelState {
    Room rooms[consts::numRooms];
};

/* ECS Archetypes for the game */






//
struct Results {
    uint32_t results;
};
struct Results2 {
   int32_t encoded_string[1000];
};

struct  MadronaEvent
{
    int32_t type;
    int32_t eventId;
    int32_t time;
    int32_t src;
    int32_t dst;
    int32_t size;
    int32_t port;
};

struct MadronaEventsQueue
{
    int32_t events[MESSAGE_BUFFER_LENGTH];
};

struct MadronaEvents
{
    int32_t events[MESSAGE_BUFFER_LENGTH];
};

struct  MadronaEventsResult
{
    int32_t events[MESSAGE_BUFFER_LENGTH];
};

struct  Configuration
{
    int32_t events[1000];
};

struct  ProcessParams
{
    int32_t params[1000];
};

struct CurStep {
    uint32_t step;
};

struct  SimulationTime
{
    int64_t time;
};


//
struct ChakraNodesData
{
    uint32_t data[1][10000000];
};





// There are 2 Agents in the environment trying to get to the destination
struct Agent : public madrona::Archetype<
    // RigidBody is a "bundle" component defined in physics.hpp in Madrona.
    // This includes a number of components into the archetype, including
    // Position, Rotation, Scale, Velocity, and a number of other components
    // used internally by the physics.
    RigidBody,

    // Internal logic state.
    GrabState,
    Progress,
    OtherAgents,
    EntityType,

    // Inet_nput
    Action,

    // Observations
    SelfObservation,
    PartnerObservations,
    RoomEntityObservations,
    DoorObservation,
    Lidar,
    StepsRemaining,

    // Reward, episode termination
    Reward,
    Done,

    // Visualization: In addition to the fly camera, src/viewer.cpp can
    // view the scene from the perspective of entities with this component
    madrona::render::RenderCamera,
    // All entities with the Renderable component will be drawn by the
    // viewer and batch renderer
    madrona::render::Renderable,



// //  
    ChakraNodesData
// //

> {};



// Archetype for the doors blocking the end of each challenge room
struct DoorEntity : public madrona::Archetype<
    RigidBody,
    OpenState,
    DoorProperties,
    EntityType,
    madrona::render::Renderable
> {};

// Archetype for the button objects that open the doors
// Buttons don't have collision but are rendered
struct ButtonEntity : public madrona::Archetype<
    Position,
    Rotation,
    Scale,
    ObjectID,
    ButtonState,
    EntityType,
    madrona::render::Renderable
> {};

// Generic archetype for entities that need physics but don't have custom
// logic associated with them.
struct PhysicsEntity : public madrona::Archetype<
    RigidBody,
    EntityType,
    madrona::render::Renderable
> {};

















//********************************************************


struct EgressPortID {
   uint32_t egress_port_id;
};

enum class NextHopType : uint8_t {
    HOST = 0,
    SWITCH = 1,
};

struct NextHop {
    uint32_t next_hop;
};

struct HSLinkDelay {
    int64_t HS_link_delay;
};

struct NICRate {
    int64_t nic_rate;
};

enum class PktType : uint8_t {
    DATA = 0,
    ACK = 1,
    PFC_PAUSE = 2,
    PFC_RESUME = 3,
    NACK = 4,
    CNP = 5,
};

enum class ECN_MARK: uint8_t {
    NO = 0,
    YES = 1,
};

struct Pkt {
    uint32_t src;   
    uint32_t dst;  // also PFC_FOR_UPSTREAM for the PFC frame 
    uint16_t l4_port;
    uint16_t header_len;
    uint16_t payload_len;
    uint16_t ip_pkt_len;
    int64_t sq_num;
    uint64_t flow_id;  // also the idx of priority queue for the PFC frame 
    int64_t enqueue_time;
    int64_t dequeue_time;
    uint8_t flow_priority;
    PktType pkt_type;  // also PFC state for the PFC frame 

    uint32_t path[MAX_PATH_LEN];
    uint8_t path_len;

    ECN_MARK ecn;
};
struct PktBuf {
    Pkt pkts[PKT_BUF_LEN];
    uint16_t head;
    uint16_t tail;
    uint16_t cur_num;
    uint32_t cur_bytes;
};

struct AckPktBuf {
    Pkt pkts[PKT_BUF_LEN];
    uint16_t head;
    uint16_t tail;
    uint16_t cur_num;
    uint32_t cur_bytes;
};


enum class PFCState : uint8_t {
    PAUSE = 2,
    RESUME = 3,
};
struct PktQueue {
    PktBuf pkt_buf[QUEUE_NUM];
    PFCState queue_pfc_state[QUEUE_NUM];
};


struct SimTime {
    int64_t sim_time;
};
struct SimTimePerUpdate {
    int64_t sim_time_per_update;
};

enum class FlowState : uint8_t {
    UNCOMPLETE = 0,
    COMPLETE = 1,
};


struct FlowID {
    uint64_t flow_id;
};


//////////////////////////////////////////////////////////////////////////////////////////////
// support multiple flow for a host/nic

// api for Astra-Sim system layer
struct FlowEvent {
    // Entity flow_entt;
    uint64_t flow_id;
    uint32_t src;
    uint32_t dst;
    uint16_t l4_port;
    uint32_t nic; // nic id
    uint64_t flow_size;
    int64_t start_time;
    int64_t stop_time;
    // uint16_t phase_num; // which step in a ring all-reduce    
    FlowState flow_state;
    int64_t extra_1;
};


struct CompletedFlowQueue {
    FlowEvent flow[MAX_NET_NPU_FLOW_NUM];
    uint16_t head;
    uint16_t tail;
    uint16_t cur_num; //flow_num
    // uint32_t cur_bytes;
};

struct NewFlowQueue {
    FlowEvent flow[MAX_NET_NPU_FLOW_NUM];
    uint16_t head;
    uint16_t tail;
    uint16_t cur_num; //flow_num
    // uint32_t cur_bytes;
};


struct Src {
    uint32_t src;
};

struct Dst {
    uint32_t dst;
};

struct L4Port {
    uint16_t l4_port;
};

struct FlowSize {
    uint64_t flow_size;
};

struct StartTime {
    int64_t start_time;

};

struct StopTime {
    int64_t stop_time;

};

struct NIC_ID {
    uint32_t nic_id; 
};

struct SndServerID {
    uint32_t snd_server_id; 
};

struct RecvServerID {
    uint32_t recv_server_id; 
};


// struct SqNum {
//     int64_t sq_num;
// };

struct SndNxt {
    int64_t snd_nxt;
};

struct SndUna {
    int64_t snd_una;
};

struct LastAckTimestamp {
    int64_t last_ack_timestamp;
};

struct NxtPktEvent {
    int64_t nxt_pkt_event;
};

struct CC_Para {
    int64_t m_rate; // current rate/Mpbs
    int64_t tar_rate; // target rate/Mbps
    double dcqcn_Alpha; // the alpha parameter in DCQCN
    double dcqcn_G; // the G parameter in DCQCN
    int64_t dcqcn_AIRate; // the increase rate in additive increase stage
    int64_t dcqcn_HAIRate; // the increase rate in hyper additive increase stage
    uint32_t dcqcn_AlphaUpdateInterval; // the timer interval to update alpha
    uint32_t dcqcn_RateDecreaseInterval; // the timer interval to decrease target rate
    uint32_t dcqcn_RateIncreaseInterval; // the timer interval to increase target rate
    uint32_t dcqcn_IncreaseStageCount; // record the count increase stage
    uint32_t dcqcn_RecoveryStageThreshold; // the threshshold of reconvery stage
    int64_t dcqcn_AlphaUpdateNextTime; // the next time (nanoseconds) to update alpha parameter
    int64_t dcqcn_RateDecreaseNextTime; // the next time (nanoseconds) to decrease target rate
    int64_t dcqcn_RateIncreaseNextTime; // the next time (nanoseconds) to increase target rate
    bool dcqcn_RateIsDecreased; // the flag to indicate rate is decreased when received ECE flag
    bool CNPState; 
};

struct Extra_1 {
    int64_t extra_1;
};



struct SndFlow : public madrona::Archetype<
    FlowID,
    Src, // src net_npu
    Dst, // dst net_npu
    L4Port, // layer 4 port

    FlowSize,
    StartTime,
    StopTime,
    
    NIC_ID, // nic id
    SndServerID, // host pair id
    RecvServerID, // host pair id

    // SqNum,
    SndNxt,
    SndUna,
    FlowState,

    //cc
    LastAckTimestamp,
    NxtPktEvent,
    PFCState,
    CC_Para,

    PktBuf,
    AckPktBuf,
    
    NICRate,
    HSLinkDelay,
    SimTime,
    SimTimePerUpdate,

    Extra_1
> {};


struct LastCNPTimestamp {
    int64_t last_cnp_timestamp;
};

struct RecvBytes {
    int64_t recv_bytes;
};

struct RecvFlow : public madrona::Archetype<
    FlowID,
    Src, // src net_npu
    Dst, // dst net_npu
    L4Port, // layer 4 port
    
    FlowSize,
    StartTime,
    StopTime,
 
    NIC_ID, // nic id
    SndServerID, // host pair id
    RecvServerID, // host pair id

    FlowState,

    LastCNPTimestamp,
    RecvBytes,

    PktBuf,

    NICRate,
    HSLinkDelay,
    SimTime,
    SimTimePerUpdate    
> {};


struct SendFlows {
    madrona::Entity flow[MAX_NET_NPU_FLOW_NUM];
    uint16_t head;
    uint16_t tail;
    uint16_t cur_num;
};


struct NET_NPU_ID {
    uint32_t net_npu_id; 
};

struct NET_NPU : public madrona::Archetype<
    NET_NPU_ID, //
    SimTime,
    SimTimePerUpdate,
    SendFlows,
    NewFlowQueue, // from Asstra-sim system layer
    CompletedFlowQueue // to Asstra-sim system layer
> {};
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
// NIC entity archetype
struct MountedFlows {
    madrona::Entity flow[MAX_NET_NPU_FLOW_NUM];
    uint16_t head;
    uint16_t tail;
    uint16_t cur_num;
};
// struct MountedFlows {
//     Map<uint32_t, Entity> flow;
// };


struct BidPktBuf {
    PktBuf snd_buf;
    PktBuf recv_buf;
};

struct TXElem {
    int64_t dequeue_time;
    uint16_t ip_pkt_len;
};
struct TXHistory {
    TXElem pkts[TX_BUF_LEN];
    uint16_t head;
    uint16_t tail;
    uint16_t cur_num;
    uint32_t cur_bytes;
};

struct Seed {
    uint32_t seed;
};


struct NIC : public madrona::Archetype<
    NIC_ID,
    NICRate,
    HSLinkDelay,
    SimTime,
    SimTimePerUpdate,
    MountedFlows,
    BidPktBuf,
    TXHistory,
    NextHopType,
    NextHop,
    Seed
> {};
///////////////////////////////////////////////////////////////////////////////////////////////


struct IngressPortID {
   uint32_t ingress_port_id;
};

enum class PortType : uint8_t {
    InPort = 0,
    EPort = 1,
};

// for the router forwarding system, of GPU_acclerated DES
struct LocalPortID {
   uint32_t local_port_id;
};
struct GlobalPortID {
   uint32_t global_port_id;
};


enum class SchedTrajType : uint8_t {
    SP = 0,
    FIFO = 1,
};


enum class SwitchType : uint8_t {
    Edge = 0,
    Aggr = 1,
    Core = 2,
};

struct SwitchID {
    uint32_t switch_id;
};


struct ForwardPlan {
    uint64_t forward_plan[INPORT_NUM];
};


struct FIBTable {
    uint8_t fib_table[NUM_HOST][MAX_NUM_NEXT_HOP];
};


struct LinkRate {
    int64_t link_rate;
};

struct SSLinkDelay {
    int64_t SS_link_delay;
};

struct QueueNumPerPort {
    uint8_t queue_num_per_port;
};


struct Switch : public madrona::Archetype<
    SwitchType, //
    SwitchID, //
    FIBTable,
    QueueNumPerPort
> {};


struct IngressPort : public madrona::Archetype<
    PortType,
    LocalPortID,
    GlobalPortID,
    SwitchID, //
    PktBuf, //
    ForwardPlan, //
    SimTime,
    SimTimePerUpdate
> {};


struct EgressPort : public madrona::Archetype<
    SchedTrajType,
    PortType,
    LocalPortID,
    GlobalPortID,
    SwitchID,
    PktQueue,
    TXHistory,
    NextHopType,
    NextHop,
    LinkRate,
    SSLinkDelay,
    SimTime,
    SimTimePerUpdate,
    Seed
> {};




struct Topo {
    uint32_t aj_link[100000][5];
    uint32_t link_num;
    uint32_t net_npu_num;

    Topo() : aj_link{}, link_num(0), net_npu_num(0) {}
};




// ----------------------------------------



// 每个 ChakraNode 占用 44 字节，等于 11 个 int
#define INTS_PER_NODE 46

#define MAX_CHAKRA_NODES 9 * 9999
#define MAX_CHAKRA_NODP_NODES 99
#define chakra_nodes_data_length 10000000
// 定义无效依赖的值
#define INVALID_DEPENDENCY 4294967295

// 当前无依赖节点的最大数量
#define CURRENT_EXEC_NODES_MAX 10

// 每隔x帧检测一次skip time
#define CHECK_SKIPTIME_INTERVAL_PER_FRAME 100

// 每个npu的最多流数
#define MAX_FLOW_PER_NPU 9999

// 每个comm节点的最大通讯量
#define MAX_FLOW_NUM_PER_COMM_NODE 999

// 每个节点的流序号区间
#define FLOW_ID_MAX_LENGTH 5000

// #define MAX_FLOW_NUM_ALL_COMM_NODE 9999

    struct NpuID
    {
        uint32_t value;
    };

    struct NodeID
    {
        uint32_t value;
    };

    enum class NodeType : int32_t
    {
        None = 0,
        COMP_NODE = 1,
        COMM_SEND_NODE = 2,
        COMM_RECV_NODE = 3,
        COMM_COLL_NODE = 4
    };

    enum CollectiveCommType : uint64_t
    {
        ALL_REDUCE = 0,
        REDUCE = 1,
        ALL_GATHER = 2,
        GATHER = 3,
        SCATTER = 4,
        BROADCAST = 5,
        ALL_TO_ALL = 6,
        REDUCE_SCATTER = 7,
        REDUCE_SCATTER_BLOCK = 8,
        BARRIER = 9
    };

    enum CommImplementationType : int32_t
    {
        Ring = 0
    };

    enum class AttributeKey : int32_t
    {
        comm_para = 1,
        comm_size = 2,
        comm_src = 3,
        comm_dst = 4,
        involved_dim = 5
    };

    struct CommModel
    {
        CommImplementationType all_reduce_implementation;
        CommImplementationType all_gather_implementation;
        CommImplementationType reduce_scatter_implementation;
        CommImplementationType all_to_all_implementation;
    };

    struct SysConfig : public madrona::Archetype<
                           CommModel>
    {
    };

    // struct CommParams {
    //     uint64_t comm_size;
    //     uint64_t comm_src;
    //     uint64_t comm_dst;
    //     uint32_t durationMicros;
    //     CollectiveCommType coll_comm_type;
    // };

    enum TaskState : int32_t
    {
        INIT = 0,
        START = 1,
        FINISH = 2
    };

    struct SysFlow
    {
        int id;
        uint64_t comm_size;
        uint64_t comm_src;
        uint64_t comm_dst;
        uint32_t durationMicros;

        // 流执行顺序id
        uint32_t exec_index;
        // 是否执行完
        TaskState state;
        // 收端完成 or 发端完成
        bool is_send;

        SysFlow()
            : id(-1), // 初始化为 0
              comm_size(0),
              comm_src(0),
              comm_dst(0),
              durationMicros(0),
              exec_index(0),
              state(TaskState::INIT),
              is_send(true) // 初始化为 true
        {
        }
    };

    struct TaskFlows
    {
        SysFlow flows[MAX_FLOW_NUM_PER_COMM_NODE];

        TaskFlows()
        {
            for (int i = 0; i < MAX_FLOW_NUM_PER_COMM_NODE; ++i)
            {
                flows[i] = SysFlow(); // 初始化每个任务
            }
        }
        void updateFlows(const SysFlow flows_finish[], int flows_finish_size)
        {
            for (int i = 0; i < flows_finish_size; ++i)
            {
                const SysFlow &finishedFlow = flows_finish[i];
                for (int j = 0; j < MAX_FLOW_NUM_PER_COMM_NODE; ++j)
                {
                    if (flows[j].id == finishedFlow.id)
                    { // 查找 id 相同的元素
                        flows[j].state = TaskState::FINISH;
                        flows[j].is_send = finishedFlow.is_send;               // 赋值 is_send
                        flows[j].durationMicros = finishedFlow.durationMicros; // 赋值 durationMicros
                        break;                                                 // 找到匹配项后，跳出内层循环
                    }
                }
            }
        }

        // 检查是否所有的任务都完成了
        bool areAllTasksDone() const
        {
            for (int i = 0; i < MAX_FLOW_NUM_PER_COMM_NODE; ++i)
            {
                if (flows[i].state == TaskState::INIT)
                {
                    continue;
                }

                if (flows[i].state == TaskState::START)
                {
                    return false;
                }
            }
            return true;
        }
    };

    struct ChakraNode
    {
        uint32_t name[20];
        NodeType type;
        uint32_t id;
        uint32_t data_deps[10];
        uint64_t comm_para;
        uint64_t comm_size;
        uint64_t comm_src;
        uint64_t comm_dst;
        CollectiveCommType comm_type;
        bool involved_dim_1;
        bool involved_dim_2;
        bool involved_dim_3;
        uint32_t durationMicros;

        // 自定义赋值操作符
        ChakraNode &operator=(const ChakraNode &other)
        {
            if (this == &other)
                return *this; // 防止自赋值

            // 复制每个成员变量
            for (int i = 0; i < 20; ++i)
            {
                name[i] = other.name[i];
            }
            type = other.type;
            id = other.id;
            for (int i = 0; i < 10; ++i)
            {
                data_deps[i] = other.data_deps[i];
            }
            comm_para = other.comm_para;
            comm_size = other.comm_size;
            comm_src = other.comm_src;
            comm_dst = other.comm_dst;
            involved_dim_1 = other.involved_dim_1;
            involved_dim_2 = other.involved_dim_2;
            involved_dim_3 = other.involved_dim_3;
            durationMicros = other.durationMicros;

            return *this;
        }
    };

    struct ChakraNodes
    {
        ChakraNode nodes[MAX_CHAKRA_NODES];
    };

    struct Chakra_Nodp_Nodes
    {
        ChakraNode nodes[MAX_CHAKRA_NODP_NODES];
    };

    struct HardwareResource
    {
        bool comp_ocupy;
        // bool comm_ocupy;
        bool one_task_finish;

        HardwareResource()
            : comp_ocupy(false),    // 初始化为 0
              one_task_finish(true) // 初始化为 true
        {
        }
    };

    struct NextProcessTimes
    {
        uint64_t times_abs[MAX_CHAKRA_NODES];
    };

    struct NextProcessTimeE : public madrona::Archetype<
                                  NextProcessTimes>
    {
    };

    struct ProcessingCompTask
    {
        int64_t time_finish_ns;
        TaskState state;
        int32_t node_id;
        // 默认构造函数
        ProcessingCompTask()
            : time_finish_ns(0),      // 初始化为 0
              state(TaskState::INIT), // 初始化为 true
              node_id(-1)             // 初始化为 -1 (表示无效节点)
        {
        }
    };

    struct ProcessingCommTask
    {
        int64_t time_finish_ns;
        TaskState state;
        int32_t node_id;
        int32_t flow_count;

        // 默认构造函数
        ProcessingCommTask()
            : time_finish_ns(0),      // 初始化为 0
              state(TaskState::INIT), // 初始化为 true
              node_id(-1),            // 初始化为 -1 (表示无效节点)
              flow_count(0)
        {
        }
    };

    struct ProcessingCommTasks
    {
        ProcessingCommTask tasks[MAX_FLOW_PER_NPU];

        int64_t flow_id;

        // 默认构造函数
        ProcessingCommTasks()
        {
            for (int i = 0; i < MAX_FLOW_PER_NPU; ++i)
            {
                tasks[i] = ProcessingCommTask(); // 初始化每个任务
            }
        }

        

        // 统计所有任务的 flow_count 的总和
        int getTotalFlowCount() 
        {
            // int totalFlowCount = 0;
            // for (int i = 0; i < MAX_FLOW_PER_NPU; ++i)
            // {
            //     if (tasks[i].state != TaskState::INIT)
            //     { // 只统计有效任务
            //         totalFlowCount += tasks[i].flow_count;
            //     }
            // }
            // return totalFlowCount;
            flow_id+=1;
            return flow_id;
        }

        // 判断是否存在指定节点 ID 的方法
        bool containsNodeId(int32_t node_id) const
        {
            for (int i = 0; i < MAX_FLOW_PER_NPU; ++i)
            {
                // 任务状态可能是start也可能是finish，还没来得及处理
                if (tasks[i].state != TaskState::INIT && tasks[i].node_id == node_id)
                {
                    return true; // 找到匹配的任务
                }
            }
            return false; // 未找到匹配的任务
        }

        void setFinish(int32_t node_id, int64_t time_finish_ns)
        {
            for (int i = 0; i < MAX_FLOW_PER_NPU; ++i)
            {      
                if (tasks[i].state == TaskState::START && tasks[i].node_id == node_id)
                {
                    printf("node id %d -> setFinish\n",node_id);
                    tasks[i].state = TaskState::FINISH;
                    tasks[i].time_finish_ns = time_finish_ns;
                    break;
                }
            }
        }

        // 添加任务的方法
        bool addTask(const ProcessingCommTask &new_task)
        {
            for (int i = 0; i < MAX_FLOW_PER_NPU; ++i)
            {
                if (tasks[i].state == TaskState::INIT)
                {                                      // 找到第一个 node_id == -1 的位置
                    tasks[i] = new_task;               // 放入新任务
                    tasks[i].state = TaskState::START; // 标记任务为有效
                    return true;                       // 添加成功
                }
            }
            return false; // 没有空闲位置，添加失败
        }

        // 判断是否至少存在一个 is_none == false 的节点
        bool has_task() const
        {
            for (int i = 0; i < MAX_FLOW_PER_NPU; ++i)
            {
                if (tasks[i].state == TaskState::START || tasks[i].state == TaskState::FINISH)
                { // 如果找到一个 is_none == false 的节点
                    return true;
                }
            }
            return false; // 如果所有节点的 is_none == true
        }

        // 查找 time_finish_ns <= t 的所有任务，并将这些任务的 is_none 设置为 true
        int dequeueTasksByTime(int64_t t, ProcessingCommTask result[], int max_result_size)
        {
            int count = 0;
            for (int i = 0; i < MAX_FLOW_PER_NPU && count < max_result_size; ++i)
            {
                if (tasks[i].state == TaskState::FINISH && tasks[i].time_finish_ns <= t)
                {
                    result[count++] = tasks[i];       // 将任务放入结果数组
                    tasks[i].state = TaskState::INIT; // 标记任务为无效（出队）
                    // tasks[i].node_id = -1;         // 重置 node_id
                    // tasks[i].time_finish_ns = 0;   // 重置 time_finish_ns
                }
            }
            return count; // 返回找到的任务个数
        }
    };

    struct NpuNode : public madrona::Archetype<
                         NpuID,
                         ChakraNodes,
                         HardwareResource,
                         ProcessingCompTask,
                         ProcessingCommTasks,
                         Chakra_Nodp_Nodes
                        >
    {
    };

    // ID &id, CollectiveCommType &collective_comm_type, CommParams &comm_params,TaskFlows &taskFlows
    struct ProcessComm_E : public madrona::Archetype<
    NpuID,NodeID,
                               // CollectiveCommType,
                               // CommParams,
                               TaskFlows
                              >
    {
    };

    // ------------------------------------------


}



// namespace madrona {
    // struct AJLink {
    //     uint32_t aj_link[100000][5];
    //     uint32_t link_num;
    //     uint32_t net_npu_num;
    // };
// }