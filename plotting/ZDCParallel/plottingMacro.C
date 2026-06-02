  
  
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


  void plot(){

    TFile* inputfile = "/afs/cern.ch/user/x/xirong/ZDCOnlineMonitoring/plotting/output_new/plotZDCEmuHists_PhysicsHIForward_Run404394ls0069.root";

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

    const double topY = 0.86;
    const double bottomY = 0.36;
    const double xLatex = 0.17;

    TLatex* lumi = new TLatex(xLatex,topY,lumiStr.c_str());
    lumi->SetNDC();
    lumi->SetTextSize(0.035);
    lumi->SetTextFont(42);

  //For loop over the trigger set
    // ------------------------------------------
    /* ZDC Emu */
    // ------------------------------------------

    string nEventStr = Form("nEvents: %i",l1MapCounter[trig.first]);
    TLatex* nEvents = new TLatex(0.65,topY,nEventStr.c_str());
    nEvents->SetNDC();
    nEvents->SetTextSize(0.035);
    nEvents->SetTextFont(42);

    TLatex* trigTex = new TLatex(xLatex,topY-0.06,trig.first.c_str());
    trigTex->SetNDC();
    trigTex->SetTextSize(0.035);
    trigTex->SetTextFont(42);

    TLatex* lumisectionTex = new TLatex(xLatex,topY-0.12,("lumisection " + lsNumStr).c_str());
    lumisectionTex->SetNDC();
    lumisectionTex->SetTextSize(0.035);
    lumisectionTex->SetTextFont(42);

    TLegend* leg = new TLegend(0.6,0.7,0.8,0.85);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.045);

    int xmax = 14000;
    
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
    nEvents->Draw("same");
    trigTex->Draw("same");
    lumisectionTex->Draw("same");
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
    nEvents->Draw("same");
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
    nEvents->Draw("same");
    trigTex->Draw("same");
    lumisectionTex->Draw("same");
  
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
    nEvents->Draw("same");
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
    nEvents->Draw("same");
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
      nEvents->Draw("same");
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
      nEvents->Draw("same");
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
      nEvents->Draw("same");
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
      nEvents->Draw("same");
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
      nEvents->Draw("same");
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
      nEvents->Draw("same");
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
    nEvents->Draw("same");
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
    nEvents->Draw("same");
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
      nEvents->Draw("same");
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
      nEvents->Draw("same");
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
      nEvents->Draw("same");
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
      nEvents->Draw("same");
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
      nEvents->Draw("same");
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
      nEvents->Draw("same");
      for(auto const & extStr : extensions){
	      cANVRB->SaveAs(Form("%s/Online_AsymmNormVRelBxAbsZoom_%s_%s.%s", outDirPlot.c_str(),trig.first.c_str(), tag.c_str(), extStr.c_str()));
      }
      delete cANVRB;
    }

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
    hZDCP_withTrig[trig.first]->SetLineColor(kRed);
    hZDCP_withTrig[trig.first]->SetLineWidth(2);
    hZDCP_withTrig[trig.first]->GetXaxis()->SetTitleSize(0.05);
    hZDCP_withTrig[trig.first]->SetMarkerStyle(20);

    TF1* fit1nP = new TF1("fit1nP", "gaus", fitxmin1n, fitymin1n);
    fit1nP->SetLineColor(kRed+2);
    fit1nP->SetLineWidth(2);
    fit1nP->SetLineStyle(2);
    fit1nP->SetRange(0.,6000);
    hZDCP_withTrig[trig.first]->Fit(fit1nP, "0", "", fitxmin1n, fitymin1n);

    TF1* fit1nM = new TF1("fit1nM", "gaus",  fitxmin1n, fitymin1n);
    fit1nM->SetLineColor(kBlue+2);
    fit1nM->SetLineWidth(2);
    fit1nM->SetLineStyle(2);
    fit1nM->SetRange(0.,6000);
    hZDCM_withTrig[trig.first]->Fit(fit1nM, "0", "", fitxmin1n, fitymin1n);

    TF1* fit1nP2 = new TF1("fit1nP2", "gaus", fitxmin2n, fitymin2n);
    fit1nP2->SetLineColor(kRed+2);
    fit1nP2->SetLineWidth(2);
    fit1nP2->SetLineStyle(2);
    fit1nP2->SetRange(0.,14000);
    hZDCP_withTrig[trig.first]->Fit(fit1nP2, "0", "", fitxmin2n, fitymin2n);

    TF1* fit1nM2 = new TF1("fit1nM2", "gaus",  fitxmin2n, fitymin2n);
    fit1nM2->SetLineColor(kBlue+2);
    fit1nM2->SetLineWidth(2);
    fit1nM2->SetLineStyle(2);
    fit1nM2->SetRange(0.,14000);
    hZDCM_withTrig[trig.first]->Fit(fit1nM2, "0", "", fitxmin2n, fitymin2n);

    TF1* fit1nM3 = new TF1("fit1nM3", "gaus",  fitxmin3n, fitymin3n);
    fit1nM3->SetLineColor(kBlue+2);
    fit1nM3->SetLineWidth(2);
    fit1nM3->SetLineStyle(2);
    fit1nM3->SetRange(0.,14000);
    hZDCM_withTrig[trig.first]->Fit(fit1nM3, "0", "", fitxmin3n, fitymin3n);

    TF1* fit1nP3 = new TF1("fit1nP3", "gaus", fitxmin3n, fitymin3n);
    fit1nP3->SetLineColor(kRed+2);
    fit1nP3->SetLineWidth(2);
    fit1nP3->SetLineStyle(2);
    fit1nP3->SetRange(0.,14000);
    hZDCP_withTrig[trig.first]->Fit(fit1nP3, "0", "", fitxmin3n, fitymin3n);

    TF1* fit1nM4 = new TF1("fit1nM4", "gaus",  fitxmin4n, fitymin4n);
    fit1nM4->SetLineColor(kBlue+2);
    fit1nM4->SetLineWidth(2);
    fit1nM4->SetLineStyle(2);
    fit1nM4->SetRange(0.,14000);
    hZDCM_withTrig[trig.first]->Fit(fit1nM4, "0", "", fitxmin4n, fitymin4n);

    TF1* fit1nP4 = new TF1("fit1nP4", "gaus", fitxmin4n, fitymin4n);
    fit1nP4->SetLineColor(kRed+2);
    fit1nP4->SetLineWidth(2);
    fit1nP4->SetLineStyle(2);
    fit1nP4->SetRange(0.,14000);
    hZDCP_withTrig[trig.first]->Fit(fit1nP4, "0", "", fitxmin4n, fitymin4n);

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
    hZDCM_withTrig[trig.first]->SetLineColor(kBlue);
    hZDCM_withTrig[trig.first]->SetLineWidth(2);
    hZDCM_withTrig[trig.first]->SetMarkerStyle(20);
    hZDCM_withTrig[trig.first]->GetXaxis()->SetTitle("ZDC Offline Energy Sum (GeV)");
    hZDCM_withTrig[trig.first]->GetXaxis()->SetTitleSize(0.05);
    hZDCM_withTrig[trig.first]->GetXaxis()->SetRangeUser(0, xmax);

    double ymax1 = std::max(hZDCM_withTrig[trig.first]->GetMaximum(), hZDCP_withTrig[trig.first]->GetMaximum()) * 1.5;
    hZDCM_withTrig[trig.first]->GetYaxis()->SetRangeUser(1, ymax1);
    hZDCM_withTrig[trig.first]->Draw();
    hZDCP_withTrig[trig.first]->Draw("same");
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

    leg2->AddEntry(hZDCP_withTrig[trig.first],"ZDC Plus ","l");
    leg2->AddEntry(hZDCM_withTrig[trig.first],"ZDC Minus ","l");
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
    nEvents->Draw("same");
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
      c2->SaveAs(Form("%s/Online_ZDCNeutronWithTrig_%s_%s.%s",outDirPlot.c_str(),trig.first.c_str(), tag.c_str(), extStr.c_str()));
    }


    // ------------------------------------------
    /* Online to GeV (nocut)*/
    // ------------------------------------------
    TLegend* leg2_nocut = new TLegend(0.5,0.6,0.8,0.85);
    leg2_nocut->SetBorderSize(0);
    leg2_nocut->SetFillStyle(0);
    leg2_nocut->SetTextSize(0.03);

    TCanvas* c_nocut = new TCanvas("c_nocut","c_nocut",800,600);
    c_nocut->cd();
    c_nocut->SetTickx(1);
    c_nocut->SetTicky(1);
    c_nocut->SetLogy();
    c_nocut->SetTopMargin(0.09);
    c_nocut->SetBottomMargin(0.11);
    c_nocut->SetLeftMargin(0.09);
    c_nocut->SetRightMargin(0.05);

    // zdc plus
    hZDCP_withTrig_nocut[trig.first]->SetLineColor(kRed);
    hZDCP_withTrig_nocut[trig.first]->SetLineWidth(2);
    hZDCP_withTrig_nocut[trig.first]->GetXaxis()->SetTitleSize(0.05);
    hZDCP_withTrig_nocut[trig.first]->SetMarkerStyle(20);

    // zdc minus
    hZDCM_withTrig_nocut[trig.first]->SetLineColor(kBlue);
    hZDCM_withTrig_nocut[trig.first]->SetLineWidth(2);
    hZDCM_withTrig_nocut[trig.first]->SetMarkerStyle(20);
    hZDCM_withTrig_nocut[trig.first]->GetXaxis()->SetTitle("ZDC Offline Energy Sum (GeV)");
    hZDCM_withTrig_nocut[trig.first]->GetXaxis()->SetTitleSize(0.05);
    hZDCM_withTrig_nocut[trig.first]->GetXaxis()->SetRangeUser(0, xmax);

    hZDCM_withTrig_nocut[trig.first]->Draw();
    hZDCP_withTrig_nocut[trig.first]->Draw("same");

    double ymax = std::max(hZDCM_withTrig_nocut[trig.first]->GetMaximum(), hZDCP_withTrig_nocut[trig.first]->GetMaximum()) * 1.5;
    hZDCM_withTrig_nocut[trig.first]->GetYaxis()->SetRangeUser(1, ymax);

    TF1* fit1nP_nocut = new TF1("fit1nP_nocut", "gaus", fitxmin1n, fitymin1n);
    fit1nP_nocut->SetLineColor(kRed+2);
    fit1nP_nocut->SetLineWidth(2);
    fit1nP_nocut->SetLineStyle(2);
    fit1nP_nocut->SetRange(0.,6000);
    hZDCP_withTrig_nocut[trig.first]->Fit(fit1nP_nocut, "0", "", fitxmin1n, fitymin1n);

    TF1* fit1nM_nocut = new TF1("fit1nM_nocut", "gaus",  fitxmin1n, fitymin1n);
    fit1nM_nocut->SetLineColor(kBlue+2);
    fit1nM_nocut->SetLineWidth(2);
    fit1nM_nocut->SetLineStyle(2);
    fit1nM_nocut->SetRange(0.,6000);
    hZDCM_withTrig_nocut[trig.first]->Fit(fit1nM_nocut, "0", "", 1000, 3000);

    TF1* fit1nP2_nocut = new TF1("fit1nP2_nocut", "gaus", fitxmin2n, fitymin2n);
    fit1nP2_nocut->SetLineColor(kRed+2);
    fit1nP2_nocut->SetLineWidth(2);
    fit1nP2_nocut->SetLineStyle(2);
    fit1nP2_nocut->SetRange(0.,14000);
    hZDCP_withTrig_nocut[trig.first]->Fit(fit1nP2_nocut, "0", "", fitxmin2n, fitymin2n);

    TF1* fit1nM2_nocut = new TF1("fit1nM2_nocut", "gaus",  fitxmin2n, fitymin2n);
    fit1nM2_nocut->SetLineColor(kBlue+2);
    fit1nM2_nocut->SetLineWidth(2);
    fit1nM2_nocut->SetLineStyle(2);
    fit1nM2_nocut->SetRange(0.,14000);
    hZDCM_withTrig_nocut[trig.first]->Fit(fit1nM2_nocut, "0", "", fitxmin2n, fitymin2n);

    TF1* fit1nM3_nocut = new TF1("fit1nM3_nocut", "gaus",  fitxmin3n, fitymin3n);
    fit1nM3_nocut->SetLineColor(kBlue+2);
    fit1nM3_nocut->SetLineWidth(2);
    fit1nM3_nocut->SetLineStyle(2);
    fit1nM3_nocut->SetRange(0.,14000);
    hZDCM_withTrig_nocut[trig.first]->Fit(fit1nM3_nocut, "0", "", fitxmin3n, fitymin3n);

    TF1* fit1nP3_nocut = new TF1("fit1nP3_nocut", "gaus", fitxmin3n, fitymin3n);
    fit1nP3_nocut->SetLineColor(kRed+2);
    fit1nP3_nocut->SetLineWidth(2);
    fit1nP3_nocut->SetLineStyle(2);
    fit1nP3_nocut->SetRange(0.,14000);
    hZDCP_withTrig_nocut[trig.first]->Fit(fit1nP3_nocut, "0", "", fitxmin3n, fitymin3n);

    TF1* fit1nM4_nocut = new TF1("fit1nM4_nocut", "gaus",  fitxmin4n, fitymin4n);
    fit1nM4_nocut->SetLineColor(kBlue+2);
    fit1nM4_nocut->SetLineWidth(2);
    fit1nM4_nocut->SetLineStyle(2);
    fit1nM4_nocut->SetRange(0.,14000);
    hZDCM_withTrig_nocut[trig.first]->Fit(fit1nM4_nocut, "0", "", fitxmin4n, fitymin4n);

    TF1* fit1nP4_nocut = new TF1("fit1nP4_nocut", "gaus", fitxmin4n, fitymin4n);
    fit1nP4_nocut->SetLineColor(kRed+2);
    fit1nP4_nocut->SetLineWidth(2);
    fit1nP4_nocut->SetLineStyle(2);
    fit1nP4_nocut->SetRange(0.,14000);
    hZDCP_withTrig_nocut[trig.first]->Fit(fit1nP4_nocut, "0", "", fitxmin4n, fitymin4n);

    muM1 = fit1nM_nocut->GetParameter(1);
    sigM1 = fit1nM_nocut->GetParameter(2);

    muP1 = fit1nP_nocut->GetParameter(1);
    sigP1 = fit1nP_nocut->GetParameter(2);

    muM2  = fit1nM2_nocut->GetParameter(1);
    sigM2 = fit1nM2_nocut->GetParameter(2);

    muP2  = fit1nP2_nocut->GetParameter(1);
    sigP2 = fit1nP2_nocut->GetParameter(2);

    muM3  = fit1nM3_nocut->GetParameter(1);
    sigM3 = fit1nM3_nocut->GetParameter(2);

    muP3  = fit1nP3_nocut->GetParameter(1);
    sigP3 = fit1nP3_nocut->GetParameter(2);

    muP4  = fit1nP4_nocut->GetParameter(1);
    sigP4 = fit1nP4_nocut->GetParameter(2);

    muM4  = fit1nM4_nocut->GetParameter(1);
    sigM4 = fit1nM4_nocut->GetParameter(2);

    // cout << "Integral Plus: " << hZDCP_withTrig->Integral() << endl;
    // cout << "Integral Minus: " << hZDCM_withTrig->Integral() << endl;

    fit1nP_nocut->Draw("same");
    fit1nM_nocut->Draw("same");
    fit1nP2_nocut->Draw("same");
    fit1nM2_nocut->Draw("same");
    fit1nP3_nocut->Draw("same");
    fit1nM3_nocut->Draw("same");
    fit1nP4_nocut->Draw("same");
    fit1nM4_nocut->Draw("same");

    leg2_nocut->AddEntry(hZDCP_withTrig[trig.first],"ZDC Plus ","l");
    leg2_nocut->AddEntry(hZDCM_withTrig[trig.first],"ZDC Minus ","l");
    leg2_nocut->AddEntry(fit1nM_nocut,Form("ZDC- peak 1n: #mu=%.1f, #sigma=%.1f", muM1, sigM1), "l");
    leg2_nocut->AddEntry(fit1nP_nocut,Form("ZDC+ peak 1n: #mu=%.1f, #sigma=%.1f", muP1, sigP1), "l");
    leg2_nocut->AddEntry(fit1nM2_nocut,Form("ZDC- peak 2n: #mu=%.1f, #sigma=%.1f", muM2, sigM2), "l");
    leg2_nocut->AddEntry(fit1nP2_nocut,Form("ZDC+ peak 2n: #mu=%.1f, #sigma=%.1f", muP2, sigP2), "l");
    leg2_nocut->AddEntry(fit1nM3_nocut,Form("ZDC- peak 3n: #mu=%.1f, #sigma=%.1f", muM3, sigM3), "l");
    leg2_nocut->AddEntry(fit1nP3_nocut,Form("ZDC+ peak 3n: #mu=%.1f, #sigma=%.1f", muP3, sigP3), "l");
    leg2_nocut->AddEntry(fit1nM4_nocut,Form("ZDC- peak 4n: #mu=%.1f, #sigma=%.1f", muM4, sigM4), "l");
    leg2_nocut->AddEntry(fit1nP4_nocut,Form("ZDC+ peak 4n: #mu=%.1f, #sigma=%.1f", muP4, sigP4), "l");

    leg2_nocut->Draw("same");
    cms->Draw("same");
    nEvents->Draw("same");
    lumi->Draw("same");
    trigTex->Draw("same");
    lumisectionTex->Draw("same");
    for(auto const & extStr : extensions){
      c_nocut->SaveAs(Form("%s/Online_ZDCNeutronWithTrig_nocut_%s_%s.%s",outDirPlot.c_str(),trig.first.c_str(), tag.c_str(), extStr.c_str()));
    }

    // ------------------------------------------
    /* Online to GeV (nocut P)*/
    // ------------------------------------------
    TLegend* leg2_nocut_P = new TLegend(0.5,0.6,0.8,0.85);
    leg2_nocut_P->SetBorderSize(0);
    leg2_nocut_P->SetFillStyle(0);
    leg2_nocut_P->SetTextSize(0.03);

    TCanvas* c_nocut_P = new TCanvas("c_nocut_P","c_nocut_P",800,600);
    c_nocut_P->cd();
    c_nocut_P->SetTickx(1);
    c_nocut_P->SetTicky(1);
    c_nocut_P->SetLogy();
    c_nocut_P->SetTopMargin(0.09);
    c_nocut_P->SetBottomMargin(0.11);
    c_nocut_P->SetLeftMargin(0.09);
    c_nocut_P->SetRightMargin(0.05);

    // zdc plus
    hZDCP_withTrig_nocut[trig.first]->SetLineColor(kRed);
    hZDCP_withTrig_nocut[trig.first]->SetLineWidth(2);
    hZDCP_withTrig_nocut[trig.first]->GetXaxis()->SetTitleSize(0.05);
    hZDCP_withTrig_nocut[trig.first]->SetMarkerStyle(20);
    hZDCP_withTrig_nocut[trig.first]->GetXaxis()->SetTitle("ZDC Offline Energy Sum (GeV)");
    hZDCP_withTrig_nocut[trig.first]->GetXaxis()->SetTitleSize(0.05);
    hZDCP_withTrig_nocut[trig.first]->GetXaxis()->SetRangeUser(0, xmax);

    hZDCP_withTrig_nocut[trig.first]->Draw();
    ymax = std::max(hZDCM_withTrig_nocut[trig.first]->GetMaximum(), hZDCP_withTrig_nocut[trig.first]->GetMaximum()) * 1.5;
    hZDCP_withTrig_nocut[trig.first]->GetYaxis()->SetRangeUser(1, ymax);

    muP1 = fit1nP_nocut->GetParameter(1);
    sigP1 = fit1nP_nocut->GetParameter(2);
    muP2  = fit1nP2_nocut->GetParameter(1);
    sigP2 = fit1nP2_nocut->GetParameter(2);
    muP3  = fit1nP3_nocut->GetParameter(1);
    sigP3 = fit1nP3_nocut->GetParameter(2);
    muP4  = fit1nP4_nocut->GetParameter(1);
    sigP4 = fit1nP4_nocut->GetParameter(2);

    fit1nP_nocut->Draw("same");
    fit1nP2_nocut->Draw("same");
    fit1nP3_nocut->Draw("same");
    fit1nP4_nocut->Draw("same");

    leg2_nocut_P->AddEntry(hZDCP_withTrig[trig.first],"ZDC Plus ","l");
    leg2_nocut_P->AddEntry(fit1nP_nocut,Form("ZDC+ peak 1n: #mu=%.1f, #sigma=%.1f", muP1, sigP1), "l");
    leg2_nocut_P->AddEntry(fit1nP2_nocut,Form("ZDC+ peak 2n: #mu=%.1f, #sigma=%.1f", muP2, sigP2), "l");
    leg2_nocut_P->AddEntry(fit1nP3_nocut,Form("ZDC+ peak 3n: #mu=%.1f, #sigma=%.1f", muP3, sigP3), "l");
    leg2_nocut_P->AddEntry(fit1nP4_nocut,Form("ZDC+ peak 4n: #mu=%.1f, #sigma=%.1f", muP4, sigP4), "l");
    leg2_nocut_P->Draw("same");
    cms->Draw("same");
    nEvents->Draw("same");
    lumi->Draw("same");
    trigTex->Draw("same");
    lumisectionTex->Draw("same");
    for(auto const & extStr : extensions){
      c_nocut_P->SaveAs(Form("%s/Online_ZDCNeutronWithTrig_nocut_PlusOnly_%s_%s.%s",outDirPlot.c_str(),trig.first.c_str(), tag.c_str(), extStr.c_str()));
    }

    // ------------------------------------------
    /* Online to GeV (nocut M)*/
    // ------------------------------------------
    TLegend* leg2_nocut_M = new TLegend(0.5,0.6,0.8,0.85);
    leg2_nocut_M->SetBorderSize(0);
    leg2_nocut_M->SetFillStyle(0);
    leg2_nocut_M->SetTextSize(0.03);

    TCanvas* c_nocut_M = new TCanvas("c_nocut_M","c_nocut_M",800,600);
    c_nocut_M->cd();
    c_nocut_M->SetTickx(1);
    c_nocut_M->SetTicky(1);
    c_nocut_M->SetLogy();
    c_nocut_M->SetTopMargin(0.09);
    c_nocut_M->SetBottomMargin(0.11);
    c_nocut_M->SetLeftMargin(0.09);
    c_nocut_M->SetRightMargin(0.05);

    // zdc minus
    hZDCM_withTrig_nocut[trig.first]->SetLineColor(kBlue);
    hZDCM_withTrig_nocut[trig.first]->SetLineWidth(2);
    hZDCM_withTrig_nocut[trig.first]->SetMarkerStyle(20);
    hZDCM_withTrig_nocut[trig.first]->GetXaxis()->SetTitle("ZDC Offline Energy Sum (GeV)");
    hZDCM_withTrig_nocut[trig.first]->GetXaxis()->SetTitleSize(0.05);
    hZDCM_withTrig_nocut[trig.first]->GetXaxis()->SetRangeUser(0, xmax);

    hZDCM_withTrig_nocut[trig.first]->Draw();

    ymax = std::max(hZDCM_withTrig_nocut[trig.first]->GetMaximum(), hZDCP_withTrig_nocut[trig.first]->GetMaximum()) * 1.5;
    hZDCM_withTrig_nocut[trig.first]->GetYaxis()->SetRangeUser(1, ymax);
    // cout << "Integral Plus: " << hZDCP_withTrig->Integral() << endl;
    // cout << "Integral Minus: " << hZDCM_withTrig->Integral() << endl;

    fit1nM_nocut->Draw("same");
    fit1nM2_nocut->Draw("same");
    fit1nM3_nocut->Draw("same");
    fit1nM4_nocut->Draw("same");

    leg2_nocut_M->AddEntry(hZDCM_withTrig[trig.first],"ZDC Minus ","l");
    leg2_nocut_M->AddEntry(fit1nM_nocut,Form("ZDC- peak 1n: #mu=%.1f, #sigma=%.1f", muM1, sigM1), "l");
    leg2_nocut_M->AddEntry(fit1nM2_nocut,Form("ZDC- peak 2n: #mu=%.1f, #sigma=%.1f", muM2, sigM2), "l");
    leg2_nocut_M->AddEntry(fit1nM3_nocut,Form("ZDC- peak 3n: #mu=%.1f, #sigma=%.1f", muM3, sigM3), "l");
    leg2_nocut_M->AddEntry(fit1nM4_nocut,Form("ZDC- peak 4n: #mu=%.1f, #sigma=%.1f", muM4, sigM4), "l");

    leg2_nocut->Draw("same");
    cms->Draw("same");
    nEvents->Draw("same");
    lumi->Draw("same");
    trigTex->Draw("same");
    lumisectionTex->Draw("same");
    for(auto const & extStr : extensions){
      c_nocut_M->SaveAs(Form("%s/Online_ZDCNeutronWithTrig_nocut_MinusOnly_%s_%s.%s",outDirPlot.c_str(),trig.first.c_str(), tag.c_str(), extStr.c_str()));
    }

    // ------------------------------------------
    /* Online to GeV (norm) */
    // ------------------------------------------
    TCanvas* c2_norm = new TCanvas("c2_norm","c2_norm",800,600);
    c2_norm->cd();
    c2_norm->SetTickx(1);
    c2_norm->SetTicky(1);
    c2_norm->SetLogy();
    c2_norm->SetTopMargin(0.09);
    c2_norm->SetBottomMargin(0.11);
    c2_norm->SetLeftMargin(0.09);
    c2_norm->SetRightMargin(0.05);

    // zdc plus
    TH1 *hZDCP_norm = (TH1*)hZDCP_withTrig[trig.first]->Clone();
    hZDCP_norm->Scale(1.0 / totalEvents);

    // zdc minus
    TH1 *hZDCM_norm = (TH1*)hZDCM_withTrig[trig.first]->Clone();
    hZDCM_norm->Scale(1.0 / totalEvents);
  
    hZDCP_norm->SetLineColor(kRed);
    hZDCP_norm->SetLineWidth(2);
    hZDCP_norm->GetXaxis()->SetTitleSize(0.05);
    hZDCP_norm->SetMarkerStyle(20);

    // zdc minus
    hZDCM_norm->SetLineColor(kBlue);
    hZDCM_norm->SetLineWidth(2);
    hZDCM_norm->SetMarkerStyle(20);

    TF1* fit1n_norm = new TF1("fit1n_norm", "gaus", fitxmin1n, fitymin1n);
    fit1n_norm->SetLineColor(kBlack);
    fit1n_norm->SetLineWidth(2);
    fit1n_norm->SetLineStyle(2);
    fit1n_norm->SetRange(0.,6000);
    hZDCP_norm->Fit(fit1n_norm, "0", "", fitxmin1n, fitymin1n);

    TF1* fit1nM_norm = new TF1("fit1nM_norm", "gaus",  fitxmin1n, fitymin1n);
    fit1nM_norm->SetLineColor(kBlue+2);
    fit1nM_norm->SetLineWidth(2);
    fit1nM_norm->SetLineStyle(2);
    fit1nM_norm->SetRange(0.,6000);
    hZDCM_norm->Fit(fit1nM_norm, "0", "", fitxmin1n, fitymin1n);

    hZDCP_norm->Draw("HIST");
    hZDCM_norm->Draw("HIST same");

    leg2->Draw("same");
    cms->Draw("same");
    nEvents->Draw("same");
    lumi->Draw("same");
    trigTex->Draw("same");
    fit1n_norm->Draw("same");
    fit1nM_norm->Draw("same");
    c2_norm->Draw("same");
    for(auto const & extStr : extensions){
      c2_norm->SaveAs(Form("%s/Online_ZDCNeutronWithTrigNormalized_%s_%s.%s",outDirPlot.c_str(),trig.first.c_str(), tag.c_str(), extStr.c_str()));
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
    clonePlus.GetXaxis()->SetRangeUser(0, xmax);
    clonePlus.Draw("");
    cloneMinus.Draw("lp same");
    cms->Draw("same");
    nEvents->Draw("same");
    lumi->Draw("same");
    trigTex->Draw("same");
    leg4->Draw("same");
    lumisectionTex->Draw("same");
    for(auto const & extStr : extensions){
      c4->SaveAs(Form("%s/TurnOn_%s_%s.%s",outDirPlot.c_str(), trig.first.c_str(), tag.c_str(), extStr.c_str()));
    }


    delete trigTex;
    //cleanup for loop
    delete c4;
    delete leg4;
    delete fit1nM;
    delete fit1nP;
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
  }
