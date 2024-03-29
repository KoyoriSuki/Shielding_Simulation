//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file DBDecay/src/TrackingAction.cc
/// \brief Implementation of the TrackingAction class
//
//
// $Id: TrackingAction.cc 78307 2013-12-11 10:55:57Z gcosmo $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "TrackingAction.hh"

#include "HistoManager.hh"
//#include "Run.hh"
#include "EventAction.hh"
#include "TrackingMessenger.hh"

#include "G4Track.hh"
#include "G4ParticleTypes.hh"
#include "G4RunManager.hh"
#include "SteppingAction.hh"
#include "RunAction.hh"
#include <iostream>
#include <iomanip>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackingAction::TrackingAction(RunAction*runAct,EventAction* EA, HistoManager* fHisto)
  :G4UserTrackingAction(),
   fRun(runAct),fEvent(EA),fTrackMessenger(0),
    fHistoManager(fHisto),
   fFullChain(true)
{
  fTrackMessenger = new TrackingMessenger(this);
}

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackingAction::~TrackingAction()
{
  delete fTrackMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void TrackingAction::PreUserTrackingAction(const G4Track* track)
{
  G4ParticleDefinition* particle = track->GetDefinition();
  G4String name   = particle->GetParticleName();
  fCharge = particle->GetPDGCharge();
  fBaryonNo = particle->GetBaryonNumber();
  fMass   = particle->GetPDGMass();  
  fParticleEnCode= particle->GetPDGEncoding();
  G4double Ekin = track->GetKineticEnergy();
  G4ThreeVector trackVertexPos = track->GetVertexPosition();
  G4ThreeVector trackVertexDir = track->GetVertexMomentumDirection();
  G4int ID      = track->GetTrackID();
  G4int parentID = track->GetParentID();
  fTrackEdepInCrystal=0.;
  fTrackEdepInPbShell=0.;
  
  //
  //-----------------update------------------------
  if(ID==1)
      fHistoManager->fTrackInfo.fInitialEnergy.push_back(Ekin);

  //------------------------------------------------
  //
  
  G4bool condition = false;

  //count particles
  //
  //if (ID>1) 
  fRun->ParticleCount(name, Ekin);
  
  //energy spectrum
  //
    G4int ih = 0;
  if (particle == G4Electron::Electron()||
      particle == G4Positron::Positron())  ih = 1;
  else if (particle == G4NeutrinoE::NeutrinoE()||
           particle == G4AntiNeutrinoE::AntiNeutrinoE()) ih = 2;
  else if (particle == G4Gamma::Gamma()) ih = 3;
  else if (particle == G4Alpha::Alpha()) ih = 4;
  else if (fCharge > 2.) ih = 5;
  //if (ih) G4AnalysisManager::Instance()->FillH1(ih, Ekin);
  
  //Ion
  //
  if (fCharge > 2.) {
    //build decay chain
    if (ID == 1) fEvent->AddDecayChain(name);
      else       fEvent->AddDecayChain(" ---> " + name);
    // 
    //full chain: put at rest; if not: kill secondary      
    G4Track* tr = (G4Track*) track;
    if (fFullChain)  tr->SetTrackStatus(fStopButAlive);
      else if (ID>1) tr->SetTrackStatus(fStopAndKill);
  }
  
  //example of saving random number seed of this fEvent, under condition
  //
  ////condition = (ih == 3);
if (condition) G4RunManager::GetRunManager()->rndmSaveThisEvent();
}
  
  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PostUserTrackingAction(const G4Track* track)
{
 
  //get time
  //   
  G4double time = track->GetGlobalTime();
  G4int ID = track->GetTrackID();
  if (ID == 1) fRun->PrimaryTiming(time);        //time of life of primary ion  
  
  //energy and momentum balance (from secondaries)
  //
  const std::vector<const G4Track*>* secondaries 
    = track->GetStep()->GetSecondaryInCurrentStep();
  size_t nbtrk = (*secondaries).size();
  if (nbtrk) {
    //there are secondaries --> it is a decay
    //
    //balance    
    G4double EkinTot = 0.;
    G4ThreeVector Pbalance = - track->GetMomentum();
    for (size_t itr=0; itr<nbtrk; itr++) {
      const G4Track* trk = (*secondaries)[itr];
      EkinTot += trk->GetKineticEnergy();
      //exclude gamma desexcitation from momentum balance
      if (trk->GetDefinition() != G4Gamma::Gamma())         
	Pbalance += trk->GetMomentum();                 
    }
    G4double Pbal = Pbalance.mag(); 
    fRun->Balance(EkinTot,Pbal);  
  }
  
//no secondaries --> end of chain    
//  
if (!nbtrk) {
  fRun->EventTiming(time);                     //total time of life
 }

//
//update energy deposition data
//
 fHistoManager->fTrackInfo.fEnergyDepositionInCrystal.push_back(fTrackEdepInCrystal);
 fHistoManager->fTrackInfo.fEnergyDepositionInShielding.push_back(fTrackEdepInPbShell);
 fHistoManager->fTrackInfo.fTime.push_back(time);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

