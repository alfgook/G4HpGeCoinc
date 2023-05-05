#include <vector>
#include "TChain.h"
#include "TBranch.h"
#include "TRandom3.h"
#include "TH2D.h"
#include <iostream>
#include <TFile.h>
#include <algorithm>

void CoincHisto(const char *filenameIn, const char *filenameOut, Double_t coincTime = 100, const Double_t Emax_keV = 2000)
{
	TChain *tree = new TChain("G4HPGeCoinc");
	tree->Add(filenameIn);

	/*Double_t        GlobalEventTime;
	Int_t           EventNumber;
	Int_t           Multiplicity;*/
	vector<int>     *detectorNbr = nullptr;
	vector<double>  *EnergyDeps = nullptr;
	vector<double>  *Times = nullptr;

	/*TBranch        *b_GlobalEventTime;   //!
	TBranch        *b_EventNumber;   //!
	TBranch        *b_Multiplicity;   //!*/
	TBranch        *b_detectorNbr;   //!
	TBranch        *b_EnergyDeps;   //!
	TBranch        *b_Times;   //!

	/*tree->SetBranchAddress("GlobalEventTime", &GlobalEventTime, &b_GlobalEventTime);
	tree->SetBranchAddress("EventNumber", &EventNumber, &b_EventNumber);
	tree->SetBranchAddress("Multiplicity", &Multiplicity, &b_Multiplicity);*/
	tree->SetBranchAddress("detectorNbr", &detectorNbr, &b_detectorNbr);
	tree->SetBranchAddress("EnergyDeps", &EnergyDeps, &b_EnergyDeps);
	tree->SetBranchAddress("Times", &Times, &b_Times);

	Long64_t nentries = tree->GetEntries();
	std::cout << "nentries = " << nentries << std::endl;

	TRandom3 rnd;

	TH2D *hE1vsE2 = new TH2D("hE1vsE2","hE1vsE2;E1 (keV);E2 (keV)",1000,0,Emax_keV,1000,0,Emax_keV);
	TH2D *hE1vsE2_res = new TH2D("hE1vsE2_res","hE1vsE2 (with resolution effect);E1 (keV);E2 (keV)",500,0,Emax_keV,500,0,Emax_keV);
	TH2D *hEsumvsE2 = new TH2D("hEsumvsE2","hEsumvsE2;Esum (keV);E1 (keV)",500,0,Emax_keV,500,0,Emax_keV);
	TH1D *hSum = new TH1D("hSum","sum energy;E (MeV)",10000,0,20);
	TH1D *hE1 = new TH1D("hE1","all segment energies;E (MeV)",10000,0,20);
	TH1D *hMult = new TH1D("hMult","detected multiplicity",10,0.5,10.5);

	Char_t name[64];
	TH2D *hE1vsE2_mult[5];
	for(int i=0;i<5;i++) {
		snprintf(name,64,"hE1vsE2_mult%d",i+2);
		hE1vsE2_mult[i] = new TH2D(name,name,1000,0,Emax_keV,1000,0,Emax_keV);
	}

	TH1D *hE1_mult[5];
	for(int i=0;i<5;i++) {
		snprintf(name,64,"hE1_mult%d",i+1);
		hE1_mult[i] = new TH1D(name,name,10000,0,20);
	}

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
		hMult->Fill(nHits);
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
			//if((*detectorNbr)[hit1]==7) continue; // hit is in the outer segment only
			Esum += (*EnergyDeps)[hit1];
			hE1->Fill((*EnergyDeps)[hit1]*1.E-03);
			if( (((Int_t) nHits)-1)>=0 && (((Int_t) nHits)-1)<5 ) {
				for(int nn=0;nn<nHits-1;nn++) hE1_mult[nn]->Fill((*EnergyDeps)[hit1]*1.E-03);
			}
		}
		hSum->Fill(Esum*1.E-03);

		for(size_t hit1=0;hit1<nHits;hit1++) {
			//if((*detectorNbr)[hit1]==7) continue; // hit is in the outer segment only
			hEsumvsE2->Fill(Esum,(*EnergyDeps)[hit1]);
			for(size_t hit2=hit1+1;hit2<nHits;hit2++) {
				//if((*detectorNbr)[hit2]==7) continue; // hit is in the outer segment only
				Double_t deltaT = fabs((*Times)[hit2] - (*Times)[hit1]);
				if(deltaT<=coincTime) {
					Double_t E1 = std::max((*EnergyDeps)[hit1],(*EnergyDeps)[hit2]);
					Double_t E2 = std::min((*EnergyDeps)[hit1],(*EnergyDeps)[hit2]);
					hE1vsE2->Fill(E1,E2);
					if( (((Int_t) nHits)-2)>=0 && (((Int_t) nHits)-2)<5 ) hE1vsE2_mult[nHits-2]->Fill(E1,E2);
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
	}
	std::cout << std::endl;

	hE1vsE2->Draw("colz");

	TFile *fOut = new TFile(filenameOut,"recreate");
	hE1vsE2->Write();
	hE1vsE2_res->Write();
	hEsumvsE2->Write();
	hSum->Write();
	hE1->Write();
	for(int i=0;i<5;i++) hE1vsE2_mult[i]->Write();
	for(int i=0;i<5;i++) hE1_mult[i]->Write();
	hMult->Write();
	delete fOut;
}