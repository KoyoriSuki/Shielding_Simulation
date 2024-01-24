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
/// \file DBDecay/src/PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class
//
//
// $Id: PrimaryGeneratorAction.cc 78307 2013-12-11 10:55:57Z gcosmo $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#include "PrimaryGeneratorAction.hh"
#include <iostream>
#include "PrimaryGeneratorMessenger.hh"
#include "G4Event.hh"
//#include "G4HEPEvtInterface.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Geantino.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "globals.hh"
#include "G4ios.hh"
#include "G4Gamma.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Orb.hh"
#include "G4UnionSolid.hh"
#include "G4NeutrinoE.hh"
#include "G4UImanager.hh"

//#include "CRYGenerator.h"
//#include "CRYSetup.h"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* det)
 : G4VUserPrimaryGeneratorAction(),
   fGParticleSource(),
   fDetector(det)
{
  fGParticleSource  = new G4GeneralParticleSource();

  // Where the UI commands works
  messenger = new PrimaryGeneratorMessenger(this);

  // If use c++ code for GPS settings
  if (useCodeOrMacFile == 0)
  {
      //
      //Energy Distribution Initialization
      //
      std::ifstream histFile("unfoldedSpectra_centerValue.txt");
      std::vector<double> histData;
      double freq;
      double totalFreq = 0.0;
      while (histFile >> freq) {
          histData.push_back(freq);
          totalFreq += freq;
      }
      std::transform(histData.begin(), histData.end(), histData.begin(),
          [totalFreq](double freq1) { return freq1 / totalFreq; });
      EnergyDistribution = std::discrete_distribution<int>(histData.begin(), histData.end());
      generator.seed(std::random_device()());
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  //delete HEPEvt;
  delete fGParticleSource;
  delete messenger;

  //fclose(countsRecord);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    if (useCodeOrMacFile == 0)
    {
        G4double x0 = 1. * cm, y0 = 1. * cm, z0 = 1. * cm;
        G4double dx0 = 5.44 * cm, dy0 = 5.44 * cm, dz0 = 5.44 * cm;
        x0 = dx0 * (G4UniformRand() - 0.5);
        y0 = dy0 * (G4UniformRand() - 0.5);
        z0 = dz0 * (G4UniformRand() - 0.5);

        //
        // create vertex
        //

        int particleflag = 0;//0:gamma //1:ion;
        if (particleflag == 0)
        {
            //particle type
            fGParticleSource->SetParticleDefinition(G4Gamma::GammaDefinition());

            //kinetic energy
                 //defined as a private member; 0:0-3MeV flat 1: unfolded
            if (energyDistributionFlag == 0)
            {
                double EventEnergy = 1;//EnergyDistribution(generator) * 11.8336 * 0.001;
                //fGParticleSource->GetCurrentSource()->GetEneDist()->SetMonoEnergy(EventEnergy * MeV);
                fGParticleSource->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Lin");
                //fGParticleSource->GetCurrentSource()->GetEneDist()->SetEmin(0.001823  * MeV);
                //fGParticleSource->GetCurrentSource()->GetEneDist()->SetEmax(3.03286 * MeV);
                fGParticleSource->GetCurrentSource()->GetEneDist()->SetEmin(2.615 * MeV);
                fGParticleSource->GetCurrentSource()->GetEneDist()->SetEmax(2.616 * MeV);
                fGParticleSource->GetCurrentSource()->GetEneDist()->SetGradient(0.);
                fGParticleSource->GetCurrentSource()->GetEneDist()->SetInterCept(1.);
            }
            else if (energyDistributionFlag == 1)
            {
                //std::random_device rd;
                //std::mt19937 gen(rd());
                std::uniform_real_distribution<> dis(-1.0, 1.0);
                // 生成随机数
                //kinetic energy
                double EventEnergy = (EnergyDistribution(generator)) * 11.8400 * 0.001 + 1.823 * 0.001 + 11.8400 / 2.0 * 0.001; //* dis(generator);
                fGParticleSource->GetCurrentSource()->GetEneDist()->SetMonoEnergy(EventEnergy * MeV);
                //for (int i = 1; i <= 100000; i++)
                //{
                //    std::cout << EnergyDistribution(generator) << ' ';
                //}
            }
            else if (energyDistributionFlag == 2)
            {
                fGParticleSource->SetParticleDefinition(G4NeutrinoE::NeutrinoEDefinition());
                fGParticleSource->GetCurrentSource()->GetEneDist()->SetMonoEnergy(1 * TeV);
            }

            //direction
            int dirFlag = 0; //0:cos 2:CZT 3: halfsphere

            //source geometry
            fGParticleSource->GetCurrentSource()->GetPosDist()->SetPosDisType("Surface");
            fGParticleSource->GetCurrentSource()->GetPosDist()->SetPosDisShape("Sphere");
            fGParticleSource->GetCurrentSource()->GetPosDist()->SetCentreCoords(G4ThreeVector(0.0 * cm, 0.0 * cm, -34 * mm));
            fGParticleSource->GetCurrentSource()->GetPosDist()->SetRadius(15 * cm);
            G4ThreeVector position;
            G4ThreeVector direction;
            if (dirFlag == 3)
            {
                fGParticleSource->GetCurrentSource()->GetAngDist()->SetAngDistType("cos");
                do {
                    position = fGParticleSource->GetCurrentSource()->GetPosDist()->GenerateOne();
                    direction = fGParticleSource->GetCurrentSource()->GetAngDist()->GenerateOne();

                } while (position.z() >= -34. * mm); // 检查z坐标是否大于球心的z坐标
                fGParticleSource->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
                fGParticleSource->GetCurrentSource()->GetPosDist()->SetCentreCoords(position);
                fGParticleSource->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(direction);
            }

            if (dirFlag == 0)
            {
                // 获取UI命令管理器的实例
                G4UImanager* UImanager = G4UImanager::GetUIpointer();

                // 设置/gps/ang/surfnorm为true
                UImanager->ApplyCommand("/gps/ang/surfnorm true");
                fGParticleSource->GetCurrentSource()->GetAngDist()->SetAngDistType("cos");
                UImanager->ApplyCommand("/gps/ang/mintheta 0. deg");
                UImanager->ApplyCommand("/gps/ang/maxtheta 90. deg");
            }
            else if (dirFlag == 1)
            {
                G4double r = sqrt(G4UniformRand()) * 12.5 * mm;
                G4double phi = G4UniformRand() * 3.14159 * 2;
                G4double z = G4UniformRand() * 22.5 * mm * 2 - 22.5 * mm;
                G4ThreeVector FocusedPoint(r * cos(phi), r * sin(phi), z - 391.5 * mm);
                fGParticleSource->GetCurrentSource()->GetAngDist()->SetAngDistType("focused");
                fGParticleSource->GetCurrentSource()->GetAngDist()->SetFocusPoint(FocusedPoint);
            }
            else if (dirFlag == 2)
            {
                G4double x = (2 * G4UniformRand() - 1) * 5.0 * mm;
                G4double y = (2 * G4UniformRand() - 1) * 2.5 * mm;
                G4double z = (2 * G4UniformRand() - 1) * 5.0 * mm;
                G4ThreeVector FocusedPoint(x, y, z - 34 * mm);
                fGParticleSource->GetCurrentSource()->GetAngDist()->SetAngDistType("focused");
                fGParticleSource->GetCurrentSource()->GetAngDist()->SetFocusPoint(FocusedPoint);
            }
        }
    }

    //generate
    fGParticleSource->GeneratePrimaryVertex(anEvent);
   
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
