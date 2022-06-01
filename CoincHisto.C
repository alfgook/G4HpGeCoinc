#include <vector>

void CoincHisto(const char *filename, Double_t coincTime = 100, const Double_t Emax_keV = 2000)
{
	TChain *tree = new TChain("G4HPGeCoinc");
	tree->Add(filename);

	vector<int>     *detectorNbr = nullptr;
	vector<double>  *EnergyDeps = nullptr;
	vector<double>  *Times = nullptr;

	TBranch        *b_detectorNbr;   //!
	TBranch        *b_EnergyDeps;   //!
	TBranch        *b_Times;   //!

	tree->SetBranchAddress("detectorNbr", &detectorNbr, &b_detectorNbr);
	tree->SetBranchAddress("EnergyDeps", &EnergyDeps, &b_EnergyDeps);
	tree->SetBranchAddress("Times", &Times, &b_Times);

	Long64_t nentries = tree->GetEntries();

	TH2D *hE1vsE2 = new TH2D("hE1vsE2","hE1vsE2;E1 (keV);E2 (keV)",500,0,Emax_keV,500,0,Emax_keV);

	for (Long64_t entry=0;entry<nentries;entry++) {
		tree->GetEntry(entry);

		size_t nHits = detectorNbr->size();
		if(EnergyDeps->size()!=nHits) {
			std::cout << "error EnergyDeps->size()!=nHits" << std::endl;
			break;
		}
		if(Times->size()!=nHits) {
			std::cout << "error Times->size()!=nHits" << std::endl;
			break;
		}

		for(size_t hit1=0;hit1<nHits;hit1++) {
			if((*Times)[hit1]==0.) std::cout << (*Times).size() << std::endl;
			if((*detectorNbr)[hit1]==7) continue; // hit is in the outer segment only
			for(size_t hit2=hit1+1;hit2<nHits;hit2++) {
				if((*detectorNbr)[hit2]==7) continue; // hit is in the outer segment only
				Double_t deltaT = fabs((*Times)[hit2] - (*Times)[hit1]);
				if(deltaT<=coincTime) {
					hE1vsE2->Fill((*EnergyDeps)[hit1],(*EnergyDeps)[hit2]);
				}
			}
		}
	}

	hE1vsE2->Draw("colz");
}