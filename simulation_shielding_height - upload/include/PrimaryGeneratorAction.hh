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
/// \file DBDecay/include/PrimaryGeneratorAction.hh
/// \brief Definition of the PrimaryGeneratorAction class
//
//
// $Id: PrimaryGeneratorAction.hh 68017 2013-03-13 13:29:53Z gcosmo $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "Randomize.hh"
#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <random>
#include <cmath>

class G4VPrimaryGenerator;
class G4Event;
class PrimaryGeneratorMessenger;
class DetectorConstruction;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction(DetectorConstruction*);    
   ~PrimaryGeneratorAction();

  public:
    virtual void GeneratePrimaries(G4Event*);
    G4GeneralParticleSource* GetParticleGun() { return fGParticleSource;}; 
    std::discrete_distribution<int> EnergyDistribution; //used to generate energy distribution
    std::mt19937 generator;//random number generator

  private:
    G4VPrimaryGenerator* HEPEvt;
    PrimaryGeneratorMessenger* messenger;
    G4bool useHEPEvt;
    G4GeneralParticleSource * fGParticleSource;
    DetectorConstruction* fDetector;

    G4int useCodeOrMacFile = 0;

    // Energy distribution, set with command argument
    // 0: 0 - 3.1 MeV uniform 
    // 1: unfolded spectrum 
    G4int energyDistributionFlag = 0;

  public:
    //
    // UI command functions
    //
    inline void SetHEPEvtGenerator(G4bool f)
    {
        useHEPEvt = f;
    }
    inline void SetEnergyDistributionFlag(G4int f)
    {
        energyDistributionFlag = f;
    }
    inline void SetUseCodeOrMacFile(G4int f)
    {
        useCodeOrMacFile = f;
    }

    inline G4bool GetHEPEvtGenerator()
    {
        return useHEPEvt;
    }
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
