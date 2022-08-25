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
#include "ns3/myon-off-helper.h"
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
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("controlpolicy");


uint32_t CWmin0=2;
uint32_t CWmax0=2;

uint32_t CWmin1=7;
uint32_t CWmax1=15;


/*static void
CalculateDelayVO (Ptr<const Packet>p,const Address &address)
{
  TimestampTag timestamp;
  p->FindFirstMatchingByteTag (timestamp);
  Time tx = timestamp.GetTimestamp();
  Time txdelay = Simulator::Now() - tx;
  double delay = txdelay.ToDouble(Time::MS);
  std::cout << "VOdelay: \t" << delay << std::endl;
}

static void
CalculateDelayVI (Ptr<const Packet>p,const Address &address)
{
  TimestampTag timestamp;
  p->FindFirstMatchingByteTag (timestamp);
  Time tx = timestamp.GetTimestamp();
  Time txdelay = Simulator::Now() - tx;
  double delay = txdelay.ToDouble(Time::MS);
  std::cout << "VIdelay: \t" << delay << std::endl;
}

static void
CalculateDelayBE (Ptr<const Packet>p,const Address &address)
{
  TimestampTag timestamp;
  p->FindFirstMatchingByteTag (timestamp);
  Time tx = timestamp.GetTimestamp();
  Time txdelay = Simulator::Now() - tx;
  double delay = txdelay.ToDouble(Time::MS);
  std::cout << "BEdelay: \t" << delay << std::endl;
  
}

static void
CalculateDelayBK (Ptr<const Packet>p,const Address &address)
{
 
  TimestampTag timestamp;
  p->FindFirstMatchingByteTag (timestamp);
  Time tx = timestamp.GetTimestamp();
  Time txdelay = Simulator::Now() - tx;
  double delay = txdelay.ToDouble(Time::MS);
  std::cout << "BKdelay: \t" << delay << std::endl;

}*/


/*void MacTxCallback(std::string context,Ptr<const Packet> packet){
  TimestampTag timestamp;
  packet->FindFirstMatchingByteTag (timestamp);
  Time tx = timestamp.GetTimestamp();
  Time txdelay = Simulator::Now() - tx;
  double delay = txdelay.ToDouble(Time::MS);
  if (delay>8){
  std::cout << "CWdecrease: \t" << delay << std::endl;
  
  }
  if (delay<6){
  std::cout << "CWdefault: \t" << delay << std::endl;
  
  }
  //NS_LOG_UNCOND("+"<<Simulator::Now().GetSeconds()<<" "<<packet->GetSize()<<" "<<context);
}*/
/*void MacRxCallback(std::string context,Ptr<const Packet> packet){
  NS_LOG_UNCOND("+"<<Simulator::Now().GetSeconds()<<" "<<packet->GetSize()<<" "<<context);
}*/
/*void MacTxCallback(std::string context,Ptr<const Packet> packet){
  NS_LOG_UNCOND("+"<<Simulator::Now().GetSeconds()<<" "<<packet->GetSize()<<" "<<context);
}*/
/*static void
TxCallback(Ptr<const Packet>p)
{
  std::cout << Simulator::Now().GetSeconds()<<" "<<p->GetSize() << std::endl;
}*/

int main (int argc, char *argv[])
{ 

  double simulationTime = 40; //seconds
  bool enablePcap = 1;
  CommandLine cmd (__FILE__);
  //cmd.AddValue ("simulationTime", "Simulation time in seconds", simulationTime);
  //cmd.AddValue ("distance", "Distance in meters between the station and the access point", distance);
  //cmd.AddValue ("enablePcap", "Enable/disable pcap file generation", enablePcap);
  //cmd.AddValue ("verifyResults", "Enable/disable results verification at the end of the simulation", verifyResults);
  cmd.Parse (argc, argv);

  uint8_t nWifi=8;
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
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("VhtMcs8"), "ControlMode", StringValue ("VhtMcs8"));
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
  float rho = 10;
  for (uint32_t i = 0; i < nWifi; i++)
  {
    double theta = i * 2 * PI / nWifi;
    positionAlloc->Add (Vector (rho * cos(theta), rho * sin(theta), 0.0));
  }
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
  serverVO.Start (Seconds (0.0));
  serverVO.Stop (Seconds (simulationTime + 1));

  Address sinkLocalAddress2 (InetSocketAddress (Ipv4Address::GetAny (), portVI));//在port上监听来自所有ip来的包
  PacketSinkHelper sinkHelper2 ("ns3::TcpSocketFactory", sinkLocalAddress2);
  ApplicationContainer serverVI= sinkHelper2.Install (wifiApNode);
  serverVI.Start (Seconds (0.0));
  serverVI.Stop (Seconds (simulationTime + 1));

  Address sinkLocalAddress3 (InetSocketAddress (Ipv4Address::GetAny (), portBE));//在port上监听来自所有ip来的包
  PacketSinkHelper sinkHelper3 ("ns3::TcpSocketFactory", sinkLocalAddress3);
  ApplicationContainer serverBE = sinkHelper3.Install (wifiApNode);
  serverBE.Start (Seconds (0.0));
  serverBE.Stop (Seconds (simulationTime + 1));

  Address sinkLocalAddress4 (InetSocketAddress (Ipv4Address::GetAny (), portBK));//在port上监听来自所有ip来的包
  PacketSinkHelper sinkHelper4 ("ns3::TcpSocketFactory", sinkLocalAddress4);
  ApplicationContainer serverBK = sinkHelper4.Install (wifiApNode);
  serverBK.Start (Seconds (0.0));
  serverBK.Stop (Seconds (simulationTime + 1));
   
  //serverVO.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&CalculateDelayVO));
  //serverVI.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&CalculateDelayVI));

  //serverBE.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&CalculateDelayBE));
  //serverBK.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&CalculateDelayBK));

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
  application1.Start(Seconds (0.0));
  application1.Stop(Seconds (40.0));
  
  OnOffHelper onOffHelperVI ("ns3::TcpSocketFactory", sinkSocketVI);
  onOffHelperVI.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelperVI.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0.5]"));
  onOffHelperVI.SetAttribute ("DataRate", DataRateValue (datarate_VI)); //64kbps voice
  onOffHelperVI.SetAttribute ("PacketSize", UintegerValue (1500)); //1500 bytes packe
  //application2.Add (onOffHelperVI.Install (wifiStaNodes.Get (5)));
  //application2.Add (onOffHelperVO.Install (wifiStaNodes.Get (3)));
  //application2.Add (onOffHelperVO.Install (wifiStaNodes.Get (4)));
  //application2.Start(Seconds (1.0));
  //application2.Stop(Seconds (10.0));

  MyOnOffHelper myonOffHelperVI ("ns3::TcpSocketFactory", sinkSocketVI);
  //myonOffHelperVI.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  //myonOffHelperVI.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  myonOffHelperVI.SetAttribute ("DataRate", DataRateValue (datarate_VI)); //64kbps voice
  myonOffHelperVI.SetAttribute ("PacketSize", UintegerValue (1500)); //1500 bytes packe
  myonOffHelperVI.SetAttribute("Threshold",UintegerValue(1000));
  application2.Add (myonOffHelperVI.Install (wifiStaNodes.Get (5)));
  //application2.Add (onOffHelperVO.Install (wifiStaNodes.Get (3)));
  //application2.Add (onOffHelperVO.Install (wifiStaNodes.Get (4)));
  application2.Start(Seconds (3.0));
  application2.Stop(Seconds (50.0));
  
  OnOffHelper onOffHelperBE ("ns3::TcpSocketFactory", sinkSocketBE);
  onOffHelperBE.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelperBE.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0.02]"));
  onOffHelperBE.SetAttribute ("DataRate", DataRateValue (datarate_BE));
  onOffHelperBE.SetAttribute ("PacketSize", UintegerValue (1500)); //bytes
  //application3.Add (onOffHelperBE.Install (wifiStaNodes.Get (4)));
  //application3.Add (onOffHelperVO.Install (wifiStaNodes.Get (6)));
  application3.Start(Seconds (0.0));
  application3.Stop(Seconds (40.0));
    
  OnOffHelper onOffHelperBK ("ns3::TcpSocketFactory", sinkSocketBK);
  onOffHelperBK.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelperBK.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0.0001]"));
  onOffHelperBK.SetAttribute ("DataRate", DataRateValue (datarate_BK));
  onOffHelperBK.SetAttribute ("PacketSize", UintegerValue (1472)); //bytes
  //application4.Add (onOffHelperBK.Install (wifiStaNodes.Get (1)));
   //for (uint8_t index = 0; index < 7; ++index){
  //application4.Add (onOffHelperBK.Install (wifiStaNodes.Get (index)));
  // }
  //application4.Add (onOffHelperVO.Install (wifiStaNodes.Get (8)));
  application4.Start(Seconds (0.0));
  application4.Stop(Seconds (40.0));

  //Config::ConnectWithoutContext ("/NodeList/0/DeviceList/0/$ns3::WifiNetDevice/MacTx", MakeCallback (mactx));
  

  //Config::Connect("/NodeList/5/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTx",MakeCallback(&MacTxCallback));
  //Config::Connect("/NodeList/5/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/MacTx",MakeCallback(&MacTxCallback));
  //Config::Connect("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx",MakeCallback(&MacRxCallback));
  //application2.Get(0)->TraceConnectWithoutContext("Tx", MakeCallback(&TxCallback));



if (enablePcap)
    {
  phy.EnablePcap ("AP", apDevice.Get (0));
    }
  Simulator::Stop (Seconds (simulationTime + 1));
  Simulator::Run ();
  Simulator::Destroy();
  
}
