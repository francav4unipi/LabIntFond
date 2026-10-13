// Usage: root -l plots.C
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TGraphErrors.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TF1.h"
#include "TMath.h"
#include <cmath>

void plots() {
  TFile *f = TFile::Open("muons.root");
  TTree *t = nullptr;
  if (!f || f->IsZombie()) return;
  f->GetObject("muons", t);
  if (!t) return;
  const char *cut = "quality==1";
  gStyle->SetOptStat(0);

  // 1D lifetime distribution: ROOT expression uses microseconds.
  auto c1 = new TCanvas("c1", "Muon decay times", 850, 600);
  auto h = new TH1D("hDecay", "Muon decay-time distribution;Decay time #Deltat [#mus];Events / 0.1 #mus", 100, 0, 10);
  t->Draw("dt_us>>hDecay", cut, "goff");
  h->SetLineColor(kAzure+2); h->SetLineWidth(2); h->SetFillColorAlpha(kAzure-9, 0.45);
  h->Draw("HIST E");
  auto expo = new TF1("expo_tau", "[0]*exp(-x/[1])", 0, 10);
  expo->SetParameters(h->GetMaximum(), 2.2);
  expo->SetParNames("normalisation", "#tau [#mus]");
  h->Fit(expo, "L", "", 0.2, 8.0); // quick diagnostic fit; inspect range and residuals
  auto leg = new TLegend(0.58,0.70,0.88,0.86);
  leg->SetHeader("Laboratorio di fisica", "C");
  leg->AddEntry(h, "quality = 1", "f"); leg->AddEntry(expo, "Esponenziale (fit rapido)", "l");
  leg->Draw();
  c1->SaveAs("decay_time.pdf"); c1->SaveAs("decay_time.png");

  // 2D histogram and event-level scatter of ADC channels.
  auto c2 = new TCanvas("c2", "ADC correlation", 850, 600);
  auto h2 = new TH2D("hADC", "Pulse amplitudes;Start pulse [ADC counts];Stop pulse [ADC counts]", 80, 50, 150, 80, 40, 180);
  t->Draw("qStop_adc:qStart_adc>>hADC", cut, "goff");
  h2->Draw("COLZ");
  c2->SaveAs("adc_correlation.pdf");

  auto c3 = new TCanvas("c3", "ADC scatter", 850, 600);
  t->SetMarkerStyle(20); t->SetMarkerSize(0.25); t->SetMarkerColor(kAzure+2);
  t->Draw("qStop_adc:qStart_adc", cut, ""); // each selected event is a point
  if (auto g = (TGraph*)gPad->GetPrimitive("Graph")) {
    g->SetTitle("Start-stop pulse amplitudes;Start pulse [ADC counts];Stop pulse [ADC counts]");
    g->SetMarkerStyle(20); g->SetMarkerSize(0.25); g->SetMarkerColor(kAzure+2);
  }
  c3->SaveAs("adc_scatter.pdf");

  // Profile: average decay time versus event number, useful for a drift check.
  auto c4 = new TCanvas("c4", "Profile drift check", 850, 600);
  auto p = new TProfile("pTrend", "Mean decay time versus event number;Event number;Mean #Deltat [#mus]", 100, 0, 1000000);
  t->Draw("dt_us:eventId>>pTrend", cut, "prof goff");
  p->SetMarkerStyle(20); p->SetMarkerColor(kOrange+7); p->Draw("E1");
  c4->SaveAs("profile_dt_vs_event.pdf");

  // TGraphErrors from histogram counts with Poisson sqrt(N) y errors.
  const int nb = h->GetNbinsX();
  auto graph = new TGraphErrors(nb);
  graph->SetName("gDecayCounts"); graph->SetTitle("Decay-time counts;Decay time [#mus];Events / bin");
  for (int b=1; b<=nb; ++b) {
    const double y=h->GetBinContent(b), x=h->GetBinCenter(b);
    graph->SetPoint(b-1,x,y);
    graph->SetPointError(b-1,h->GetBinWidth(b)/2.,std::sqrt(y));
  }
  auto c5 = new TCanvas("c5", "Graph with errors", 850, 600);
  graph->SetMarkerStyle(20); graph->SetMarkerColor(kAzure+2); graph->SetLineColor(kAzure+2);
  graph->Draw("AP"); c5->SaveAs("decay_graph_errors.pdf");
}
