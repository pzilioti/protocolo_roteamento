#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-module.h"
#include "ns3/netanim-module.h"

// Topologia da rede
//         S1    c14 r    0.0      1.0    r  c4 c4 c14    r    2.0         3.0   r   S2      c14
// 	   ==========1----------2---------3===============11----------12---------13================
//         |  |  |       p2p1       p2p1     |  |  |  |       p2p2         p2p2      |   |   |   |
//         4  5  6                           7  8  9  10 r                           14  15  16  17  
//         csma1                             csma2    |                              csma3
//                                                    | p2p3 4.0
//						      |
//                                                    |    5.0   r   c4          c4          c14
//                                                    18---------19============================
//                                                         p2p3      |   |   |   |   |   |   |
//                                                                   20  21  22  23  24  25  26
//                                                                   csma4
  
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("RIP - Distance Vector");

int main (int argc, char **argv)
{
	LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
	LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

	CommandLine cmd;
	cmd.Parse (argc, argv);

	uint32_t np2p = 3;
	uint32_t ncsma1 = 3;
	uint32_t ncsma2 = 3;
	uint32_t ncsma3 = 4;
	uint32_t ncsma4 = 7;

	// Configuração do nodes p2p1
	NodeContainer p2p1Nodes;
	p2p1Nodes.Create (np2p);

	RipHelper ripRouting1;
 	Ipv4ListRoutingHelper RHlist1;
 	RHlist1.Add (ripRouting1, 1);

  	InternetStackHelper stack1;
  	stack1.SetRoutingHelper (RHlist1);
 	stack1.Install (p2p1Nodes);

 	PointToPointHelper p2p1Helper;
    	p2p1Helper.SetDeviceAttribute ("DataRate", StringValue ("50Mbps"));
    	p2p1Helper.SetChannelAttribute ("Delay", StringValue ("1ms")); 

 	NetDeviceContainer p2p1Devices1 = p2p1Helper.Install (p2p1Nodes.Get(0) , p2p1Nodes.Get(1)); 
	NetDeviceContainer p2p1Devices2 = p2p1Helper.Install (p2p1Nodes.Get(1) , p2p1Nodes.Get(2));

	Ipv4AddressHelper ipv4;
	ipv4.SetBase (Ipv4Address ("10.0.0.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer p2p11 = ipv4.Assign (p2p1Devices1);
	ipv4.SetBase (Ipv4Address ("10.0.1.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer p2p12 = ipv4.Assign (p2p1Devices2);

	// Configuração do nodes p2p2
	NodeContainer p2p2Nodes;
	p2p2Nodes.Create (np2p);

	RipHelper ripRouting2;
 	Ipv4ListRoutingHelper RHlist2;
 	RHlist2.Add (ripRouting2, 1);

  	InternetStackHelper stack2;
  	stack2.SetRoutingHelper (RHlist2);
 	stack2.Install (p2p2Nodes);

 	PointToPointHelper p2p2Helper;
    	p2p2Helper.SetDeviceAttribute ("DataRate", StringValue ("50Mbps"));
    	p2p2Helper.SetChannelAttribute ("Delay", StringValue ("1ms")); 

 	NetDeviceContainer p2p2Devices1 = p2p2Helper.Install (p2p2Nodes.Get(0) , p2p2Nodes.Get(1)); 
	NetDeviceContainer p2p2Devices2 = p2p2Helper.Install (p2p2Nodes.Get(1) , p2p2Nodes.Get(2));

	ipv4.SetBase (Ipv4Address ("10.0.2.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer p2p21 = ipv4.Assign (p2p2Devices1);
	ipv4.SetBase (Ipv4Address ("10.0.3.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer p2p22 = ipv4.Assign (p2p2Devices2);

	// Configuração do nodes p2p3
	NodeContainer p2p3Nodes;
	p2p3Nodes.Create (np2p);

	RipHelper ripRouting3;
 	Ipv4ListRoutingHelper RHlist3;
 	RHlist3.Add (ripRouting3, 1);

  	InternetStackHelper stack3;
  	stack3.SetRoutingHelper (RHlist3);
 	stack3.Install (p2p3Nodes);

 	PointToPointHelper p2p3Helper;
    	p2p3Helper.SetDeviceAttribute ("DataRate", StringValue ("50Mbps"));
    	p2p3Helper.SetChannelAttribute ("Delay", StringValue ("1ms")); 

 	NetDeviceContainer p2p3Devices1 = p2p3Helper.Install (p2p3Nodes.Get(0) , p2p3Nodes.Get(1)); 
	NetDeviceContainer p2p3Devices2 = p2p3Helper.Install (p2p3Nodes.Get(1) , p2p3Nodes.Get(2));

	ipv4.SetBase (Ipv4Address ("10.0.4.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer p2p31 = ipv4.Assign (p2p3Devices1);
	ipv4.SetBase (Ipv4Address ("10.0.5.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer p2p32 = ipv4.Assign (p2p3Devices2);

	// Configurações dos nodes csma 1, 2, 3 e 4
	NodeContainer csma1Nodes;
  	csma1Nodes.Create (ncsma1);

 	NodeContainer csma2Nodes;
	csma2Nodes.Create (ncsma2);

 	NodeContainer csma3Nodes;
	csma3Nodes.Create (ncsma3);

 	NodeContainer csma4Nodes;
	csma4Nodes.Create (ncsma4);

	InternetStackHelper stackCmsa;
 	stackCmsa.Install (csma1Nodes);
 	stackCmsa.Install (csma2Nodes);
	stackCmsa.Install (csma3Nodes);
	stackCmsa.Install (csma4Nodes);

 	csma1Nodes.Add (p2p1Nodes.Get (0));
 	csma2Nodes.Add (p2p1Nodes.Get (np2p - 1));
	csma2Nodes.Add (p2p2Nodes.Get (0));
	csma2Nodes.Add (p2p3Nodes.Get (0));
	csma3Nodes.Add (p2p2Nodes.Get (np2p - 1));
	csma4Nodes.Add (p2p3Nodes.Get (np2p - 1));

 	CsmaHelper csmaHelper;
 	csmaHelper.SetChannelAttribute ("DataRate", StringValue ("5Mbps"));  
 	csmaHelper.SetChannelAttribute ("Delay", TimeValue (MicroSeconds (5)));

 	NetDeviceContainer csma1Devices = csmaHelper.Install (csma1Nodes);
 	NetDeviceContainer csma2Devices = csmaHelper.Install (csma2Nodes);
	NetDeviceContainer csma3Devices = csmaHelper.Install (csma3Nodes);
	NetDeviceContainer csma4Devices = csmaHelper.Install (csma4Nodes);

 	ipv4.SetBase (Ipv4Address ("10.0.6.0"), Ipv4Mask ("255.255.255.0"));
 	Ipv4InterfaceContainer csma1 = ipv4.Assign (csma1Devices);
 	ipv4.SetBase (Ipv4Address ("10.0.7.0"), Ipv4Mask ("255.255.255.0"));
 	Ipv4InterfaceContainer csma2 = ipv4.Assign (csma2Devices);
 	ipv4.SetBase (Ipv4Address ("10.0.8.0"), Ipv4Mask ("255.255.255.0"));
 	Ipv4InterfaceContainer csma3 = ipv4.Assign (csma3Devices);
 	ipv4.SetBase (Ipv4Address ("10.0.9.0"), Ipv4Mask ("255.255.255.0"));
 	Ipv4InterfaceContainer csma4 = ipv4.Assign (csma4Devices);

  	Ptr<Ipv4StaticRouting> staticRouting;
	// do servidor 4 para o roteador
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (csma1Nodes.Get (0)->GetObject<Ipv4> ()->GetRoutingProtocol ());
 	staticRouting->SetDefaultRoute ("10.0.6.4", 1 );
	// do cliente 7 para o roteador
 	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (csma2Nodes.Get (0)->GetObject<Ipv4> ()->GetRoutingProtocol ());
 	staticRouting->SetDefaultRoute ("10.0.7.5", 1 );
	// do cliente 8 para o roteador
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (csma2Nodes.Get (1)->GetObject<Ipv4> ()->GetRoutingProtocol ());
 	staticRouting->SetDefaultRoute ("10.0.7.5", 1 );
	// do cliente 20 para o roteador
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (csma4Nodes.Get (0)->GetObject<Ipv4> ()->GetRoutingProtocol ());
 	staticRouting->SetDefaultRoute ("10.0.9.8", 1 );
	// do cliente 23 para o roteador
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (csma4Nodes.Get (3)->GetObject<Ipv4> ()->GetRoutingProtocol ());
 	staticRouting->SetDefaultRoute ("10.0.9.8", 1 );

	// do servidor 14 para o roteador
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (csma3Nodes.Get (0)->GetObject<Ipv4> ()->GetRoutingProtocol ());
 	staticRouting->SetDefaultRoute ("10.0.8.5", 1 );
	// do cliente 6 para o roteador
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (csma1Nodes.Get (2)->GetObject<Ipv4> ()->GetRoutingProtocol ());
 	staticRouting->SetDefaultRoute ("10.0.6.4", 1 );
	// do cliente 9 para o roteador
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (csma2Nodes.Get (2)->GetObject<Ipv4> ()->GetRoutingProtocol ());
 	staticRouting->SetDefaultRoute ("10.0.7.5", 1 );
	// do cliente 16 para o roteador
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (csma3Nodes.Get (2)->GetObject<Ipv4> ()->GetRoutingProtocol ());
 	staticRouting->SetDefaultRoute ("10.0.8.5", 1 );
	// do cliente 26 para o roteador
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (csma4Nodes.Get (6)->GetObject<Ipv4> ()->GetRoutingProtocol ());
 	staticRouting->SetDefaultRoute ("10.0.9.8", 1 );


	// o 4 eh um servidor
 	UdpEchoServerHelper echoServer4 (9);
 	ApplicationContainer server4Apps = echoServer4.Install (csma1Nodes.Get (0));
 	server4Apps.Start (Seconds (10.0));
 	server4Apps.Stop (Seconds (500.0));

	// o 7 eh o primeiro a enviar ao servidor 4
 	UdpEchoClientHelper echoClient7 (csma1.GetAddress (0), 9);
 	echoClient7.SetAttribute ("MaxPackets", UintegerValue (1));
 	echoClient7.SetAttribute ("Interval", TimeValue (Seconds (1)));
 	echoClient7.SetAttribute ("PacketSize", UintegerValue (1024));

 	ApplicationContainer client7Apps = echoClient7.Install (csma2Nodes.Get (0));
 	client7Apps.Start (Seconds (50.0));
  	client7Apps.Stop (Seconds (500.0));

	// o 8 eh o segundo cliente a enviar ao servidor 4
 	UdpEchoClientHelper echoClient8 (csma1.GetAddress (0), 9);
 	echoClient8.SetAttribute ("MaxPackets", UintegerValue (1));
 	echoClient8.SetAttribute ("Interval", TimeValue (Seconds (1)));
 	echoClient8.SetAttribute ("PacketSize", UintegerValue (1024));

 	ApplicationContainer client8Apps = echoClient8.Install (csma2Nodes.Get (1));
 	client8Apps.Start (Seconds (100.0));
  	client8Apps.Stop (Seconds (500.0));

	// o 20 eh o terceiro cliente a enviar ao servidor 4
 	UdpEchoClientHelper echoClient20 (csma1.GetAddress (0), 9);
 	echoClient20.SetAttribute ("MaxPackets", UintegerValue (1));
 	echoClient20.SetAttribute ("Interval", TimeValue (Seconds (1)));
 	echoClient20.SetAttribute ("PacketSize", UintegerValue (1024));

 	ApplicationContainer client20Apps = echoClient20.Install (csma4Nodes.Get (0));
 	client20Apps.Start (Seconds (200.0));
  	client20Apps.Stop (Seconds (500.0));

	// o 23 eh o quarto cliente a enviar ao servidor 4
 	UdpEchoClientHelper echoClient23 (csma1.GetAddress (0), 9);
 	echoClient23.SetAttribute ("MaxPackets", UintegerValue (1));
 	echoClient23.SetAttribute ("Interval", TimeValue (Seconds (1)));
 	echoClient23.SetAttribute ("PacketSize", UintegerValue (1024));

 	ApplicationContainer client23Apps = echoClient23.Install (csma4Nodes.Get (3));
 	client23Apps.Start (Seconds (350.0));
  	client23Apps.Stop (Seconds (500.0));


	// o 14 eh um servidor
 	UdpEchoServerHelper echoServer14 (8);
 	ApplicationContainer server14Apps = echoServer14.Install (csma3Nodes.Get (0));
 	server14Apps.Start (Seconds (10.0));
 	server14Apps.Stop (Seconds (700.0));

	// o 6 eh o primeiro a enviar ao servidor 14
 	UdpEchoClientHelper echoClient6 (csma3.GetAddress (0), 8);
 	echoClient6.SetAttribute ("MaxPackets", UintegerValue (1));
 	echoClient6.SetAttribute ("Interval", TimeValue (Seconds (1)));
 	echoClient6.SetAttribute ("PacketSize", UintegerValue (1024));

 	ApplicationContainer client6Apps = echoClient6.Install (csma1Nodes.Get (2));
 	client6Apps.Start (Seconds (80.0));
  	client6Apps.Stop (Seconds (500.0));

	// o 9 eh o primeiro a enviar ao servidor 14
 	UdpEchoClientHelper echoClient9 (csma3.GetAddress (0), 8);
 	echoClient9.SetAttribute ("MaxPackets", UintegerValue (1));
 	echoClient9.SetAttribute ("Interval", TimeValue (Seconds (1)));
 	echoClient9.SetAttribute ("PacketSize", UintegerValue (1024));

 	ApplicationContainer client9Apps = echoClient9.Install (csma2Nodes.Get (2));
 	client9Apps.Start (Seconds (85.0));
  	client9Apps.Stop (Seconds (500.0));

	// o 16 eh o primeiro a enviar ao servidor 14
 	UdpEchoClientHelper echoClient16 (csma3.GetAddress (0), 8);
 	echoClient16.SetAttribute ("MaxPackets", UintegerValue (1));
 	echoClient16.SetAttribute ("Interval", TimeValue (Seconds (1)));
 	echoClient16.SetAttribute ("PacketSize", UintegerValue (1024));

 	ApplicationContainer client16Apps = echoClient16.Install (csma3Nodes.Get (2));
 	client16Apps.Start (Seconds (600.0));
  	client16Apps.Stop (Seconds (700.0));

	// o 26 eh o segundo cliente a enviar ao servidor 14
 	UdpEchoClientHelper echoClient26 (csma3.GetAddress (0), 8);
 	echoClient26.SetAttribute ("MaxPackets", UintegerValue (1));
 	echoClient26.SetAttribute ("Interval", TimeValue (Seconds (1)));
 	echoClient26.SetAttribute ("PacketSize", UintegerValue (1024));

 	ApplicationContainer client26Apps = echoClient26.Install (csma4Nodes.Get (6));
 	client26Apps.Start (Seconds (650.0));
  	client26Apps.Stop (Seconds (700.0));

/*
	AnimationInterface anim ("anim-ospf.xml");
	anim.SetConstantPosition (p2p1Nodes.Get (0), 4.0, 1.0);
	anim.SetConstantPosition (p2p1Nodes.Get (1), 5.0, 1.5);
	anim.SetConstantPosition (p2p1Nodes.Get (2), 6.0, 2.0);

	anim.SetConstantPosition (p2p2Nodes.Get (0), 7.0, 2.5);
	anim.SetConstantPosition (p2p2Nodes.Get (1), 8.0, 3.0);
	anim.SetConstantPosition (p2p2Nodes.Get (2), 9.0, 3.5);

	anim.SetConstantPosition (p2p3Nodes.Get (0), 10.0, 4.0);
	anim.SetConstantPosition (p2p3Nodes.Get (1), 11.0, 4.5);
	anim.SetConstantPosition (p2p3Nodes.Get (2), 12.0, 5.0);

	anim.SetConstantPosition (csma1Nodes.Get (0), 4.0, 1.0);
	anim.SetConstantPosition (csma1Nodes.Get (1), 4.0, 1.0);
	anim.SetConstantPosition (csma1Nodes.Get (2), 4.0, 1.0);

	anim.SetConstantPosition (csma2Nodes.Get (0), 4.0, 1.0);
	anim.SetConstantPosition (csma2Nodes.Get (1), 4.0, 1.0);
	anim.SetConstantPosition (csma2Nodes.Get (2), 4.0, 1.0);

	anim.SetConstantPosition (csma3Nodes.Get (0), 4.0, 1.0);
	anim.SetConstantPosition (csma3Nodes.Get (1), 4.0, 1.0);
	anim.SetConstantPosition (csma3Nodes.Get (2), 4.0, 1.0);
	anim.SetConstantPosition (csma3Nodes.Get (3), 4.0, 1.0);

	anim.SetConstantPosition (csma4Nodes.Get (0), 4.0, 1.0);
	anim.SetConstantPosition (csma4Nodes.Get (1), 4.0, 1.0);
	anim.SetConstantPosition (csma4Nodes.Get (2), 4.0, 1.0);
	anim.SetConstantPosition (csma4Nodes.Get (3), 4.0, 1.0);
	anim.SetConstantPosition (csma4Nodes.Get (4), 4.0, 1.0);
	anim.SetConstantPosition (csma4Nodes.Get (5), 4.0, 1.0);
*/

        Simulator::Stop (Seconds (700.0));
  	Simulator::Run ();
  	Simulator::Destroy ();

  	return 0;
}
