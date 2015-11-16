

#ifndef REGULAR_FORWARDING_H
#define REGULAR_FORWARDING_H


#include "ns3/ndn-forwarding-strategy.h"
#include "ns3/log.h"
#include "ns3/ndnSIM-module.h"

#include "ns3/ndnSIM/model/pit/ndn-pit-impl.h"
#include "ns3/ndnSIM/utils/trie/fifo-policy.h"


//#include "ns3/ndn-forwarding-strategy.h"
#include "ns3/ndnSIM/model/fw/flooding.h"
#include "log.h"

namespace ns3 {
namespace ndn {
namespace fw {

class RegularForwarding :
    public Flooding// ForwardingStrategy
{
private:
  typedef Flooding super; //ForwardingStrategy super;

public:
  static TypeId
  GetTypeId ();


  /**
   * @brief Default constructor
   */
  RegularForwarding ();

protected:


  virtual void
    OnInterest (Ptr<Face> face,
                Ptr<Interest> interest);

};

} // namespace fw
} // namespace ndn
} // namespace ns3

#endif //REGULAR_FORWARDING_H
