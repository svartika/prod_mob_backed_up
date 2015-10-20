

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


bool AnchorPointForwarding::DoPitForwarding (Ptr<Face> inFace, Ptr<const Interest> interest, Ptr<pit::Entry> pitEntry)
{
	int propagatedCount = 0;

	if ((interest->GetPitForwardingFlag () & 1) == 1) // If Traceable
	{
		// To-do: Pit Forwarding Table
		if (m_pull) Pull (inFace, interest, pitEntry);
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
		std::cout<<"vartika1";
		NS_LOG_INFO ("vartika1");
		if(RedirectInterestToAnchor(inFace, interest, pitEntry)) return true;
		//(4) ends
	}
	std::cout<<"vartika2";
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

  int propagatedCount = 0;
  // If No FIB entry or Only default entry, do not forward
  if ((! pitEntry->GetFibEntry ()) || (pitEntry->GetFibEntry ()->GetPrefix ().toUri () == "/"))
    {
      NS_LOG_DEBUG ("! No FIB entry");
      return 0;
    }

  BOOST_FOREACH (const fib::FaceMetric &metricFace, pitEntry->GetFibEntry ()->m_faces.get<fib::i_metric> ())
    {
      //NS_LOG_DEBUG ("Trying " << boost::cref(metricFace));
      if (metricFace.GetStatus () == fib::FaceMetric::NDN_FIB_RED) // all non-read faces are in the front of the list
        break;

      if (!TrySendOutInterest (inFace, metricFace.GetFace (), interest, pitEntry))
        {
          NS_LOG_DEBUG ("Failed: Propagated to " << *metricFace.GetFace ());
          continue;
        }
      NS_LOG_DEBUG ("Propagated to " << *metricFace.GetFace ());
      propagatedCount++;
    }

  NS_LOG_INFO ("Propagated to " << propagatedCount << " faces");
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

	std::string anchorPrefix = name->getPostfix (1, 1).toUri ();
	//std::cout<<"anchorPrefix: " << anchorPrefix;
	//NS_LOG_INFO ("vartika1: anchorPrefix: " << anchorPrefix);

	std::string producerPrefix = name->getPrefix (2,0).toUri ();
	//std::cout<<"producerPrefix: " << producerPrefix;
	//NS_LOG_INFO ("vartika1: producerPrefix: " << producerPrefix);

	//change interest name from /producer/file/anchor/anchor1/%14 to /anchor/anchor1/producer/file/%14
	std::string newName  = anchorPrefix+producerPrefix+seqPrefix;
	//std::cout<<"newName: "<<newName;
	//NS_LOG_INFO ("vartika1: newName: " << newName);
	Ptr<ndn::Name> newInterestName =   Create<ndn::Name>(newName);

	Ptr<ndn::Interest> interest = Create<ndn::Interest> ();
	interest->SetNonce            (orgInterest->GetNonce());
	interest->SetName             (newInterestName);
	interest->SetInterestLifetime (orgInterest->GetInterestLifetime());
	interest->SetPitForwardingFlag (orgInterest->GetPitForwardingFlag ());
	if(interest->GetPitForwardingNamePtr () != 0) {
		interest->SetPitForwardingName (orgInterest->GetPitForwardingName ());
	}
	//create new interest with new name ( /producer/file/anchor/anchor1/%14 to /anchor/anchor1/producer/file/%14)

	std::cout<<"interest->GetName(): "<<interest->GetName();
	NS_LOG_INFO ("vartika1: interest->GetName(): " << interest->GetName());

	//express new interest - taken care below
	//make PIT entry - taken care below
	Ptr<pit::Entry> newPitEntry =  m_pit->Create (interest);
	if (newPitEntry != 0)
	{
		DidCreatePitEntry (inFace, interest, newPitEntry);
	}
	else
	{
		FailedToCreatePitEntry (inFace, interest);
		return false;
	}

	if( DoFlooding (inFace, interest, newPitEntry) )  {
		std::cout<<"true from DoFlooding";
		NS_LOG_INFO ("vartika1: true from DoFlooding");
		return true;
	}
	else  {
		std::cout<<"false from PitForwarding::DoFlooding";
		NS_LOG_INFO ("vartika1: false from PitForwarding::DoFlooding");
		return false;
	}
}

//20151013
void AnchorPointForwarding::OnInterest (Ptr<Face> inFace, Ptr<Interest> interest)
{

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
          DidCreatePitEntry (inFace, interest, pitEntry);
        }
      else
        {
          FailedToCreatePitEntry (inFace, interest);
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
      return;
    }

  if (similarInterest)
    {
      DidForwardSimilarInterest (inFace, interest, pitEntry);
    }

  PropagateInterest (inFace, interest, pitEntry);
}
//20151013

} // namespace fw
} // namespace ndn
} // namespace ns3
