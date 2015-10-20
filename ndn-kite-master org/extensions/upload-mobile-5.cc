/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn>
 */


#include "upload-mobile-5.h"
#include <sstream>



NS_LOG_COMPONENT_DEFINE ("MobileApp5");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED (MobileApp5);


MobileApp5::MobileApp5 ()
{
  m_face = 0;
}

TypeId
MobileApp5::GetTypeId ()
{
  static TypeId tid = TypeId ("MobileApp5")
    .SetParent<Application> ()
    .AddConstructor<MobileApp5> ()
    .AddAttribute ("ServerPrefix", "The name prefix of server.",
                  StringValue ("/server5/upload/mobile5/file"),
                  MakeStringAccessor (&MobileApp5::m_serverPrefix),
                  MakeStringChecker ())
    .AddAttribute ("MobilePrefix", "The name prefix of mobile.",
                  StringValue ("/mobile5/file"),
                  MakeStringAccessor (&MobileApp5::m_mobilePrefix),
                  MakeStringChecker ())
    .AddAttribute ("RequestPeriod", "The time interval of sending request Interest.",
                   DoubleValue (2.0),
                   MakeDoubleAccessor (&MobileApp5::m_requestPeriod),
                   MakeDoubleChecker<double> (0.0, 120.0))
    ;
  return tid;
}

void
MobileApp5::StartApplication ()
{
  NS_LOG_FUNCTION (this);

  m_face = CreateObject<ApiFace> (GetNode ());
  Ptr<ndn::Name> prefix = Create<ndn::Name> (m_mobilePrefix);
  m_face->SetInterestFilter (prefix, MakeCallback (&MobileApp5::OnInterest, this));
  SendInterest ();
}

void
MobileApp5::StopApplication ()
{
  m_face->Shutdown ();
  m_face = 0;
}

void
MobileApp5::OnInterest (Ptr<const ndn::Name> origName, Ptr<const ndn::Interest> interest)
{
	/*vartika to write log here to track time when mobile gets
	interest from server (consumer) to send data 20150923*/
	Time now = Simulator::Now();
	//std::cout << "4.MobileApp::OnInterest: on receive interest" << now.GetMilliSeconds();
	//20150924
	std::ostringstream oss;
	oss<< "4 MobileApp5::OnInterest::on_receive_interest: " << now.GetMilliSeconds()<<" , ";
	std::string tsLog(oss.str());
	Log::write_ts_to_log_file(tsLog);
	oss.flush();

	NS_LOG_FUNCTION (interest);

	Ptr<ndn::Data> data = Create<ndn::Data> (Create<Packet> (1024));
	data->SetName (Create<ndn::NameComponents> (interest->GetName ()));

	/*vartika to write log here to track time when mobile puts data for
	server (consumer)   20150923*/
	now = Simulator::Now();
	//std::cout << "5.MobileApp::OnInterest:before put data" << now.GetMilliSeconds();
	//20150924
	std::ostringstream oss1;
	oss1<< "5 MobileApp5::OnInterest:before_put_data: " << now.GetMilliSeconds()<<" , ";
	std::string tsLog1(oss1.str());
	Log::write_ts_to_log_file(tsLog1);
	oss1.flush();


	Simulator::ScheduleNow (&ApiFace::Put, m_face, data);
}

void
MobileApp5::OnData (Ptr<const ndn::Interest> origInterest, Ptr<const ndn::Data> data)
{
  NS_LOG_FUNCTION (data->GetNamePtr ()); 
}

void
MobileApp5::OnTimeout (Ptr<const ndn::Interest> interest)
{
  SendInterest ();
  return;
}

void
MobileApp5::SendInterest ()
{
  Ptr<ndn::Interest> interest = Create<ndn::Interest> ();
  UniformVariable rand (0,std::numeric_limits<uint32_t>::max ());
  Ptr<ndn::Name> name = Create<ndn::Name> (m_serverPrefix); 
  interest->SetNonce            (rand.GetValue ());
  interest->SetName             (name);
  interest->SetInterestLifetime (Seconds (m_requestPeriod)); // 1 is just a random value 
  interest->SetPitForwardingFlag (1); // Tracable

  /*vartika to write log here to track time when
  mobile says i have data 20150923*/
  Time now = Simulator::Now();
  //std::cout << "1.MobileApp::SendInterest " << now.GetMilliSeconds();
  //20150924
  //std::string tsLog = boost::lexical_cast<string>(now.GetMilliSeconds());
  std::ostringstream oss;
  oss<< "1 MobileApp5::SendInterest: " << now.GetMilliSeconds()<<" , ";
  std::string tsLog(oss.str());
  Log::write_ts_to_log_file(tsLog);
  oss.flush();

  float perPacketSize = interest->GetPayload()->GetSerializedSize();
  traced_interst_size += interest->GetPayload()->GetSerializedSize();
  //to ask jaebeom traced_interst_size += interest->GetWire()->GetSerializedSize(); // try this also // this does not generate logs...some error maybe..pbb;y needs to be created
  std::ostringstream oss1;
  	oss1<< "Traced Interest (Interest for prefix update (this we need)): " << traced_interest_ctr++<<" , "
  			"Traced Interest size: "<< traced_interst_size << ", "
  			"perPacketSize: "<< perPacketSize;
  	std::string tsLog1(oss1.str());
  	Log::write_to_traced_interest_tracker_node_n(tsLog1, "/home/vartika-kite/kite_log_traced_5.txt");
  	oss.flush();

  Simulator::ScheduleNow (&ApiFace::ExpressInterest, m_face, interest,
                     MakeCallback (&MobileApp5::OnData, this),
                     MakeCallback (&MobileApp5::OnTimeout, this));


}


} // namespace ndn
} // namespace ns3
