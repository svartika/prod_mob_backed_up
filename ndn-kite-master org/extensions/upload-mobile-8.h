/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn>
 */

#ifndef MOBILE_APP_H
#define MOBILE_APP_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/ndnSIM/ndn.cxx/ndn-api-face.h"
//vartika 20150924
#include "log.h"

//20150929
//#include "ns3/ndnSIM/model/wire/ccnb/wire-ccnb-interest.cc"

namespace ns3 {
namespace ndn {

class MobileApp8 : public Application
{

public:
	MobileApp8 ();

  static TypeId
  GetTypeId ();
  
  virtual void
  StartApplication ();

  virtual void
  StopApplication ();

  void
  OnInterest (Ptr<const ndn::Name> origName, Ptr<const ndn::Interest> interest);

  void
  OnData (Ptr<const Interest> origInterest, Ptr<const ndn::Data> data);

  void
  OnTimeout (Ptr<const ndn::Interest> interest);

  void
  SendInterest ();

private:
  std::string m_serverPrefix;
  Ptr<ApiFace> m_face;
  std::string m_mobilePrefix;
  double m_requestPeriod;

  //vartika 20150929
  int traced_interest_ctr;
  double traced_interst_size;
};

} // namespace nlsr
} // namespace ns3

#endif
