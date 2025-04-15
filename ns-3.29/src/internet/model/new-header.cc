#include "new-header.h"
#include "ns3/address-utils.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("NewHeader");

TypeId NewHeader::GetTypeId (void) {
    static TypeId tid = TypeId("ns3::NewHeader")
    .SetParent<Header> ()
    .AddConstructor<NewHeader> ();
    return tid;
}

TypeId NewHeader::GetInstanceTypeId (void) const {
    return GetTypeId();
}

void NewHeader::SetTime (void) {
    m_time = Simulator::Now().GetMicroSeconds();
}

uint64_t NewHeader::GetTime (void) const {
    return m_time;
}
uint32_t NewHeader::GetSerializedSize (void) const {
    return 8;
}

void NewHeader::Serialize (Buffer::Iterator start) const {
    start.WriteHtonU64(m_time);
}

uint32_t NewHeader::Deserialize(Buffer::Iterator start) {
    Buffer::Iterator i = start;
    m_time = i.ReadNtohU64();
    return i.GetDistanceFrom(start);
}

void NewHeader::Print(std::ostream &os) const {
    os << "m_time = " << m_time << "\n";
}

}