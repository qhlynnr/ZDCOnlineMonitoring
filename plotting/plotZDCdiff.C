#include <string>

void plotVvsDiff()
{
    // V and H values
    std::vector<double> V = {
         0,   0,  30, -60,  60,
        75,  90,   0,  60,  30
    };

    std::vector<double> H = {
         0, -30,   0,   0,   0,
         0,   0,   0, -60,   0
    };

    std::vector<int> runNum = {
        187,
        188,
        189,
        191,
        192,
        193,
        194,
        195,
        196,
        197
    };

    // ZDCP values
    std::vector<double> ZDCP = {
        2162.58,
        2118.93,
        2207.16,
        2254.48,
        2060.52,
        2034.80,
        2006.64,
        2173.47,
        1973.17,
        2043.40
    };

    // ZDCM values
    std::vector<double> ZDCM = {
        1894.45,
        1928.20,
        1840.18,
        1812.77,
        1999.84,
        2010.93,
        2047.87,
        1927.43,
        2023.16,
        1936.98
    };

    // Difference values
    std::vector<double> diff = {
        268.133,
        190.733,
        366.982,
        441.709,
         60.684,
         23.86,
        -41.23,
        246.04,
        -49.99,
        106.42
    };

    // Plot V vs diff
    TGraph *g = new TGraph(V.size());

    for (int i = 0; i < V.size(); ++i){
        g->SetPoint(i, V[i], diff[i]);
        
    }
    g->SetTitle("Vertical position of beam vs Difference between ZDC P and M;V;Diff");

    g->SetMarkerStyle(20);
    g->SetMarkerSize(1.3);

    TCanvas *c = new TCanvas("c", "c", 800, 600);
    g->Draw("AP");
    for (int i = 0; i < V.size(); ++i) {
        int color = (H[i] != 0) ? kRed : kBlack;
        TGraph *gpt = new TGraph(1, &V[i], &diff[i]);
        gpt->SetMarkerStyle(20);
        gpt->SetMarkerSize(1.3);
        gpt->SetMarkerColor(color);
        gpt->Draw("P SAME");

        TLatex label;
        label.SetTextSize(0.025);
        label.SetTextAlign(22); // center
        label.DrawLatex(V[i]+5, diff[i], Form("%d", runNum[i]));
    }

    c->SaveAs("zdcplot0524/VerticalvsZDCDiffplots.png");

}

void plotZDCdiff(){
    plotVvsDiff();
}