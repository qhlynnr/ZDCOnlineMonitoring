//
/*
 * checkUnpacker.C: Macro to check that the unpacked sums and the emulated sums are equivalent.
 * Input: Folder of L1Ntuples
 * Output: A root file of histograms relevant to checking the unpacking.
 * Authors: Hannah Bossi <hannah.bossi@cern.ch>, Gian Michele Innocenti, <gian.michele.innocenti@cern.ch>
 * 9/10/23
*/

#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

#include "TDirectory.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TChain.h"

#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphAsymmErrors.h"
#include "TCanvas.h"
#include "TLegend.h"

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

//Adding JSON reader for 2025 datataking
#include "include/JSON_handler.h"

using namespace std;

enum bxSchemeBits {
  kEmpty = 0,
  kBx1 = 1,
  kBx2 = 2,
  kBxBoth = 3
};

void GetFiles(char const *input, vector<string> &files) {
  TSystemDirectory dir(input, input);
  TList *list = dir.GetListOfFiles();
  if (list) {
    TSystemFile *file;
    string fname;
    TIter next(list);
    while ((file = (TSystemFile *)next())) {
      fname = file->GetName();
      if (file->IsDirectory() && (fname.find(".") == string::npos)) {
        string newDir = string(input) + fname + "/";
        cout << "NewDir: " << newDir << endl;
        GetFiles(newDir.c_str(), files);
      } else if ((fname.find(".root") != string::npos)) {
        files.push_back(string(input) + fname);
	//        cout << files.back() << endl;
      }
    }
  }

  return;
}

void FillChain(TChain &chain, vector<string> &files) {
  for (auto file : files) {
    chain.Add(file.c_str());
  }
}

map<int, bxSchemeBits> getBxScheme(string schemeCSV)
{
  map<int, bxSchemeBits> bxScheme;

  ifstream inFile(schemeCSV.c_str());
  string line;
  while(getline(inFile, line)){
    std::cout << "LINE: " << line << std::endl;

    if(line.size() == 0) continue;
    if(line.find("bunch_number") != std::string::npos) continue;
    //Processing csv so comma separate
    vector<string> lineCommaSep;
    while(line.find(",") != string::npos && line.find(",") != 0){
      lineCommaSep.push_back(line.substr(0,line.find(",")));
      line.replace(0, line.find(",")+1, "");
    }

    //Grab bxnum, and define the bit as empty, bx1, bx2, or both
    int bxNum = std::stoi(lineCommaSep[0]);
    double bx1 = std::stod(lineCommaSep[2]);
    double bx2 = std::stod(lineCommaSep[3]);
    bxSchemeBits bxBit = kEmpty;
    if(bx1 > 0.0 && bx2 > 0.0) bxBit = kBxBoth;
    else if(bx1 > 0.0) bxBit = kBx1;
    else if(bx2 > 0.0) bxBit = kBx2;

    bxScheme[bxNum] = bxBit;
  }
  inFile.close();

  return bxScheme;
}


// params
// input: folder of l1ntuples to read from - must contain the branches specified above
// verbose: whether or not to print out the full output (error messgaes will always be printed)

int plotZDCEmuLoop() {

    // const char *textfilewithfolders = "/eos/cms/store/group/phys_heavyions/cmcginn/Run2026/PhysicsHIPhysicsRawPrime10/404/195/"


    string outDirPlot = "/eos/cms/store/group/phys_heavyions/xirong/ZDCNeutronPeakPlots/";
    std::vector<std::string> folderNames;
    std::ifstream file("AllFolders.txt");

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty())
            folderNames.push_back(line);
    }

    cout << "Number of folder" << folderNames.size() << endl;


  //Define the format you will save to
  vector<string> extensions = {"png"};//, "C"}; //add the .C output if you want to do rapid edits after the fact
  string pdName = "PhysicsHIPhysicsRawPrime";//Have to replace this for alt pds

  //Try to extract the run number automatically from the path
  const string outFileName = "output/plotZDCEmuHists_" + pdName + "_RunAll.root";
  vector<string> files;

    //Some auto-handling of things link run->Fill, nBunches, etc.
    map<string, string> runNumToFill;
    //2025 runToFills
    runNumToFill["399937"] = "11331";
    runNumToFill["399925"] = "11330";//second fill with tracker
    runNumToFill["399916"] = "11330";//Cosmics to test valid bx range in ttrees
    runNumToFill["399912"] = "11328";//First fill w/ tracker back
    runNumToFill["399843"] = "11323";//No tracker fill
    runNumToFill["399706"] = "11314";
    runNumToFill["399655"] = "11310";
    runNumToFill["399593"] = "11305";
    runNumToFill["399588"] = "11304";
    runNumToFill["399572"] = "11303";//CMS VdM
    runNumToFill["399543"] = "11297";
    //2026 runToFills
    runNumToFill["404157"] = "11771";//spurious
    runNumToFill["404186"] = "11772";
    runNumToFill["404187"] = "11772";
    runNumToFill["404188"] = "11772";
    runNumToFill["404189"] = "11772";
    runNumToFill["404190"] = "11772";
    runNumToFill["404191"] = "11772";
    runNumToFill["404192"] = "11772";
    runNumToFill["404193"] = "11772";
    runNumToFill["404194"] = "11772";
    runNumToFill["404195"] = "11772";
    runNumToFill["404196"] = "11772";
    runNumToFill["404197"] = "11772";

    map<string, string> fillToBunches;
    //2025 fillsToBunches
    fillToBunches["11331"] = "1032";
    fillToBunches["11330"] = "1032";//Cosmics and second fill with tracker
    fillToBunches["11328"] = "1032";//First fill w/ tracker back
    fillToBunches["11323"] = "1032"; //No tracker fill
    fillToBunches["11314"] = "1032";
    fillToBunches["11310"] = "1032";
    fillToBunches["11305"] = "642";
    fillToBunches["11304"] = "642";
    fillToBunches["11303"] = "374";//CMS VdM
    fillToBunches["11297"] = "204";
    //2026 fillsToBunches
    fillToBunches["11771"] = "6";//spurious
    fillToBunches["11772"] = "6";//spurious


    bool isMB = false;
    bool isZeroBias = false;
    bool isEmptyBunches = false;
    bool isCosmics = false;

    //2026.05.23 - early runs turn off json
    bool applyJSON = false;

    //JSON_handler for sticking to "good" lumi sections; exceptions made for tracker out fill, VdM
    JSON_handler dcs;//give a file path argument for overriding default

  // START LOOP HERE//
  string runYear = "2026";
  string runNum = "";
  for (int i = 0; i < folderNames.size(); i++){
    string input = folderNames[i];
    string inputStr = string(input);

    string pdnum = "";
    string key = "PhysicsHIPhysicsRawPrime";

    size_t pos = inputStr.find(key);
    if (pos != string::npos) {
        pos += key.length();
        // Extract digits after the key
        while (pos < inputStr.size() && isdigit(inputStr[pos])) {
            pdnum += inputStr[pos];
            ++pos;
        }
    }
    cout << "pdnum: " << pdnum << endl;

    runNum = inputStr;
    //Consider: ExpressCosmics, HIExpress, HIForwardX
    string formatInput = "/eos/cms/store/group/phys_heavyions/cmcginn/Run" + runYear + "/" + pdName + pdnum + "/";
    
    cout << "Running over:  " << input << endl;
    cout << "Format Input: " << formatInput << endl;

    if(runNum.find(formatInput) != string::npos){
        runNum.replace(0, formatInput.size(), "");

        while(runNum.find("/") != string::npos){
        runNum.replace(runNum.find("/"), 1, "");
        }
    }
    else{
        cout << "input '" << input << "' format unrecognized, return 1" << endl;
        return 1;
    }
    cout << "Input: " << input << endl;
    GetFiles(input.c_str(), files);

    //Since we process per run, lets test if the run is good

    /*if(applyJSON){
        if(!dcs.isGoodRun(stoi(runNum)) && runNum.find("399843") == std::string::npos){//skip 399843 since tracker out doesnt matter for ZDC
        cout << __PRETTY_FUNCTION__ << " WARNING: Run '" << runNum << "' not found in json, return 1" << endl;
        return 1;
        }
    }
    else{
        cout << __PRETTY_FUNCTION__ << " WARNING: JSON IS DISABLED - BE CAREFUL INTERPRETING RESULTS! Proceeding..." << endl;
    }*/
  
    isMB = inputStr.find("HIPhysicsRawPrime");
    isZeroBias = inputStr.find("PhysicsHIForward") != string::npos;
    isEmptyBunches = inputStr.find("HIExpress") != string::npos;
    isCosmics = inputStr.find("ExpressCosmics") != string::npos;
  }  

  cout << "[Debug] Files to be processed: " << endl;
    for (auto const & file : files){
        cout << "File: " << file << endl;
    }

    string fill = runNumToFill[runNum]; 
    if(fill.size() == 0){cout << "Fill not set. return 1" << endl; return 1;}

    string nBunch = fillToBunches[fill];
    if(nBunch.size() == 0){cout << "NBunch for fill=" << fill << " not set. return 1" << endl; return 1;}

    //BX scheme for fills where we have it
    bool doBxMap = false;
    map<int, bxSchemeBits> bxMap;
    string bxSchemeCSV = "";
    int maxBxFromMap = -1;

    bool fillForBxMap = fill.find("11323") != string::npos;
    fillForBxMap = fillForBxMap || fill.find("11328") != string::npos;
    fillForBxMap = fillForBxMap || fill.find("11330") != string::npos;
    fillForBxMap = fillForBxMap || fill.find("11331") != string::npos;
    fillForBxMap = fillForBxMap || fill.find("11332") != string::npos;
    if(fillForBxMap){
        doBxMap = true;
        bxSchemeCSV = "input/data_BxSchemeFill" + fill + ".csv";
        bxMap = getBxScheme(bxSchemeCSV);

        for(auto const & val : bxMap){
        if(val.first > maxBxFromMap) maxBxFromMap = val.first;
        }
    }
    //Define the relative bx range
    int maxDeltaBxPlus = 0;
    int maxDeltaBxMinus = 0;
    map<int, int> deltaBxCounter, absToRelBxPlusMap, absToRelBxMinusMap;
    if(doBxMap){
        for(auto const & val : bxMap){
        int pos = val.first;
        int localDelta = 0;
        while(bxMap[pos] != kBxBoth){
        --pos;
        ++localDelta;
        if(pos < 0) pos = maxBxFromMap;
        }

        absToRelBxPlusMap[val.first] = localDelta;
        if(localDelta > maxDeltaBxPlus) maxDeltaBxPlus = localDelta;

        ++deltaBxCounter[localDelta];

        pos = val.first;
        localDelta = 0;
        while(bxMap[pos] != kBxBoth){
        ++pos;
        --localDelta;
        if(pos > maxBxFromMap) pos = 0;
        }

        absToRelBxMinusMap[val.first] = localDelta;
        if(localDelta < maxDeltaBxMinus) maxDeltaBxMinus = localDelta;
        }
    }

    cout << "MaxDeltaBxPlus: " << maxDeltaBxPlus << endl;
    cout << "MaxDeltaBxMinus: " << maxDeltaBxMinus << endl;

    for(auto const & val : deltaBxCounter){
        cout << " " << val.first << ", " << val.second << endl;
    }


    cout << __LINE__ << endl;

    const int nHist = 3;//We run w/ max number of triggers

    //Define a trigger set for additional filtering offline
    map<string, int> l1MB;
    l1MB["L1_ZeroBias"] = 1;//ZB is also in rawprime, and can be useful
    l1MB["L1_MinimumBiasHF1_AND_BptxAND"] = 94;//MB w/o ZDC
    l1MB["L1_MinimumBiasZDC1n_Th1_OR_MinimumBiasHF1_AND_BptxAND"] = 99;//MB w/ ZDC

    map<string, int> l1ZeroBias;
    l1ZeroBias["L1_ZeroBias"] = 1; // Defined order in OMS for uGTTree, example run https://cmsoms.cern.ch/cms/triggers/l1_rates?cms_run=399843

    map<string, int> l1EmptyBunches;
    l1EmptyBunches["L1_NotBptxOR"] = 20; //same order as above in OMS
    l1EmptyBunches["L1_UnpairedBunchBptxMinus"] = 35;
    l1EmptyBunches["L1_UnpairedBunchBptxPlus"] = 36;

    map<string, int> l1Cosmics;
    l1Cosmics["L1_AlwaysTrue"] = 0; //same order as above in OMS

    // End loop here //
    cout << __LINE__ << endl;

    //Test nHist
    if(l1MB.size() > nHist && isMB){
        cout << "L1MB map exceeds nhist size (" << nHist << "). return 1" << endl;
        return 1;
    }
    if(l1ZeroBias.size() > nHist && isZeroBias){
        cout << "L1ZeroBias map exceeds nhist size (" << nHist << "). return 1" << endl;
        return 1;
    }
    if(l1EmptyBunches.size() > nHist && isEmptyBunches){
        cout << "L1EmptyBunches map exceeds nhist size (" << nHist << "). return 1" << endl;
        return 1;
    }
    if(l1Cosmics.size() > nHist && isCosmics){
        cout << "L1Cosmics map exceeds nhist size (" << nHist << "). return 1" << endl;
        return 1;
    }
    cout << __LINE__ << endl;

    //Generic l1 map + test that you are using zerobias or empty bx
    string globalLabel = "";
    map<string, int> l1Map;
    map<string, int> l1MapCounter;
    if(isMB){
        l1Map = l1MB;
        globalLabel = "MBPD";
    }
    else if(isZeroBias){
        l1Map = l1ZeroBias;
        globalLabel = "ZeroBias";
    }
    else if(isEmptyBunches){
        l1Map = l1EmptyBunches;
        globalLabel = "Empty Bx";
    }
    else if(isCosmics){
        l1Map = l1Cosmics;
        globalLabel = "Cosmics";
    }
    else{
        cout << "isEmptyBunches, isCosmics, and isZeroBias is false. return 1" << endl;
        return 1;
    }

    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);

    // Lynn fix: Changing tag
    string tag = "Run" + runNum;
    string lumiStr = "Run: " + runNum + ", Fill: " + fill + ", nBunches: " + nBunch;
    string cmsLabel = "#bf{CMS}#it{Preliminary} " + runYear + " PbPb (5.36 TeV) " + globalLabel;
    cout << __LINE__ << endl;
    //  if(isZeroBias) cmsLabel = cmsLabel + " ZeroBias";
    //  else if(isEmptyBunches) cmsLabel = cmsLabel + " Empty Bx";

    const double topY = 0.86;
    const double bottomY = 0.36;
    const double xLatex = 0.17;

    TLatex* cms = new TLatex(0.10,0.92,cmsLabel.c_str());
    cms->SetNDC();
    cms->SetTextSize(0.05);
    cms->SetTextFont(42);

    TLatex* lumi = new TLatex(xLatex,topY,lumiStr.c_str());
    lumi->SetNDC();
    lumi->SetTextSize(0.035);
    lumi->SetTextFont(42);
    cout << __LINE__ << endl;
  /* read in the unpacked information */
  TChain zdcChain("zdcanalyzer/zdcrechit");
  FillChain(zdcChain, files);
  TTreeReader zdcReader(&zdcChain);
  TTreeReaderValue<float> sumPlus(zdcReader, "sumPlus");
  TTreeReaderValue<float> sumMinus(zdcReader, "sumMinus");

  /* read in the unpacked information */
  TChain unpackerChain("l1UpgradeTree/L1UpgradeTree");
  FillChain(unpackerChain, files);
  TTreeReader unpackerReader(&unpackerChain);
  TTreeReaderValue<unsigned short> nEtSums(unpackerReader, "nSums");
  TTreeReaderValue<vector<float> > unpackerEtSum(unpackerReader, "sumEt");
  TTreeReaderValue<vector<float>>	unpackerEtSumBx(unpackerReader, "sumBx");
  TTreeReaderValue<vector<short>>	unpackerEtSumType(unpackerReader, "sumType");
  TTreeReaderValue<vector<short> > unpackerSum(unpackerReader, "sumZDCIEt");
  TTreeReaderValue<vector<short>>	unpackerType(unpackerReader, "sumZDCType");
  TTreeReaderValue<vector<float>>	unpackerBx(unpackerReader, "sumZDCBx");
  TTreeReaderValue<unsigned short> nJets(unpackerReader, "nJets");
  TTreeReaderValue<vector<float>>	jetEt(unpackerReader, "jetEt");
  TTreeReaderValue<vector<float>>	jetEta(unpackerReader, "jetEta");

  TChain hiEventChain("l1EventTree/L1EventTree");
  FillChain(hiEventChain, files);
  TTreeReader eventReader(&hiEventChain);
  TTreeReaderValue<uint> runNumber(eventReader, "run");
  TTreeReaderValue<ULong64_t> eventNumber(eventReader, "event");
  TTreeReaderValue<uint> lumiSection(eventReader, "lumi");
  TTreeReaderValue<uint> bx(eventReader, "bx");

  TChain emuChain("l1UpgradeEmuTree/L1UpgradeTree");
  FillChain(emuChain, files);
  TTreeReader emuReader(&emuChain);
  TTreeReaderValue<vector<short>> emuSum(emuReader, "sumZDCIEt");
  TTreeReaderValue<vector<short>> emuType(emuReader, "sumZDCType");
  TTreeReaderValue<vector<float>>	emuBx(emuReader, "sumZDCBx");
  TTreeReaderValue<unsigned short> nSums(emuReader, "nSumsZDC");

   /* read in information on the emulated trigger fires*/
  TChain l1TrigChain("l1uGTTree/L1uGTTree");
  FillChain(l1TrigChain, files);
  TTreeReader emuTrigReader(&l1TrigChain);
  TTreeReaderValue<vector<bool>> initialTrigDecision(emuTrigReader, "m_algoDecisionFinal");

  /* create histograms */
  map<string, TH1D*> hZDCP_Emu;
  map<string, TH1D*> hZDCM_Emu;
  map<string, TH1D*> hZDCP_withTrig;
  map<string, TH1D*> hZDCM_withTrig;
  map<string, TH1D*> hZDCP;
  map<string, TH1D*> hZDCM;
  map<string, TH1D*> hZDCP_EmuScaled;
  map<string, TH1D*> hZDCM_EmuScaled;
  map<string, TH1D*> hZDC_Asymm;
  map<string, TH1D*> hZDC_AsymmNorm;
  map<string, TH2D*> hZDC_AsymmVBx;
  map<string, TH2D*> hZDC_AsymmNormVBx;
  map<string, TH2D*> hZDC_AsymmVRelBxPlus;
  map<string, TH2D*> hZDC_AsymmNormVRelBxPlus;
  map<string, TH2D*> hZDC_AsymmVRelBxPlusZoom;
  map<string, TH2D*> hZDC_AsymmNormVRelBxPlusZoom;
  map<string, TH2D*> hZDC_AsymmVRelBxMinus;
  map<string, TH2D*> hZDC_AsymmNormVRelBxMinus;
  map<string, TH2D*> hZDC_AsymmVRelBxMinusZoom;
  map<string, TH2D*> hZDC_AsymmNormVRelBxMinusZoom;
  map<string, TH2D*> hZDC_AsymmVRelBxAbs;
  map<string, TH2D*> hZDC_AsymmNormVRelBxAbs;
  map<string, TH2D*> hZDC_AsymmVRelBxAbsZoom;
  map<string, TH2D*> hZDC_AsymmNormVRelBxAbsZoom;
  map<string, TH2D*> hZDCAsymCorr;
  map<string, TH2D*> hZDCAsymCorrJet;

  int maxZDCESum = 1024;
  if(isEmptyBunches) maxZDCESum = 512;

  int histPos = 0;
  for(auto const & trig : l1Map){
    l1MapCounter[trig.first] = 0;

    hZDCP_Emu[trig.first] = new TH1D(("hZDCP_Emu_" + trig.first).c_str(), "ZDC Plus Emulated", 100, 0, maxZDCESum);
    hZDCM_Emu[trig.first] = new TH1D(("hZDCM_Emu_" + trig.first).c_str(), "ZDC Minus Emulated", 100, 0, maxZDCESum);
    hZDCP_withTrig[trig.first] = new TH1D(("hZDCP_withTrig_" + trig.first).c_str(), "ZDC Plus", 100, 0, 15000);
    hZDCM_withTrig[trig.first] = new TH1D(("hZDCM_withTrig_" + trig.first).c_str(), "ZDC Minus", 100, 0, 15000);
    hZDCP[trig.first] = new TH1D(("hZDCP_" + trig.first).c_str(), "ZDC Plus", 100, 0, 15000);
    hZDCM[trig.first] = new TH1D(("hZDCM_" + trig.first).c_str(), "ZDC Minus", 100, 0, 15000);
    hZDCP_EmuScaled[trig.first] = new TH1D(("hZDCP_EmuScaled_" + trig.first).c_str(), "ZDC Plus", 100, 0, 15000);
    hZDCM_EmuScaled[trig.first] = new TH1D(("hZDCM_EmuScaled_" + trig.first).c_str(), "ZDC Minus", 100, 0, 15000);
    hZDC_Asymm[trig.first] = new TH1D(("Asymm_" + trig.first).c_str(), "", 50, -300, 300);
    hZDC_AsymmNorm[trig.first] = new TH1D(("AsymmNorm_" + trig.first).c_str(), "", 20, -2, 2);
    hZDC_AsymmVBx[trig.first] = new TH2D(("AsymmVBx_" + trig.first).c_str(), "", 50, -300, 300, 100, -0.5, 3599.5);
    hZDC_AsymmNormVBx[trig.first] = new TH2D(("AsymmNormVBx_" + trig.first).c_str(), "", 20, -2, 2, 100, -0.5, 3599.5);

    if(doBxMap){
      int nBinsRel = maxDeltaBxPlus;
      while(nBinsRel > 50){
	if(nBinsRel%2 != 0) ++nBinsRel;
	nBinsRel /= 2;
      }

      hZDC_AsymmVRelBxPlus[trig.first] = new TH2D(("AsymmVRelBxPlus_" + trig.first).c_str(), "", 50, -300, 300, nBinsRel, 0, maxDeltaBxPlus);
      hZDC_AsymmNormVRelBxPlus[trig.first] = new TH2D(("AsymmNormVRelBxPlus_" + trig.first).c_str(), "", 20, -2, 2, nBinsRel, 0, maxDeltaBxPlus);

      hZDC_AsymmVRelBxPlusZoom[trig.first] = new TH2D(("AsymmVRelBxPlusZoom_" + trig.first).c_str(), "", 50, -300, 300, 20, -0.5, 19.5);
      hZDC_AsymmNormVRelBxPlusZoom[trig.first] = new TH2D(("AsymmNormVRelBxPlusZoom_" + trig.first).c_str(), "", 20, -2, 2, 20, -0.5, 19.5);

      //Define a reasonable coarseness to your binning
      nBinsRel = TMath::Abs(maxDeltaBxMinus);
      while(nBinsRel > 100){
	if(nBinsRel%2 != 0) ++nBinsRel;
	nBinsRel /= 2;
      }

      hZDC_AsymmVRelBxMinus[trig.first] = new TH2D(("AsymmVRelBxMinus_" + trig.first).c_str(), "", 50, -300, 300, nBinsRel, maxDeltaBxMinus, 0);
      hZDC_AsymmNormVRelBxMinus[trig.first] = new TH2D(("AsymmNormVRelBxMinus_" + trig.first).c_str(), "", 20, -2, 2, nBinsRel, maxDeltaBxMinus, 0);

      hZDC_AsymmVRelBxMinusZoom[trig.first] = new TH2D(("AsymmVRelBxMinusZoom_" + trig.first).c_str(), "", 50, -300, 300, 20, -19.5, 0.5);
      hZDC_AsymmNormVRelBxMinusZoom[trig.first] = new TH2D(("AsymmNormVRelBxMinusZoom_" + trig.first).c_str(), "", 20, -2, 2, 20, -19.5, 0.5);

      hZDC_AsymmVRelBxAbs[trig.first] = new TH2D(("AsymmVRelBxAbs_" + trig.first).c_str(), "", 50, -300, 300, nBinsRel, 0, maxDeltaBxPlus);
      hZDC_AsymmNormVRelBxAbs[trig.first] = new TH2D(("AsymmNormVRelBxAbs_" + trig.first).c_str(), "", 20, -2, 2, nBinsRel, 0, maxDeltaBxPlus);

      hZDC_AsymmVRelBxAbsZoom[trig.first] = new TH2D(("AsymmVRelBxAbsZoom_" + trig.first).c_str(), "", 50, -300, 300, 20, -0.5, 19.5);
      hZDC_AsymmNormVRelBxAbsZoom[trig.first] = new TH2D(("AsymmNormVRelBxAbsZoom_" + trig.first).c_str(), "", 20, -2, 2, 20, -0.5, 19.5);
    }

    hZDCAsymCorr[trig.first] = new TH2D(("AsymmCorr_" + trig.first).c_str(), "",  50, 0, 300,  50, 0, 300);
    hZDCAsymCorrJet[trig.first] = new TH2D(("AsymmCorrJet_" + trig.first).c_str(), "",  40, -5.0, 5.0,40, -2, 2);
  }

  //Counter to not spam i/o w/ json skipping messages
  map<int, int> runJSONMessage;

  uint minBx = 100000;
  uint maxBx = 0;

  Long64_t totalEvents = emuReader.GetEntries(true);
  for (Long64_t i = 0; i < totalEvents; i++) {
    emuReader.Next(); emuTrigReader.Next(); zdcReader.Next(); unpackerReader.Next(); eventReader.Next();
    // zero bias

    if (i % 10000 == 0) {
      cout << "Event " << i << endl;
    }

    //correct the bx value; in OMS range is 0-3563, in L1EventTree 1-3564
    //We will opt to match OMS maps, so 3564 -> 0
    uint bxCorr = (*bx);
    if(bxCorr == 3564) bxCorr = 0;
    if(bxCorr < minBx) minBx = bxCorr;
    if(bxCorr > maxBx) maxBx = bxCorr;

    map<string, int> trigDecisionMap;

    bool goodTrigger = false;
    for(auto const & trig : l1Map){
      trigDecisionMap[trig.first] = (*initialTrigDecision)[trig.second];
      if(trigDecisionMap[trig.first] == 1){
	goodTrigger = true;
	++(l1MapCounter[trig.first]);
      }
    }
    if(!goodTrigger) continue;

    //Tweak applyJSON, except where we are making exceptions for special purposes
    if(*runNumber == 399843){
      if(runJSONMessage.count(399843) == 0) cout << "Not applying JSON to run 399843 (tracker in bad state, good for ZDC testing still)" << endl;
      applyJSON = false; //tracker out fill, we make an exception to test empty bunches
      runJSONMessage[399843] = 1;

      //we apply our own selection based on OMS inspection, up to LS 1172
      //https://cmsoms.cern.ch/cms/triggers/l1_rates?cms_run=399843
      if(*lumiSection > 1172) continue;
    }
    else if(isCosmics) applyJSON = false;//this isn't relevant for your testing

    if(applyJSON){
      if(!dcs.isGood(*runNumber,*lumiSection)) continue;
    }


    //cout << *nSums << endl;
    int unpackerSumPlus = -1;
    int unpackerSumMinus = -1;
    for(int j = 0; j < *nSums; j++){
      if((*unpackerType)[j] == 27 && (*unpackerBx)[j] == 0){
	for(auto const & trigVal : trigDecisionMap){
	  if(trigVal.second) hZDCP_Emu[trigVal.first]->Fill((*unpackerSum)[j]);
	}
	unpackerSumPlus = (*unpackerSum)[j];
      }
      else if((*unpackerType)[j] == 28 && (*unpackerBx)[j] == 0) {
	for(auto const & trigVal : trigDecisionMap){
	  if(trigVal.second) hZDCM_Emu[trigVal.first]->Fill((*unpackerSum)[j]);
	}
	unpackerSumMinus = (*unpackerSum)[j];
      }
    }
//cout << "Run: " <<  *runNumber << " lumi: " << *lumiSection << " event: " <<  *eventNumber << " sumZDCp: " << unpackerSumPlus << " sumZDCm: " << unpackerSumMinus << endl;

    // float hfp = -1.0;
    // float hfm = -1.0;
    // for(int j = 0; j < *nEtSums; j++){
    //   //cout << "Sum type: " << (*unpackerEtSumType)[j] << endl;
    //     if((*unpackerEtSumType)[j] == 11){
    //         hfp = (*unpackerEtSum)[j];
    //     }
    //     if((*unpackerEtSumType)[j] == 12) {
    //         cout << " (*unpackerEtSumBx)[j]: " <<  (*unpackerEtSumBx)[j] << " (*unpackerEtSum)[j]: " <<  (*unpackerEtSum)[j] << endl;
    //         hfm = (*unpackerEtSum)[j];
    //     }
    // }
    // cout << "plus: " << hfp << " minus: " << hfm << endl;

    float emuMinus = (*unpackerSum)[4]*2;
    float emuPlus = (*unpackerSum)[5]*2;

    //cout << "emuPlus: " << emuPlus << " sumPlus: " << *sumPlus << endl;

    for(auto const & trigVal : trigDecisionMap){
      if(trigVal.second){
	if(emuPlus > 40)hZDCP_withTrig[trigVal.first]->Fill(*sumPlus);
	if(emuMinus> 40)hZDCM_withTrig[trigVal.first]->Fill(*sumMinus);

	hZDCM_EmuScaled[trigVal.first]->Fill(emuMinus*16);
	hZDCP_EmuScaled[trigVal.first]->Fill(emuPlus*16);
      }
    }

    double maxJetEt = -1.0;
    for(int j = 0; j < *nJets; j++){
      if((*jetEt)[j] > 20){
	for(auto const & trigVal : trigDecisionMap){
	  if(trigVal.second) hZDCAsymCorrJet[trigVal.first]->Fill((*jetEta)[j],float(unpackerSumPlus - unpackerSumMinus)/(unpackerSumPlus + unpackerSumMinus));
	}
      }
    }
    for(auto const & trigVal : trigDecisionMap){
      if(trigVal.second){
	bool isAboveThresh = unpackerSumPlus > 50.0 || unpackerSumMinus > 50.0;

	hZDC_Asymm[trigVal.first]->Fill(float(unpackerSumPlus - unpackerSumMinus));
	if(isAboveThresh) hZDC_AsymmNorm[trigVal.first]->Fill(float(unpackerSumPlus - unpackerSumMinus)/(unpackerSumPlus + unpackerSumMinus));

	hZDC_AsymmVBx[trigVal.first]->Fill(float(unpackerSumPlus - unpackerSumMinus), bxCorr);
	if(isAboveThresh) hZDC_AsymmNormVBx[trigVal.first]->Fill(float(unpackerSumPlus - unpackerSumMinus)/(unpackerSumPlus + unpackerSumMinus), bxCorr);

	if(doBxMap){
	  int relBxPlus = absToRelBxPlusMap[bxCorr];
	  int relBxMinus = absToRelBxMinusMap[bxCorr];
	  int relBxAbs = TMath::Min(relBxPlus, TMath::Abs(relBxMinus));

	  if(relBxPlus == 0){
	    cout << "File, run, lumi, evt: " << hiEventChain.GetFile()->GetName() << ", " << *runNumber << ", " << *lumiSection << ", " << *eventNumber << endl;

	    cout << " Relbxplus 0, absbx: " << bxCorr << endl;
	  }

	  if(relBxMinus == 0){
	    cout << "File, run, lumi, evt: " << hiEventChain.GetFile()->GetName() << ", " << *runNumber << ", " << *lumiSection << ", " << *eventNumber << endl;

	    cout << " Relbxminus 0, absbx: " << bxCorr << endl;
	  }

	  hZDC_AsymmVRelBxPlus[trigVal.first]->Fill(float(unpackerSumPlus - unpackerSumMinus), relBxPlus);
	  if(isAboveThresh) hZDC_AsymmNormVRelBxPlus[trigVal.first]->Fill(float(unpackerSumPlus - unpackerSumMinus)/(unpackerSumPlus + unpackerSumMinus), relBxPlus);

	  hZDC_AsymmVRelBxPlusZoom[trigVal.first]->Fill(float(unpackerSumPlus - unpackerSumMinus), relBxPlus);
	  if(isAboveThresh) hZDC_AsymmNormVRelBxPlusZoom[trigVal.first]->Fill(float(unpackerSumPlus - unpackerSumMinus)/(unpackerSumPlus + unpackerSumMinus), relBxPlus);

	  hZDC_AsymmVRelBxMinus[trigVal.first]->Fill(float(unpackerSumPlus - unpackerSumMinus), relBxMinus);
	  if(isAboveThresh) hZDC_AsymmNormVRelBxMinus[trigVal.first]->Fill(float(unpackerSumPlus - unpackerSumMinus)/(unpackerSumPlus + unpackerSumMinus), relBxMinus);

	  hZDC_AsymmVRelBxMinusZoom[trigVal.first]->Fill(float(unpackerSumPlus - unpackerSumMinus), relBxMinus);
	  if(isAboveThresh) hZDC_AsymmNormVRelBxMinusZoom[trigVal.first]->Fill(float(unpackerSumPlus - unpackerSumMinus)/(unpackerSumPlus + unpackerSumMinus), relBxMinus);

	  hZDC_AsymmVRelBxAbs[trigVal.first]->Fill(float(unpackerSumPlus - unpackerSumMinus), relBxAbs);
	  if(isAboveThresh) hZDC_AsymmNormVRelBxAbs[trigVal.first]->Fill(float(unpackerSumPlus - unpackerSumMinus)/(unpackerSumPlus + unpackerSumMinus), relBxAbs);

	  hZDC_AsymmVRelBxAbsZoom[trigVal.first]->Fill(float(unpackerSumPlus - unpackerSumMinus), relBxAbs);
	  if(isAboveThresh) hZDC_AsymmNormVRelBxAbsZoom[trigVal.first]->Fill(float(unpackerSumPlus - unpackerSumMinus)/(unpackerSumPlus + unpackerSumMinus), relBxAbs);
	}

	hZDCAsymCorr[trigVal.first]->Fill(unpackerSumPlus, unpackerSumMinus);

	hZDCP[trigVal.first]->Fill(*sumPlus);
	hZDCM[trigVal.first]->Fill(*sumMinus);
      }
    }

  } // end loop over  the  number of events

  std::cout << "L" << __LINE__ << std::endl;
  //For loop over the trigger set
  for(auto const & trig : l1Map){
    //Check you had fills, if no it segfaults
    if(l1MapCounter[trig.first] == 0){
      cout << "Skipping plotting '" << trig.first << "', no counts" << endl;
      continue;
    }
    else{
      cout << "Plotting for " << trig.first << ", nEvt=" << l1MapCounter[trig.first] << endl;
    }
    // ------------------------------------------
    /* ZDC Emu */
    // ------------------------------------------

    TLatex* trigTex = new TLatex(xLatex,topY-0.06,trig.first.c_str());
    trigTex->SetNDC();
    trigTex->SetTextSize(0.035);
    trigTex->SetTextFont(42);

    TLegend* leg = new TLegend(0.6,0.7,0.8,0.85);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.045);

    TCanvas* c = new TCanvas("c","c",800,600);
    c->cd();
    c->SetTickx(1);
    c->SetTicky(1);
    c->SetLogy();
    c->SetTopMargin(0.09);
    c->SetBottomMargin(0.11);
    c->SetLeftMargin(0.09);
    c->SetRightMargin(0.05);

    // zdc plus
    hZDCP_Emu[trig.first]->SetLineColor(kRed);
    hZDCP_Emu[trig.first]->SetLineWidth(2);
    hZDCP_Emu[trig.first]->GetXaxis()->SetTitleSize(0.05);
    hZDCP_Emu[trig.first]->SetMarkerStyle(20);
    hZDCP_Emu[trig.first]->GetXaxis()->SetTitle("ZDC L1 Energy Sum");
    //hZDCM_Emu->GetXaxis()->SetRangeUser(0, 200);

    // zdc minus
    hZDCM_Emu[trig.first]->SetLineColor(kBlue);
    hZDCM_Emu[trig.first]->SetLineWidth(2);
    hZDCM_Emu[trig.first]->SetMarkerStyle(20);
    hZDCM_Emu[trig.first]->GetXaxis()->SetTitle("ZDC L1 Energy Sum");
    hZDCM_Emu[trig.first]->GetXaxis()->SetTitleSize(0.05);
    //hZDCM_Emu[trig.first]->GetXaxis()->SetRangeUser(0, 200);
    hZDCM_Emu[trig.first]->Draw();
    hZDCP_Emu[trig.first]->Draw("same");
    leg->AddEntry(hZDCP_Emu[trig.first],"ZDC Plus","l");
    leg->AddEntry(hZDCM_Emu[trig.first],"ZDC Minus","l");
    leg->Draw("same");
    cms->Draw("same");
    lumi->Draw("same");
    trigTex->Draw("same");
    for(auto const & extStr : extensions){
      c->SaveAs(Form("%s/Online_ZDCEmu_%s_%s.%s", outDirPlot.c_str(),trig.first.c_str(), tag.c_str(), extStr.c_str()));
    }

    // ------------------------------------------
    /* ZDC Emu Ratio */
    // ------------------------------------------

    TCanvas* c12 = new TCanvas("c12","c12",800,600);
    c12->cd();
    c12->SetTickx(1);
    c12->SetTicky(1);
    // c12->SetLogy();
    c12->SetTopMargin(0.09);
    c12->SetBottomMargin(0.11);
    c12->SetLeftMargin(0.09);
    c12->SetRightMargin(0.05);

    TLine* l = new TLine(0, 1.0, 1023, 1.0);
    l->SetLineStyle(2);
    l->SetLineColor(kGray+2);
    // zdc plus
    TH1D* ratioPlus = (TH1D*)hZDCP_Emu[trig.first]->Clone("ratioPlus");
    ratioPlus->Divide(hZDCM_Emu[trig.first]);
    ratioPlus->GetYaxis()->SetTitle("Ratio of Plus / Minus");
    ratioPlus->GetYaxis()->SetRangeUser(0.0,3.0);

    ratioPlus->Draw();
    l->Draw();
    leg->Draw("same");
    cms->Draw("same");
    lumi->Draw("same");
    trigTex->Draw("same");
    for(auto const & extStr : extensions){
      c12->SaveAs(Form("%s/Online_ZDCEmuRatio_%s_%s.%s", outDirPlot.c_str(),trig.first.c_str(), tag.c_str(), extStr.c_str()));
    }

    // ------------------------------------------
    /* ZDCCorr*/
    // ------------------------------------------

    TCanvas* cC = new TCanvas("cC","cC",700,600);
    cC->cd();
    cC->SetTickx(1);
    cC->SetTicky(1);
    cC->SetLogz(1);
    cC->SetTopMargin(0.09);
    cC->SetBottomMargin(0.11);
    cC->SetLeftMargin(0.11);
    cC->SetRightMargin(0.11);

    // zdc plus
    hZDCAsymCorr[trig.first]->GetXaxis()->SetTitle("ZDCp L1 sum");
    hZDCAsymCorr[trig.first]->GetYaxis()->SetTitle("ZDCm L1 sum");

    hZDCAsymCorr[trig.first]->Draw("colz");
    // asymm->Draw("same");
    //leg->Draw("same");
    cms->Draw("same");
    lumi->Draw("same");
    trigTex->Draw("same");
    for(auto const & extStr : extensions){
      cC->SaveAs(Form("%s/Online_CorrAsymm_%s_%s.%s", outDirPlot.c_str(), trig.first.c_str(), tag.c_str(), extStr.c_str()));
    }

    // ------------------------------------------
    /* ZDCCorrJet*/
    // ------------------------------------------

    TCanvas* cCj = new TCanvas("cCj","cCj",700,600);
    cCj->cd();
    cCj->SetTickx(1);
    cCj->SetTicky(1);
    //cCj->SetLogz(1);
    cCj->SetTopMargin(0.09);
    cCj->SetBottomMargin(0.11);
    cCj->SetLeftMargin(0.15);
    cCj->SetRightMargin(0.11);

    // zdc plus
    hZDCAsymCorrJet[trig.first]->GetYaxis()->SetTitle("(ZDCp - ZDCm)/(ZDCp + ZDCm)");
    hZDCAsymCorrJet[trig.first]->GetXaxis()->SetTitle("Jet Eta");

    hZDCAsymCorrJet[trig.first]->Draw("colz");
    // asymm->Draw("same");
    //leg->Draw("same");
    cms->Draw("same");
    lumi->Draw("same");
    trigTex->Draw("same");
    for(auto const & extStr : extensions){
      cCj->SaveAs(Form("%s/Online_CorrAsymmJet_%s_%s.%s",outDirPlot.c_str(),trig.first.c_str(), tag.c_str(), extStr.c_str()));
    }

    // ------------------------------------------
    /* ZDCAsymm */
    // ------------------------------------------

    TCanvas* cA = new TCanvas("cA","cA",700,600);
    cA->cd();
    cA->SetTickx(1);
    cA->SetTicky(1);
    //cA->SetLogy();
    cA->SetTopMargin(0.09);
    cA->SetBottomMargin(0.11);
    cA->SetLeftMargin(0.09);
    cA->SetRightMargin(0.05);

    TLatex* asymm = new TLatex(xLatex,topY-0.06*2,Form("#mu : %0.2f", hZDC_Asymm[trig.first]->GetMean()));
    asymm->SetNDC();
    asymm->SetTextSize(0.035);
    asymm->SetTextFont(42);

    // zdc plus
    hZDC_Asymm[trig.first]->SetLineColor(kRed);
    hZDC_Asymm[trig.first]->SetLineWidth(2);
    hZDC_Asymm[trig.first]->GetXaxis()->SetTitleSize(0.05);
    hZDC_Asymm[trig.first]->SetMarkerStyle(20);
    hZDC_Asymm[trig.first]->GetXaxis()->SetTitle("(ZDCp - ZDCm)");

    hZDC_Asymm[trig.first]->Draw();
    asymm->Draw("same");
    lumi->Draw("same");
    trigTex->Draw("same");
    //leg->Draw("same");
    cms->Draw("same");
    for(auto const & extStr : extensions){
      cA->SaveAs(Form("%s/Online_Asymm_%s_%s.%s", outDirPlot.c_str(),trig.first.c_str(), tag.c_str(), extStr.c_str()));
    }


    // ------------------------------------------
    /* ZDCAsymmVBx */
    // ------------------------------------------

    TCanvas* cAVB = new TCanvas("cAVB","cAVB",700,600);
    cAVB->cd();
    cAVB->SetTickx(1);
    cAVB->SetTicky(1);
    //cAVB->SetLogy(s);
    cAVB->SetTopMargin(0.09);
    cAVB->SetBottomMargin(0.11);
    cAVB->SetLeftMargin(0.11);
    cAVB->SetRightMargin(0.11);

    // zdc plus
    hZDC_AsymmVBx[trig.first]->GetXaxis()->SetTitleSize(0.05);
    hZDC_AsymmVBx[trig.first]->GetXaxis()->SetTitle("(ZDCp - ZDCm)");
    hZDC_AsymmVBx[trig.first]->GetYaxis()->SetTitle("Bunch #");

    hZDC_AsymmVBx[trig.first]->Draw("COLZ");
    lumi->Draw("same");
    trigTex->Draw("same");
    //leg->Draw("same");
    cms->Draw("same");
    for(auto const & extStr : extensions){
      cAVB->SaveAs(Form("%s/Online_AsymmVBx_%s_%s.%s",outDirPlot.c_str(), trig.first.c_str(), tag.c_str(), extStr.c_str()));
    }

    // ------------------------------------------
    /* ZDCAsymmVRelBxPlus */
    // ------------------------------------------

    if(doBxMap){
      TCanvas* cAVRB = new TCanvas("cAVRB","cAVRB",700,600);
      cAVRB->cd();
      cAVRB->SetTickx(1);
      cAVRB->SetTicky(1);
      //cAVRB->SetLogy(s);
      cAVRB->SetTopMargin(0.09);
      cAVRB->SetBottomMargin(0.11);
      cAVRB->SetLeftMargin(0.11);
      cAVRB->SetRightMargin(0.11);

      // zdc plus
      hZDC_AsymmVRelBxPlus[trig.first]->GetXaxis()->SetTitleSize(0.05);
      hZDC_AsymmVRelBxPlus[trig.first]->GetXaxis()->SetTitle("(ZDCp - ZDCm)");
      hZDC_AsymmVRelBxPlus[trig.first]->GetYaxis()->SetTitle("Bunch # Behind Filled Bunch");

      hZDC_AsymmVRelBxPlus[trig.first]->Draw("COLZ");
      lumi->Draw("same");
      trigTex->Draw("same");
      //leg->Draw("same");
      cms->Draw("same");
      for(auto const & extStr : extensions){
	cAVRB->SaveAs(Form("%s/Online_AsymmVRelBxPlus_%s_%s.%s",outDirPlot.c_str(), trig.first.c_str(), tag.c_str(), extStr.c_str()));
      }
      delete cAVRB;
    }

    // ------------------------------------------
    /* ZDCAsymmVRelBxPlusZoom */
    // ------------------------------------------

    if(doBxMap){
      TCanvas* cAVRB = new TCanvas("cAVRB","cAVRB",700,600);
      cAVRB->cd();
      cAVRB->SetTickx(1);
      cAVRB->SetTicky(1);
      //cAVRB->SetLogy(s);
      cAVRB->SetTopMargin(0.09);
      cAVRB->SetBottomMargin(0.11);
      cAVRB->SetLeftMargin(0.11);
      cAVRB->SetRightMargin(0.11);

      // zdc plus
      hZDC_AsymmVRelBxPlusZoom[trig.first]->GetXaxis()->SetTitleSize(0.05);
      hZDC_AsymmVRelBxPlusZoom[trig.first]->GetXaxis()->SetTitle("(ZDCp - ZDCm)");
      hZDC_AsymmVRelBxPlusZoom[trig.first]->GetYaxis()->SetTitle("Bunch # Behind Filled Bunch");

      hZDC_AsymmVRelBxPlusZoom[trig.first]->Draw("COLZ");
      lumi->Draw("same");
      trigTex->Draw("same");
      //leg->Draw("same");
      cms->Draw("same");
      for(auto const & extStr : extensions){
	cAVRB->SaveAs(Form("%s/Online_AsymmVRelBxPlusZoom_%s_%s.%s",outDirPlot.c_str(), trig.first.c_str(), tag.c_str(), extStr.c_str()));
      }
      delete cAVRB;
    }

    // ------------------------------------------
    /* ZDCAsymmVRelBxMinus */
    // ------------------------------------------

    if(doBxMap){
      TCanvas* cAVRB = new TCanvas("cAVRB","cAVRB",700,600);
      cAVRB->cd();
      cAVRB->SetTickx(1);
      cAVRB->SetTicky(1);
      //cAVRB->SetLogy(s);
      cAVRB->SetTopMargin(0.09);
      cAVRB->SetBottomMargin(0.11);
      cAVRB->SetLeftMargin(0.11);
      cAVRB->SetRightMargin(0.11);

      // zdc plus
      hZDC_AsymmVRelBxMinus[trig.first]->GetXaxis()->SetTitleSize(0.05);
      hZDC_AsymmVRelBxMinus[trig.first]->GetXaxis()->SetTitle("(ZDCp - ZDCm)");
      hZDC_AsymmVRelBxMinus[trig.first]->GetYaxis()->SetTitle("Bunch # Ahead Filled Bunch");

      hZDC_AsymmVRelBxMinus[trig.first]->Draw("COLZ");

      lumi->DrawLatex(xLatex, bottomY, lumi->GetTitle());
      trigTex->DrawLatex(xLatex, bottomY-0.06, trigTex->GetTitle());

      //leg->Draw("same");
      cms->Draw("same");
      for(auto const & extStr : extensions){
	cAVRB->SaveAs(Form("%s/Online_AsymmVRelBxMinus_%s_%s.%s", outDirPlot.c_str(),trig.first.c_str(), tag.c_str(), extStr.c_str()));
      }
      delete cAVRB;
    }

    // ------------------------------------------
    /* ZDCAsymmVRelBxMinusZoom */
    // ------------------------------------------

    if(doBxMap){
      TCanvas* cAVRB = new TCanvas("cAVRB","cAVRB",700,600);
      cAVRB->cd();
      cAVRB->SetTickx(1);
      cAVRB->SetTicky(1);
      //cAVRB->SetLogy(s);
      cAVRB->SetTopMargin(0.09);
      cAVRB->SetBottomMargin(0.11);
      cAVRB->SetLeftMargin(0.11);
      cAVRB->SetRightMargin(0.11);

      // zdc plus
      hZDC_AsymmVRelBxMinusZoom[trig.first]->GetXaxis()->SetTitleSize(0.05);
      hZDC_AsymmVRelBxMinusZoom[trig.first]->GetXaxis()->SetTitle("(ZDCp - ZDCm)");
      hZDC_AsymmVRelBxMinusZoom[trig.first]->GetYaxis()->SetTitle("Bunch # Ahead Filled Bunch");

      hZDC_AsymmVRelBxMinusZoom[trig.first]->Draw("COLZ");

      lumi->DrawLatex(xLatex, bottomY, lumi->GetTitle());
      trigTex->DrawLatex(xLatex, bottomY-0.06, trigTex->GetTitle());

      //leg->Draw("same");
      cms->Draw("same");
      for(auto const & extStr : extensions){
	cAVRB->SaveAs(Form("%s/Online_AsymmVRelBxMinusZoom_%s_%s.%s",outDirPlot.c_str(), trig.first.c_str(), tag.c_str(), extStr.c_str()));
      }
      delete cAVRB;
    }

    // ------------------------------------------
    /* ZDCAsymmVRelBxAbs */
    // ------------------------------------------

    if(doBxMap){
      TCanvas* cAVRB = new TCanvas("cAVRB","cAVRB",700,600);
      cAVRB->cd();
      cAVRB->SetTickx(1);
      cAVRB->SetTicky(1);
      //cAVRB->SetLogy(s);
      cAVRB->SetTopMargin(0.09);
      cAVRB->SetBottomMargin(0.11);
      cAVRB->SetLeftMargin(0.11);
      cAVRB->SetRightMargin(0.11);

      // zdc plus
      hZDC_AsymmVRelBxAbs[trig.first]->GetXaxis()->SetTitleSize(0.05);
      hZDC_AsymmVRelBxAbs[trig.first]->GetXaxis()->SetTitle("(ZDCp - ZDCm)");
      hZDC_AsymmVRelBxAbs[trig.first]->GetYaxis()->SetTitle("Bunch # To Closest Filled Bunch");

      hZDC_AsymmVRelBxAbs[trig.first]->Draw("COLZ");

      lumi->Draw("same");
      trigTex->Draw("same");

      //leg->Draw("same");
      cms->Draw("same");
      for(auto const & extStr : extensions){
	cAVRB->SaveAs(Form("%s/Online_AsymmVRelBxAbs_%s_%s.%s",outDirPlot.c_str(), trig.first.c_str(), tag.c_str(), extStr.c_str()));
      }
      delete cAVRB;
    }

    // ------------------------------------------
    /* ZDCAsymmVRelBxAbsZoom */
    // ------------------------------------------

    if(doBxMap){
      TCanvas* cAVRB = new TCanvas("cAVRB","cAVRB",700,600);
      cAVRB->cd();
      cAVRB->SetTickx(1);
      cAVRB->SetTicky(1);
      //cAVRB->SetLogy(s);
      cAVRB->SetTopMargin(0.09);
      cAVRB->SetBottomMargin(0.11);
      cAVRB->SetLeftMargin(0.11);
      cAVRB->SetRightMargin(0.11);

      // zdc plus
      hZDC_AsymmVRelBxAbsZoom[trig.first]->GetXaxis()->SetTitleSize(0.05);
      hZDC_AsymmVRelBxAbsZoom[trig.first]->GetXaxis()->SetTitle("(ZDCp - ZDCm)");
      hZDC_AsymmVRelBxAbsZoom[trig.first]->GetYaxis()->SetTitle("Bunch # To Closest Filled Bunch");

      hZDC_AsymmVRelBxAbsZoom[trig.first]->Draw("COLZ");

      lumi->Draw("same");
      trigTex->Draw("same");

      //leg->Draw("same");
      cms->Draw("same");
      for(auto const & extStr : extensions){
	cAVRB->SaveAs(Form("%s/Online_AsymmVRelBxAbsZoom_%s_%s.%s",outDirPlot.c_str(), trig.first.c_str(), tag.c_str(), extStr.c_str()));
      }
      delete cAVRB;
    }


    // ------------------------------------------
    /* ZDCAsymmNorm */
    // ------------------------------------------

    TCanvas* cAN = new TCanvas("cAN","cAN",700,600);
    cAN->cd();
    cAN->SetTickx(1);
    cAN->SetTicky(1);
    //cAN->SetLogy();
    cAN->SetTopMargin(0.09);
    cAN->SetBottomMargin(0.11);
    cAN->SetLeftMargin(0.09);
    cAN->SetRightMargin(0.05);

    TLatex* asymmNorm = new TLatex(xLatex,topY - 0.06*3,Form("#mu : %0.2f", hZDC_AsymmNorm[trig.first]->GetMean()));
    asymmNorm->SetNDC();
    asymmNorm->SetTextSize(0.035);
    asymmNorm->SetTextFont(42);

    TLatex* cutAsymmNorm = new TLatex(xLatex,topY - 0.06*2,Form("ZDCp > 50 || ZDCm > 50"));
    cutAsymmNorm->SetNDC();
    cutAsymmNorm->SetTextSize(0.035);
    cutAsymmNorm->SetTextFont(42);

    // zdc plus
    hZDC_AsymmNorm[trig.first]->SetLineColor(kRed);
    hZDC_AsymmNorm[trig.first]->SetLineWidth(2);
    hZDC_AsymmNorm[trig.first]->GetXaxis()->SetTitleSize(0.05);
    hZDC_AsymmNorm[trig.first]->SetMarkerStyle(20);
    hZDC_AsymmNorm[trig.first]->GetXaxis()->SetTitle("(ZDCp - ZDCm)/(ZDCp + ZDCm)");

    hZDC_AsymmNorm[trig.first]->Draw();
    asymmNorm->Draw("same");
    cutAsymmNorm->Draw("same");
    lumi->Draw("same");
    trigTex->Draw("same");
    //leg->Draw("same");
    cms->Draw("same");
    for(auto const & extStr : extensions){
      cAN->SaveAs(Form("%s/Online_AsymmNorm_%s_%s.%s", outDirPlot.c_str(),trig.first.c_str(), tag.c_str(), extStr.c_str()));
    }

    // ------------------------------------------
    /* ZDCAsymmVBx */
    // ------------------------------------------

    TCanvas* cANVB = new TCanvas("cANVB","cANVB",700,600);
    cANVB->cd();
    cANVB->SetTickx(1);
    cANVB->SetTicky(1);
    //cANVB->SetLogy(s);
    cANVB->SetTopMargin(0.09);
    cANVB->SetBottomMargin(0.11);
    cANVB->SetLeftMargin(0.11);
    cANVB->SetRightMargin(0.11);

    // zdc plus
    hZDC_AsymmNormVBx[trig.first]->GetXaxis()->SetTitleSize(0.05);
    hZDC_AsymmNormVBx[trig.first]->GetXaxis()->SetTitle("(ZDCp - ZDCm)/(ZDCp + ZDCm)");
    hZDC_AsymmNormVBx[trig.first]->GetYaxis()->SetTitle("Bunch #");

    hZDC_AsymmNormVBx[trig.first]->Draw("COLZ");
    lumi->Draw("same");
    trigTex->Draw("same");
    cutAsymmNorm->Draw("same");
    //leg->Draw("same");
    cms->Draw("same");
    for(auto const & extStr : extensions){
      cANVB->SaveAs(Form("%s/Online_AsymmNormVBx_%s_%s.%s",outDirPlot.c_str(), trig.first.c_str(), tag.c_str(), extStr.c_str()));
    }


    // ------------------------------------------
    /* ZDCAsymmNormVRelBxPlus */
    // ------------------------------------------

    if(doBxMap){
      TCanvas* cANVRB = new TCanvas("cANVRB","cANVRB",700,600);
      cANVRB->cd();
      cANVRB->SetTickx(1);
      cANVRB->SetTicky(1);
      //cANVRB->SetLogy(s);
      cANVRB->SetTopMargin(0.09);
      cANVRB->SetBottomMargin(0.11);
      cANVRB->SetLeftMargin(0.11);
      cANVRB->SetRightMargin(0.11);

      // zdc plus
      hZDC_AsymmNormVRelBxPlus[trig.first]->GetXaxis()->SetTitleSize(0.05);
      hZDC_AsymmNormVRelBxPlus[trig.first]->GetXaxis()->SetTitle("(ZDCp - ZDCm)/(ZDCp + ZDCm)");
      hZDC_AsymmNormVRelBxPlus[trig.first]->GetYaxis()->SetTitle("Bunch # Behind Filled Bunch");

      hZDC_AsymmNormVRelBxPlus[trig.first]->Draw("COLZ");
      lumi->Draw("same");
      trigTex->Draw("same");
      cutAsymmNorm->Draw("same");
      //leg->Draw("same");
      cms->Draw("same");
      for(auto const & extStr : extensions){
	cANVRB->SaveAs(Form("%s/Online_AsymmNormVRelBxPlus_%s_%s.%s", outDirPlot.c_str(),trig.first.c_str(), tag.c_str(), extStr.c_str()));
      }
      delete cANVRB;
    }

    // ------------------------------------------
    /* ZDCAsymmNormVRelBxPlusZoom */
    // ------------------------------------------

    if(doBxMap){
      TCanvas* cANVRB = new TCanvas("cANVRB","cANVRB",700,600);
      cANVRB->cd();
      cANVRB->SetTickx(1);
      cANVRB->SetTicky(1);
      //cANVRB->SetLogy(s);
      cANVRB->SetTopMargin(0.09);
      cANVRB->SetBottomMargin(0.11);
      cANVRB->SetLeftMargin(0.11);
      cANVRB->SetRightMargin(0.11);

      // zdc plus
      hZDC_AsymmNormVRelBxPlusZoom[trig.first]->GetXaxis()->SetTitleSize(0.05);
      hZDC_AsymmNormVRelBxPlusZoom[trig.first]->GetXaxis()->SetTitle("(ZDCp - ZDCm)/(ZDCp + ZDCm)");
      hZDC_AsymmNormVRelBxPlusZoom[trig.first]->GetYaxis()->SetTitle("Bunch # Behind Filled Bunch");

      hZDC_AsymmNormVRelBxPlusZoom[trig.first]->Draw("COLZ");
      lumi->Draw("same");
      trigTex->Draw("same");
      //leg->Draw("same");
      cms->Draw("same");
      cutAsymmNorm->Draw("same");
      for(auto const & extStr : extensions){
	    cANVRB->SaveAs(Form("%s/Online_AsymmNormVRelBxPlusZoom_%s_%s.%s", outDirPlot.c_str(),trig.first.c_str(), tag.c_str(), extStr.c_str()));
      }
      delete cANVRB;
    }

    // ------------------------------------------
    /* ZDCAsymmNormVRelBxMinus */
    // ------------------------------------------

    if(doBxMap){
      TCanvas* cANVRB = new TCanvas("cANVRB","cANVRB",700,600);
      cANVRB->cd();
      cANVRB->SetTickx(1);
      cANVRB->SetTicky(1);
      //cANVRB->SetLogy(s);
      cANVRB->SetTopMargin(0.09);
      cANVRB->SetBottomMargin(0.11);
      cANVRB->SetLeftMargin(0.11);
      cANVRB->SetRightMargin(0.11);

      // zdc plus
      hZDC_AsymmNormVRelBxMinus[trig.first]->GetXaxis()->SetTitleSize(0.05);
      hZDC_AsymmNormVRelBxMinus[trig.first]->GetXaxis()->SetTitle("(ZDCp - ZDCm)/(ZDCp + ZDCm)");
      hZDC_AsymmNormVRelBxMinus[trig.first]->GetYaxis()->SetTitle("Bunch # Ahead Filled Bunch");

      hZDC_AsymmNormVRelBxMinus[trig.first]->Draw("COLZ");

      lumi->DrawLatex(xLatex, bottomY, lumi->GetTitle());
      trigTex->DrawLatex(xLatex, bottomY-0.06, trigTex->GetTitle());
      cutAsymmNorm->DrawLatex(xLatex, bottomY-0.06*2.0, cutAsymmNorm->GetTitle());

      //leg->Draw("same");
      cms->Draw("same");
      for(auto const & extStr : extensions){
	cANVRB->SaveAs(Form("%s/Online_AsymmNormVRelBxMinus_%s_%s.%s",outDirPlot.c_str(), trig.first.c_str(), tag.c_str(), extStr.c_str()));
      }
      delete cANVRB;
    }

    // ------------------------------------------
    /* ZDCAsymmNormVRelBxMinusZoom */
    // ------------------------------------------

    if(doBxMap){
      TCanvas* cANVRB = new TCanvas("cANVRB","cANVRB",700,600);
      cANVRB->cd();
      cANVRB->SetTickx(1);
      cANVRB->SetTicky(1);
      //cANVRB->SetLogy(s);
      cANVRB->SetTopMargin(0.09);
      cANVRB->SetBottomMargin(0.11);
      cANVRB->SetLeftMargin(0.11);
      cANVRB->SetRightMargin(0.11);

      // zdc plus
      hZDC_AsymmNormVRelBxMinusZoom[trig.first]->GetXaxis()->SetTitleSize(0.05);
      hZDC_AsymmNormVRelBxMinusZoom[trig.first]->GetXaxis()->SetTitle("(ZDCp - ZDCm)/(ZDCp + ZDCm)");
      hZDC_AsymmNormVRelBxMinusZoom[trig.first]->GetYaxis()->SetTitle("Bunch # Ahead Filled Bunch");

      hZDC_AsymmNormVRelBxMinusZoom[trig.first]->Draw("COLZ");

      lumi->DrawLatex(xLatex, bottomY, lumi->GetTitle());
      trigTex->DrawLatex(xLatex, bottomY-0.06, trigTex->GetTitle());
      cutAsymmNorm->DrawLatex(xLatex, bottomY-0.06*2.0, cutAsymmNorm->GetTitle());
      cms->Draw("same");

      for(auto const & extStr : extensions){
	cANVRB->SaveAs(Form("%s/Online_AsymmNormVRelBxMinusZoom_%s_%s.%s",outDirPlot.c_str(), trig.first.c_str(), tag.c_str(), extStr.c_str()));
      }
      delete cANVRB;
    }

    // ------------------------------------------
    /* ZDCAsymmNormVRelBxAbs */
    // ------------------------------------------

    if(doBxMap){
      TCanvas* cANVRB = new TCanvas("cANVRB","cANVRB",700,600);
      cANVRB->cd();
      cANVRB->SetTickx(1);
      cANVRB->SetTicky(1);
      //cANVRB->SetLogy(s);
      cANVRB->SetTopMargin(0.09);
      cANVRB->SetBottomMargin(0.11);
      cANVRB->SetLeftMargin(0.11);
      cANVRB->SetRightMargin(0.11);

      // zdc plus
      hZDC_AsymmNormVRelBxAbs[trig.first]->GetXaxis()->SetTitleSize(0.05);
      hZDC_AsymmNormVRelBxAbs[trig.first]->GetXaxis()->SetTitle("(ZDCp - ZDCm)/(ZDCp + ZDCm)");
      hZDC_AsymmNormVRelBxAbs[trig.first]->GetYaxis()->SetTitle("Bunch # To Closest Filled Bunch");

      hZDC_AsymmNormVRelBxAbs[trig.first]->Draw("COLZ");

      lumi->Draw("same");
      trigTex->Draw("same");
      cutAsymmNorm->Draw("same");
      //leg->Draw("same");
      cms->Draw("same");
      for(auto const & extStr : extensions){
	cANVRB->SaveAs(Form("%s/Online_AsymmNormVRelBxAbs_%s_%s.%s",outDirPlot.c_str(), trig.first.c_str(), tag.c_str(), extStr.c_str()));
      }
      delete cANVRB;
    }

    // ------------------------------------------
    /* ZDCAsymmNormVRelBxAbsZoom */
    // ------------------------------------------

    if(doBxMap){
      TCanvas* cANVRB = new TCanvas("cANVRB","cANVRB",700,600);
      cANVRB->cd();
      cANVRB->SetTickx(1);
      cANVRB->SetTicky(1);
      //cANVRB->SetLogy(s);
      cANVRB->SetTopMargin(0.09);
      cANVRB->SetBottomMargin(0.11);
      cANVRB->SetLeftMargin(0.11);
      cANVRB->SetRightMargin(0.11);

      // zdc plus
      hZDC_AsymmNormVRelBxAbsZoom[trig.first]->GetXaxis()->SetTitleSize(0.05);
      hZDC_AsymmNormVRelBxAbsZoom[trig.first]->GetXaxis()->SetTitle("(ZDCp - ZDCm)/(ZDCp + ZDCm)");
      hZDC_AsymmNormVRelBxAbsZoom[trig.first]->GetYaxis()->SetTitle("Bunch # To Closest Filled Bunch");

      hZDC_AsymmNormVRelBxAbsZoom[trig.first]->Draw("COLZ");

      lumi->Draw("same");
      trigTex->Draw("same");
      cutAsymmNorm->Draw("same");
      cms->Draw("same");

      for(auto const & extStr : extensions){
	cANVRB->SaveAs(Form("%s/Online_AsymmNormVRelBxAbsZoom_%s_%s.%s", outDirPlot.c_str(),trig.first.c_str(), tag.c_str(), extStr.c_str()));
      }
      delete cANVRB;
    }

    // ------------------------------------------
    /* Online to GeV */
    // ------------------------------------------

    TLegend* leg2 = new TLegend(0.6,0.7,0.8,0.85);
    leg2->SetBorderSize(0);
    leg2->SetFillStyle(0);
    leg2->SetTextSize(0.045);

    TCanvas* c2 = new TCanvas("c2","c2",800,600);
    c2->cd();
    c2->SetTickx(1);
    c2->SetTicky(1);
    c2->SetLogy();
    c2->SetTopMargin(0.09);
    c2->SetBottomMargin(0.11);
    c2->SetLeftMargin(0.09);
    c2->SetRightMargin(0.05);

    // zdc plus
    hZDCP_withTrig[trig.first]->SetLineColor(kRed);
    hZDCP_withTrig[trig.first]->SetLineWidth(2);
    hZDCP_withTrig[trig.first]->GetXaxis()->SetTitleSize(0.05);
    hZDCP_withTrig[trig.first]->SetMarkerStyle(20);

    TF1* fit1n = new TF1("fit1n", "gaus", 1000, 3000);
    fit1n->SetLineColor(kBlack);
    fit1n->SetLineWidth(2);
    fit1n->SetLineStyle(2);
    fit1n->SetRange(0.,6000);
    hZDCP_withTrig[trig.first]->Fit(fit1n, "0", "", 1000, 3000);

    TF1* fit1nM = new TF1("fit1nM", "gaus",  1000, 3000);
    fit1nM->SetLineColor(kGray+2);
    fit1nM->SetLineWidth(2);
    fit1nM->SetLineStyle(2);
    fit1nM->SetRange(0.,6000);
    hZDCM_withTrig[trig.first]->Fit(fit1nM, "0", "", 1000, 3000);

    // zdc minus
    hZDCM_withTrig[trig.first]->SetLineColor(kBlue);
    hZDCM_withTrig[trig.first]->SetLineWidth(2);
    hZDCM_withTrig[trig.first]->SetMarkerStyle(20);
    hZDCM_withTrig[trig.first]->GetXaxis()->SetTitle("ZDC Offline Energy Sum (GeV)");
    hZDCM_withTrig[trig.first]->GetXaxis()->SetTitleSize(0.05);
    hZDCM_withTrig[trig.first]->GetXaxis()->SetRangeUser(0, 10000);
    hZDCM_withTrig[trig.first]->GetYaxis()->SetRangeUser(1, 1e5);
    hZDCM_withTrig[trig.first]->Draw();
    hZDCP_withTrig[trig.first]->Draw("same");
    // cout << "Integral Plus: " << hZDCP_withTrig->Integral() << endl;
    // cout << "Integral Minus: " << hZDCM_withTrig->Integral() << endl;

    leg2->AddEntry(hZDCP_withTrig[trig.first],"ZDC Plus ","l");
    leg2->AddEntry(hZDCM_withTrig[trig.first],"ZDC Minus","l");
    leg2->Draw("same");
    cms->Draw("same");
    lumi->Draw("same");
    trigTex->Draw("same");
    fit1n->Draw("same");
    fit1nM->Draw("same");
    for(auto const & extStr : extensions){
      c2->SaveAs(Form("%s/Online_ZDCNeutronWithTrig_%s_%s.%s",outDirPlot.c_str(),trig.first.c_str(), tag.c_str(), extStr.c_str()));
    }

    // ------------------------------------------
    /* ratio */
    // ------------------------------------------
    TH1D* clonePlusHist = (TH1D*)hZDCP_withTrig[trig.first]->Clone("numeratorPlus");
    TH1D* cloneMinusHist = (TH1D*)hZDCM_withTrig[trig.first]->Clone("numeratorMinus");

    TGraphAsymmErrors clonePlus(clonePlusHist, hZDCP[trig.first], "cl=0.683 b(1,1) mode");
    TGraphAsymmErrors cloneMinus(cloneMinusHist, hZDCM[trig.first], "cl=0.683 b(1,1) mode");

    TLegend* leg4 = new TLegend(0.3,0.2,0.8,0.3);
    leg4->SetBorderSize(0);
    leg4->SetFillStyle(0);
    leg4->SetTextSize(0.045);

    TCanvas* c4 = new TCanvas("c4","c4",800,600);
    c4->cd();
    c4->SetTickx(1);
    c4->SetTicky(1);
    c4->SetTopMargin(0.09);
    c4->SetBottomMargin(0.11);
    c4->SetLeftMargin(0.09);
    c4->SetRightMargin(0.05);
    clonePlus.SetLineWidth(2);
    cloneMinus.SetLineWidth(2);
    clonePlus.SetLineStyle(1);
    clonePlus.SetMarkerStyle(20);
    cloneMinus.SetMarkerStyle(20);
    cloneMinus.SetMarkerColor(kBlue);
    clonePlus.SetMarkerColor(kRed);
    cloneMinus.SetLineColor(kBlue);
    clonePlus.SetLineColor(kRed);
    clonePlus.GetXaxis()->SetTitle("ZDC Offline Energy Sum (GeV)");
    clonePlus.GetYaxis()->SetTitle("Efficiency");

    leg4->AddEntry(&clonePlus, "ZDC plus", "lp");
    leg4->AddEntry(&cloneMinus, "ZDC minus", "lp");

    cloneMinus.SetLineStyle(1);
    clonePlus.GetYaxis()->SetRangeUser(0, 1.1);
    clonePlus.GetXaxis()->SetRangeUser(0, 10000);
    clonePlus.Draw("");
    cloneMinus.Draw("lp same");
    cms->Draw("same");
    lumi->Draw("same");
    trigTex->Draw("same");
    leg4->Draw("same");
    for(auto const & extStr : extensions){
      c4->SaveAs(Form("%s/TurnOn_%s_%s.%s",outDirPlot.c_str(), trig.first.c_str(), tag.c_str(), extStr.c_str()));
    }


    delete trigTex;
    //cleanup for loop
    delete c4;
    delete leg4;
    delete fit1nM;
    delete fit1n;
    delete c2;
    delete leg2;
    delete cANVB;
    delete asymmNorm;
    delete cAN;
    delete cAVB;
    delete asymm;
    delete cA;
    delete cCj;
    delete cC;
    delete l;
    delete c12;
    delete c;
    delete leg;
  }//end for loop over l1 map

  std::cout << "L" << __LINE__ << std::endl;

  //write all histograms to file
  TFile* outFile_p = new TFile(outFileName.c_str(), "RECREATE");
  //config for knowing the triggers
  TEnv* config_p = new TEnv("config");
  config_p->SetValue("NTRIG", (int)l1Map.size());

  int pos = 0;
  for(auto const & trig : l1Map){
    string labelName = "TRIG." + to_string(pos);
    string label = trig.first;
    config_p->SetValue(labelName.c_str(), label.c_str());
    ++pos;
  }

  //Now write the hists
  pos = 0;
  vector<string> histNames;
  for(auto const & trig : l1Map){
    hZDCP_Emu[trig.first]->Write("", TObject::kOverwrite);
    hZDCM_Emu[trig.first]->Write("", TObject::kOverwrite);
    hZDCP_withTrig[trig.first]->Write("", TObject::kOverwrite);
    hZDCM_withTrig[trig.first]->Write("", TObject::kOverwrite);
    hZDCP[trig.first]->Write("", TObject::kOverwrite);
    hZDCM[trig.first]->Write("", TObject::kOverwrite);
    hZDCP_EmuScaled[trig.first]->Write("", TObject::kOverwrite);
    hZDCM_EmuScaled[trig.first]->Write("", TObject::kOverwrite);
    hZDC_Asymm[trig.first]->Write("", TObject::kOverwrite);
    hZDC_AsymmNorm[trig.first]->Write("", TObject::kOverwrite);
    if(doBxMap){
      hZDC_AsymmVBx[trig.first]->Write("", TObject::kOverwrite);
      hZDC_AsymmNormVBx[trig.first]->Write("", TObject::kOverwrite);
      hZDC_AsymmVRelBxPlus[trig.first]->Write("", TObject::kOverwrite);
      hZDC_AsymmNormVRelBxPlus[trig.first]->Write("", TObject::kOverwrite);
      hZDC_AsymmVRelBxPlusZoom[trig.first]->Write("", TObject::kOverwrite);
      hZDC_AsymmNormVRelBxPlusZoom[trig.first]->Write("", TObject::kOverwrite);
      hZDC_AsymmVRelBxMinus[trig.first]->Write("", TObject::kOverwrite);
      hZDC_AsymmNormVRelBxMinus[trig.first]->Write("", TObject::kOverwrite);
      hZDC_AsymmVRelBxMinusZoom[trig.first]->Write("", TObject::kOverwrite);
      hZDC_AsymmNormVRelBxMinusZoom[trig.first]->Write("", TObject::kOverwrite);
      hZDC_AsymmVRelBxAbs[trig.first]->Write("", TObject::kOverwrite);
      hZDC_AsymmNormVRelBxAbs[trig.first]->Write("", TObject::kOverwrite);
      hZDC_AsymmVRelBxAbsZoom[trig.first]->Write("", TObject::kOverwrite);
      hZDC_AsymmNormVRelBxAbsZoom[trig.first]->Write("", TObject::kOverwrite);
    }
    hZDCAsymCorr[trig.first]->Write("", TObject::kOverwrite);
    hZDCAsymCorrJet[trig.first]->Write("", TObject::kOverwrite);

    if(pos == 0){
      histNames.push_back(hZDCP_Emu[trig.first]->GetName());
      histNames.push_back(hZDCM_Emu[trig.first]->GetName());
      histNames.push_back(hZDCP_withTrig[trig.first]->GetName());
      histNames.push_back(hZDCM_withTrig[trig.first]->GetName());
      histNames.push_back(hZDCP[trig.first]->GetName());
      histNames.push_back(hZDCM[trig.first]->GetName());
      histNames.push_back(hZDCP_EmuScaled[trig.first]->GetName());
      histNames.push_back(hZDCM_EmuScaled[trig.first]->GetName());
      histNames.push_back(hZDC_Asymm[trig.first]->GetName());
      histNames.push_back(hZDC_AsymmNorm[trig.first]->GetName());
      if(doBxMap){
	histNames.push_back(hZDC_AsymmVBx[trig.first]->GetName());
	histNames.push_back(hZDC_AsymmNormVBx[trig.first]->GetName());
	histNames.push_back(hZDC_AsymmVRelBxPlus[trig.first]->GetName());
	histNames.push_back(hZDC_AsymmNormVRelBxPlus[trig.first]->GetName());
	histNames.push_back(hZDC_AsymmVRelBxPlusZoom[trig.first]->GetName());
	histNames.push_back(hZDC_AsymmNormVRelBxPlusZoom[trig.first]->GetName());
	histNames.push_back(hZDC_AsymmVRelBxMinus[trig.first]->GetName());
	histNames.push_back(hZDC_AsymmNormVRelBxMinus[trig.first]->GetName());
	histNames.push_back(hZDC_AsymmVRelBxMinusZoom[trig.first]->GetName());
	histNames.push_back(hZDC_AsymmNormVRelBxMinusZoom[trig.first]->GetName());
	histNames.push_back(hZDC_AsymmVRelBxAbs[trig.first]->GetName());
	histNames.push_back(hZDC_AsymmNormVRelBxAbs[trig.first]->GetName());
	histNames.push_back(hZDC_AsymmVRelBxAbsZoom[trig.first]->GetName());
	histNames.push_back(hZDC_AsymmNormVRelBxAbsZoom[trig.first]->GetName());
      }
      histNames.push_back(hZDCAsymCorr[trig.first]->GetName());
      histNames.push_back(hZDCAsymCorrJet[trig.first]->GetName());

      for(unsigned int i = 0; i < histNames.size(); ++i){
	string histName = histNames[i];
	histName.replace(histName.find(trig.first), trig.first.size(), "");
	histNames[i] = histName;
      }
    }
    ++pos;

    //and cleanup
    delete hZDCP_Emu[trig.first];
    delete hZDCM_Emu[trig.first];
    delete hZDCP_withTrig[trig.first];
    delete hZDCM_withTrig[trig.first];
    delete hZDCP[trig.first];
    delete hZDCM[trig.first];
    delete hZDCP_EmuScaled[trig.first];
    delete hZDCM_EmuScaled[trig.first];
    delete hZDC_Asymm[trig.first];
    delete hZDC_AsymmNorm[trig.first];
    if(doBxMap){
      delete hZDC_AsymmVBx[trig.first];
      delete hZDC_AsymmNormVBx[trig.first];
      delete hZDC_AsymmVRelBxPlus[trig.first];
      delete hZDC_AsymmNormVRelBxPlus[trig.first];
      delete hZDC_AsymmVRelBxPlusZoom[trig.first];
      delete hZDC_AsymmNormVRelBxPlusZoom[trig.first];
      delete hZDC_AsymmVRelBxMinus[trig.first];
      delete hZDC_AsymmNormVRelBxMinus[trig.first];
      delete hZDC_AsymmVRelBxMinusZoom[trig.first];
      delete hZDC_AsymmNormVRelBxMinusZoom[trig.first];
      delete hZDC_AsymmVRelBxAbs[trig.first];
      delete hZDC_AsymmNormVRelBxAbs[trig.first];
      delete hZDC_AsymmVRelBxAbsZoom[trig.first];
      delete hZDC_AsymmNormVRelBxAbsZoom[trig.first];
    }
    delete hZDCAsymCorr[trig.first];
    delete hZDCAsymCorrJet[trig.first];
  }

  std::cout << "L" << __LINE__ << std::endl;

  //Finish config w/ histnames
  config_p->SetValue("NHIST", (int)histNames.size());
  for(unsigned int i = 0; i < histNames.size(); ++i){
    string label = "HIST." + to_string(i);
    config_p->SetValue(label.c_str(), histNames[i].c_str());
  }

  config_p->Write("config", TObject::kOverwrite);
  delete config_p;

  std::cout << "L" << __LINE__ << std::endl;

  outFile_p->Close();
  delete outFile_p;

  //output summary info and return
  cout << "Writing histograms to '" << outFileName << "'." << endl;
//  cout << "Trigger summary for Run " << runNum << ", " << globalLabel << ": " << endl;
  for(auto const & counter : l1MapCounter){
    cout << " " << counter.first << ": " << counter.second << endl;
  }

  cout << "Min-max bx: " << minBx << "-" << maxBx << endl;

  std::cout << "L" << __LINE__ << std::endl;

  return 0;

}
