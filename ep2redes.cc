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
// r     10.1.1.0    r    s    c    r     10.1.3.0      r
// n0 -------------- n1   n2   n3   n4 ---------------- n5
//         p2p1       |    |    |    |      p2p2         |l
//                    ================                   |a 10.1.4.0
//                     LAN1 10.1.2.0                   n14n
//                                                       |3
//   r                 r   c       c       r             |
//  n6----------------n7  n8  n9  n10 n11 n12-----------n13
//          p2p5       |   |   |   |   |   |    p2p4     r
//        10.1.7.0     =====================   10.1.5.0
//                      LAN2  10.1.6.0
//


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ep2redes");

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  bool protocoloRip = false;

  CommandLine cmd;
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("protocoloRip", "Choose which routing protocol to use", protocoloRip);

  cmd.Parse (argc,argv);

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  NodeContainer p2p1;
  p2p1.Create (2);

  NodeContainer p2p2;
  p2p2.Create (2);

  NodeContainer lan3;
  lan3.Add (p2p2.Get (1));
  lan3.Create (2);

  NodeContainer p2p4;
  p2p4.Add (lan3.Get (2));
  p2p4.Create (1);

  NodeContainer p2p5;
  p2p5.Create (2);

  NodeContainer lan1;
  lan1.Add (p2p1.Get (1));
  lan1.Create (2);
  lan1.Add (p2p2.Get (0));
  

  NodeContainer lan2;
  lan2.Add (p2p4.Get (1));
  lan2.Create (4);
  lan2.Add (p2p5.Get (0));

  /*
  NodeContainer roteadores;
  roteadores.Add (p2p1.Get (0));
  roteadores.Add (p2p1.Get (1));
  roteadores.Add (p2p2.Get (1));
  roteadores.Add (lan3.Get (1));
  roteadores.Add (p2p4.Get (1));
  roteadores.Add (p2p5.Get (0));
  roteadores.Add (p2p5.Get (1));
  roteadores.Add (lan1.Get (2));
  roteadores.Add (lan2.Get (1));
  roteadores.Add (lan2.Get (3));
  */

  NodeContainer aplicacao;
  aplicacao.Add (lan1.Get (1)); //servidor
  aplicacao.Add (lan1.Get (2));
  aplicacao.Add (lan2.Get (2));
  aplicacao.Add (lan2.Get (4));

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2p1Devices;
  p2p1Devices = pointToPoint.Install (p2p1);

  NetDeviceContainer p2p2Devices;
  p2p2Devices = pointToPoint.Install (p2p2);

  NetDeviceContainer p2p4Devices;
  p2p4Devices = pointToPoint.Install (p2p4);

  NetDeviceContainer p2p5Devices;
  p2p5Devices = pointToPoint.Install (p2p5);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer lan1Devices;
  lan1Devices = csma.Install (lan1);

  NetDeviceContainer lan2Devices;
  lan2Devices = csma.Install (lan2);

  NetDeviceContainer lan3Devices;
  lan3Devices = csma.Install (lan3);

  InternetStackHelper stack;

  //escolhe o protocolo de acordo com o parametro. Default é o OSPF
  if(protocoloRip == true){
    //protocolo de roteamento de distance vector (RIP)
    RipHelper ripRouting;
    Ipv4ListRoutingHelper RHlist;
    RHlist.Add (ripRouting, 0);

    InternetStackHelper stack1;
    stack1.SetRoutingHelper (RHlist);
    stack1.Install (p2p1);
    stack1.Install (p2p2);
    stack1.Install (p2p4);
    stack1.Install (p2p5);
  }else{
    stack.Install (p2p1);
    stack.Install (p2p2);
    stack.Install (p2p4);
    stack.Install (p2p5);
  }

  stack.Install (lan1.Get (1));
  stack.Install (lan1.Get (2));  
  stack.Install (lan2.Get (1));
  stack.Install (lan2.Get (2));
  stack.Install (lan2.Get (3));
  stack.Install (lan2.Get (4));
  stack.Install (lan3.Get (1));

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2p1Interfaces;
  p2p1Interfaces = address.Assign (p2p1Devices);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer lan1Interfaces;
  lan1Interfaces = address.Assign (lan1Devices);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer p2p2Interfaces;
  p2p2Interfaces = address.Assign (p2p2Devices);

  address.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer p2p3Interfaces;
  p2p3Interfaces = address.Assign (lan3Devices);

  address.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer p2p4Interfaces;
  p2p4Interfaces = address.Assign (p2p4Devices);

  address.SetBase ("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer lan2Interfaces;
  lan2Interfaces = address.Assign (lan2Devices);

  address.SetBase ("10.1.7.0", "255.255.255.0");
  Ipv4InterfaceContainer p2p5Interfaces;
  p2p5Interfaces = address.Assign (p2p5Devices);


  if(protocoloRip == true){
    Ptr<Ipv4StaticRouting> staticRouting;
    staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (aplicacao.Get (0)->GetObject<Ipv4> ()->GetRoutingProtocol ());
    staticRouting->SetDefaultRoute ("10.1.2.1", 0 );
    staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (aplicacao.Get (1)->GetObject<Ipv4> ()->GetRoutingProtocol ());
    staticRouting->SetDefaultRoute ("10.1.2.3", 0 );
    staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (aplicacao.Get (2)->GetObject<Ipv4> ()->GetRoutingProtocol ());
    staticRouting->SetDefaultRoute ("10.1.5.2", 0 );
    staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (aplicacao.Get (3)->GetObject<Ipv4> ()->GetRoutingProtocol ());
    staticRouting->SetDefaultRoute ("10.1.5.2", 0 );
  }else{
    //protocolo de roteamento link state (OSPF)
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  }


  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (aplicacao.Get (0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (500.0));

  UdpEchoClientHelper echoClient (lan1Interfaces.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (aplicacao.Get (1));
  clientApps.Start (Seconds (100.0));
  clientApps.Stop (Seconds (500.0));

  UdpEchoClientHelper client2 (lan1Interfaces.GetAddress (1), 9);
  client2.SetAttribute ("MaxPackets", UintegerValue (1));
  client2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  client2.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer client2Apps = client2.Install (aplicacao.Get (2));
  client2Apps.Start (Seconds (101.0));
  client2Apps.Stop (Seconds (500.0));

  UdpEchoClientHelper client3 (lan1Interfaces.GetAddress (1), 9);
  client3.SetAttribute ("MaxPackets", UintegerValue (1));
  client3.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  client3.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer client3Apps = client3.Install (aplicacao.Get (3));
  client3Apps.Start (Seconds (102.0));
  client3Apps.Stop (Seconds (500.0));


//  pointToPoint.EnablePcapAll ("second");
//  csma.EnablePcap ("second", csmaDevices.Get (1), true);

  
  Simulator::Stop (Seconds (500.0));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
