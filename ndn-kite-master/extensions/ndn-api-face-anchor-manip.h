/* -*- Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil -*- */


#include "ns3/ndn-api-face.h"


#ifndef NDN_API_FACE_ANCHOR_MANIP_H
#define NDN_API_FACE_ANCHOR_MANIP_H

namespace ns3 {
namespace ndn {

class ApiFacePrivManip;

class ApiFaceAnchorManip  : public ns3::ndn::ApiFace
{
public:

  ApiFaceAnchorManip (Ptr<Node> node);
  ~ApiFaceAnchorManip ();

  virtual bool SendInterest (Ptr<const Interest> interest);

private:
  ApiFacePrivManip *m_this;
};

} // ndn
} // ns3

#endif // NDN_API_HANDLER_ANCHOR_MANIP_H
