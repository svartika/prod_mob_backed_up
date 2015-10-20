
#ifndef INTEREST_AGGREGATION_H
#define INTEREST_AGGREGATION_H

#include "interest-pit-forwarding.h"
#include "ns3/log.h"

namespace ns3 {
namespace ndn {
namespace fw {

/**
 * @ingroup ndn-fw
 * @brief Interest aggregration for traced intests
 *
 * An Interest I(A) will be aggregated with another interest I(B) if
 * I(A)'s destination (anchor) is same as I(B)'s destination
 * and both I(A) and I(B) have PitForwardingFlag set to 1.
 *
 * Otherwise, Interests will be forwarded using PitForwarding strategy:
 *
 * Usage example:
 * @code
 *     ndnHelper.SetForwardingStrategy ("ns3::ndn::fw::PitAggregateBeforeForwarding");
 *     ...
 *     ndnHelper.Install (nodes);
 * @endcode
 */

class PitAggregateBeforeForwarding :
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
  PitAggregateBeforeForwarding ();

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
protected:
  static LogComponent g_log;
};

} // namespace fw
} // namespace ndn
} // namespace ns3

#endif // INTEREST_AGGREGATION_H
