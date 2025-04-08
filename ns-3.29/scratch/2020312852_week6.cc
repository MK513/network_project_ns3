#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/traced-callback.h"
#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/core-module.h"
#include <iostream>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/bridge-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;
NS_LOG_COMPONENT_DEFINE("NewApp");

class NewApp : public Application {
public:
    static TypeId GetTypeId (void);
    NewApp ();
    virtual ~NewApp();

private:
    virtual void StartApplication(void);
    virtual void StopApplication(void);

    void ScheduleTx(void);
    void SendPacket(void);
    void HandleRead(Ptr<Socket> socket);

    bool m_mode;
    Address m_address;
    uint32_t m_nPackets;
    DataRate m_dataRate;

    Ptr<Socket> m_socket;
    uint32_t m_packetSize;
    uint32_t m_packetSent;
    EventId m_sendEvent;
    bool m_running;

};

TypeId
NewApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NewApp")
    .SetParent<Application> ()
    .AddConstructor<NewApp> ()
    .AddAttribute ("Mode", 
                   "sender true, receiver false",
                   BooleanValue (false),
                   MakeBooleanAccessor (&NewApp::m_mode),
                   MakeBooleanChecker())
    .AddAttribute ("Address", 
                   "The Address",
                   AddressValue (),
                   MakeAddressAccessor (&NewApp::m_address),
                   MakeAddressChecker ())
    .AddAttribute ("NPackets", 
                   "packet",
                   UintegerValue (10000),
                   MakeUintegerAccessor (&NewApp::m_nPackets),
                   MakeUintegerChecker<uint32_t>())
    .AddAttribute ("DataRate", 
                    "The data rate",
                    DataRateValue (DataRate("500kb/s")),
                    MakeDataRateAccessor (&NewApp::m_dataRate),
                    MakeDataRateChecker ())
    ;
  return tid;
}

NewApp::NewApp()
    :m_socket(0),
    m_packetSize(1000),
    m_packetSent(0),
    m_running(false)
{
    NS_LOG_FUNCTION(this);
}

NewApp::~NewApp()
{
    m_socket = 0;
}

void NewApp::StartApplication()
{
    if (m_mode == true) {
        if (!m_socket) {
            TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
            m_socket = Socket::CreateSocket(GetNode(), tid);
            m_socket->Bind();
            m_socket->Connect(m_address);
        }
        m_running = true;
        SendPacket();
    } else {
        if (!m_socket) {
            TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
            m_socket = Socket::CreateSocket(GetNode(), tid);
            m_socket->Bind(m_address);
            m_socket->Listen();
            m_socket->ShutdownSend();
            m_socket->SetRecvCallback(MakeCallback(&NewApp::HandleRead, this));
        }
    }
}

void NewApp::SendPacket(void)
{
    Ptr<Packet> packet = Create<Packet> (m_packetSize);

    m_socket->Send(packet);

    std::ostringstream oss;

    InetSocketAddress iaddr = InetSocketAddress::ConvertFrom (m_address);  
    oss << "At time " << Simulator::Now().GetSeconds()
            << "s client sent " << packet->GetSize() << " bytes to "
            << iaddr.GetIpv4() << " port " << iaddr.GetPort();
    NS_LOG_INFO(oss.str());

    // NS_LOG_INFO("NewApp Sent Packet");

    if (++m_packetSent < m_nPackets)
    {
        ScheduleTx();
    }
}
    
void NewApp::ScheduleTx()
{
    if (m_running)
    {
        Time tNext(
            Seconds(m_packetSize * 8 / static_cast<double>(m_dataRate.GetBitRate())));
        m_sendEvent = Simulator::Schedule(tNext, &NewApp::SendPacket, this);
    }
}

void NewApp::HandleRead(Ptr<Socket> socket)
{
    Ptr<Packet> packet;
    Address from;
    while ((packet = m_socket->RecvFrom(from)))
    {
        if (packet->GetSize() > 0)
        {
            std::ostringstream oss;
            InetSocketAddress iaddr = InetSocketAddress::ConvertFrom (from); 

            oss << "At time " << Simulator::Now().GetSeconds()
                    << "s server received " << packet->GetSize() << " bytes from "
                    << iaddr.GetIpv4() << " port " << iaddr.GetPort();
            NS_LOG_INFO(oss.str());
            // NS_LOG_INFO("NewApp Received Packet");
        }
    }
}

void NewApp::StopApplication()
{
    m_running = false;
    if (m_sendEvent.IsRunning())
    {
        Simulator::Cancel(m_sendEvent);
    }
    if (m_socket)
    {
        m_socket->Close();
    }
}

class NewAppHelper {
public:
    NewAppHelper (bool mode, Address address);
    void SetAttribute(std::string name, const AttributeValue &value);
    ApplicationContainer Install (Ptr<Node> node) const;
    ApplicationContainer Install (std::string nodeName) const;
    ApplicationContainer Install (NodeContainer c) const;

private:
    Ptr<Application> InstallPriv (Ptr<Node> node) const;
    ObjectFactory m_factory;
};

NewAppHelper::NewAppHelper (bool mode, Address address) {
    m_factory.SetTypeId(NewApp::GetTypeId());
    m_factory.Set("Mode", BooleanValue(mode));
    m_factory.Set("Address", AddressValue(address));
}

void NewAppHelper::SetAttribute (
    std::string name,
    const AttributeValue &value
) {
    m_factory.Set(name, value);
}

Ptr<Application>
NewAppHelper::InstallPriv (Ptr<Node> node) const {
    Ptr<Application> app = m_factory.Create<NewApp> ();
    node->AddApplication(app);
    return app;
}

ApplicationContainer NewAppHelper::Install(Ptr<Node> node) const {
    return ApplicationContainer(InstallPriv(node));
}

ApplicationContainer NewAppHelper::Install(std::string nodeName) const {
    Ptr<Node> node = Names::Find<Node> (nodeName);
    return ApplicationContainer(InstallPriv(node));
}

ApplicationContainer NewAppHelper::Install(NodeContainer c) const {
    ApplicationContainer apps;
    for (NodeContainer::Iterator i = c.Begin(); i != c.End(); ++i)
    {
        apps.Add(InstallPriv(*i));
    }
    return apps;
}



int main(int argc, char *argv[]) {
    
    Time::SetResolution (Time::NS);
    LogComponentEnable ("NewApp", LOG_LEVEL_INFO);

    // NS_LOG_INFO ("Create nodes.");
    NodeContainer nodes;
    nodes.Create (2);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

    NetDeviceContainer devices;
    devices = pointToPoint.Install (nodes);

    InternetStackHelper stack;
    stack.Install (nodes);

    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.0");

    Ipv4InterfaceContainer interfaces = address.Assign (devices);

    //Sender ( Client )

    uint16_t port = 8080;
    Address destination (InetSocketAddress(interfaces.GetAddress(1), port));

    NewAppHelper sender (true, destination);
    sender.SetAttribute("NPackets", UintegerValue(10));
    sender.SetAttribute("DataRate", DataRateValue(DataRate("2Mb/s")));
    ApplicationContainer senderApp = sender.Install(nodes.Get(0));

    senderApp.Start(Seconds(1.0));
    senderApp.Stop(Seconds(5.0));

    //Receiver ( Server )
    Address any (InetSocketAddress(Ipv4Address::GetAny(), port));

    NewAppHelper receiver (false, any);
    ApplicationContainer receiverApp = receiver.Install(nodes.Get(1));

    receiverApp.Start(Seconds(0.5));
    receiverApp.Stop(Seconds(7.0));

    // NS_LOG_INFO ("Run Simulation.");
    Simulator::Run ();
    Simulator::Destroy ();
    // NS_LOG_INFO ("Done.");
}