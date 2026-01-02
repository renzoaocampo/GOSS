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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include"G4SDManager.hh"

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
  //man->SetVerbose(1);

  fMessenger = new DetectorMessenger(this);

  fWorldMat   = man->FindOrBuildMaterial("G4_Galactic");

  fDetectordMat   = man->FindOrBuildMaterial("G4_Si");
  fWorldXY = 50.*cm;
  fWorldZ = 100.*cm;

  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
  delete fMessenger;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  DumpGeometryParameters();

  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  //
  // World
  //

  G4Box* sWorld = new G4Box("World",
			    fWorldXY, fWorldXY, fWorldZ);
  G4LogicalVolume* lWorld = new G4LogicalVolume(sWorld,
						fWorldMat, "World");
  G4VPhysicalVolume* phWorld = new G4PVPlacement(0, G4ThreeVector(),
						 "World", lWorld,
						 0, false, 0);

  //
  // Visualization attributes
  //
  auto visAtt = new G4VisAttributes(G4Colour(0.1,0.5,1.0));
  visAtt->SetVisibility(true);
  lWorld->SetVisAttributes(visAtt);






//--------------------------------


//& Número de detectores en cada fila y columna d
G4int numDetectors =  1; // 23;  

G4int outerNumDetectors =0  ; //31;
//& Separación
G4double separation = 0.8*cm;
G4double outerSeparation = 1.4 * cm; 
//& Capas
G4int numLayers = 5; //7
G4double layerSeparation =  2.*cm;
//& PROFUNDIDAD PRIMERA CAPA
G4double profundidadGrupo= 15*cm;

 G4double SDiodeX = 0.265*cm;
 G4double SDiodeY = 0.265*cm;
 G4double SDiodeZ = 0.06*cm; 
 
 //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^v
// Creación del cubo exterior (coberturadiodo)
 G4int ndeth =1;  
// Crear el volumen sólido del detector
  solidDetector = new G4Box("solidDetector", SDiodeX/2, SDiodeY/2, SDiodeZ/2);

// Crear el volumen lógico del detector
  logicDetector = new G4LogicalVolume(solidDetector,fWorldMat, "logicDetector");
    G4int ndet =1;
// Colocar los detectores en la cuadrícula y capas
for(G4int layer = 0; layer < numLayers; layer++) {
    for(G4int i = 0; i < numDetectors; i++) {
        for(G4int j = 0; j < numDetectors; j++) {
            G4double xPos = -0.5*(numDetectors-1)*separation + i*separation;
            G4double yPos = -0.5*(numDetectors-1)*separation + j*separation;
            G4double zPos = profundidadGrupo -layer * (layerSeparation)  ;
            
            G4VPhysicalVolume *physDetector = new G4PVPlacement(
                0,
                G4ThreeVector(xPos, yPos, zPos),
                logicDetector,
                "physDetector",
                 lWorld ,
                false,
                ndet
                
            );
            ndet++;
        } 
}
}
  
  
//-------------------------------------------


  return phWorld;
}

void DetectorConstruction::ConstructSDandField(){ 
auto sensDet = new MySensitiveDetector("SensitiveDetector", "TrackerHitsCollection");
 G4SDManager::GetSDMpointer()->AddNewDetector(sensDet);
    if(logicDetector != NULL)
  logicDetector->SetSensitiveDetector(sensDet);// logicmosfet es diodo. logic detector es mosfet solo en prototipo 4
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DumpGeometryParameters()
{

  G4cout << "\n===================================================" << G4endl;
  G4cout << "#               IAEAphsp Geometry                 #" << G4endl;
  G4cout << "===================================================" << G4endl;
  G4cout << "  WorldXY = " << fWorldXY/cm << " cm " << G4endl;
  G4cout << "  WorldZ = " << fWorldZ/cm << " cm " << G4endl;
  G4cout << "  WorldMat: " << fWorldMat->GetName() << G4endl;
  G4cout << "===================================================\n" << G4endl;
}

