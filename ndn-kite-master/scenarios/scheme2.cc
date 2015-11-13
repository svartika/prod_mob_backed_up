/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"

using namespace ns3;

int 
main (int argc, char *argv[])
{
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
	ndnHelper.SetForwardingStrategy("ns3::ndn::fw::AnchorPointForwarding"); //PitForwarding");

	ndnHelper.SetContentStore ("ns3::ndn::cs::Nocache"); //, "MaxSize", "1");


	ndnHelper.SetDefaultRoutes (true); // there must be an entry in FIB for an Interest, otherwise the Interest will be dropped

	std::string consumerPrefix = "/consumer1";
	std::string anchorPrefix = "/anchor1/producer/file";
	Ptr<Node> consNode = grid.GetNode (0, 0);

	//set route to consumer - begins
	Ptr<ndn::FaceContainer> faces = ndnHelper.Install (mobileNodes);
	for (ndn::FaceContainer::Iterator i = faces->Begin ();
			i != faces->End ();
			i++)
	{
		Ptr<ndn::Face> face = faces->Get (i);
		ndn::StackHelper::AddRoute (face->GetNode (), consumerPrefix, face->GetId (), 1);
		//set route to anchor - begins
		ndn::StackHelper::AddRoute (face->GetNode (), anchorPrefix, face->GetId (), 1); // does this remove previous route?? //vartiak needs to check - 20151019
		//set route to anchor - ends
	}

	for (int i = 0; i<gridSize; i++)
	{
		for (int j = 0; j<gridSize; j++)
		{
			ndnHelper.Install (grid.GetNode (i, j));
			if (i ==0 && j==0) continue;
			int m = i>j ? (i - 1) : i;
			int n = i>j ? j : (j - 1);
			ndn::StackHelper::AddRoute (grid.GetNode (i, j), consumerPrefix, grid.GetNode (m, n), 1);
		}
	}
	//set route to consumer - ends


	int anchorPos = gridSize%2==0? (gridSize/2)-1 : (gridSize-1)/2;
	Ptr<Node> anchorNode = grid.GetNode (anchorPos, anchorPos);

	//set route to anchor - begins

	for (int i = 0; i<gridSize; i++)
	{
		for (int j = 0; j<gridSize; j++)
		{
			if (i ==anchorPos && j==anchorPos) continue;
			int m = j==anchorPos ? (i<anchorPos?i+1:i-1) : i;
			int n = j==anchorPos ? j : (j<anchorPos?j+1:j-1);
			ndn::StackHelper::AddRoute (grid.GetNode (i, j), anchorPrefix, grid.GetNode (m, n), 1);
		}
	}
	//set route to anchor - ends
	ndn::StackHelper::AddRoute (consNode, "/producer/file", grid.GetNode (1,0), 1);
	//set route to old location of producer - begins

	//set route to old location of producer - ends

	ndn::AppHelper anchorAppHelper ("AnchorApp");
	ApplicationContainer anchorApp = anchorAppHelper.Install (anchorNode);
	anchorApp.Start (Seconds (0.0));

	ndn::AppHelper producerAppHelper ("ProducerApp");
	ApplicationContainer producerApp = producerAppHelper.Install (mobileNodes);
	producerApp.Start (Seconds (1.0));

	ndn::AppHelper consumerAppHelper ("ConsumerApp");
	ApplicationContainer consumerApp= consumerAppHelper.Install (consNode);
	consumerApp.Start (Seconds (1.5));

	std::cout<<"vartika says..is ok?";
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
