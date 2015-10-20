
#ifndef INTEREST_ANCHOR_H
#define INTEREST_ANCHOR_H

#include "interest-pit-forwarding.h"
#include "ns3/log.h"

namespace ns3 {
namespace ndn {
namespace fw {

/**
 * @ingroup ndn-fw
 * @brief
 */

class AnchorPointForwarding :
    public PitForwarding
{
private:
  typedef PitForwarding super;
  //Ptr<AggregatedInterests> aggregatedInterests;

public:
  static TypeId
  GetTypeId ();

  /**
   * @brief Helper function to retrieve logging name for the forwarding strategy
   */
  static std::string
  GetLogName ();
  
  /**
   * @brief Default constructor
   */
  AnchorPointForwarding ();

protected:
  // inherited from  ForwardingStrategy
  virtual bool
  DoPitForwarding (Ptr<Face> inFace,
                   Ptr<const Interest> interest,
                   Ptr<pit::Entry> pitEntry);

  //20151013
  virtual void
  OnInterest (Ptr<Face> face,
              Ptr<Interest> interest);

  virtual bool
  DoFlooding (Ptr<Face> inFace,
                Ptr<const Interest> interest,
                Ptr<pit::Entry> pitEntry);

  bool RedirectInterestToAnchor( Ptr<Face> inFace, Ptr<const Interest> orgInterest, Ptr<pit::Entry> pitEntry);
protected:
  static LogComponent g_log;
};

} // namespace fw
} // namespace ndn
} // namespace ns3

#endif // INTEREST_ANCHOR_H
