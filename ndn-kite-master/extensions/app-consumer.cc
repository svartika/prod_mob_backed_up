/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */


#include "app-consumer.h"
#include <sstream>

NS_LOG_COMPONENT_DEFINE ("ConsumerApp");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED (ConsumerApp);


ConsumerApp::ConsumerApp ()
{
  m_face = 0;
}

TypeId
ConsumerApp::GetTypeId ()
{
  static TypeId tid = TypeId ("ConsumerApp")
    .SetParent<Application> ()
    .AddConstructor<ConsumerApp> ()
    .AddAttribute ("ConsumerPrefix", "The name prefix of consumer.",
                  StringValue ("/consumer"),
                  MakeStringAccessor (&ConsumerApp::m_consumerPrefix),
                  MakeStringChecker ())
    .AddAttribute ("RequestPeriod", "The time interval of sending request Interest.",
                   DoubleValue (1.0),
                   MakeDoubleAccessor (&ConsumerApp::m_requestPeriod),
                   MakeDoubleChecker<double> (0.0, 120.0))
    ;
  return tid;
}

void
ConsumerApp::StartApplication ()
{
  // NS_LOG_FUNCTION (this);
  m_face = CreateObject<ApiFace> (GetNode ());
  Ptr<ndn::Name> prefix = Create<ndn::Name> (m_consumerPrefix);
  m_face->SetInterestFilter (prefix, MakeCallback (&ConsumerApp::OnInterest, this));
  m_mobilePrefixSize = 1;
  m_credit = 20;
  m_seq = 0;

  //vartika 20150929
  tracing_interest_ctr = 0;
  tracing_interest_size = 0;
  data_ctr = 0;

  //vartika request data - begins - 20151019
  m_mobilePrefix = "/producer/file";
  m_anchorPrefix = "/anchor1";

  m_seq = 0;
  m_credit = 20;
  for (int i=0; i < m_credit; i++)
  {
	  SendInterestToProducer (m_seq);
  }
  //vartika request data - ends
}

void
ConsumerApp::StopApplication ()
{
  m_face->Shutdown ();
  m_face = 0;
}

void
ConsumerApp::OnInterest (Ptr<const ndn::Name> origName, Ptr<const ndn::Interest> interest)
{
return;


//	NS_LOG_FUNCTION (interest);
	/*vartika to write log here to track time when server (consumer)
	gets interest from mobile producer 20150923*/
	Time now = Simulator::Now();
	//std::cout << "2.ServerApp::OnInterest  " << now.GetMilliSeconds();
	//20150924
	std::ostringstream oss;
	oss<< "2 ConsumerApp::OnInterest: " << now.GetMilliSeconds()<<" , ";
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

}

void
ConsumerApp::OnData (Ptr<const ndn::Interest> origInterest, Ptr<const ndn::Data> data)
{
	std::cout<<"ConsumerApp::OnData with *data: "<< *data<<"\n";
	std::ostringstream oss1;
	oss1<< "Data packets Received: " << data_ctr++;
	std::string tsLog1(oss1.str());
	Log::write_to_on_data_tracker(tsLog1, "/home/vartika-kite/ndn-kite-master/results/res/consumer_receives_data.txt");
	oss1.flush();

	SendInterestToProducer (m_seq);

	return;




	std::cout<<"ConsumerApp::OnData "<<"\n";
	NS_LOG_INFO ("vartika1: ConsumerApp::OnData ");

	/*vartika to write log here to track time when server (consumer) gets
	data from mobie producer 20150923*/
	Time now = Simulator::Now();
	//std::cout << "6.ServerApp::OnData  " << now.GetMilliSeconds();
	//20150924
	std::ostringstream oss;
	oss<< "6 ConsumerApp::OnData: " << now.GetMilliSeconds();
	std::string tsLog(oss.str());
	Log::write_ts_to_log_file(tsLog);
	oss.flush();






	NS_LOG_FUNCTION (data->GetNamePtr ());
	SendInterestToProducer (m_seq);
	m_seq ++;
}

void
ConsumerApp::OnTimeout (Ptr<const ndn::Interest> interest)
{
	/*SendInterestToProducer (m_seq++); // no use calling this. cos interest wont timeout here....it will always be consumed
	return;*/

	  Ptr<const ndn::Name> name = interest->GetNamePtr ();
	  SendInterestToProducer (name->get (-1).toNumber ());
	  return;
}

void
ConsumerApp::SendInterestToProducer (uint32_t seq)
{
	NS_LOG_FUNCTION (seq);
	Ptr<ndn::Interest> interest = Create<ndn::Interest> ();
	UniformVariable rand (0,std::numeric_limits<uint32_t>::max ());

	std::string strSeq = "/%0";
	std::stringstream ss;
	ss<<((int)m_seq%20)+1;
	//ss<<(seq);
	strSeq.append(ss.str());

	std::string interestName = m_mobilePrefix+ "/anchor" + m_anchorPrefix + strSeq;
	Ptr<ndn::Name> name = Create<ndn::Name> (interestName); // m_anchorPrefix); //m_mobilePrefix); //commented for now - doubt

	//name->appendNumber (seq); // commented for now - doubt

	interest->SetNonce            (rand.GetValue ());
	interest->SetName             (name);
	interest->SetInterestLifetime (Seconds (4.0));//( m_requestPeriod));
	m_seq++;
	//vartika has a doubt here - 20151019
	//i think this will always fail ..PIT will be only present from producer to anchor not in this path!
	//interest->SetPitForwardingFlag (2); // TraceOnly
	//interest->SetPitForwardingName (m_mobilePrefix);

	/*track time when server (consumer) sends interest to mobile producer to fetch data 20150923*/
	Time now = Simulator::Now();
	std::cout << "3 ConsumerApp::SendInterestToProducer interest with " << interest->GetName() << " strSeq: " << strSeq<< " at "<<now.GetMilliSeconds()<<"\n";
	std::ostringstream oss;
	oss<< "3 ConsumerApp::SendInterestToProducer: " << now.GetMilliSeconds()<<" , ";
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
	Log::write_to_tracing_interest_tracker_node_n(tsLog1, "/home/vartika-kite/ndn-kite-master/results/res/consumer_sent_tracing.txt");
	oss1.flush();

	/*Simulator::ScheduleNow (&ApiFace::ExpressInterest, m_face, interest,
			 MakeCallback (&ConsumerApp::OnData, this),
			 MakeCallback (&ConsumerApp::OnTimeout, this));*/

	Simulator::Schedule (Seconds (0.3), &ApiFace::ExpressInterest, m_face, interest,
					 MakeCallback (&ConsumerApp::OnData, this),
					 MakeCallback (&ConsumerApp::OnTimeout, this));

}


} // namespace ndn
} // namespace ns3
