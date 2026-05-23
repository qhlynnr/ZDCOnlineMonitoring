import ROOT

# Don't draw all of the plots, instead jsut save the 
ROOT.gROOT.SetBatch(True) 


RunEra = "PbPb 2025" 
RunNumber = "Run399415"

RunEnergy = "5.36 TeV"

InputFileName = "ZDCAnalyze_2026_404157_FSC.root"#"HIForward0_2024_R388750.root"

fileBegin = "PbPbCommissioning_FSC_TDC" # Beginning of plot output, Full output will be <fileBegin>+<ZDC Channel>.png

nbin_minus, min_minus, max_minus = 50,0,150 # Binning for ZDC Minus
nbin_plus, min_plus, max_plus = 50,0,150 # Binning for ZDC Plus

doAutoFit = False  # Use the mean and width from histogram stats to initialize gaussian fit
ConsiderAllTimeSlices = False # adds all TDC values less than 50 to the Timing Histogram, Otherwise only TDC info from TS with highest charge or preceding TS in case of overflow is considered.
doPlotting = True

Min_Charge = 1000 # minimum charge to consider a TDC value

fitMin = 40
fitMax = 60

ZdcFitRange = {
  "ZDCm_EM7": [fitMin, fitMax ], 
  "ZDCm_EM8": [fitMin, fitMax ], 
  "ZDCm_EM9": [fitMin, fitMax ], 
  "ZDCm_EM10": [fitMin, fitMax ], 
  "ZDCm_EM11": [fitMin, fitMax ], 
  "ZDCm_EM12": [fitMin, fitMax ], 
  "ZDCp_EM7": [fitMin, fitMax ], 
  "ZDCp_EM8": [fitMin, fitMax ], 
  "ZDCp_EM9": [fitMin, fitMax ], 
  "ZDCp_EM10": [fitMin, fitMax ], 
  "ZDCp_EM11": [fitMin, fitMax ], 
  "ZDCp_EM12": [fitMin, fitMax ], 
}




# ZDC Rechit and Digi trees from zdcanalyzer
ZDCRecHit_treeName = "zdcanalyzer/zdcrechit"
FSCDigi_TreeName = "fscanalyzer/fscdigi"

chain = ROOT.TChain(ZDCRecHit_treeName)
chain.Add(  InputFileName)

# Add a friend tree
chain.AddFriend("fscdigi = {}".format(FSCDigi_TreeName), InputFileName) 

# chain.AddFriend("hlttree = hltanalysis/HltTree", InputFileName) 

dataframe = ROOT.RDataFrame(chain)

tdcFuncBegin = "chargefCTs0, tdcTs0, chargefCTs1, tdcTs1, chargefCTs2, tdcTs2, chargefCTs3, tdcTs3, chargefCTs4, tdcTs4, chargefCTs5, tdcTs5"


FuncType = "GetTdcTime"
ZdcChargeDict = {
  "ZDCm_EM7": ["{}({}, 0)".format(FuncType, tdcFuncBegin),  nbin_minus, min_minus, max_minus], 
  "ZDCm_EM8": ["{}({}, 1)".format(FuncType, tdcFuncBegin),  nbin_minus, min_minus, max_minus], 
  "ZDCm_EM9": ["{}({}, 2)".format(FuncType, tdcFuncBegin),  nbin_minus, min_minus, max_minus], 
  "ZDCm_EM10": ["{}({}, 3)".format(FuncType, tdcFuncBegin),  nbin_minus, min_minus, max_minus], 
  "ZDCm_EM11": ["{}({}, 4)".format(FuncType, tdcFuncBegin),  nbin_minus, min_minus, max_minus], 
  "ZDCm_EM12": ["{}({}, 5)".format(FuncType, tdcFuncBegin),  nbin_minus, min_minus, max_minus], 
  "ZDCp_EM7": ["{}({}, 6)".format(FuncType, tdcFuncBegin),  nbin_minus, min_minus, max_minus], 
  "ZDCp_EM8": ["{}({}, 7)".format(FuncType, tdcFuncBegin),  nbin_minus, min_minus, max_minus], 
  "ZDCp_EM9": ["{}({}, 8)".format(FuncType, tdcFuncBegin),  nbin_minus, min_minus, max_minus], 
  "ZDCp_EM10": ["{}({}, 9)".format(FuncType, tdcFuncBegin),  nbin_minus, min_minus, max_minus], 
  "ZDCp_EM11": ["{}({}, 10)".format(FuncType, tdcFuncBegin),  nbin_minus, min_minus, max_minus], 
  "ZDCp_EM12": ["{}({}, 11)".format(FuncType, tdcFuncBegin),  nbin_minus, min_minus, max_minus], 
}


TDC_Variable = "tdcTs@"

ZdcChargeDict_TDC = {
  "ZDCm_EM7": ["{}[0]".format(TDC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCm_EM8": ["{}[1]".format(TDC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCm_EM9": ["{}[2]".format(TDC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCm_EM10": ["{}[3]".format(TDC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCm_EM11": ["{}[4]".format(TDC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCm_EM12": ["{}[5]".format(TDC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCp_EM7": ["{}[6]".format(TDC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCp_EM8": ["{}[7]".format(TDC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCp_EM9": ["{}[8]".format(TDC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCp_EM10": ["{}[9]".format(TDC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCp_EM11": ["{}[10]".format(TDC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCp_EM12": ["{}[11]".format(TDC_Variable),  nbin_minus, min_minus, max_minus], 
}

fC_Variable = "chargefCTs@"

ZdcChargeDict_fC = {
  "ZDCm_EM7": ["{}[0]".format(fC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCm_EM8": ["{}[1]".format(fC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCm_EM9": ["{}[2]".format(fC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCm_EM10": ["{}[3]".format(fC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCm_EM11": ["{}[4]".format(fC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCm_EM12": ["{}[5]".format(fC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCp_EM7": ["{}[6]".format(fC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCp_EM8": ["{}[7]".format(fC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCp_EM9": ["{}[8]".format(fC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCp_EM10": ["{}[9]".format(fC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCp_EM11": ["{}[10]".format(fC_Variable),  nbin_minus, min_minus, max_minus], 
  "ZDCp_EM12": ["{}[11]".format(fC_Variable),  nbin_minus, min_minus, max_minus], 
}



# Select events for the analysis
ROOT.gInterpreter.Declare(
    """
using ROOT::RVecF;
using ROOT::RVecI;
float GetTdcTime(const RVecF &Charge0, const RVecI &Tdc0, const RVecF &Charge1, const RVecI &Tdc1, const RVecF &Charge2, const RVecI &Tdc2,
const RVecF &Charge3, const RVecI &Tdc3, const RVecF &Charge4, const RVecI &Tdc4, const RVecF &Charge5, const RVecI &Tdc5, int index, float min_charge =2000)
{

  float max_charge =0;
  
  int max_charge_index = -1;
  int max_charge_TDC = -1;
  int TDC_array[6];


  
  for( int i = 0; i < 6; i++) TDC_array[i] = 99;
  
  if(Charge0[index] > max_charge) { max_charge = Charge0[index]; max_charge_index = 0; max_charge_TDC = Tdc0[index]; TDC_array[0] = Tdc0[index];}
  if(Charge1[index] > max_charge) { max_charge = Charge1[index]; max_charge_index = 1; max_charge_TDC = Tdc1[index]; TDC_array[1] = Tdc1[index];}
  if(Charge2[index] > max_charge) { max_charge = Charge2[index]; max_charge_index = 2; max_charge_TDC = Tdc2[index]; TDC_array[2] = Tdc2[index];}
  if(Charge3[index] > max_charge) { max_charge = Charge3[index]; max_charge_index = 3; max_charge_TDC = Tdc3[index]; TDC_array[3] = Tdc3[index];}
  if(Charge4[index] > max_charge) { max_charge = Charge4[index]; max_charge_index = 4; max_charge_TDC = Tdc4[index]; TDC_array[4] = Tdc4[index];}
  if(Charge5[index] > max_charge) { max_charge = Charge5[index]; max_charge_index = 5; max_charge_TDC = Tdc5[index]; TDC_array[5] = Tdc5[index];}
  
  float Time = -5;
  if(index == 13){
    std::cout << "max charged index: " << max_charge_index << " max charge: " << max_charge << "Charge2[index]: " << Charge2[index] << std::endl;
  }
  
  if (min_charge > max_charge) max_charge_index = -1;
  
  if( max_charge_index == 0){
    if (max_charge_TDC < 60) Time = 25.0*max_charge_index + max_charge_TDC/2.0;
  }

  if( max_charge_index >= 1){
    if( max_charge_TDC ==62){
        int tmp_tdc = TDC_array[max_charge_index];
        if(tmp_tdc < 60) Time = 25.0*(max_charge_index -1) + tmp_tdc/2.0;
    }
    if (max_charge_TDC < 60) Time = 25.0*max_charge_index + max_charge_TDC/2.0;
  }

    return(Time);    
}
"""
)





textsize = .03


    
means = []
widths = []

ZDC_TimeStats = {}

for x in ZdcChargeDict:
    tmp_sigma = ZdcFitRange[x][1]-ZdcFitRange[x][0]
    tmp_mean = .5*(ZdcFitRange[x][1]+ZdcFitRange[x][0])
    
    Timing_Hist = ROOT.TH1D("TimingHist", "", ZdcChargeDict[x][1],ZdcChargeDict[x][2],ZdcChargeDict[x][3])
    
    if ConsiderAllTimeSlices:
        hist_list = []
        for TS in range(0,6):
            Time_var = (ZdcChargeDict_TDC[x][0]).replace("@","{}".format(TS))
            fC_var = (ZdcChargeDict_fC[x][0]).replace("@","{}".format(TS))
            hist_list.append(dataframe.Define("fC_var", fC_var).Filter("fC_var > {}".format(Min_Charge)).Define("test",Time_var).Filter("test < 51").Define("Cor_Time", "test/2.0 + {}".format(25.0*TS)).Histo1D((x, x, ZdcChargeDict[x][1],ZdcChargeDict[x][2],ZdcChargeDict[x][3]), "Cor_Time"))
            
            Timing_Hist.Add(hist_list[-1].GetValue(),1.0)
        

    if not ConsiderAllTimeSlices:
        Timing_Hist = dataframe.Define("test",ZdcChargeDict[x][0]).Histo1D((x, x, ZdcChargeDict[x][1],ZdcChargeDict[x][2],ZdcChargeDict[x][3]), "test")
    
    
    if doAutoFit:
        tmp_mean = Timing_Hist.GetMean()
        tmp_sigma = 1.0
    PedCharge, mean, sigma, model = 0,0,0, 0
    
    
    if not doAutoFit:
        # PedCharge = ROOT.RooRealVar("PedCharge", "fC value for {}".format(x), 0, ZdcFitRange[x][0],ZdcFitRange[x][1])
        PedCharge = ROOT.RooRealVar("PedCharge", "fC value for {}".format(x), 0, ZdcChargeDict[x][2],ZdcChargeDict[x][3])    
        mean = ROOT.RooRealVar("mean", "mean of gaussians", tmp_mean,ZdcFitRange[x][0],ZdcFitRange[x][1])
        sigma = ROOT.RooRealVar("sigma", "width of gaussians", .25*tmp_sigma, .05*tmp_sigma,4*tmp_sigma)
        model = ROOT.RooGaussian("model", "Signal component 1", PedCharge, mean, sigma)
        PedCharge.setRange("signal",ZdcFitRange[x][0],ZdcFitRange[x][1]);   
        
    if doAutoFit:
        PedCharge = ROOT.RooRealVar("PedCharge", "fC value for {}".format(x), 0, ZdcChargeDict[x][2],ZdcChargeDict[x][3])    
        mean = ROOT.RooRealVar("mean", "mean of gaussians", tmp_mean,tmp_mean - 5.0,tmp_mean + 5.0)
        sigma = ROOT.RooRealVar("sigma", "width of gaussians", .25*tmp_sigma, .05*tmp_sigma,4*tmp_sigma)
        model = ROOT.RooGaussian("model", "Signal component 1", PedCharge, mean, sigma)
        PedCharge.setRange("signal",tmp_mean - 5.0,tmp_mean + 5.0);        

    data = ROOT.RooDataHist("PedCharge", "PedCharge", [PedCharge], Import=Timing_Hist.GetValue())
    model.fitTo(data,ROOT.RooFit.Range("signal"), PrintLevel = -1,PrintEvalErrors = -1); 

    if doPlotting:
        xframe = PedCharge.frame(Title="")
        data.plotOn(xframe)
        
        model.plotOn(xframe, LineStyle="-", LineColor=1);     
        
        
        c = ROOT.TCanvas("rf201_composite_{}".format(x), "rf201_composite_{}".format(x), 1600, 1200)
        ROOT.gPad.SetLeftMargin(0.15)
        ROOT.gStyle.SetOptTitle(0)
        ROOT.gStyle.SetOptStat(0)
        ROOT.gPad.SetRightMargin(0.05)
        ROOT.gPad.SetTopMargin(0.05)
        ROOT.gPad.SetBottomMargin(0.12)
        c.SetTicks(1,1)
        xframe.GetYaxis().SetTitleOffset(0.0)
        # xframe.SetMinimum(1);
        
        xframe.GetXaxis().SetTitle("Time (ns): {}".format(x))
        xframe.GetYaxis().SetTitle("Number of Events")
        xframe.GetYaxis().SetTitleSize(0.04)
        xframe.GetXaxis().SetTitleSize(0.04)
        xframe.GetYaxis().SetTitleOffset(1.5)
        xframe.GetXaxis().SetTitleOffset(1.5)
        xframe.GetYaxis().SetMaxDigits(3)
        xframe.GetXaxis().SetMaxDigits(3)
        ROOT.TGaxis.SetExponentOffset(0.01, -0.055, "y")
        
        xframe.Draw()
        label = ROOT.TLatex(); label.SetNDC(True); label.SetTextColor(1); label.SetTextSize(textsize);
        label.DrawLatex(.55, .8, "#splitline{{Gaussain Fit mean = {:0.2f} #pm {:0.2f} }}{{ width = {:0.2f} #pm {:0.2f} }}".format(mean.getValV(),mean.getError(),sigma.getValV(),sigma.getError()))

    
        c.SetLogy(0)
        c.SaveAs("{}_{}.png".format(fileBegin, x))

    means.append(mean.getValV())
    widths.append(sigma.getValV())
    
    ZDC_TimeStats[x] = [mean.getValV(),sigma.getValV(),mean.getError(),sigma.getError()]
    
    

    
    
print("| Channel | Mean | Mean Error | Sigma | Sigma Error |")
print("|---------|------|------------|-------|-------------|")
for x in ZDC_TimeStats:
    print(f"| {x} | {ZDC_TimeStats[x][0]:.2f} | {ZDC_TimeStats[x][2]:.2f} | {ZDC_TimeStats[x][1]:.2f} | {ZDC_TimeStats[x][3]:.2f} |")

