#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

int main () {
  Time::SetResolution (Time::NS);
  LogComponentEnable ("OnOffApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("PacketSink", LOG_LEVEL_INFO);

  // 노드 생성: 0 - sender, 1 - router, 2 - receiver
  NodeContainer nodes;
  nodes.Create (3);

  // 링크 설정
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer d1 = p2p.Install (nodes.Get(0), nodes.Get(1));
  NetDeviceContainer d2 = p2p.Install (nodes.Get(1), nodes.Get(2));

  // 인터넷 스택 설치
  InternetStackHelper stack;
  stack.InstallAll();

  // IP 주소 설정
  Ipv4AddressHelper address;

  address.SetBase ("10.0.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i1 = address.Assign (d1);

  address.SetBase ("10.0.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i2 = address.Assign (d2);

  // 라우팅 테이블 자동 설정
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  // UDP Sink 설치 (노드 2)
  uint16_t port = 9; // 임의의 포트
  Address sinkAddress (InetSocketAddress (i2.GetAddress (1), port));
  PacketSinkHelper sinkHelper ("ns3::UdpSocketFactory", sinkAddress);
  ApplicationContainer sinkApp = sinkHelper.Install (nodes.Get (2));
  sinkApp.Start (Seconds (0.0));
  sinkApp.Stop (Seconds (10.0));

  // OnOffApplication 설치 (노드 0 → 노드 2)
  OnOffHelper onoff ("ns3::UdpSocketFactory", sinkAddress);
  onoff.SetAttribute ("DataRate", StringValue ("1Mbps"));
  onoff.SetAttribute ("PacketSize", UintegerValue (1024));
  onoff.SetAttribute ("StartTime", TimeValue (Seconds (1.0)));
  onoff.SetAttribute ("StopTime", TimeValue (Seconds (9.0)));

  ApplicationContainer app = onoff.Install (nodes.Get (0));

  // pcap 캡처
  p2p.EnablePcapAll ("test");

  Simulator::Stop (Seconds (10.0));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}