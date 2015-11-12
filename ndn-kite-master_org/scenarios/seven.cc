/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn>
 */
 
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"

using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("Seven");

int 
main (int argc, char *argv[])
{

  NS_LOG_FUNCTION ("vartika");

  int isKite = 1;
  int gridSize = 4;
  int mobileSize = 1;
  int speed = 40;
  int stopTime = 100;
  int joinTime = 1;

  CommandLine cmd;
  cmd.AddValue("kite", "enable Kite", isKite);
  cmd.AddValue("speed", "mobile speed m/s", speed);
  cmd.AddValue("size", "# mobile", mobileSize);
  cmd.AddValue("grid", "grid size", gridSize);  
  cmd.AddValue("stop", "stop time", stopTime);  
  cmd.AddValue("join", "join period", joinTime);  
  cmd.Parse (argc, argv);

  PointToPointHelper p2p;
  PointToPointGridHelper grid (gridSize, gridSize, p2p);
  grid.BoundingBox(0,0,400,400);

  NodeContainer wifiNodes;
  NodeContainer mobileNodes;
  mobileNodes.Create (mobileSize);

  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  wifiPhy.SetChannel (wifiChannel.Create ());
  wifiPhy.Set ("TxPowerStart",DoubleValue(5));
  wifiPhy.Set ("TxPowerEnd",DoubleValue(5));
  wifiPhy.Set ("TxPowerLevels",UintegerValue (1));
  wifiPhy.Set ("TxGain",DoubleValue (1));
  wifiPhy.Set ("RxGain",DoubleValue (1));

  WifiHelper wifi = WifiHelper::Default ();
  NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager");
  wifiMac.SetType("ns3::AdhocWifiMac");

  wifi.Install( wifiPhy, wifiMac, wifiNodes.GetGlobal());
   
  Ptr<RandomRectanglePositionAllocator> positionAlloc = CreateObject<RandomRectanglePositionAllocator> ();
  Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
  x->SetAttribute ("Min", DoubleValue (150));
  x->SetAttribute ("Max", DoubleValue (550));
  positionAlloc->SetX (x);
  positionAlloc->SetY (x);

  MobilityHelper mobility;
  mobility.SetPositionAllocator(positionAlloc);
  std::stringstream ss;
  ss << "ns3::UniformRandomVariable[Min=" << speed << "|Max=" << speed << "]";

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
    "Bounds", RectangleValue (Rectangle (150, 550, 150, 550)),
    "Distance", DoubleValue (150),
    "Speed", StringValue (ss.str ()));
  mobility.Install (mobileNodes); 

  ndn::StackHelper ndnHelper;
  ndnHelper.SetForwardingStrategy("ns3::ndn::fw::PitForwarding");
  ndnHelper.SetDefaultRoutes (true); // there must be an entry in FIB for an Interest, otherwise the Interest will be dropped

  std::string syncPrefix = "/server/upload";
  Ptr<Node> rpNode = grid.GetNode (0, 0);

  std::string syncPrefix2 = "/server2/upload";
  Ptr<Node> rpNode2 = grid.GetNode (1, 0);

  std::string syncPrefix3 = "/server3/upload";
  Ptr<Node> rpNode3 = grid.GetNode (2, 0);

  std::string syncPrefix4 = "/server4/upload";
  Ptr<Node> rpNode4 = grid.GetNode (3, 0);

  std::string syncPrefix5 = "/server5/upload";
  Ptr<Node> rpNode5 = grid.GetNode (0, 1);

  std::string syncPrefix6 = "/server6/upload";
  Ptr<Node> rpNode6 = grid.GetNode (1, 1);

  std::string syncPrefix7 = "/server7/upload";
  Ptr<Node> rpNode7 = grid.GetNode (1, 2);

  /*Ptr<ndn::FaceContainer> faces = ndnHelper.Install (mobileNodes);
  for (ndn::FaceContainer::Iterator i = faces->Begin ();
       i != faces->End ();
       i++)
  {
    Ptr<ndn::Face> face = faces->Get (i);
    ndn::StackHelper::AddRoute (face->GetNode (), syncPrefix, face->GetId (), 1);
  }*/


  for(uint32_t nodeCount=0; nodeCount<mobileNodes.GetN(); ++nodeCount)
   {

 	  Ptr<ndn::FaceContainer> faces = ndnHelper.Install (mobileNodes.Get(nodeCount));
 	  for (ndn::FaceContainer::Iterator i = faces->Begin ();
 		   i != faces->End ();
 		   i++)
 	  {
 		Ptr<ndn::Face> face = faces->Get (i);
 		//if(nodeCount==1) {
 			ndn::StackHelper::AddRoute (face->GetNode (), syncPrefix, face->GetId (), 1);
 		//} else if(nodeCount==2) {
 			//vartika 20150920
 			ndn::StackHelper::AddRoute (face->GetNode (), syncPrefix2, face->GetId (), 1);
 		//} else if(nodeCount==2) {
			//vartika 20150920
			ndn::StackHelper::AddRoute (face->GetNode (), syncPrefix3, face->GetId (), 1);
		//} else if(nodeCount==3) {
			//vartika 20150920
			ndn::StackHelper::AddRoute (face->GetNode (), syncPrefix4, face->GetId (), 1);
		//} else if(nodeCount==4) {
			//vartika 20150920
			ndn::StackHelper::AddRoute (face->GetNode (), syncPrefix5, face->GetId (), 1);
		//} else if(nodeCount==5) {
			//vartika 20150920
			ndn::StackHelper::AddRoute (face->GetNode (), syncPrefix6, face->GetId (), 1);
		//} else if(nodeCount==6) {
			//vartika 20150920
			ndn::StackHelper::AddRoute (face->GetNode (), syncPrefix7, face->GetId (), 1);
		//}
 	  }
   }

  for (int i = 0; i<gridSize; i++)
  {
    for (int j = 0; j<gridSize; j++)
    {
        ndnHelper.Install (grid.GetNode (i, j));
        if (i ==0 && j==0) continue;
        int m = i>j ? (i - 1) : i;
        int n = i>j ? j : (j - 1);
        ndn::StackHelper::AddRoute (grid.GetNode (i, j), syncPrefix, grid.GetNode (m, n), 1);
    }
  }

  ndn::AppHelper rpHelper ("ServerApp");
  ApplicationContainer rpApp= rpHelper.Install (rpNode);
  rpApp.Start (Seconds (0));

  ndn::AppHelper appHelper ("MobileApp");
  ApplicationContainer app = appHelper.Install (mobileNodes.Get(0)); //mn1);//mobileNodes.Get(1));
  app.Start (Seconds (1));

  ndn::AppHelper rpHelper2 ("ServerApp2");
   ApplicationContainer rpApp2 = rpHelper2.Install (rpNode2);
   rpApp2.Start (Seconds (0));

   ndn::AppHelper appHelper2 ("MobileApp2");
   ApplicationContainer app2 = appHelper2.Install (mobileNodes.Get(1)); //mn1);//mobileNodes.Get(2));
   app2.Start (Seconds (1));

   ndn::AppHelper rpHelper3 ("ServerApp3");
   ApplicationContainer rpApp3 = rpHelper3.Install (rpNode3);
   rpApp2.Start (Seconds (0));

   ndn::AppHelper appHelper3 ("MobileApp3");
   ApplicationContainer app3 = appHelper3.Install (mobileNodes.Get(2)); //mn1);//mobileNodes.Get(2));
   app3.Start (Seconds (1));

   ndn::AppHelper rpHelper4 ("ServerApp4");
   ApplicationContainer rpApp4 = rpHelper4.Install (rpNode4);
   rpApp4.Start (Seconds (0));

   ndn::AppHelper appHelper4 ("MobileApp4");
   ApplicationContainer app4 = appHelper4.Install (mobileNodes.Get(3)); //mn1);//mobileNodes.Get(2));
   app4.Start (Seconds (1));

   ndn::AppHelper rpHelper5 ("ServerApp5");
   ApplicationContainer rpApp5 = rpHelper5.Install (rpNode5);
   rpApp5.Start (Seconds (0));

   ndn::AppHelper appHelper5 ("MobileApp5");
   ApplicationContainer app5 = appHelper5.Install (mobileNodes.Get(4)); //mn1);//mobileNodes.Get(2));
   app5.Start (Seconds (1));

   ndn::AppHelper rpHelper6 ("ServerApp6");
   ApplicationContainer rpApp6 = rpHelper6.Install (rpNode6);
   rpApp6.Start (Seconds (0));

   ndn::AppHelper appHelper6 ("MobileApp6");
   ApplicationContainer app6 = appHelper6.Install (mobileNodes.Get(5)); //mn1);//mobileNodes.Get(2));
   app6.Start (Seconds (1));

   ndn::AppHelper rpHelper7 ("ServerApp7");
   ApplicationContainer rpApp7 = rpHelper7.Install (rpNode7);
   rpApp7.Start (Seconds (0));

   ndn::AppHelper appHelper7 ("MobileApp7");
   ApplicationContainer app7 = appHelper7.Install (mobileNodes.Get(6)); //mn1);//mobileNodes.Get(2));
   app7.Start (Seconds (1));


   NS_LOG_FUNCTION ("vartika" <<"mn1, mn2, mn3, mn4, mn5, mn6, mn7 "<<mobileNodes.Get(0)
		   <<mobileNodes.Get(1)<<mobileNodes.Get(2)<<mobileNodes.Get(3)
		   <<mobileNodes.Get(4)<<mobileNodes.Get(5)<<mobileNodes.Get(6));

  Simulator::Stop (Seconds (stopTime));
  ndn::L3AggregateTracer::InstallAll ("results/aggregate-trace.txt", Seconds (1));

//vartika - 20150925 - begins
//ndn::L3AggregateTracer::Install (grid.GetNode (0, 0), "results/node0-0.txt", Seconds (1));
//ndn::L3AggregateTracer::Install (grid.GetNode (1, 0), "results/node1-0.txt", Seconds (1));
//ndn::L3AggregateTracer::Install (grid.GetNode (0, 1), "results/node0-1.txt", Seconds (1));
//ndn::L3AggregateTracer::Install (grid.GetNode (1, 1), "results/node1-1.txt", Seconds (1));
ndn::L3AggregateTracer::Install (mobileNodes, "results/mobile-node.txt", Seconds (1));
//vartika - 20150925 - ends

  //ndn::AppDelayTracer::Install (mobileNodes, "results/app-delays-trace.txt");
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
