#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Rip-test");


int main (int argc, char **argv)
{
	LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
	LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

	CommandLine cmd;
	cmd.Parse (argc, argv);

	uint32_t np2p = 3;
	uint32_t ncsma1 =3;
	uint32_t ncsma2 =4;

	NodeContainer p2pNodes;
	p2pNodes.Create (np2p);

	RipHelper ripRouting;
 	Ipv4ListRoutingHelper RHlist;
 	RHlist.Add (ripRouting, 1);


  	InternetStackHelper stack1;
  	stack1.SetRoutingHelper (RHlist);
 	stack1.Install (p2pNodes);

 	PointToPointHelper p2pHelper;
    p2pHelper.SetDeviceAttribute ("DataRate", StringValue ("50Mbps"));
    p2pHelper.SetChannelAttribute ("Delay", StringValue ("1ms")); 


 	NetDeviceContainer p2pDevices1 = p2pHelper.Install (p2pNodes.Get(0) , p2pNodes.Get(1)); 
	NetDeviceContainer p2pDevices2 = p2pHelper.Install (p2pNodes.Get(1) , p2pNodes.Get(2));


	Ipv4AddressHelper ipv4;
	ipv4.SetBase (Ipv4Address ("10.0.0.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer p2p1 = ipv4.Assign (p2pDevices1);
	ipv4.SetBase (Ipv4Address ("10.0.1.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer p2p2 = ipv4.Assign (p2pDevices2);




	NodeContainer csma1Nodes;
  	csma1Nodes.Create (ncsma1);

 	NodeContainer csma2Nodes;
	csma2Nodes.Create (ncsma2);

	InternetStackHelper stack2;
 	stack2.Install (csma1Nodes);
 	stack2.Install (csma2Nodes);

 	csma1Nodes.Add (p2pNodes.Get (0));
 	csma2Nodes.Add (p2pNodes.Get (np2p - 1));

 	CsmaHelper csmaHelper;
 	csmaHelper.SetChannelAttribute ("DataRate", StringValue ("5Mbps"));  
 	csmaHelper.SetChannelAttribute ("Delay", TimeValue (MicroSeconds (5)));

 	NetDeviceContainer csma1Devices = csmaHelper.Install (csma1Nodes);
 	NetDeviceContainer csma2Devices = csmaHelper.Install (csma2Nodes);

 	ipv4.SetBase (Ipv4Address ("10.0.2.0"), Ipv4Mask ("255.255.255.0"));
 	Ipv4InterfaceContainer csma1 = ipv4.Assign (csma1Devices);
 	ipv4.SetBase (Ipv4Address ("10.0.3.0"), Ipv4Mask ("255.255.255.0"));
 	Ipv4InterfaceContainer csma2 = ipv4.Assign (csma2Devices);

  	Ptr<Ipv4StaticRouting> staticRouting;
 	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (csma2Nodes.Get (0)->GetObject<Ipv4> ()->GetRoutingProtocol ());
 	staticRouting->SetDefaultRoute ("10.0.3.5", 1 );
 	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (csma1Nodes.Get (0)->GetObject<Ipv4> ()->GetRoutingProtocol ());
 	staticRouting->SetDefaultRoute ("10.0.2.4", 1 );
 	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (csma2Nodes.Get (1)->GetObject<Ipv4> ()->GetRoutingProtocol ());
 	staticRouting->SetDefaultRoute ("10.0.3.5", 1 );

 	UdpEchoServerHelper echoServer (9);
 	ApplicationContainer serverApps = echoServer.Install (csma1Nodes.Get (0));
 	serverApps.Start (Seconds (10.0));
 	serverApps.Stop (Seconds (500.0));

 	UdpEchoClientHelper echoClient (csma1.GetAddress (0), 9);
 	echoClient.SetAttribute ("MaxPackets", UintegerValue (2));
 	echoClient.SetAttribute ("Interval", TimeValue (Seconds (1)));
 	echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

 	ApplicationContainer clientApps = echoClient.Install (csma2Nodes.Get (0));
 	clientApps.Start (Seconds (50.0));
  	clientApps.Stop (Seconds (500.0));

 	UdpEchoClientHelper echoClient2 (csma1.GetAddress (0), 9);
 	echoClient2.SetAttribute ("MaxPackets", UintegerValue (2));
 	echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1)));
 	echoClient2.SetAttribute ("PacketSize", UintegerValue (1024));

 	ApplicationContainer clientApps2 = echoClient2.Install (csma2Nodes.Get (1));
 	clientApps2.Start (Seconds (55.0));
  	clientApps2.Stop (Seconds (500.0));

        Simulator::Stop (Seconds (500.0));
  	Simulator::Run ();
  	Simulator::Destroy ();

  	return 0;


}
