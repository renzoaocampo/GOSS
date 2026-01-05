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

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4GenericMessenger.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"

#include "SensitiveDetector.hh"
#include "LinacInfinityGeometry.hh"

class G4LogicalVolume;
class G4Material;
class DetectorMessenger;

/// Detector construction with configurable phantom and detector grid
///
/// Geometry parameters can be set via UI commands:
/// - /my_geom/worldXY, /my_geom/worldZ - World dimensions
/// - /my_geom/phantom/... - Phantom material and dimensions
/// - /my_geom/detector/... - Detector grid configuration

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  DetectorConstruction();
  virtual ~DetectorConstruction() override;

  G4VPhysicalVolume* Construct() override;

  G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; }

  void DumpGeometryParameters();

  //============================================
  // World setters
  //============================================
  inline void SetWorldXY(G4double val) { fWorldXY = val; }
  inline void SetWorldZ(G4double val)  { fWorldZ = val; }

  //============================================
  // Phantom setters
  //============================================
  void SetPhantomMaterial(const G4String& materialName);
  inline void SetPhantomHalfSizeX(G4double val) { fPhantomHalfX = val; }
  inline void SetPhantomHalfSizeY(G4double val) { fPhantomHalfY = val; }
  inline void SetPhantomHalfSizeZ(G4double val) { fPhantomHalfZ = val; }
  inline void SetPhantomPositionZ(G4double val) { fPhantomPosZ = val; }

  //============================================
  // Detector grid setters
  //============================================
  void SetDetectorMaterial(const G4String& materialName);
  inline void SetDetectorHalfSizeX(G4double val) { fDetectorHalfX = val; }
  inline void SetDetectorHalfSizeY(G4double val) { fDetectorHalfY = val; }
  inline void SetDetectorHalfSizeZ(G4double val) { fDetectorHalfZ = val; }
  inline void SetDetectorGridSize(G4int val)     { fDetectorGridN = val; }
  inline void SetDetectorSpacing(G4double val)   { fDetectorSpacing = val; }
  inline void SetDetectorNumLayers(G4int val)    { fDetectorNumLayers = val; }
  inline void SetDetectorLayerSpacing(G4double val) { fDetectorLayerSpacing = val; }
  inline void SetDetectorFirstLayerZ(G4double val)  { fDetectorFirstLayerZ = val; }

  //============================================
  // LINAC geometry control
  //============================================
  inline void EnableLinac(bool enable) { fEnableLinac = enable; }
  inline void SetLinacFieldSize(G4double fieldSize) { fLinacFieldSize = fieldSize; }

private:
  //============================================
  // World parameters
  //============================================
  G4double fWorldXY;
  G4double fWorldZ;
  G4Material* fWorldMat;

  //============================================
  // Phantom parameters
  //============================================
  G4String fPhantomMaterialName;
  G4Material* fPhantomMat;
  G4double fPhantomHalfX;      // Half-size in X
  G4double fPhantomHalfY;      // Half-size in Y
  G4double fPhantomHalfZ;      // Half-size in Z
  G4double fPhantomPosZ;       // Center Z position

  //============================================
  // Detector grid parameters
  //============================================
  G4String fDetectorMaterialName;
  G4Material* fDetectorMat;
  G4double fDetectorHalfX;     // Detector half-size X
  G4double fDetectorHalfY;     // Detector half-size Y
  G4double fDetectorHalfZ;     // Detector half-size Z (thickness/2)
  G4int fDetectorGridN;        // Number of detectors per row/column
  G4double fDetectorSpacing;   // Center-to-center spacing
  G4int fDetectorNumLayers;    // Number of detector layers
  G4double fDetectorLayerSpacing;  // Z spacing between layers
  G4double fDetectorFirstLayerZ;   // Z position of first layer

  //============================================
  // Internal objects
  //============================================
  DetectorMessenger* fMessenger;
  G4Box* solidDetector;
  G4LogicalVolume* logicDetector;
  G4VPhysicalVolume* physDetector;
  G4LogicalVolume* fScoringVolume;

  //============================================
  // LINAC geometry
  //============================================
  LinacInfinityGeometry* fLinacGeometry;
  bool fEnableLinac;
  G4double fLinacFieldSize;

  virtual void ConstructSDandField() override;
};

#endif
