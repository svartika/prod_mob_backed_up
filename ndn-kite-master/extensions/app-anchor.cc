/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn>
 */


#include "app-anchor.h"
#include <sstream>

NS_LOG_COMPONENT_DEFINE ("AnchorApp");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED (AnchorApp);


AnchorApp::AnchorApp ()
{
  m_face = 0;
}

TypeId
AnchorApp::GetTypeId ()
{
  static TypeId tid = TypeId ("AnchorApp")
    .SetParent<Application> ()
    .AddConstructor<AnchorApp> ()
    .AddAttribute ("Anchor Prefix", "The name prefix of server.",
                  StringValue ("/anchor1"),
                  MakeStringAccessor (&AnchorApp::m_anchorPrefix),
                  MakeStringChecker ())
    .AddAttribute ("RequestPeriod", "The time interval of sending request Interest.",
                   DoubleValue (1.0),
                   MakeDoubleAccessor (&AnchorApp::m_requestPeriod),
                   MakeDoubleChecker<double> (0.0, 120.0))
    ;
  return tid;
}

void
AnchorApp::StartApplication ()
{
  NS_LOG_FUNCTION (this);

  m_face = CreateObject<ApiFace> (GetNode ());
  Ptr<ndn::Name> prefix = Create<ndn::Name> (m_anchorPrefix);
  m_face->SetInterestFilter (prefix, MakeCallback (&AnchorApp::OnInterest, this));
  m_mobilePrefixSize = 1;
  m_credit = 20;
  m_seq = 0;

  tracing_interest_ctr = 0;
  tracing_interest_size = 0;
  data_ctr = 0;
}

void
AnchorApp::StopApplication ()
{
  m_face->Shutdown ();
  m_face = 0;
}

void
AnchorApp::OnInterest (Ptr<const ndn::Name> origName, Ptr<const ndn::Interest> interest)
{
	std::cout<<"AnchorApp::OnInterest: vartika says received interest at anchor: "<< this<<"origName: " <<origName->toUri()<<" interest: "<<interest->GetName()<<"\n";
	NS_LOG_FUNCTION ("vartika says received interest at anchor: "<< this<<"origName: " <<origName->toUri()<<" interest: "<<interest->GetName());

	/*		if tracing interest
			if(interest->GetPitForwardingFlag () == 2) {
				if(interest->GetPitForwardingNamePtr () != 0) {
					std::string forwardingInterestName = interest->GetPitForwardingName ();
					Ptr<pit::Entry> pfEntry = m_pit->Find (interest->GetPitForwardingName ());
					if (pfEntry == 0) {
						//no traced interest .. cannot forward yet..make PIT entry and wait
					}
				}
			}*/
	//if traced interest ( it will be taken care in forwarding engine - i.e. PIT entry will be made and it will be handled according to Kite)
	//remove self name -> leaving /producer/file/%14 or extract trace name
	//finds traced interest in PIT
	//makes PIT entry for
	//then follows the traced Interest to the new location

	//implement (6) here
	//todo on 21 october
	//doubt i want to know what is the order of flow
	//oninterest in cs on interest in application on interest of forwarding engine (forwarding strategy)
}

void
AnchorApp::OnData (Ptr<const ndn::Interest> origInterest, Ptr<const ndn::Data> data)
{

	/*vartika to write log here to track time when server (consumer) gets
	data from mobie producer 20150923*/
	Time now = Simulator::Now();
	std::cout << "6.ServerApp::OnData  " << now.GetMilliSeconds()<<"\n";
	//20150924
	std::ostringstream oss;
	oss<< "6 ServerApp::OnData: " << now.GetMilliSeconds() <<" , ";//std::endl;
	std::string tsLog(oss.str());
	Log::write_ts_to_log_file(tsLog);
	oss.flush();

	//20151001
	std::ostringstream oss1;
	oss1<< "Data packets Received: " << data_ctr++;
	std::string tsLog1(oss1.str());
	Log::write_to_on_data_tracker(tsLog1, "/home/vartika-kite/kite_log_data_1.txt");
	oss1.flush();

	NS_LOG_FUNCTION (data->GetNamePtr ());
	SendInterest (m_seq);
	m_seq ++;
}

void
AnchorApp::OnTimeout (Ptr<const ndn::Interest> interest)
{
  Ptr<const ndn::Name> name = interest->GetNamePtr ();
  SendInterest (name->get (-1).toNumber ());
  return;
}

void
AnchorApp::SendInterest (int seq)
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
	std::cout << "3.ServerApp::SendInterest " << now.GetMilliSeconds()<<"\n";
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
	Log::write_to_tracing_interest_tracker_node_n(tsLog1, "/home/vartika-kite/kite_log_tracing_1.txt");
	oss1.flush();


	Simulator::Schedule (Seconds (0), &ApiFace::ExpressInterest, m_face, interest,
					 MakeCallback (&AnchorApp::OnData, this),
					 MakeCallback (&AnchorApp::OnTimeout, this));
	//m_face->ExpressInterest( interest,
	//                   MakeCallback (&ServerApp::OnData, this),
	//                   MakeCallback (&ServerApp::OnTimeout, this));


}


} // namespace ndn
} // namespace ns3
