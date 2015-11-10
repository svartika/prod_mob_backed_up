
#ifndef INTEREST_ANCHOR_H
#define INTEREST_ANCHOR_H

#include "interest-pit-forwarding.h"
#include "ns3/log.h"
#include "log.h"

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

  virtual int
  Pull (Ptr<Face> inFace,
          Ptr<const Interest> interest,
          Ptr<pit::Entry> pitEntry);


  bool RedirectInterestToAnchor( Ptr<Face> inFace, Ptr<const Interest> orgInterest, Ptr<pit::Entry> pitEntry);

  void LoopOverPit(Ptr<Face> inFace, Ptr<const Interest> interest, std::string funcName);



  //for testing why tracing interest is dropped at next node on way back (while being pulled) - begins
  virtual bool CanSendOutInterest (Ptr<Face> inFace, Ptr<Face> outFace, Ptr<const Interest> interest, Ptr<pit::Entry> pitEntry);

  virtual bool TrySendOutInterest (Ptr<Face> inFace, Ptr<Face> outFace, Ptr<const Interest> interest, Ptr<pit::Entry> pitEntry);

  virtual void DidSendOutInterest (Ptr<Face> inFace, Ptr<Face> outFace, Ptr<const Interest> interest,Ptr<pit::Entry> pitEntry);
  //for testing why tracing interest is dropped at next node on way back (while being pulled) - ends

  virtual bool TrySendOutTracingInterest (Ptr<Face> inFace, Ptr<Face> outFace, Ptr<const Interest> interest, Ptr<pit::Entry> pitEntry);

  virtual void OnData (Ptr<Face> face, Ptr<Data> data);

  Ptr<ndn::Name> ReCreateOriginalName( Ptr<Face> inFace, Name dataName);

  int received_tracing_interest_ctr;
  void ProcessTracingInterestForProducer(Ptr<Face> inFace, Ptr<Face> outFace, Ptr<const Interest> interest);

protected:
  static LogComponent g_log;




};

} // namespace fw
} // namespace ndn
} // namespace ns3

#endif // INTEREST_ANCHOR_H
