/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn>
 */

#include "regular-forwarding.h"

#include "ns3/ndn-interest.h"
#include "ns3/ndn-pit.h"
#include "ns3/ndn-pit-entry.h"

#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/boolean.h"

#include <boost/ref.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
namespace ll = boost::lambda;

namespace ns3 {
namespace ndn {
namespace fw {

NS_OBJECT_ENSURE_REGISTERED (RegularForwarding);



TypeId RegularForwarding::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::ndn::fw::RegularForwarding")
    .SetGroupName ("Ndn")
    .SetParent <ForwardingStrategy> ()
    .AddConstructor <RegularForwarding> ()
    ;
  return tid;
}

RegularForwarding::RegularForwarding ()
{
  // m_pft = Create<Pft> ();
}


void RegularForwarding::OnInterest (Ptr<Face> inFace, Ptr<Interest> interest)
{
  std::cout<<"\nRegularForwarding::OnInterest at: " << this <<" *inFace - interest->GetName (): "<<*inFace << interest->GetName ()<<"\n";

  std::string interestName = interest->GetName().toUri();

  std::cout<<"read from file - regular interest\n";
  Log::write_int_count();

  Flooding::OnInterest(inFace, interest);
}

} // namespace fw
} // namespace ndn
} // namespace ns3
