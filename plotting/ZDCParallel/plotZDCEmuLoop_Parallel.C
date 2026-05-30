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
#include <regex>

#include "TDirectory.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TChain.h"
#include "TStyle.h"


#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphAsymmErrors.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TEnv.h"

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <chrono>
#include <ctime>
#include "TSystem.h"


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

int plotZDCEmuLoop(
                    vector<string> lsNumSetVector = {"0069","0070",
                    "0071","0072","0073","0074","0075","0076","0077","0078","0079",
                    "0080","0081","0082","0083","0084","0085","0086","0087","0088","0089","0090",
                    "0091","0092","0093","0094","0095","0096","0097","0098","0099","0100","0101",
                    "0102","0103","0104","0105","0106","0107","0108","0109","0110","0111","0112"
                    },
                  string intxtfilename = "/afs/cern.ch/user/x/xirong/ZDCOnlineMonitoring/plotting/Forward_394.txt",
                  string key = "PhysicsHIForward",
                  string outfoldername = "/afs/cern.ch/user/x/xirong/ZDCOnlineMonitoring/plotting/output_new/") {
    // const char *textfilewithfolders = "/eos/cms/store/group/phys_heavyions/cmcginn/Run2026/PhysicsHIPhysicsRawPrime10/404/195/"

    cout << "L" << __LINE__ << endl;
  
    std::vector<std::string> folderNames;
    std::ifstream file(intxtfilename);

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()){
          if (line.back() == ':'){
            line.pop_back();
          }
          cout << "line: " << line << endl;
          folderNames.push_back(line);
        }
    }

    cout << "Number of folders: " << folderNames.size() << endl;


    //Define the format you will save to
    vector<string> extensions = {"png","C"}; //add the .C output if you want to do rapid edits after the fact

    vector<string> files;
    vector<string> files_lumisections = {};
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
    runNumToFill["404394"] = "11794";
  
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
    runNumToFill["404338"] = "11787";
    runNumToFill["404359"] = "11791";
    

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
    fillToBunches["11787"] = "58";
    fillToBunches["11791"] = "338";
    fillToBunches["11794"] = "642";


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
  string lsNum = "9999";
  string pdName = key;

  for (int i = 0; i < folderNames.size(); i++){
    string input = folderNames[i];
    string inputStr = string(input);

    string pdnum = "";
    size_t pos = inputStr.find(key);
    if (pos != string::npos) {
        pos += key.length();
        // Extract digits after the key
        while (pos < inputStr.size() && isdigit(inputStr[pos])) {
            pdnum += inputStr[pos];
            ++pos;
        }
    }
    runNum = inputStr;
    //Consider: ExpressCosmics, HIExpress, HIForwardX
    string formatInput = "/eos/cms/store/group/phys_heavyions/cmcginn/Run" + runYear + "/" + pdName + pdnum + "/";
   /* std::regex runRe("(\\d{3})(?:/|/:)?$");
    std::smatch runMatch;

    if (std::regex_search(runNum, runMatch, runRe)) {
        runNum = runMatch[1];
        cout << "Run Num:" << runNum << end
    }    */
    if(runNum.find(formatInput) != string::npos){
        runNum.replace(0, formatInput.size(), "");

        while(runNum.find("/") != string::npos){
          runNum.replace(runNum.find("/"), 1, "");
        }
        while(runNum.find(":") != string::npos){
          runNum.replace(runNum.find(":"), 1, "");
        }
    }
    else{
        cout << "input '" << input << "' format unrecognized, return 1" << endl;
        cout << "Format " << formatInput << endl;
        return 1;
    }
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
  
    isMB = inputStr.find("HIPhysicsRawPrime") != string::npos;
    isZeroBias = inputStr.find("PhysicsHIForward") != string::npos;
    isEmptyBunches = inputStr.find("HIExpress") != string::npos;
    isCosmics = inputStr.find("ExpressCosmics") != string::npos;
  }  
    TTree tree("Tree", "Tree");
    int lumi;
    auto branch = tree.Branch("lumi", &lumi);
    cout << "L" << __LINE__ << endl;
    for (string lsNumSet : lsNumSetVector){
      cout << "ls Num set:" << lsNumSet << endl;
      lumi = stoi(lsNumSet);
      tree.Fill();
      for (auto const & file : files){
          std::regex re("ls(\\d+)");
          std::smatch match;
          if (std::regex_search(file, match, re)) {
            lsNum = match[1];
          }
          if (lsNum == lsNumSet){
              files_lumisections.push_back(file);
            }
          }
      }
    string lsNumStr = lsNumSetVector[0] + "to" + lsNumSetVector.back();
    cout << "Files with lumisection:" << lsNumStr << endl;
    for (auto const & file : files_lumisections){
        cout << file << endl;
    }

    cout << "runnum: " << runNum << endl;
    string fill = runNumToFill[runNum]; 
    cout << "fill: " << fill << endl;
    if(fill.size() == 0){cout << "Fill not set. for run " << runNum << " return 1 "<< endl; return 1;}

    string nBunch = fillToBunches[fill];
    if(nBunch.size() == 0){cout << "NBunch for fill=" << fill << " not set. return 1" << endl; return 1;}
    string outDirPlot = "/eos/cms/store/group/phys_heavyions/xirong/ZDCNeutronPeakPlots/Run" + runNum + "ls" + lsNumStr + key + "/";
    if (gSystem->AccessPathName(outDirPlot.c_str())) {
        gSystem->mkdir(outDirPlot.c_str(), kTRUE);
    }

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

    cout << __LINE__ << endl;
  /* read in the unpacked information */
  TChain zdcChain("zdcanalyzer/zdcrechit");
  FillChain(zdcChain, files_lumisections);
  TTreeReader zdcReader(&zdcChain);
  TTreeReaderValue<float> sumPlus(zdcReader, "sumPlus");
  TTreeReaderValue<float> sumMinus(zdcReader, "sumMinus");

  /* read in the unpacked information */
  TChain unpackerChain("l1UpgradeTree/L1UpgradeTree");
  FillChain(unpackerChain, files_lumisections);
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
  FillChain(hiEventChain, files_lumisections);
  TTreeReader eventReader(&hiEventChain);
  TTreeReaderValue<uint> runNumber(eventReader, "run");
  TTreeReaderValue<ULong64_t> eventNumber(eventReader, "event");
  TTreeReaderValue<uint> lumiSection(eventReader, "lumi");
  TTreeReaderValue<uint> bx(eventReader, "bx");

  TChain emuChain("l1UpgradeEmuTree/L1UpgradeTree");
  FillChain(emuChain, files_lumisections);
  TTreeReader emuReader(&emuChain);
  TTreeReaderValue<vector<short>> emuSum(emuReader, "sumZDCIEt");
  TTreeReaderValue<vector<short>> emuType(emuReader, "sumZDCType");
  TTreeReaderValue<vector<float>>	emuBx(emuReader, "sumZDCBx");
  TTreeReaderValue<unsigned short> nSums(emuReader, "nSumsZDC");

   /* read in information on the emulated trigger fires*/
  TChain l1TrigChain("l1uGTTree/L1uGTTree");
  FillChain(l1TrigChain, files_lumisections);
  TTreeReader emuTrigReader(&l1TrigChain);
  TTreeReaderValue<vector<bool>> initialTrigDecision(emuTrigReader, "m_algoDecisionFinal");

  /* create histograms */
  map<string, TH1D*> hZDCP_Emu;
  map<string, TH1D*> hZDCM_Emu;
  map<string, TH1D*> hZDCP_withTrig;
  map<string, TH1D*> hZDCM_withTrig;

  map<string, TH1D*> hZDCP_withTrig_nocut;
  map<string, TH1D*> hZDCM_withTrig_nocut;

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
  int histxmax = 20000;
  for(auto const & trig : l1Map){
    l1MapCounter[trig.first] = 0;

    hZDCP_Emu[trig.first] = new TH1D(("hZDCP_Emu_" + trig.first).c_str(), "ZDC Plus Emulated", 100, 0, maxZDCESum);
    hZDCM_Emu[trig.first] = new TH1D(("hZDCM_Emu_" + trig.first).c_str(), "ZDC Minus Emulated", 100, 0, maxZDCESum);
    hZDCP_withTrig[trig.first] = new TH1D(("hZDCP_withTrig_" + trig.first).c_str(), "ZDC Plus", 100, 0, histxmax);
    hZDCM_withTrig[trig.first] = new TH1D(("hZDCM_withTrig_" + trig.first).c_str(), "ZDC Minus", 100, 0, histxmax);
    
    hZDCP_withTrig_nocut[trig.first] = new TH1D(("hZDCP_withTrig_nocut_" + trig.first).c_str(), "ZDC Plus", 100, 0, histxmax);
    hZDCM_withTrig_nocut[trig.first] = new TH1D(("hZDCM_withTrig_nocut_" + trig.first).c_str(), "ZDC Minus", 100, 0, histxmax);

    hZDCP[trig.first] = new TH1D(("hZDCP_" + trig.first).c_str(), "ZDC Plus", 100, 0, histxmax);
    hZDCM[trig.first] = new TH1D(("hZDCM_" + trig.first).c_str(), "ZDC Minus", 100, 0, histxmax);
    hZDCP_EmuScaled[trig.first] = new TH1D(("hZDCP_EmuScaled_" + trig.first).c_str(), "ZDC Plus", 100, 0, histxmax);
    hZDCM_EmuScaled[trig.first] = new TH1D(("hZDCM_EmuScaled_" + trig.first).c_str(), "ZDC Minus", 100, 0, histxmax);
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

    // Store start time point
    auto start = std::chrono::steady_clock::now();
    // Print wall-clock start time
    std::time_t start_time = std::time(nullptr);
    std::cout << "Start time: " << std::ctime(&start_time);
    
    //Try to extract sthe run number automatically from the path
    const string outFileName = outfoldername + "plotZDCEmuHists_" + pdName + "_Run" + runNum + "ls" + lsNumStr + ".root";

    string tag = "Run" + runNum + "ls" + lsNumStr;
    string lumiStr = "Run: " + runNum + ", Fill: " + fill + ", nBunches: " + nBunch;
    string cmsLabel = "#bf{CMS}#it{Work-in-Progress} " + runYear + " PbPb (5.36 TeV) " + globalLabel;

    
    cout << __LINE__ << endl;
    //  if(isZeroBias) cmsLabel = cmsLabelfitxmin4n + " ZeroBias";
    //  else if(isEmptyBunches) cmsLabel = cmsLabel + " Empty Bx";
    cout << "L" << __LINE__ << endl;

    int L1ECutValue = 40;
    cout << "L" << __LINE__ << endl;
  for (Long64_t i = 0; i < totalEvents; i++) {
    emuReader.Next(); emuTrigReader.Next(); zdcReader.Next(); unpackerReader.Next(); eventReader.Next();
    // zero bias
    if (i % 50000 == 0) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);
        std::cout << "Loop " << i << " | time elapsed = "
                  << elapsed.count()
                  << " s\n";
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
      if(emuPlus > L1ECutValue)hZDCP_withTrig[trigVal.first]->Fill(*sumPlus);
      if(emuMinus> L1ECutValue)hZDCM_withTrig[trigVal.first]->Fill(*sumMinus);
      hZDCP_withTrig_nocut[trigVal.first]->Fill(*sumPlus);
      hZDCM_withTrig_nocut[trigVal.first]->Fill(*sumMinus);

      hZDCM_EmuScaled[trigVal.first]->Fill(emuMinus*16);
      hZDCP_EmuScaled[trigVal.first]->Fill(emuPlus*16);
      }
    }

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

  //write all histograms to file
  TFile* outFile_p = new TFile(outFileName.c_str(), "RECREATE");
  //config for knowing the triggers
  TEnv* config_p = new TEnv("config");
  config_p->SetValue("NTRIG", (int)l1Map.size());
  config_p->SetValue("cmsLabel",cmsLabel.c_str());
  config_p->SetValue("lumiStr",lumiStr.c_str());
  config_p->SetValue("tag",tag.c_str());

  int pos = 0;
  for(auto const & trig : l1Map){
    string labelName = "TRIG." + to_string(pos);
    string label = trig.first;
    config_p->SetValue(labelName.c_str(), label.c_str());
    std::string nevtstr = "nEvents." + std::to_string(pos);
    config_p->SetValue(nevtstr.c_str(), l1MapCounter[trig.first]);
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

int main(int argc, char *argv[]){

 //   string lumi = argv[1];

    std::vector<std::string> lsNumSetVector;

    std::stringstream ss(argv[1]);
    std::string item;

    while (std::getline(ss, item, ',')) {
      lsNumSetVector.push_back(item);
    }
    //vector<string> lsNumSetVector = {lumi};
    /*vector<string> lsNumSetVector = {"0069","0070",
                        "0071","0072","0073","0074","0075","0076","0077","0078","0079",
                        "0080","0081","0082","0083","0084","0085","0086","0087","0088","0089","0090",
                        "0091","0092","0093","0094","0095","0096","0097","0098","0099","0100","0101",
                        "0102","0103","0104","0105","0106","0107","0108","0109","0110","0111","0112"};*/
    plotZDCEmuLoop(lsNumSetVector,argv[2],argv[3]);
}