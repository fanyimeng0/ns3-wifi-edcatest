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
#include "ns3/poisson-helper.h"
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


double m_phy1TxBegin = 0;
double m_phy2TxBegin = 0;
double m_phy3TxBegin = 0;
double m_phy4TxBegin = 0;

void
Tx1callback(Ptr<const Packet>packet,double energy)
{
  Ptr<Packet> m_currentPacket;
  WifiMacHeader hdr;
  m_currentPacket = packet->Copy();
  m_currentPacket->RemoveHeader (hdr);
  if(hdr.IsData()){
    m_phy1TxBegin=m_phy1TxBegin+packet->GetSize();
    /*std::cout << "PHY TX :" << m_phyTxBegin << " " << std::endl;
    std::cout << "GetAddr1 :" << hdr.GetAddr1() << " " << std::endl;
    std::cout << "GetAddr2 :" << hdr.GetAddr2() << " " << std::endl;
    std::cout << "type :" << hdr.GetTypeString() << std::endl;
    std::cout << "Time: " << ns3::Simulator::Now()<< std::endl << std::endl;*/
    //std::cout << "ack :" << hdr.IsAck() << std::endl;
    //std::cout << "rts :" << hdr.IsRts() << std::endl;
  }
}
static void
CalculateBE (Ptr<const Packet>packet,const Address &address)
{
    m_phy2TxBegin=m_phy2TxBegin+packet->GetSize();
    
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

void
showresult()
{

  std::cout << "RadioVI: \t" << m_phy1TxBegin/(m_phy1TxBegin+m_phy2TxBegin) <<  std::endl;
  std::cout << "RadioBE: \t" << m_phy2TxBegin/(m_phy1TxBegin+m_phy2TxBegin) <<  std::endl;
  //std::cout << "Radio3: \t" << m_phy3TxBegin/(m_phy1TxBegin+m_phy2TxBegin+m_phy3TxBegin) <<  std::endl;
  std::cout << "Throughput1: \t" << m_phy1TxBegin*8/1000000 <<  std::endl;

  m_phy1TxBegin=0;
  m_phy2TxBegin=0;
  
 
  Time t = Simulator::Now();
  if(t.GetSeconds() <23)
	{
		Simulator::Schedule(Seconds(1), showresult);
	}
}



int main (int argc, char *argv[])
{ 

  double simulationTime = 23; //seconds
  double phymode = 0;
  CommandLine cmd (__FILE__);
  cmd.AddValue ("simulationTime", "Simulation time in seconds", simulationTime);
  cmd.AddValue ("phymode","Phy mode", phymode);
  

  cmd.Parse (argc, argv);
  uint8_t nWifi=4;
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
  WifiMacHelper mac;
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("HtMcs0"));//default
  if (phymode==0){
    wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("VhtMcs0"));
  }
  if (phymode==1){
    wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("VhtMcs1"));
  }
  if (phymode==2){
    wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("VhtMcs2"));
  }
  if (phymode==3){
    wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("VhtMcs3"));
  }
  if (phymode==4){
    wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("VhtMcs4"));
  }
  if (phymode==5){
    wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("VhtMcs5"));
  }
  if (phymode==6){
    wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("VhtMcs6"));
  }
  if (phymode==7){
    wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("VhtMcs7"));
  }
  if (phymode==8){
    wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("VhtMcs8"));
  }
  NetDeviceContainer staDevices,apDevice;
  Ssid ssid;

  //Network A
  ssid = Ssid ("network-A");
  /*"SCS=2"*/
  phy.Set ("Antennas", UintegerValue (2));
  phy.Set ("MaxSupportedTxSpatialStreams", UintegerValue (2));
  phy.Set ("MaxSupportedRxSpatialStreams", UintegerValue (2));
  phy.Set ("ChannelWidth",UintegerValue (20));
  phy.Set("Frequency",UintegerValue (5180));
  phy.Set("ShortPlcpPreambleSupported",BooleanValue (false));
  /*Qossupport for mac*/
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
  uint32_t datarate_VI = 6000000; 
  uint64_t datarate_BE = 5000000;
  uint32_t datarate_BK = 50000000;
  uint32_t portVO = 9;
  uint32_t portVI = 10;
  uint32_t portBE = 11;
  uint32_t portBK = 12;
  Address sinkLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), portVO));//listen packet from all ip
  PacketSinkHelper sinkHelper ("ns3::UdpSocketFactory", sinkLocalAddress);
  ApplicationContainer serverVO = sinkHelper.Install (wifiApNode);
  serverVO.Start (Seconds (0.0));
  serverVO.Stop (Seconds (simulationTime + 1));


  Address sinkLocalAddress2 (InetSocketAddress (Ipv4Address::GetAny (), portVI));
  PacketSinkHelper sinkHelper2 ("ns3::UdpSocketFactory", sinkLocalAddress2);
  ApplicationContainer serverVI= sinkHelper2.Install (wifiApNode);
  serverVI.Start (Seconds (0.0));
  serverVI.Stop (Seconds (simulationTime + 1));
  
  
  Address sinkLocalAddress3 (InetSocketAddress (Ipv4Address::GetAny (), portBE));
  PacketSinkHelper sinkHelper3 ("ns3::UdpSocketFactory", sinkLocalAddress3);
  ApplicationContainer serverBE = sinkHelper3.Install (wifiApNode);
  serverBE.Start (Seconds (0.0));
  serverBE.Stop (Seconds (simulationTime + 1));

  Address sinkLocalAddress4 (InetSocketAddress (Ipv4Address::GetAny (), portBK));
  PacketSinkHelper sinkHelper4 ("ns3::UdpSocketFactory", sinkLocalAddress4);
  ApplicationContainer serverBK = sinkHelper4.Install (wifiApNode);
  serverBK.Start (Seconds (0.0));
  serverBK.Stop (Seconds (simulationTime + 1));


  serverVI.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&CalculateDelayVI));
  serverBE.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&CalculateBE));
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

  OnOffHelper onOffHelperVO ("ns3::UdpSocketFactory", sinkSocketVO);
  onOffHelperVO.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelperVO.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0.02]"));
  onOffHelperVO.SetAttribute ("DataRate", DataRateValue (datarate_VO)); //64kbps voice
  onOffHelperVO.SetAttribute ("PacketSize", UintegerValue (80)); //80 bytes packet
  application1.Start(Seconds (3.0));
  application1.Stop(Seconds (23.0));
  
  /*OnOff application*/
  OnOffHelper onOffHelperVI ("ns3::UdpSocketFactory", sinkSocketVI);
  onOffHelperVI.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelperVI.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onOffHelperVI.SetAttribute ("DataRate", DataRateValue (datarate_VI)); //64kbps voice
  onOffHelperVI.SetAttribute ("PacketSize", UintegerValue (1500)); //1500 bytes packe
  //application2.Start(Seconds (3.0));
  //application2.Stop(Seconds (23.0));
  //application2.Add (onOffHelperVI.Install (wifiStaNodes.Get (0)));

  /*MyOnOff application*/
  PoissonHelper PoissonHelperVI ("ns3::UdpSocketFactory", sinkSocketVI);
  PoissonHelperVI.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  PoissonHelperVI.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  PoissonHelperVI.SetAttribute ("DataRate", DataRateValue (datarate_VI)); 
  PoissonHelperVI.SetAttribute ("PacketSize", UintegerValue (1500)); 
  PoissonHelperVI.SetAttribute ("Interval", StringValue ("ns3::ExponentialRandomVariable[Mean=0.0025|Bound=0.0]")); 
  application2.Add (PoissonHelperVI.Install (wifiStaNodes.Get (0)));

  application2.Start(Seconds (3.0));
  application2.Stop(Seconds (50.0));
  

  OnOffHelper onOffHelperBE ("ns3::UdpSocketFactory", sinkSocketBE);
  onOffHelperBE.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelperBE.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onOffHelperBE.SetAttribute ("DataRate", DataRateValue (datarate_BE));
  onOffHelperBE.SetAttribute ("PacketSize", UintegerValue (1500)); //bytes
  application3.Add (onOffHelperBE.Install (wifiStaNodes.Get (0)));
  application3.Add (onOffHelperBE.Install (wifiStaNodes.Get (1)));
  application3.Start(Seconds (3.0));
  application3.Stop(Seconds (23.0));
    
  OnOffHelper onOffHelperBK ("ns3::UdpSocketFactory", sinkSocketBK);
  onOffHelperBK.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelperBK.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onOffHelperBK.SetAttribute ("DataRate", DataRateValue (datarate_BK));
  onOffHelperBK.SetAttribute ("PacketSize", UintegerValue (1500)); 
  application4.Start(Seconds (0.0));
  application4.Stop(Seconds (23.0));


  Config::ConnectWithoutContext("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxBegin",MakeCallback(&Tx1callback));

  if(1==1){
  Simulator::Schedule(Seconds(1), showresult);}


  Simulator::Stop (Seconds (simulationTime + 1));
  Simulator::Run ();
  Simulator::Destroy();
  
}
