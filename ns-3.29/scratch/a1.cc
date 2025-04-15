#include <iostream>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/netanim-module.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("netpA1");

// static void
// Rxtime (std::string context, Ptr<const Packet> p, const Address &a)
// {
//     static double bytes1, bytes2 = 0;
//     if (context == "FlowBg") {
//         bytes1 += p->GetSize();
//         NS_LOG_UNCOND("0\t" << Simulator::Now().GetSeconds()
//         << "\t" << bytes1 * 8 / 1000000 / (Simulator::Now().GetSeconds() - 1));
//     } 
//     else if (context == "Flow1") {
//         bytes1 += p->GetSize();
//         NS_LOG_UNCOND("1\t" << Simulator::Now().GetSeconds()
//         << "\t" << bytes1 * 8 / 1000000 / (Simulator::Now().GetSeconds() - 1));
//     } else if (context == "Flow2") {
//         bytes2 += p->GetSize();
//         NS_LOG_UNCOND("2\t" << Simulator::Now().GetSeconds()
//         << "\t" << bytes2 * 8 / 1000000 / (Simulator::Now().GetSeconds() - 3));
//     }
// }

int
main (int argc, char *argv[])
{
    // LogComponentEnable ("OnOffApplication", LOG_LEVEL_INFO);
    // LogComponentEnable ("PacketSink", LOG_LEVEL_INFO);

    CommandLine cmd;
    cmd.Parse(argc, argv);

    Time::SetResolution (Time::NS);
    LogComponentEnable ("netpA1", LOG_LEVEL_ALL);

    NS_LOG_INFO("Create nodes.");
    NodeContainer terminals;
    terminals.Create(4);

    NodeContainer routers;
    routers.Create(2);
    
    NS_LOG_INFO("Build Topology.");

    // NetDeviceContainer terminalDevices;
    // NetDeviceContainer routerDevices;
    
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", StringValue("10us"));

    // 00, 01
    NetDeviceContainer link_n0_r1 = csma.Install(NodeContainer(terminals.Get(0), routers.Get(0)));
    NetDeviceContainer link_n2_r1 = csma.Install(NodeContainer(terminals.Get(2), routers.Get(0)));
    NetDeviceContainer link_n1_r2 = csma.Install(NodeContainer(terminals.Get(1), routers.Get(1)));
    NetDeviceContainer link_n3_r2 = csma.Install(NodeContainer(terminals.Get(3), routers.Get(1)));


    // r1 ↔ r2 (Point-to-Point)
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("50Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("10ms"));
    NetDeviceContainer p2pDevices = p2p.Install(routers);


    InternetStackHelper internet;
    internet.Install (terminals);
    internet.Install (routers);

    NS_LOG_INFO ("Assign IP Addresses.");
    Ipv4AddressHelper address;
    // n0 <-> r1
    address.SetBase("10.1.1.0", "255.255.255.0");
    address.Assign(link_n0_r1);

    // n2 <-> r1
    address.SetBase("10.1.2.0", "255.255.255.0");
    address.Assign(link_n2_r1);

    // n1 <-> r2
    address.SetBase("10.1.3.0", "255.255.255.0");
    address.Assign(link_n1_r2);

    // n3 <-> r2
    address.SetBase("10.1.4.0", "255.255.255.0");
    address.Assign(link_n3_r2);

    // r1 <-> r2
    address.SetBase("10.1.5.0", "255.255.255.0");
    address.Assign(p2pDevices);


    NS_LOG_INFO ("Create Applications.");
    uint16_t bgPort = 8000;
    uint16_t f1Port = 8001;
    uint16_t f2Port = 8002;

    // BG Flow (n2 -> n1)
    OnOffHelper onoffBg ("ns3::UdpSocketFactory",
        Address (InetSocketAddress (Ipv4Address ("10.1.3.1"), bgPort)));
    onoffBg.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1000.0]"));
    onoffBg.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
    onoffBg.SetAttribute ("DataRate", DataRateValue (DataRate("70Mbps")));

    ApplicationContainer app1 = onoffBg.Install (terminals.Get (2));
    app1.Start (Seconds (4.0));
    app1.Stop (Seconds (25.0));

    PacketSinkHelper sinkBg ("ns3::UdpSocketFactory", 
        Address (InetSocketAddress (Ipv4Address::GetAny (), bgPort)));
    
    ApplicationContainer sinkApp1 = sinkBg.Install (terminals.Get (1));
    sinkApp1.Start (Seconds (0.0));
    sinkApp1.Stop (Seconds (25.0));
    // Ptr<PacketSink> pktSink1 = DynamicCast<PacketSink>(sinkApp1.Get(0));
    // pktSink1->TraceConnect("Rx", "FlowBg", MakeCallback(&Rxtime));

    // Flow1 (n0 -> n1)
    OnOffHelper onoffF1 ("ns3::UdpSocketFactory",
        Address (InetSocketAddress (Ipv4Address ("10.1.3.1"), f1Port)));
    onoffF1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=10.0]"));
    onoffF1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
    onoffF1.SetAttribute ("DataRate", DataRateValue (DataRate("90Mbps")));
    
    ApplicationContainer app2 = onoffF1.Install (terminals.Get (0));
    app2.Start (Seconds(10.0));
    app2.Stop (Seconds (20.0));

    PacketSinkHelper sinkF1 ("ns3::UdpSocketFactory", 
        Address (InetSocketAddress (Ipv4Address::GetAny (), f1Port)));

    ApplicationContainer sinkApp2 = sinkF1.Install (terminals.Get (1));
    sinkApp2.Start (Seconds (0.0));
    sinkApp2.Stop (Seconds (20.0));
    // Ptr<PacketSink> pktSink2 = DynamicCast<PacketSink>(sinkApp2.Get(0));
    // pktSink2->TraceConnect("Rx", "Flow1", MakeCallback(&Rxtime));

    
    // Flow2 (n3 -> n0)
    OnOffHelper onoffF2 ("ns3::UdpSocketFactory",
        Address (InetSocketAddress (Ipv4Address ("10.1.1.1"), f2Port)));
    onoffF2.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=16.0]"));
    onoffF2.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
    onoffF2.SetAttribute ("DataRate", DataRateValue (DataRate("100Mbps")));
    
    ApplicationContainer app3 = onoffF2.Install (terminals.Get (3));
    app3.Start (Seconds(12.0));
    app3.Stop (Seconds (28.0));

    PacketSinkHelper sinkF2 ("ns3::UdpSocketFactory", 
        Address (InetSocketAddress (Ipv4Address::GetAny (), f2Port)));

    ApplicationContainer sinkApp3 = sinkF2.Install (terminals.Get (0));
    sinkApp3.Start (Seconds (0.0));
    sinkApp3.Stop (Seconds (28.0));
    // // Ptr<PacketSink> pktSink3 = DynamicCast<PacketSink>(sinkApp3.Get(0));
    // // pktSink3->TraceConnect("Rx", "Flow2", MakeCallback(&Rxtime));

    /* Enable Pcap tracing */
    csma.EnablePcapAll ("a1-csma", false);
    p2p.EnablePcapAll("a1-p2p", false);


    for (uint32_t i = 0; i < terminals.GetN(); ++i) {
        Ptr<Ipv4> ipv4 = terminals.Get(i)->GetObject<Ipv4>();
        for (uint32_t j = 0; j < ipv4->GetNInterfaces(); ++j) {
            std::cout << "terminals " << i << " Interface " << j << ": " 
                      << ipv4->GetAddress(j, 0).GetLocal() << std::endl;
        }
    }

    for (uint32_t i = 0; i < routers.GetN(); ++i) {
        Ptr<Ipv4> ipv4 = routers.Get(i)->GetObject<Ipv4>();
        for (uint32_t j = 0; j < ipv4->GetNInterfaces(); ++j) {
            std::cout << "routers " << i << " Interface " << j << ": " 
                      << ipv4->GetAddress(j, 0).GetLocal() << std::endl;
        }
    }
    

    // AnimationInterface anim ("a1.xml");

    // anim.SetConstantPosition(terminals.Get(0), 10.0, 30.0); // n0
    // anim.SetConstantPosition(terminals.Get(1), 90.0, 30.0); // n1
    // anim.SetConstantPosition(terminals.Get(2), 10.0, 10.0); // n2
    // anim.SetConstantPosition(terminals.Get(3), 90.0, 10.0); // n3

    // anim.SetConstantPosition(routers.Get(0), 30.0, 20.0);   // r0
    // anim.SetConstantPosition(routers.Get(1), 70.0, 20.0);   // r1

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    NS_LOG_INFO ("Run Simulation.");
    Simulator::Run ();
    Simulator::Destroy ();
    NS_LOG_INFO ("Done.");
}
