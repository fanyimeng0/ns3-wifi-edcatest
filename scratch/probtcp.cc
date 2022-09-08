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


double m_phy1TxBegin = 0;
double m_phy2TxBegin = 0;
double m_phy3TxBegin = 0;
double m_drop1=0;
double m_drop2=0;

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

void
Tx2callback(Ptr<const Packet>packet,double energy)
{
  Ptr<Packet> m_currentPacket;
  WifiMacHeader hdr;
  m_currentPacket = packet->Copy();
  m_currentPacket->RemoveHeader (hdr);
  if(hdr.IsData()){
    m_phy2TxBegin=m_phy2TxBegin+packet->GetSize();
    /*std::cout << "PHY TX :" << m_phyTxBegin << " " << std::endl;
    std::cout << "GetAddr1 :" << hdr.GetAddr1() << " " << std::endl;
    std::cout << "GetAddr2 :" << hdr.GetAddr2() << " " << std::endl;
    std::cout << "type :" << hdr.GetTypeString() << std::endl;
    std::cout << "Time: " << ns3::Simulator::Now()<< std::endl << std::endl;*/
    //std::cout << "ack :" << hdr.IsAck() << std::endl;
    //std::cout << "rts :" << hdr.IsRts() << std::endl;
  }
}

void
Tx3callback(Ptr<const Packet>packet,double energy)
{
  Ptr<Packet> m_currentPacket;
  WifiMacHeader hdr;
  m_currentPacket = packet->Copy();
  m_currentPacket->RemoveHeader (hdr);
  if(hdr.IsData()){
    m_phy3TxBegin=m_phy3TxBegin+packet->GetSize();
    /*std::cout << "PHY TX :" << m_phyTxBegin << " " << std::endl;
    std::cout << "GetAddr1 :" << hdr.GetAddr1() << " " << std::endl;
    std::cout << "GetAddr2 :" << hdr.GetAddr2() << " " << std::endl;
    std::cout << "type :" << hdr.GetTypeString() << std::endl;
    std::cout << "Time: " << ns3::Simulator::Now()<< std::endl << std::endl;*/
    //std::cout << "ack :" << hdr.IsAck() << std::endl;
    //std::cout << "rts :" << hdr.IsRts() << std::endl;
  }
}

void
Drop1callback(Ptr<const Packet>packet)
{
  Ptr<Packet> m_currentPacket;
  WifiMacHeader hdr;
  m_currentPacket = packet->Copy();
  m_currentPacket->RemoveHeader (hdr);
  if(hdr.IsData()){
    m_drop1=m_drop1+packet->GetSize();
    /*std::cout << "PHY TX :" << m_phyTxBegin << " " << std::endl;
    std::cout << "GetAddr1 :" << hdr.GetAddr1() << " " << std::endl;
    std::cout << "GetAddr2 :" << hdr.GetAddr2() << " " << std::endl;
    std::cout << "type :" << hdr.GetTypeString() << std::endl;
    std::cout << "Time: " << ns3::Simulator::Now()<< std::endl << std::endl;*/
    //std::cout << "ack :" << hdr.IsAck() << std::endl;
    //std::cout << "rts :" << hdr.IsRts() << std::endl;
  }
}
void
Drop2callback(Ptr<const Packet>packet)
{
  Ptr<Packet> m_currentPacket;
  WifiMacHeader hdr;
  m_currentPacket = packet->Copy();
  m_currentPacket->RemoveHeader (hdr);
  if(hdr.IsData()){
    m_drop2=m_drop2+packet->GetSize();
    /*std::cout << "PHY TX :" << m_phyTxBegin << " " << std::endl;
    std::cout << "GetAddr1 :" << hdr.GetAddr1() << " " << std::endl;
    std::cout << "GetAddr2 :" << hdr.GetAddr2() << " " << std::endl;
    std::cout << "type :" << hdr.GetTypeString() << std::endl;
    std::cout << "Time: " << ns3::Simulator::Now()<< std::endl << std::endl;*/
    //std::cout << "ack :" << hdr.IsAck() << std::endl;
    //std::cout << "rts :" << hdr.IsRts() << std::endl;
  }
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
  if (m_phy1TxBegin+m_phy2TxBegin!=0){
  std::cout << "Radio1: \t" << m_phy1TxBegin/(m_phy1TxBegin+m_phy2TxBegin+m_phy3TxBegin) <<  std::endl;
  std::cout << "Radio2: \t" << m_phy2TxBegin/(m_phy1TxBegin+m_phy2TxBegin+m_phy3TxBegin) <<  std::endl;
  std::cout << "Radio3: \t" << m_phy3TxBegin/(m_phy1TxBegin+m_phy2TxBegin+m_phy3TxBegin) <<  std::endl;
  std::cout << "Throughput1: \t" << m_phy1TxBegin*8/1000000 <<  std::endl;
  std::cout << "Throughput2: \t" << m_phy2TxBegin*8/1000000 <<  std::endl;
  std::cout << "Throughput3: \t" << m_phy3TxBegin*8/1000000 <<  std::endl;

  //std::cout << "Drop1: \t" << m_drop1*8/1000000 <<  std::endl;
  //std::cout << "Drop2: \t" << m_drop2*8/1000000 <<  std::endl;
  
  
  }
  m_phy1TxBegin=0;
  m_phy2TxBegin=0;
  m_phy3TxBegin=0;
  m_drop1=0;
  m_drop2=0;
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

  double simulationTime = 23; //seconds
  int policy = 0;
  int MCS= 8;
  double packetThreshold = 0;
  bool enablePcap = 1;
  int vinum=0;
  uint32_t CW=7;
  CommandLine cmd (__FILE__);
  cmd.AddValue ("simulationTime", "Simulation time in seconds", simulationTime);
  cmd.AddValue ("MCS", "datamdoe", MCS);
  cmd.AddValue ("enablePcap", "Enable/disable pcap file generation", enablePcap);
  cmd.AddValue ("policy", "The policy set to improve the performance", policy);
  cmd.AddValue ("VInum","The number that how many STA will have VI application",vinum);
  cmd.AddValue ("CW", "The policy set to improve the performance", CW);
  cmd.Parse (argc, argv);

  uint8_t nWifi=2;
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
  switch(MCS)
  {
    case 8 :
      wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("VhtMcs8"), "ControlMode", StringValue ("VhtMcs8"));
      packetThreshold = 16/5 * 141 * 1e3/ 8;
      break;
    case 6 :
      wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("VhtMcs6"), "ControlMode", StringValue ("VhtMcs6"));
      packetThreshold = 16/5 * 117 * 1e3/ 8;
      break;
    case 5 :
      wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("VhtMcs5"), "ControlMode", StringValue ("VhtMcs5"));
      packetThreshold = 16/5 * 104 * 1e3/ 8;
      break;
    case 4 :
      wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("VhtMcs4"), "ControlMode", StringValue ("VhtMcs4"));
      packetThreshold = 16/5 * 78 * 1e3/ 8;
      break;
    case 3 :
      wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("VhtMcs3"), "ControlMode", StringValue ("VhtMcs3"));
      packetThreshold = 16/5 * 52 * 1e3/ 8;
      break;
  }
  WifiMacHelper mac;
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("HtMcs3"), "ControlMode", StringValue ("HtMcs3"));
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
  positionAlloc->Add (Vector (10.0, 0.0, 0.0));
  positionAlloc->Add (Vector (10.0, 0.0, 0.0));

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
  uint32_t datarate_VI =  6000000; 
  uint64_t datarate_BE = 5000000;
  uint32_t datarate_BK = 50000000;
  uint32_t portVO = 9;
  uint32_t portVI = 10;
  uint32_t portBE = 11;
  uint32_t portBK = 12;
  Address sinkLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), portVO));//listen packet from all ip
  PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", sinkLocalAddress);
  ApplicationContainer serverVO = sinkHelper.Install (wifiApNode);
  serverVO.Start (Seconds (0.0));
  serverVO.Stop (Seconds (simulationTime + 1));


  Address sinkLocalAddress2 (InetSocketAddress (Ipv4Address::GetAny (), portVI));
  PacketSinkHelper sinkHelper2 ("ns3::TcpSocketFactory", sinkLocalAddress2);
  ApplicationContainer serverVI= sinkHelper2.Install (wifiApNode);
  serverVI.Start (Seconds (0.0));
  serverVI.Stop (Seconds (simulationTime + 1));
  
  
  Address sinkLocalAddress3 (InetSocketAddress (Ipv4Address::GetAny (), portBE));
  PacketSinkHelper sinkHelper3 ("ns3::TcpSocketFactory", sinkLocalAddress3);
  ApplicationContainer serverBE = sinkHelper3.Install (wifiApNode);
  serverBE.Start (Seconds (0.0));
  serverBE.Stop (Seconds (simulationTime + 1));

  Address sinkLocalAddress4 (InetSocketAddress (Ipv4Address::GetAny (), portBK));
  PacketSinkHelper sinkHelper4 ("ns3::TcpSocketFactory", sinkLocalAddress4);
  ApplicationContainer serverBK = sinkHelper4.Install (wifiApNode);
  serverBK.Start (Seconds (0.0));
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
  application1.Start(Seconds (3.0));
  application1.Stop(Seconds (23.0));
  
  /*OnOff application*/
  OnOffHelper onOffHelperVI ("ns3::TcpSocketFactory", sinkSocketVI);
  onOffHelperVI.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelperVI.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onOffHelperVI.SetAttribute ("DataRate", DataRateValue (datarate_VI)); //64kbps voice
  onOffHelperVI.SetAttribute ("PacketSize", UintegerValue (1500)); //1500 bytes packe
  MyOnOffHelper myonOffHelperVI ("ns3::TcpSocketFactory", sinkSocketVI);
  myonOffHelperVI.SetAttribute ("DataRate", DataRateValue (datarate_VI)); 
  myonOffHelperVI.SetAttribute ("PacketSize", UintegerValue (1500)); 
  myonOffHelperVI.SetAttribute("Threshold",UintegerValue(packetThreshold ));

  application2.Start(Seconds (3.0));
  application2.Stop(Seconds (23.0));

  OnOffHelper onOffHelperBE ("ns3::TcpSocketFactory", sinkSocketBE);
  onOffHelperBE.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelperBE.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onOffHelperBE.SetAttribute ("DataRate", DataRateValue (datarate_BE));
  onOffHelperBE.SetAttribute ("PacketSize", UintegerValue (1500)); //bytes
  application3.Start(Seconds (3.0));
  application3.Stop(Seconds (23.0));
    
  OnOffHelper onOffHelperBK ("ns3::TcpSocketFactory", sinkSocketBK);
  onOffHelperBK.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelperBK.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onOffHelperBK.SetAttribute ("DataRate", DataRateValue (datarate_BK));
  onOffHelperBK.SetAttribute ("PacketSize", UintegerValue (1472)); 
  for (uint8_t index = 0; index < 2; ++index){
  application4.Add (onOffHelperBK.Install (wifiStaNodes.Get (index)));
   }
  
  application4.Start(Seconds (0.0));
  application4.Stop(Seconds (23.0));

 
  Config::Set ("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/BK_Txop/MinCw",  UintegerValue (15));
  Config::Set ("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/BK_Txop/MaxCw",  UintegerValue (1023));

  
  Config::Set ("/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/BK_Txop/MinCw",  UintegerValue (CW));
  Config::Set ("/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/BK_Txop/MaxCw",  UintegerValue (1023));

  


  
  Config::ConnectWithoutContext("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxBegin",MakeCallback(&Tx1callback));
  Config::ConnectWithoutContext("/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxBegin",MakeCallback(&Tx2callback));
  Config::ConnectWithoutContext("/NodeList/2/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxBegin",MakeCallback(&Tx3callback));

  Config::ConnectWithoutContext("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxDrop",MakeCallback(&Drop1callback));
  Config::ConnectWithoutContext("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxDrop",MakeCallback(&Drop2callback));
  if(1==0){
  Config::ConnectWithoutContext("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/BK_Txop/BackoffTrace", MakeCallback(&backoffvalue1));
  Config::ConnectWithoutContext("/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/BK_Txop/BackoffTrace", MakeCallback(&backoffvalue2));
  }
  if(1==0){
  Config::ConnectWithoutContext("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/BK_Txop/CwTrace", MakeCallback(&cwTrace1));
  Config::ConnectWithoutContext("/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/BK_Txop/CwTrace", MakeCallback(&cwTrace2));}

  if(1==1){
  Simulator::Schedule(Seconds(1), showresult);}

  


  //LogComponentEnable("ChannelAccessManager", LOG_LEVEL_INFO); 

  /*Ptr<NetDevice> dev = wifiStaNodes.Get (0)->GetDevice (0);
  Ptr<WifiNetDevice> wifi_dev = DynamicCast<WifiNetDevice> (dev);
  Ptr<WifiMac> wifi_mac = wifi_dev->GetMac ();
  PointerValue ptr;
  Ptr<QosTxop> edca;

  dev = wifiStaNodes.Get (0)->GetDevice (0);
  wifi_dev = DynamicCast<WifiNetDevice> (dev);
  wifi_mac = wifi_dev->GetMac ();
  wifi_mac->GetAttribute ("BK_Txop", ptr);
  edca = ptr.Get<QosTxop> ();
  edca->SetMinCw(511);
  edca->SetMaxCw(1023);
  uint32_t cwmax=edca->GetMaxCw();
  uint32_t cwmin=edca->GetMinCw();
  std::cout << "mincw" <<cwmin<<"maxcw"<<cwmax<< std::endl;*/

  //phy.EnablePcap ("TCP", apDevice.Get (0));



  Simulator::Stop (Seconds (simulationTime + 1));
  Simulator::Run ();
  Simulator::Destroy();
  
}
