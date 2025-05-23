#include <madrona/mw_gpu_entry.hpp>

#include "sim.hpp"
#include "level_gen.hpp"

#include <algorithm>

#include "collective/topo_logic.hpp"

using namespace madrona;
using namespace madrona::math;
using namespace madrona::phys;

#define PRINT_PKT_LOG 1


// #define PRINT_SYS_LOG 1
// #define PRINT_CC_LOG 1

#define SYS_LOG true

#define SYS_CHECK false

#define ENABLE_TEST false

#define MAX_MODE_16_LIMIT true



namespace RenderingSystem = madrona::render::RenderingSystem;

namespace madEscape {

// Register all the ECS components and archetypes that will be
// used in the simulation
void Sim::registerTypes(ECSRegistry &registry, const Config &cfg)
{
    base::registerTypes(registry);
    phys::PhysicsSystem::registerTypes(registry);

    RenderingSystem::registerTypes(registry, cfg.renderBridge);

    registry.registerComponent<Action>();
    registry.registerComponent<SelfObservation>();
    registry.registerComponent<Reward>();
    registry.registerComponent<Done>();
    registry.registerComponent<GrabState>();
    registry.registerComponent<Progress>();
    registry.registerComponent<OtherAgents>();
    registry.registerComponent<PartnerObservations>();
    registry.registerComponent<RoomEntityObservations>();
    registry.registerComponent<DoorObservation>();
    registry.registerComponent<ButtonState>();
    registry.registerComponent<OpenState>();
    registry.registerComponent<DoorProperties>();
    registry.registerComponent<Lidar>();
    registry.registerComponent<StepsRemaining>();
    registry.registerComponent<EntityType>();



//
    registry.registerComponent<CurStep>();
    registry.registerComponent<Results>();
    registry.registerComponent<Results2>();
    registry.registerComponent<SimulationTime>();
    registry.registerComponent<MadronaEventsQueue>();
    registry.registerComponent<MadronaEvents>();
    registry.registerComponent<MadronaEventsResult>();
    registry.registerComponent<ProcessParams>();
//
    registry.registerComponent<ChakraNodesData>();


    registry.registerSingleton<WorldReset>();
    registry.registerSingleton<LevelState>();

    // -------------------------------------
    registry.registerComponent<NpuID>();
    registry.registerComponent<NodeID>();
    registry.registerComponent<ChakraNodes>();
    registry.registerComponent<HardwareResource>();
    registry.registerComponent<ProcessingCompTask>();
    registry.registerComponent<ProcessingCommTasks>();
    registry.registerComponent<Chakra_Nodp_Nodes>();
    registry.registerArchetype<NpuNode>();

    registry.registerComponent<NextProcessTimes>();
    registry.registerArchetype<NextProcessTimeE>();

    registry.registerComponent<CommModel>();
    registry.registerArchetype<SysConfig>();

    registry.registerComponent<TaskFlows>();
    registry.registerArchetype<ProcessComm_E>();



    registry.registerArchetype<Agent>();
    registry.registerArchetype<PhysicsEntity>();
    registry.registerArchetype<DoorEntity>();
    registry.registerArchetype<ButtonEntity>();

    registry.exportSingleton<WorldReset>(
        (uint32_t)ExportID::Reset);
    registry.exportColumn<Agent, Action>(
        (uint32_t)ExportID::Action);
    registry.exportColumn<Agent, SelfObservation>(
        (uint32_t)ExportID::SelfObservation);
    registry.exportColumn<Agent, PartnerObservations>(
        (uint32_t)ExportID::PartnerObservations);
    registry.exportColumn<Agent, RoomEntityObservations>(
        (uint32_t)ExportID::RoomEntityObservations);
    registry.exportColumn<Agent, DoorObservation>(
        (uint32_t)ExportID::DoorObservation);
    registry.exportColumn<Agent, Lidar>(
        (uint32_t)ExportID::Lidar);
    registry.exportColumn<Agent, StepsRemaining>(
        (uint32_t)ExportID::StepsRemaining);
    registry.exportColumn<Agent, Reward>(
        (uint32_t)ExportID::Reward);
    registry.exportColumn<Agent, Done>(
        (uint32_t)ExportID::Done);




//
    registry.exportColumn<Agent, Results>((uint32_t)ExportID::Results);
    registry.exportColumn<Agent, Results2>((uint32_t)ExportID::Results2);
    registry.exportColumn<Agent, SimulationTime>((uint32_t)ExportID::SimulationTime);
    registry.exportColumn<Agent, MadronaEvents>((uint32_t)ExportID::MadronaEvents);
    registry.exportColumn<Agent, MadronaEventsResult>((uint32_t)ExportID::MadronaEventsResult);
    registry.exportColumn<Agent, ProcessParams>((uint32_t)ExportID::ProcessParams);
//
    registry.exportColumn<Agent, ChakraNodesData>((uint32_t)ExportID::ChakraNodesData);


//
    //***************************************************
    // for the router forwarding function, of GPU_acclerated DES
    registry.registerComponent<NICRate>();
    registry.registerComponent<HSLinkDelay>();

    registry.registerComponent<NextHopType>();
    registry.registerComponent<NextHop>();
    
    registry.registerComponent<Pkt>();
    registry.registerComponent<PktBuf>();
    
    //pfc
    registry.registerComponent<PFCState>();
    //PktQueue is composed of multiple PktBuf and a PFCState array
    registry.registerComponent<PktQueue>(); 
    
    registry.registerComponent<SimTime>();
    registry.registerComponent<SimTimePerUpdate>();


    ///////////////////////////////////////////////////////
    // components for send flow entity
    registry.registerComponent<FlowID>();
    registry.registerComponent<Src>();
    registry.registerComponent<Dst>();
    registry.registerComponent<L4Port>();

    registry.registerComponent<FlowSize>();
    registry.registerComponent<StartTime>();
    registry.registerComponent<StopTime>();

    registry.registerComponent<NIC_ID>();
    registry.registerComponent<SndServerID>();
    registry.registerComponent<RecvServerID>();
    
    registry.registerComponent<SndNxt>();
    registry.registerComponent<SndUna>();
    registry.registerComponent<FlowState>();
    registry.registerComponent<Extra_1>();
    
    registry.registerComponent<AckPktBuf>();

    registry.registerComponent<LastAckTimestamp>();
    registry.registerComponent<NxtPktEvent>();
    registry.registerComponent<CC_Para>();

    registry.registerArchetype<SndFlow>();

    ///////////////////////////////////////////////////////
    // components for recv flow entity
    registry.registerComponent<LastCNPTimestamp>();
    registry.registerComponent<RecvBytes>();

    registry.registerArchetype<RecvFlow>();
    
    registry.registerComponent<NET_NPU_ID>();
    registry.registerComponent<SendFlows>();
    registry.registerComponent<NewFlowQueue>(); 
    registry.registerComponent<CompletedFlowQueue>();



    registry.registerArchetype<NET_NPU>();

    ///////////////////////////////////////////////////////
    // components for NIC entity
    registry.registerComponent<MountedFlows>();
    registry.registerComponent<BidPktBuf>();
    registry.registerComponent<TXHistory>();  
    registry.registerComponent<Seed>();

    registry.registerArchetype<NIC>();

    ///////////////////////////////////////////////////////
    // components for switch entity
    registry.registerComponent<SwitchID>();
    registry.registerComponent<SwitchType>();
    registry.registerComponent<FIBTable>();
    registry.registerComponent<QueueNumPerPort>();
    
    registry.registerArchetype<Switch>();

    ///////////////////////////////////////////////////////
    // components for egress and ingress entity
    registry.registerComponent<LinkRate>();
    registry.registerComponent<SSLinkDelay>();

    registry.registerComponent<SchedTrajType>();
    registry.registerComponent<PortType>();
    registry.registerComponent<LocalPortID>();
    registry.registerComponent<GlobalPortID>();

    registry.registerComponent<ForwardPlan>();

    registry.registerArchetype<IngressPort>();
    registry.registerArchetype<EgressPort>();
//


}







//random generator (Linear Congruential Generator)
class LCG {
private:
    unsigned long a;
    unsigned long c;
    unsigned long modulus;
    unsigned long seed;

public:
    LCG(unsigned long _a, unsigned long _c, unsigned long _modulus, unsigned long _seed)
        : a(_a), c(_c), modulus(_modulus), seed(_seed) {}

    unsigned long next() {
        seed = (a * seed + c) % modulus;
        return seed;
    }

    void set_seed(unsigned long _seed) {
        seed = _seed;
    }
};

//******************************************
//the operation set for queue managing

// Helper type trait to check if T has a member named cur_bytes
template <typename T, typename = void>
struct has_cur_bytes : std::false_type {};

template <typename T>
struct has_cur_bytes<T, std::void_t<decltype(std::declval<T>().cur_bytes)>> : std::true_type {};

template<typename T, size_t N>
size_t getArrayLength(T (&)[N]) {
    return N;
}

template<typename T>
inline Pkt& get_elem(T& queue, int32_t idx) 
{
    return queue.pkts[(queue.head+idx) % getArrayLength(queue.pkts)];
}

template<typename T1, typename T2>
inline bool _dequeue(T1 &queue, T2& elem) 
{
    if (queue.cur_num == 0) {
        // The queue is empty, cannot dequeue
        return false;
    }
    elem = queue.pkts[queue.head];
    queue.head = (queue.head + 1) % PKT_BUF_LEN;
    queue.cur_num -= 1;
    // // printf("queue.cur_num: %u\n", queue.cur_num);

    if constexpr (has_cur_bytes<T1>::value) {
        queue.cur_bytes -= elem.ip_pkt_len;
    }
    // queue.cur_bytes -= elem.ip_pkt_len;

    return true; 
}

template<typename T1, typename T2>
inline bool fetch_elem(T1& queue, int32_t idx, T2& elem) 
{
    if (queue.cur_num == 0) 
        return false; // The queue is empty, cannot dequeue
    //elem = queue.pkts[(queue.head+idx) % getArrayLength(queue.pkts)]; // use modulo operation to wrap around
    elem = queue.pkts[(queue.head+idx) % PKT_BUF_LEN];
    //queue.head = (queue.head + 1) % (PKT_BUF_LEN); // use modulo operation to wrap around

    return true; 
}

template<typename T1, typename T2>
inline void _enqueue(T1 &queue, T2 pkt) {
    // if (queue.cur_num >= PKT_BUF_LEN) {
    //     // printf("queue is overloaded\n");
    //     // exit(0);
    // }

    queue.pkts[queue.tail] = pkt;
    //queue.tail = (queue.tail + 1) % getArrayLength(queue.pkts);
    queue.tail = (queue.tail + 1) % PKT_BUF_LEN;
    queue.cur_num += 1;

    if constexpr (has_cur_bytes<T1>::value) {
        queue.cur_bytes += pkt.ip_pkt_len;
    }
    // queue.cur_bytes += pkt.ip_pkt_len;
}

template<typename T>
inline uint16_t get_queue_len(T &queue) {
    return queue.cur_num;
}

template<typename T>
inline void clear_queue(T &queue) {
    queue.head = 0;
    queue.tail = 0;
    queue.cur_num = 0;

    if constexpr (has_cur_bytes<T>::value) {
        queue.cur_bytes = 0;
    }
    // queue.cur_bytes = 0;
}

// remove the range of queue between start and end
template<typename T1, typename T2>
inline void rm_queue_range(T1 &queue, T2 pkt, uint16_t start, uint16_t end) {
    for (uint16_t i = start; i < end; i++) {
        _dequeue(queue, pkt);
    }
}


template<typename T1, typename T2>
inline bool _dequeue_flow(T1& queue, T2& elem) 
{
    if (queue.cur_num == 0) {
        // The queue is empty, cannot dequeue
        return false;
    }

    elem = queue.flow[queue.head];
    queue.head = (queue.head + 1) % MAX_NET_NPU_FLOW_NUM;
    queue.cur_num -= 1;

    return true; 
}


template<typename T1, typename T2>
inline void _enqueue_flow(T1 &queue, T2 pkt) {
    if (queue.cur_num >= MAX_NET_NPU_FLOW_NUM) {
        // printf("error: flow queue is overloaded\n");
        // exit(0);
    }
    queue.flow[queue.tail] = pkt;
    queue.tail = (queue.tail + 1) % MAX_NET_NPU_FLOW_NUM;
    queue.cur_num += 1;
}



///////////////////////////////////////////////////////////////////////////////////
// DCQCN CC logic
void inline dcqcn_rate_increase(CC_Para &cc_para, uint32_t src, long curSimTime)
{
    if (cc_para.dcqcn_IncreaseStageCount < cc_para.dcqcn_RecoveryStageThreshold)
    {
        // fast recovery stage
        // update current rate
        cc_para.m_rate = cc_para.m_rate / 2 + cc_para.tar_rate / 2;

        #if PRINT_CC_LOG
        printf("In fast recovery stage. sender: %d, sim_time: %ld, target rate is %.3lf Gbps, and current rate is %.3lf Gbps\n", src, curSimTime, cc_para.tar_rate/(1000*1000*1000.0), cc_para.m_rate/(1000*1000*1000.0));
        #endif
    }
    else if (cc_para.dcqcn_IncreaseStageCount == cc_para.dcqcn_RecoveryStageThreshold)
    {
        // additive increase stage
        // target rate increases with fixed AI sending rate
        cc_para.tar_rate += cc_para.dcqcn_AIRate;

        // if target rate is bigger than the device rate, set to device rate
        if (cc_para.tar_rate > NIC_RATE) {
            cc_para.tar_rate = NIC_RATE;
        }
        // update current rate
        cc_para.m_rate = cc_para.m_rate / 2 + cc_para.tar_rate / 2;

        #if PRINT_CC_LOG
        printf("In additive increase stage. sender: %d, sim_time: %ld, target rate is %.3lf Gbps, and current rate is %.3lf Gbps\n", src, curSimTime, cc_para.tar_rate/(1000*1000*1000.0), cc_para.m_rate/(1000*1000*1000.0));
        #endif
    }
    else
    {
        // hyper increase stage
        // target rate increases with fixed AI sending rate
        cc_para.tar_rate += cc_para.dcqcn_HAIRate;

        // if target rate is bigger than the device rate, set to device rate
        if (cc_para.tar_rate > NIC_RATE) {
            cc_para.tar_rate = NIC_RATE;
        }
        // update current rate
        cc_para.m_rate = cc_para.m_rate / 2 + cc_para.tar_rate / 2;

        printf("In hyper increase stage. sender: %d, sim_time: %ld, target rate is %.3lf Gbps, and current rate is %.3lf Gbps\n", src, curSimTime, cc_para.tar_rate/(1000*1000*1000.0), cc_para.m_rate/(1000*1000*1000.0));
    }

    // increase the stage count
    cc_para.dcqcn_IncreaseStageCount++;
}

void inline dcqcn_alpha_update(CC_Para &cc_para)
{
    // if we receive CNP packet, increase value of alpha
    if (cc_para.CNPState == true)
    {
        cc_para.dcqcn_Alpha = (1 - cc_para.dcqcn_G) * cc_para.dcqcn_Alpha + cc_para.dcqcn_G;
        // cc_para.CNPState = false;
    }
    // else decrease the value of alpha
    else
    {
        cc_para.dcqcn_Alpha = (1 - cc_para.dcqcn_G) * cc_para.dcqcn_Alpha;
    }
}

// decrease stage of sending rate in DCQCN, when received ECE packet
void inline dcqcn_rate_decrease(CC_Para &cc_para, uint32_t src, long curSimTime) {
    // if (cc_para.ECNState != ECNState.ECE_RECV)
    if (cc_para.CNPState != true)
        return;
    // record current rate to later fast recovery
    #if PRINT_CC_LOG
    printf(" original rate: %.3lf, dcqcn_Alpha: %.3lf\n", cc_para.m_rate/(1000*1000*1000.0), cc_para.dcqcn_Alpha);
    #endif

    cc_para.tar_rate = cc_para.m_rate;
    cc_para.m_rate = (long int)(cc_para.m_rate * (1.0 - cc_para.dcqcn_Alpha / 2.0));
    cc_para.dcqcn_IncreaseStageCount = 0;

    #if PRINT_CC_LOG
    printf("In decrease stage. sender: %d, sim_time: %ld, target rate is %.3lf Gbps, and current rate is %.3lf Gbps\n", src, curSimTime, cc_para.tar_rate/(1000*1000*1000.0), cc_para.m_rate/(1000*1000*1000.0));
    #endif
}

// reset the alpha update and rate increase timer, when rate is decreased
void inline dcqcn_timer_reset_if_decrease(CC_Para &cc_para, long curSimTime) {

    // if rate isn't decreased
    if (!cc_para.dcqcn_RateIsDecreased)
        return;

    // since we decrease the rate, we need to reset rate increase timer
    cc_para.dcqcn_AlphaUpdateNextTime = cc_para.dcqcn_RateIncreaseNextTime = curSimTime + cc_para.dcqcn_RateIncreaseInterval;
    //
    cc_para.dcqcn_RateIsDecreased = false;
}

// 
void inline dcqcn_timer_rate_increase(CC_Para &cc_para, uint32_t src, long curSimTime) {
    // if current simulation time is smaller than the next rate increase time,
    // should not tigger this function
    if (curSimTime <= cc_para.dcqcn_RateIncreaseNextTime)
        return;

    // schedule the next rate increase timer
    cc_para.dcqcn_RateIncreaseNextTime += cc_para.dcqcn_RateIncreaseInterval;

    // increase sending rate
    dcqcn_rate_increase(cc_para, src, curSimTime);

}

void inline dcqcn_timer_alpha_update(CC_Para &cc_para, long curSimTime) {

    // if current simulation time is smaller than the next rate decrease time,
    // should not tigger this function
    if (curSimTime <= cc_para.dcqcn_AlphaUpdateNextTime)
        return;
    // schedule the next alpha update timer
    cc_para.dcqcn_AlphaUpdateNextTime += cc_para.dcqcn_AlphaUpdateInterval;

    dcqcn_alpha_update(cc_para);
}
///////////////////////////////////////////////////////////////////////////////////
// 

inline void PrintPkt(Pkt p, char *desc) {
    // if (p.sq_num != 1664400) return; //1664400
    if (p.pkt_type == PktType::DATA)
         printf("pkt type: DATA\n");
    else if (p.pkt_type == PktType::ACK)
         printf("pkt type: ACK\n");
    else if (p.pkt_type == PktType::PFC_PAUSE)
         printf("pkt type: PFC_PAUSE\n");
    else if (p.pkt_type == PktType::PFC_RESUME)
         printf("pkt type: PFC_RESUME\n");
    else if (p.pkt_type == PktType::NACK)
         printf("pkt type: NACK\n");
    else
         printf("pkt type: CNP\n");
     printf("%s:: src: %d, dst: %d, ecn: %hhu, payload_len: %hu, sq_num: %ld, enqueue_time: %ld, dequeue_time: %ld\n", desc, p.src, p.dst, p.ecn, p.payload_len, p.sq_num, p.enqueue_time, p.dequeue_time);
}

template<typename T>
inline void PrintQueue(T &queue) {
    // printf("packet buffer is: \n");
    for (uint16_t i = 0; i < queue.cur_num; i++) {
        Pkt pkt = get_elem(queue, i);
        PrintPkt(pkt, "");
    }
    // printf("\n");
}

inline void create_pkt(Pkt &pkt,
    uint32_t src,   // PFC state for the PFC frame 
    uint32_t dst,  // PFC_FOR_UPSTREAM for the PFC frame 
    uint16_t l4_port,
    uint16_t header_len,
    uint16_t payload_len, 
    uint16_t ip_pkt_len,
    int64_t sq_num,
    uint64_t flow_id,  // also the idx of priority queue for the PFC frame 
    int64_t enqueue_time,
    int64_t dequeue_time,
    uint8_t flow_priority,
    PktType pkt_type,
    uint32_t _path[MAX_PATH_LEN],
    uint8_t path_len,
    ECN_MARK ecn)
{
    pkt.src = src;
    pkt.dst = dst;
    pkt.l4_port = l4_port;
    pkt.header_len = header_len;
    pkt.payload_len = payload_len;
    pkt.ip_pkt_len = ip_pkt_len;
    pkt.sq_num = sq_num;
    pkt.flow_id = flow_id;
    pkt.enqueue_time = enqueue_time;
    pkt.dequeue_time = dequeue_time;
    pkt.flow_priority = flow_priority;
    pkt.pkt_type = pkt_type;
    memcpy(pkt.path, _path, MAX_PATH_LEN * sizeof(uint32_t));
    pkt.path_len = path_len;
    pkt.ecn = ecn;
}


// create send flow entity and recv flow entity
inline void create_flow(Engine &ctx, FlowEvent flow_event, uint64_t flow_id, int64_t sim_time) {
    // printf("Entering create_flow for flow_id: %lu\n", flow_id);

    // // Lock only around shared resource access
    // ctx.data().flow_lock.lock();

// 4 type of entity for each flow:
// 1) snd flow in sender side (send data pkt)
// 2) recv flow in receiver side (receive data pkt)
// 3) snd flow in receiver side (send ACK/NACK/CNP pkt)
// 4) recv flow in sender side (receive ACK/NACK/CNP pkt)

// only snd flows in receiver side change their src and dst.

//1) snd flow in sender side (send data pkt)
    Entity snd_flow_entt = ctx.makeEntity<SndFlow>();

    ctx.get<FlowID>(snd_flow_entt).flow_id = flow_id;
    ctx.get<Src>(snd_flow_entt).src = flow_event.src;
    ctx.get<Dst>(snd_flow_entt).dst = flow_event.dst;
    ctx.get<L4Port>(snd_flow_entt).l4_port = flow_event.l4_port; //FIXME

    ctx.get<FlowSize>(snd_flow_entt).flow_size = flow_event.flow_size;
    ctx.get<StartTime>(snd_flow_entt).start_time = flow_event.start_time;
    ctx.get<StopTime>(snd_flow_entt).stop_time = 0;

    ctx.get<NIC_ID>(snd_flow_entt).nic_id = flow_event.src;
    // ctx.get<SndServerID>(snd_flow_entt).snd_server_id = flow_events[i][6];
    // ctx.get<RecvServerID>(snd_flow_entt).recv_server_id = flow_events[i][7];
    
    ctx.get<SndNxt>(snd_flow_entt).snd_nxt = 0;
    ctx.get<SndUna>(snd_flow_entt).snd_una = 0;
    ctx.get<FlowState>(snd_flow_entt) = FlowState::UNCOMPLETE;
    ctx.get<Extra_1>(snd_flow_entt).extra_1 =flow_event.extra_1;

    ctx.get<LastAckTimestamp>(snd_flow_entt).last_ack_timestamp = 0; //1ms
    ctx.get<NxtPktEvent>(snd_flow_entt).nxt_pkt_event = 0;
    
    ctx.get<PFCState>(snd_flow_entt) = PFCState::RESUME;

    ctx.get<CC_Para>(snd_flow_entt).m_rate = DCQCN_CurrentRate;
    ctx.get<CC_Para>(snd_flow_entt).tar_rate = DCQCN_TargetRate;
    ctx.get<CC_Para>(snd_flow_entt).dcqcn_Alpha = DCQCN_Alpha;
    ctx.get<CC_Para>(snd_flow_entt).dcqcn_G = DCQCN_G;
    ctx.get<CC_Para>(snd_flow_entt).dcqcn_AIRate = DCQCN_AIRate;
    ctx.get<CC_Para>(snd_flow_entt).dcqcn_HAIRate = DCQCN_HAIRate; 
    ctx.get<CC_Para>(snd_flow_entt).dcqcn_RateDecreaseInterval = DCQCN_RateDecreaseInterval;
    ctx.get<CC_Para>(snd_flow_entt).dcqcn_RateIncreaseInterval = DCQCN_RateIncreaseInterval;
    ctx.get<CC_Para>(snd_flow_entt).dcqcn_AlphaUpdateInterval = DCQCN_AlphaUpdateInterval;
    ctx.get<CC_Para>(snd_flow_entt).dcqcn_IncreaseStageCount = DCQCN_IncreaseStageCount;
    ctx.get<CC_Para>(snd_flow_entt).dcqcn_RateDecreaseNextTime = DCQCN_RateDecreaseNextTime;
    ctx.get<CC_Para>(snd_flow_entt).dcqcn_RateIncreaseNextTime = DCQCN_RateIncreaseNextTime;
    ctx.get<CC_Para>(snd_flow_entt).dcqcn_AlphaUpdateNextTime = DCQCN_AlphaUpdateNextTime;
    ctx.get<CC_Para>(snd_flow_entt).dcqcn_RateIsDecreased = DCQCN_RateIsDecreased;
    ctx.get<CC_Para>(snd_flow_entt).CNPState = false;

    ctx.get<PktBuf>(snd_flow_entt).head = 0;
    ctx.get<PktBuf>(snd_flow_entt).tail = 0;
    ctx.get<PktBuf>(snd_flow_entt).cur_num = 0;
    ctx.get<PktBuf>(snd_flow_entt).cur_bytes = 0;

    ctx.get<AckPktBuf>(snd_flow_entt).head = 0;
    ctx.get<AckPktBuf>(snd_flow_entt).tail = 0;
    ctx.get<AckPktBuf>(snd_flow_entt).cur_num = 0;
    ctx.get<AckPktBuf>(snd_flow_entt).cur_bytes = 0;

    ctx.get<NICRate>(snd_flow_entt).nic_rate = 1000LL*1000*1000*100; // 100 Gbps
    ctx.get<HSLinkDelay>(snd_flow_entt).HS_link_delay = HS_LINK_DELAY; // 1 us, 1000 ns 

    // ctx.get<SimTime>(snd_flow_entt).sim_time = flow_event.start_time;
    ctx.get<SimTime>(snd_flow_entt).sim_time = sim_time;
    // ctx.get<SimTime>(snd_flow_entt).sim_time = 0;
    ctx.get<SimTimePerUpdate>(snd_flow_entt).sim_time_per_update = LOOKAHEAD_TIME; //1000ns

    // uint32_t src = flow_event.src;

    ctx.data().snd_flows[flow_event.src][flow_id] = snd_flow_entt;
//

// 2) recv flow in receiver side (receive data pkt)
    Entity rcv_flow_entt = ctx.makeEntity<RecvFlow>();
    
    ctx.get<FlowID>(rcv_flow_entt).flow_id = flow_id;
    ctx.get<Src>(rcv_flow_entt).src = flow_event.src;
    ctx.get<Dst>(rcv_flow_entt).dst = flow_event.dst;
    ctx.get<L4Port>(rcv_flow_entt).l4_port = flow_event.l4_port; //FIXME

    ctx.get<FlowSize>(rcv_flow_entt).flow_size = flow_event.flow_size;
    ctx.get<StartTime>(rcv_flow_entt).start_time = flow_event.start_time;
    ctx.get<StopTime>(rcv_flow_entt).stop_time = 0;

    ctx.get<NIC_ID>(rcv_flow_entt).nic_id = flow_event.src;

    ctx.get<FlowState>(rcv_flow_entt) = FlowState::UNCOMPLETE;

    ctx.get<LastCNPTimestamp>(rcv_flow_entt).last_cnp_timestamp = 0; //1ms

    ctx.get<RecvBytes>(rcv_flow_entt).recv_bytes = 0;

    ctx.get<PktBuf>(rcv_flow_entt).head = 0;
    ctx.get<PktBuf>(rcv_flow_entt).tail = 0;
    ctx.get<PktBuf>(rcv_flow_entt).cur_num = 0;
    ctx.get<PktBuf>(rcv_flow_entt).cur_bytes = 0;

    ctx.get<NICRate>(rcv_flow_entt).nic_rate = 1000LL*1000*1000*100; // 100 Gbps  
    ctx.get<HSLinkDelay>(rcv_flow_entt).HS_link_delay = HS_LINK_DELAY; // 1 us, 1000 ns 

    ctx.get<SimTime>(rcv_flow_entt).sim_time = sim_time;
    ctx.get<SimTimePerUpdate>(rcv_flow_entt).sim_time_per_update = LOOKAHEAD_TIME; //1000ns

    // Lock only around shared resource access
    ctx.data().flow_lock.lock();

    ctx.data().recv_flows[flow_event.dst][flow_id] = rcv_flow_entt;

    ctx.data().flow_lock.unlock();
//



// 3) snd flow in receiver side (send ACK/NACK/CNP pkt)
    Entity recv_snd_flow_entt = ctx.makeEntity<SndFlow>();

    ctx.get<FlowID>(recv_snd_flow_entt).flow_id = flow_id;
    ctx.get<Src>(recv_snd_flow_entt).src = flow_event.src;
    ctx.get<Dst>(recv_snd_flow_entt).dst = flow_event.dst;
    ctx.get<L4Port>(recv_snd_flow_entt).l4_port = flow_event.l4_port; //FIXME

    ctx.get<FlowSize>(recv_snd_flow_entt).flow_size = 0;
    ctx.get<StartTime>(recv_snd_flow_entt).start_time = flow_event.start_time;
    ctx.get<StopTime>(recv_snd_flow_entt).stop_time = 0;

    ctx.get<NIC_ID>(recv_snd_flow_entt).nic_id = flow_event.dst;
    // ctx.get<SndServerID>(recv_snd_flow_entt).snd_server_id = flow_events[i][6];
    // ctx.get<RecvServerID>(recv_snd_flow_entt).recv_server_id = flow_events[i][7];

    ctx.get<SndNxt>(recv_snd_flow_entt).snd_nxt = 0;
    ctx.get<SndUna>(recv_snd_flow_entt).snd_una = 0;
    ctx.get<FlowState>(recv_snd_flow_entt) = FlowState::UNCOMPLETE;
    ctx.get<Extra_1>(recv_snd_flow_entt).extra_1 =flow_event.extra_1;

    ctx.get<LastAckTimestamp>(recv_snd_flow_entt).last_ack_timestamp = 0; //1ms
    ctx.get<NxtPktEvent>(recv_snd_flow_entt).nxt_pkt_event = 0;

    ctx.get<PFCState>(recv_snd_flow_entt) = PFCState::RESUME;

    ctx.get<CC_Para>(recv_snd_flow_entt).m_rate = DCQCN_CurrentRate;
    ctx.get<CC_Para>(recv_snd_flow_entt).tar_rate = DCQCN_TargetRate;
    ctx.get<CC_Para>(recv_snd_flow_entt).dcqcn_Alpha = DCQCN_Alpha;
    ctx.get<CC_Para>(recv_snd_flow_entt).dcqcn_G = DCQCN_G;
    ctx.get<CC_Para>(recv_snd_flow_entt).dcqcn_AIRate = DCQCN_AIRate;
    ctx.get<CC_Para>(recv_snd_flow_entt).dcqcn_HAIRate = DCQCN_HAIRate; 
    ctx.get<CC_Para>(recv_snd_flow_entt).dcqcn_RateDecreaseInterval = DCQCN_RateDecreaseInterval;
    ctx.get<CC_Para>(recv_snd_flow_entt).dcqcn_RateIncreaseInterval = DCQCN_RateIncreaseInterval;
    ctx.get<CC_Para>(recv_snd_flow_entt).dcqcn_AlphaUpdateInterval = DCQCN_AlphaUpdateInterval;
    ctx.get<CC_Para>(recv_snd_flow_entt).dcqcn_IncreaseStageCount = DCQCN_IncreaseStageCount;
    ctx.get<CC_Para>(recv_snd_flow_entt).dcqcn_RateDecreaseNextTime = DCQCN_RateDecreaseNextTime;
    ctx.get<CC_Para>(recv_snd_flow_entt).dcqcn_RateIncreaseNextTime = DCQCN_RateIncreaseNextTime;
    ctx.get<CC_Para>(recv_snd_flow_entt).dcqcn_AlphaUpdateNextTime = DCQCN_AlphaUpdateNextTime;
    ctx.get<CC_Para>(recv_snd_flow_entt).dcqcn_RateIsDecreased = DCQCN_RateIsDecreased;
    ctx.get<CC_Para>(recv_snd_flow_entt).CNPState = false;

    ctx.get<PktBuf>(recv_snd_flow_entt).head = 0;
    ctx.get<PktBuf>(recv_snd_flow_entt).tail = 0;
    ctx.get<PktBuf>(recv_snd_flow_entt).cur_num = 0;
    ctx.get<PktBuf>(recv_snd_flow_entt).cur_bytes = 0;

    ctx.get<AckPktBuf>(recv_snd_flow_entt).head = 0;
    ctx.get<AckPktBuf>(recv_snd_flow_entt).tail = 0;
    ctx.get<AckPktBuf>(recv_snd_flow_entt).cur_num = 0;
    ctx.get<AckPktBuf>(recv_snd_flow_entt).cur_bytes = 0;

    ctx.get<NICRate>(recv_snd_flow_entt).nic_rate = 1000LL*1000*1000*100; // 100 Gbps
    ctx.get<HSLinkDelay>(recv_snd_flow_entt).HS_link_delay = HS_LINK_DELAY; // 1 us, 1000 ns 

    // ctx.get<SimTime>(recv_snd_flow_entt).sim_time = flow_event.start_time;
    ctx.get<SimTime>(recv_snd_flow_entt).sim_time = sim_time;
    // ctx.get<SimTime>(recv_snd_flow_entt).sim_time = 0;
    ctx.get<SimTimePerUpdate>(recv_snd_flow_entt).sim_time_per_update = LOOKAHEAD_TIME; //1000ns

    ctx.data().flow_lock.lock();

    ctx.data().recv_snd_flows[flow_event.dst][flow_id] = recv_snd_flow_entt;

    ctx.data().flow_lock.unlock();

//
// 4) recv flow in sender side (receive ACK/NACK/CNP pkt)
    Entity snd_recv_flow_entt = ctx.makeEntity<RecvFlow>();

    ctx.get<FlowID>(snd_recv_flow_entt).flow_id = flow_id;
    //
    ctx.get<Src>(snd_recv_flow_entt).src = flow_event.src;
    ctx.get<Dst>(snd_recv_flow_entt).dst = flow_event.dst;
    //
    ctx.get<L4Port>(snd_recv_flow_entt).l4_port = flow_event.l4_port; //FIXME

    ctx.get<FlowSize>(snd_recv_flow_entt).flow_size = 0;
    ctx.get<StartTime>(snd_recv_flow_entt).start_time = flow_event.start_time;
    ctx.get<StopTime>(snd_recv_flow_entt).stop_time = 0;

    ctx.get<NIC_ID>(snd_recv_flow_entt).nic_id = flow_event.dst;

    ctx.get<FlowState>(snd_recv_flow_entt) = FlowState::UNCOMPLETE;

    ctx.get<LastCNPTimestamp>(snd_recv_flow_entt).last_cnp_timestamp = 0; //1ms

    ctx.get<RecvBytes>(snd_recv_flow_entt).recv_bytes = 0;

    ctx.get<PktBuf>(snd_recv_flow_entt).head = 0;
    ctx.get<PktBuf>(snd_recv_flow_entt).tail = 0;
    ctx.get<PktBuf>(snd_recv_flow_entt).cur_num = 0;
    ctx.get<PktBuf>(snd_recv_flow_entt).cur_bytes = 0;

    ctx.get<NICRate>(snd_recv_flow_entt).nic_rate = 1000LL*1000*1000*100; // 100 Gbps  
    ctx.get<HSLinkDelay>(snd_recv_flow_entt).HS_link_delay = HS_LINK_DELAY; // 1 us, 1000 ns 

    ctx.get<SimTime>(snd_recv_flow_entt).sim_time = sim_time;
    ctx.get<SimTimePerUpdate>(snd_recv_flow_entt).sim_time_per_update = LOOKAHEAD_TIME; //1000ns

    // Lock only around shared resource access
    ctx.data().flow_lock.lock();

    ctx.data().snd_recv_flows[flow_event.src][flow_id] = snd_recv_flow_entt;

    ctx.data().flow_lock.unlock();

    // printf("Exiting create_flow for flow_id: %lu\n", flow_id);
}


uint32_t flow_id = 0;

inline void setFlow(Engine &ctx, uint64_t comm_src, uint64_t comm_dst, uint64_t flow_size, uint32_t &flow_id) {
    ctx.data().flow_lock.lock();
    flow_id++;
    ctx.data().flow_lock.unlock();

    Entity net_npu = ctx.data()._net_npus[comm_src]; 
    int64_t sim_time = ctx.get<SimTime>(net_npu).sim_time;
    FlowEvent flow_event = {flow_id, (uint32_t)comm_src, (uint32_t)comm_dst, 1, 0, flow_size, sim_time, 0, FlowState::UNCOMPLETE};
    
    ctx.data().flow_lock.lock();
    _enqueue_flow(ctx.get<NewFlowQueue>(net_npu), flow_event);
    ctx.data().flow_lock.unlock(); 

    printf("set flow id %d: %d->%d %d, \n", flow_id, comm_src, comm_dst, flow_size);
}


inline void comm_set_flow(Engine &ctx, NET_NPU_ID _net_npu_id,
                       NewFlowQueue &_new_flow_queue, SimTime &_sim_time,  
                       SimTimePerUpdate &_sim_time_per_update) {
    
    #if PRINT_SYS_LOG
    if (_net_npu_id.net_npu_id == 0) {
        printf("*********Enter into comm_set_flow, net_npu_id: %u\n", _net_npu_id.net_npu_id);
        printf("comm_set_flow: _new_flow_queue, before enqueue, len = %d\n", get_queue_len(_new_flow_queue));
    }
    #endif

    if(_sim_time.sim_time % 10000 != 0) {return;}
    // int k = 4;
    // int net_npu_num = k*k*k*1/4;   
    // if (_net_npu_id.net_npu_id >= 3) return;
    // printf("net_npu_num: %d, net_npu_id: %d\n", net_npu_num, _net_npu_id.net_npu_id);

    if (_net_npu_id.net_npu_id == 0) {
        // uint32_t src = _net_npu_id.net_npu_id;
        // uint32_t dst = _net_npu_id.net_npu_id+1;
        uint32_t src = _net_npu_id.net_npu_id;
        uint32_t dst = 1;
        // uint64_t flow_id = 0;
        uint64_t flow_size = 1000;

        // Lock only around shared resource access
        setFlow(ctx, src, dst, flow_size, flow_id);
    }

    if (_net_npu_id.net_npu_id == 2) {
        // uint32_t src = _net_npu_id.net_npu_id;
        // uint32_t dst = _net_npu_id.net_npu_id+1;
        uint32_t src = _net_npu_id.net_npu_id;
        uint32_t dst = 3;
        // uint64_t flow_id = 0;
        uint64_t flow_size = 1000;
        
        setFlow(ctx, src, dst, flow_size, flow_id);
    }

    #if PRINT_SYS_LOG
    if (_net_npu_id.net_npu_id == 0) {
        printf("Exiting create_flow for net_npu_id: %u\n", _net_npu_id.net_npu_id);
    }
    #endif
}





// // network interface
// // 检测/获取流完成事件：返回流完成数量，同时把流完成事件赋值flows_finish
// // func
// // 系统层定义的结构体
// struct SysFlow {
//     uint32_t id;
//     uint64_t comm_size;
//     uint64_t comm_src;
//     uint64_t comm_dst;
//     uint32_t durationMicros;
// };


inline int checkFlowFinish(Engine &ctx, uint32_t net_npu_id, SysFlow flows_finish[])
{
    Entity net_npu = ctx.data()._net_npus[net_npu_id]; 
    CompletedFlowQueue _completed_flow_queue =  ctx.get<CompletedFlowQueue>(net_npu);
    uint32_t flow_num_finish = get_queue_len(_completed_flow_queue);
    for (uint32_t i = 0; i < flow_num_finish; i++)
    {
        FlowEvent flow_event;
        _dequeue_flow(_completed_flow_queue, flow_event);

        flows_finish[i].id = flow_event.flow_id;
        flows_finish[i].comm_size = flow_event.flow_size;
        flows_finish[i].comm_src = flow_event.src;
        flows_finish[i].comm_dst = flow_event.dst;
        flows_finish[i].durationMicros = flow_event.stop_time - flow_event.start_time;
        flows_finish[i].state=TaskState::FINISH;
        printf("######### check flow finish: flow id %d: %d->%d %d, #########\n",flows_finish[i].id, flows_finish[i].comm_src,flows_finish[i].comm_dst,flows_finish[i].comm_size);
    }

    return flow_num_finish;
}



inline void setup_flow(Engine &ctx, NET_NPU_ID _net_npu_id,
                       NewFlowQueue &_new_flow_queue, SimTime &_sim_time,
                       SimTimePerUpdate &_sim_time_per_update) {
    #if PRINT_SYS_LOG
    printf("Entering setup_flow for NET_NPU_ID: %d\n", _net_npu_id.net_npu_id);
    #endif

    uint32_t flow_event_num = get_queue_len(_new_flow_queue);
    for (uint32_t i = 0; i < flow_event_num; i++) {
        FlowEvent flow_event;
        _dequeue_flow(_new_flow_queue, flow_event);

        //printf("Dequeued flow_event with flow_id: %lu\n", flow_event.flow_id);

        // Lock only around shared resource access
        // ctx.data().flow_lock.lock();
        create_flow(ctx, flow_event, flow_event.flow_id, _sim_time.sim_time);
        // ctx.data().flow_lock.unlock();

        //printf("Created flow with flow_id: %lu\n", flow_event.flow_id);
    }

    clear_queue(_new_flow_queue);
    _sim_time.sim_time += _sim_time_per_update.sim_time_per_update;

    #if PRINT_SYS_LOG
    printf("Exiting setup_flow for NET_NPU_ID: %d\n", _net_npu_id.net_npu_id);
    #endif
}


inline void check_flow_state(Engine &ctx, NET_NPU_ID &_net_npu_id, CompletedFlowQueue &_completed_flow_queue,
                             SimTime &_sim_time, SimTimePerUpdate &_sim_time_per_update) {

    // printf("xx1\n");
    
    #if PRINT_SYS_LOG
    if (_net_npu_id.net_npu_id == 0||_net_npu_id.net_npu_id == 1) {// printf("*********Enter into check_flow_state:*********\n");}
    // else {return;}
    }
    #endif
    
    uint64_t tmp_flow_ids[MAP_SIZE];
    memset(tmp_flow_ids, 0, MAP_SIZE*sizeof(uint64_t));
    uint32_t cnt = 0; 

    Map<uint64_t, Entity> all_snd_flow = ctx.data().snd_flows[_net_npu_id.net_npu_id];

    for (auto it = all_snd_flow.begin(); it != all_snd_flow.end(); ++it) {
        auto& entry = *it;
        Entity snd_flow_entt = entry.value;

        if (ctx.get<FlowState>(snd_flow_entt) == FlowState::COMPLETE) {
            FlowEvent flow_event = {ctx.get<FlowID>(snd_flow_entt).flow_id, ctx.get<Src>(snd_flow_entt).src, ctx.get<Dst>(snd_flow_entt).dst, \
                                    ctx.get<L4Port>(snd_flow_entt).l4_port, ctx.get<NIC_ID>(snd_flow_entt).nic_id, \
                                    ctx.get<FlowSize>(snd_flow_entt).flow_size, ctx.get<StartTime>(snd_flow_entt).start_time, \
                                    ctx.get<StopTime>(snd_flow_entt).stop_time, ctx.get<FlowState>(snd_flow_entt),ctx.get<Extra_1>(snd_flow_entt).extra_1};
            _enqueue_flow(_completed_flow_queue, flow_event);
            // printf("check_flow_state: _completed_flow_queue: %d\n", get_queue_len(_completed_flow_queue));
            //

            uint64_t flow_id = ctx.get<FlowID>(snd_flow_entt).flow_id;
            tmp_flow_ids[cnt++] = flow_id;
            uint32_t src = ctx.get<Src>(snd_flow_entt).src;
            uint32_t dst = ctx.get<Dst>(snd_flow_entt).dst;

            Entity recv_flow_entt = ctx.data().recv_flows[dst][flow_id];
            ctx.destroyEntity(recv_flow_entt);
            // Lock only around shared resource access
            ctx.data().flow_lock.lock();
            ctx.data().recv_flows[dst].remove(flow_id);
            ctx.data().flow_lock.unlock();

            Entity recv_snd_flow_entt = ctx.data().recv_snd_flows[dst][flow_id];
            ctx.destroyEntity(recv_snd_flow_entt);
            // Lock only around shared resource access
            ctx.data().flow_lock.lock();
            ctx.data().recv_snd_flows[dst].remove(flow_id);
            ctx.data().flow_lock.unlock();

            Entity snd_recv_flow_entt = ctx.data().snd_recv_flows[src][flow_id];
            ctx.destroyEntity(snd_recv_flow_entt);       
            // Lock only around shared resource access
            ctx.data().flow_lock.lock();
            ctx.data().snd_recv_flows[src].remove(flow_id);
            ctx.data().flow_lock.unlock();

            ctx.destroyEntity(snd_flow_entt);
        }
    }

    for (uint32_t i = 0; i < cnt; i++) {
        ctx.data().snd_flows[_net_npu_id.net_npu_id].remove(tmp_flow_ids[i]);
    }

    SysFlow flows_finish[100];
    uint32_t flow_num_finish = 0;
    flow_num_finish = checkFlowFinish(ctx, _net_npu_id.net_npu_id, flows_finish);
    if (flow_num_finish > 0) {
        printf("flow_num_finish: %d\n", flow_num_finish);
    }
    // printf("check_flow_state: before clear_queue: _completed_flow_queue: %d\n", get_queue_len(_completed_flow_queue));
    clear_queue(_completed_flow_queue);
    // printf("check_flow_state: after clear_queue: _completed_flow_queue: %d\n", get_queue_len(_completed_flow_queue));
    // printf("xx2\n");
}




inline void flow_send(Engine &ctx, FlowID &_flow_id, Src &_src, Dst &_dst, L4Port &_l4_port, 
                      FlowSize &_flow_size, StartTime &_start_time, StopTime &_stop_time,
                      NIC_ID &_nic_id, SndServerID &_snd_server_id, RecvServerID &_recv_server_id,
                      SndNxt &_snd_nxt, SndUna &_snd_una, FlowState &_flow_state, 
                      LastAckTimestamp &_last_ack_timestamp, NxtPktEvent &_nxt_pkt_event, 
                      PFCState &_pfc_state, CC_Para &_cc_para, 
                      PktBuf &_snd_buf, AckPktBuf &_ack_buf, 
                      NICRate &_nic_rate, HSLinkDelay &_HS_link_delay, 
                      SimTime &_sim_time, SimTimePerUpdate &_sim_time_per_update) {
    // Entity in_port = ctx.data().inPorts[_next_hop.next_hop];
    int64_t end_time = _sim_time.sim_time + _sim_time_per_update.sim_time_per_update;
    #if PRINT_SYS_LOG
    // if (_flow_id.flow_id == 0 || _flow_id.flow_id == 1) {
    printf("\n*******Enter into flow send*********\n");
    printf("src: %d, dst: %d, flow_id: %lu, start _sim_time.sim_time: %ld, end_time: %ld\n", _src.src, _dst.dst, _flow_id.flow_id, _sim_time.sim_time, end_time);
    // printf("end_time: %ld\n", end_time);
    printf("cc target rate: %.3lf, cc cur rate: %.3lf\n", _cc_para.tar_rate/(1000*1000*1000.0), _cc_para.m_rate/(1000*1000*1000.0));
    // }
    #endif

    // uint16_t flow_id = 0;
    uint16_t header_len = 40; 
    uint8_t flow_priority = 0;
    uint32_t path[MAX_PATH_LEN];
    memset(path, 0, MAX_PATH_LEN*sizeof(uint32_t));

    Pkt pkt;
    // clear_queue(_snd_buf);
    int16_t sent_bytes = 0; 
    while (_sim_time.sim_time < end_time) {
        //// printf("send sim_time: %lld\n", _sim_time.sim_time);   
        //// printf("_sim_time.sim_time < end_time");
        Pkt ack_p;
        // if (_flow_id.flow_id == 0) { 
        //     // printf("sim_time: %d, nxt_pkt_event: %d\n", _sim_time.sim_time, _nxt_pkt_event.nxt_pkt_event);
        // }

        // new arrival of non-data(ACK/NACK/CNP) packet
        uint16_t ack_pkt_cnt = 0;
        while (fetch_elem(_ack_buf, 0, ack_p)) {  // if ack pkt is available
            if ((ack_p.enqueue_time >= end_time) && (ack_p.enqueue_time > _nxt_pkt_event.nxt_pkt_event &&_snd_nxt.snd_nxt != _flow_size.flow_size)) {
                break;
            }

            // if (_flow_id.flow_id == 0) { 
            //     //// printf("ack_p.enqueue_time: %ld, nxt_pkt_event: %ld\n", ack_p.enqueue_time, _nxt_pkt_event.nxt_pkt_event);
            // }

            _dequeue(_ack_buf, ack_p);

            // if (ack_p.dst == _src.src) {// reaction for ACK/NACK packets
            #if PRINT_PKT_LOG
            PrintPkt(ack_p, "*ACK received by sender: ");
            #endif
            if (ack_p.pkt_type == PktType::ACK) { // ACK
                _snd_una.snd_una = ack_p.sq_num;
                // printf("sender id: %d, variable set: snd_una: %ld\n", _src.src, _snd_una.snd_una);

                if (_snd_una.snd_una >= _flow_size.flow_size) {
                    // _nxt_pkt_event.nxt_pkt_event = INT64_MAX;
                    if (_flow_size.flow_size > 0 && _flow_state == FlowState::UNCOMPLETE) {
                        printf("flow sender id: %d, flow receiver id: %d, flow id is %lu, flow is finished\n", _src.src, _dst.dst, _flow_id.flow_id, _snd_una.snd_una);
                    }

                    _flow_state = FlowState::COMPLETE;
                    break;
                }

                _last_ack_timestamp.last_ack_timestamp = _sim_time.sim_time;
            }
            else if (ack_p.pkt_type == PktType::NACK) { // NACK
                _snd_nxt.snd_nxt = ack_p.sq_num;
                _snd_una.snd_una = ack_p.sq_num;
                //// printf("variable set: snd_una: %ld, snd_nxt: %ld\n", _snd_una.snd_una,_snd_nxt.snd_nxt);

                _last_ack_timestamp.last_ack_timestamp = _sim_time.sim_time;
            }
            else if (ack_p.pkt_type == PktType::CNP){ // CNP
                _cc_para.CNPState = true;
                dcqcn_rate_decrease(_cc_para, ack_p.src, _sim_time.sim_time);
                dcqcn_alpha_update(_cc_para);
                _cc_para.CNPState = false;
                //record the rate is decreased
                //use it to determine whether up
                _cc_para.dcqcn_RateIsDecreased = true;
            }
            else { //PFC pause/resume packet 
                if (ack_p.pkt_type == PktType::PFC_PAUSE) {
                    _pfc_state = PFCState::PAUSE;
                }
                else {
                        _pfc_state = PFCState::RESUME;
                }
            }
            // }
            // else { // the ACK/NACK packet is not for me, send it out by the sender's receiver 
            //     _enqueue(_snd_buf, ack_p);
            // }
            _sim_time.sim_time += 1; // increase 1 ns
            ack_pkt_cnt += 1;
        }

        // DCQCN timer function
        // the order should be: check if decrease, update alpha, increase send rate
        dcqcn_timer_reset_if_decrease(_cc_para, _sim_time.sim_time);
        dcqcn_timer_alpha_update(_cc_para, _sim_time.sim_time);
        dcqcn_timer_rate_increase(_cc_para, _src.src, _sim_time.sim_time);


        // if (_sender_id.sender_id == 0) {
        //     // printf("snd_una: %ld, snd_nxt: %ld\n", _snd_una.snd_una,_snd_nxt.snd_nxt);
        // }

        if (_snd_una.snd_una >= _flow_size.flow_size) {
            _flow_state = FlowState::COMPLETE;

            // printf("Flow is finished: sender net_npu id: %d, recv net_npu id: %d, flow id : %d, _start_time: %ld ns, stop time : %ld ns\n", _src.src, _dst.dst, _flow_id.flow_id, _start_time.start_time, _stop_time.stop_time);
            break;
        }

        if (_sim_time.sim_time > _nxt_pkt_event.nxt_pkt_event) {
            _nxt_pkt_event.nxt_pkt_event = _sim_time.sim_time;
        }
        else {
            _sim_time.sim_time = _nxt_pkt_event.nxt_pkt_event;
        }
        // } // cannot handle when _sim_time.sim_time == nxt_pkt_event

        if ((_sim_time.sim_time - _last_ack_timestamp.last_ack_timestamp) >= RETRANSMIT_TIMER) {
           _snd_nxt.snd_nxt = _snd_una.snd_una;
            _last_ack_timestamp.last_ack_timestamp = _sim_time.sim_time;
        }

        // there must no new send event in this time frame
        if (ack_pkt_cnt == 0 && _snd_nxt.snd_nxt == _flow_size.flow_size) {break;}

        // FIXME TBD
        // the beggining of flow
        if (_snd_nxt.snd_nxt == 0) { 
            if (_start_time.start_time >= _sim_time.sim_time && _start_time.start_time <= end_time) {
                _sim_time.sim_time = _start_time.start_time;
            }
            else if (_start_time.start_time > end_time) {
                break;
            }
        }
        //

        if (_pfc_state == PFCState::PAUSE) {
            // printf("In send phase: PFCState::PAUSE\n");
        }

        if (_flow_state == FlowState::UNCOMPLETE && _snd_nxt.snd_nxt < _flow_size.flow_size && _sim_time.sim_time >= _start_time.start_time && _pfc_state == PFCState::RESUME) { // the flow is unfinished
            // flow is not finished
            uint64_t remain_data_len = (_flow_size.flow_size -_snd_nxt.snd_nxt);

            // if (remain_data_len <= 0 ) {continue;}

            uint16_t payload_len = remain_data_len;
            if (remain_data_len > (MTU-header_len)) {   
                payload_len = MTU-header_len;
            }
            uint16_t pkt_size = payload_len+header_len;
            int64_t dequeue_time = _sim_time.sim_time + (pkt_size*8*1.0/_nic_rate.nic_rate)*(1000*1000*1000); // unit is ns
            //int64_t enqueue_time = dequeue_time + _HS_link_delay.HS_link_delay;
            int64_t enqueue_time = dequeue_time;

            PktType pkt_type = PktType::DATA;
            
            int64_t sq_num = _snd_nxt.snd_nxt;

            // Pkt pkt;
            create_pkt(pkt, _src.src, _dst.dst, _l4_port.l4_port, header_len, payload_len, header_len+payload_len, sq_num, _flow_id.flow_id, enqueue_time, dequeue_time, flow_priority, pkt_type, path, 0, ECN_MARK::NO);

            #if PRINT_PKT_LOG
            PrintPkt(pkt, "flow send by sender: ");
            #endif

            _enqueue(_snd_buf, pkt);
            // printf("flow_send: _snd_buf, after enqueue: %d\n", get_queue_len(_snd_buf));

           _snd_nxt.snd_nxt += payload_len;

            // nxt_pkt_event must be >= dequeue_time, because cur_rate(m_rate) must be <= nic_rate;
            _nxt_pkt_event.nxt_pkt_event = _sim_time.sim_time + (pkt_size*8*1.0/_cc_para.m_rate)*(1000*1000*1000);

            _sim_time.sim_time = _sim_time.sim_time > dequeue_time ? _sim_time.sim_time : dequeue_time;
            //if (pkt_cnt > 20) break;

            sent_bytes += pkt_size;

            // FIXME TBD
            if (_snd_una.snd_una == _flow_size.flow_size) {
                _flow_state = FlowState::COMPLETE;
                _stop_time.stop_time = _sim_time.sim_time;
                if (_flow_size.flow_size > 0) {
                    printf("Flow is finished: sender net_npu id: %d, recv net_npu id: %d, flow id : %lu, _start_time: %ld ns, stop time : %ld ns\n", _src.src, _dst.dst, _flow_id.flow_id, _start_time.start_time, _stop_time.stop_time);
                }
                break;
            }
        }
    }

    // if (_sender_id.sender_id == 0 || _sender_id.sender_id == 1) {
    // if (_sender_id.sender_id == 0) {
        // // printf("sender: %d, sim_time: %ld,, send rate is %.2f Gbps\n", _sender_id.sender_id, _sim_time.sim_time, sent_bytes*8*1.0/_sim_time_per_update.sim_time_per_update);
    // }
    _sim_time.sim_time = end_time;
    if (_sim_time.sim_time > _nxt_pkt_event.nxt_pkt_event) {
        _nxt_pkt_event.nxt_pkt_event = _sim_time.sim_time;
    }
}


inline void clear_q(Engine &ctx, FlowID &_flow_id, Src &_src, Dst &_dst, L4Port &_l4_port, 
                      FlowSize &_flow_size, StartTime &_start_time, StopTime &_stop_time,
                      NIC_ID &_nic_id, SndServerID &_snd_server_id, RecvServerID &_recv_server_id,
                      SndNxt &_snd_nxt, SndUna &_snd_una, FlowState &_flow_state, 
                      LastAckTimestamp &_last_ack_timestamp, NxtPktEvent &_nxt_pkt_event, 
                      PFCState &_pfc_state, CC_Para &_cc_para, 
                      PktBuf &_snd_buf, AckPktBuf &_ack_buf, 
                      NICRate &_nic_rate, HSLinkDelay &_HS_link_delay, 
                      SimTime &_sim_time, SimTimePerUpdate &_sim_time_per_update) {
    clear_queue(_snd_buf);
}



inline void nic_forward(Engine &ctx, NIC_ID &_nic_id, NICRate &_nic_rate, SimTime &_sim_time, 
                        SimTimePerUpdate &_sim_time_per_update, BidPktBuf &_bid_pkt_buf) {
    #if PRINT_SYS_LOG
    if (_nic_id.nic_id == 0) {
        printf("\n*******Enter into nic_forward*********\n");
        printf("start _sim_time.sim_time: %ld, end_time: %ld\n", _sim_time.sim_time, _sim_time.sim_time+_sim_time_per_update.sim_time_per_update);
    }
    #endif

    //snd flow in sender side (send data pkt)
    Map<uint64_t, Entity> tmp_snd_flow = ctx.data().snd_flows[_nic_id.nic_id];
    for (auto it = tmp_snd_flow.begin(); it != tmp_snd_flow.end(); ++it) {
        auto& entry = *it;
        Entity flow_entt = entry.value;
        // printf("nic_id: %d\n", _nic_id.nic_id);
        // printf("src: %d, dst: %d, flow_id: %lu\n", ctx.get<Src>(flow_entt).src, ctx.get<Dst>(flow_entt).dst, ctx.get<FlowID>(flow_entt).flow_id);
        PktBuf snd_flow_buf = ctx.get<PktBuf>(flow_entt);
        
        uint16_t pkt_num = get_queue_len(snd_flow_buf);
        Pkt pkt;
        for (uint16_t k = 0; k < pkt_num; k++) {
            _dequeue(snd_flow_buf, pkt);

            // printf("nic_forward: after dequeue, snd_flow_buf: %d\n", get_queue_len(snd_flow_buf));
            _enqueue(_bid_pkt_buf.snd_buf, pkt);
            #if PRINT_PKT_LOG
            PrintPkt(pkt, "nic_forward(data pkt): ");
            #endif
            // printf("nic_forward: after enqueue, _bid_pkt_buf.snd_buf: %d\n", get_queue_len(_bid_pkt_buf.snd_buf));
        }
        clear_queue(ctx.get<PktBuf>(flow_entt));
        // Entity snd_flow = ctx.data().snd_flows[_nic_id.nic_id][entry.key];
        // clear_queue(ctx.get<PktBuf>(snd_flow));
        
        // printf("nic_forward: after clear_queue, snd_flow_buf: %d\n", get_queue_len(snd_flow_buf));
    }
    
    // snd flow in receiver side (send ACK/NACK/CNP pkt)
    Map<uint64_t, Entity> tmp_recv_snd_flow = ctx.data().recv_snd_flows[_nic_id.nic_id];
    for (auto it = tmp_recv_snd_flow.begin(); it != tmp_recv_snd_flow.end(); ++it) {
        auto& entry = *it;
        Entity flow_entt = entry.value;

        PktBuf recv_snd_flow_buf = ctx.get<PktBuf>(flow_entt);
        
        // printf("\nnic_id: %d\n", ctx.get<NIC_ID>(flow_entt).nic_id);
        uint16_t pkt_num = get_queue_len(recv_snd_flow_buf);
        // printf("pkt num of recv_snd_flow_buf: %d\n", pkt_num);
        Pkt pkt;
        for (uint16_t k = 0; k < pkt_num; k++) {
            _dequeue(recv_snd_flow_buf, pkt);

            // PrintPkt(pkt, "nic_forward: ");
            // printf("nic_forward: after dequeue, recv_snd_flow_buf: %d\n", get_queue_len(recv_snd_flow_buf));
            _enqueue(_bid_pkt_buf.snd_buf, pkt);
            #if PRINT_PKT_LOG
            PrintPkt(pkt, "nic_forward(ACK/NACK/CNP): ");
            #endif
            // printf("nic_forward: after enqueue, _bid_pkt_buf.snd_buf: %d\n", get_queue_len(_bid_pkt_buf.snd_buf));
        }
        clear_queue(ctx.get<PktBuf>(flow_entt));
        // Entity recv_snd_flow = ctx.data().recv_snd_flows[0][entry.key];
        // clear_queue(ctx.get<PktBuf>(recv_snd_flow));
        
        // printf("nic_forward: after clear_queue, recv_snd_flow_buf: %d\n", get_queue_len(recv_snd_flow_buf));
    }
}


inline void insertionSort(PktBuf &data) {
        int32_t i, j;
        Pkt key;
        uint16_t len = get_queue_len(data);
        for (i = 1; i < len; i++) {
            key = get_elem(data, i);
            j = i - 1;

            // Move elements of data[0..i-1] that are greater than key.enqueue_time to one position ahead of their current position
            while (j >= 0 && get_elem(data, j).enqueue_time > key.enqueue_time) {
                get_elem(data, j+1) = get_elem(data, j);

                j = j - 1;
            }
            get_elem(data, j+1) = key;
        }
}


inline void nic_transmit(Engine &ctx, NIC_ID &_nic_id, 
                         NICRate &_nic_rate, HSLinkDelay &_HS_link_delay,
                         SimTime &_sim_time, SimTimePerUpdate &_sim_time_per_update, 
                         BidPktBuf &_bid_pkt_buf, TXHistory &_tx_history, 
                         NextHop &_next_hop, Seed &_seed) {
    int64_t end_time = _sim_time.sim_time + _sim_time_per_update.sim_time_per_update;
    
    #if PRINT_SYS_LOG
    if (_nic_id.nic_id == 0) {
        printf("\n*******Enter into nic_transmit*********\n");
        printf("start _sim_time.sim_time: %ld, end_time: %ld\n", _sim_time.sim_time, end_time);
    }
    #endif
    insertionSort(_bid_pkt_buf.snd_buf);

    Entity in_port = ctx.data().inPorts[_next_hop.next_hop];

    Pkt pkt;
    while(_sim_time.sim_time < end_time) {
        if(!fetch_elem(_bid_pkt_buf.snd_buf, 0, pkt)) break;
        if(pkt.enqueue_time >= end_time) break;
        _dequeue(_bid_pkt_buf.snd_buf, pkt);
        
        // printf("nic_transmit: after dequeue, _bid_pkt_buf.snd_buf: %u\n", get_queue_len(_bid_pkt_buf.snd_buf));
        
        uint32_t now_queue_len = 0; // record the queue length when a packet comes in real time
        int loc;  // FIXME
        for (loc = get_queue_len(_tx_history); loc > 0; loc--) {
            TXElem tx_elem = {0, 0};
            fetch_elem(_tx_history, loc-1, tx_elem);
            // // printf("tx_elem.dequeue_time: %ld\n", tx_elem.dequeue_time);
            if (pkt.enqueue_time < tx_elem.dequeue_time) {
                now_queue_len += tx_elem.ip_pkt_len;
            }
            else {
                break;
            }
        }

        if (loc > 0) {
            TXElem tmp_tx_elem;
            rm_queue_range(_tx_history, tmp_tx_elem, 0, loc); // FIXME, only handle when using 1 priority queue
        }

        // tail drop
        if ((pkt.payload_len+now_queue_len) > BUF_SIZE) {
            continue;
        }

        pkt.dequeue_time = (pkt.enqueue_time > _sim_time.sim_time ? pkt.enqueue_time : _sim_time.sim_time) + \
                             (pkt.ip_pkt_len*8*1.0/_nic_rate.nic_rate)*(1000*1000*1000);
        pkt.enqueue_time = pkt.dequeue_time + _HS_link_delay.HS_link_delay; //FIXME


        TXElem tx_elem = {pkt.dequeue_time, pkt.ip_pkt_len};
        _enqueue(_tx_history, tx_elem); 

        // ECN marking  
        LCG lcg(48271, 0, 2147483647, _seed.seed);
        float mark_prob;
        float rand_num;
        if (now_queue_len <= K_MIN && pkt.ecn == ECN_MARK::NO) {
            pkt.ecn = ECN_MARK::NO;
        }
        else if (now_queue_len > K_MIN && now_queue_len < K_MAX) {
            mark_prob = (now_queue_len-K_MIN)*P_MAX/(K_MAX-K_MIN);
            rand_num = (lcg.next()%1000)/1000.0;
            // // printf("rand num is : %f\n", rand_num);
            if (rand_num < mark_prob) {pkt.ecn = ECN_MARK::YES;}
            _seed.seed = lcg.next();
        }
        else {
            pkt.ecn = ECN_MARK::YES;
        }

        _enqueue(ctx.get<PktBuf>(in_port), pkt);

        #if PRINT_PKT_LOG
        PrintPkt(pkt, "nic_transmit: ");
        #endif

        _sim_time.sim_time = pkt.dequeue_time;
    }

    _sim_time.sim_time = end_time;
}


//******************************************************************************************
// the forward system is split into 3 subsystems: set_forward_plan, _forward and remove_pkts
// set_forward_plan on *ingress* port
template <typename T>
void printBits(T value) {
    for(CountT i = sizeof(T) * 8 - 1; i >= 0; i--) {
        if(value & (1 << i))  printf("1");
        else  printf("0");
    }
    // printf("\n");
}

inline void PrintPath(uint32_t *path, uint8_t path_len) {
    printf("path: length: %d: ", path_len);
    for(int32_t i = 0; i < path_len/2; i++) {
        // for (int32_t k = 2*i; k < 2 * (i+1); k++) {
        printf("(%d, %d), ", path[2*i], path[2*i+1]);
    }
    printf("\n");
    // for(int32_t i = 0; i < path_len; i++) {
    //     // for (int32_t k = 2*i; k < 2 * (i+1); k++) {
    //     // printf("%d, ", path[i]);
    // }
    // // printf("\n");
}

inline void set_forward_plan(Engine &ctx,
                            LocalPortID &_local_port_id,
                            GlobalPortID &_global_port_id,
                            SwitchID &_switch_id,
                            PktBuf &_queue,
                            ForwardPlan &_forward_plan,
                            SimTime & _sim_time,
                            SimTimePerUpdate &_sim_time_per_update)
{   
    int64_t end_time = _sim_time.sim_time + _sim_time_per_update.sim_time_per_update;

    #if PRINT_SYS_LOG
    if (_global_port_id.global_port_id == 0) {
        printf("\n\n*******Enter into set_forward_plan sys*********\n\n");
        printf("start _sim_time.sim_time: %ld, end_time: %ld\n", _sim_time.sim_time, end_time);
    }
    #endif
    //reset
    for (uint32_t i = 0; i < INPORT_NUM; i++) {_forward_plan.forward_plan[i] = 0;}


    // // printf("_switch_id: %d, _local_port_id: %d, global_port_id: %d\n", \
    //         _switch_id.switch_id, _local_port_id.local_port_id, _global_port_id.global_port_id);
    
    uint16_t q_len = get_queue_len(_queue);
    //if (q_len > 0 & _global_port_id.global_port_id == 0) PrintQueue(_queue);

    // if (_global_port_id.global_port_id == 0) {
    //     // printf("set_forward_plan start sim_time: %lld\n", _sim_time.sim_time);
    //     // printf("end_time: %lld\n", end_time);
    //     // printf("the length of ingress queue: %d\n", q_len);
    //     // printf("q_tail and q_head are: %d and %d\n", _queue.tail, _queue.head);
    // }
    // // if (_switch_id.switch_id == 8) {
    // // printf("_switch_id: %d, _local_port_id: %d, global_port_id: %d\n", \
    //         _switch_id.switch_id, _local_port_id.local_port_id, _global_port_id.global_port_id);
    // }
    uint16_t idx = 0;
    while(q_len--) { // at most processing q_len packets
        Pkt pkt;
        // if(idx > 20) break;
        // fetch_elem packet from buffer while checking whether buffer is empty
        if(!fetch_elem(_queue, idx, pkt)) break;

        if(pkt.enqueue_time >= end_time) break;
        // if (pkt.ecn == ECN_MARK::YES) {
        //PrintPkt(pkt, "forward queue: ");
        // }
        //// printf("pkt.enqueue_time: %lld, end_time: %lld\n", pkt.enqueue_time, end_time);

        if (pkt.pkt_type != PktType::PFC_PAUSE && pkt.pkt_type != PktType::PFC_RESUME) {
            int32_t sw_idx = _switch_id.switch_id;
            //// printf("sw_idx: %d, ", sw_idx);
            Entity sw_id = ctx.data()._switches[sw_idx];
            // hash
            // uint32_t port_idx = crc32_hash(pkt.src, pkt.dst, );             
            //
            // int32_t next_hop_link_idx = ctx.get<FIBTable>(sw_id).fib_table[pkt.dst][pkt.flow_id];
            int32_t next_hop_link_idx = ctx.get<FIBTable>(sw_id).fib_table[pkt.dst][0];
            _forward_plan.forward_plan[next_hop_link_idx] = _forward_plan.forward_plan[next_hop_link_idx] | (1<<idx);
        }
        //  broadcast PFC pause/resume frame into all egress port excepting for the current port itself
        else {
            if (pkt.dst == 0) {// the pfc frame is received from the downstream egress port 
                Entity e_port = ctx.data().ePorts[_global_port_id.global_port_id]; 
                // the idx of priority queue is placed in the filed of pkt.flow_id
                // the PFC state is placed in the filed of the pkt.pkt_type
                if (pkt.pkt_type == PktType::PFC_PAUSE)
                    ctx.get<PktQueue>(e_port).queue_pfc_state[pkt.flow_id] = PFCState::PAUSE;
                else 
                    ctx.get<PktQueue>(e_port).queue_pfc_state[pkt.flow_id] = PFCState::RESUME;
            }

            for (uint32_t i = 0; i < INPORT_NUM; i++) { // broadcast
                if (i != _local_port_id.local_port_id) 
                    _forward_plan.forward_plan[i] = _forward_plan.forward_plan[i] | (1<<idx);
            }
        }

        idx += 1;
    }
    // // printf("switch_id %d, local_port_id: %d\n", _switch_id.switch_id,_local_port_id.local_port_id);
    // if (_switch_id.switch_id == 1 && _local_port_id.local_port_id == 2) {
    //     // printf("Forward plan of the %d-th ingress port in %d-th switch: \n", _local_port_id.local_port_id, _switch_id.switch_id);
    //     for(CountT i = 0; i < INPORT_NUM; i++) {
    //         // printf("towards %d-th egress port: ", i);
    //         printBits<uint16_t>(_forward_plan.forward_plan[i]);
    //     }
    //     // printf("\n");
    // }
}


// on *egress* ports
// each egress port actively fetches packets from in_ports
inline void _forward(Engine &ctx,
                    SchedTrajType sched_traj_type,
                    LocalPortID &_local_port_id,
                    GlobalPortID &_global_port_id,
                    PktQueue &_queue,
                    SwitchID &_switch_id,
                    SimTime &_sim_time,
                    SimTimePerUpdate &_sim_time_per_update)
{
    // FIXME: modify it to support the any shape of network topo
    int64_t end_time = _sim_time.sim_time + _sim_time_per_update.sim_time_per_update;

    #if PRINT_SYS_LOG
    if (_global_port_id.global_port_id == 0) {
        printf("\n*******Enter into _forward sys*********\n");
        printf("start _sim_time.sim_time: %ld, end_time: %ld\n", _sim_time.sim_time, end_time);
    }
    #endif
    // if (_global_port_id.global_port_id == 5) {
    //     return;
    // }
    Entity in_port;
    uint16_t fwd_bitmap;
    // int32_t _numInPort = ctx.data().numInPort;

    // coupled with fattree topology
    for(CountT i = _switch_id.switch_id*K_ARY; i < (_switch_id.switch_id+1)*K_ARY; i++) { 

        in_port = ctx.data().inPorts[i];
        fwd_bitmap = ctx.get<ForwardPlan>(in_port).forward_plan[_local_port_id.local_port_id];

        PktBuf pkt_buf = ctx.get<PktBuf>(in_port);

        // if (_global_port_id.global_port_id == 5) {
        //     // printf("_forward: \n");
        //     printBits<uint16_t>(fwd_bitmap);
        //     // PrintQueue(pkt_buf);
        // }

        //if (get_queue_len(_queue) <= 0) return;
        uint16_t num = 0;
        uint16_t count_bit = sizeof(fwd_bitmap)*8;
        while(count_bit--) {
            if ((fwd_bitmap & (1<<num)) != 0) { // the packet should be forward to the e_port
                // // printf("the %d-th packet needs to be forward\n", num);
                Pkt pkt;
                //assert(_dequeue(pkt_buf, pkt) != 0);
                // PrintQueue(pkt_buf);
                fetch_elem(pkt_buf, num, pkt);

                // path tracer
                pkt.path[pkt.path_len] = ctx.get<SwitchID>(in_port).switch_id;
                pkt.path[pkt.path_len+1] = ctx.get<LocalPortID>(in_port).local_port_id;
                pkt.path_len += 2;
                assert(pkt.path_len <= MAX_PATH_LEN);
                
                if (pkt.pkt_type == PktType::PFC_PAUSE || pkt.pkt_type == PktType::PFC_RESUME)  {
                    pkt.dst = 0;
                }

                // // printf("%d", pkt.flow_id); 
                uint8_t queue_idx = pkt.flow_priority;
                _enqueue(_queue.pkt_buf[queue_idx], pkt);
                
                #if PRINT_PKT_LOG
                PrintPkt(pkt, "forward: ");
                #endif
            }
            num += 1;

            // if(_global_port_id.global_port_id==5 && i == 2) {
            //     // printf("count_bit: %d\n", count_bit);
            // }
        }
    }
    //???????
    //_sim_time.sim_time = end_time; // FIXME
}


// on *ingress* port
// remove batch packets from ingress port buffer
inline void remove_pkts(Engine &ctx,
                        LocalPortID &_local_port_id,
                        GlobalPortID &_global_port_id,
                        PktBuf &_queue,
                        ForwardPlan &_forward_plan,
                        SimTime &_sim_time,
                        SimTimePerUpdate &_sim_time_per_update)
{   

    int64_t end_time = _sim_time.sim_time + _sim_time_per_update.sim_time_per_update;
    #if PRINT_SYS_LOG
    if (_global_port_id.global_port_id == 0) {
        // printf("*******Enter into remove_pkts sys*********\n");
        // // printf("end_time: %lld\n", end_time);
    }
    #endif


    uint16_t q_len = get_queue_len(_queue);
    // if (_global_port_id.global_port_id == 6) {
    //     // printf("global_port_id: %d, pkt buffer length before remove: %d\n", _global_port_id.global_port_id, q_len);
    // }
    // if (q_len <= 0) return;
    if (q_len <= 0) {
        _sim_time.sim_time = end_time;
        return;
    }

    // uint16_t idx = 0;
    Pkt pkt;
    while(fetch_elem(_queue, 0, pkt)) {
        // fetch_elem packet from buffer while checking whether buffer is empty
        //// printf("enqueue_time: %ld, end_time: %ld\n", pkt.enqueue_time, end_time);
        if(pkt.enqueue_time >= end_time) {
            //// printf("enqueue_time: %ld, end_time: %ld\n", pkt.enqueue_time, end_time);
            break;
        }
        _dequeue(_queue, pkt);
        //PrintPkt(pkt, "remove pkt in ingress port: ");
    }
    // if (_global_port_id.global_port_id == 6) {
    //     // printf("global_port_id: %d, pkt buffer length after remove: %d\n", _global_port_id.global_port_id, get_queue_len(_queue));
    // }
    //???????
    _sim_time.sim_time = end_time; // FIXME
    
    //if (_global_port_id.global_port_id == 0) PrintQueue(_queue);

    // //reset forward_plan
    // for (uint8_t i = 0; i < INPORT_NUM; i++) {_forward_plan.forward_plan[i] = 0;}
}
//========================================================================================================


inline void transmit(Engine &ctx, SchedTrajType sched_traj_type, 
                     PortType port_type, LocalPortID &_local_port_id, 
                     GlobalPortID &_global_port_id, SwitchID &_switch_id, 
                     NextHop &_next_hop, NextHopType next_hop_type, 
                     PktQueue &_pkt_queue, TXHistory &_tx_history, 
                     SSLinkDelay &_ss_link_delay, LinkRate &_link_rate, 
                     SimTime &_sim_time, SimTimePerUpdate &_sim_time_per_update, Seed &_seed)
{
    int64_t end_time = _sim_time.sim_time + _sim_time_per_update.sim_time_per_update;
    
    #if PRINT_SYS_LOG
    if (_global_port_id.global_port_id == 0) {
        printf("\n*******Enter into transmit sys*********\n");
        printf("start _sim_time.sim_time: %ld, end_time: %ld\n", _sim_time.sim_time, end_time);
    }
    #endif

    // if (_global_port_id.global_port_id != 2 && _global_port_id.global_port_id != 33) {
    //      return;

    uint8_t queue_num = ctx.get<QueueNumPerPort>(ctx.data()._switches[_switch_id.switch_id]).queue_num_per_port;
    // strict priority(SP) to schedule packets 
    //// printf("queue_num: %d\n", queue_num);
    for (uint8_t i = 0; i < queue_num; i++) {
        if (get_queue_len(_pkt_queue.pkt_buf[i]) <= 0) continue;

        if (_pkt_queue.queue_pfc_state[i] == PFCState::PAUSE) {
            // printf("transmit PFC state is PAUSE\n");
            continue;
        }

        // if (_global_port_id.global_port_id == 2) PrintQueue(_pkt_queue.pkt_buf[i]);
        // // printf("before insertionSort: \n");
        insertionSort(_pkt_queue.pkt_buf[i]); // Sorting the packets in the queue using Insertion Sort
        // // printf("after insertionSort: \n");

        // if (_global_port_id.global_port_id == 2) PrintQueue(_pkt_queue.pkt_buf[i]);
        
        // uint16_t pkt_cnt = 0;
        while(_sim_time.sim_time < end_time) {
            // if (_sim_time.sim_time >= end_time) break;
            // fetch_elem packet from buffer, while checking whether buffer is empty
            Pkt pkt;
            // printf("_dequeue _pkt_queue.pkt_buf[%d]: \n", i);

            if(!fetch_elem(_pkt_queue.pkt_buf[i], 0, pkt)) break;
            if(pkt.enqueue_time >= end_time) break;
            _dequeue(_pkt_queue.pkt_buf[i], pkt);

            uint32_t now_queue_len = 0; // record the queue length when a packet comes in real time
            int loc;  // FIXME
            for (loc = get_queue_len(_tx_history); loc > 0; loc--) {
                TXElem tx_elem = {0, 0};
                fetch_elem(_tx_history, loc-1, tx_elem);
                // // printf("tx_elem.dequeue_time: %ld\n", tx_elem.dequeue_time);
                if (pkt.enqueue_time < tx_elem.dequeue_time) {
                    now_queue_len += tx_elem.ip_pkt_len;
                }
                else {
                    break;
                }
            }
            
            // // printf("global port id: %d, now the queue len: %d\n", _global_port_id.global_port_id, now_queue_len);
            // // printf("clear tx_history: \n");

            if (loc > 0) {
                TXElem tmp_tx_elem;
                rm_queue_range(_tx_history, tmp_tx_elem, 0, loc); // FIXME, only handle when using 1 priority queue
            }

            // tail drop
            if ((pkt.payload_len+now_queue_len) > BUF_SIZE) {
                continue;
            }

            //uint16_t pkt_size = pkt.header_len + pkt.payload_len;
            pkt.dequeue_time = (pkt.enqueue_time > _sim_time.sim_time ? pkt.enqueue_time : _sim_time.sim_time) + \
                               (pkt.ip_pkt_len*8*1.0/_link_rate.link_rate)*(1000*1000*1000);
            pkt.enqueue_time = pkt.dequeue_time + _ss_link_delay.SS_link_delay; //FIXME

            TXElem tx_elem = {pkt.dequeue_time, pkt.ip_pkt_len};
            _enqueue(_tx_history, tx_elem); 

            // path tracer
            pkt.path[pkt.path_len] = _switch_id.switch_id;
            pkt.path[pkt.path_len+1] = _local_port_id.local_port_id;
            pkt.path_len += 2;
            
            // ECN marking  
            // pkt.ecn = ECN_MARK::NO;
            // Create a LCG with parameters: a = 48271, c = 0, modulus = 2147483647, seed = 1
            LCG lcg(48271, 0, 2147483647, _seed.seed);
            float mark_prob;
            float rand_num;
            if (now_queue_len <= K_MIN && pkt.ecn == ECN_MARK::NO) {
                pkt.ecn = ECN_MARK::NO;
            }
            else if (now_queue_len > K_MIN && now_queue_len < K_MAX) {
                mark_prob = (now_queue_len-K_MIN)*P_MAX/(K_MAX-K_MIN);
                rand_num = (lcg.next()%1000)/1000.0;
                // // printf("rand num is : %f\n", rand_num);
                if (rand_num < mark_prob) {pkt.ecn = ECN_MARK::YES;}
                _seed.seed = lcg.next();
            }
            else {
                pkt.ecn = ECN_MARK::YES;
            }

            // if (pkt.ecn == ECN_MARK::YES) {
                //// printf("pkt.ecn: %d, rand_num: %f, mark_prob: %f\n", pkt.ecn, rand_num, mark_prob);
            //PrintPkt(pkt, "DATA pkt is marked with ecn: ");
            // }
            // float rand_num = (lcg.next()%1000)/1000.0;
            // // printf("rand num is : %f\n", rand_num);

            //move packet from this egress port to next hop port / the dest host
            Entity next_hop_ett;
            if (next_hop_type == NextHopType::SWITCH) {
                // printf("next_hop: %d\n", _next_hop.next_hop);
                next_hop_ett = ctx.data().inPorts[_next_hop.next_hop];
                _enqueue(ctx.get<PktBuf>(next_hop_ett), pkt);
                #if PRINT_PKT_LOG
                PrintPkt(pkt, "switch transmit to switch: ");
                #endif
            }
            else {
                // printf("next_hop: %d\n", _next_hop.next_hop);
                next_hop_ett = ctx.data()._nics[_next_hop.next_hop];
                _enqueue(ctx.get<BidPktBuf>(next_hop_ett).recv_buf, pkt);
                #if PRINT_PKT_LOG
                PrintPkt(pkt, "switch transmit to NIC: ");
                #endif

                // PrintPath(pkt.path, pkt.path_len);
            }
            // if (_global_port_id.global_port_id == 4) continue;
            //
            // if (pkt.pkt_type == PktType::ACK || pkt.pkt_type == PktType::NACK) {
            //     // printf("pkt_type: %d, switch_id: %d, global_port_id: %d, NextHopType: %d, next hop link: %d, src: %d --> dst: %d, payload_len: %d, sq_num: %lld, dequeue_time: %lld, enqueue_time: %lld\n", \
            //             pkt.pkt_type, _switch_id.switch_id, _global_port_id.global_port_id, next_hop_type, _next_hop.next_hop, pkt.src, pkt.dst, pkt.payload_len, pkt.sq_num, pkt.dequeue_time, pkt.enqueue_time);
            // }

            
            // if (pkt_cnt++ < 7) {
            //     _enqueue(ctx.get<PktBuf>(next_hop_ett), pkt);
            //     // pkt_cnt = 0;
            // }
            // _seed.seed = lcg.next()%1000;
            //// printf("enqueue normal pkt: \n");

            _sim_time.sim_time = pkt.dequeue_time;

            // check whether the queue length excceeds the buffer limit 
            if (now_queue_len >= BUF_SIZE*(PFC_THR/100.0)) {
                // uint32_t PFC_FOR_UPSTREAM = 1;
                // int64_t pfc_enqueue_time = pkt.enqueue_time;
                // uint8_t queue_idx = i; // pkt.flow_priority; // the idx of priority queue is placed in the filed(4-th element) of pkt
                // uint8_t flow_priority = 0; // and the pfc frame is set with highest flow priority

                // uint32_t path[MAX_PATH_LEN];
                // memset(path, 0, MAX_PATH_LEN*sizeof(uint32_t));
                // Pkt pfc_frame;
                // // Pkt pfc_frame = {0, PFC_FOR_UPSTREAM, pkt.l4_port, 40, 0, 0, queue_idx, pfc_enqueue_time, 0, flow_priority, PktType::PFC_PAUSE}; // and PktType is PFC PAUSE
                // // printf("create_pkt: \n");

                // create_pkt(pfc_frame, 0, PFC_FOR_UPSTREAM, pkt.l4_port, 40, 0, 40, 0, queue_idx, pfc_enqueue_time, 0, flow_priority, PktType::PFC_PAUSE, path, 0, ECN_MARK::NO);
                // // printf("after create_pkt: \n");
                // Entity in_port_same = ctx.data().inPorts[_global_port_id.global_port_id]; // in_port in the same port as the egress port
                // // printf("enqueue pfc_frame: \n");
                // _enqueue(ctx.get<PktBuf>(in_port_same), pfc_frame);
                // // printf("after enqueue pfc_frame: \n"); 
            }
        }
    }
    //???????
    _sim_time.sim_time = end_time; // FIXME
}


inline void nic_receive(Engine &ctx, NIC_ID &_nic_id,
                        BidPktBuf &_bid_pkt_buf, NICRate &_nic_rate,
                        SimTime &_sim_time, SimTimePerUpdate &_sim_time_per_update) {
    
    int64_t end_time = _sim_time.sim_time + _sim_time_per_update.sim_time_per_update;
    
    #if PRINT_SYS_LOG
    if (_nic_id.nic_id == 0) {
        printf("\n*******Enter into nic_receive*********\n");
        printf("start _sim_time.sim_time: %ld, end_time: %ld\n", _sim_time.sim_time, end_time);
    }
    #endif
    // Entity recv_flow = ctx.data().recv_flows[0][0];

    Pkt pkt;
    // PktBuf recv_buf;
    while (_sim_time.sim_time < end_time) {
        if(!fetch_elem(_bid_pkt_buf.recv_buf, 0, pkt)) break;
        if(pkt.enqueue_time >= end_time) break;
        _dequeue(_bid_pkt_buf.recv_buf, pkt);
        // PrintPkt(pkt, "nic receive buffer");
        // recv flow entity
        Entity recv_flow = Entity::none();
        if (pkt.pkt_type == PktType::DATA ) {
            printf("data packet: pkt.src: %d, pkt.dst: %d, pkt.flow_id: %d\n", pkt.src, pkt.dst, pkt.flow_id);
            recv_flow = ctx.data().recv_flows[pkt.dst][pkt.flow_id];
        }
        else { //ACK/NACK/CNP
            printf("non-data(ACK/NACK/CNP) packet: pkt.flow_id: %d\n", pkt.flow_id);
            recv_flow = ctx.data().snd_recv_flows[pkt.dst][pkt.flow_id]; // dst and src switch in //ACK/NACK/CNP pkt
        }

        if (recv_flow == Entity::none()) {
            printf("Error: no flow entity exist for the pkt(src: %d, dst: %d, flow_id: %lu\n", pkt.src, pkt.dst, pkt.flow_id);
            return;
        }
        // printf(" _enqueue(ctx.get<PktBuf>(recv_flow), pkt)\n");
        // recv_buf = ctx.get<PktBuf>(recv_flow);
        _enqueue(ctx.get<PktBuf>(recv_flow), pkt);
        // printf("pkt.pkt_type: %hhu, recv recv_queue length: %d\n", pkt.pkt_type, get_queue_len(ctx.get<PktBuf>(recv_flow)));
 
        // printf("5\n");
        // PrintPath(pkt.path, pkt.path_len);
        if(pkt.enqueue_time >= end_time) break;
    }
    //_sim_time.sim_time = end_time;
}

// receive system: 
// 1) generate ACK/NACK/CNP; 
// 2) or move them into the ack buffer of receiver's sender 
inline void flow_receive(Engine &ctx, FlowID &_flow_id, PktBuf &_recv_queue,
                         NICRate &_nic_rate, HSLinkDelay &_HS_link_delay, 
                         SimTime &_sim_time, SimTimePerUpdate &_sim_time_per_update,
                         RecvBytes &_recv_bytes, L4Port &_l4_port,
                         LastCNPTimestamp &_last_cnp_timestamp) {

    int64_t end_time = _sim_time.sim_time + _sim_time_per_update.sim_time_per_update;
    // if (_flow_id.flow_id == 0) {
    #if PRINT_SYS_LOG
    printf("\n*******Enter into flow receive*********\n");
    printf("start _sim_time.sim_time: %ld, end_time: %ld\n", _sim_time.sim_time, end_time);
    #endif
    // }

    Pkt pkt;
    uint8_t flow_priority = 0;
    uint16_t header_len = 40;
    while (_sim_time.sim_time < end_time) {
        // break;
        // printf("_recv_queue length: %d\n", get_queue_len(_recv_queue));
        if(!fetch_elem(_recv_queue, 0, pkt)) break;

        if(pkt.enqueue_time >= end_time) break;
        _dequeue(_recv_queue, pkt);

        // PrintPath(pkt.path, pkt.path_len);
        if(pkt.enqueue_time >= end_time) break;
        
        // Entity snd_flow_entt = ctx.data()._snd_flows[pkt.flow_id];
        

        if (pkt.pkt_type != PktType::DATA) { //ACK/NACK is received by sender's receiver
            Entity snd_flow_entt = ctx.data().snd_flows[pkt.dst][pkt.flow_id];
            if (snd_flow_entt == Entity::none()) {return;}

            _enqueue(ctx.get<AckPktBuf>(snd_flow_entt), pkt); //enqueue into the ack_pkt_buf

            // printf("flow_id: %d\n", _flow_id.flow_id);
            #if PRINT_PKT_LOG
            PrintPkt(pkt, "ACK/NACK received by sender's receiver: ");
            #endif
        }
        else { //DATA pkt is received by receiver
            uint16_t pkt_size = pkt.header_len + pkt.payload_len;
            _sim_time.sim_time += (pkt_size*8*1.0/_nic_rate.nic_rate)*(1000*1000*1000);

            // printf("flow_id: %d\n", _flow_id.flow_id);
            // printf("src: %d recv: %d, expected_recv_bytes: %ld\n", recv_flow_queue.recv_flow[0].src, recv_flow_queue.recv_flow[0].dst, _recv_bytes.recv_bytes); 
            #if PRINT_PKT_LOG
            PrintPkt(pkt, "DATA received by receiver: ");
            #endif
            uint32_t path[MAX_PATH_LEN];
            memset(path, 0, MAX_PATH_LEN*sizeof(uint32_t));

            // int64_t expect_byte = + payload_len;
            uint32_t ack_src = pkt.dst; //recv_flow_queue.recv_flow[0].dst;
            uint32_t ack_dst = pkt.src; //recv_flow_queue.recv_flow[0].src;

            int64_t dequeue_time = _sim_time.sim_time + 1;
            int64_t enqueue_time = _sim_time.sim_time + 1 + _HS_link_delay.HS_link_delay;
            

            int64_t sq_num;
            Pkt ack_pkt;
            if (pkt.sq_num == _recv_bytes.recv_bytes) { // packet order is right: generate ACK
                _recv_bytes.recv_bytes += pkt.payload_len;
                sq_num = _recv_bytes.recv_bytes;
                PktType pkt_type = PktType::ACK;

                create_pkt(ack_pkt, ack_src, ack_dst, \
                        _l4_port.l4_port, header_len, 0, header_len, sq_num, \
                        pkt.flow_id, enqueue_time, dequeue_time, flow_priority, pkt_type, path, 0, ECN_MARK::NO);
            }
            else { // packets is out of order, NACK
                sq_num = _recv_bytes.recv_bytes;
                PktType pkt_type = PktType::NACK;

                create_pkt(ack_pkt, ack_src, ack_dst, \
                           _l4_port.l4_port, header_len, 0, header_len, sq_num, \
                           pkt.flow_id, enqueue_time, dequeue_time, flow_priority, pkt_type, path, 0, ECN_MARK::NO);     
            }
            
            Entity recv_snd_flow_entt = ctx.data().recv_snd_flows[ack_src][pkt.flow_id];
            if (recv_snd_flow_entt == Entity::none()) {return;}

            _enqueue(ctx.get<PktBuf>(recv_snd_flow_entt), ack_pkt);
            #if PRINT_PKT_LOG
            PrintPkt(ack_pkt, "ACK/NACK generated by receiver: ");
            #endif
            // printf("pkt.ecn: %d\n", pkt.ecn);
            // packet with ecn marking, generate a cnp packet
            if (pkt.ecn == ECN_MARK::YES && (_sim_time.sim_time - _last_cnp_timestamp.last_cnp_timestamp) >= CNP_DURATION) { 
            // if (pkt.ecn == ECN_MARK::YES) { 
                Pkt cnp_pkt;
                PktType pkt_type = PktType::CNP;
                create_pkt(cnp_pkt, ack_src, ack_dst, \
                           _l4_port.l4_port, header_len, 0, header_len, sq_num, \
                           pkt.flow_id, enqueue_time, dequeue_time, flow_priority, pkt_type, path, 0, ECN_MARK::YES);

                _enqueue(ctx.get<PktBuf>(recv_snd_flow_entt), cnp_pkt);
                #if PRINT_PKT_LOG
                PrintPkt(cnp_pkt, "CNP generated by receiver: ");
                #endif
                _last_cnp_timestamp.last_cnp_timestamp = _sim_time.sim_time;
            }
        }
    }
    _sim_time.sim_time = end_time;
}












    // -----------------------------------------------------------------------------------

    // 将整数数组解析为 ChakraNodes 数组
    int parseChakraNodes(const ChakraNodesData &chakraNodesData, int row, ChakraNode parsedNodes[], int intArrayLength = chakra_nodes_data_length, int maxNodes = MAX_CHAKRA_NODES)
    {

        int validNodes = 0; // 有效节点计数

        for (int i = 0; i < maxNodes; ++i)
        {
            int baseIndex = i * INTS_PER_NODE;
            if (baseIndex + INTS_PER_NODE > intArrayLength)
                break; // 确保不越界

            // 解析单个节点
            for (int j = 0; j < 20; ++j)
            {
                parsedNodes[validNodes].name[j] = chakraNodesData.data[row][baseIndex + j];
            }
            baseIndex += 20;

            parsedNodes[validNodes].type = (NodeType)chakraNodesData.data[row][baseIndex];
            baseIndex += 1;

            parsedNodes[validNodes].id = chakraNodesData.data[row][baseIndex];

            // 初始节点没有id
            if (parsedNodes[validNodes].id == -1)
            {
                parsedNodes[validNodes].id = 0;
            }

            baseIndex += 1;

            for (int j = 0; j < 10; ++j)
            {
                parsedNodes[validNodes].data_deps[j] = chakraNodesData.data[row][baseIndex + j];
            }
            baseIndex += 10;

            parsedNodes[validNodes].comm_para = ((uint64_t)chakraNodesData.data[row][baseIndex + 1] << 32) | chakraNodesData.data[row][baseIndex];
            baseIndex += 2;

            parsedNodes[validNodes].comm_size = ((uint64_t)chakraNodesData.data[row][baseIndex + 1] << 32) | chakraNodesData.data[row][baseIndex];
            baseIndex += 2;

            parsedNodes[validNodes].comm_src = ((uint64_t)chakraNodesData.data[row][baseIndex + 1] << 32) | chakraNodesData.data[row][baseIndex];
            baseIndex += 2;

            parsedNodes[validNodes].comm_dst = ((uint64_t)chakraNodesData.data[row][baseIndex + 1] << 32) | chakraNodesData.data[row][baseIndex];
            baseIndex += 2;

            parsedNodes[validNodes].involved_dim_1 = chakraNodesData.data[row][baseIndex];
            parsedNodes[validNodes].involved_dim_2 = chakraNodesData.data[row][baseIndex + 1];
            parsedNodes[validNodes].involved_dim_3 = chakraNodesData.data[row][baseIndex + 2];
            baseIndex += 3;

            parsedNodes[validNodes].durationMicros = chakraNodesData.data[row][baseIndex];
            baseIndex += 1;

            // 检查是否全为 0（表示补全区）
            bool isPadding = true;
            for (int j = i * INTS_PER_NODE; j < i * INTS_PER_NODE + INTS_PER_NODE; ++j)
            {
                if (chakraNodesData.data[row][j] != 0)
                {
                    isPadding = false;
                    break;
                }
            }
            if (isPadding)
            {
                break; // 忽略后续的补全区
            }

            // 完成一个节点解析后递增 validNodes
            validNodes++;
        }
        return validNodes; // 返回有效节点的数量
    }

    int filterNoDependencyNodes(const ChakraNodes &chakraNodes, ChakraNode filteredNodes[], int maxNodes = CURRENT_EXEC_NODES_MAX)
    {

        int count = 0; // 无依赖节点计数

        // 遍历所有节点
        for (size_t i = 0; i < MAX_CHAKRA_NODES; ++i)
        {
            const ChakraNode &node = chakraNodes.nodes[i];

            // 跳过无效节点
            if (node.type == NodeType::None)
            {
                continue;
            }

            // 检查是否没有依赖
            bool hasDependency = false;
            for (int j = 0; j < 10; ++j)
            {
                if (node.data_deps[j] != INVALID_DEPENDENCY)
                {
                    hasDependency = true;
                    break;
                }
            }

            // 如果没有依赖，添加到结果数组
            if (!hasDependency)
            {
                if (count < maxNodes)
                {
                    filteredNodes[count++] = node;
                }
                else
                {
                    // 超过最大限制，停止添加
                    break;
                }
            }
        }

        return count; // 返回无依赖节点的数量
    }

    void removeNode(ChakraNodes &chakraNodes, uint32_t nodeId)
    {

        // 遍历所有节点，找到目标节点并将其标记为无效
        for (size_t i = 0; i < MAX_CHAKRA_NODES; ++i)
        {
            ChakraNode &node = chakraNodes.nodes[i];

            // 如果找到目标节点，标记为无效
            if (node.id == nodeId)
            {
                node.type = NodeType::None; // 标记为无效节点
                for (int j = 0; j < 10; ++j)
                {
                    node.data_deps[j] = INVALID_DEPENDENCY; // 清空依赖
                }
                node.id = INVALID_DEPENDENCY; // 将节点 ID 设置为无效
                break;
            }
        }

        // 遍历所有节点，移除对目标节点的依赖
        for (size_t i = 0; i < MAX_CHAKRA_NODES; ++i)
        {
            ChakraNode &node = chakraNodes.nodes[i];

            // 跳过无效节点
            if (node.type == NodeType::None)
            {
                continue;
            }

            // 检查并移除对目标节点的依赖
            for (int j = 0; j < 10; ++j)
            {
                if (node.data_deps[j] == nodeId)
                {
                    node.data_deps[j] = INVALID_DEPENDENCY; // 设置为无效依赖
                }
            }
        }
    }

    // network interface
    // 获取当前仿真时间
    // func
    inline uint64_t getCurrentTime(Engine &ctx)
    {
        // return ctx.get<SimTime>(ctx.data().timer_entity).time;
        Entity net_npu = ctx.data()._net_npus[0]; 
        int64_t sim_time = ctx.get<SimTime>(net_npu).sim_time;
        return sim_time;
    }

    // network interface
    // 指示当前是否存在流发送
    // func
    inline bool isExistedFlow(Engine &ctx)
    {
        return false;
    }

    // network interface
    // 增加仿真时间：使所有组件增加仿真时间
    // func
    inline void addSimtime(Engine &ctx, uint64_t t_ns)
    {
        // ctx.get<SimTime>(ctx.data().timer_entity).time += t_ns;
    }

    // network interface
    // 设置流：
    // func
    // inline void setFlow(Engine &ctx, uint64_t comm_src, uint64_t comm_dst, uint64_t comm_size, uint32_t flow_id)
    // {
    //     printf("set flow id %d: %d->%d %d, \n", flow_id, comm_src, comm_dst, comm_size);
    // }

    // network interface
    // 检测/获取流完成事件：返回流完成数量，同时把流完成事件赋值flows_finish
    // func
    // inline uint32_t checkFlowFinish(Engine &ctx, uint32_t npu_id, SysFlow flows_finish[])
    // {
    //     return 0;
    // }

    // inline void procssTime(Engine &ctx,
    //                        SimTime &simTime)
    // {
    //     // simTime.time += (1000 * 1000000);
    //     simTime.time += (1000);
    //     printf("current time : %ld\n", simTime.time);
    // }

    uint64_t msToNs(uint64_t milliseconds)
    {
        return milliseconds * 1000000; // 1 ms = 1,000,000 ns
    }

    inline void skipTime_add_time(Engine &ctx, uint64_t t_relative_ns)
    {
        for (size_t i = 0; i < MAX_CHAKRA_NODES; i++)
        {
            if (ctx.get<NextProcessTimes>(ctx.data().next_process_time_entity).times_abs[i] != 0)
            {
                ctx.get<NextProcessTimes>(ctx.data().next_process_time_entity).times_abs[i] = getCurrentTime(ctx) + t_relative_ns;
                break;
            }
        }
    }

    // 自定义排序函数：从大到小冒泡排序
    void customSortDescending(uint64_t *array, size_t length)
    {
        for (size_t i = 0; i < length - 1; ++i)
        {
            for (size_t j = 0; j < length - 1 - i; ++j)
            {
                if (array[j] < array[j + 1])
                { // 从大到小比较
                    // 交换两个元素
                    uint64_t temp = array[j];
                    array[j] = array[j + 1];
                    array[j + 1] = temp;
                }
            }
        }
    }

    inline uint64_t skipTime_sort_time_rMin(Engine &ctx)
    {

        // 找到第一个等于 0 的元素的位置
        size_t zeroIndex = MAX_CHAKRA_NODES; // 默认没有找到 0

        for (size_t i = 0; i < MAX_CHAKRA_NODES; ++i)
        {

            if (ctx.get<NextProcessTimes>(ctx.data().next_process_time_entity).times_abs[i] == 0)
            {

                zeroIndex = i;
                break;
            }
        }

        // 对第一个 0 之前的元素进行从大到小排序
        if (zeroIndex > 0 && zeroIndex < MAX_CHAKRA_NODES)
        {
            uint64_t *array = ctx.get<NextProcessTimes>(ctx.data().next_process_time_entity).times_abs;
            customSortDescending(array, zeroIndex);
        }

        return ctx.get<NextProcessTimes>(ctx.data().next_process_time_entity).times_abs[zeroIndex];
    }

    inline void skipTime_remove_time(Engine &ctx, uint64_t x_ns)
    {
        // 找到第一个等于 0 的元素的位置
        size_t zeroIndex = MAX_CHAKRA_NODES; // 默认没有找到 0
        for (size_t i = 0; i < MAX_CHAKRA_NODES; ++i)
        {
            if (ctx.get<NextProcessTimes>(ctx.data().next_process_time_entity).times_abs[i] != 0)
            {
                ctx.get<NextProcessTimes>(ctx.data().next_process_time_entity).times_abs[i] -= x_ns;
            }
            else
            {
                break;
            }
        }
    }

    inline void init(Engine &ctx,
                     ChakraNodesData &chakra_nodes_data)
    {
        printf("init npus.\n");
        // 获取行数和列数
        size_t rows = sizeof(chakra_nodes_data.data) / sizeof(chakra_nodes_data.data[0]);       // 总大小 / 单行大小
        size_t cols = sizeof(chakra_nodes_data.data[0]) / sizeof(chakra_nodes_data.data[0][0]); // 单行大小 / 单个元素大小
        size_t npu_nums = rows;
        size_t npu_data_num = cols;
        printf("sizeof(chakra_nodes_data.data):%d\n", sizeof(chakra_nodes_data.data));
        printf("sizeof(chakra_nodes_data.data[0]:%d\n", sizeof(chakra_nodes_data.data[0]));
        printf("npu_nums:%d\n", npu_nums);
        printf("npu_data_num:%d\n", npu_data_num);

        for (int32_t i = 0; i < npu_nums; i++)
        {
            Entity npuNode = ctx.makeEntity<NpuNode>();
            ctx.get<NpuID>(npuNode).value = i;
            int nodeCount = parseChakraNodes(chakra_nodes_data, i, ctx.get<ChakraNodes>(npuNode).nodes);
            if(nodeCount==0)
            {
                ctx.destroyEntity(npuNode);
                return;
            }
            ctx.get<HardwareResource>(npuNode) = HardwareResource();
            ctx.get<ProcessingCompTask>(npuNode) = ProcessingCompTask();
            ctx.get<ProcessingCommTasks>(npuNode) = ProcessingCommTasks();
            ctx.get<ProcessingCommTasks>(npuNode).flow_id = i *FLOW_ID_MAX_LENGTH;
            // ctx.get<Entity>(npuNode) = npuNode;

            printf("npu %d: turn %d nodes.\n", i, nodeCount);
            ctx.data().chakra_nodes_entities[i] = npuNode;
        }

        ctx.destroyEntity(ctx.data().init_entity);
        printf("init npus over.\n");
    }

    inline void processNpuNodes(Engine &ctx,
                                NpuID &id,
                                ChakraNodes &chakraNodes,
                                HardwareResource &hardwareResource,
                                ProcessingCompTask &processingCompTask,
                                ProcessingCommTasks &processingCommTasks)
    {
        if (SYS_LOG && id.value == 0)
        {
            printf("### sys1 ### : exec processNpuNodes.\n");
        }

        // append nodes
        if (hardwareResource.one_task_finish)
        {
            ChakraNode current_exec_nodes[CURRENT_EXEC_NODES_MAX];
            int count = filterNoDependencyNodes(chakraNodes, current_exec_nodes);
            printf("total no dp node:%d\n", count);
            // process no np nodes
            for (size_t i = 0; i < count; i++)
            {

                ChakraNode node = current_exec_nodes[i];
                // 会重复查找，已经在处理时，跳过
                if (processingCommTasks.containsNodeId(node.id))
                {
                    continue;
                }
                if (hardwareResource.comp_ocupy && node.id == processingCompTask.node_id)
                {
                    continue;
                }
                if (SYS_LOG && id.value == 0)
                {
                    printf("----process node %d:----\n", current_exec_nodes[i].id);
                }
                switch (node.type)
                {
                case NodeType::COMP_NODE:
                {
                    if (!hardwareResource.comp_ocupy)
                    {
                        processingCompTask.time_finish_ns = getCurrentTime(ctx) + msToNs(node.durationMicros);
                        if (SYS_LOG && id.value == 0)
                        {
                            printf("processingCompTask.time_finish_ns: %ld\n", processingCompTask.time_finish_ns);
                        }
                        processingCompTask.state = TaskState::START;
                        processingCompTask.node_id = node.id;

                        // set flag
                        hardwareResource.comp_ocupy = true;

                        // set skip time - add time.
                        skipTime_add_time(ctx, processingCompTask.time_finish_ns);
                    }
                    else
                    {
                        if (SYS_LOG && id.value == 0)
                        {
                            printf("no comp hardware!");
                        }
                    }
                    break;
                }
                case NodeType::COMM_SEND_NODE:
                {

                    ProcessingCommTask processingCommTask = ProcessingCommTask();
                    if (SYS_LOG && id.value == 0)
                    {
                        printf("processingCommTask send %d -> %d .\n", node.comm_src, node.comm_dst);
                    }
                    processingCommTask.state = TaskState::START;
                    processingCommTask.node_id = node.id;
                    processingCommTask.flow_count = 1;
                    processingCommTasks.addTask(processingCommTask);

                    // create comm entity
                    Entity process_e = ctx.makeEntity<ProcessComm_E>();
                    ctx.get<NpuID>(process_e).value = id.value;
                    ctx.get<NodeID>(process_e).value = node.id;
                    uint32_t flow_id = processingCommTasks.getTotalFlowCount();
                    ctx.get<TaskFlows>(process_e).flows[0] = SysFlow();
                    ctx.get<TaskFlows>(process_e).flows[0].id = flow_id;
                    ctx.get<TaskFlows>(process_e).flows[0].comm_size = node.comm_size;
                    ctx.get<TaskFlows>(process_e).flows[0].comm_src = node.comm_src;
                    ctx.get<TaskFlows>(process_e).flows[0].comm_dst = node.comm_dst;
                    ctx.get<TaskFlows>(process_e).flows[0].state = TaskState::START;
                    ctx.get<TaskFlows>(process_e).flows[0].is_send = true;

                    ctx.data().node_flows_exec_entity[id.value][node.id]= process_e;
                    uint64_t src=node.comm_src;
                    uint64_t dst=node.comm_dst;
                    if(MAX_MODE_16_LIMIT)
                    {
                        if(src>15)
                        {
                            src=0;
                        }
                        if(dst>15)
                        {
                            dst=15;
                        }
                    }

                    setFlow(ctx, src, dst, node.comm_size, flow_id);

                    break;
                }
                case NodeType::COMM_RECV_NODE:
                {
                    ProcessingCommTask processingCommTask = ProcessingCommTask();
                    if (SYS_LOG && id.value == 0)
                    {
                        printf("processingCommTask recv %d -> %d .\n", node.comm_src, node.comm_dst);
                    }
                    processingCommTask.state = TaskState::START;
                    processingCommTask.node_id = node.id;
                    processingCommTask.flow_count = 1;
                    processingCommTasks.addTask(processingCommTask);

                    // create comm entity
                    Entity process_e = ctx.makeEntity<ProcessComm_E>();
                    ctx.get<NpuID>(process_e).value = id.value;
                    ctx.get<NodeID>(process_e).value = node.id;
                    uint32_t flow_id = processingCommTasks.getTotalFlowCount();
                    ctx.get<TaskFlows>(process_e).flows[0] = SysFlow();
                    ctx.get<TaskFlows>(process_e).flows[0].id = flow_id;
                    ctx.get<TaskFlows>(process_e).flows[0].comm_size = node.comm_size;
                    ctx.get<TaskFlows>(process_e).flows[0].comm_src = node.comm_src;
                    ctx.get<TaskFlows>(process_e).flows[0].comm_dst = node.comm_dst;
                    ctx.get<TaskFlows>(process_e).flows[0].state = TaskState::START;
                    ctx.get<TaskFlows>(process_e).flows[0].is_send = false;

                    ctx.data().node_flows_exec_entity[id.value][node.id]= process_e;
                    // setFlow(Engine &ctx, uint64_t comm_src, uint64_t comm_dst, uint64_t comm_size, uint32_t flow_id)
                    uint64_t src=node.comm_src;
                    uint64_t dst=node.comm_dst;
                    if(MAX_MODE_16_LIMIT)
                    {
                        if(src>15)
                        {
                            src=0;
                        }
                        if(dst>15)
                        {
                            dst=15;
                        }
                    }

                    setFlow(ctx, src, dst, node.comm_size, flow_id);

                    break;
                }
                case NodeType::COMM_COLL_NODE:
                {
                    ProcessingCommTask processingCommTask = ProcessingCommTask();
                    if (SYS_LOG && id.value == 0)
                    {
                        printf("processingCommTask: coll .\n");
                    }
                    processingCommTask.state = TaskState::START;
                    processingCommTask.node_id = node.id;
                    processingCommTask.flow_count = 1;
                    processingCommTasks.addTask(processingCommTask);

                    // create comm entity
                    Entity process_e = ctx.makeEntity<ProcessComm_E>();
                    ctx.get<NpuID>(process_e).value = id.value;
                    ctx.get<NodeID>(process_e).value = node.id;
                    uint32_t flow_id = processingCommTasks.getTotalFlowCount();
                    ctx.get<TaskFlows>(process_e).flows[0] = SysFlow();
                    ctx.get<TaskFlows>(process_e).flows[0].id = flow_id;
                    ctx.get<TaskFlows>(process_e).flows[0].comm_size = 1000;
                    ctx.get<TaskFlows>(process_e).flows[0].comm_src = 0;
                    ctx.get<TaskFlows>(process_e).flows[0].comm_dst = 1;
                    ctx.get<TaskFlows>(process_e).flows[0].state = TaskState::START;
                    ctx.get<TaskFlows>(process_e).flows[0].is_send = true;


                    ctx.data().node_flows_exec_entity[id.value][node.id]= process_e;


                    // setFlow(Engine &ctx, uint64_t comm_src, uint64_t comm_dst, uint64_t comm_size, uint32_t flow_id)
                    setFlow(ctx, 0, 1, 1000, flow_id);
                    break;
                }
                default:
                    break;
                }
            }

            // set flag
            hardwareResource.one_task_finish = false;
        }
    }

    inline void processCommCheckFlow(Engine &ctx, NpuID &npu_id, NodeID &node_id, TaskFlows &taskFlows)
    {
        if (SYS_LOG && npu_id.value == 0)
        {
            printf("### sys2 ### : exec processCommCheckFlow.\n");
        }

        SysFlow flows_finish[MAX_FLOW_NUM_PER_COMM_NODE];
        uint32_t flow_finish_count = checkFlowFinish(ctx, npu_id.value, flows_finish);
        if (ENABLE_TEST)
        {
            for (int i = 0; i < 20; ++i)
            {
                if (taskFlows.flows[i].state == TaskState::START)
                {
                    taskFlows.flows[i].state = TaskState::FINISH;
                    printf("test : flow id %d -> set flow finish.\n", taskFlows.flows[i].id);
                }
            }
        }

        if (flow_finish_count > 0)
        {
            taskFlows.updateFlows(flows_finish, flow_finish_count);
        }

        if (taskFlows.areAllTasksDone())
        {
            printf("node id %d -> taskFlows.areAllTasksDone\n", node_id);

            ctx.get<ProcessingCommTasks>(ctx.data().chakra_nodes_entities[npu_id.value]).setFinish(node_id.value, getCurrentTime(ctx));
        
            ctx.destroyEntity(ctx.data().node_flows_exec_entity[npu_id.value][node_id.value]);
        }
    }

    // network logic

    uint16_t frame_skiptime = 0;
    inline void checkSkipTime(Engine &ctx, NextProcessTimes &t)
    {
        if (SYS_LOG)
        {
            printf("### sys4 ### : exec checkSkipTime.\n");
        }

        frame_skiptime++;
        if (frame_skiptime / CHECK_SKIPTIME_INTERVAL_PER_FRAME == 0)
        {
            frame_skiptime = 0;
            if (!isExistedFlow(ctx))
            {
                uint64_t min_time = skipTime_sort_time_rMin(ctx);
                if (min_time != 0)
                {
                    addSimtime(ctx, min_time - getCurrentTime(ctx));
                    if (SYS_LOG)
                    {
                        printf("skip to time:%d\n", getCurrentTime(ctx));
                    }
                    skipTime_remove_time(ctx, min_time);
                }
            }
        }
    }

    inline void removeNpuNodes(Engine &ctx,
                               NpuID &id,
                               ChakraNodes &chakraNodes,
                               HardwareResource &hardwareResource,
                               ProcessingCompTask &processingCompTask,
                               ProcessingCommTasks &processingCommTasks)
    {
        if (SYS_LOG && id.value == 0)
        {
            printf("### sys3 ### : exec removeNpuNodes.\n");
        }

        if (SYS_LOG && id.value == 0)
        {
            printf("----check comp task:----\n");
        }
        // process comp
        if (processingCompTask.state == TaskState::START && (processingCompTask.time_finish_ns >= getCurrentTime(ctx)))
        {
            // if (SYS_LOG && id.value == 0)
            // {
            //     printf("processingCompTask %d over.\n", processingCompTask.node_id);
            // }
            // release node.
            removeNode(chakraNodes, processingCompTask.node_id);
            if (SYS_LOG && id.value == 0)
            {
                printf("remode comp node : %d\n", processingCompTask.node_id);
            }
            // reset flag.
            hardwareResource.one_task_finish = true;
            processingCompTask.state = TaskState::INIT;
            hardwareResource.comp_ocupy = false;
        }
        if (SYS_LOG && id.value == 0)
        {
            printf("----check comm tasks:----\n");
        }
        // process comm
        if (processingCommTasks.has_task())
        {
            // if (SYS_LOG && id.value == 0)
            // {
            //     printf("node id %d -> processingCommTasks.has_task() .\n", id.value);
            // }

            int64_t t = getCurrentTime(ctx);
            ProcessingCommTask result[MAX_FLOW_PER_NPU];
            int task_count = processingCommTasks.dequeueTasksByTime(t, result, MAX_FLOW_PER_NPU);

            if (SYS_LOG && id.value == 0 && task_count > 0)
            {
                printf("npu id %d -> processingCommTasks over count :%d .\n", id.value, task_count);
            }

            if (task_count > 0)
            {
                for (size_t i = 0; i < task_count; i++)
                {
                    // if (SYS_LOG && id.value == 0)
                    // {
                    //     printf("processingCompTask over.\n");
                    // }
                    // release node.
                    removeNode(chakraNodes, processingCommTasks.tasks[i].node_id);
                    if (SYS_LOG && id.value == 0)
                    {
                        printf("release comm node : %d\n", processingCommTasks.tasks[i].node_id);
                    }
                }
            }

            // reset flag.
            hardwareResource.one_task_finish = true;
        }
    }

    













// Build the task graph
void Sim::setupTasks(TaskGraphManager &taskgraph_mgr, const Config &cfg)
{
    TaskGraphBuilder &builder = taskgraph_mgr.init(TaskGraphID::Step);


    // ------------------sys--------------------------------------------------------------

    // auto sys_init = builder.addToGraph<ParallelForNode<Engine, init,
    //                                                     ChakraNodesData>>({});
    // auto sys_process_node = builder.addToGraph<ParallelForNode<Engine, processNpuNodes,
    //                                                             NpuID, ChakraNodes, HardwareResource, ProcessingCompTask, ProcessingCommTasks>>({sys_init});

    // --------------------------------------------------------------------------------

    auto get_flow_sys = builder.addToGraph<ParallelForNode<Engine, comm_set_flow, NET_NPU_ID, NewFlowQueue, \
                                             SimTime, SimTimePerUpdate>>({}); 

    auto setup_flow_sys = builder.addToGraph<ParallelForNode<Engine, setup_flow, NET_NPU_ID, NewFlowQueue, \
    SimTime, SimTimePerUpdate>>({get_flow_sys}); 

    auto nic_receive_sys = builder.addToGraph<ParallelForNode<Engine, nic_receive, NIC_ID, \
                                              BidPktBuf, NICRate, SimTime, SimTimePerUpdate>>({setup_flow_sys});

    auto flow_receive_sys = builder.addToGraph<ParallelForNode<Engine, flow_receive, FlowID, PktBuf, \
                                           NICRate, HSLinkDelay, SimTime, SimTimePerUpdate, \
                                           RecvBytes, L4Port, LastCNPTimestamp>>({nic_receive_sys});

    auto flow_send_sys = builder.addToGraph<ParallelForNode<Engine, flow_send, FlowID, Src, Dst, // dst net_npu
                                                            L4Port, // layer 4 port
                                                            FlowSize, StartTime, StopTime,
                                                            NIC_ID, SndServerID, RecvServerID, // host pair id
                                                            SndNxt, SndUna, FlowState,
                                                            LastAckTimestamp, NxtPktEvent, PFCState, CC_Para,
                                                            PktBuf, AckPktBuf,
                                                            NICRate, HSLinkDelay, SimTime, SimTimePerUpdate>>({flow_receive_sys});

    auto check_flow_state_sys = builder.addToGraph<ParallelForNode<Engine, check_flow_state, NET_NPU_ID, CompletedFlowQueue, \
                                                   SimTime, SimTimePerUpdate>>({flow_send_sys}); 

    auto nic_forward_sys = builder.addToGraph<ParallelForNode<Engine, nic_forward, \
                                              NIC_ID, NICRate, SimTime, \
                                              SimTimePerUpdate, BidPktBuf>>({check_flow_state_sys});


    auto nic_transmit_sys = builder.addToGraph<ParallelForNode<Engine, nic_transmit, NIC_ID, \
                                               NICRate, HSLinkDelay, \
                                               SimTime, SimTimePerUpdate, \
                                               BidPktBuf, TXHistory, \
                                               NextHop, Seed>>({nic_forward_sys});

    auto set_forward_plan_sys = builder.addToGraph<ParallelForNode<Engine, set_forward_plan, \
                                                   LocalPortID, GlobalPortID, \
                                                   SwitchID, PktBuf, ForwardPlan, \
                                                   SimTime, SimTimePerUpdate>>({nic_transmit_sys});

    auto forward_sys = builder.addToGraph<ParallelForNode<Engine, _forward, SchedTrajType, \
                                          LocalPortID, GlobalPortID, PktQueue, SwitchID, \
                                          SimTime, SimTimePerUpdate>>({set_forward_plan_sys});

    auto remove_pkts_sys = builder.addToGraph<ParallelForNode<Engine, remove_pkts, \
                                              LocalPortID, GlobalPortID, PktBuf, ForwardPlan, \
                                              SimTime, SimTimePerUpdate>>({forward_sys}); 

    auto transmit_sys = builder.addToGraph<ParallelForNode<Engine, transmit, SchedTrajType, \
    PortType, LocalPortID, GlobalPortID, SwitchID, NextHop, NextHopType, PktQueue, TXHistory, \
    SSLinkDelay, LinkRate, SimTime, SimTimePerUpdate, Seed>>({remove_pkts_sys});       


    // ------------------sys--------------------------------------------------------------
    // auto sys_process_comm = builder.addToGraph<ParallelForNode<Engine, processCommCheckFlow,
    //                                                             NpuID, NodeID, TaskFlows>>({transmit_sys});
    
    // auto sys_remove_node=builder.addToGraph<ParallelForNode<Engine, removeNpuNodes,
    // NpuID, ChakraNodes, HardwareResource, ProcessingCompTask, ProcessingCommTasks>>({sys_process_comm});
    
    // auto sys_skip_time = builder.addToGraph<ParallelForNode<Engine, checkSkipTime, NextProcessTimes>>({sys_remove_node});

    // -----------------------------------------------------------------------------------
    
}

Sim::Sim(Engine &ctx,
         const Config &cfg,
         const WorldInit &)
    : WorldBase(ctx)
{
    // Currently the physics system needs an upper bound on the number of
    // entities that will be stored in the BVH. We plan to fix this in
    // a future release.
    printf("\n***********enter Sim construction function!\n");
    printf("\ncfg.kAray: %d\n", cfg.kAray);
    printf("\ncfg.ccMethod: %d\n", cfg.ccMethod);
    printf("\nLinks[0]: %d\n", cfg.Links[0][0]);
    printf("\nLinks[0]: %d\n", cfg.Links[1][0]); 
    printf("\ncfg.topo:link_num: %d\n", cfg.topo.link_num);
    printf("\ncfg.topo:net_npu_num: %d\n", cfg.topo.net_npu_num);
    printf("\ncfg.topo:aj_link[0][0]: %d\n", cfg.topo.aj_link[0][0]);

    // constexpr CountT max_total_entities = consts::numAgents +
    //     consts::numRooms * (consts::maxEntitiesPerRoom + 3) +
    //     4; // side walls + floor
    constexpr CountT max_total_entities = 1 +
        consts::numRooms * (consts::maxEntitiesPerRoom + 3) +
        4; // side walls + floor

    phys::PhysicsSystem::init(ctx, cfg.rigidBodyObjMgr,
        consts::deltaT, consts::numPhysicsSubsteps, -9.8f * math::up,
        max_total_entities);

    initRandKey = cfg.initRandKey;
    autoReset = cfg.autoReset;

    enableRender = cfg.renderBridge != nullptr;

    if (enableRender) {
        RenderingSystem::init(ctx, cfg.renderBridge);
    }

    curWorldEpisode = 0;

    // // Creates agents, walls, etc.
    // createPersistentEntities(ctx);
    // // Generate initial world state
    // initWorld(ctx);


// copied from level_gen.cpp by gui fei
    // Entity agent = ctx.makeRenderableEntity<Agent>();

    // // Create a render view for the agent
    // if (ctx.data().enableRender) {
    //     render::RenderingSystem::attachEntityToView(ctx,
    //             agent,
    //             100.f, 0.001f,
    //             1.5f * math::up);
    // }

    // ctx.get<Scale>(agent) = Diag3x3 { 1, 1, 1 };
    // ctx.get<ObjectID>(agent) = ObjectID { (int32_t)SimObject::Agent };
    // ctx.get<ResponseType>(agent) = ResponseType::Dynamic;
    // ctx.get<GrabState>(agent).constraintEntity = Entity::none();
    // ctx.get<EntityType>(agent) = EntityType::Agent;

    // ctx.get<CurStep>(agent).step = 0;

//



//
    //***************************************************
    // for the router forwarding function, of GPU_acclerated DES
    numInPort = 0;
    numEPort = 0;

    // numSender = 0;
    // numReceiver = 0;
    numSwitch = 0;  

    // num_snd_flow = 0;
    // num_recv_flow = 0;

    num_nic = 0;
    num_net_npu = 0;
    
    for (int i = 0; i < K_ARY*K_ARY*K_ARY/4; i++) {
        flow_cnt[i] = 0;
    }
    
    printf("Gen entity as following: \n");
    generate_switch(ctx, cfg.kAray);
    generate_in_port(ctx, cfg.kAray);
    generate_e_port(ctx, cfg.kAray);
    generate_host(ctx, cfg.kAray);

    printf("Total %d switches, %d in_ports, %d e_ports, %d nics, %d net_npus\n", \
           ctx.data().numSwitch, ctx.data().numInPort, ctx.data().numEPort, \
           ctx.data().num_nic, ctx.data().num_net_npu);
    // generate_fib_table(ctx, K_ARY);
    //***************************************************
//

    // init
    Entity agent = ctx.makeEntity<Agent>();
    ctx.data().init_entity = agent;

    // sim time entity
    // Entity sp = ctx.makeEntity<SimTimeProcessor>();
    // ctx.get<SimTime>(sp).time = 0;
    // ctx.data().timer_entity = sp;

    // next process time entity
    Entity nextProcessTimeE = ctx.makeEntity<NextProcessTimeE>();
    for (size_t i = 0; i < MAX_CHAKRA_NODES; i++)
    {
        ctx.get<NextProcessTimes>(nextProcessTimeE).times_abs[i] = 0;
    }
    ctx.data().next_process_time_entity = nextProcessTimeE;

    // Sys_config
    Entity sc = ctx.makeEntity<SysConfig>();
    ctx.get<CommModel>(sc).all_reduce_implementation = CommImplementationType::Ring;
    ctx.get<CommModel>(sc).all_gather_implementation = CommImplementationType::Ring;
    ctx.get<CommModel>(sc).reduce_scatter_implementation = CommImplementationType::Ring;
    ctx.get<CommModel>(sc).all_to_all_implementation = CommImplementationType::Ring;
    ctx.data().sys_config_entity = sc;

    // collective topo
    RingTopology ring_topo;
    ring_topo.ring_id = 0;
    ring_topo.total_nodes_in_ring = 10;
    ring_topo.index_in_ring = 0;
    ring_topo.dimension = Dimension::Local;
    ring_topo.id_to_index[0] = 0;
    ring_topo.index_to_id[1] = 1;
    ring_topo.index_to_id[2] = 2;
    ring_topo.id_to_index[3] = 3;
    

}

// This declaration is needed for the GPU backend in order to generate the
// CUDA kernel for world initialization, which needs to be specialized to the
// application's world data type (Sim) and config and initialization types.
// On the CPU it is a no-op.
MADRONA_BUILD_MWGPU_ENTRY(Engine, Sim, Sim::Config, Sim::WorldInit);

}
