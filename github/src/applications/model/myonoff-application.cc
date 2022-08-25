/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
//
// Copyright (c) 2006 Georgia Tech Research Corporation
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation;
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Author: George F. Riley<riley@ece.gatech.edu>
//

// ns3 - On/Off Data Source Application class
// George F. Riley, Georgia Tech, Spring 2007
// Adapted from ApplicationOnOff in GTNetS.

#include "ns3/log.h"
#include "ns3/address.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/packet-socket-address.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/random-variable-stream.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "myonoff-application.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/string.h"
#include "ns3/pointer.h"
#include "ns3/boolean.h"
#include "ns3/TimestampTag.h"
#include "ns3/Mytag.h"
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

namespace ns3 {
using namespace std;
NS_LOG_COMPONENT_DEFINE ("MyOnOffApplication");

NS_OBJECT_ENSURE_REGISTERED (MyOnOffApplication);

TypeId
MyOnOffApplication::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MyOnOffApplication")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<MyOnOffApplication> ()
    .AddAttribute ("DataRate", "The data rate in on state.",
                   DataRateValue (DataRate ("500kb/s")),
                   MakeDataRateAccessor (&MyOnOffApplication::m_cbrRate),
                   MakeDataRateChecker ())
    .AddAttribute ("PacketSize", "The size of packets sent in on state",
                   UintegerValue (512),
                   MakeUintegerAccessor (&MyOnOffApplication::m_pktSize),
                   MakeUintegerChecker<uint32_t> (1))
    .AddAttribute ("Remote", "The address of the destination",
                   AddressValue (),
                   MakeAddressAccessor (&MyOnOffApplication::m_peer),
                   MakeAddressChecker ())
    .AddAttribute ("Local",
                   "The Address on which to bind the socket. If not set, it is generated automatically.",
                   AddressValue (),
                   MakeAddressAccessor (&MyOnOffApplication::m_local),
                   MakeAddressChecker ())
    .AddAttribute ("OnTime", "A RandomVariableStream used to pick the duration of the 'On' state.",
                   StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                   MakePointerAccessor (&MyOnOffApplication::m_onTime),
                   MakePointerChecker <RandomVariableStream>())
    .AddAttribute ("OffTime", "A RandomVariableStream used to pick the duration of the 'Off' state.",
                   StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                   MakePointerAccessor (&MyOnOffApplication::m_offTime),
                   MakePointerChecker <RandomVariableStream>())
    .AddAttribute ("Threshold", 
                   "The total number of bytes to send. Once these bytes are sent, "
                   "no packet is sent again, even in on state. The value zero means "
                   "that there is no limit.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&MyOnOffApplication::m_threshold),
                   MakeUintegerChecker<uint64_t> ())
    .AddAttribute ("Protocol", "The type of protocol to use. This should be "
                   "a subclass of ns3::SocketFactory",
                   TypeIdValue (UdpSocketFactory::GetTypeId ()),
                   MakeTypeIdAccessor (&MyOnOffApplication::m_tid),
                   // This should check for SocketFactory as a parent
                   MakeTypeIdChecker ())
    .AddAttribute ("EnableSeqTsSizeHeader",
                   "Enable use of SeqTsSizeHeader for sequence number and timestamp",
                   BooleanValue (false),
                   MakeBooleanAccessor (&MyOnOffApplication::m_enableSeqTsSizeHeader),
                   MakeBooleanChecker ())
    .AddTraceSource ("Tx", "A new packet is created and is sent",
                     MakeTraceSourceAccessor (&MyOnOffApplication::m_txTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("TxWithAddresses", "A new packet is created and is sent",
                     MakeTraceSourceAccessor (&MyOnOffApplication::m_txTraceWithAddresses),
                     "ns3::Packet::TwoAddressTracedCallback")
    .AddTraceSource ("TxWithSeqTsSize", "A new packet is created with SeqTsSizeHeader",
                     MakeTraceSourceAccessor (&MyOnOffApplication::m_txTraceWithSeqTsSize),
                     "ns3::PacketSink::SeqTsSizeCallback")
  ;
  return tid;
}


MyOnOffApplication::MyOnOffApplication ()
  : m_socket (0),
    m_connected (false),
    m_residualBits (0),
    m_lastStartTime (Seconds (0)),
    m_totBytes (0),
    m_unsentPacket (0)
{
  NS_LOG_FUNCTION (this);
}

MyOnOffApplication::~MyOnOffApplication()
{
  NS_LOG_FUNCTION (this);
}

void 
MyOnOffApplication::SetMaxBytes (uint64_t maxBytes)
{
  NS_LOG_FUNCTION (this << maxBytes);
  m_maxBytes = maxBytes;
}
void 
MyOnOffApplication::SetTagvalue(uint64_t  tagvalue)
{
  NS_LOG_FUNCTION (this << tagvalue);
  m_tagvalue = tagvalue;
}


Ptr<Socket>
MyOnOffApplication::GetSocket (void) const
{
  NS_LOG_FUNCTION (this);
  return m_socket;
}

int64_t 
MyOnOffApplication::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  //m_onTime->SetStream (stream);
  //m_offTime->SetStream (stream + 1);
  return 0;
}


void
MyOnOffApplication::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  CancelEvents ();
  m_socket = 0;
  m_unsentPacket = 0;
  vector<double> packetsize;
  vector<double> packettime;
  m_pakvec = packetsize;
  m_timevec = packettime;
  // chain up
  Application::DoDispose ();
}

// Application Methods
void MyOnOffApplication::StartApplication () // Called at time specified by Start
{
  NS_LOG_FUNCTION (this);

  vector<double> packetsize;
  ifstream inFile("/home/mfy/Desktop/packets.csv", ios::in);
	string lineStr;
	while (getline(inFile, lineStr))
	{
		// 打印整行字符串
		//cout << lineStr << endl;
		// 存成二维表结构
		stringstream ss(lineStr);
		string str;
		// 按照逗号分隔
		while (getline(ss, str, ',')){
			packetsize.push_back(stod(str));}
      
	}
  m_pakvec=packetsize;
  vector<double> packettime;
  ifstream inFile2("/home/mfy/Desktop/times.csv", ios::in);
	string lineStr2;
	while (getline(inFile2, lineStr2))
	{
		// 打印整行字符串
		//cout << lineStr << endl;
		// 存成二维表结构
		stringstream ss(lineStr2);
		string str;
		// 按照逗号分隔
		while (getline(ss, str, ',')){
      
			packettime.push_back(3+stod(str)/1000000);}
	}
  m_timevec=packettime;
  
   //std::cout << "startend " <<  std::endl;

  // Create the socket if not already
  if (!m_socket)
    {
      m_socket = Socket::CreateSocket (GetNode (), m_tid);
      int ret = -1;

      if (! m_local.IsInvalid())
        {
          NS_ABORT_MSG_IF ((Inet6SocketAddress::IsMatchingType (m_peer) && InetSocketAddress::IsMatchingType (m_local)) ||
                           (InetSocketAddress::IsMatchingType (m_peer) && Inet6SocketAddress::IsMatchingType (m_local)),
                           "Incompatible peer and local address IP version");
          ret = m_socket->Bind (m_local);
        }
      else
        {
          if (Inet6SocketAddress::IsMatchingType (m_peer))
            {
              ret = m_socket->Bind6 ();
            }
          else if (InetSocketAddress::IsMatchingType (m_peer) ||
                   PacketSocketAddress::IsMatchingType (m_peer))
            {
              ret = m_socket->Bind ();
            }
        }

      if (ret == -1)
        {
          NS_FATAL_ERROR ("Failed to bind socket");
        }

      m_socket->Connect (m_peer);
      m_socket->SetAllowBroadcast (true);
      m_socket->ShutdownRecv ();

      m_socket->SetConnectCallback (
        MakeCallback (&MyOnOffApplication::ConnectionSucceeded, this),
        MakeCallback (&MyOnOffApplication::ConnectionFailed, this));
    }
  m_cbrRateFailSafe = m_cbrRate;

  // Insure no pending event
  CancelEvents ();
  // If we are not yet connected, there is nothing to do here
  // The ConnectionComplete upcall will start timers at that time
  //if (!m_connected) return;
  ScheduleStartEvent ();
}

void MyOnOffApplication::StopApplication () // Called at time specified by Stop
{
  NS_LOG_FUNCTION (this);

  CancelEvents ();
  if(m_socket != 0)
    {
      m_socket->Close ();
    }
  else
    {
      NS_LOG_WARN ("OnOffApplication found null socket to close in StopApplication");
    }
}

void MyOnOffApplication::CancelEvents ()
{
  NS_LOG_FUNCTION (this);

  if (m_sendEvent.IsRunning () && m_cbrRateFailSafe == m_cbrRate )
    { // Cancel the pending send packet event
      // Calculate residual bits since last packet sent
      Time delta (Simulator::Now () - m_lastStartTime);
      int64x64_t bits = delta.To (Time::S) * m_cbrRate.GetBitRate ();
      m_residualBits += bits.GetHigh ();
    }
  m_cbrRateFailSafe = m_cbrRate;
  Simulator::Cancel (m_sendEvent);
  Simulator::Cancel (m_startStopEvent);
  // Canceling events may cause discontinuity in sequence number if the
  // SeqTsSizeHeader is header, and m_unsentPacket is true
  if (m_unsentPacket)
    {
      NS_LOG_DEBUG ("Discarding cached packet upon CancelEvents ()");
    }
  m_unsentPacket = 0;
}

// Event handlers
void MyOnOffApplication::StartSending ()
{
  NS_LOG_FUNCTION (this);
  m_lastStartTime = Simulator::Now ();
  ScheduleNextTx ();  // Schedule the send packet event
  //lScheduleStopEvent ();
}

void MyOnOffApplication::StopSending ()
{
  NS_LOG_FUNCTION (this);
  CancelEvents ();
  m_totBytes = 0;


}

// Private helpers
void MyOnOffApplication::ScheduleNextTx ()
{
  NS_LOG_FUNCTION (this);

  if (m_maxBytes == 0 || m_totBytes < m_maxBytes)
    {
      NS_ABORT_MSG_IF (m_residualBits > m_pktSize * 8, "Calculation to compute next send time will overflow");
      uint32_t bits = m_pktSize * 8 - m_residualBits;
      NS_LOG_LOGIC ("bits = " << bits);
      Time nextTime (Seconds (bits /
                              static_cast<double>(m_cbrRate.GetBitRate ()))); // Time till next packet
      NS_LOG_LOGIC ("nextTime = " << nextTime.As (Time::S));
      
      m_sendEvent = Simulator::Schedule (nextTime,
                                         &MyOnOffApplication::SendPacket, this);
      //std::cout << "nowbyte: " << m_totBytes << "maxbyte"<< m_maxBytes<< std::endl;
      
    
    }
  else
    { // All done, cancel any pending events
      //std::cout << "stop " <<  std::endl;
      StopSending ();
      ScheduleStartEvent ();
    }
}

void MyOnOffApplication::ScheduleStartEvent ()
{  // Schedules the event to start sending data (switch to the "On" state)
  
  NS_LOG_FUNCTION (this);
  Time now = Simulator::Now();
  Time nexttime=Seconds (m_timevec.front());
  while (nexttime<=now)
  {
  vector<double>::iterator k = m_timevec.begin();
  m_timevec.erase(k);//删除第一个元素
  nexttime=Seconds (m_timevec.front());
  vector<double>::iterator j = m_pakvec.begin();
  m_pakvec.erase(j);//删除第一个元素
  }
  Time offInterval=nexttime-now;
  uint64_t maxbytes = int(m_pakvec.front());
  MyOnOffApplication::SetMaxBytes(maxbytes);
  if(m_maxBytes>m_threshold){
    MyOnOffApplication::SetTagvalue(1);
  }
  else{MyOnOffApplication::SetTagvalue(0);}
  
 
  NS_LOG_LOGIC ("start at " << offInterval.As (Time::S));
  
  if (m_timevec.size()==1)
  {
    MyOnOffApplication::StopApplication();
  }
  else{
  //double delay = offInterval.ToDouble(Time::MS);
  //std::cout << "startsend: " << delay <<  std::endl;
  m_startStopEvent = Simulator::Schedule (offInterval, &MyOnOffApplication::StartSending, this);}
}

void MyOnOffApplication::ScheduleStopEvent ()
{  // Schedules the event to stop sending data (switch to "Off" state)
  NS_LOG_FUNCTION (this);

  Time onInterval = Seconds (m_onTime->GetValue ());
  NS_LOG_LOGIC ("stop at " << onInterval.As (Time::S));
  m_startStopEvent = Simulator::Schedule (onInterval, &MyOnOffApplication::StopSending, this);
}


void MyOnOffApplication::SendPacket ()
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT (m_sendEvent.IsExpired ());
  if(m_maxBytes-m_totBytes<1500){
    m_pktSize=m_maxBytes-m_totBytes;

  }

  Ptr<Packet> packet;
  if (m_unsentPacket)
    {
      packet = m_unsentPacket;
    }
  else if (m_enableSeqTsSizeHeader)
    {
      Address from, to;
      m_socket->GetSockName (from);
      m_socket->GetPeerName (to);
      SeqTsSizeHeader header;
      header.SetSeq (m_seq++);
      header.SetSize (m_pktSize);
      NS_ABORT_IF (m_pktSize < header.GetSerializedSize ());
      packet = Create<Packet> (m_pktSize - header.GetSerializedSize ());
      // Trace before adding header, for consistency with PacketSink
      m_txTraceWithSeqTsSize (packet, from, to, header);
      packet->AddHeader (header);
    }
  else
    {
      packet = Create<Packet> (m_pktSize);
           
      TimestampTag timestamp;
      timestamp.SetTimestamp(Simulator::Now());
      packet->AddByteTag(timestamp);
      MyTag tag;
      tag.SetSimpleValue (m_tagvalue);
      packet->AddByteTag(tag);
    }

  int actual = m_socket->Send (packet);
  if ((unsigned) actual == m_pktSize)
    {
      m_txTrace (packet);
      m_totBytes += m_pktSize;
      m_unsentPacket = 0;
      Address localAddress;
      m_socket->GetSockName (localAddress);
      if (InetSocketAddress::IsMatchingType (m_peer))
        {
          NS_LOG_INFO ("At time " << Simulator::Now ().As (Time::S)
                       << " on-off application sent "
                       <<  packet->GetSize () << " bytes to "
                       << InetSocketAddress::ConvertFrom(m_peer).GetIpv4 ()
                       << " port " << InetSocketAddress::ConvertFrom (m_peer).GetPort ()
                       << " total Tx " << m_totBytes << " bytes");
          m_txTraceWithAddresses (packet, localAddress, InetSocketAddress::ConvertFrom (m_peer));
        }
      else if (Inet6SocketAddress::IsMatchingType (m_peer))
        {
          NS_LOG_INFO ("At time " << Simulator::Now ().As (Time::S)
                       << " on-off application sent "
                       <<  packet->GetSize () << " bytes to "
                       << Inet6SocketAddress::ConvertFrom(m_peer).GetIpv6 ()
                       << " port " << Inet6SocketAddress::ConvertFrom (m_peer).GetPort ()
                       << " total Tx " << m_totBytes << " bytes");
          m_txTraceWithAddresses (packet, localAddress, Inet6SocketAddress::ConvertFrom(m_peer));
        }
    }
  else
    {
      NS_LOG_DEBUG ("Unable to send packet; actual " << actual << " size " << m_pktSize << "; caching for later attempt");
      m_unsentPacket = packet;
    }
  m_residualBits = 0;
  m_lastStartTime = Simulator::Now ();
  ScheduleNextTx ();

  //std::cout << "sendpacket " <<  std::endl;
}


void MyOnOffApplication::ConnectionSucceeded (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  m_connected = true;
}

void MyOnOffApplication::ConnectionFailed (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  NS_FATAL_ERROR ("Can't connect");
}


} // Namespace ns3
