// Usage: root -l event_loop.C
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include <iostream>
#include <cmath>

void event_loop() {
  TFile *in = TFile::Open("muons.root");
  TTree *t = nullptr;
  if (!in || in->IsZombie()) return;
  in->GetObject("muons", t);
  if (!t) return;

  Long64_t eventId=0, tStart_ns=0, tStop_ns=0;
  double dt_us=0.; int qStart_adc=0, qStop_adc=0, quality=0;
  t->SetBranchAddress("eventId", &eventId);
  t->SetBranchAddress("tStart_ns", &tStart_ns);
  t->SetBranchAddress("tStop_ns", &tStop_ns);
  t->SetBranchAddress("dt_us", &dt_us);
  t->SetBranchAddress("qStart_adc", &qStart_adc);
  t->SetBranchAddress("qStop_adc", &qStop_adc);
  t->SetBranchAddress("quality", &quality);

  TH1D h("hSelected", "Selected decay times;Decay time [#mus];Events / 0.1 #mus", 100, 0, 10);
  Long64_t nRead=0, nQuality=0, nSelected=0;
  double sumDt=0.;
  const Long64_t n=t->GetEntries();
  for (Long64_t i=0; i<n; ++i) {
    if (t->GetEntry(i) <= 0) continue;
    ++nRead;
    if (!quality) continue;
    ++nQuality;
    const bool pulseWindow = qStart_adc>60 && qStart_adc<145 && qStop_adc>55 && qStop_adc<170;
    const bool timeWindow = dt_us>0.2 && dt_us<8.0;
    if (!pulseWindow || !timeWindow) continue;
    ++nSelected;
    sumDt += dt_us;
    h.Fill(dt_us);
  }
  std::cout << "read=" << nRead << " quality=" << nQuality
            << " selected=" << nSelected;
  if (nSelected) std::cout << " mean dt [us]=" << sumDt/nSelected;
  std::cout << "\n";
  auto c = new TCanvas("cLoop", "Event-loop selection", 850, 600);
  h.Draw("E"); c->SaveAs("event_loop_selection.pdf");
}
