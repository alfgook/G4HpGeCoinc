//
// $Id$
//
/// \file RunAction.cc
/// \brief Implementation of the RunAction class

#include "RunAction.hh"
#include "G4Run.hh"
#include "G4GenericMessenger.hh"
#include "Randomize.hh"
#include "G4AnalysisManager.hh"
#include <iostream>
#include <ios>
#include "G4ios.hh"



RunAction::RunAction()
{
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetNtupleMerging(true);
  analysisManager->SetFileName("OutPut.root"); //default filename, can be changed by macro command

  analysisManager->CreateH1("Edep_det1","Edep_det1",4000,0.,4000); // 1 keV binning
  analysisManager->CreateH1("Edep_det2","Edep_det2",4000,0.,4000); // 1 keV binning
  analysisManager->CreateH2("E0vsE1","E0vsE1",2000,0.,4000.,2000,0.,4000.); //

  analysisManager->CreateNtuple("G4HPGeCoinc", "G4HPGeCoinc");
  analysisManager->CreateNtupleIColumn("detectorNbr",vDetectorNbrs);
  analysisManager->CreateNtupleDColumn("EnergyDeps",vEnergyDeps);
  analysisManager->CreateNtupleDColumn("Times",vTimes);
  analysisManager->FinishNtuple();
}


RunAction::~RunAction()
{

}


void RunAction::BeginOfRunAction(const G4Run* run)
{ 
  G4cout << "### Run " << run->GetRunID() << " start." << G4endl;

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->OpenFile();
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* /*run*/)
{
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::AddNtupleRow()
{
  G4AnalysisManager::Instance()->AddNtupleRow();
  vDetectorNbrs.clear();
  vEnergyDeps.clear();
  vTimes.clear();
}