/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn>
 */


#include "app-producer.h"
#include <sstream>



NS_LOG_COMPONENT_DEFINE ("ProducerApp");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED (ProducerApp);


ProducerApp::ProducerApp ()
{
  m_face = 0;
}

TypeId
ProducerApp::GetTypeId ()
{
  static TypeId tid = TypeId ("ProducerApp")
    .SetParent<Application> ()
    .AddConstructor<ProducerApp> ()
    .AddAttribute ("Anchor Prefix", "The name prefix of server.",
                  StringValue ("/anchor1"),
                  MakeStringAccessor (&ProducerApp::m_anchorPrefix),
                  MakeStringChecker ())
    .AddAttribute ("MobilePrefix", "The name prefix of mobile.",
                  StringValue ("/producer/file"),
                  MakeStringAccessor (&ProducerApp::m_mobilePrefix),
                  MakeStringChecker ())
    .AddAttribute ("RequestPeriod", "The time interval of sending request Interest.",
                   DoubleValue (1.0),
                   MakeDoubleAccessor (&ProducerApp::m_requestPeriod),
                   MakeDoubleChecker<double> (0.0, 120.0))
    ; // interest life time as 1
  return tid;
}

void
ProducerApp::StartApplication ()
{
  NS_LOG_FUNCTION (this);

  m_face = CreateObject<ApiFace> (GetNode ());
  Ptr<ndn::Name> prefix = Create<ndn::Name> (m_mobilePrefix);
  m_face->SetInterestFilter (prefix, MakeCallback (&ProducerApp::OnInterest, this));
  m_seq=0;
  int credit = 8;
  for (int i=0; i < credit; i++)
  {
	  SendTracedInterestToAnchor ();
  }

  received_tracing_interest_ctr = 0; //20151006
}

void
ProducerApp::StopApplication ()
{
  m_face->Shutdown ();
  m_face = 0;
}

void
ProducerApp::OnInterest (Ptr<const ndn::Name> origName, Ptr<const ndn::Interest> interest)
{
	Time now = Simulator::Now();
	std::cout <<"4 ProducerApp::OnInterest::on_receive_interest: " << now.GetMilliSeconds()<<"\n";
	/*vartika to write log here to track time when mobile gets
	interest from server (consumer) to send data 20150923*/

	//std::cout << "4.MobileApp::OnInterest: on receive interest" << now.GetMilliSeconds();
	//20150924
	std::ostringstream oss;
	oss<< "4 ProducerApp::OnInterest::on_receive_interest: " << now.GetMilliSeconds()<<" , ";
	std::string tsLog(oss.str());
	Log::write_ts_to_log_file(tsLog);
	oss.flush();

	std::ostringstream oss2;
	oss2<< "on interest/ put data" << received_tracing_interest_ctr++<<" , ";
	std::string tsLog2(oss2.str());
	Log::write_to_tracing_interest_tracker_node_n(tsLog2, "/home/vartika-kite/ndn-kite-master/results/res/received_tracing_interest_ctr.txt");
	oss2.flush();

	NS_LOG_FUNCTION (interest);

	Ptr<ndn::Data> data = Create<ndn::Data> (Create<Packet> (1024));
	data->SetName (Create<ndn::NameComponents> (interest->GetName ()));
	std::cout <<"ProducerApp::OnInterest: interest->GetName (): " << interest->GetName ()<<"\n";
	/*vartika to write log here to track time when mobile puts data for
	server (consumer)   20150923*/
	now = Simulator::Now();
	//std::cout << "5.MobileApp::OnInterest:before put data" << now.GetMilliSeconds();
	//20150924
	std::ostringstream oss1;
	oss1<< "5 ProducerApp::OnInterest:before_put_data: " << now.GetMilliSeconds()<<" , ";
	std::string tsLog1(oss1.str());
	Log::write_ts_to_log_file(tsLog1);
	oss1.flush();


	Simulator::ScheduleNow (&ApiFace::Put, m_face, data);
}

void
ProducerApp::OnData (Ptr<const ndn::Interest> origInterest, Ptr<const ndn::Data> data)
{
  NS_LOG_FUNCTION (data->GetNamePtr ()); 
  Time now = Simulator::Now();
  	std::cout <<"4 ProducerApp::OnData::on_receive_interest: " <<"origInterest->GetName(): "<<origInterest->GetName() << "\n";
}

void
ProducerApp::OnTimeout (Ptr<const ndn::Interest> interest)
{
	//return;
  SendTracedInterestToAnchor ();
  return;
}

void
ProducerApp::SendTracedInterestToAnchor ()
{

	std::string seq = "/%0";
	//std::cout << "1 ProducerApp::SendTracedInterestToAnchor: old seq: " << seq<<"\n";
	std::stringstream ss;
	//ss<<((int)m_seq%3)+1;
	ss<<(m_seq+1);
	seq.append(ss.str());


	Ptr<ndn::Interest> interest = Create<ndn::Interest> ();
	UniformVariable rand (0,std::numeric_limits<uint32_t>::max ());
	std::string interestName = m_anchorPrefix + m_mobilePrefix + seq; //maybe remove the mobile prefix from traced interest (only use from forwarding name)
	Ptr<ndn::Name> name = Create<ndn::Name> (interestName);


	interest->SetNonce            (rand.GetValue ());
	interest->SetName             (name);
	interest->SetInterestLifetime (Seconds(4.0)); //Seconds (m_requestPeriod));
	interest->SetPitForwardingFlag (1); // Tracable



	//std::cout << "1 ProducerApp::SendTracedInterestToAnchor: new seq: " << seq<<"\n";

	interest->SetPitForwardingName (m_mobilePrefix+seq);
	m_seq++;

	/*track time when mobile says i have data 20150923*/
	Time now = Simulator::Now();
	std::cout << "1 ProducerApp::SendTracedInterestToAnchor: " << now.GetMilliSeconds()<<"\n";
	std::ostringstream oss;
	oss<< "1 ProducerApp::SendTracedInterestToAnchor: " << now.GetMilliSeconds()<<" , ";
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
	Log::write_to_traced_interest_tracker_node_n(tsLog1, "/home/vartika-kite/ndn-kite-master/results/res/kite_log_traced.txt");
	oss.flush();

	Simulator::ScheduleNow (&ApiFace::ExpressInterest, m_face, interest,
			MakeCallback (&ProducerApp::OnData, this),
			MakeCallback (&ProducerApp::OnTimeout, this));
	/*Simulator::Schedule (Seconds (1.0),&ApiFace::ExpressInterest, m_face, interest,
			MakeCallback (&ProducerApp::OnData, this),
			MakeCallback (&ProducerApp::OnTimeout, this));*/

}


} // namespace ndn
} // namespace ns3
