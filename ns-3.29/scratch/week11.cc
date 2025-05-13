#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("week11");
int main (int argc, char *argv[])
{
    LogComponentEnable("UdpReliableEchoClientApplication", LOG_LEVEL_INFO);
    //LogComponentEnable("UdpReliableEchoServerApplication", LOG_LEVEL_INFO);

    Ptr<Node> nSrc1 = CreateObject<Node> ();
    Ptr<Node> nSrc2 = CreateObject<Node> ();
    Ptr<Node> nRtr = CreateObject<Node> ();
    Ptr<Node> nDst = CreateObject<Node> ();

    NodeContainer nodes = NodeContainer (nSrc1, nSrc2, nRtr, nDst);

    NodeContainer nSrc1nRtr = NodeContainer(nSrc1, nRtr);
    NodeContainer nSrc2nRtr = NodeContainer(nSrc2, nRtr);
    NodeContainer nRtrnDst = NodeContainer(nRtr, nDst);

    InternetStackHelper stack;
    stack.Install (nodes);

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
    p2p.SetChannelAttribute ("Delay", StringValue ("5ms"));
    p2p.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("1500B"));

    NetDeviceContainer dSrc1dRtr = p2p.Install (nSrc1nRtr);
    NetDeviceContainer dSrc2dRtr = p2p.Install (nSrc2nRtr);
    NetDeviceContainer dRtrdDst = p2p.Install (nRtrnDst);

    Ipv4AddressHelper ipv4;
    ipv4.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer iSrc1iRtr = ipv4.Assign (dSrc1dRtr);
    ipv4.SetBase ("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer iSrc2iRtr = ipv4.Assign (dSrc2dRtr);
    ipv4.SetBase ("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer iRtriDst = ipv4.Assign (dRtrdDst);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	uint16_t sinkPort = 8080;
	uint16_t reliablePort = 9090;
	Address sinkAddress (InetSocketAddress (iRtriDst.GetAddress (1), sinkPort));
	Address reliableAddress (InetSocketAddress (iRtriDst.GetAddress (1), reliablePort));

	PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", sinkAddress);
	ApplicationContainer sinkApp = packetSinkHelper.Install(nDst);
	sinkApp.Start (Seconds (0.));
	sinkApp.Stop (Seconds (31.));

	UdpReliableEchoServerHelper udpServerHelper (reliablePort);
	ApplicationContainer serverApp = udpServerHelper.Install(nDst);
	serverApp.Start (Seconds (0.));
	serverApp.Stop (Seconds (31.));

	OnOffHelper onoff("ns3::UdpSocketFactory", sinkAddress);
	onoff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
	onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
	onoff.SetAttribute("DataRate", DataRateValue(10000000));
	onoff.SetAttribute("PacketSize", UintegerValue(512));
	ApplicationContainer sourceApp = onoff.Install(nSrc2);
	sourceApp.Start (Seconds (1.));
	sourceApp.Stop (Seconds (30.));

	UdpReliableEchoClientHelper clientHelper(reliableAddress, reliablePort);
	clientHelper.SetAttribute("PacketSize", UintegerValue(1024));
	clientHelper.SetAttribute("MaxPackets", UintegerValue(1000000));
	clientHelper.SetAttribute("Interval", TimeValue(Seconds(0.01)));
	ApplicationContainer clientApp = clientHelper.Install(nSrc1);
	clientApp.Start (Seconds (1.));
	clientApp.Stop (Seconds (30.));

	Simulator::Stop (Seconds (33));
	Simulator::Run ();
	Simulator::Destroy ();
}
