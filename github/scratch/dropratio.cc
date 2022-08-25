/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 Sébastien Deronne
 *
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
 *
 * Author: Sébastien Deronne <sebastien.deronne@gmail.com>
 */
#define PI 3.14159265
#include "ns3/command-line.h"
#include "ns3/string.h"
#include "ns3/pointer.h"
#include "ns3/log.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/mobility-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/udp-client-server-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/wifi-net-device.h"
#include "ns3/qos-txop.h"
#include "ns3/wifi-mac.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/TimestampTag.h"
#include "ns3/flow-monitor-module.h"

// This is an example that illustrates 802.11 QoS for different Access Categories.
// It defines 4 independent Wi-Fi networks (working on different logical channels
// on the same "ns3::YansWifiPhy" channel object).
// Each network contains one access point and one station. Each station continuously
// transmits data packets to its respective AP.
//
// Network topology (numbers in parentheses are channel numbers):
//
//  BSS A (36)        BSS B (40)       BSS C (44)        BSS D (48)
//   *      *          *      *         *      *          *      *
//   |      |          |      |         |      |          |      |
//  AP A   STA A      AP B   STA B     AP C   STA C      AP D   STA D
//
// The configuration is the following on the 4 networks:
// - STA A sends AC_BE traffic to AP A with default AC_BE TXOP value of 0 (1 MSDU);
// - STA B sends AC_BE traffic to AP B with non-default AC_BE TXOP of 3.008 ms;
// - STA C sends AC_VI traffic to AP C with default AC_VI TXOP of 3.008 ms;
// - STA D sends AC_VI traffic to AP D with non-default AC_VI TXOP value of 0 (1 MSDU);
//
// The user can select the distance between the stations and the APs, can enable/disable the RTS/CTS mechanism
// and can choose the payload size and the simulation duration.
// Example: ./waf --run "wifi-80211e-txop --distance=10 --simulationTime=20 --payloadSize=1000"
//
// The output prints the throughput measured for the 4 cases/networks described above. When TXOP is enabled, results show
// increased throughput since the channel is granted for a longer duration. TXOP is enabled by default for AC_VI and AC_VO,
// so that they can use the channel for a longer duration than AC_BE and AC_BK.

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("80211eTxop");

int main (int argc, char *argv[])
{
  double simulationTime = 10; //seconds
  bool enablePcap = 1;

  CommandLine cmd (__FILE__);
  
  cmd.Parse (argc, argv);
  uint8_t nWifi=1;
  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi);
  NodeContainer wifiApNode;
  wifiApNode.Create (1);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy;
  phy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
  phy.SetChannel (channel.Create ());
  
  WifiHelper wifi; //the default standard of 802.11a will be selected by this helper since the program doesn't specify another one
  wifi.SetStandard (WIFI_STANDARD_80211ax_5GHZ);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("HtMcs0"), "ControlMode", StringValue ("HtMcs0"));
  WifiMacHelper mac;

  NetDeviceContainer staDevices,apDevice;
  Ssid ssid;

  //Network A
  ssid = Ssid ("network-A");
  phy.Set ("ChannelNumber", UintegerValue (36));
  mac.SetType ("ns3::StaWifiMac",
               "QosSupported", BooleanValue (true),
               "Ssid", SsidValue (ssid));

  staDevices = wifi.Install (phy, mac, wifiStaNodes);

  mac.SetType ("ns3::ApWifiMac",
               "QosSupported", BooleanValue (true),
               "Ssid", SsidValue (ssid),
               "EnableBeaconJitter", BooleanValue (false));
  apDevice = wifi.Install (phy, mac, wifiApNode.Get (0));

  /* Setting mobility model */
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));
  positionAlloc->Add (Vector (11.25, 0.0, 0.0));
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);
  mobility.Install (wifiStaNodes);


  /* Internet stack */
  InternetStackHelper stack;
  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);

  Ipv4AddressHelper address;
  address.SetBase ("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer staNodeInterfaces,apNodeInterface;
  apNodeInterface = address.Assign (apDevice);
  staNodeInterfaces = address.Assign (staDevices);

  /* Setting applications */
  int32_t datarate_VO = 64000; //64kbps
  uint32_t datarate_VI = 2500000000; 
  uint64_t datarate_BE = 1000000000;
  uint32_t datarate_BK = 1000000000;
  uint32_t portVO = 9;
  uint32_t portVI = 10;
  uint32_t portBE = 11;
  uint32_t portBK = 12;
  Address sinkLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), portVO));//在port上监听来自所有ip来的包
  PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", sinkLocalAddress);
  ApplicationContainer serverVO = sinkHelper.Install (wifiApNode);
  serverVO.Start (Seconds (1.0));
  serverVO.Stop (Seconds (simulationTime + 1));

  Address sinkLocalAddress2 (InetSocketAddress (Ipv4Address::GetAny (), portVI));//在port上监听来自所有ip来的包
  PacketSinkHelper sinkHelper2 ("ns3::TcpSocketFactory", sinkLocalAddress2);
  ApplicationContainer serverVI= sinkHelper2.Install (wifiApNode);
  serverVI.Start (Seconds (1.0));
  serverVI.Stop (Seconds (simulationTime + 1));

  Address sinkLocalAddress3 (InetSocketAddress (Ipv4Address::GetAny (), portBE));//在port上监听来自所有ip来的包
  PacketSinkHelper sinkHelper3 ("ns3::TcpSocketFactory", sinkLocalAddress3);
  ApplicationContainer serverBE = sinkHelper3.Install (wifiApNode);
  serverBE.Start (Seconds (1.0));
  serverBE.Stop (Seconds (simulationTime + 1));

  Address sinkLocalAddress4 (InetSocketAddress (Ipv4Address::GetAny (), portBK));//在port上监听来自所有ip来的包
  PacketSinkHelper sinkHelper4 ("ns3::TcpSocketFactory", sinkLocalAddress4);
  ApplicationContainer serverBK = sinkHelper4.Install (wifiApNode);
  serverBK.Start (Seconds (1.0));
  serverBK.Stop (Seconds (simulationTime + 1));

  ApplicationContainer application1;
  ApplicationContainer application2;
  ApplicationContainer application3;
  ApplicationContainer application4;
  InetSocketAddress sinkSocketVO (apNodeInterface.GetAddress (0), portVO);
  sinkSocketVO.SetTos (0xc0);
  InetSocketAddress sinkSocketVI (apNodeInterface.GetAddress (0), portVI);
  sinkSocketVI.SetTos (0xb8);
  InetSocketAddress sinkSocketBE (apNodeInterface.GetAddress (0), portBE);
  sinkSocketBE.SetTos (0x70);
  InetSocketAddress sinkSocketBK (apNodeInterface.GetAddress (0), portBK);
  sinkSocketBK.SetTos (0x28);

  OnOffHelper onOffHelperVO ("ns3::TcpSocketFactory", sinkSocketVO);
  onOffHelperVO.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelperVO.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0.02]"));
  onOffHelperVO.SetAttribute ("DataRate", DataRateValue (datarate_VO)); //64kbps voice
  onOffHelperVO.SetAttribute ("PacketSize", UintegerValue (80)); //80 bytes packet
  //application1.Add (onOffHelperVO.Install (wifiStaNodes.Get (6)));
  //application1.Add (onOffHelperVO.Install (wifiStaNodes.Get (1)));
  //application1.Add (onOffHelperVO.Install (wifiStaNodes.Get (2)));
  //application1.Add (onOffHelperVO.Install (wifiStaNodes.Get (3)));
  //application1.Start(Seconds (1.0));
  //application1.Stop(Seconds (10.0));
  

  OnOffHelper onOffHelperVI ("ns3::TcpSocketFactory", sinkSocketVI);
  onOffHelperVI.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelperVI.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onOffHelperVI.SetAttribute ("DataRate", DataRateValue (datarate_VI)); //64kbps voice
  onOffHelperVI.SetAttribute ("PacketSize", UintegerValue (1500)); //80 bytes packe
  application2.Add (onOffHelperVI.Install (wifiStaNodes.Get (0)));
  //application2.Add (onOffHelperVO.Install (wifiStaNodes.Get (3)));
  //application2.Add (onOffHelperVO.Install (wifiStaNodes.Get (4)));
  application2.Start(Seconds (1.0));
  application2.Stop(Seconds (10.0));

  OnOffHelper onOffHelperBE ("ns3::TcpSocketFactory", sinkSocketBE);
  onOffHelperBE.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelperBE.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0.02]"));
  onOffHelperBE.SetAttribute ("DataRate", DataRateValue (datarate_BE));
  onOffHelperBE.SetAttribute ("PacketSize", UintegerValue (1500)); //bytes
  //application3.Add (onOffHelperBE.Install (wifiStaNodes.Get (4)));
  //application3.Add (onOffHelperVO.Install (wifiStaNodes.Get (6)));
  //application3.Start(Seconds (1.0));
  //application3.Stop(Seconds (10.0));
  
  
    
  OnOffHelper onOffHelperBK ("ns3::TcpSocketFactory", sinkSocketBK);
  onOffHelperBK.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelperBK.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0.02]"));
  onOffHelperBK.SetAttribute ("DataRate", DataRateValue (datarate_BK));
  onOffHelperBK.SetAttribute ("PacketSize", UintegerValue (1472)); //bytes
  //application4.Add (onOffHelperBK.Install (wifiStaNodes.Get (0)));
  //application4.Add (onOffHelperVO.Install (wifiStaNodes.Get (8)));
  //application4.Start(Seconds (1.0));
  //application4.Stop(Seconds (10.0));

  FlowMonitorHelper flowmon;
	Ptr<FlowMonitor> monitor = flowmon.InstallAll ();
if (enablePcap)
    {
  phy.EnablePcap ("AP", apDevice.Get (0));
  /*phy.EnablePcap ("STA_A", staDevices.Get (0));
  phy.EnablePcap ("STA_B", staDevices.Get (1));
  phy.EnablePcap ("STA_C", staDevices.Get (2));
  phy.EnablePcap ("STA_D", staDevices.Get (3));
  phy.EnablePcap ("STA_E", staDevices.Get (4)); 
  phy.EnablePcap ("STA_F", staDevices.Get (5));
  phy.EnablePcap ("STA_G", staDevices.Get (6));
  phy.EnablePcap ("STA_H", staDevices.Get (7));*/
    }
  

 
  



  Simulator::Stop (Seconds (simulationTime + 1));
  Simulator::Run ();
  uint32_t SentPackets=0;
  uint32_t LostPckets=0;
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();
  
  for(std::map<FlowId, FlowMonitor::FlowStats>::const_iterator set = stats.begin(); set != stats.end(); set++) 
  {
    SentPackets=SentPackets+(set->second.txPackets);
    LostPckets=LostPckets+(set->second.txPackets-set->second.rxPackets);
  }
  std::cout << "Sent: \t" << SentPackets << std::endl;
  std::cout << "Lost: \t" << LostPckets << std::endl;
  Simulator::Destroy();
  
}
