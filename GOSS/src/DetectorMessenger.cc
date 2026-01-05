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

#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

DetectorMessenger::DetectorMessenger(DetectorConstruction* geom)
  : fGeom(geom)
{
  //============================================
  // DIRECTORIES
  //============================================
  fGeomDir = new G4UIdirectory("/goss/geom/");
  fGeomDir->SetGuidance("GOSS Geometry configuration commands.");

  fPhantomDir = new G4UIdirectory("/goss/geom/phantom/");
  fPhantomDir->SetGuidance("Phantom geometry configuration.");

  fDetectorDir = new G4UIdirectory("/goss/geom/detector/");
  fDetectorDir->SetGuidance("Detector grid configuration.");

  fLinacDir = new G4UIdirectory("/goss/geom/linac/");
  fLinacDir->SetGuidance("LINAC geometry configuration.");

  fLinacInfinityDir = new G4UIdirectory("/goss/geom/linac/ElektaInfinity6MV/");
  fLinacInfinityDir->SetGuidance("Elekta Infinity 6MV LINAC configuration.");

  //============================================
  // WORLD COMMANDS
  //============================================
  fWorldXYCmd = new G4UIcmdWithADoubleAndUnit("/goss/geom/worldXY", this);
  fWorldXYCmd->SetGuidance("Set HALF-SIZE of world in X and Y directions.");
  fWorldXYCmd->SetGuidance("Example: /goss/geom/worldXY 50 cm -> world is 100cm x 100cm");
  fWorldXYCmd->SetParameterName("worldHalfXY", false);
  fWorldXYCmd->SetUnitCategory("Length");
  fWorldXYCmd->AvailableForStates(G4State_PreInit);

  fWorldZCmd = new G4UIcmdWithADoubleAndUnit("/goss/geom/worldZ", this);
  fWorldZCmd->SetGuidance("Set HALF-SIZE of world in Z direction.");
  fWorldZCmd->SetGuidance("Example: /goss/geom/worldZ 100 cm -> world extends -100 to +100 cm");
  fWorldZCmd->SetParameterName("worldHalfZ", false);
  fWorldZCmd->SetUnitCategory("Length");
  fWorldZCmd->AvailableForStates(G4State_PreInit);

  //============================================
  // PHANTOM COMMANDS
  //============================================
  fPhantomMaterialCmd = new G4UIcmdWithAString("/goss/geom/phantom/material", this);
  fPhantomMaterialCmd->SetGuidance("Set phantom material using NIST name.");
  fPhantomMaterialCmd->SetGuidance("Examples: G4_WATER, G4_PMMA, G4_TISSUE_SOFT_ICRP");
  fPhantomMaterialCmd->SetParameterName("material", false);
  fPhantomMaterialCmd->AvailableForStates(G4State_PreInit);

  fPhantomHalfXCmd = new G4UIcmdWithADoubleAndUnit("/goss/geom/phantom/halfSizeX", this);
  fPhantomHalfXCmd->SetGuidance("Set phantom HALF-SIZE in X direction.");
  fPhantomHalfXCmd->SetGuidance("Full size = 2 * halfSize. Example: 15 cm -> 30 cm total");
  fPhantomHalfXCmd->SetParameterName("phantomHalfX", false);
  fPhantomHalfXCmd->SetUnitCategory("Length");
  fPhantomHalfXCmd->AvailableForStates(G4State_PreInit);

  fPhantomHalfYCmd = new G4UIcmdWithADoubleAndUnit("/goss/geom/phantom/halfSizeY", this);
  fPhantomHalfYCmd->SetGuidance("Set phantom HALF-SIZE in Y direction.");
  fPhantomHalfYCmd->SetGuidance("Full size = 2 * halfSize. Example: 15 cm -> 30 cm total");
  fPhantomHalfYCmd->SetParameterName("phantomHalfY", false);
  fPhantomHalfYCmd->SetUnitCategory("Length");
  fPhantomHalfYCmd->AvailableForStates(G4State_PreInit);

  fPhantomHalfZCmd = new G4UIcmdWithADoubleAndUnit("/goss/geom/phantom/halfSizeZ", this);
  fPhantomHalfZCmd->SetGuidance("Set phantom HALF-SIZE in Z direction (depth/2).");
  fPhantomHalfZCmd->SetGuidance("Full depth = 2 * halfSize. Example: 20 cm -> 40 cm total");
  fPhantomHalfZCmd->SetParameterName("phantomHalfZ", false);
  fPhantomHalfZCmd->SetUnitCategory("Length");
  fPhantomHalfZCmd->AvailableForStates(G4State_PreInit);

  fPhantomPosZCmd = new G4UIcmdWithADoubleAndUnit("/goss/geom/phantom/positionZ", this);
  fPhantomPosZCmd->SetGuidance("Set Z position of phantom CENTER.");
  fPhantomPosZCmd->SetGuidance("Example: 10 cm -> phantom center at z=10cm");
  fPhantomPosZCmd->SetParameterName("phantomPosZ", false);
  fPhantomPosZCmd->SetUnitCategory("Length");
  fPhantomPosZCmd->AvailableForStates(G4State_PreInit);

  //============================================
  // DETECTOR COMMANDS
  //============================================
  fDetectorMaterialCmd = new G4UIcmdWithAString("/goss/geom/detector/material", this);
  fDetectorMaterialCmd->SetGuidance("Set detector material using NIST name.");
  fDetectorMaterialCmd->SetGuidance("Examples: G4_Si, G4_SILICON_DIOXIDE, G4_Ge");
  fDetectorMaterialCmd->SetParameterName("material", false);
  fDetectorMaterialCmd->AvailableForStates(G4State_PreInit);

  fDetectorHalfXCmd = new G4UIcmdWithADoubleAndUnit("/goss/geom/detector/halfSizeX", this);
  fDetectorHalfXCmd->SetGuidance("Set detector HALF-SIZE in X direction.");
  fDetectorHalfXCmd->SetGuidance("Full size = 2 * halfSize. Example: 0.1325 cm -> 0.265 cm total");
  fDetectorHalfXCmd->SetParameterName("detHalfX", false);
  fDetectorHalfXCmd->SetUnitCategory("Length");
  fDetectorHalfXCmd->AvailableForStates(G4State_PreInit);

  fDetectorHalfYCmd = new G4UIcmdWithADoubleAndUnit("/goss/geom/detector/halfSizeY", this);
  fDetectorHalfYCmd->SetGuidance("Set detector HALF-SIZE in Y direction.");
  fDetectorHalfYCmd->SetGuidance("Full size = 2 * halfSize. Example: 0.1325 cm -> 0.265 cm total");
  fDetectorHalfYCmd->SetParameterName("detHalfY", false);
  fDetectorHalfYCmd->SetUnitCategory("Length");
  fDetectorHalfYCmd->AvailableForStates(G4State_PreInit);

  fDetectorHalfZCmd = new G4UIcmdWithADoubleAndUnit("/goss/geom/detector/halfThickness", this);
  fDetectorHalfZCmd->SetGuidance("Set detector HALF-THICKNESS (Z direction).");
  fDetectorHalfZCmd->SetGuidance("Full thickness = 2 * halfThickness. Example: 0.03 cm -> 0.06 cm");
  fDetectorHalfZCmd->SetParameterName("detHalfZ", false);
  fDetectorHalfZCmd->SetUnitCategory("Length");
  fDetectorHalfZCmd->AvailableForStates(G4State_PreInit);

  fDetectorGridNCmd = new G4UIcmdWithAnInteger("/goss/geom/detector/gridSize", this);
  fDetectorGridNCmd->SetGuidance("Set number of detectors per row AND column.");
  fDetectorGridNCmd->SetGuidance("Total detectors per layer = gridSize^2. Example: 40 -> 1600 detectors/layer");
  fDetectorGridNCmd->SetParameterName("gridN", false);
  fDetectorGridNCmd->SetRange("gridN>0");
  fDetectorGridNCmd->AvailableForStates(G4State_PreInit);

  fDetectorSpacingCmd = new G4UIcmdWithADoubleAndUnit("/goss/geom/detector/spacing", this);
  fDetectorSpacingCmd->SetGuidance("Set CENTER-TO-CENTER spacing between adjacent detectors.");
  fDetectorSpacingCmd->SetGuidance("Must be >= detector full size. Example: 0.8 cm");
  fDetectorSpacingCmd->SetParameterName("spacing", false);
  fDetectorSpacingCmd->SetUnitCategory("Length");
  fDetectorSpacingCmd->AvailableForStates(G4State_PreInit);

  fDetectorLayersCmd = new G4UIcmdWithAnInteger("/goss/geom/detector/numLayers", this);
  fDetectorLayersCmd->SetGuidance("Set number of detector layers along Z axis.");
  fDetectorLayersCmd->SetGuidance("Example: 5 -> 5 layers of detector grids");
  fDetectorLayersCmd->SetParameterName("numLayers", false);
  fDetectorLayersCmd->SetRange("numLayers>0");
  fDetectorLayersCmd->AvailableForStates(G4State_PreInit);

  fDetectorLayerSpacingCmd = new G4UIcmdWithADoubleAndUnit("/goss/geom/detector/layerSpacing", this);
  fDetectorLayerSpacingCmd->SetGuidance("Set Z distance between consecutive detector layers.");
  fDetectorLayerSpacingCmd->SetGuidance("Layers at: firstZ, firstZ-spacing, firstZ-2*spacing, ...");
  fDetectorLayerSpacingCmd->SetParameterName("layerSpacing", false);
  fDetectorLayerSpacingCmd->SetUnitCategory("Length");
  fDetectorLayerSpacingCmd->AvailableForStates(G4State_PreInit);

  fDetectorFirstLayerZCmd = new G4UIcmdWithADoubleAndUnit("/goss/geom/detector/firstLayerZ", this);
  fDetectorFirstLayerZCmd->SetGuidance("Set Z position of the FIRST (shallowest) detector layer.");
  fDetectorFirstLayerZCmd->SetGuidance("Subsequent layers are at firstZ - n*layerSpacing");
  fDetectorFirstLayerZCmd->SetParameterName("firstLayerZ", false);
  fDetectorFirstLayerZCmd->SetUnitCategory("Length");
  fDetectorFirstLayerZCmd->AvailableForStates(G4State_PreInit);

  //============================================
  // LINAC ELEKTA INFINITY 6MV COMMANDS
  //============================================
  fLinacInfinityEnableCmd = new G4UIcmdWithAString("/goss/geom/linac/ElektaInfinity6MV/enable", this);
  fLinacInfinityEnableCmd->SetGuidance("Enable/disable Elekta Infinity 6MV LINAC geometry.");
  fLinacInfinityEnableCmd->SetGuidance("Usage: /goss/geom/linac/ElektaInfinity6MV/enable true");
  fLinacInfinityEnableCmd->SetGuidance("       /goss/geom/linac/ElektaInfinity6MV/enable false");
  fLinacInfinityEnableCmd->SetParameterName("enable", false);
  fLinacInfinityEnableCmd->SetCandidates("true false");
  fLinacInfinityEnableCmd->AvailableForStates(G4State_PreInit);

  fLinacInfinityFieldSizeCmd = new G4UIcmdWithADoubleAndUnit("/goss/geom/linac/ElektaInfinity6MV/fieldSize", this);
  fLinacInfinityFieldSizeCmd->SetGuidance("Set Elekta Infinity 6MV field size at isocenter (square field).");
  fLinacInfinityFieldSizeCmd->SetGuidance("MLC and Jaws will automatically adjust to this field size.");
  fLinacInfinityFieldSizeCmd->SetGuidance("Example: /goss/geom/linac/ElektaInfinity6MV/fieldSize 15 cm -> 15x15 cm field");
  fLinacInfinityFieldSizeCmd->SetParameterName("fieldSize", false);
  fLinacInfinityFieldSizeCmd->SetUnitCategory("Length");
  fLinacInfinityFieldSizeCmd->SetRange("fieldSize>0");
  fLinacInfinityFieldSizeCmd->AvailableForStates(G4State_PreInit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

DetectorMessenger::~DetectorMessenger()
{
  // Directories
  delete fGeomDir;
  delete fPhantomDir;
  delete fDetectorDir;

  // World
  delete fWorldXYCmd;
  delete fWorldZCmd;

  // Phantom
  delete fPhantomMaterialCmd;
  delete fPhantomHalfXCmd;
  delete fPhantomHalfYCmd;
  delete fPhantomHalfZCmd;
  delete fPhantomPosZCmd;

  // Detector
  delete fDetectorMaterialCmd;
  delete fDetectorHalfXCmd;
  delete fDetectorHalfYCmd;
  delete fDetectorHalfZCmd;
  delete fDetectorGridNCmd;
  delete fDetectorSpacingCmd;
  delete fDetectorLayersCmd;
  delete fDetectorLayerSpacingCmd;
  delete fDetectorFirstLayerZCmd;

  // LINAC
  delete fLinacDir;
  delete fLinacInfinityDir;
  delete fLinacInfinityEnableCmd;
  delete fLinacInfinityFieldSizeCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  //============================================
  // WORLD
  //============================================
  if (command == fWorldXYCmd)
    fGeom->SetWorldXY(fWorldXYCmd->GetNewDoubleValue(newValue));
  else if (command == fWorldZCmd)
    fGeom->SetWorldZ(fWorldZCmd->GetNewDoubleValue(newValue));

  //============================================
  // PHANTOM
  //============================================
  else if (command == fPhantomMaterialCmd)
    fGeom->SetPhantomMaterial(newValue);
  else if (command == fPhantomHalfXCmd)
    fGeom->SetPhantomHalfSizeX(fPhantomHalfXCmd->GetNewDoubleValue(newValue));
  else if (command == fPhantomHalfYCmd)
    fGeom->SetPhantomHalfSizeY(fPhantomHalfYCmd->GetNewDoubleValue(newValue));
  else if (command == fPhantomHalfZCmd)
    fGeom->SetPhantomHalfSizeZ(fPhantomHalfZCmd->GetNewDoubleValue(newValue));
  else if (command == fPhantomPosZCmd)
    fGeom->SetPhantomPositionZ(fPhantomPosZCmd->GetNewDoubleValue(newValue));

  //============================================
  // DETECTOR
  //============================================
  else if (command == fDetectorMaterialCmd)
    fGeom->SetDetectorMaterial(newValue);
  else if (command == fDetectorHalfXCmd)
    fGeom->SetDetectorHalfSizeX(fDetectorHalfXCmd->GetNewDoubleValue(newValue));
  else if (command == fDetectorHalfYCmd)
    fGeom->SetDetectorHalfSizeY(fDetectorHalfYCmd->GetNewDoubleValue(newValue));
  else if (command == fDetectorHalfZCmd)
    fGeom->SetDetectorHalfSizeZ(fDetectorHalfZCmd->GetNewDoubleValue(newValue));
  else if (command == fDetectorGridNCmd)
    fGeom->SetDetectorGridSize(fDetectorGridNCmd->GetNewIntValue(newValue));
  else if (command == fDetectorSpacingCmd)
    fGeom->SetDetectorSpacing(fDetectorSpacingCmd->GetNewDoubleValue(newValue));
  else if (command == fDetectorLayersCmd)
    fGeom->SetDetectorNumLayers(fDetectorLayersCmd->GetNewIntValue(newValue));
  else if (command == fDetectorLayerSpacingCmd)
    fGeom->SetDetectorLayerSpacing(fDetectorLayerSpacingCmd->GetNewDoubleValue(newValue));
  else if (command == fDetectorFirstLayerZCmd)
    fGeom->SetDetectorFirstLayerZ(fDetectorFirstLayerZCmd->GetNewDoubleValue(newValue));

  //============================================
  // LINAC ELEKTA INFINITY 6MV
  //============================================
  else if (command == fLinacInfinityEnableCmd) {
    if (newValue == "true" || newValue == "1")
      fGeom->EnableLinac(true);
    else if (newValue == "false" || newValue == "0")
      fGeom->EnableLinac(false);
  }
  else if (command == fLinacInfinityFieldSizeCmd)
    fGeom->SetLinacFieldSize(fLinacInfinityFieldSizeCmd->GetNewDoubleValue(newValue));
}
