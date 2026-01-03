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


#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorMessenger.hh"
#include "G4IAEAphspReader.hh"

#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4GeneralParticleSource.hh"
#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(const G4int threads)
  :fThreads(threads)
{
  fIAEAphspReaderName = "";
  fVerbose = 0;
  fMessenger = new PrimaryGeneratorMessenger(this);
  
  // Use GPS for flexible particle source configuration
  fGPS = new G4GeneralParticleSource();
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  if (fVerbose > 0) G4cout << "Destroying PrimaryGeneratorAction" << G4endl;
  delete fGPS;
  delete fMessenger;
  
  if (fIAEAphspReader) delete fIAEAphspReader;
  if (fVerbose > 0) G4cout << "PrimaryGeneratorAction destroyed" << G4endl;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // Priority: PHSP reader > GPS
  if (fIAEAphspReader) {
    fIAEAphspReader->GeneratePrimaryVertex(anEvent);
  }
  else {
    // Use GPS - configured via /gps/... commands
    fGPS->GeneratePrimaryVertex(anEvent);
  }
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::SetIAEAphspReader(const G4String filename)
{
  fIAEAphspReaderName = filename;
  fIAEAphspReader = new G4IAEAphspReader(filename, fThreads);
}
