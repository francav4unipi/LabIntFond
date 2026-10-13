// Usage: root -l -q convert_raw.C
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "TFile.h"
#include "TTree.h"

void convert_raw() {
  std::ifstream in("muons_raw.txt");
  if (!in) { std::cerr << "Cannot open muons_raw.txt\n"; return; }

  TFile out("muons.root", "RECREATE");
  TTree tree("muons", "Muon start-stop teaching data");
  Long64_t eventId=0, tStart_ns=0, tStop_ns=0;
  double dt_us=0.;
  int qStart_adc=0, qStop_adc=0, quality=0;
  tree.Branch("eventId", &eventId, "eventId/L");
  tree.Branch("tStart_ns", &tStart_ns, "tStart_ns/L");
  tree.Branch("tStop_ns", &tStop_ns, "tStop_ns/L");
  tree.Branch("dt_us", &dt_us, "dt_us/D");
  tree.Branch("qStart_adc", &qStart_adc, "qStart_adc/I");
  tree.Branch("qStop_adc", &qStop_adc, "qStop_adc/I");
  tree.Branch("quality", &quality, "quality/I");

  Long64_t rows=0, malformed=0, negativeDt=0;
  std::string line;
  while (std::getline(in, line)) {
    if (line.empty() || line[0] == '#') continue;
    std::istringstream row(line);
    if (!(row >> eventId >> tStart_ns >> tStop_ns >> qStart_adc >> qStop_adc >> quality)) {
      ++malformed;
      continue;
    }
    const Long64_t delta_ns = tStop_ns - tStart_ns;
    if (delta_ns < 0) { ++negativeDt; continue; }
    dt_us = delta_ns * 1.e-3; // 1 microsecond = 1000 ns
    tree.Fill();
    ++rows;
  }
  tree.Write();
  out.Close();
  std::cout << "Rows written: " << rows << ", negative intervals: " << negativeDt
            << ", malformed rows: " << malformed << "\n";
}
