#include "new-app-helper.h"
#include "ns3/new-app.h"
#include "ns3/uinteger.h"
#include "ns3/names.h"
#include "ns3/boolean.h"

using namespace ns3;

NewAppHelper::NewAppHelper (bool mode, Address address) {
    m_factory.SetTypeId(NewApp::GetTypeId());
    m_factory.Set("Mode", BooleanValue(mode));
    m_factory.Set("Address", AddressValue(address));
}

void NewAppHelper::SetAttribute (std::string name, const AttributeValue &value)
{
    m_factory.Set(name, value);
}

Ptr<Application>
NewAppHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<NewApp> ();
  node ->AddApplication (app);

  return app;
}

ApplicationContainer NewAppHelper::Install(Ptr<Node> node) const 
{
    return ApplicationContainer(InstallPriv(node));
}

ApplicationContainer NewAppHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer NewAppHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
  {
      apps.Add (InstallPriv (*i));
  }

  return apps;
}
