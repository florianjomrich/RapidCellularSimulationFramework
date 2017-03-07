/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */
 
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/config-store-module.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;
// ------------------------------Code for CSV file parsing -----Starts here----------------------------------------------//
class CSVRow
{
    public:
        std::string const& operator[](std::size_t index) const
        {
            return m_data[index];
        }
        std::size_t size() const
        {
            return m_data.size();
        }
        void readNextRow(std::istream& str)
        {
            std::string         line;
            std::getline(str, line);

            std::stringstream   lineStream(line);
            std::string         cell;

            m_data.clear();
            while(std::getline(lineStream, cell, ','))
            {
                m_data.push_back(cell);
            }
        }
    private:
        std::vector<std::string>    m_data;
};

std::istream& operator>>(std::istream& str, CSVRow& data)
{
    data.readNextRow(str);
    return str;
}

struct cell_tower_parameter
{
    double latitude_x;
    double longitude_y;
    double download_earfcn;
    double upload_earfcn;
    double transmit_bandwidth;
    double transmission_power;
    double antenna_height;
    double antenna_gain;
    double antenna_sending_direction_angle;
      
        
};

// ------XXXXX-------------------Code for CSV file parsing -----Ends here---------------------XXXXX-------------------------//

//-------------------------------Logs on terminal ------------- Starts here -----------------------------------------------------------//
NS_LOG_COMPONENT_DEFINE ("LenaX2HandoverMeasures");

void
NotifyConnectionEstablishedUe (std::string context,
                               uint64_t imsi,
                               uint16_t cellid,
                               uint16_t rnti)
{
  std::cout << context
            << " UE IMSI " << imsi
            << ": connected to CellId " << cellid
            << " with RNTI " << rnti
            << std::endl;
}

void
NotifyHandoverStartUe (std::string context,
                       uint64_t imsi,
                       uint16_t cellid,
                       uint16_t rnti,
                       uint16_t targetCellId)
{
  std::cout << context
            << " UE IMSI " << imsi
            << ": previously connected to CellId " << cellid
            << " with RNTI " << rnti
            << ", doing handover to CellId " << targetCellId
            << std::endl;
}

void
NotifyHandoverEndOkUe (std::string context,
                       uint64_t imsi,
                       uint16_t cellid,
                       uint16_t rnti)
{
  std::cout << context
            << " UE IMSI " << imsi
            << ": successful handover to CellId " << cellid
            << " with RNTI " << rnti
            << std::endl;
}

void
NotifyConnectionEstablishedEnb (std::string context,
                                uint64_t imsi,
                                uint16_t cellid,
                                uint16_t rnti)
{
  std::cout << context
            << " eNB CellId " << cellid
            << ": successful connection of UE with IMSI " << imsi
            << " RNTI " << rnti
            << std::endl;
}

void
NotifyHandoverStartEnb (std::string context,
                        uint64_t imsi,
                        uint16_t cellid,
                        uint16_t rnti,
                        uint16_t targetCellId)
{
  std::cout << context
            << " eNB CellId " << cellid
            << ": start handover of UE with IMSI " << imsi
            << " RNTI " << rnti
            << " to CellId " << targetCellId
            << std::endl;
}

void
NotifyHandoverEndOkEnb (std::string context,
                        uint64_t imsi,
                        uint16_t cellid,
                        uint16_t rnti)
{
  std::cout << context
            << " eNB CellId " << cellid
            << ": completed handover of UE with IMSI " << imsi
            << " RNTI " << rnti
            << std::endl;
}
//-------XXXXX-------------------Logs on terminal -----------------Ends here ----------------------------XXXXX-------------//

//-------------------------------Main NS3 program -----------------Starts here --------------------------------------------//
int
main (int argc, char *argv[])
{

//-------------------------------Code for Counting number of UEs ----Starts Here --------------------------------------------//

const int MAX_CHARS_PER_LINE = 512;
const char* const DELIMITER = " ";
int numberOfUes=0;

std::vector<std::string> listoftoken;
std::ifstream fin;
fin.open("/home/ubuntu/Downloads/ns-allinone-3.26/ns-3.26/src/lte/examples/mobsmple.tcl"); // open a file
	if (!fin.good())
		return 1; // exit if file not found
	while (!fin.eof())
	{
		char buf[MAX_CHARS_PER_LINE];
        fin.getline(buf, MAX_CHARS_PER_LINE);
		std::stringstream st;
        st << strtok(buf, DELIMITER); 
        if (st.str().size()) // if line is not empty
        {
			if (st.str().size()>= 9)
            {
                int lastBracketPos = st.str().find(")");
                std::string number = st.str().substr(7,lastBracketPos-7);
                std::stringstream numString(number);
                int intNumber;
                numString >> intNumber;
				numberOfUes = std::max(intNumber ,numberOfUes);
				listoftoken.push_back(st.str());
            }
        }
		
    }

numberOfUes= numberOfUes+1;
std::cout<<"Number of Ues ="<<numberOfUes<<std::endl;

//--------------XXXXX------------Code for Counting number of UEs --------Ends Here --------------XXXXX--------------------------//
   
uint16_t numberOfEnbs = 0; 
uint16_t numBearersPerUe = 1;
double simTime = 140;//225;//445;//442;//275;//182;      
double enbTxPowerDbm = 46.0; //default value for tranmission power if not individually specified for the cell tower - this value is taken


//-------------------Opening and reading CSV file contents for ENb//  as well as // setting the bounding box----------Starts  Here ------//


std::ifstream    file1;
file1.open("/home/ubuntu/Downloads/ns-allinone-3.26/ns-3.26/src/lte/examples/cellTowersForNS3.csv");  // csv file name 

double latitude_x ;
double longitude_y ;
double download_earfcn;
double upload_earfcn;
double transmit_bandwidth;
double transmission_power;
double antenna_height;
double antenna_gain;
double antenna_sending_direction_angle;


//parameters of the bounding box
double x_min_bounding_box =0;
double x_max_bounding_box =0;
double y_min_bounding_box =0;
double y_max_bounding_box =0;
	
	std::vector<cell_tower_parameter> CellTowerCSVInformationVector;
	CSVRow              row;

    int i=0;
    int firstline = 1;
    while(file1 >> row)
    {
        if(!firstline){ 

        latitude_x =atof(row[13].c_str());
        longitude_y =atof(row[14].c_str());
        download_earfcn =atof(row[7].c_str());
        upload_earfcn=atof(row[8].c_str());
        transmit_bandwidth=atof(row[10].c_str());
        transmission_power =atof(row[19].c_str());
        antenna_height=atof(row[24].c_str());
        antenna_gain=atof(row[27].c_str());
        antenna_sending_direction_angle=atof(row[25].c_str());

        cell_tower_parameter current_row = {latitude_x, longitude_y, download_earfcn,upload_earfcn,transmit_bandwidth,transmission_power,
                                            antenna_height,antenna_gain,antenna_sending_direction_angle};

        CellTowerCSVInformationVector.push_back(current_row);
        i++;
        numberOfEnbs++;

        //set the Bounding-Box
        if(latitude_x<x_min_bounding_box) x_min_bounding_box = latitude_x - 2000;
        if(latitude_x>x_max_bounding_box) x_max_bounding_box = latitude_x + 2000;
        if(longitude_y<y_min_bounding_box) y_min_bounding_box = longitude_y - 2000;
        if(longitude_y>y_max_bounding_box) y_max_bounding_box = longitude_y + 2000;

        }

        else{
        firstline = 0; //just skip the first line
        }
    }
//------XXXXX-----Opening and reading CSV file contents for ENb//  as well as // setting the bounding box----------Ends  Here ------------XXXXX------//

//---------------  configuring Default values---------------------Starts here---------------------------------------------------------------------//

Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (1250));
Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("100Mb/s"));
Config::SetDefault ("ns3::LteHelper::UseIdealRrc", BooleanValue (true));
Config::SetDefault("ns3::LteEnbRrc::DefaultTransmissionMode",UintegerValue(2));

//pathloss model
Config::SetDefault ("ns3::LteHelper::PathlossModel", StringValue ("ns3::FriisPropagationLossModel"));
       
// Command line arguments
CommandLine cmd;
cmd.AddValue ("simTime", "Total duration of the simulation (in seconds)", simTime);
cmd.AddValue ("enbTxPowerDbm", "TX power [dBm] used by HeNBs (defalut = 46.0)", enbTxPowerDbm);
cmd.Parse (argc, argv);

//ConfigStore:
ConfigStore config;
config.ConfigureDefaults ();

//---------XXXXX-----------configuring Default values-----------------------------------Ends here ----------------XXXXX---------------//

//--------------------------Create UE // Install mobility model -----------------------Starts Here-----------------//
NodeContainer ueNodes;
ueNodes.Create (numberOfUes);
Ns2MobilityHelper ns2 = Ns2MobilityHelper ("/home/ubuntu/Downloads/ns-allinone-3.26/ns-3.26/src/lte/examples/mobsmple.tcl");
ns2.Install();
//-----------XXXXX-----------Create UE // Install mobility model ----------------------Ends Here--------XXXXX---------//

//----------------------------------Configure Lte attributes via lte helper  ----------------------Starts Here-----------------//
Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
lteHelper->SetEpcHelper (epcHelper);
lteHelper->SetSchedulerType ("ns3::RrFfMacScheduler");
lteHelper->SetHandoverAlgorithmType ("ns3::A3RsrpHandoverAlgorithm");
lteHelper->SetHandoverAlgorithmAttribute ("Hysteresis",
                                              DoubleValue (3.0));
lteHelper->SetHandoverAlgorithmAttribute ("TimeToTrigger",
                                              TimeValue (MilliSeconds (256)));
//----------XXXXX------------------------Configure Lte attributes via lte helper  ----------------------Ends  Here---------XXX--------//

//------------------------------------Create Packet gateway and remote host-----------------------Starts Here----------------------//
Ptr<Node> pgw = epcHelper->GetPgwNode ();

// Create a single RemoteHost
NodeContainer remoteHostContainer;
remoteHostContainer.Create (1);
Ptr<Node> remoteHost = remoteHostContainer.Get (0);
InternetStackHelper internet;
internet.Install (remoteHostContainer);


//---------XXXXX-----------------Create Packet gateway and remote host-------------Ends Here------------XXX----------//

// ----------------------------Create the Internet// and add routing ---------------------- Starts Here-------------------------//
PointToPointHelper p2ph;
p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("1000Gb/s")));
p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
Ipv4AddressHelper ipv4h;
ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

// Routing of the Internet Host (towards the LTE network)
Ipv4StaticRoutingHelper ipv4RoutingHelper;
Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
// interface 0 is localhost, 1 is the p2p device
remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

// ------XXXXX-----------------Create the Internet// and add routing ------------------ Ends Here-------------XXXXX------------//

// ---------------------------------Create eNB// add Mobility ------------------ Starts Here-------------------------//
NodeContainer enbNodes;
enbNodes.Create (numberOfEnbs);
Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator> ();
for (uint16_t i = 0; i < numberOfEnbs; i++)
    {
      Vector enbPosition (CellTowerCSVInformationVector[i].latitude_x, CellTowerCSVInformationVector[i].longitude_y, CellTowerCSVInformationVector[i].antenna_height);
      enbPositionAlloc->Add (enbPosition);
    }
MobilityHelper enbMobility;
enbMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
enbMobility.SetPositionAllocator (enbPositionAlloc);
enbMobility.Install (enbNodes);


// --------XXXXX---------------------------Create eNB// add Mobility --------------------- Ends Here---------------XXXXX----------//

// ---------------------------Configuring antenna parameters for ENb// Installing ltedevices in enb  ------------------ Starts Here-------------------------//

//First antenna
NetDeviceContainer enbLteDevs;       

//all the following antennas
for (uint16_t i = 0; i < numberOfEnbs; i++)
    {      
		lteHelper->SetEnbAntennaModelType ("ns3::CosineAntennaModel");
		lteHelper->SetEnbAntennaModelAttribute ("Orientation", DoubleValue (CellTowerCSVInformationVector[i].antenna_sending_direction_angle));
		lteHelper->SetEnbAntennaModelAttribute ("Beamwidth",   DoubleValue (66)); 
		lteHelper->SetEnbAntennaModelAttribute ("MaxGain",     DoubleValue (CellTowerCSVInformationVector[i].antenna_gain));
		lteHelper->SetEnbDeviceAttribute ("DlEarfcn", UintegerValue (100));           // manual configuration.. can also be configured through csv file. 
		lteHelper->SetEnbDeviceAttribute ("UlEarfcn", UintegerValue (100 + 18000));  // manual configuration.. can also be configured through csv file.
		lteHelper->SetEnbDeviceAttribute ("DlBandwidth", UintegerValue (100));      // manual configuration.. can also be configured through csv file. 
		lteHelper->SetEnbDeviceAttribute ("UlBandwidth", UintegerValue (100));     // manual configuration.. can also be configured through csv file. 
		if(i==0)
		{//first enB
			enbLteDevs = lteHelper->InstallEnbDevice (enbNodes.Get (i));
        }
		else
		{
			enbLteDevs.Add ( lteHelper->InstallEnbDevice (enbNodes.Get (i)));
        }      
      
	}

  
//Configure the enBs
for (uint16_t i = 0; i < numberOfEnbs; i++)
    { //configure the transmission power of all the enB (cell tower) individually
		Ptr<LteEnbPhy> enb0Phy = enbLteDevs.Get(i)->GetObject<LteEnbNetDevice> ()->GetPhy (); 
		enb0Phy->SetTxPower (CellTowerCSVInformationVector[i].transmission_power); 
		//std::cout<< "Transmission power:"<<CellTowerCSVInformationVector[i].transmission_power << std::endl;
         
     } 
// ---------XXXXX------Configuring antenna parameters for ENb// Installing ltedevices in enb  ------------------ Ends Here------------XXXXX-------------//

// ----------------------Installing Ltedevices and the Ip stack in UE  ------------------------------ Starts Here--------------------------------//
// Install LTE Devices in UEs 
NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);   

// Install the IP stack on the UEs
internet.Install (ueNodes);
Ipv4InterfaceContainer ueIpIfaces;
ueIpIfaces = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));

// Attach all UEs to the first eNodeB
for (int i = 0; i < numberOfUes; i++)
    {
      lteHelper->Attach (ueLteDevs.Get (i), enbLteDevs.Get (0));
    }
// --------XXXXX-------------------Installing Ltedevices and the Ip stack in UE  ------------------ Ends Here--------------XXXXX------------------//
NS_LOG_LOGIC ("setting up applications");

//--------------------------Install and start applications on UEs and remote host-------------------Starts Here----------------------//
uint16_t dlPort = 10000;
uint16_t ulPort = 20000;

// randomize a bit start times to avoid simulation artifacts
// (e.g., buffer overflows due to packet transmissions happening exactly at the same time)
Ptr<UniformRandomVariable> startTimeSeconds = CreateObject<UniformRandomVariable> ();
startTimeSeconds->SetAttribute ("Min", DoubleValue (0));
startTimeSeconds->SetAttribute ("Max", DoubleValue (0.010));

for (int u = 0; u < numberOfUes; ++u)
    {
		Ptr<Node> ue = ueNodes.Get (u);
		// Set the default gateway for the UE
		Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ue->GetObject<Ipv4> ());
		ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);

		for (uint32_t b = 0; b < numBearersPerUe; ++b)
		{
			++dlPort;
			++ulPort;

			OnOffHelper onoff ("ns3::UdpSocketFactory", Address (InetSocketAddress (ueIpIfaces.GetAddress (u), dlPort))); // (the address of the remote node to send traffic to. in this case n3)
			onoff.SetAttribute("OnTime",StringValue("ns3::ConstantRandomVariable[Constant=30]"));
			onoff.SetAttribute("OffTime",StringValue("ns3::ConstantRandomVariable[Constant=0]"));
			ApplicationContainer appsdl = onoff.Install(remoteHost); //install in n0
			PacketSinkHelper sink1 ("ns3::UdpSocketFactory", Address(InetSocketAddress(Ipv4Address::GetAny(), dlPort)));
			appsdl = sink1.Install(ue);
	
			
			OnOffHelper onoff2 ("ns3::UdpSocketFactory", Address (InetSocketAddress (remoteHostAddr, ulPort))); // (the address of the remote node to send traffic to. in this case n3)
			onoff2.SetAttribute("OnTime",  StringValue("ns3::ConstantRandomVariable[Constant=5000]"));
			onoff2.SetAttribute("OffTime",StringValue("ns3::ConstantRandomVariable[Constant=0]"));
			ApplicationContainer appsul = onoff2.Install(ue); //install in n0
			PacketSinkHelper sink2 ("ns3::UdpSocketFactory", Address(InetSocketAddress(Ipv4Address::GetAny(), ulPort)));
			appsul = sink2.Install(remoteHost);

			Ptr<EpcTft> tft = Create<EpcTft> (); //packet filter setup - by default any traffic is allowed
			EpcTft::PacketFilter dlpf;
			dlpf.localPortStart = dlPort;
			dlpf.localPortEnd = dlPort;
			tft->Add (dlpf);
			EpcTft::PacketFilter ulpf;
			ulpf.remotePortStart = ulPort;
			ulpf.remotePortEnd = ulPort;
			tft->Add (ulpf);
			EpsBearer bearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT); //NGBR - Non Guaranteed Bit Rate
			lteHelper->ActivateDedicatedEpsBearer (ueLteDevs.Get (u), bearer, tft);

			Time startTime = Seconds (startTimeSeconds->GetValue ());
			appsul.Start (startTime);
			appsdl.Start (startTime);

        }
    }
//---------XXXXX-----------------Install and start applications on UEs and remote host-------------------Ends Here--------XXXXX--------------//

// Add X2 inteface
lteHelper->AddX2Interface (enbNodes);

// Traces generated out of simulation
lteHelper->EnablePhyTraces ();
lteHelper->EnableMacTraces ();
lteHelper->EnableRlcTraces ();
lteHelper->EnablePdcpTraces ();

Ptr<RadioBearerStatsCalculator> rlcStats = lteHelper->GetRlcStats ();
rlcStats->SetAttribute ("EpochDuration", TimeValue (Seconds (1.0)));
Ptr<RadioBearerStatsCalculator> pdcpStats = lteHelper->GetPdcpStats ();
pdcpStats->SetAttribute ("EpochDuration", TimeValue (Seconds (1.0)));

// connect custom trace sinks for RRC connection establishment and handover notification
Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionEstablished", MakeCallback (&NotifyConnectionEstablishedEnb));
Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/ConnectionEstablished", MakeCallback (&NotifyConnectionEstablishedUe));
Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverStart", MakeCallback (&NotifyHandoverStartEnb));
Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/HandoverStart", MakeCallback (&NotifyHandoverStartUe));
Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverEndOk", MakeCallback (&NotifyHandoverEndOkEnb));
Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/HandoverEndOk", MakeCallback (&NotifyHandoverEndOkUe));

AnimationInterface anim ("hand.xml");
AnimationInterface::SetConstantPosition (pgw, 750, 1000 ); 
AnimationInterface::SetConstantPosition (remoteHost, 1000, 1000); 
anim.EnablePacketMetadata (true); 

//visualisation of the radio map
Ptr<RadioEnvironmentMapHelper> remHelper = CreateObject<RadioEnvironmentMapHelper> ();
remHelper->SetAttribute ("ChannelPath", StringValue ("/ChannelList/1")); //channel 1 oder 2 sind belegt - einer Upload - anderer Download
remHelper->SetAttribute ("OutputFile", StringValue ("carrem.out"));
remHelper->SetAttribute ("XMin", DoubleValue (x_min_bounding_box));
remHelper->SetAttribute ("XMax", DoubleValue (x_max_bounding_box));
remHelper->SetAttribute ("XRes", UintegerValue (200));
remHelper->SetAttribute ("YMin", DoubleValue (y_min_bounding_box));
remHelper->SetAttribute ("YMax", DoubleValue (y_max_bounding_box));
remHelper->SetAttribute ("YRes", UintegerValue (200));
remHelper->SetAttribute ("Z", DoubleValue (1.5));

//these two values don't work in this scenario somehow - see the check in lena-dual-stripe.cc therefore
//therefore keep them uncommented
//remHelper->SetAttribute ("UseDataChannel", BooleanValue (true));
//remHelper->SetAttribute ("RbId", IntegerValue (10));

//uncomment for plotting of Radio Environment Map (Snr Heat map with gnuplot script)
//remHelper->Install (); 
// simulation will stop right after the REM has been generated

// Flow monitor
//FlowMonitorHelper flowmon;
//Ptr<FlowMonitor> monitor = flowmonHelper.InstallAll (); //does not work correctly - since no FlowProbe should be installed on enBs
//Ptr<FlowMonitor> monitor = flowmon.Install(ueNodes); // UE
//monitor = flowmon.Install(remoteHostContainer);  //remote Host - these two are enough to test properly

//ConfigStore:
config.ConfigureAttributes ();

//run the simulation 
Simulator::Stop (Seconds (10));
Simulator::Run ();

// Flow monitor
//---------------------------------------------------Flowmonitor calulate throughput-------------- Starts here ----------------------------------------//
/*
Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
	for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
		{
			Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
			if ((t.sourceAddress=="1.0.0.2" && t.destinationAddress == "7.0.0.2"))
			{
				std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
				std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
				std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
				std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps\n";
				
			}
     
			else if ((t.sourceAddress=="7.0.0.2" && t.destinationAddress == "1.0.0.2"))
			{
				std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
				std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
				std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
				std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps\n";
			}
			else if ((t.sourceAddress=="12.0.0.5" && t.destinationAddress == "12.0.0.6"))
			{
				std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
				std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
				std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
				std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps\n";
			}
			else if ((t.sourceAddress=="12.0.0.6" && t.destinationAddress == "12.0.0.5"))
			{
				std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
				std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
				std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
				std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps\n";
			}
		}
flowmon.SerializeToXmlFile("CarHandoverFlowMonitorOutput.xml", true, true); */

// -----------------XXXXX----------------------------------flowmonitor calulate throughput---------------- Ends here ------------------XXXXX----------------------//

Simulator::Destroy ();
return 0;
// -----------------XXXXX----------------------------------Main program---------------- Ends here ------------------XXXXX----------------------//
}

