/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn>
 */


#include "upload-server-9.h"
#include <sstream>

NS_LOG_COMPONENT_DEFINE ("ServerApp9");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED (ServerApp9);


ServerApp9::ServerApp9 ()
{
  m_face = 0;
}

TypeId
ServerApp9::GetTypeId ()
{
  static TypeId tid = TypeId ("ServerApp9")
    .SetParent<Application> ()
    .AddConstructor<ServerApp9> ()
    .AddAttribute ("ServerPrefix", "The name prefix of server.",
                  StringValue ("/server9/upload"),
                  MakeStringAccessor (&ServerApp9::m_serverPrefix),
                  MakeStringChecker ())
    .AddAttribute ("RequestPeriod", "The time interval of sending request Interest.",
                   DoubleValue (2.0),
                   MakeDoubleAccessor (&ServerApp9::m_requestPeriod),
                   MakeDoubleChecker<double> (0.0, 120.0))
    ;
  return tid;
}

void
ServerApp9::StartApplication ()
{
  NS_LOG_FUNCTION ("vartika"<<this);
	// NS_LOG_FUNCTION (this);
  m_face = CreateObject<ApiFace> (GetNode ());
  Ptr<ndn::Name> prefix = Create<ndn::Name> (m_serverPrefix);
  m_face->SetInterestFilter (prefix, MakeCallback (&ServerApp9::OnInterest, this));
  m_mobilePrefixSize = 1;
  m_credit = 20;
  m_seq = 0;

  //vartika 20150929
  tracing_interest_ctr = 0;
  tracing_interest_size = 0;
  data_ctr = 0;
}

void
ServerApp9::StopApplication ()
{
  m_face->Shutdown ();
  m_face = 0;
}

void
ServerApp9::OnInterest (Ptr<const ndn::Name> origName, Ptr<const ndn::Interest> interest)
{

	/*vartika to write log here to track time when server (consumer)
	gets interest from mobile producer 20150923*/
	Time now = Simulator::Now();
	//std::cout << "2.ServerApp::OnInterest  " << now.GetMilliSeconds();
	//20150924
	std::ostringstream oss;
	oss<< "2 ServerApp9::OnInterest: " << now.GetMilliSeconds()<<" , ";
	std::string tsLog(oss.str());
	Log::write_ts_to_log_file(tsLog);
	oss.flush();

	NS_LOG_FUNCTION (interest);
	if (m_seq > 0)
	{
		return;
	}
	Ptr<const ndn::Name> name = interest->GetNamePtr ();
	m_uploadName = Create<ndn::Name> (name->toUri ());
	//m_uploadName = Create<ndn::Name> ("/server/upload/mobile/file");
	m_mobilePrefix = name->getPostfix (m_mobilePrefixSize+1, 0).toUri ();
	NS_LOG_DEBUG ("Mobile prefix " << m_mobilePrefix);
	m_seq = 1;
	int i = 0;
	for (; i < m_credit; i++)
	{
		SendInterest (m_seq);
		m_seq++;
	}
}

void
ServerApp9::OnData (Ptr<const ndn::Interest> origInterest, Ptr<const ndn::Data> data)
{

	/*vartika to write log here to track time when server (consumer) gets
	data from mobie producer 20150923*/
	Time now = Simulator::Now();
	//std::cout << "6.ServerApp::OnData  " << now.GetMilliSeconds();
	//20150924
	std::ostringstream oss;
	oss<< "6 ServerApp9::OnData: " << now.GetMilliSeconds() <<" , ";//std::endl;
	std::string tsLog(oss.str());
	Log::write_ts_to_log_file(tsLog);
	oss.flush();

	//20151001
	std::ostringstream oss1;
	oss1<< "Data packets Received: " << data_ctr++;
	std::string tsLog1(oss1.str());
	Log::write_to_on_data_tracker(tsLog1, "/home/vartika-kite/kite_log_data_9.txt");
	oss1.flush();


	NS_LOG_FUNCTION (data->GetNamePtr ());
	SendInterest (m_seq);
	m_seq ++;
}

void
ServerApp9::OnTimeout (Ptr<const ndn::Interest> interest)
{
  Ptr<const ndn::Name> name = interest->GetNamePtr ();
  SendInterest (name->get (-1).toNumber ());
  return;
}

void
ServerApp9::SendInterest (int seq)
{

	NS_LOG_FUNCTION (seq);
	Ptr<ndn::Interest> interest = Create<ndn::Interest> ();
	UniformVariable rand (0,std::numeric_limits<uint32_t>::max ());
	Ptr<ndn::Name> name = Create<ndn::Name> (m_mobilePrefix);
	name->appendNumber (seq);

	interest->SetNonce            (rand.GetValue ());
	interest->SetName             (name);
	interest->SetInterestLifetime (Seconds (m_requestPeriod)); // 1 is just a random value
	interest->SetPitForwardingFlag (2); // TraceOnly
	interest->SetPitForwardingName (m_uploadName);

	/*vartika to write log here to track time when server (consumer)
	sends interest to mobile producer to fetch data 20150923*/
	Time now = Simulator::Now();
	//std::cout << "3.ServerApp::SendInterest " << now.GetMilliSeconds();
	//20150924
	std::ostringstream oss;
	oss<< "3 ServerApp::SendInterest: " << now.GetMilliSeconds()<<" , ";
	std::string tsLog(oss.str());
	Log::write_ts_to_log_file(tsLog);
	oss.flush();

	float perPacketSize = interest->GetPayload()->GetSerializedSize();
	tracing_interest_size += interest->GetPayload()->GetSerializedSize();
	std::ostringstream oss1;
	oss1<< "Tracing Interest (Interest for data): " << tracing_interest_ctr++<<" , "
			"Tracing Interest size: "<< tracing_interest_size << ", "
			"perPacketSize: "<< perPacketSize;

	std::string tsLog1(oss1.str());
	Log::write_to_tracing_interest_tracker_node_n(tsLog1, "/home/vartika-kite/kite_log_tracing_9.txt");
	oss1.flush();


	Simulator::Schedule (Seconds (0), &ApiFace::ExpressInterest, m_face, interest,
					 MakeCallback (&ServerApp9::OnData, this),
					 MakeCallback (&ServerApp9::OnTimeout, this));
	//m_face->ExpressInterest( interest,
	//                   MakeCallback (&ServerApp::OnData, this),
	//                   MakeCallback (&ServerApp::OnTimeout, this));


}


} // namespace ndn
} // namespace ns3
