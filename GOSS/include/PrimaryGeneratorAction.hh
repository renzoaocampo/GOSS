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

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4Event;
class G4GeneralParticleSource;
class G4IAEAphspReader;
class PrimaryGeneratorMessenger;

/// Primary generator action using GPS or IAEA phase-space reader
///
/// Priority: If IAEA PHSP reader is configured, it takes precedence.
/// Otherwise, uses G4GeneralParticleSource (configurable via /gps/... commands)

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction(const G4int threads);
  virtual ~PrimaryGeneratorAction() override;
 
  void GeneratePrimaries(G4Event* anEvent) override;

  // PHSP reader configuration
  void SetIAEAphspReader(const G4String filename);
  inline G4IAEAphspReader* GetIAEAphspReader() const { return fIAEAphspReader; }
  
  // Verbose control
  inline void SetVerbose(const G4int val) { fVerbose = val; }
  inline G4int GetVerbose() const { return fVerbose; }

private:
  // GPS for primary generation (configured via /gps/... commands)
  G4GeneralParticleSource* fGPS = nullptr;
  
  // PHSP reader (takes priority over GPS if configured)
  G4IAEAphspReader* fIAEAphspReader = nullptr;
  G4int fThreads;
  G4String fIAEAphspReaderName;

  G4int fVerbose;
  PrimaryGeneratorMessenger* fMessenger;
};

#endif
