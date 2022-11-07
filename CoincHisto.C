#include <vector>
#include "TChain.h"
#include "TBranch.h"
#include "TRandom3.h"
#include "TH2D.h"
#include <iostream>
#include <TFile.h>

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
	std::cout << "nentries = " << nentries << std::endl;

	TRandom3 rnd;

	TH2D *hE1vsE2 = new TH2D("hE1vsE2","hE1vsE2;E1 (keV);E2 (keV)",500,0,Emax_keV,500,0,Emax_keV);
	TH2D *hE1vsE2_res = new TH2D("hE1vsE2_res","hE1vsE2 (with resolution effect);E1 (keV);E2 (keV)",500,0,Emax_keV,500,0,Emax_keV);
	TH2D *hEsumvsE2 = new TH2D("hEsumvsE2","hEsumvsE2;E1 (keV);E2 (keV)",500,0,Emax_keV,500,0,Emax_keV);

	// timing resolution
	const Double_t sigmaT = 10./2.355; // 10 ns FWHM

	// energy resolution
	const Double_t aRes = 5.87E-04;
    const Double_t bRes = 3.95E-04;
    const Double_t cRes = 7.47;

	for (Long64_t entry=0;entry<nentries;entry++) {
		tree->GetEntry(entry);
		if(entry%(nentries/100)==0) {
			std::cout << "*" << std::flush;
		}

		size_t nHits = detectorNbr->size();
		if(EnergyDeps->size()!=nHits) {
			std::cout << "error EnergyDeps->size()!=nHits" << std::endl;
			break;
		}
		if(Times->size()!=nHits) {
			std::cout << "error Times->size()!=nHits" << std::endl;
			break;
		}

		Double_t Esum = 0.;
		for(size_t hit1=0;hit1<nHits;hit1++) {
			if((*detectorNbr)[hit1]==7) continue; // hit is in the outer segment only
			Esum += (*EnergyDeps)[hit1];
			for(size_t hit2=hit1+1;hit2<nHits;hit2++) {
				if((*detectorNbr)[hit2]==7) continue; // hit is in the outer segment only
				Double_t deltaT = fabs((*Times)[hit2] - (*Times)[hit1]);
				if(deltaT<=coincTime) {
					hE1vsE2->Fill((*EnergyDeps)[hit1],(*EnergyDeps)[hit2]);
				}

				deltaT = rnd.Gaus((*Times)[hit2] - (*Times)[hit1],sigmaT);
				if(fabs(deltaT)<=coincTime) {
					Double_t sigmaE1 = 0.4246609*(aRes + bRes*sqrt((*EnergyDeps)[hit1] + cRes*(*EnergyDeps)[hit1]*(*EnergyDeps)[hit1]));
					Double_t sigmaE2 = 0.4246609*(aRes + bRes*sqrt((*EnergyDeps)[hit2] + cRes*(*EnergyDeps)[hit2]*(*EnergyDeps)[hit2]));
					for(int i=0;i<20;i++) {
						Double_t E1 = rnd.Gaus((*EnergyDeps)[hit1],sigmaE1);
						for(int j=0;j<20;j++) {
							Double_t E2 = rnd.Gaus((*EnergyDeps)[hit2],sigmaE2);
							if(deltaT>0) {
								hE1vsE2_res->Fill(E1,E2,0.0025);
							} else {
								hE1vsE2_res->Fill(E2,E1,0.0025);
							}
						}
					}
				}
			}
		}

		Double_t E7 = 0.;
		Double_t t7 = 0.;
		for(size_t hit1=0;hit1<nHits;hit1++) {
			if((*detectorNbr)[hit1]==7) {
				E7 = (*EnergyDeps)[hit1];
			}
		}

		if(E7>0.) { //if there is a hit in the outer segment
			Double_t Esum = 0.;
			for(size_t hit1=0;hit1<nHits;hit1++) {
				if((*detectorNbr)[hit1]==7) continue;
					Double_t deltaT = fabs((*Times)[hit1] - t7);
					if(deltaT<=coincTime) {
						Esum += (*EnergyDeps)[hit1];
					}
			}

			hEsumvsE2->Fill(E7,Esum);
		}

	}
	std::cout << std::endl;

	hE1vsE2->Draw("colz");

	TFile *fOut = new TFile("Co60_CoicHisto.root","recreate");
	hE1vsE2->Write();
	hE1vsE2_res->Write();
	hEsumvsE2->Write();
	delete fOut;
}