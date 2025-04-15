#ifndef NEW_APP_H
#define NEW_APP_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/traced-callback.h"
#include "ns3/data-rate.h"

namespace ns3 {

class Socket;

class NewApp: public Application
{
  public:
      static TypeId GetTypeId (void);
      NewApp();
      virtual ~NewApp();
  private:
      virtual void StartApplication (void);
      virtual void StopApplication (void);

      void ScheduleTx();
      void SendPacket();
      void HandleRead(Ptr<Socket> socket);

      bool m_mode; // Tx: true, Rx: false
      Address m_address;
      uint32_t m_nPackets;
      DataRate m_dataRate;

      Ptr<Socket> m_socket;
      uint32_t m_packetSize;
      uint32_t m_packetsSent;
      EventId m_sendEvent;
      bool m_running;
};

}

#endif
