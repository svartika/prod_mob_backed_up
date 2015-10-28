

#include "interest-anchor-forwarding.h"
#include "ns3/ndnSIM/utils/ndn-fw-hop-count-tag.h" //20151013
#include <boost/ref.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
namespace ll = boost::lambda;

namespace ns3 {
namespace ndn {
namespace fw {

NS_OBJECT_ENSURE_REGISTERED (AnchorPointForwarding);

LogComponent AnchorPointForwarding::g_log = LogComponent (AnchorPointForwarding::GetLogName ().c_str ());

std::string
AnchorPointForwarding::GetLogName ()
{
  return super::GetLogName ()+".AnchorPointForwarding";
}

TypeId AnchorPointForwarding::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::ndn::fw::AnchorPointForwarding")
    .SetGroupName ("Ndn")
    .SetParent <PitForwarding> ()
    .AddConstructor <AnchorPointForwarding> ()
 /*   .AddAttribute ("Pull", "Enable Pull.",
                   BooleanValue(true),
                   MakeBooleanAccessor (&PitForwarding::m_pull),
                   MakeBooleanChecker ())*/
    ;
  return tid;
}

AnchorPointForwarding::AnchorPointForwarding ()
{
  // m_pft = Create<Pft> ();
}

int AnchorPointForwarding::Pull (Ptr<Face> inFace, Ptr<const Interest> interest, Ptr<pit::Entry> pitEntry)
{
	int propagatedCount = 0;
	std::cout<<"AnchorPointForwarding::Pull at: "<<this<<"interest->GetName (): "<<interest->GetName ()<<"\n";
	//if (interest->GetPitForwardingNamePtr () != 0)
	//	std::cout<<"AnchorPointForwarding::Pull at: "<<this<<"interest->GetName (): "<<interest->GetName ()<<" .. interest->GetPitForwardingName (): "<<interest->GetPitForwardingName () <<"\n";

	if (((interest->GetPitForwardingFlag () & 1) != 1))   { // if NOT Tracable
		return 0;
	}

	for (Ptr<pit::Entry> pfEntry = m_pit->Begin(); pfEntry != m_pit->End(); pfEntry = m_pit->Next(pfEntry))
	{
		Ptr<const Interest> pfInterest = pfEntry->GetInterest ();

		if (pfInterest->GetPitForwardingNamePtr () != 0)
			std::cout<<"AnchorPointForwarding::Pull at: "<<this<<" pfInterest->GetPitForwardingName (): "<<pfInterest->GetPitForwardingName ()<<"\n";
		else
			std::cout<<"AnchorPointForwarding::Pull at: "<<this<<"pfInterest->GetPitForwardingName (): empty"<<"\n";

		if (pfInterest->GetPitForwardingNamePtr () != 0 && interest->GetPitForwardingNamePtr () != 0 ) {
			std::cout<<"AnchorPointForwarding::Pull at: "<<this<<" current entry->GetPitForwardingNamePtr (): "<<pfInterest->GetPitForwardingNamePtr ()<<" interest->GetPitForwardingNamePtr (): "<<interest->GetPitForwardingNamePtr ()<<"\n";
			std::string interestForwardingName = interest->GetPitForwardingName().toUri();
			std::string pfInterestForwardingName  = pfInterest->GetPitForwardingName().toUri();
			if(interestForwardingName.compare(pfInterestForwardingName) ==0) {
				pit::Entry::out_iterator face = pfEntry->GetOutgoing ().find (inFace);
				std::cout<<"AnchorPointForwarding::Pull : pfEntry->GetOutgoingCount (): "<<pfEntry->GetOutgoingCount ()<<"*inFace: "<<*inFace<<"\n";
				if (face == pfEntry-> GetOutgoing ().end ()) // Not yet being sent to inFace
				{
					Ptr<Face> outFace = 0;
					pit::Entry::in_iterator face = pfEntry->GetIncoming ().begin ();
					std::cout<<"AnchorPointForwarding::Pull : face->m_face "<<face->m_face<<"\n";
					for (; face != pfEntry->GetIncoming ().end (); face++)
					{
						int num=0;
						std::cout<<"AnchorPointForwarding::Pull: pfEntry->GetIncoming() face value in for loop: "<<face->m_face<<" inFace: "<<inFace<< " *inFace: "<<*inFace<<"\n";

						if (inFace != face->m_face)
						{
							outFace = face->m_face;
							std::cout<<"num: "<<num<<"endl";
							break;
						}
					}
					if (outFace == 0) // pulled by itself
					{
						std::cout<<"AnchorPointForwarding::Pull: outFace is zero in AnchorPointForwarding::Pull"<<"\n";
						return 0;
					}
					NS_LOG_INFO ("Interest Pulled by " << interest->GetName ());
					NS_LOG_DEBUG ("Inface: " << *outFace << " Outface: " << *inFace << " pfInterest" << *pfInterest);
					std::cout<<"AnchorPointForwarding::Pull: "<<"Interest Pulled by " << interest->GetName () <<" at: "<<*outFace << " Outface: " << *inFace << " pfInterest" << *pfInterest<<"\n";
					if (TrySendOutInterest (outFace, inFace, pfInterest, pfEntry))
					{
						NS_LOG_DEBUG ("Succeed: Interest Pulled to " << *inFace);
						propagatedCount++;
					}
					else
					{
						NS_LOG_DEBUG ("Failed: Interest Pulled to " << *inFace);
					}
				}
			}
		}
	}
	return propagatedCount;
}

bool AnchorPointForwarding::DoPitForwarding (Ptr<Face> inFace, Ptr<const Interest> interest, Ptr<pit::Entry> pitEntry)
{
	int propagatedCount = 0;

	if ((interest->GetPitForwardingFlag () & 1) == 1) // If Traceable
	{
		// To-do: Pit Forwarding Table
		std::cout<<"DoPitForwarding Pull at: "<<this<<interest->GetName ()<<"with m_pull: "<<m_pull<<"\n";
		if (m_pull) AnchorPointForwarding::Pull (inFace, interest, pitEntry);
	}

	if (interest->GetPitForwardingNamePtr () == 0)
	{
		return false;
	}

	NS_LOG_FUNCTION (this << interest->GetName ());
	NS_LOG_INFO ("PF Name: " << interest->GetPitForwardingName () << " Face: " << *inFace);
	Ptr<pit::Entry> pfEntry = m_pit->Find (interest->GetPitForwardingName ()); //first record with shorter or equal prefix as in content object will be found
	if (pfEntry == 0)
	{
		//NS_LOG_DEBUG ("! Entry Not Found");

		//doubt -- right now I am putting step (4) here but i will move this to doFlooding ..not Fib entry found after adding routes in to that -- 20151020 --vartika
		//(4) begins
		std::cout<<"AnchorPointForwarding::DoPitForwarding at: "<<this<<" vartika1 (pfEntry == 0)"<<"\n";
		NS_LOG_INFO ("vartika1");
		if(RedirectInterestToAnchor(inFace, interest, pitEntry)) return true;
		else return false;
		//(4) ends
	}
	std::cout<<"AnchorPointForwarding::DoPitForwarding at: "<<this<<" vartika2 (pfEntry != 0)"<<"\n";
	NS_LOG_INFO ("vartika2");
	Ptr<const Interest> pfInterest = pfEntry->GetInterest ();
	// Issue: for Interests with the same name, only the first one is here
	if ((pfInterest->GetPitForwardingFlag () & 1) != 1) // If not Tracable
	{
		NS_LOG_DEBUG ("! Entry Not Tracable");
		return false;
	}

	// this single Interest will fan out to all incomming faces except the one through which the Interest came in
	Ptr<Face> outFace = 0;
	for (pit::Entry::in_iterator face = pfEntry->GetIncoming ().begin ();
			face != pfEntry->GetIncoming ().end ();
			face++)
	{
		if (inFace == face->m_face)
		{
			NS_LOG_DEBUG ("Same Face " << *face->m_face);
			continue;
		}
		if (TrySendOutInterest (inFace, face->m_face, interest, pitEntry))
		{
			NS_LOG_DEBUG ("Propagated to " << *face->m_face);
			outFace = face->m_face;
			propagatedCount++;
		}
		else
		{
			NS_LOG_DEBUG ("Failed: Propagated to " << *face->m_face);
		}
	}

	NS_LOG_INFO ("Propagated to " << propagatedCount << " faces");
	return propagatedCount > 0;
}

bool
AnchorPointForwarding::DoFlooding (Ptr<Face> inFace,
                           Ptr<const Interest> interest,
                           Ptr<pit::Entry> pitEntry)
{
  NS_LOG_FUNCTION ("vartika1: "<<this << interest->GetName ());
  std::cout<<"AnchorPointForwarding::DoFlooding "<< this << interest->GetName () <<"\n";

  int propagatedCount = 0;
  // If No FIB entry or Only default entry, do not forward
  if ((! pitEntry->GetFibEntry ()) || (pitEntry->GetFibEntry ()->GetPrefix ().toUri () == "/"))
    {
	  std::cout<<"AnchorPointForwarding::DoFlooding says No FIB entry"<<"\n";
      NS_LOG_DEBUG ("! No FIB entry");
      return 0;
    }

  BOOST_FOREACH (const fib::FaceMetric &metricFace, pitEntry->GetFibEntry ()->m_faces.get<fib::i_metric> ())
    {
      //NS_LOG_DEBUG ("Trying " << boost::cref(metricFace));
      if (metricFace.GetStatus () == fib::FaceMetric::NDN_FIB_RED) // all non-read faces are in the front of the list
        break;

      std::string myString = "blah";
      std::stringstream buffer;
      metricFace.GetFace()->Print(buffer);
      myString = buffer.str();
      //std::cout<<"myString: "<<myString<< "buffer.str()"<< buffer.str()<<"\n";
      //std::cout<<"myString.find(ApiFace): "<<myString.find("ApiFace") << "std::string::npos: "<<std::string::npos<<"\n";
      //std::cout<<"metricFace.GetFace()->Print(std::cout)" <<metricFace.GetFace()->Print(std::cout)<<"\n";
      //std::cout<<"metricFace.GetFace()->GetTypeId ().GetName(): "<<metricFace.GetFace()->GetTypeId ().GetName()<<"metricFace.GetFace()->GetTypeId(): "<<metricFace.GetFace()->GetTypeId()<<"\n";
      if(myString.find("ApiFace")!=std::string::npos) { //this means that it is on application face ..like dev=ApiFace(5)
     // if(metricFace.GetFace()->GetTypeId ().GetName().compare("ns3::ndn::NetDeviceFace")==0) {
    	  std::cout<<"found match to string ApiFace ..local face  for interest->GetName().toUri(): "<<interest->GetName().toUri()<<"\n";
		  if(interest->GetName().toUri().find("/anchor1") !=std::string::npos ) {
			  // interest to anchor1 and not on local face (on network interface)
			  // traced or tracing - should not be consumed - this is a temporary hack - to ask jaebeom / dabin - doubt - 20151026
			  //break;
			  LoopOverPit(inFace, interest, "AnchorPointForwarding::DoFlooding");
			  return 1;
		  }
      }
      if (!TrySendOutInterest (inFace, metricFace.GetFace (), interest, pitEntry))
        {
          NS_LOG_DEBUG ("Failed: Propagated to " << *metricFace.GetFace ());
          std::cout<<"AnchorPointForwarding::DoFlooding Failed: Propagated to " << *metricFace.GetFace ()<<"\n";
          continue;
        }
      NS_LOG_DEBUG ("Propagated to " << *metricFace.GetFace ());
      std::cout<<"AnchorPointForwarding::DoFlooding Propagated to " << *metricFace.GetFace ()<<"\n";
      propagatedCount++;
  }

  LoopOverPit(inFace, interest, "AnchorPointForwarding::DoFlooding");

  NS_LOG_INFO ("Propagated to " << propagatedCount << " faces");
  std::cout<<"AnchorPointForwarding::DoFlooding return value: Propagated to " << propagatedCount << " faces"<<"\n";
  return propagatedCount > 0;
}

bool AnchorPointForwarding::RedirectInterestToAnchor(Ptr<Face> inFace, Ptr<const Interest> orgInterest, Ptr<pit::Entry> pitEntry)
{
	//extract anchor
	Ptr<const ndn::Name> name = orgInterest->GetNamePtr ();
	Ptr<ndn::Name> interestName = Create<ndn::Name> (name->toUri ());

	std::string seqPrefix = name->getPostfix (1, 0).toUri ();
	//std::cout<<"seqPrefix: " << seqPrefix;
	//NS_LOG_INFO ("vartika1: seqPrefix: " << seqPrefix);
	if(seqPrefix.compare("/")==0) return false;

	std::string anchorPrefix = name->getPostfix (1, 1).toUri ();
	//std::cout<<"anchorPrefix: " << anchorPrefix;
	//NS_LOG_INFO ("vartika1: anchorPrefix: " << anchorPrefix);
	if(anchorPrefix.compare("/")==0 ) return false;
	if(anchorPrefix.compare("anchor1")==0 ) return false;

	std::string producerPrefix = name->getPrefix (2,0).toUri ();
	//std::cout<<"producerPrefix: " << producerPrefix;
	//NS_LOG_INFO ("vartika1: producerPrefix: " << producerPrefix);
	if(producerPrefix.compare("/")==0) return false;

	//change interest name from /producer/file/anchor/anchor1/%14 to /anchor/anchor1/producer/file/%14
	std::string newName  = anchorPrefix+producerPrefix+seqPrefix;
	std::cout<<"newName: "<<newName<<"\n";
	NS_LOG_INFO ("vartika1: newName: " << newName);
	Ptr<ndn::Name> newInterestName =   Create<ndn::Name>(newName);

	Ptr<ndn::Interest> interest = Create<ndn::Interest> ();
	interest->SetNonce            (orgInterest->GetNonce());
	interest->SetName             (newInterestName);
	interest->SetInterestLifetime (orgInterest->GetInterestLifetime());
	interest->SetPitForwardingFlag (orgInterest->GetPitForwardingFlag ());
	if(orgInterest->GetPitForwardingNamePtr () != 0) {
		interest->SetPitForwardingName (orgInterest->GetPitForwardingName ());
	}
	//create new interest with new name ( /producer/file/anchor/anchor1/%14 to /anchor/anchor1/producer/file/%14)

	std::cout<<"AnchorPointForwarding::RedirectInterestToAnchor at: " << this <<" interest->GetName(): "<<interest->GetName()<<"\n";
	NS_LOG_INFO ("vartika1: interest->GetName(): " << interest->GetName());

	//express new interest - taken care below
	//make PIT entry - taken care below
	Ptr<pit::Entry> newPitEntry =  m_pit->Create (interest);
	if (newPitEntry != 0)
	{
		ForwardingStrategy::DidCreatePitEntry (inFace, interest, newPitEntry);
	}
	else
	{
		ForwardingStrategy::FailedToCreatePitEntry (inFace, interest);
		return false;
	}

	if( DoFlooding (inFace, interest, newPitEntry) )  {
		std::cout<<"AnchorPointForwarding::RedirectInterestToAnchor at: " << this <<" true from DoFlooding"<<"\n";
		NS_LOG_INFO ("vartika1: true from DoFlooding");
		return true;
	}
	else  {
		std::cout<<"AnchorPointForwarding::RedirectInterestToAnchor at: " << this <<" false from PitForwarding. Now, DoFlooding"<<"\n";
		NS_LOG_INFO ("vartika1: false from PitForwarding. Now, DoFlooding");
		return false;
	}
}
void AnchorPointForwarding::LoopOverPit(Ptr<Face> inFace, Ptr<const Interest> interest, std::string funcName) {
  //loop through pit table begins
 /* for (Ptr<pit::Entry> pfEntry = m_pit->Begin(); pfEntry != m_pit->End(); pfEntry = m_pit->Next(pfEntry))
	{
		Ptr<const Interest> pfInterest = pfEntry->GetInterest ();

		std::cout<<funcName<<" loop through pit table begins"<<"\n";

		pit::Entry::out_iterator face = pfEntry->GetOutgoing ().find (inFace);
		std::cout<< funcName <<": pfEntry->GetOutgoingCount (): "<<pfEntry->GetOutgoingCount ()<<" *inFace: "<<*inFace<<"\n";
		if (face == pfEntry-> GetOutgoing ().end ()) // Not yet being sent to inFace
		{
			Ptr<Face> outFace = 0;
			pit::Entry::in_iterator face = pfEntry->GetIncoming ().begin ();
			std::cout<<funcName<<": face->m_face "<<face->m_face<<"\n";
			for (; face != pfEntry->GetIncoming ().end (); face++)
			{
				int num=0;
				std::cout<<funcName<<": pfEntry->GetIncoming() face value in for loop: "<<face->m_face<<"\n";
				std::cout<<funcName<<": inFace value in for loop: "<<" inFace: "<<inFace<< " *inFace: "<<*inFace<<"\n";
				if (inFace != face->m_face)
				{
					outFace = face->m_face;
					std::cout<<funcName<<": num: "<<num<<"endl";
					break;
				}
			}
			if (outFace == 0) // pulled by itself
			{
				std::cout<<funcName<<": outFace is zero"<<"\n";
			}

		}

	}*/
  //loop through pit table ends

  //loop two begins
  std::cout<<funcName<<"::LoopOverPit:  m_pit: "<< m_pit <<" m_pit->Begin(): "<<m_pit->Begin()<<"\n";
  std::cout<<funcName<<"::LoopOverPit: m_pit->GetSize ()" << m_pit->GetSize () <<"\n";
  if(m_pit->GetMaxPitEntryLifetime ().GetMilliSeconds()<1000)m_pit->SetMaxPitEntryLifetime(Seconds(4.0));
  for (Ptr<pit::Entry> pfEntry = m_pit->Begin(); pfEntry != m_pit->End(); pfEntry = m_pit->Next(pfEntry))
  {
	Ptr<const Interest> pfInterest = pfEntry->GetInterest ();
	std::cout<<funcName<<"::LoopOverPit:  pfInterest->GetName(): "<< pfInterest->GetName() <<"\n";
	Ptr<Face> outFace = 0;
	pit::Entry::in_iterator face = pfEntry->GetIncoming ().begin ();
	std::cout<<funcName<<"::LoopOverPit: face->m_face "<<face->m_face<<"\n";
	for (; face != pfEntry->GetIncoming ().end (); face++)
	{
		int num=0;
		std::cout<<funcName<<"::LoopOverPit: pfEntry->GetIncoming(): "<<face->m_face<<" inFace: "<<inFace<< " *inFace: "<<*inFace<<"\n";
		std::cout<<funcName<<"::LoopOverPit: inFace value in for loop: "<<" inFace: "<<inFace<< " *inFace: "<<*inFace<<"\n";
		if (inFace != face->m_face)
		{
			outFace = face->m_face;
			std::cout<<funcName<<"::LoopOverPit: num: "<<num<<"\n";
			break;
		}
	}
	if (outFace == 0) // pulled by itself
	{
		std::cout<<funcName<<"::LoopOverPit: outFace is zero"<<"\n";
	}
  }
  //loop two ends

}
//20151013
void AnchorPointForwarding::OnInterest (Ptr<Face> inFace, Ptr<Interest> interest)
{
  std::cout<<"\n\nAnchorPointForwarding::OnInterest at: " << this <<" *inFace - interest->GetName (): "<<*inFace << interest->GetName ()<<"\n";
  NS_LOG_FUNCTION (inFace << interest->GetName ());
  m_inInterests (interest, inFace);

  Ptr<pit::Entry> pitEntry = m_pit->Lookup (*interest);

  bool similarInterest = true;
  if (pitEntry == 0) // vartika says - if no pit entry
    {

      similarInterest = false;
      pitEntry = m_pit->Create (interest);
      if (pitEntry != 0)
        {
    	  ForwardingStrategy::DidCreatePitEntry (inFace, interest, pitEntry);

    	  LoopOverPit(inFace, interest, "AnchorPointForwarding::OnInterest");

          std::string interestName = "empty";
          if(interest->GetPitForwardingNamePtr () != 0)
        	  interestName = interest->GetPitForwardingName ().toUri();
          std::cout<<"AnchorPointForwarding::OnInterest at: " << this <<" created PIT entry with forwarding name: " <<interestName<< " and inFace: "<< inFace<< "\n";
        }
      else
        {
    	  ForwardingStrategy::FailedToCreatePitEntry (inFace, interest);
          return;
        }
    }

  bool isDuplicated = true;
  if (!pitEntry->IsNonceSeen (interest->GetNonce ()))
    {
      pitEntry->AddSeenNonce (interest->GetNonce ());
      isDuplicated = false;
    }

  if (isDuplicated)
    {
      DidReceiveDuplicateInterest (inFace, interest, pitEntry);
      return;
    }
  std::cout<<"AnchorPointForwarding::OnInterest at: " << this <<" isDuplicated: " <<isDuplicated << "\n";

  Ptr<Data> contentObject;
  contentObject = m_contentStore->Lookup (interest);
  if (contentObject != 0)
    {
      FwHopCountTag hopCountTag;
      if (interest->GetPayload ()->PeekPacketTag (hopCountTag))
        {
          contentObject->GetPayload ()->AddPacketTag (hopCountTag);
        }

      pitEntry->AddIncoming (inFace/*, Seconds (1.0)*/);

      // Do data plane performance measurements
      WillSatisfyPendingInterest (0, pitEntry);

      // Actually satisfy pending interest
      SatisfyPendingInterest (0, contentObject, pitEntry);

      std::cout<<"AnchorPointForwarding::OnInterest at: " << this <<" after calling SatisfyPendingInterest "  << "\n";

      return;
    }

  if (similarInterest && ShouldSuppressIncomingInterest (inFace, interest, pitEntry))
    {
      pitEntry->AddIncoming (inFace/*, interest->GetInterestLifetime ()*/);
      // update PIT entry lifetime
      pitEntry->UpdateLifetime (interest->GetInterestLifetime ());

      // Suppress this interest if we're still expecting data from some other face
      NS_LOG_DEBUG ("Suppress interests");
      m_dropInterests (interest, inFace);

      DidSuppressSimilarInterest (inFace, interest, pitEntry);

      std::cout<<"AnchorPointForwarding::OnInterest at: " << this <<" after calling DidSuppressSimilarInterest "  << "\n";

      return;
    }

  if (similarInterest)
    {
      DidForwardSimilarInterest (inFace, interest, pitEntry);

      std::cout<<"AnchorPointForwarding::OnInterest at: " << this <<" after calling DidForwardSimilarInterest "  << "\n";
    }

  PropagateInterest (inFace, interest, pitEntry);
}
//20151013






} // namespace fw
} // namespace ndn
} // namespace ns3
