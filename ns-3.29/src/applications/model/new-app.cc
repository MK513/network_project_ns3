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
#include "ns3/boolean.h"
#include "new-app.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("NewApp");
NS_OBJECT_ENSURE_REGISTERED (NewApp);

TypeId
NewApp::GetTypeId() {
  static TypeId tid = TypeId ("ns3::NewApp")
      .SetParent<Application> ()
      .AddConstructor<NewApp>()
      .AddAttribute ("Mode", "The mode : Sender(true), Receiver(false)",
	  BooleanValue(false),
	  MakeBooleanAccessor(&NewApp::m_mode),
	  MakeBooleanChecker())
      .AddAttribute ("Address", "The Address",
	  AddressValue(),
	  MakeAddressAccessor(&NewApp::m_address),
	  MakeAddressChecker())
      .AddAttribute ("NPackets", "The total number of packets to send",
	  UintegerValue(10000),
	  MakeUintegerAccessor(&NewApp::m_nPackets),
	  MakeUintegerChecker<uint32_t>())
      .AddAttribute ("DataRate", "The data rate",
	  DataRateValue(DataRate("500kb/s")),
	  MakeDataRateAccessor(&NewApp::m_dataRate),
	  MakeDataRateChecker())
      ;
  return tid;
}

NewApp::NewApp()
    :m_socket(0),
     m_packetSize(1000),
     m_packetsSent(0),
     m_running(false)
{
     NS_LOG_FUNCTION (this);
}

NewApp::~NewApp()
{
  m_socket = 0;
}

void NewApp::StartApplication()
{
    NS_LOG_FUNCTION (this);
    if(m_mode == true)
    {
        if(!m_socket){
	    TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
	    m_socket = Socket::CreateSocket(GetNode(), tid);
	    m_socket ->Bind();
	    m_socket ->Connect(m_address);
	}
	m_running = true;
	SendPacket();
    } else {
	if(!m_socket){
	    TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
	    m_socket = Socket::CreateSocket(GetNode(), tid);
	    m_socket ->Bind(m_address);
	    m_socket ->Listen();
	    m_socket ->ShutdownSend();
	    m_socket ->SetRecvCallback(MakeCallback(&NewApp::HandleRead, this));
	}
    }
}

void NewApp::SendPacket(void)
{
    Ptr<Packet> packet = Create<Packet> (m_packetSize);

    m_socket ->Send(packet);

    if (InetSocketAddress::IsMatchingType (m_address))
    {
    	NS_LOG_INFO("At time " << Simulator::Now ().GetSeconds () << "s client sent " << m_packetSize << " bytes to " <<
		     InetSocketAddress::ConvertFrom (m_address).GetIpv4 () << " port " << 
		     InetSocketAddress::ConvertFrom (m_address).GetPort ());
    }       
    else if (Inet6SocketAddress::IsMatchingType (m_address))
    {
    	NS_LOG_INFO("At time " << Simulator::Now ().GetSeconds () << "s client sent " << m_packetSize << " bytes to " <<
		     Inet6SocketAddress::ConvertFrom (m_address).GetIpv6 () << " port " << 
		     Inet6SocketAddress::ConvertFrom (m_address).GetPort ());
    }       
   
    if(++m_packetsSent < m_nPackets) 
    {
        ScheduleTx();
    }
}

void NewApp::ScheduleTx()
{
    if(m_running)
    {
	Time tNext (
	    Seconds (m_packetSize*8/static_cast<double>(m_dataRate.GetBitRate())));
	m_sendEvent = Simulator::Schedule(tNext, &NewApp::SendPacket, this);
    }
}

void NewApp::HandleRead(Ptr<Socket> socket)
{
  Ptr<Packet> packet;
  Address from;
  while ((packet = m_socket ->RecvFrom(from)))
  {
    if (InetSocketAddress::IsMatchingType (from))
      {
        NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server received " << packet->GetSize () << " bytes from " <<
                     InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " <<
                     InetSocketAddress::ConvertFrom (from).GetPort ());
      }
    else if (Inet6SocketAddress::IsMatchingType (from))
      {
        NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server received " << packet->GetSize () << " bytes from " <<
                     Inet6SocketAddress::ConvertFrom (from).GetIpv6 () << " port " <<
                     Inet6SocketAddress::ConvertFrom (from).GetPort ());
      }
  }
}

void NewApp::StopApplication()
{
  m_running = false;
  if(m_sendEvent.IsRunning())
  {
      Simulator::Cancel (m_sendEvent);
  }
  if(m_socket)
  {
      m_socket ->Close();
  }
}


