{
gStyle->SetOptTitle(0);
gStyle->SetPadColor(kWhite);
gStyle->SetCanvasColor(kWhite);
gStyle->SetFrameFillColor(kWhite);
gStyle->SetTextFont(42);
gStyle->SetStatColor(kWhite);
gStyle->SetStatFont(42);
gStyle->SetTitleFont(42, "X");
gStyle->SetTitleFont(42, "Y");
gStyle->SetTitleFont(42, "Z");
gStyle->SetLabelFont(42, "X");
gStyle->SetLabelFont(42, "Y");
gStyle->SetLabelFont(42, "Z");

gStyle->SetLabelSize(0.06, "XYZ");
gStyle->SetTitleSize(0.06, "XYZ");
gStyle->SetFrameLineWidth(2);

gStyle->SetLineColor(kBlack);
gStyle->SetHistLineColor(kBlack);
gStyle->SetFuncColor(kBlack);

//gStyle->SetLegendFillColor(kWhite);
//gStyle->SetLegendFillColor(kRed);
gStyle->SetFillColor(kWhite);

//gStyle->SetNdivisions(1010, "X");
//gStyle->SetNdivisions(1005, "X");
gStyle->SetNdivisions(505, "Y");
gStyle->SetNdivisions(505, "Z");

gStyle->SetPalette(1);
gStyle->SetOptFit(1111);
gStyle->SetOptStat("emroui");
gStyle->SetStatX(0.99);
gStyle->SetStatY(0.90);
gStyle->SetStatW(0.18);
gStyle->SetStatH(0.15);
gStyle->SetPadLeftMargin(0.10);
gStyle->SetPadRightMargin(0.25);
gStyle->SetPadBottomMargin(0.15);
gStyle->SetTitleOffset(0.8, "Y");
gStyle->SetStatBorderSize(1); // removes shadow
}
