/* -*- Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil -*- */

#include "ndn-api-face-anchor-manip.h"

#include "ns3/ndnSIM/ndn.cxx/detail/pending-interests-container.h"
#include "ns3/ndnSIM/ndn.cxx/detail/registered-prefix-container.h"

#include <ns3/random-variable.h>

#include <ns3/ndn-l3-protocol.h>

#include <ns3/ndn-interest.h>
#include <ns3/ndn-data.h>
#include <ns3/ndn-face.h>
#include <ns3/ndn-fib.h>

#include <ns3/packet.h>
#include <ns3/log.h>


using namespace std;
using namespace boost;
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ndn.ApiFaceAnchorManip");

namespace ns3 {
namespace ndn {

using namespace detail;

class ApiFacePrivManip
{
public:
	ApiFacePrivManip () : m_rand (0, std::numeric_limits<uint32_t>::max ())
  {
  }

  ns3::UniformVariable m_rand; // nonce generator

  PendingInterestContainer m_pendingInterests;
  RegisteredPrefixContainer m_expectedInterests;
};


ApiFaceAnchorManip::ApiFaceAnchorManip (Ptr<Node> node) : ApiFace (node)
{
/*  NS_LOG_FUNCTION (this << boost::cref (*this));

  NS_ASSERT_MSG (GetNode ()->GetObject<L3Protocol> () != 0,
                 "NDN stack should be installed on the node " << GetNode ());

  GetNode ()->GetObject<L3Protocol> ()->AddFace (this);
  this->SetUp (true);
  this->SetFlags (APPLICATION);*/
}

ApiFaceAnchorManip::~ApiFaceAnchorManip ()
{
/*  NS_LOG_FUNCTION (this << boost::cref (*this));

  delete m_this;*/
}


bool ApiFaceAnchorManip::SendInterest (Ptr<const Interest> interest)
{


	//ApiFace::SendInterest(interest);

	NS_LOG_FUNCTION (this << interest);

	NS_LOG_DEBUG ("<< I " << interest->GetName ());
	std::cout<<"ApiFaceAnchorManip::SendInterest this: "<<this <<" interest<< interest->GetName ()"<<  interest->GetName ().toUri()<<"\n";
	if (!IsUp ())
	{
		std::cout<<"ApiFaceAnchorManip::SendInterest !IsUp () -> return false"<<"\n";
		return false;
	}
	// the app cannot set several filters for the same prefix
	RegisteredPrefixContainer::iterator entry = m_this->m_expectedInterests.longest_prefix_match (interest->GetName ());
	if (entry == m_this->m_expectedInterests.end ())
	{
		std::cout<<"ApiFaceAnchorManip::SendInterest entry == m_this->m_expectedInterests.end () -> return false" <<"\n";
		return false;
	}

	if (!entry->payload ()->m_callback.IsNull ())
		entry->payload ()->m_callback (entry->payload ()->GetPrefix (), interest);
	return true;
}



}
}
