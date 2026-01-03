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

#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;

class DetectorConstruction;

/// Messenger for geometry configuration
///
/// Commands:
/// - /my_geom/worldXY, worldZ - World dimensions
/// - /my_geom/phantom/... - Phantom configuration
/// - /my_geom/detector/... - Detector grid configuration

class DetectorMessenger : public G4UImessenger
{
public:
  DetectorMessenger(DetectorConstruction* geom);
  virtual ~DetectorMessenger() override;
    
  void SetNewValue(G4UIcommand* command, G4String newValue) override;

private:
  DetectorConstruction* fGeom;

  //============================================
  // Directories
  //============================================
  G4UIdirectory* fGeomDir;
  G4UIdirectory* fPhantomDir;
  G4UIdirectory* fDetectorDir;

  //============================================
  // World commands
  //============================================
  G4UIcmdWithADoubleAndUnit* fWorldXYCmd;
  G4UIcmdWithADoubleAndUnit* fWorldZCmd;

  //============================================
  // Phantom commands
  //============================================
  G4UIcmdWithAString* fPhantomMaterialCmd;
  G4UIcmdWithADoubleAndUnit* fPhantomHalfXCmd;
  G4UIcmdWithADoubleAndUnit* fPhantomHalfYCmd;
  G4UIcmdWithADoubleAndUnit* fPhantomHalfZCmd;
  G4UIcmdWithADoubleAndUnit* fPhantomPosZCmd;

  //============================================
  // Detector commands
  //============================================
  G4UIcmdWithAString* fDetectorMaterialCmd;
  G4UIcmdWithADoubleAndUnit* fDetectorHalfXCmd;
  G4UIcmdWithADoubleAndUnit* fDetectorHalfYCmd;
  G4UIcmdWithADoubleAndUnit* fDetectorHalfZCmd;
  G4UIcmdWithAnInteger* fDetectorGridNCmd;
  G4UIcmdWithADoubleAndUnit* fDetectorSpacingCmd;
  G4UIcmdWithAnInteger* fDetectorLayersCmd;
  G4UIcmdWithADoubleAndUnit* fDetectorLayerSpacingCmd;
  G4UIcmdWithADoubleAndUnit* fDetectorFirstLayerZCmd;
};

#endif
