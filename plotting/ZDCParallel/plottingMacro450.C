  
  
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

void plottingMacro450(){

    string inputfile = "/afs/cern.ch/user/x/xirong/ZDCOnlineMonitoring/plotting/output/output_HIForward_Run404359_450to475/merged/plotZDCEmuHists_PhysicsHIForward_Run404359_new.root";
    string outDirPlot = "/afs/cern.ch/user/x/xirong/ZDCOnlineMonitoring/plotting/PlotNew/Plots450New/";
    vector<string> extensions = {"png","C","pdf"}; //add the .C output if you want to do rapid edits after the fact

    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    TFile *f = TFile::Open(inputfile.c_str());
    if (!f){
      cout << "no input file" << endl;
    }
    TEnv* env = (TEnv*)f->Get("config");
    TH1D* hZDCP_L1_ZeroBias = (TH1D*)f->Get("hZDCP_L1_ZeroBias");
    TH1D* hZDCM_L1_ZeroBias = (TH1D*)f->Get("hZDCM_L1_ZeroBias");
    TH1I* nEventsHist = (TH1I*)f->Get("nEvents");
    //int nEvents = 0;
    int nEvents = nEventsHist->GetBinContent(1);
    int NTrig = env->GetValue("NTRIG",0);
    string cmsLabel = env->GetValue("cmsLabel","");
    cmsLabel = "#bf{CMS}#it{Preliminary}  2026 PbPb (5.36 TeV) ";
    string cmsLabel_WIP = "#bf{CMS}#it{WIP}  2026 PbPb (5.36 TeV) ";

    string lumiStr = env->GetValue("lumiStr","");
    string tag = env->GetValue("tag","");
    cout << "NTrig: " << NTrig << endl;
    int histPos = 0;
    double maxJetEt = -1.0;
    int fitxmin1n = 1000;
    int fitymin1n = 3000;

    int fitxmin2n = 4000;
    int fitymin2n = 6000;

    int fitxmin3n = 6700;
    int fitymin3n = 8000;

    int fitxmin4n = 8500;
    int fitymin4n = 11000;

    int fitxmin4nP = 9250;
    int fitymin4nP = 10500;


    int xmax = 20000;
    int ymin = 5000;
    int xminzoom = 5000;
    int xmaxzoom = 18000;
    int yminzoom = 10000;
    int ymaxzoom = 26000;

    const double topY = 0.86;
    const double bottomY = 0.36;
    const double xLatex = 0.17;
    
    TLatex* cms = new TLatex(0.10,0.92,cmsLabel.c_str());
    cms->SetNDC();
    cms->SetTextSize(0.05);
    cms->SetTextFont(42);

      TLatex* cmsWIP = new TLatex(0.10,0.92,cmsLabel_WIP.c_str());
    cmsWIP->SetNDC();
    cmsWIP->SetTextSize(0.05);
    cmsWIP->SetTextFont(42);
    
    TLatex* lumi = new TLatex(xLatex,topY,lumiStr.c_str());
    lumi->SetNDC();
    lumi->SetTextSize(0.035);
    lumi->SetTextFont(42);

    for (int i = 0; i < NTrig; i++){
        string labelName = "TRIG." + to_string(i);
        string triglabel = env->GetValue(labelName.c_str(),"");
        cout << "TrigLabel: " << triglabel << endl;
      
      string nEventStr = Form("nEvents: %i",nEvents);
      TLatex* nEventsTex = new TLatex(0.65,topY,nEventStr.c_str());
      nEventsTex->SetNDC();
      nEventsTex->SetTextSize(0.035);
      nEventsTex->SetTextFont(42);

      TLatex* trigTex = new TLatex(xLatex,topY-0.06,triglabel.c_str());
      trigTex->SetNDC();
      trigTex->SetTextSize(0.035);
      trigTex->SetTextFont(42);

      TLatex* lumisectionTex = new TLatex(xLatex,topY-0.12,"lumisection 450-475");
      lumisectionTex->SetNDC();
      lumisectionTex->SetTextSize(0.035);
      lumisectionTex->SetTextFont(42);

      // ------------------------------------------
      /* Online to GeV */
      // ------------------------------------------

      string hexColorPlus = "kRed+1";
      string hexColorMinus = "kBlue+1";
      string hexColorPlusFit = "kRed-7";
      string hexColorMinusFit = "kBlue-7";


      TLegend* leg2 = new TLegend(0.5,0.6,0.8,0.85);
      leg2->SetBorderSize(0);
      leg2->SetFillStyle(0);
      leg2->SetTextSize(0.03);

      TCanvas* c2 = new TCanvas("c2","c2",800,800);
      c2->cd();
      c2->SetTickx(1);
      c2->SetTicky(1);
      c2->SetLogy();
      c2->SetTopMargin(0.09);
      c2->SetBottomMargin(0.11);
      c2->SetLeftMargin(0.09);
      c2->SetRightMargin(0.05);

      // zdc plus
      hZDCP_L1_ZeroBias->SetLineColor(hexColorPlus);
      hZDCP_L1_ZeroBias->SetLineWidth(2);
      hZDCP_L1_ZeroBias->GetXaxis()->SetTitleSize(0.05);
      hZDCP_L1_ZeroBias->SetMarkerStyle(20);

      TF1* fit1nP = new TF1("fit1nP", "gaus", fitxmin1n, fitymin1n);
      fit1nP->SetLineColor(hexColorPlusFit);
      fit1nP->SetLineWidth(2);
      fit1nP->SetLineStyle(2);
      fit1nP->SetRange(0.,6000);
      hZDCP_L1_ZeroBias->Fit(fit1nP, "0", "", fitxmin1n, fitymin1n);

      TF1* fit1nM = new TF1("fit1nM", "gaus",  fitxmin1n, fitymin1n);
      fit1nM->SetLineColor(hexColorMinusFit);
      fit1nM->SetLineWidth(2);
      fit1nM->SetLineStyle(2);
      fit1nM->SetRange(0.,6000);
      hZDCM_L1_ZeroBias->Fit(fit1nM, "0", "", fitxmin1n, fitymin1n);

      TF1* fit1nP2 = new TF1("fit1nP2", "gaus", fitxmin2n, fitymin2n);
      fit1nP2->SetLineColor(hexColorPlusFit);
      fit1nP2->SetLineWidth(2);
      fit1nP2->SetLineStyle(2);
      fit1nP2->SetRange(0.,20000);
      hZDCP_L1_ZeroBias->Fit(fit1nP2, "0", "", fitxmin2n, fitymin2n);

      TF1* fit1nM2 = new TF1("fit1nM2", "gaus",  fitxmin2n, fitymin2n);
      fit1nM2->SetLineColor(hexColorMinusFit);
      fit1nM2->SetLineWidth(2);
      fit1nM2->SetLineStyle(2);
      fit1nM2->SetRange(0.,20000);
      hZDCM_L1_ZeroBias->Fit(fit1nM2, "0", "", fitxmin2n, fitymin2n);

      TF1* fit1nM3 = new TF1("fit1nM3", "gaus",  fitxmin3n, fitymin3n);
      fit1nM3->SetLineColor(hexColorMinusFit);
      fit1nM3->SetLineWidth(2);
      fit1nM3->SetLineStyle(2);
      fit1nM3->SetRange(0.,20000);
      hZDCM_L1_ZeroBias->Fit(fit1nM3, "0", "", fitxmin3n, fitymin3n);

      TF1* fit1nP3 = new TF1("fit1nP3", "gaus", fitxmin3n, fitymin3n);
      fit1nP3->SetLineColor(hexColorPlusFit);
      fit1nP3->SetLineWidth(2);
      fit1nP3->SetLineStyle(2);
      fit1nP3->SetRange(0.,20000);
      hZDCP_L1_ZeroBias->Fit(fit1nP3, "0", "", fitxmin3n, fitymin3n);

      TF1* fit1nM4 = new TF1("fit1nM4", "gaus",  fitxmin4n, fitymin4n);
      fit1nM4->SetLineColor(hexColorMinusFit);
      fit1nM4->SetLineWidth(2);
      fit1nM4->SetLineStyle(2);
      fit1nM4->SetRange(0.,20000);
      hZDCM_L1_ZeroBias->Fit(fit1nM4, "0", "", fitxmin4n, fitymin4n);

      TF1* fit1nP4 = new TF1("fit1nP4", "gaus", fitxmin4nP, fitymin4nP);
      fit1nP4->SetLineColor(hexColorPlusFit);
      fit1nP4->SetLineWidth(2);
      fit1nP4->SetLineStyle(2);
      fit1nP4->SetRange(0.,20000);
      hZDCP_L1_ZeroBias->Fit(fit1nP4, "0", "", fitxmin4nP, fitymin4nP);

      double muM1 = fit1nM->GetParameter(1);
      double sigM1 = fit1nM->GetParameter(2);

      double muP1 = fit1nP->GetParameter(1);
      double sigP1 = fit1nP->GetParameter(2);

      double muM2  = fit1nM2->GetParameter(1);
      double sigM2 = fit1nM2->GetParameter(2);

      double muP2  = fit1nP2->GetParameter(1);
      double sigP2 = fit1nP2->GetParameter(2);

      double muM3  = fit1nM3->GetParameter(1);
      double sigM3 = fit1nM3->GetParameter(2);

      double muP3  = fit1nP3->GetParameter(1);
      double sigP3 = fit1nP3->GetParameter(2);

      double muP4  = fit1nP4->GetParameter(1);
      double sigP4 = fit1nP4->GetParameter(2);

      double muM4  = fit1nM4->GetParameter(1);
      double sigM4 = fit1nM4->GetParameter(2);


      cout << "Ratio mu 2/mu 1 (minus):" << muM2/muM1 << endl;
      cout << "Ratio mu 2/mu 1 (plus):" << muP2/muP1 << endl;
      cout << "Ratio mu 3/mu 1 (minus):" << muM3/muM1 << endl;
      cout << "Ratio mu 3/mu 1 (plus):" << muP3/muP1 << endl;
      cout << "Ratio mu 4/mu 1 (minus):" << muM4/muM1 << endl;
      cout << "Ratio mu 4/mu 1 (plus):" << muP4/muP1 << endl;



      // zdc minus
      hZDCM_L1_ZeroBias->SetLineColor(hexColorMinus);
      hZDCM_L1_ZeroBias->SetLineWidth(2);
      hZDCM_L1_ZeroBias->SetMarkerStyle(20);
      hZDCM_L1_ZeroBias->GetXaxis()->SetTitle("ZDC Offline Energy Sum (GeV)");
      hZDCM_L1_ZeroBias->GetXaxis()->SetTitleSize(0.05);
      hZDCM_L1_ZeroBias->GetXaxis()->SetRangeUser(0, xmax);

      double ymax = std::max(hZDCM_L1_ZeroBias->GetMaximum(), hZDCP_L1_ZeroBias->GetMaximum()) * 1.5;
      hZDCM_L1_ZeroBias->GetYaxis()->SetRangeUser(ymin, ymax);
      hZDCM_L1_ZeroBias->Draw();
      hZDCP_L1_ZeroBias->Draw("same");
      // cout << "Integral Plus: " << hZDCP_withTrig->Integral() << endl;
      // cout << "Integral Minus: " << hZDCM_withTrig->Integral() << endl;

      double redChi1M = fit1nM->GetChisquare() / fit1nM->GetNDF();
      double redChi1P = fit1nP->GetChisquare() / fit1nP->GetNDF();
      double redChi1M2 = fit1nM2->GetChisquare() / fit1nM2->GetNDF();
      double redChi1P2 = fit1nP2->GetChisquare() / fit1nP2->GetNDF();

      cout << "Reduced Chi Squared of fits (with cut):" << endl; 
      cout << "First Gaussian (1000-3000): +: " << redChi1P << " -: " << redChi1M << endl;
      cout << "Second Gaussian (3000-7000): +: " << redChi1P2 << " -: " << redChi1P2 << endl;
      cout << "Third Gaussian (6000-9000): +: " << redChi1P2 << " -: " << redChi1P2 << endl;

      leg2->AddEntry(hZDCP_L1_ZeroBias,"ZDC Plus ","l");
      leg2->AddEntry(hZDCM_L1_ZeroBias,"ZDC Minus ","l");
      leg2->AddEntry(fit1nM,Form("ZDC- peak 1n: #mu=%.1f, #sigma=%.1f", muM1, sigM1), "l");
      leg2->AddEntry(fit1nP,Form("ZDC+ peak 1n: #mu=%.1f, #sigma=%.1f", muP1, sigP1), "l");
      leg2->AddEntry(fit1nM2,Form("ZDC- peak 2n: #mu=%.1f, #sigma=%.1f", muM2, sigM2), "l");
      leg2->AddEntry(fit1nP2,Form("ZDC+ peak 2n: #mu=%.1f, #sigma=%.1f", muP2, sigP2), "l");
      leg2->AddEntry(fit1nM3,Form("ZDC- peak 3n: #mu=%.1f, #sigma=%.1f", muM3, sigM3), "l");
      leg2->AddEntry(fit1nP3,Form("ZDC+ peak 3n: #mu=%.1f, #sigma=%.1f", muP3, sigP3), "l");
      leg2->AddEntry(fit1nM4,Form("ZDC- peak 4n: #mu=%.1f, #sigma=%.1f", muM4, sigM4), "l");
      leg2->AddEntry(fit1nP4,Form("ZDC+ peak 4n: #mu=%.1f, #sigma=%.1f", muP4, sigP4), "l");

      leg2->Draw("same");
      cms->Draw("same");
      nEventsTex->Draw("same");
      lumi->Draw("same");
      trigTex->Draw("same");
      fit1nP->Draw("same");
      fit1nM->Draw("same");
      fit1nP2->Draw("same");
      fit1nM2->Draw("same");
      fit1nP3->Draw("same");
      fit1nM3->Draw("same");
      fit1nP4->Draw("same");
      fit1nM4->Draw("same");
      lumisectionTex->Draw("same");

      for(auto const & extStr : extensions){
        c2->SaveAs(Form("%s/Online_ZDCNeutronWithTrig_%s_%s.%s",outDirPlot.c_str(),triglabel.c_str(), tag.c_str(), extStr.c_str()));
      }

      // ------------------------------------------
      /* Online to GeV Plus Only*/
      // ------------------------------------------

      TLegend* leg2_P = new TLegend(0.5,0.6,0.8,0.85);
      leg2_P->SetBorderSize(0);
      leg2_P->SetFillStyle(0);
      leg2_P->SetTextSize(0.03);

      TCanvas* c2P = new TCanvas("c2","c2",800,600);
      c2P->cd();
      c2P->SetTickx(1);
      c2P->SetTicky(1);
      c2P->SetLogy();
      c2P->SetTopMargin(0.09);
      c2P->SetBottomMargin(0.11);
      c2P->SetLeftMargin(0.09);
      c2P->SetRightMargin(0.05);

      hZDCP_L1_ZeroBias->SetLineColor(hexColorPlus);
      hZDCP_L1_ZeroBias->SetLineWidth(2);
      hZDCP_L1_ZeroBias->SetMarkerStyle(20);
      hZDCP_L1_ZeroBias->GetXaxis()->SetTitle("ZDC Offline Energy Sum (GeV)");
      hZDCP_L1_ZeroBias->GetXaxis()->SetTitleSize(0.05);
      hZDCP_L1_ZeroBias->GetXaxis()->SetRangeUser(0, xmax);

      ymax = std::max(hZDCM_L1_ZeroBias->GetMaximum(), hZDCP_L1_ZeroBias->GetMaximum()) * 1.5;
      hZDCP_L1_ZeroBias->GetYaxis()->SetRangeUser(ymin, ymax);
      hZDCP_L1_ZeroBias->Draw();

      leg2_P->AddEntry(hZDCP_L1_ZeroBias,"ZDC Plus ","l");
      leg2_P->AddEntry(fit1nP,Form("ZDC+ peak 1n: #mu=%.1f, #sigma=%.1f", muP1, sigP1), "l");
      leg2_P->AddEntry(fit1nP2,Form("ZDC+ peak 2n: #mu=%.1f, #sigma=%.1f", muP2, sigP2), "l");
      leg2_P->AddEntry(fit1nP3,Form("ZDC+ peak 3n: #mu=%.1f, #sigma=%.1f", muP3, sigP3), "l");
      leg2_P->AddEntry(fit1nP4,Form("ZDC+ peak 4n: #mu=%.1f, #sigma=%.1f", muP4, sigP4), "l");

      leg2_P->Draw("same");
      cms->Draw("same");
      nEventsTex->Draw("same");
      lumi->Draw("same");
      trigTex->Draw("same");
      fit1nP->Draw("same");
      fit1nP2->Draw("same");
      fit1nP3->Draw("same");
      fit1nP4->Draw("same");
      lumisectionTex->Draw("same");

      for(auto const & extStr : extensions){
        c2P->SaveAs(Form("%s/Online_ZDCNeutronWithTrig_PlusOnly%s_%s.%s",outDirPlot.c_str(),triglabel.c_str(), tag.c_str(), extStr.c_str()));
      }

      // ------------------------------------------
      /* Online to GeV Minus Only*/
      // ------------------------------------------

      TLegend* leg2_M = new TLegend(0.5,0.6,0.8,0.85);
      leg2_M->SetBorderSize(0);
      leg2_M->SetFillStyle(0);
      leg2_M->SetTextSize(0.03);

      TCanvas* c2M = new TCanvas("c2","c2",800,600);
      c2M->cd();
      c2M->SetTickx(1);
      c2M->SetTicky(1);
      c2M->SetLogy();
      c2M->SetTopMargin(0.09);
      c2M->SetBottomMargin(0.11);
      c2M->SetLeftMargin(0.09);
      c2M->SetRightMargin(0.05);

      hZDCM_L1_ZeroBias->SetLineColor(hexColorMinus);
      hZDCM_L1_ZeroBias->SetLineWidth(2);
      hZDCM_L1_ZeroBias->SetMarkerStyle(20);
      hZDCM_L1_ZeroBias->GetXaxis()->SetTitle("ZDC Offline Energy Sum (GeV)");
      hZDCM_L1_ZeroBias->GetXaxis()->SetTitleSize(0.05);
      hZDCM_L1_ZeroBias->GetXaxis()->SetRangeUser(0, xmax);

      ymax = std::max(hZDCM_L1_ZeroBias->GetMaximum(), hZDCP_L1_ZeroBias->GetMaximum()) * 1.5;
      hZDCM_L1_ZeroBias->GetYaxis()->SetRangeUser(ymin, ymax);
      hZDCM_L1_ZeroBias->Draw();

      leg2_M->AddEntry(hZDCM_L1_ZeroBias,"ZDC Minus ","l");
      leg2_M->AddEntry(fit1nM,Form("ZDC- peak 1n: #mu=%.1f, #sigma=%.1f", muP1, sigP1), "l");
      leg2_M->AddEntry(fit1nM2,Form("ZDC- peak 2n: #mu=%.1f, #sigma=%.1f", muP2, sigP2), "l");
      leg2_M->AddEntry(fit1nM3,Form("ZDC- peak 3n: #mu=%.1f, #sigma=%.1f", muP3, sigP3), "l");
      leg2_M->AddEntry(fit1nM4,Form("ZDC- peak 4n: #mu=%.1f, #sigma=%.1f", muP4, sigP4), "l");

      leg2_M->Draw("same");
      cms->Draw("same");
      nEventsTex->Draw("same");
      lumi->Draw("same");
      trigTex->Draw("same");
      fit1nM->Draw("same");
      fit1nM2->Draw("same");
      fit1nM3->Draw("same");
      fit1nM4->Draw("same");
      lumisectionTex->Draw("same");

      for(auto const & extStr : extensions){
        c2M->SaveAs(Form("%s/Online_ZDCNeutronWithTrig_MinusOnly%s_%s.%s",outDirPlot.c_str(),triglabel.c_str(), tag.c_str(), extStr.c_str()));
      }

      // ------------------------------------------
      /* Online to GeV Minus Only (nofit) */
      // ------------------------------------------

      TLegend* leg2_M_nofit = new TLegend(0.5,0.6,0.8,0.85);
      leg2_M_nofit->SetBorderSize(0);
      leg2_M_nofit->SetFillStyle(0);
      leg2_M_nofit->SetTextSize(0.03);

      TCanvas* c2M_nofit = new TCanvas("c2","c2",800,600);
      c2M_nofit->cd();
      c2M_nofit->SetTickx(1);
      c2M_nofit->SetTicky(1);
      c2M_nofit->SetLogy();
      c2M_nofit->SetTopMargin(0.09);
      c2M_nofit->SetBottomMargin(0.11);
      c2M_nofit->SetLeftMargin(0.09);
      c2M_nofit->SetRightMargin(0.05);

      hZDCM_L1_ZeroBias->SetLineColor(hexColorMinus);
      hZDCM_L1_ZeroBias->SetLineWidth(2);
      hZDCM_L1_ZeroBias->SetMarkerStyle(20);
      hZDCM_L1_ZeroBias->GetXaxis()->SetTitle("ZDC Offline Energy Sum (GeV)");
      hZDCM_L1_ZeroBias->GetXaxis()->SetTitleSize(0.05);
      hZDCM_L1_ZeroBias->GetXaxis()->SetRangeUser(0, xmax);

      ymax = std::max(hZDCM_L1_ZeroBias->GetMaximum(), hZDCP_L1_ZeroBias->GetMaximum()) * 1.5;
      hZDCM_L1_ZeroBias->GetYaxis()->SetRangeUser(ymin, ymax);
      hZDCM_L1_ZeroBias->Draw();

      leg2_M_nofit->AddEntry(hZDCM_L1_ZeroBias,"ZDC Minus ","l");
      leg2_M_nofit->Draw("same");
      cms->Draw("same");
      nEventsTex->Draw("same");
      lumi->Draw("same");
      trigTex->Draw("same");
      lumisectionTex->Draw("same");

      for(auto const & extStr : extensions){
        c2M_nofit->SaveAs(Form("%s/Online_ZDCNeutronWithTrig_MinusOnly_nofit_%s_%s.%s",outDirPlot.c_str(),triglabel.c_str(), tag.c_str(), extStr.c_str()));
      }

      // ------------------------------------------
      /* Online to GeV Plus Only (nofit) */
      // ------------------------------------------

      TLegend* leg2_P_nofit = new TLegend(0.5,0.6,0.8,0.85);
      leg2_P_nofit->SetBorderSize(0);
      leg2_P_nofit->SetFillStyle(0);
      leg2_P_nofit->SetTextSize(0.03);

      TCanvas* c2P_nofit = new TCanvas("c2","c2",800,600);
      c2P_nofit->cd();
      c2P_nofit->SetTickx(1);
      c2P_nofit->SetTicky(1);
      c2P_nofit->SetLogy();
      c2P_nofit->SetTopMargin(0.09);
      c2P_nofit->SetBottomMargin(0.11);
      c2P_nofit->SetLeftMargin(0.09);
      c2P_nofit->SetRightMargin(0.05);

      hZDCP_L1_ZeroBias->SetLineColor(hexColorPlus);
      hZDCP_L1_ZeroBias->SetLineWidth(2);
      hZDCP_L1_ZeroBias->SetMarkerStyle(20);
      hZDCP_L1_ZeroBias->GetXaxis()->SetTitle("ZDC Offline Energy Sum (GeV)");
      hZDCP_L1_ZeroBias->GetXaxis()->SetTitleSize(0.05);
      hZDCP_L1_ZeroBias->GetXaxis()->SetRangeUser(0, xmax);

      ymax = std::max(hZDCP_L1_ZeroBias->GetMaximum(), hZDCP_L1_ZeroBias->GetMaximum()) * 1.5;
      hZDCP_L1_ZeroBias->GetYaxis()->SetRangeUser(ymin, ymax);
      hZDCP_L1_ZeroBias->Draw();

      leg2_P_nofit->AddEntry(hZDCP_L1_ZeroBias,"ZDC Plus ","l");
      leg2_P_nofit->Draw("same");
      nEventsTex->Draw("same");
      cms->Draw("same");
      lumi->Draw("same");
      trigTex->Draw("same");
      lumisectionTex->Draw("same");

      for(auto const & extStr : extensions){
        c2P_nofit->SaveAs(Form("%s/Online_ZDCNeutronWithTrig_PlusOnly_nofit_%s_%s.%s",outDirPlot.c_str(),triglabel.c_str(), tag.c_str(), extStr.c_str()));
      }

      // ------------------------------------------
      /* Online to GeV Minus Only (zoomed)*/
      // ------------------------------------------

      TCanvas* c2M_Z = new TCanvas("c2_Z","c2_Z",800,600);
      c2M_Z->cd();
      c2M_Z->SetTickx(1);
      c2M_Z->SetTicky(1);
      c2M_Z->SetTopMargin(0.09);
      c2M_Z->SetBottomMargin(0.11);
      c2M_Z->SetLeftMargin(0.09);
      c2M_Z->SetRightMargin(0.05);

      TH1D* hZDCM_L1_ZeroBias_Zoom = (TH1D*)hZDCM_L1_ZeroBias->Clone("hZDCM_L1_ZeroBias_Zoom");
      hZDCM_L1_ZeroBias_Zoom->GetXaxis()->SetRangeUser(xminzoom, xmaxzoom);
      hZDCM_L1_ZeroBias_Zoom->GetYaxis()->SetRangeUser(yminzoom, ymaxzoom);
      hZDCM_L1_ZeroBias_Zoom->Draw();

      leg2->Draw("same");
      cms->Draw("same");
      nEventsTex->Draw("same");
      lumi->Draw("same");
      trigTex->Draw("same");
      fit1nM->Draw("same");
      fit1nM2->Draw("same");
      fit1nM3->Draw("same");
      fit1nM4->Draw("same");
      lumisectionTex->Draw("same");

      for(auto const & extStr : extensions){
        c2M_Z->SaveAs(Form("%s/Online_ZDCNeutronWithTrig_MinusOnly_Zoomed_%s_%s.%s",outDirPlot.c_str(),triglabel.c_str(), tag.c_str(), extStr.c_str()));
      }
      // ------------------------------------------
      /* Online to GeV Plus Only (zoomed)*/
      // ------------------------------------------

      TCanvas* c2P_Z = new TCanvas("c2_PZ","c2_PZ",800,600);
      c2P_Z->cd();
      c2P_Z->SetTickx(1);
      c2P_Z->SetTicky(1);
      c2P_Z->SetTopMargin(0.09);
      c2P_Z->SetBottomMargin(0.11);
      c2P_Z->SetLeftMargin(0.09);
      c2P_Z->SetRightMargin(0.05);

        // Clone the histogram for zoomed view
        TH1D* hZDCP_L1_ZeroBias_Zoom = (TH1D*)hZDCP_L1_ZeroBias->Clone("hZDCP_L1_ZeroBias_Zoom");
        hZDCP_L1_ZeroBias_Zoom->GetXaxis()->SetRangeUser(xminzoom, xmaxzoom);
        hZDCP_L1_ZeroBias_Zoom->GetYaxis()->SetRangeUser(yminzoom, ymaxzoom);
        hZDCP_L1_ZeroBias_Zoom->Draw();

      leg2->Draw("same");
      cms->Draw("same");
      nEventsTex->Draw("same");
      lumi->Draw("same");
      trigTex->Draw("same");
      fit1nP->Draw("same");
      fit1nP2->Draw("same");
      fit1nP3->Draw("same");
      fit1nP4->Draw("same");
      lumisectionTex->Draw("same");

      for(auto const & extStr : extensions){
        c2P_Z->SaveAs(Form("%s/Online_ZDCNeutronWithTrig_PlusOnly_Zoomed_%s_%s.%s",outDirPlot.c_str(),triglabel.c_str(), tag.c_str(), extStr.c_str()));
      }

        // ------------------------------------------
      /* Online to GeV for LHCC*/
      // ------------------------------------------
      xmax = 15000;
      ymin = 5000;
      xminzoom = 6000;
      xmaxzoom = 15000;
      yminzoom = 11000;
      ymaxzoom = 27000;

      TLegend* leg2_LHCC = new TLegend(0.12,0.5,0.26,0.88);
      leg2_LHCC->SetBorderSize(0);
      leg2_LHCC->SetFillStyle(0);
      leg2_LHCC->SetTextSize(0.03);

      TCanvas* c2_LHCC = new TCanvas("c2_LHCC","c2_LHCC",800,600);
      c2_LHCC->cd();
      c2_LHCC->SetTickx(1);
      c2_LHCC->SetTicky(1);
      c2_LHCC->SetLogy();
      c2_LHCC->SetTopMargin(0.09);
      c2_LHCC->SetBottomMargin(0.11);
      c2_LHCC->SetLeftMargin(0.10);
      c2_LHCC->SetRightMargin(0.05);

      TF1* total_plus = new TF1("pstotal","gaus(0)+gaus(3)+gaus(6)+gaus(9)",1000,12000);
      Double_t parP[12];
      // Get the parameters from the fit
      fit1nP->GetParameters(&parP[0]);
      fit1nP2->GetParameters(&parP[3]);
      fit1nP3->GetParameters(&parP[6]);
      fit1nP4->GetParameters(&parP[9]); 

      TF1* total_minus = new TF1("mstotal","gaus(0)+gaus(3)+gaus(6)+gaus(9)",1000,12000);
      Double_t parM[12];
      // Get the parameters from the fit
      fit1nM->GetParameters(&parM[0]);
      fit1nM2->GetParameters(&parM[3]);
      fit1nM3->GetParameters(&parM[6]);
      fit1nM4->GetParameters(&parM[9]); 

      total_plus->SetParameters(parP);
      total_plus->SetLineColor(hexColorPlus);
      total_plus->SetLineWidth(2);
      total_minus->SetParameters(parM);
      total_minus->SetLineColor(hexColorMinus);
      total_minus->SetLineWidth(2);

      hZDCP_L1_ZeroBias->Fit(total_plus,"R+");
      hZDCM_L1_ZeroBias->Fit(total_minus,"R+");

      // zdc plus
      hZDCP_L1_ZeroBias->SetLineColor(hexColorPlus);
      hZDCP_L1_ZeroBias->SetLineWidth(2);
      hZDCP_L1_ZeroBias->GetXaxis()->SetTitleSize(0.05);
      hZDCP_L1_ZeroBias->SetMarkerStyle(20);

      // zdc minus
      hZDCM_L1_ZeroBias->SetLineColor(hexColorMinus);
      hZDCM_L1_ZeroBias->SetLineWidth(2);
      hZDCM_L1_ZeroBias->SetMarkerStyle(20);
      hZDCM_L1_ZeroBias->GetXaxis()->SetTitle("ZDC Offline Energy Sum (GeV)");
      hZDCM_L1_ZeroBias->GetXaxis()->SetTitleSize(0.05);
      hZDCM_L1_ZeroBias->GetXaxis()->SetRangeUser(0, xmax);
      hZDCP_L1_ZeroBias->GetXaxis()->SetRangeUser(0, xmax);

      hZDCM_L1_ZeroBias->GetYaxis()->SetTitle("Number of Events");
      hZDCM_L1_ZeroBias->GetYaxis()->SetTitleSize(0.05);
      hZDCM_L1_ZeroBias->GetYaxis()->SetRangeUser(ymin, ymax);
      hZDCM_L1_ZeroBias->Draw();
      hZDCP_L1_ZeroBias->Draw("same");
      // cout << "Integral Plus: " << hZDCP_withTrig->Integral() << endl;
      // cout << "Integral Minus: " << hZDCM_withTrig->Integral() << endl;

      cout << "Reduced Chi Squared of fits (with cut):" << endl; 
      cout << "First Gaussian (1000-3000): +: " << redChi1P << " -: " << redChi1M << endl;
      cout << "Second Gaussian (3000-7000): +: " << redChi1P2 << " -: " << redChi1P2 << endl;

      leg2_LHCC->AddEntry(hZDCP_L1_ZeroBias,"ZDC Plus ","l");
      leg2_LHCC->AddEntry(hZDCM_L1_ZeroBias,"ZDC Minus ","l");

      leg2_LHCC->AddEntry(fit1nM,Form("ZDC- 1n: #mu=%.1f GeV, #sigma=%.1f GeV", muM1, sigM1), "l");
      leg2_LHCC->AddEntry(fit1nP,Form("ZDC+ 1n: #mu=%.1f GeV, #sigma=%.1f GeV", muP1, sigP1), "l");
      leg2_LHCC->AddEntry(fit1nM2,Form("ZDC- 2n: #mu=%.1f GeV, #sigma=%.1f GeV", muM2, sigM2), "l");
      leg2_LHCC->AddEntry(fit1nP2,Form("ZDC+ 2n: #mu=%.1f GeV, #sigma=%.1f GeV", muP2, sigP2), "l");
      leg2_LHCC->AddEntry(fit1nM3,Form("ZDC- 3n: #mu=%.1f GeV, #sigma=%.1f GeV", muM3, sigM3), "l");
      leg2_LHCC->AddEntry(fit1nP3,Form("ZDC+ 3n: #mu=%.1f GeV, #sigma=%.1f GeV", muP3, sigP3), "l");
      leg2_LHCC->AddEntry(fit1nM4,Form("ZDC- 4n: #mu=%.1f GeV, #sigma=%.1f GeV", muM4, sigM4), "l");
      leg2_LHCC->AddEntry(fit1nP4,Form("ZDC+ 4n: #mu=%.1f GeV, #sigma=%.1f GeV", muP4, sigP4), "l");

      leg2_LHCC->AddEntry(total_minus,"ZDC Minus Combined Fit","l");
      leg2_LHCC->AddEntry(total_plus,"ZDC Plus Combined Fit","l");

      cms->Draw("same");
     // nEventsTex->Draw("same");
    //  lumi->Draw("same");
     // trigTex->Draw("same");

      total_plus->Draw("same");
      total_minus->Draw("same");
      fit1nP->Draw("same");
      fit1nM->Draw("same");
      fit1nP2->Draw("same");
      fit1nM2->Draw("same");
      fit1nP3->Draw("same");
      fit1nM3->Draw("same");
      fit1nP4->Draw("same");
      fit1nM4->Draw("same");

     // lumisectionTex->Draw("same");

      double widthsmallpad = 0.40;
      double heightsmallpad = widthsmallpad*4/3;
      double padx1 = 0.51;
      double padx2 = padx1+widthsmallpad;
      double pady1 = 0.32;
      double pady2 = pady1+heightsmallpad;

      TPad *smallpad = new TPad("smallpad", "Pad 1", padx1, pady1, padx2, pady2);
      smallpad->Draw();
      smallpad->cd();
      smallpad->SetTopMargin(0.00);
      smallpad->SetBottomMargin(0.22);
      smallpad->SetLeftMargin(0.22);
      smallpad->SetRightMargin(0.00);

      smallpad->SetTickx(1);
      smallpad->SetTicky(1);
      
      hZDCM_L1_ZeroBias_Zoom->GetXaxis()->SetTitleSize((1/heightsmallpad) * hZDCM_L1_ZeroBias->GetXaxis()->GetTitleSize()*0.85);
      hZDCM_L1_ZeroBias_Zoom->GetYaxis()->SetTitleSize((1/heightsmallpad) * hZDCM_L1_ZeroBias->GetYaxis()->GetTitleSize());
      hZDCM_L1_ZeroBias_Zoom->GetXaxis()->SetLabelSize((1/heightsmallpad) * hZDCM_L1_ZeroBias->GetXaxis()->GetLabelSize());
      hZDCM_L1_ZeroBias_Zoom->GetYaxis()->SetLabelSize((1/heightsmallpad) * hZDCM_L1_ZeroBias->GetYaxis()->GetLabelSize());

      hZDCP_L1_ZeroBias_Zoom->GetXaxis()->SetTitleSize((1/heightsmallpad) * hZDCP_L1_ZeroBias->GetXaxis()->GetTitleSize()*0.85);
      hZDCP_L1_ZeroBias_Zoom->GetYaxis()->SetTitleSize((1/heightsmallpad) * hZDCP_L1_ZeroBias->GetYaxis()->GetTitleSize());
      hZDCP_L1_ZeroBias_Zoom->GetXaxis()->SetLabelSize((1/heightsmallpad) * hZDCP_L1_ZeroBias->GetXaxis()->GetLabelSize());
      hZDCP_L1_ZeroBias_Zoom->GetYaxis()->SetLabelSize((1/heightsmallpad) * hZDCP_L1_ZeroBias->GetYaxis()->GetLabelSize());

      hZDCP_L1_ZeroBias_Zoom->GetXaxis()->SetRangeUser(xminzoom, xmaxzoom);
      hZDCP_L1_ZeroBias_Zoom->GetYaxis()->SetRangeUser(yminzoom, ymaxzoom);
      hZDCM_L1_ZeroBias_Zoom->GetXaxis()->SetRangeUser(xminzoom, xmaxzoom);
      hZDCM_L1_ZeroBias_Zoom->GetYaxis()->SetRangeUser(yminzoom, ymaxzoom);

      hZDCM_L1_ZeroBias_Zoom->GetXaxis()->SetNdivisions(505);
      hZDCM_L1_ZeroBias_Zoom->GetYaxis()->SetNdivisions(504);
      hZDCM_L1_ZeroBias_Zoom->Draw();
      hZDCP_L1_ZeroBias_Zoom->Draw("same");

      //fit1nP3->Draw("same");
     // fit1nM3->Draw("same");
     // fit1nP4->Draw("same");
     // fit1nM4->Draw("same");
      TLatex* latex = new TLatex(0.45,0.90,"Zoom of 3n and 4n");
      latex->SetNDC();
      latex->SetTextSize(0.06);
      latex->SetTextFont(42);
  

      latex->Draw("same");
      c2_LHCC->cd();
      leg2_LHCC->Draw("same");


      for(auto const & extStr : extensions){
        c2_LHCC->SaveAs(Form("%s/Online_ZDCNeutronWithTrig_LHCC_%s_%s.%s",outDirPlot.c_str(),triglabel.c_str(), tag.c_str(), extStr.c_str()));
      }

       // ------------------------------------------
      /* Internal Check*/
      // ------------------------------------------

        TCanvas* c_internalcheck = new TCanvas("c_internalcheck","c_internalcheck",800,600);
        c_internalcheck->cd();
        c_internalcheck->SetTickx(1);
        c_internalcheck->SetTicky(1);
        c_internalcheck->SetTopMargin(0.09);
        c_internalcheck->SetBottomMargin(0.11);
        c_internalcheck->SetLeftMargin(0.13);
        c_internalcheck->SetRightMargin(0.05);

        TLegend* leg_internal = new TLegend(0.5,0.7,0.8,0.85);
        leg_internal->SetBorderSize(0);
        leg_internal->SetFillStyle(0);
        leg_internal->SetTextSize(0.03);

        TGraphErrors *grP = new TGraphErrors();
        TGraphErrors *grM = new TGraphErrors();

        grM->SetMarkerColor(kBlue);
        grM->SetMarkerStyle(20);
        grM->SetMarkerSize(1.2); 
        grM->SetLineColor(kBlue);


        grP->SetMarkerColor(kRed);
        grP->SetMarkerStyle(20);
        grP->SetMarkerSize(1.2);
        grP->SetLineColor(kRed);

        grM->SetPoint(0, 1.0, muM1);      // point 0: x=1, mean=10
        grM->SetPointError(0, 0.0, sigM1);  // x error=0, y error=std dev
        grP->SetPoint(0, 1.0, muP1);
        grP->SetPointError(0, 0, sigP1);

        grM->SetPoint(1, 2.0, muM2);      // point 0: x=1, mean=10
        grM->SetPointError(1, 0, sigM2);
        grP->SetPoint(1, 2.0, muP2);
        grP->SetPointError(1, 0.0, sigP2);

        grM->SetPoint(2, 3.0, muM3);      // point 0: x=1, mean=10
        grM->SetPointError(2, 0, sigM3);
        grP->SetPoint(2, 3.0, muP3);
        grP->SetPointError(2, 0, sigP3);

        grM->SetPoint(3, 4.0, muM4);      // point 0: x=1, mean=10
        grM->SetPointError(3, 0, sigM4);
        grP->SetPoint(3, 4.0, muP4);
        grP->SetPointError(3, 0, sigP4);

        grM->GetXaxis()->SetTitle("Peak Number");
        grM->GetYaxis()->SetTitle("Mean of Gaussian Fit (GeV)");
        grM->SetTitle("Mean of Gaussian Fits for ZDC Peaks");
        grM->GetXaxis()->SetLimits(0.5, 4.5);
      //  grM->GetYaxis()->SetRangeUser(0, 15000);

        leg_internal->AddEntry(grM,"ZDC-","P");
        leg_internal->AddEntry(grP,"ZDC+","P");
        
        grM->Draw("AP");
        grP->Draw("P same");
        TLine *line = new TLine(0.5, 2680, 4.5, 2680);
        line->SetLineColor(kBlack);
        line->SetLineStyle(2); // dashed
        line->SetLineWidth(2);
        line->Draw("same");
        TLine *line2 = new TLine(0.5, 2680*2, 4.5, 2680*2);
        line2->SetLineColor(kBlack);
        line2->SetLineStyle(2); // dashed
        line2->SetLineWidth(2);
        line2->Draw("same");
        TLine *line3 = new TLine(0.5, 2680*3, 4.5, 2680*3);
        line3->SetLineColor(kBlack);
        line3->SetLineStyle(2); // dashed
        line3->SetLineWidth(2);
        line3->Draw("same");
        TLine *line4 = new TLine(0.5, 2680*4, 4.5, 2680*4);
        line4->SetLineColor(kBlack);
        line4->SetLineStyle(2); // dashed
        line4->SetLineWidth(2);
        line4->Draw("same");

        cmsWIP->Draw("same");
        nEventsTex->Draw("same");
        lumi->Draw("same");
        trigTex->Draw("same");

        leg_internal->AddEntry(line,"Expected Peak Position","l");
        leg_internal->Draw("same");


        for(auto const & extStr : extensions){
          c_internalcheck->SaveAs(Form("%s/Meanofpeakcheck_%s_%s.%s",outDirPlot.c_str(),triglabel.c_str(), tag.c_str(), extStr.c_str()));
        }

      cout << "Run404359 ls 450-475:" << endl;

      cout << "Ratio mu 2/mu 1 (minus):" << muM2/muM1 << endl;
      cout << "Ratio mu 2/mu 1 (Minus Upper Bound):" << (muM2+sigM2)/(muM1-sigM1) << endl;
      cout << "Ratio mu 2/mu 1 (Minus Lower Bound):" << (muM2-sigM2)/(muM1+sigM1) << endl;
      
      cout << endl;

      cout << "Ratio mu 2/mu 1 (plus):" << muP2/muP1 << endl;
      cout << "Ratio mu 2/mu 1 (Plus Upper Bound):" << (muP2+sigM2)/(muP1-sigP1) << endl;
      cout << "Ratio mu 2/mu 1 (Plus Lower Bound):" << (muP2-sigM2)/(muP1+sigP1) << endl;

      cout << endl;

      cout << "Ratio mu 3/mu 1 (minus):" << muM3/muM1 << endl;
      cout << "Ratio mu 3/mu 1 (Minus Upper Bound):" << (muM3+sigM3)/(muM1-sigM1) << endl;
      cout << "Ratio mu 3/mu 1 (Minus Lower Bound):" << (muM3-sigM3)/(muM1+sigM1) << endl;

      cout << endl;

      cout << "Ratio mu 3/mu 1 (plus):" << muP3/muP1 << endl;
      cout << "Ratio mu 3/mu 1 (Plus Upper Bound):" << (muP3+sigP3)/(muP1-sigP1) << endl;
      cout << "Ratio mu 3/mu 1 (Plus Lower Bound):" << (muP3-sigP3)/(muP1+sigP1) << endl;

      cout << endl;

      cout << "Ratio mu 4/mu 1 (minus):" << muM4/muM1 << endl;
      cout << "Ratio mu 4/mu 1 (Minus Upper Bound):" << (muM4+sigM4)/(muM1-sigM1) << endl;
      cout << "Ratio mu 4/mu 1 (Minus Lower Bound):" << (muM4-sigM4)/(muM1+sigM1) << endl;

      cout << endl;

      cout << "Ratio mu 4/mu 1 (plus):" << muP4/muP1 << endl;
      cout << "Ratio mu 4/mu 1 (Plus Upper Bound):" << (muP4+sigP4)/(muP1-sigP1) << endl;
      cout << "Ratio mu 4/mu 1 (Plus Lower Bound):" << (muP4-sigP4)/(muP1+sigP1) << endl;

      cout << "L" << __LINE__ << endl;
      //delete total_plus;
      delete fit1nP;
      delete fit1nM;
      delete fit1nP2;
      delete fit1nM2;
      delete fit1nP3;
      delete fit1nM3;
      delete fit1nP4;
      delete fit1nM4;
     // delete total_minus;
      delete latex;
      cout << "L" << __LINE__ << endl;
    }
    cout << "L" << __LINE__ << endl;
    delete f;
    delete env;
    delete cms;
    delete lumi;
    cout << "L" << __LINE__ << endl;
  }//end for loop over l1 map

