/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

 #include "ns3/core-module.h"
 #include "ns3/network-module.h"
 #include "ns3/internet-module.h"
 #include "ns3/point-to-point-module.h"
 #include "ns3/applications-module.h"
 
 using namespace ns3;
 
 NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");
 
 int
 main (int argc, char *argv[])
 {
   std::string delay = "2ms";
   std::string bw = "5Mbps";
 
   CommandLine cmd;
   cmd.AddValue("Delay", "Line Delay", delay);
   cmd.AddValue("DataRate", "Bandwidth", bw);
   cmd.Parse (argc, argv);
 
   
   Time::SetResolution (Time::NS);
   //LogComponentEnable ("UdpClientApplication", LOG_LEVEL_INFO);
   //LogComponentEnable ("UdpServerApplication", LOG_LEVEL_INFO);
 
   NodeContainer nodes;
   nodes.Create (2);
 
   PointToPointHelper pointToPoint;
   //pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
   //pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
   pointToPoint.SetDeviceAttribute ("DataRate", StringValue (bw));
   pointToPoint.SetChannelAttribute("Delay", StringValue(delay));
 
   NetDeviceContainer devices;
   devices = pointToPoint.Install (nodes);
 
   InternetStackHelper stack;
   stack.Install (nodes);
 
   Ipv4AddressHelper address;
   address.SetBase ("10.1.1.0", "255.255.255.0");
 
   Ipv4InterfaceContainer interfaces = address.Assign (devices);
 
   UdpServerHelper udpServer (9);
 
   ApplicationContainer serverApps = udpServer.Install (nodes.Get (1));
   serverApps.Start (Seconds (1.0));
   serverApps.Stop (Seconds (10.0));

 
   UdpClientHelper udpClient (interfaces.GetAddress (1), 9);
   //udpClient.SetAttribute ("MaxPackets", UintegerValue (1));
   //udpClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
   //udpClient.SetAttribute ("PacketSize", UintegerValue (1024));
   udpClient.SetAttribute("MaxPackets", UintegerValue(10000));
   udpClient.SetAttribute ("Interval", TimeValue (Seconds (0.008192)));
   udpClient.SetAttribute ("PacketSize", UintegerValue (1024));
 
   ApplicationContainer clientApps = udpClient.Install (nodes.Get (0));
   clientApps.Start (Seconds (1.0));
   clientApps.Stop (Seconds (10.0));

   pointToPoint.EnablePcapAll("2020312852");
 
   Simulator::Run ();
   Simulator::Destroy ();
   return 0;
 }
 