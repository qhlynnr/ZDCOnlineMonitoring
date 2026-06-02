  
  
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

void plottingMacroNew(){

    string inputfile = "/afs/cern.ch/user/x/xirong/ZDCOnlineMonitoring/plotting/output_HIForward_Run404394_69to112_Updated/plotZDCEmuHists_PhysicsHIForward_Run404394.root";
    string outDirPlot = "/afs/cern.ch/user/x/xirong/ZDCOnlineMonitoring/plotting/PlotNew/PlotsLumiLevel/";
    vector<string> extensions = {"png","C"}; //add the .C output if you want to do rapid edits after the fact

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
    int nEvents = nEventsHist->GetBinContent(1);
    int NTrig = env->GetValue("NTRIG",0);
    string cmsLabel = env->GetValue("cmsLabel","");
    string lumiStr = env->GetValue("lumiStr","");
    string tag = env->GetValue("tag","");
    cout << "NTrig: " << NTrig << endl;
    int histPos = 0;
    double maxJetEt = -1.0;
    int fitxmin1n = 2000;
    int fitymin1n = 4000;
    int fitxmin2n = 5000;
    int fitymin2n = 6500;       
    int fitxmin3n = 8000;
    int fitymin3n = 9000;
    int fitxmin4n = 11000;
    int fitymin4n = 12000;
    int xmax = 20000;
    int ymin = 5000;

    int xminzoom = 6000;
    int xmaxzoom = 20000;
    int yminzoom = 10000;
    int ymaxzoom = 50000;

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

      TLatex* lumisectionTex = new TLatex(xLatex,topY-0.12,"lumisection 69-112");
      lumisectionTex->SetNDC();
      lumisectionTex->SetTextSize(0.035);
      lumisectionTex->SetTextFont(42);

      // ------------------------------------------
      /* Online to GeV */
      // ------------------------------------------

      TLegend* leg2 = new TLegend(0.5,0.6,0.8,0.85);
      leg2->SetBorderSize(0);
      leg2->SetFillStyle(0);
      leg2->SetTextSize(0.03);

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
      hZDCP_L1_ZeroBias->SetLineColor(kRed);
      hZDCP_L1_ZeroBias->SetLineWidth(2);
      hZDCP_L1_ZeroBias->GetXaxis()->SetTitleSize(0.05);
      hZDCP_L1_ZeroBias->SetMarkerStyle(20);

      TF1* fit1nP = new TF1("fit1nP", "gaus", fitxmin1n, fitymin1n);
      fit1nP->SetLineColor(kRed+2);
      fit1nP->SetLineWidth(2);
      fit1nP->SetLineStyle(2);
      fit1nP->SetRange(0.,6000);
      hZDCP_L1_ZeroBias->Fit(fit1nP, "0", "", fitxmin1n, fitymin1n);

      TF1* fit1nM = new TF1("fit1nM", "gaus",  fitxmin1n, fitymin1n);
      fit1nM->SetLineColor(kBlue+2);
      fit1nM->SetLineWidth(2);
      fit1nM->SetLineStyle(2);
      fit1nM->SetRange(0.,6000);
      hZDCM_L1_ZeroBias->Fit(fit1nM, "0", "", fitxmin1n, fitymin1n);

      TF1* fit1nP2 = new TF1("fit1nP2", "gaus", fitxmin2n, fitymin2n);
      fit1nP2->SetLineColor(kRed+2);
      fit1nP2->SetLineWidth(2);
      fit1nP2->SetLineStyle(2);
      fit1nP2->SetRange(0.,20000);
      hZDCP_L1_ZeroBias->Fit(fit1nP2, "0", "", fitxmin2n, fitymin2n);

      TF1* fit1nM2 = new TF1("fit1nM2", "gaus",  fitxmin2n, fitymin2n);
      fit1nM2->SetLineColor(kBlue+2);
      fit1nM2->SetLineWidth(2);
      fit1nM2->SetLineStyle(2);
      fit1nM2->SetRange(0.,20000);
      hZDCM_L1_ZeroBias->Fit(fit1nM2, "0", "", fitxmin2n, fitymin2n);

      TF1* fit1nM3 = new TF1("fit1nM3", "gaus",  fitxmin3n, fitymin3n);
      fit1nM3->SetLineColor(kBlue+2);
      fit1nM3->SetLineWidth(2);
      fit1nM3->SetLineStyle(2);
      fit1nM3->SetRange(0.,20000);
      hZDCM_L1_ZeroBias->Fit(fit1nM3, "0", "", fitxmin3n, fitymin3n);

      TF1* fit1nP3 = new TF1("fit1nP3", "gaus", fitxmin3n, fitymin3n);
      fit1nP3->SetLineColor(kRed+2);
      fit1nP3->SetLineWidth(2);
      fit1nP3->SetLineStyle(2);
      fit1nP3->SetRange(0.,20000);
      hZDCP_L1_ZeroBias->Fit(fit1nP3, "0", "", fitxmin3n, fitymin3n);

      TF1* fit1nM4 = new TF1("fit1nM4", "gaus",  fitxmin4n, fitymin4n);
      fit1nM4->SetLineColor(kBlue+2);
      fit1nM4->SetLineWidth(2);
      fit1nM4->SetLineStyle(2);
      fit1nM4->SetRange(0.,20000);
      hZDCM_L1_ZeroBias->Fit(fit1nM4, "0", "", fitxmin4n, fitymin4n);

      TF1* fit1nP4 = new TF1("fit1nP4", "gaus", fitxmin4n, fitymin4n);
      fit1nP4->SetLineColor(kRed+2);
      fit1nP4->SetLineWidth(2);
      fit1nP4->SetLineStyle(2);
      fit1nP4->SetRange(0.,20000);
      hZDCP_L1_ZeroBias->Fit(fit1nP4, "0", "", fitxmin4n, fitymin4n);

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

      // zdc minus
      hZDCM_L1_ZeroBias->SetLineColor(kBlue);
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

      hZDCP_L1_ZeroBias->SetLineColor(kRed);
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

      hZDCM_L1_ZeroBias->SetLineColor(kBlue);
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

      hZDCM_L1_ZeroBias->SetLineColor(kBlue);
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

      hZDCP_L1_ZeroBias->SetLineColor(kRed);
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

      hZDCM_L1_ZeroBias->GetXaxis()->SetRangeUser(xminzoom, xmaxzoom);
      hZDCM_L1_ZeroBias->GetYaxis()->SetRangeUser(yminzoom, ymaxzoom);
      hZDCM_L1_ZeroBias->Draw();

      leg2->Draw("same");
      cms->Draw("same");
     //("same");
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

      hZDCP_L1_ZeroBias->GetXaxis()->SetRangeUser(xminzoom, xmaxzoom);
      hZDCP_L1_ZeroBias->GetYaxis()->SetRangeUser(yminzoom, ymaxzoom);
      hZDCP_L1_ZeroBias->Draw();

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

      cout << "L" << __LINE__ << endl;
      delete fit1nP;
      delete fit1nM;
      delete fit1nP2;
      delete fit1nM2;
      delete fit1nP3;
      delete fit1nM3;
      delete fit1nP4;
      delete fit1nM4;
      cout << "L" << __LINE__ << endl;
    }
    cout << "L" << __LINE__ << endl;
    delete f;
    delete env;
    delete cms;
    delete lumi;
    cout << "L" << __LINE__ << endl;
  }//end for loop over l1 map

