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

#include "ns3/netanim-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

// Default Network Topology
//
// c     10.1.1.0    r    r    s    r     10.1.3.0      c
// n0 -------------- n1   n2   n3   n4 ---------------- n5
//    point-to-point  |    |    |    |  point-to-point
//                    ================
//                      LAN 10.1.2.0


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Teste2");

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nCsma = 3;

  CommandLine cmd;
  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);

  cmd.Parse (argc,argv);

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  nCsma = nCsma == 0 ? 1 : nCsma;

  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  NodeContainer csmaNodes;
  csmaNodes.Add (p2pNodes.Get (1));
  csmaNodes.Create (nCsma);

  NodeContainer rede2;
  rede2.Add (csmaNodes.Get (nCsma));
  rede2.Create (1);

  NodeContainer roteadores;
  roteadores.Add (csmaNodes.Get (0));
  roteadores.Add (csmaNodes.Get (1));
  roteadores.Add (csmaNodes.Get (3));

  NodeContainer aplicacao;
  aplicacao.Add (csmaNodes.Get (2));
  aplicacao.Add (p2pNodes.Get (0));
  aplicacao.Add (rede2.Get (1));

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);

  NetDeviceContainer rede2Devices;
  rede2Devices = pointToPoint.Install (rede2);

  InternetStackHelper stack;

  //protocolo de roteamento link state (OSPF)
  //Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  //stack.Install (roteadores);  

  //protocolo de roteamento de distance vector (RIP)
  RipHelper ripRouting;
  Ipv4ListRoutingHelper RHlist;
  RHlist.Add (ripRouting, 1);

  InternetStackHelper stack1;
  stack1.SetRoutingHelper (RHlist);
  stack1.Install (roteadores);
  //fim dos protocolos de roteamento
 

  stack.Install (aplicacao);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer rede2Interfaces;
  rede2Interfaces = address.Assign (rede2Devices);

  Ptr<Ipv4StaticRouting> staticRouting;
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (aplicacao.Get (0)->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.1.2.1", 1 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (aplicacao.Get (2)->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.1.2.4", 1 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (aplicacao.Get (1)->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.1.2.2", 1 );

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (2));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (2), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (p2pNodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  UdpEchoClientHelper client2 (csmaInterfaces.GetAddress (2), 9);
  client2.SetAttribute ("MaxPackets", UintegerValue (2));
  client2.SetAttribute ("Interval", TimeValue (Seconds (2.0)));
  client2.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer client2Apps = client2.Install (rede2.Get (1));
  client2Apps.Start (Seconds (2.5));
  client2Apps.Stop (Seconds (10.0));


  pointToPoint.EnablePcapAll ("second");
  csma.EnablePcap ("second", csmaDevices.Get (1), true);

  AnimationInterface anim ("teste2.xml");
  anim.SetConstantPosition (p2pNodes.Get(0), 1.0, 1.0);
  anim.SetConstantPosition (p2pNodes.Get(1), 1.0, 2.0);
  anim.SetConstantPosition (csmaNodes.Get(1), 2.0, 3.0);
  anim.SetConstantPosition (csmaNodes.Get(2), 2.0, 4.0);
  anim.SetConstantPosition (csmaNodes.Get(3), 2.0, 5.0);
  anim.SetConstantPosition (rede2.Get(1), 1.0, 6.0);

  Simulator::Stop (Seconds (500.0));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
