// Usage: root -l inspect_tree.C
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include <iostream>

void inspect_tree() {
  TFile *f = TFile::Open("muons.root");
  if (!f || f->IsZombie()) { std::cerr << "Cannot open muons.root\n"; return; }
  TTree *t = nullptr;
  f->GetObject("muons", t);
  if (!t) { std::cerr << "Tree 'muons' not found\n"; return; }

  t->Print();                         // names, types and structure
  std::cout << "entries = " << t->GetEntries() << "\n";
  t->Scan("eventId:tStart_ns:tStop_ns:dt_us:qStart_adc:qStop_adc:quality", "", "", 8);
  std::cout << "quality==1: " << t->GetEntries("quality==1") << "\n";
  std::cout << "dt_us<0 or dt_us>20: " << t->GetEntries("dt_us<0 || dt_us>20") << "\n";
  std::cout << "accepted, 0.2<dt_us<8: "
            << t->GetEntries("quality==1 && dt_us>0.2 && dt_us<8") << "\n";
  // Compare a cut with its complement. Check counts and inspect boundary events.
  t->Scan("eventId:dt_us", "quality==1 && dt_us>8", "", 10);
}
