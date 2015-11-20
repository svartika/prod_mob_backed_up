/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn>
 */


#include "upload-server.h"
#include <sstream>

NS_LOG_COMPONENT_DEFINE ("ServerApp");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED (ServerApp);


ServerApp::ServerApp ()
{
  m_face = 0;
}

TypeId
ServerApp::GetTypeId ()
{
  static TypeId tid = TypeId ("ServerApp")
    .SetParent<Application> ()
    .AddConstructor<ServerApp> ()
    .AddAttribute ("ServerPrefix", "The name prefix of server.",
                  StringValue ("/server/upload"),
                  MakeStringAccessor (&ServerApp::m_serverPrefix),
                  MakeStringChecker ())
    .AddAttribute ("RequestPeriod", "The time interval of sending request Interest.",
                   DoubleValue (4.0),
                   MakeDoubleAccessor (&ServerApp::m_requestPeriod),
                   MakeDoubleChecker<double> (0.0, 120.0))
    ;
  return tid;
}

void
ServerApp::StartApplication ()
{
  NS_LOG_FUNCTION (this);

  m_face = CreateObject<ApiFace> (GetNode ());
  Ptr<ndn::Name> prefix = Create<ndn::Name> (m_serverPrefix);
  m_face->SetInterestFilter (prefix, MakeCallback (&ServerApp::OnInterest, this));
  m_mobilePrefixSize = 1;
  m_credit = 20;
  m_seq = 0;

  //vartika 20150929
  tracing_interest_ctr = 0;
  tracing_interest_size = 0;
  data_ctr = 0;
}

void
ServerApp::StopApplication ()
{
  m_face->Shutdown ();
  m_face = 0;
}

void
ServerApp::OnInterest (Ptr<const ndn::Name> origName, Ptr<const ndn::Interest> interest)
{

	/*vartika to write log here to track time when server (consumer)
	gets interest from mobile producer 20150923*/
	Time now = Simulator::Now();
	std::cout << "2.ServerApp::OnInterest  " << now.GetMilliSeconds();
	//20150924
	/*std::ostringstream oss;
	oss<< "2 ServerApp::OnInterest: " << now.GetMilliSeconds()<<" , ";
	std::string tsLog(oss.str());
	Log::write_ts_to_log_file(tsLog);
	oss.flush();*/

	NS_LOG_FUNCTION (interest);
	if (m_seq > 0)
	{
		return;
	}

	// 2015.11.20 dabinbug
	//std::cout<<"m_seq is zero\n"<<std::endl;
	//---------enddabin --------------

	Ptr<const ndn::Name> name = interest->GetNamePtr ();
	m_uploadName = Create<ndn::Name> (name->toUri ());
	//m_uploadName = Create<ndn::Name> ("/server/upload/mobile/file");
	m_mobilePrefix = name->getPostfix (m_mobilePrefixSize+1, 0).toUri ();
	NS_LOG_DEBUG ("Mobile prefix " << m_mobilePrefix);
	m_seq = 1;
	int i = 0;
	for (; i < m_credit; i++)
	{
		//if(m_seq<30) {
			Log::write_org_interests();
			SendInterest (m_seq);
		//}
		//2015.11.20 ----
		//std::cout << "dabin: Send Interst, seq: m_seq"<<m_seq<< std::endl;
		//----end dabn
		m_seq++;
	}
}

void ServerApp::LoopThroughMap()
{
	for (std::map<std::string, long>::iterator it=_tsMap.begin(); it!=_tsMap.end(); ++it)
	    std::cout <<"ServerApp::LoopThroughMap: "<< it->first << " => " << it->second << '\n';
}

void
ServerApp::OnData (Ptr<const ndn::Interest> origInterest, Ptr<const ndn::Data> data)
{

	/*vartika to write log here to track time when server (consumer) gets
	data from mobie producer 20150923*/
	Time now = Simulator::Now();
	std::cout << "6.ServerApp::OnData  " << now.GetMilliSeconds() <<" .... ";
	//20150924

	LoopThroughMap();

	std::string intName = origInterest->GetName().toUri();
	std::map<std::string, long>::iterator it = _tsMap.find(intName);
	if (it != _tsMap.end())
	{
		long sent_ts = it->second;
		long latency = ((long)now.GetMilliSeconds()) - sent_ts;
		std::cout << "writing to received_data_ts: origInterest: " << origInterest->GetName() <<"latency: "<<latency << "\n";

		std::ostringstream oss;
		oss<< latency<<"\n";
		std::string tsLog(oss.str());

		Log::write_received_data_ts(tsLog);
		oss.flush();

	}


	//20151001
	std::ostringstream oss1;
	//oss1<< "Data packets Received: " << data_ctr++;
	oss1<< data_ctr++;
	std::string tsLog1(oss1.str());
	Log::write_to_on_data_tracker(tsLog1, "/home/vartika-kite/ndn-kite-master_org/results/res/consumer_receives_data.txt");
	oss1.flush();

	NS_LOG_FUNCTION (data->GetNamePtr ());

	//if(m_seq<30) {
		Log::write_org_interests();
		SendInterest (m_seq);
	//}
	//2015.11.20 ----
	//std::cout << now << ")dabin: Send Interst, seq: m_seq"<<m_seq<< std::endl;
	//----end dabn

	m_seq ++;
}

void
ServerApp::OnTimeout (Ptr<const ndn::Interest> interest)
{
  Log::write_retransmitted_interests();
  Ptr<const ndn::Name> name = interest->GetNamePtr ();
  SendInterest (name->get (-1).toNumber ());
  return;
}

void
ServerApp::SendInterest (int seq)
{
	//if(seq>30) return;

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
	std::cout << "3.ServerApp::SendInterest with " << interest->GetName() << " seq: " << seq<< "at " << now.GetMilliSeconds()<<"\n";
	//20150924

	//Log::write_sent_tracing_ts(interest->GetName()->toUri(), now.GetMilliSeconds());
	std::string intName = interest->GetName().toUri();
	_tsMap[intName]=now.GetMilliSeconds();

	float perPacketSize = interest->GetPayload()->GetSerializedSize();
	tracing_interest_size += interest->GetPayload()->GetSerializedSize();
	std::ostringstream oss1;
	/*oss1<< "Tracing Interest (Interest for data): " << tracing_interest_ctr++<<" , "
			"Tracing Interest size: "<< tracing_interest_size << ", "
			"perPacketSize: "<< perPacketSize;*/

	oss1<< tracing_interest_ctr++;

	std::string tsLog1(oss1.str());
	Log::write_to_tracing_interest_tracker_node_n(tsLog1, "/home/vartika-kite/ndn-kite-master_org/results/res/consumer_sent_tracing.txt");
	oss1.flush();


	Simulator::Schedule (Seconds (0.3), &ApiFace::ExpressInterest, m_face, interest,
					 MakeCallback (&ServerApp::OnData, this),
					 MakeCallback (&ServerApp::OnTimeout, this));
	//m_face->ExpressInterest( interest,
	//                   MakeCallback (&ServerApp::OnData, this),
	//                   MakeCallback (&ServerApp::OnTimeout, this));


}


} // namespace ndn
} // namespace ns3
