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

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "G4SDManager.hh"

#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
{
  G4NistManager* man = G4NistManager::Instance();

  fMessenger = new DetectorMessenger(this);

  //============================================
  // World defaults
  //============================================
  fWorldXY = 50.*cm;
  fWorldZ = 100.*cm;
  fWorldMat = man->FindOrBuildMaterial("G4_Galactic");

  //============================================
  // Phantom defaults
  //============================================
  fPhantomMaterialName = "G4_WATER";
  fPhantomMat = man->FindOrBuildMaterial(fPhantomMaterialName);
  fPhantomHalfX = 15.*cm;
  fPhantomHalfY = 15.*cm;
  fPhantomHalfZ = 20.*cm;
  fPhantomPosZ = 0.*cm;

  //============================================
  // Detector grid defaults
  //============================================
  fDetectorMaterialName = "G4_Si";
  fDetectorMat = man->FindOrBuildMaterial(fDetectorMaterialName);
  fDetectorHalfX = 0.1325*cm;    // Half of 0.265 cm
  fDetectorHalfY = 0.1325*cm;    // Half of 0.265 cm
  fDetectorHalfZ = 0.03*cm;      // Half of 0.06 cm
  fDetectorGridN = 40;           // 40x40 grid
  fDetectorSpacing = 0.8*cm;     // Center-to-center
  fDetectorNumLayers = 5;
  fDetectorLayerSpacing = 2.*cm;
  fDetectorFirstLayerZ = 15.*cm;

  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
  delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetPhantomMaterial(const G4String& materialName)
{
  G4NistManager* man = G4NistManager::Instance();
  G4Material* mat = man->FindOrBuildMaterial(materialName);
  if (mat) {
    fPhantomMaterialName = materialName;
    fPhantomMat = mat;
    G4cout << "Phantom material set to: " << materialName << G4endl;
  } else {
    G4cerr << "ERROR: Material " << materialName << " not found!" << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetDetectorMaterial(const G4String& materialName)
{
  G4NistManager* man = G4NistManager::Instance();
  G4Material* mat = man->FindOrBuildMaterial(materialName);
  if (mat) {
    fDetectorMaterialName = materialName;
    fDetectorMat = mat;
    G4cout << "Detector material set to: " << materialName << G4endl;
  } else {
    G4cerr << "ERROR: Material " << materialName << " not found!" << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  DumpGeometryParameters();

  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  //============================================
  // WORLD
  //============================================
  G4Box* sWorld = new G4Box("World", fWorldXY, fWorldXY, fWorldZ);
  G4LogicalVolume* lWorld = new G4LogicalVolume(sWorld, fWorldMat, "World");
  G4VPhysicalVolume* phWorld = new G4PVPlacement(0, G4ThreeVector(),
                                                  "World", lWorld,
                                                  0, false, 0);

  auto visWorld = new G4VisAttributes(G4Colour(0.1, 0.5, 1.0));
  visWorld->SetVisibility(true);
  lWorld->SetVisAttributes(visWorld);

  //============================================
  // PHANTOM (inside world)
  //============================================
  G4Box* sPhantom = new G4Box("Phantom", fPhantomHalfX, fPhantomHalfY, fPhantomHalfZ);
  G4LogicalVolume* lPhantom = new G4LogicalVolume(sPhantom, fPhantomMat, "Phantom");
  new G4PVPlacement(0, G4ThreeVector(0, 0, fPhantomPosZ),
                    "Phantom", lPhantom, phWorld, false, 0);

  auto visPhantom = new G4VisAttributes(G4Colour(0.0, 0.8, 0.8, 0.3));
  visPhantom->SetVisibility(true);
  visPhantom->SetForceSolid(true);
  lPhantom->SetVisAttributes(visPhantom);

  //============================================
  // DETECTOR GRID (inside phantom)
  //============================================
  solidDetector = new G4Box("Detector", fDetectorHalfX, fDetectorHalfY, fDetectorHalfZ);
  logicDetector = new G4LogicalVolume(solidDetector, fDetectorMat, "logicDetector");

  auto visDetector = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));
  visDetector->SetVisibility(true);
  logicDetector->SetVisAttributes(visDetector);

  G4int detectorCopyNumber = 1;
  
  for (G4int layer = 0; layer < fDetectorNumLayers; layer++) {
    for (G4int i = 0; i < fDetectorGridN; i++) {
      for (G4int j = 0; j < fDetectorGridN; j++) {
        G4double xPos = -0.5 * (fDetectorGridN - 1) * fDetectorSpacing + i * fDetectorSpacing;
        G4double yPos = -0.5 * (fDetectorGridN - 1) * fDetectorSpacing + j * fDetectorSpacing;
        G4double zPos = fDetectorFirstLayerZ - layer * fDetectorLayerSpacing;

        new G4PVPlacement(0,
                          G4ThreeVector(xPos, yPos, zPos),
                          logicDetector,
                          "physDetector",
                          lWorld,  // Placed in world (detectors can be inside or outside phantom)
                          false,
                          detectorCopyNumber++);
      }
    }
  }

  G4cout << "Created " << (detectorCopyNumber - 1) << " detectors" << G4endl;

  return phWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  auto sensDet = new MySensitiveDetector("SensitiveDetector", "DoseHitsCollection");
  G4SDManager::GetSDMpointer()->AddNewDetector(sensDet);
  logicDetector->SetSensitiveDetector(sensDet);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DumpGeometryParameters()
{
  G4cout << "\n===================================================" << G4endl;
  G4cout << "#                 GOSS Geometry                   #" << G4endl;
  G4cout << "===================================================" << G4endl;
  G4cout << "  World:" << G4endl;
  G4cout << "    XY half-size = " << fWorldXY/cm << " cm" << G4endl;
  G4cout << "    Z half-size  = " << fWorldZ/cm << " cm" << G4endl;
  G4cout << "  Phantom:" << G4endl;
  G4cout << "    Material     = " << fPhantomMaterialName << G4endl;
  G4cout << "    Half-size    = " << fPhantomHalfX/cm << " x " 
         << fPhantomHalfY/cm << " x " << fPhantomHalfZ/cm << " cm" << G4endl;
  G4cout << "    Position Z   = " << fPhantomPosZ/cm << " cm" << G4endl;
  G4cout << "  Detectors:" << G4endl;
  G4cout << "    Material     = " << fDetectorMaterialName << G4endl;
  G4cout << "    Half-size    = " << fDetectorHalfX/cm << " x "
         << fDetectorHalfY/cm << " x " << fDetectorHalfZ/cm << " cm" << G4endl;
  G4cout << "    Grid         = " << fDetectorGridN << " x " << fDetectorGridN << G4endl;
  G4cout << "    Spacing      = " << fDetectorSpacing/cm << " cm" << G4endl;
  G4cout << "    Layers       = " << fDetectorNumLayers << G4endl;
  G4cout << "    Layer sep    = " << fDetectorLayerSpacing/cm << " cm" << G4endl;
  G4cout << "    First layer Z= " << fDetectorFirstLayerZ/cm << " cm" << G4endl;
  G4cout << "===================================================" << G4endl;
}
