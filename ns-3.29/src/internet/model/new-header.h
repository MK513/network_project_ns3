#ifndef NEW_HEADER_H
#define NEW_HEADER_H

#include "ns3/header.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"

namespace ns3 {

class NewHeader : public Header
{
public:
    static TypeId GetTypeId (void);
    virtual TypeId GetInstanceTypeId (void) const;

    virtual void Print(std::ostream & os) const;
    virtual uint32_t GetSerializedSize (void) const;
    virtual void Serialize(Buffer::Iterator start) const;
    virtual uint32_t Deserialize(Buffer::Iterator start);

    void SetTime(void);
    uint64_t GetTime(void) const;
private:
    uint64_t m_time;
};

}

#endif