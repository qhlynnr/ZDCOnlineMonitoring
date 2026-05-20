void analyzePedestalsGlobal(std::string filename, int runNumber); // analyzer for global runs, uses ZDC analyzer
void analyzePedestalsLocal(TFile* inFile, int runNumber, bool doDummy); 
void analyzePedestals(); // main function
void FillChain(TChain &chain, std::string filename); 


/// main function to analyze pedestals 
void analyzePedstals(){
    int runNumber = 404027; 
    std::string filename = Form("ZDCAnalyzer_USC_%d.root", runNumber); 
    TFile* inFile = TFile::Open(filename.c_str()); 
    bool isLocal = true; 
   

    if(isLocal){
        std::cout << "~~~~~ Analyzing pedestals from a LOCAL run: " << filename.c_str() << " ~~~~~" << std::endl;
        analyzePedestalsLocal(inFile, runNumber, false);
    }
    else{
        std::cout << "~~~~~ Analyzing pedestals from a GLOBAL run: " << filename.c_str() << " ~~~~~" << std::endl;
        analyzePedestalsGlobal(filename, runNumber); 
    }

}


// ---------------------------------------------------------------------
/// ------------ analyze pedestals for global running -------------------
// ---------------------------------------------------------------------
void analyzePedestalsGlobal(std::string filename, int runNumber){
    
    TChain zdcChain("zdcanalyzer/zdcdigi");
    FillChain(zdcChain, filename);
    TTreeReader zdcReader(&zdcChain);
    // TTreeReaderValue<float> sumPlus(zdcReader, "sumPlus");
    // TTreeReaderValue<float> sumMinus(zdcReader, "sumMinus");
    TTreeReaderArray<int> z_side(zdcReader, "zside");
    TTreeReaderArray<int> sec(zdcReader, "section");
    TTreeReaderArray<int> ch(zdcReader, "channel");
    TTreeReaderArray<int> adc0(zdcReader, "adcTs0"); 
    TTreeReaderArray<int> adc1(zdcReader, "adcTs1"); 
    TTreeReaderArray<int> adc2(zdcReader, "adcTs2"); 
    TTreeReaderArray<int> adc3(zdcReader, "adcTs3"); 
    TTreeReaderArray<int> adc4(zdcReader, "adcTs4");
    TTreeReaderArray<int> adc5(zdcReader, "adcTs5");
    TTreeReaderValue<int> N(zdcReader, "n");

    std::string namesNoSpace[20] = {"hZDCM_EM1", "hZDCM_EM2", "hZDCM_EM3", "hZDCM_EM4",  "hZDCM_EM5",  "hZDCM_HAD1", "hZDCM_HAD2", "hZDCM_HAD3", "hZDCM_HAD4", "hZDCP_EM1", "hZDCP_EM2", "hZDCP_EM3", "hZDCP_EM4",  "hZDCP_EM5",  "hZDCP_HAD1", "hZDCP_HAD2", "hZDCP_HAD3", "hZDCP_HAD4"};
    int colors[20] = {kRed+2, kRed-4, kOrange+7, kOrange, kYellow-4, kSpring+10, kSpring, kGreen-3, kGreen+3, kTeal-7, kTeal, kAzure+10, kAzure-4, kBlue+2, kViolet+8, kViolet-1, kMagenta+1, kMagenta-4, kPink+7, kPink-4};
    std::string ColorStrings[10] = {"#3f90da", "#ffa90e", "#bd1f01", "#94a4a2", "#832db6", "#a96b59", "#e76300", "#b9ac70", "#717581", "#92dadd"};//{"#5790fc", "#f89c20", "#e42536", "#964a8b", "#9c9ca1", "#7a21dd"};
    std::vector<TH1D*> adcHists; 
    for(int h = 0; h < 18; h++){
        TH1D* adcHist = new TH1D(Form("ADC_%d", h), Form("ADC_%d", h), 255, 0, 255); 
        adcHists.push_back(adcHist); 
    } 
    
    std::vector<TH1D*> fscHists; 
    for(int h = 0; h < 6; h++){
        TH1D* fscHist = new TH1D(Form("ADC_FSC_%d", h), Form("ADC_FSC_%d", h), 255, 0, 255); 
        fscHists.push_back(fscHist); 
    }

    std::vector<TH1D*> rpdHists; 
    for(int h = 0; h < 16; h++){
        TH1D* rpdHist = new TH1D(Form("ADC_RPD_%d", h), Form("ADC_RPD_%d", h), 255, 0, 255); 
        rpdHists.push_back(rpdHist); 
    }  

    Long64_t totalEvents = zdcReader.GetEntries(true);
    std::cout << "Total number of events " << totalEvents << std::endl;
    for (Long64_t i = 0; i < totalEvents; i++) {
        zdcReader.Next();
        for (int j = 0; j < *N; j++) {
            // require that we be on the plus or minus side
            if (z_side[j] != -1 && z_side[j] != 1)
                continue;
            //if(sec[j] == 4 && z_side[j] == -1)rpdHists.Fill(adc2[j]);
            // requre that it is EM or HAD
            if (sec[j] != 1 && sec[j] != 2)
                continue;
            if (sec[j] == 2 && (ch[j] < 1 || ch[j] > 4))
                continue;
            // require that EM Channels are 1-5 and HAD channels are 1-4
            if (sec[j] == 1 && (ch[j] < 1 || ch[j] > 6)) fscHists.at(ch[j]-7)->Fill(adc2[j]);
            if (sec[j] == 1 && (ch[j] < 1 || ch[j] > 5))continue;
            int idet = (z_side[j] == 1 ? 9 : 0) + (sec[j] == 2 ? 5 : 0) + (ch[j] - 1);
            adcHists.at(idet)->Fill(adc2[j]); 
        }
    }

    // now plot in the same style as before 
    gStyle->SetOptTitle(0); 
    gStyle->SetOptStat(0); 


    for(int i = 0; i < 18; i++){
        int colorIndex; 
        if(i > 9)colorIndex = i-9; 
        else colorIndex = i; 
        adcHists[i]->SetLineColor(TColor::GetColor(ColorStrings[colorIndex].c_str()));
        adcHists[i]->SetLineWidth(3);
        adcHists[i]->SetMarkerStyle(20);
        adcHists[i]->SetMarkerColor(TColor::GetColor(ColorStrings[colorIndex].c_str()));
        adcHists[i]->SetFillColor(TColor::GetColor(ColorStrings[colorIndex].c_str()));
        adcHists[i]->SetFillStyle(3004);
    }

    TCanvas* cADCMinus = new TCanvas("cADCMinus", "cADCMinus", 800, 600);
    cADCMinus->SetLogy();
    cADCMinus->SetRightMargin(0.05);
    adcHists[0]->SetLineColor(TColor::GetColor(ColorStrings[0].c_str()));
    adcHists[0]->SetLineWidth(3);
    adcHists[0]->SetMarkerStyle(20);
    adcHists[0]->SetMarkerColor(TColor::GetColor(ColorStrings[0].c_str()));
    adcHists[0]->SetFillColor(TColor::GetColor(ColorStrings[0].c_str()));
    adcHists[0]->SetFillStyle(3004);
    adcHists[0]->GetXaxis()->SetTitle("ADC Per Channel");
    adcHists[0]->GetYaxis()->SetRangeUser(1, 1e10);
    adcHists[0]->GetXaxis()->SetRangeUser(0, 250);
    adcHists[0]->Draw();
    TF1* fit = new TF1("fit1nM", "gaus",  0, 250);
    fit->SetLineColor(adcHists[0]->GetLineColor());
    fit->SetLineWidth(3);
    fit->SetLineStyle(2);
    fit->Draw("same");
    adcHists[0]->Fit(fit);



    TLegend* legADC = new TLegend(0.16, 0.57, 0.7, 0.88);
    legADC->SetBorderSize(0);
    legADC->SetTextSize(0.04);
    legADC->AddEntry(adcHists[0], Form("%s, #mu = %0.2f, #sigma = %0.2f", namesNoSpace[0].c_str(),fit->GetParameter(1), fit->GetParameter(2)), "f");


    for(int x = 1; x < 9; x++){
        adcHists[x]->Draw("same"); 
        TF1* fitPed = new TF1("fit1nM", "gaus",  1, 100);
        fitPed->SetLineColor(adcHists[x]->GetLineColor());
        fitPed->SetLineWidth(3);
        fitPed->SetLineStyle(2);
        adcHists[x]->Fit(fitPed); 
        fitPed->Draw("same");
        legADC->AddEntry(adcHists[x], Form("%s, #mu = %0.2f, #sigma = %0.2f", namesNoSpace[x].c_str(),fitPed->GetParameter(1), fitPed->GetParameter(2)), "f");

    }

    TLatex* cms = new TLatex(0.10,0.92,Form("#bf{ZDC} Run %d #it{Work in progress}", runNumber));
    cms->SetNDC();
    cms->SetTextSize(0.05);
    cms->SetTextFont(42);

    legADC->Draw();
    cms->Draw();
    cADCMinus->SaveAs(Form("ZDCAnalyzer_%d_Minus.pdf", runNumber));


    TCanvas* cADCPlus = new TCanvas("cADCPlus", "cADCPlus", 800, 600);
    cADCPlus->SetLogy();
    cADCPlus->SetRightMargin(0.05);
    adcHists[9]->SetLineColor(TColor::GetColor(ColorStrings[0].c_str()));
    adcHists[9]->SetLineWidth(3);
    adcHists[9]->SetMarkerStyle(20);
    adcHists[9]->SetMarkerColor(TColor::GetColor(ColorStrings[0].c_str()));
    adcHists[9]->SetFillColor(TColor::GetColor(ColorStrings[0].c_str()));
    adcHists[9]->SetFillStyle(3004);
    adcHists[9]->GetXaxis()->SetTitle("ADC Per Channel");
    adcHists[9]->GetYaxis()->SetRangeUser(1, 1e10);
    adcHists[9]->GetXaxis()->SetRangeUser(0, 250);
    adcHists[9]->Draw();
    cms->Draw(); 
    TLegend* legADCPlus = new TLegend(0.16, 0.57, 0.7, 0.88);
    legADCPlus->SetBorderSize(0);
    //legADC->SetNColumns(2);
     for(int x = 10; x < 18; x++){
        adcHists[x]->Draw("same");
    }
    for (int i = 9; i < 18; i++) {
        TF1* fitPed = new TF1("fit1nM", "gaus",  0, 250);
        fitPed->SetLineColor(adcHists[i]->GetLineColor());
        fitPed->SetLineWidth(3);
        fitPed->SetLineStyle(2);
        adcHists[i]->Fit(fitPed); 
        fitPed->Draw("same");
        legADCPlus->AddEntry(adcHists[i], Form("%s, #mu = %0.2f, #sigma = %0.2f", namesNoSpace[i].c_str(),fitPed->GetParameter(1), fitPed->GetParameter(2)), "f");
    }
    legADCPlus->SetTextSize(0.04);
    legADCPlus->Draw();
    cADCPlus->SaveAs(Form("ZDCAnalyzer_%d_Plus.pdf", runNumber));


        // Draw RPD Channels
    TCanvas* cADCRPD = new TCanvas("cADCRPD", "cADCRPD", 800, 600);
    cADCRPD->SetLogy();
    cADCRPD->SetRightMargin(0.05);
    cADCRPD->cd(); 

    rpdHists[0]->SetLineColor(TColor::GetColor(ColorStrings[0].c_str()));
    rpdHists[0]->SetLineWidth(3);
    rpdHists[0]->SetMarkerStyle(20);
    rpdHists[0]->SetMarkerColor(TColor::GetColor(ColorStrings[0].c_str()));
    rpdHists[0]->SetFillColor(TColor::GetColor(ColorStrings[0].c_str()));
    rpdHists[0]->SetFillStyle(3004);
    rpdHists[0]->GetXaxis()->SetTitle("ADC Per Channel");
    rpdHists[0]->GetYaxis()->SetRangeUser(1, 1e10);
    rpdHists[0]->GetXaxis()->SetRangeUser(0, 200);
    rpdHists[0]->Draw();
    cms->Draw(); 

    TLegend* legADCRPD = new TLegend(0.16, 0.57, 0.7, 0.88);
    legADCRPD->SetBorderSize(0);
     for(int x = 0; x < 16; x++){
        rpdHists[x]->SetLineColor(TColor::GetColor(ColorStrings[x].c_str()));
        rpdHists[x]->SetLineWidth(3);
        rpdHists[x]->SetMarkerStyle(20);
        rpdHists[x]->SetMarkerColor(TColor::GetColor(ColorStrings[x].c_str()));
        rpdHists[x]->SetFillColor(TColor::GetColor(ColorStrings[x].c_str()));
        rpdHists[x]->SetFillStyle(3004);
        rpdHists[x]->Draw("same"); 
        TF1* fitPed = new TF1("fitRPD", "gaus",  60, 100);
        fitPed->SetLineColor(rpdHists[x]->GetLineColor());
        fitPed->SetLineWidth(3);
        fitPed->SetLineStyle(2);
        rpdHists[x]->Fit(fitPed); 
        fitPed->Draw("same");
        legADCRPD->AddEntry(fscHists[x], Form("ZDCmRPD %d, #mu = %0.2f, #sigma = %0.2f", x+1,fitPed->GetParameter(1), fitPed->GetParameter(2)), "f");
     }
    legADCRPD->SetTextSize(0.04);
    legADCRPD->Draw();
    cADCRPD->SaveAs(Form("ZDCAnalyzer_%d_RPD.pdf", runNumber));

    // Draw FSC Channels
    TCanvas* cADCFSC = new TCanvas("cADCFSC", "cADCFSC", 800, 600);
    cADCFSC->SetLogy();
    cADCFSC->SetRightMargin(0.05);
    cADCFSC->cd(); 

    fscHists[0]->SetLineColor(TColor::GetColor(ColorStrings[0].c_str()));
    fscHists[0]->SetLineWidth(3);
    fscHists[0]->SetMarkerStyle(20);
    fscHists[0]->SetMarkerColor(TColor::GetColor(ColorStrings[0].c_str()));
    fscHists[0]->SetFillColor(TColor::GetColor(ColorStrings[0].c_str()));
    fscHists[0]->SetFillStyle(3004);
    fscHists[0]->GetXaxis()->SetTitle("ADC Per Channel");
    fscHists[0]->GetYaxis()->SetRangeUser(1, 1e10);
    fscHists[0]->GetXaxis()->SetRangeUser(0, 200);
    fscHists[0]->Draw();
    cms->Draw(); 

    std::string FSCNames[6] = {"FSC Station 2 - Top", "FSC Station 2 - Bottom", "FSC Station 3 - Bottom Left", "FSC Station 3 - Bottom Right", "FSC Station 3 - Top Left", "FSC Station 3 - Top Right"};
    TLegend* legADCFSC = new TLegend(0.16, 0.57, 0.7, 0.88);
    legADCFSC->SetBorderSize(0);
     for(int x = 0; x < 6; x++){
        fscHists[x]->SetLineColor(TColor::GetColor(ColorStrings[x].c_str()));
        fscHists[x]->SetLineWidth(3);
        fscHists[x]->SetMarkerStyle(20);
        fscHists[x]->SetMarkerColor(TColor::GetColor(ColorStrings[x].c_str()));
        fscHists[x]->SetFillColor(TColor::GetColor(ColorStrings[x].c_str()));
        fscHists[x]->SetFillStyle(3004);
        fscHists[x]->Draw("same"); 
        TF1* fitPed = new TF1("fitFSC", "gaus",  60, 100);
        fitPed->SetLineColor(fscHists[x]->GetLineColor());
        fitPed->SetLineWidth(3);
        fitPed->SetLineStyle(2);
        fscHists[x]->Fit(fitPed); 
        fitPed->Draw("same");
        legADCFSC->AddEntry(fscHists[x], Form("%s, #mu = %0.2f, #sigma = %0.2f", FSCNames[x].c_str(),fitPed->GetParameter(1), fitPed->GetParameter(2)), "f");
     }
    legADCFSC->SetTextSize(0.04);
    legADCFSC->Draw();
    cADCFSC->SaveAs(Form("ZDCAnalyzer_%d_FSC.pdf", runNumber));
}


// ---------------------------------------------------------------------
/// ------------ analyze pedestals for Local running -------------------
// ---------------------------------------------------------------------

void analyzePedestalsLocal(TFile* inFile , int runNumber, bool doDummy){
    TDirectoryFile* inDirect = (TDirectoryFile*)inFile->Get("zdcana"); 
    TDirectoryFile* adcDirect = (TDirectoryFile*)inDirect->Get("ADC");
    std::string namesNoSpace[32] = {"hZDCM_EM1", "hZDCM_EM2", "hZDCM_EM3", "hZDCM_EM4", "hZDCM_EM5", "hZDCM_HAD1", "hZDCM_HAD2", "hZDCM_HAD3", "hZDCM_HAD4", "hZDCP_EM1", "hZDCP_EM2", "hZDCP_EM3", "hZDCP_EM4",  "hZDCP_EM5",  "hZDCP_HAD1", "hZDCP_HAD2", "hZDCP_HAD3", "hZDCP_HAD4", "hZDCM_EM7", "hZDCM_EM8", "hZDCM_EM9", "hZDCM_EM10", "hZDCM_EM11", "hZDCM_EM12", "hZDCP_EM7", "hZDCP_EM8", "hZDCP_EM9", "hZDCP_EM10", "hZDCP_EM11", "hZDCP_EM12"};
    std::string namesNoSpaceRPD[32] = {"hZDCM_PRD0","hZDCM_PRD1","hZDCM_PRD2", "hZDCM_PRD3", "hZDCM_PRD4", "hZDCM_PRD5", "hZDCM_PRD6", "hZDCM_PRD7", "hZDCM_PRD8", "hZDCM_PRD9", "hZDCM_PRD10", "hZDCM_PRD11", "hZDCM_PRD12", "hZDCM_PRD13", "hZDCM_PRD14", "hZDCM_PRD15" };

    std::string namesNoSpaceDummy[20] = {"hZDCM_EM6", "hZDCM_EM7", "hZDCM_EM8", "hZDCM_RPD12", "hZDCM_RPD13", "hZDCM_RPD14", "hZDCM_RPD15", "hZDCP_EM6", "hZDCP_EM7", "hZDCP_EM8", "hZDCP_RPD12", "hZDCP_RPD13",  "hZDCP_RPD14",  "hZDCP_RPD15"};
    int colors[20] = {kRed+2, kRed-4, kOrange+7, kOrange, kYellow-4, kSpring+10, kSpring, kGreen-3, kGreen+3, kTeal-7, kTeal, kAzure+10, kAzure-4, kBlue+2, kViolet+8, kViolet-1, kMagenta+1, kMagenta-4, kPink+7, kPink-4};
    std::string ColorStrings[16] = {"#3f90da", "#ffa90e", "#bd1f01", "#94a4a2", "#832db6", "#a96b59", "#e76300", "#b9ac70", "#717581", "#92dadd", "#5790fc", "#f89c20", "#e42536", "#964a8b", "#9c9ca1", "#7a21dd"};
    int nHistsWithFSC = 30; 
    int nHists = 18; 
    if(doDummy){
        nHists = 14; 
    }

    // now get all the histograms
    std::vector<TH1D*> adcHists; 
    for(int h = 0; h < nHistsWithFSC; h++){
        std::cout <<  h << " Getting: " << namesNoSpace[h].c_str() << std::endl; 
        TH2D* twodHist; 
        if(doDummy) twodHist = (TH2D*)adcDirect->Get(namesNoSpaceDummy[h].c_str());
        else twodHist = (TH2D*)adcDirect->Get(namesNoSpace[h].c_str());
        int ts0Bin =  twodHist->GetXaxis()->FindBin(0.);  
        int ts1Bin =  twodHist->GetXaxis()->FindBin(1.);  
        int tsBin = twodHist->GetXaxis()->FindBin(2.);  
        int ts3Bin =  twodHist->GetXaxis()->FindBin(3.);  
        int ts4Bin =  twodHist->GetXaxis()->FindBin(4.);  
        int ts5Bin =  twodHist->GetXaxis()->FindBin(5.);  
        int ts6Bin =  twodHist->GetXaxis()->FindBin(6.); 
        TH1D* proj0 = (TH1D*)twodHist->ProjectionY(Form("%s_proj0", twodHist->GetName()), ts0Bin, ts0Bin); 
        TH1D* proj1 = (TH1D*)twodHist->ProjectionY(Form("%s_proj1", twodHist->GetName()), ts1Bin, ts1Bin); 
        TH1D* proj3 = (TH1D*)twodHist->ProjectionY(Form("%s_proj3", twodHist->GetName()), ts3Bin, ts3Bin); 
        TH1D* proj4 = (TH1D*)twodHist->ProjectionY(Form("%s_proj4", twodHist->GetName()), ts4Bin, ts4Bin); 
        TH1D* proj5 = (TH1D*)twodHist->ProjectionY(Form("%s_proj5", twodHist->GetName()), ts5Bin, ts5Bin); 
        TH1D* proj2 = (TH1D*)twodHist->ProjectionY(Form("%s_proj", twodHist->GetName()), tsBin, tsBin); 
        TH1D* proj6 = (TH1D*)twodHist->ProjectionY(Form("%s_proj6", twodHist->GetName()), ts6Bin, ts6Bin); 
        proj2->Add(proj0); 
        proj2->Add(proj1); 
        proj2->Add(proj3); 
        proj2->Add(proj4); 
        proj2->Add(proj5); 
        proj2->Add(proj6); 
        adcHists.push_back(proj2);
    }
    std::cout << "Number of adc hists " << adcHists.size() << std::endl;

 // now plot in the same style as before 
    gStyle->SetOptTitle(0); 
    gStyle->SetOptStat(0); 


    for(int i = 0; i < nHistsWithFSC; i++){
        std::cout << "here " << i << std::endl;
        int colorIndex; 
        if(i > nHists/2)colorIndex = i-nHists/2; 
        else colorIndex = i; 
        if(i > nHists) colorIndex = i - nHists; 
        adcHists[i]->SetLineColor(TColor::GetColor(ColorStrings[colorIndex].c_str()));
        adcHists[i]->SetLineWidth(3);
        adcHists[i]->SetMarkerStyle(20);
        adcHists[i]->SetMarkerColor(TColor::GetColor(ColorStrings[colorIndex].c_str()));
        adcHists[i]->SetFillColor(TColor::GetColor(ColorStrings[colorIndex].c_str()));
        adcHists[i]->SetFillStyle(3004);
    }

    TCanvas* cADCMinus = new TCanvas("cADCMinus", "cADCMinus", 800, 600);
    cADCMinus->SetLogy();
    cADCMinus->SetRightMargin(0.05);
    adcHists[0]->SetLineColor(TColor::GetColor(ColorStrings[0].c_str()));
    adcHists[0]->SetLineWidth(3);
    adcHists[0]->SetMarkerStyle(20);
    adcHists[0]->SetMarkerColor(TColor::GetColor(ColorStrings[0].c_str()));
    adcHists[0]->SetFillColor(TColor::GetColor(ColorStrings[0].c_str()));
    adcHists[0]->SetFillStyle(3004);
    adcHists[0]->GetXaxis()->SetTitle("ADC Per Channel");
    adcHists[0]->GetYaxis()->SetRangeUser(1, 1e10);
    adcHists[0]->GetXaxis()->SetRangeUser(0, 250);
    adcHists[0]->Draw();
    TF1* fit = new TF1("fit1nM", "gaus",  0, 250);
    fit->SetLineColor(adcHists[0]->GetLineColor());
    fit->SetLineWidth(3);
    fit->SetLineStyle(2);
    fit->Draw("same");
    adcHists[0]->Fit(fit);



    TLegend* legADC = new TLegend(0.16, 0.57, 0.7, 0.88);
    legADC->SetBorderSize(0);
    legADC->SetTextSize(0.04);
    if(doDummy)legADC->AddEntry(adcHists[0], Form("%s, #mu = %0.2f, #sigma = %0.2f", namesNoSpaceDummy[0].c_str(),fit->GetParameter(1), fit->GetParameter(2)), "f");
    else legADC->AddEntry(adcHists[0], Form("%s, #mu = %0.2f, #sigma = %0.2f", namesNoSpace[0].c_str(),fit->GetParameter(1), fit->GetParameter(2)), "f");


    for(int x = 1; x < nHists/2; x++){
        adcHists[x]->Draw("same"); 
        TF1* fitPed = new TF1("fit1nM", "gaus",  0, 60);
        fitPed->SetLineColor(adcHists[x]->GetLineColor());
        fitPed->SetLineWidth(3);
        fitPed->SetLineStyle(2);
        adcHists[x]->Fit(fitPed); 
        fitPed->Draw("same");
        if(doDummy) legADC->AddEntry(adcHists[x], Form("%s, #mu = %0.2f, #sigma = %0.2f", namesNoSpaceDummy[x].c_str(),fitPed->GetParameter(1), fitPed->GetParameter(2)), "f");
        else legADC->AddEntry(adcHists[x], Form("%s, #mu = %0.2f, #sigma = %0.2f", namesNoSpace[x].c_str(),fitPed->GetParameter(1), fitPed->GetParameter(2)), "f");

    }

    TLatex* cms = new TLatex(0.10,0.92,Form("#bf{ZDC} Run %d #it{Work in progress}", runNumber));
    cms->SetNDC();
    cms->SetTextSize(0.05);
    cms->SetTextFont(42);

    TLatex* cmsFSC = new TLatex(0.10,0.92,Form("#bf{FSC} Run %d #it{Work in progress}", runNumber));
    cmsFSC->SetNDC();
    cmsFSC->SetTextSize(0.05);
    cmsFSC->SetTextFont(42);

    legADC->Draw();
    cms->Draw();
    if(doDummy)cADCMinus->SaveAs(Form("ZDCAnalyzer_%d_Minus_DUMMY.pdf", runNumber));
    else cADCMinus->SaveAs(Form("ZDCAnalyzer_%d_Minus.pdf", runNumber));


    TCanvas* cADCPlus = new TCanvas("cADCPlus", "cADCPlus", 800, 600);
    cADCPlus->SetLogy();
    cADCPlus->SetRightMargin(0.05);
    cADCPlus->cd(); 

    int index = nHists/2; 
    adcHists[index]->SetLineColor(TColor::GetColor(ColorStrings[0].c_str()));
    adcHists[index]->SetLineWidth(3);
    adcHists[index]->SetMarkerStyle(20);
    adcHists[index]->SetMarkerColor(TColor::GetColor(ColorStrings[0].c_str()));
    adcHists[index]->SetFillColor(TColor::GetColor(ColorStrings[0].c_str()));
    adcHists[index]->SetFillStyle(3004);
    adcHists[index]->GetXaxis()->SetTitle("ADC Per Channel");
    adcHists[index]->GetYaxis()->SetRangeUser(1, 1e10);
    adcHists[index]->GetXaxis()->SetRangeUser(0, 250);
    adcHists[index]->Draw();
    cms->Draw(); 
    TLegend* legADCPlus = new TLegend(0.16, 0.57, 0.7, 0.88);
    legADCPlus->SetBorderSize(0);
     for(int x = 9; x < nHists; x++){
        adcHists[x]->Draw("same"); 
        TF1* fitPed = new TF1("fit1nP", "gaus",  0, 100);
        fitPed->SetLineColor(adcHists[x]->GetLineColor());
        fitPed->SetLineWidth(3);
        fitPed->SetLineStyle(2);
        adcHists[x]->Fit(fitPed); 
        fitPed->Draw("same");
        if(doDummy)legADCPlus->AddEntry(adcHists[x], Form("%s, #mu = %0.2f, #sigma = %0.2f", namesNoSpaceDummy[x].c_str(),fitPed->GetParameter(1), fitPed->GetParameter(2)), "f");
        else legADCPlus->AddEntry(adcHists[x], Form("%s, #mu = %0.2f, #sigma = %0.2f", namesNoSpace[x].c_str(),fitPed->GetParameter(1), fitPed->GetParameter(2)), "f");
     }
    legADCPlus->SetTextSize(0.04);
    legADCPlus->Draw();
    if(doDummy)cADCPlus->SaveAs(Form("ZDCAnalyzer_%d_Plus_DUMMY.pdf", runNumber));
    else cADCPlus->SaveAs(Form("ZDCAnalyzer_%d_Plus.pdf", runNumber));


    // Draw FSC Channels
    TCanvas* cADCFSC = new TCanvas("cADCFSC", "cADCFSC", 800, 600);
    cADCFSC->SetLogy();
    cADCFSC->SetRightMargin(0.05);
    cADCFSC->cd(); 

    int indexFSC = nHists; 
    adcHists[indexFSC]->SetLineColor(TColor::GetColor(ColorStrings[0].c_str()));
    adcHists[indexFSC]->SetLineWidth(3);
    adcHists[indexFSC]->SetMarkerStyle(20);
    adcHists[indexFSC]->SetMarkerColor(TColor::GetColor(ColorStrings[0].c_str()));
    adcHists[indexFSC]->SetFillColor(TColor::GetColor(ColorStrings[0].c_str()));
    adcHists[indexFSC]->SetFillStyle(3004);
    adcHists[indexFSC]->GetXaxis()->SetTitle("ADC Per Channel");
    adcHists[indexFSC]->GetYaxis()->SetRangeUser(1, 1e10);
    adcHists[indexFSC]->GetXaxis()->SetRangeUser(0, 200);
    adcHists[indexFSC]->Draw();
    cmsFSC->Draw(); 

    std::string FSCNames[12] = {"FSCM Station 2 - Top", "FSCM Station 2 - Bottom", "FSCM Station 3 - Bottom Left", "FSCM Station 3 - Bottom Right", "FSCM Station 3 - Top Left", "FSCM Station 3 - Top Right", "FSCP Station 2 - Top", "FSCP Station 2 - Bottom", "FSCP Station 3 - Bottom Left", "FSCP Station 3 - Bottom Right", "FSCP Station 3 - Top Left", "FSCP Station 3 - Top Right"};
    TLegend* legADCFSC = new TLegend(0.16, 0.57, 0.7, 0.88);
    legADCFSC->SetBorderSize(0);
     for(int x = nHists; x < nHistsWithFSC-6; x++){
        adcHists[x]->Draw("same"); 
        TF1* fitPed = new TF1("fitFSC", "gaus",  0, 150);
        fitPed->SetLineColor(adcHists[x]->GetLineColor());
        fitPed->SetLineWidth(3);
        fitPed->SetLineStyle(2);
        adcHists[x]->Fit(fitPed); 
        fitPed->Draw("same");
        legADCFSC->AddEntry(adcHists[x], Form("%s, #mu = %0.2f, #sigma = %0.2f", FSCNames[x-nHists].c_str(),fitPed->GetParameter(1), fitPed->GetParameter(2)), "f");
     }
    legADCFSC->SetTextSize(0.04);
    legADCFSC->Draw();
    cADCFSC->SaveAs(Form("ZDCAnalyzer_%d_FSCM.pdf", runNumber));




    // Draw FSC Channels
    TCanvas* cADCFSCp = new TCanvas("cADCFSCp", "cADCFSCp", 800, 600);
    cADCFSCp->SetLogy();
    cADCFSCp->SetRightMargin(0.05);
    cADCFSCp->cd(); 

    int indexFSCp = nHistsWithFSC-6; 
    adcHists[indexFSCp]->SetLineColor(TColor::GetColor(ColorStrings[0].c_str()));
    adcHists[indexFSCp]->SetLineWidth(3);
    adcHists[indexFSCp]->SetMarkerStyle(20);
    adcHists[indexFSCp]->SetMarkerColor(TColor::GetColor(ColorStrings[0].c_str()));
    adcHists[indexFSCp]->SetFillColor(TColor::GetColor(ColorStrings[0].c_str()));
    adcHists[indexFSCp]->SetFillStyle(3004);
    adcHists[indexFSCp]->GetXaxis()->SetTitle("ADC Per Channel");
    adcHists[indexFSCp]->GetYaxis()->SetRangeUser(1, 1e10);
    adcHists[indexFSCp]->GetXaxis()->SetRangeUser(0, 200);
    adcHists[indexFSCp]->Draw();
    cmsFSC->Draw(); 

    TLegend* legADCFSCp = new TLegend(0.16, 0.57, 0.7, 0.88);
    legADCFSCp->SetBorderSize(0);
     for(int x = nHistsWithFSC - 6; x < nHistsWithFSC; x++){
        adcHists[x]->Draw("same"); 
        TF1* fitPed = new TF1("fitFSC", "gaus",  0, 150);
        fitPed->SetLineColor(adcHists[x]->GetLineColor());
        fitPed->SetLineWidth(3);
        fitPed->SetLineStyle(2);
        adcHists[x]->Fit(fitPed); 
        fitPed->Draw("same");
        legADCFSCp->AddEntry(adcHists[x], Form("%s, #mu = %0.2f, #sigma = %0.2f", FSCNames[x-nHists].c_str(),fitPed->GetParameter(1), fitPed->GetParameter(2)), "f");
     }
    legADCFSCp->SetTextSize(0.04);
    legADCFSCp->Draw();
    cADCFSCp->SaveAs(Form("ZDCAnalyzer_%d_FSCP.pdf", runNumber));
}



void FillChain(TChain &chain, std::string filename) {
    chain.Add(filename.c_str());  
}
