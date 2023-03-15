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
#include "ns3/Mytag.h"
#include "ns3/channel-access-manager.h"
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("controlpolicy");


double m_BE1 = 0;
double m_BE2 = 0;

static void
CalculateDelayVI1 (Ptr<const Packet>p,const Address &address)
{
  TimestampTag timestamp;
  p->FindFirstMatchingByteTag (timestamp);
  Time tx = timestamp.GetTimestamp();
  Time txdelay = Simulator::Now() - tx;
  double delay = txdelay.ToDouble(Time::MS);
  std::cout << "VI1: \t" << delay << std::endl;

}static void
CalculateDelayVI2 (Ptr<const Packet>p,const Address &address)
{
  TimestampTag timestamp;
  p->FindFirstMatchingByteTag (timestamp);
  Time tx = timestamp.GetTimestamp();
  Time txdelay = Simulator::Now() - tx;
  double delay = txdelay.ToDouble(Time::MS);
  std::cout << "VI2: \t" << delay << std::endl;

}
static void
CalculateBE1 (Ptr<const Packet>packet,const Address &address)
{
    m_BE1=m_BE1+packet->GetSize();
   
    
}
static void
CalculateBE2 (Ptr<const Packet>packet,const Address &address)
{
    m_BE2=m_BE2+packet->GetSize();
   
    
}


void 
backoffvalue1(uint32_t m_backoffTrace)
{
  std::cout << "Backoffvalue 1:" << m_backoffTrace  << std::endl;
}

void 
backoffvalue2(uint32_t m_backoffTrace)
{
  std::cout << "Backoffvalue 2:" << m_backoffTrace  << std::endl;
}
void
showresult()
{
  

  std::cout << "Throughput1: \t" << m_BE1*8/1000000 <<  std::endl;
  std::cout << "Throughput2: \t" << m_BE2*8/1000000 <<  std::endl;
 

  //std::cout << "Drop1: \t" << m_drop1*8/1000000 <<  std::endl;
  //std::cout << "Drop2: \t" << m_drop2*8/1000000 <<  std::endl;
  m_BE1=0;
  m_BE2=0;
  Time t = Simulator::Now();
  if(t.GetSeconds() <23)
	{
		Simulator::Schedule(Seconds(1), showresult);
	}
}

void
cwTrace1 (uint32_t oldValue, uint32_t newValue)
{
  std::cout << "Traced 1 " << oldValue << " to " << newValue << std::endl;
}
void
cwTrace2 (uint32_t oldValue, uint32_t newValue)
{
  std::cout << "Traced 2 " << oldValue << " to " << newValue << std::endl;
}





int main (int argc, char *argv[])
{ 

  double simulationTime = 23; //second

  uint32_t CW=7;
  CommandLine cmd (__FILE__);
  cmd.AddValue ("simulationTime", "Simulation time in seconds", simulationTime);
  cmd.AddValue ("CW", "The policy set to improve the performance", CW);
  cmd.Parse (argc, argv);

  uint8_t nWifi=3;
  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi);
  NodeContainer wifiApNode;
  wifiApNode.Create (1);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy;
  phy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
  phy.SetChannel (channel.Create ());
  WifiHelper wifi;
  WifiHelper wifi1; //the default standard of 802.11a will be selected by this helper since the program doesn't specify another one
  WifiHelper wifi2;
  WifiHelper wifi3;


  wifi.SetStandard (WIFI_STANDARD_80211ax_5GHZ);
  wifi1.SetStandard (WIFI_STANDARD_80211ax_5GHZ);
  wifi2.SetStandard (WIFI_STANDARD_80211ax_5GHZ);
  wifi3.SetStandard (WIFI_STANDARD_80211ax_5GHZ);
  
  WifiMacHelper mac;
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("VhtMcs0"));
  wifi1.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("VhtMcs0"));
  wifi2.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("VhtMcs1"));
  wifi3.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("VhtMcs5"));
  NetDeviceContainer staDevice1,staDevice2,staDevice3,apDevice;
  Ssid ssid;

  //Network A
  ssid = Ssid ("network-A");
  /*"SCS=2"*/
  phy.Set ("Antennas", UintegerValue (2));
  phy.Set ("MaxSupportedTxSpatialStreams", UintegerValue (2));
  phy.Set ("MaxSupportedRxSpatialStreams", UintegerValue (2));
  phy.Set ("ChannelWidth",UintegerValue (20));
  phy.Set("Frequency",UintegerValue (5180));
  /*Qossupport for mac*/
  mac.SetType ("ns3::StaWifiMac",
               "QosSupported", BooleanValue (true),
               "Ssid", SsidValue (ssid));

  staDevice1 = wifi1.Install (phy, mac, wifiStaNodes.Get(0));
  staDevice2 = wifi2.Install (phy, mac, wifiStaNodes.Get(1));
  staDevice3 = wifi3.Install (phy, mac, wifiStaNodes.Get(2));

  
  mac.SetType ("ns3::ApWifiMac",
               "QosSupported", BooleanValue (true),
               "Ssid", SsidValue (ssid));
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
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);
  mobility.Install (wifiStaNodes);
  /* Internet stack */
  InternetStackHelper stack;
  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);

  Ipv4AddressHelper address;
  address.SetBase ("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer staNodeInterface1,staNodeInterface2,staNodeInterface3,apNodeInterface;
  apNodeInterface = address.Assign (apDevice);
  staNodeInterface1 = address.Assign (staDevice1);
  staNodeInterface2 = address.Assign (staDevice2);
  staNodeInterface3 = address.Assign (staDevice3);
  

  /* Setting applications */
  int32_t datarate_VO = 64000; //64kbps
  uint32_t datarate_VI = 50000000; 
  uint64_t datarate_BE = 4800000;
  uint32_t datarate_BK = 20000000;
  uint32_t portVO = 9;
  uint32_t portVI = 10;
  uint32_t portBE = 11;
  uint32_t portBK = 12;
  uint32_t portVI1 = 13;
  uint32_t portBE1 = 14;
  Address sinkLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), portVO));//listen packet from all ip
  PacketSinkHelper sinkHelper ("ns3::UdpSocketFactory", sinkLocalAddress);
  ApplicationContainer serverVO = sinkHelper.Install (wifiApNode);
  serverVO.Start (Seconds (0.0));
  serverVO.Stop (Seconds (simulationTime + 1));


  Address sinkLocalAddress2 (InetSocketAddress (Ipv4Address::GetAny (), portVI));
  PacketSinkHelper sinkHelper2 ("ns3::UdpSocketFactory", sinkLocalAddress2);
  ApplicationContainer serverVI1= sinkHelper2.Install (wifiApNode);
  serverVI1.Start (Seconds (0.0));
  serverVI1.Stop (Seconds (simulationTime + 1));

  Address sinkLocalAddress21 (InetSocketAddress (Ipv4Address::GetAny (), portVI1));
  PacketSinkHelper sinkHelper21 ("ns3::UdpSocketFactory", sinkLocalAddress21);
  ApplicationContainer serverVI2= sinkHelper21.Install (wifiApNode);
  serverVI2.Start (Seconds (0.0));
  serverVI2.Stop (Seconds (simulationTime + 1));

  
  Address sinkLocalAddress3 (InetSocketAddress (Ipv4Address::GetAny (), portBE));
  PacketSinkHelper sinkHelper3 ("ns3::UdpSocketFactory", sinkLocalAddress3);
  ApplicationContainer serverBE1 = sinkHelper3.Install (wifiApNode);
  serverBE1.Start (Seconds (0.0));
  serverBE1.Stop (Seconds (simulationTime + 1));

  Address sinkLocalAddress31 (InetSocketAddress (Ipv4Address::GetAny (), portBE1));
  PacketSinkHelper sinkHelper31 ("ns3::UdpSocketFactory", sinkLocalAddress31);
  ApplicationContainer serverBE2 = sinkHelper31.Install (wifiApNode);
  serverBE2.Start (Seconds (0.0));
  serverBE2.Stop (Seconds (simulationTime + 1));

  Address sinkLocalAddress4 (InetSocketAddress (Ipv4Address::GetAny (), portBK));
  PacketSinkHelper sinkHelper4 ("ns3::UdpSocketFactory", sinkLocalAddress4);
  ApplicationContainer serverBK = sinkHelper4.Install (wifiApNode);
  serverBK.Start (Seconds (0.0));
  serverBK.Stop (Seconds (simulationTime + 1));


  serverVI1.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&CalculateDelayVI1));
  serverVI2.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&CalculateDelayVI2));
  serverBE1.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&CalculateBE1));
  serverBE2.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&CalculateBE2));
  

  

  ApplicationContainer application1;
  ApplicationContainer application2,application21;
  ApplicationContainer application3,application31;
  ApplicationContainer application4;
  InetSocketAddress sinkSocketVO (apNodeInterface.GetAddress (0), portVO);
  sinkSocketVO.SetTos (0xc0);
  InetSocketAddress sinkSocketVI (apNodeInterface.GetAddress (0), portVI);
  sinkSocketVI.SetTos (0xb8);
  InetSocketAddress sinkSocketBE (apNodeInterface.GetAddress (0), portBE);
  sinkSocketBE.SetTos (0x70);
  InetSocketAddress sinkSocketVI1 (apNodeInterface.GetAddress (0), portVI1);
  sinkSocketVI1.SetTos (0xb8);
  InetSocketAddress sinkSocketBE1 (apNodeInterface.GetAddress (0), portBE1);
  sinkSocketBE1.SetTos (0x70);
  InetSocketAddress sinkSocketBK (apNodeInterface.GetAddress (0), portBK);
  sinkSocketBK.SetTos (0x28);

  OnOffHelper onOffHelperVO ("ns3::UdpSocketFactory", sinkSocketVO);
  onOffHelperVO.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelperVO.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0.02]"));
  onOffHelperVO.SetAttribute ("DataRate", DataRateValue (datarate_VO)); //64kbps voice
  onOffHelperVO.SetAttribute ("PacketSize", UintegerValue (80)); //80 bytes packet
  application1.Start(Seconds (3.0));
  application1.Stop(Seconds (23.0));
  
  /*OnOff application*/

  PoissonHelper PoissonHelperVI1 ("ns3::UdpSocketFactory", sinkSocketVI);
  PoissonHelperVI1.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  PoissonHelperVI1.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  PoissonHelperVI1.SetAttribute ("DataRate", DataRateValue (datarate_VI)); 
  PoissonHelperVI1.SetAttribute ("PacketSize", UintegerValue (1500)); 
  PoissonHelperVI1.SetAttribute ("Interval", StringValue ("ns3::ExponentialRandomVariable[Mean=0.0025|Bound=0.0]")); 
  application2.Add (PoissonHelperVI1.Install (wifiStaNodes.Get (0)));
  //application2.Add (PoissonHelperVI.Install (wifiStaNodes.Get (1)));

  PoissonHelper PoissonHelperVI2 ("ns3::UdpSocketFactory", sinkSocketVI1);
  PoissonHelperVI2.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  PoissonHelperVI2.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  PoissonHelperVI2.SetAttribute ("DataRate", DataRateValue (datarate_VI)); 
  PoissonHelperVI2.SetAttribute ("PacketSize", UintegerValue (1500)); 
  PoissonHelperVI2.SetAttribute ("Interval", StringValue ("ns3::ExponentialRandomVariable[Mean=0.0025|Bound=0.0]")); 
  application2.Add (PoissonHelperVI2.Install (wifiStaNodes.Get (1)));
  //application2.Add (PoissonHelperVI.Install (wifiStaNodes.Get (1)));

  application2.Start(Seconds (3.0));
  application2.Stop(Seconds (50.0));


  OnOffHelper onOffHelperBE ("ns3::UdpSocketFactory", sinkSocketBE);
  onOffHelperBE.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelperBE.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onOffHelperBE.SetAttribute ("DataRate", DataRateValue (datarate_BE));
  onOffHelperBE.SetAttribute ("PacketSize", UintegerValue (1500)); //bytes
  application3.Start(Seconds (3.0));
  application3.Stop(Seconds (23.0));
  application3.Add (onOffHelperBE.Install (wifiStaNodes.Get (0)));

  OnOffHelper onOffHelperBE1 ("ns3::UdpSocketFactory", sinkSocketBE1);
  onOffHelperBE1.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelperBE1.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onOffHelperBE1.SetAttribute ("DataRate", DataRateValue (datarate_BE));
  onOffHelperBE1.SetAttribute ("PacketSize", UintegerValue (1500)); //bytes
  application31.Start(Seconds (3.0));
  application31.Stop(Seconds (23.0));
  application31.Add (onOffHelperBE1.Install (wifiStaNodes.Get (1)));
    
  OnOffHelper onOffHelperBK ("ns3::UdpSocketFactory", sinkSocketBK);
  onOffHelperBK.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelperBK.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onOffHelperBK.SetAttribute ("DataRate", DataRateValue (datarate_BK));
  onOffHelperBK.SetAttribute ("PacketSize", UintegerValue (1500)); 
  application4.Start(Seconds (0.0));
  application4.Stop(Seconds (23.0));
 
  //Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/VI_Txop/TxopLimit", TimeValue(MicroSeconds (0)));
  //Config::Set ("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/BE_Txop/MinCw",  UintegerValue (400));
  //Config::Set ("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/BE_Txop/MaxCw",  UintegerValue (400));
  
  //Config::Set ("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/VI_Txop/MaxCw",  UintegerValue (7));
  //Config::Set ("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/VI_Txop/MinCw",  UintegerValue (7));

  //Config::Set ("/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/BE_Txop/MinCw",  UintegerValue (400));
  //Config::Set ("/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/BE_Txop/MaxCw",  UintegerValue (400));
  
  //Config::Set ("/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/VI_Txop/MaxCw",  UintegerValue (13));
  //Config::Set ("/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/VI_Txop/MinCw",  UintegerValue (13));
  if(1==1){
  Simulator::Schedule(Seconds(1), showresult);}
 if(1==0){
  Config::ConnectWithoutContext("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/VI_Txop/BackoffTrace", MakeCallback(&backoffvalue1));
  Config::ConnectWithoutContext("/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/VI_Txop/BackoffTrace", MakeCallback(&backoffvalue2));
  }
  if(1==0){
  Config::ConnectWithoutContext("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/VI_Txop/CwTrace", MakeCallback(&cwTrace1));
  Config::ConnectWithoutContext("/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/VI_Txop/CwTrace", MakeCallback(&cwTrace2));}
  phy.EnablePcap ("AP_A", apDevice.Get (0));




  Simulator::Stop (Seconds (simulationTime + 1));
  Simulator::Run ();
  Simulator::Destroy();
  
}
