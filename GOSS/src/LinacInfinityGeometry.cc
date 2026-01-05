#include "LinacInfinityGeometry.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4Region.hh"
#include "G4RotationMatrix.hh"
#include <cmath>

//==============================================================================
// Constructor
//==============================================================================
LinacInfinityGeometry::LinacInfinityGeometry()
    : fWreAlloy(nullptr),
      fAluminum(nullptr),
      fStainlessSteel(nullptr),
      fTungsten(nullptr),
      fShieldingMaterial(nullptr),
      fSSD(100.0 * cm),
      fFieldSize(15.0 * cm)
{
    DefineMaterials();
}

//==============================================================================
// Destructor
//==============================================================================
LinacInfinityGeometry::~LinacInfinityGeometry()
{
    // Geant4 manages memory for materials and volumes
}

//==============================================================================
// Define Materials
//==============================================================================
void LinacInfinityGeometry::DefineMaterials()
{
    G4NistManager* nist = G4NistManager::Instance();
    
    // Tungsten alloy for target (using pure tungsten as approximation)
    fWreAlloy = nist->FindOrBuildMaterial("G4_W");
    
    // Aluminum
    fAluminum = nist->FindOrBuildMaterial("G4_Al");
    
    // Stainless steel
    fStainlessSteel = nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");
    
    // Tungsten for collimators
    fTungsten = nist->FindOrBuildMaterial("G4_W");
    
    // Shielding material (lead)
    fShieldingMaterial = nist->FindOrBuildMaterial("G4_Pb");
}

//==============================================================================
// Main Construction Method
//==============================================================================
void LinacInfinityGeometry::ConstructLinac(G4LogicalVolume* worldLogical, G4double fieldSize)
{
    fFieldSize = fieldSize;
    
    G4cout << "\n========================================" << G4endl;
    G4cout << "  Constructing LINAC Infinity Geometry" << G4endl;
    G4cout << "  SSD = " << fSSD/cm << " cm" << G4endl;
    G4cout << "  Field Size = " << fFieldSize/cm << " cm" << G4endl;
    G4cout << "========================================\n" << G4endl;
    
    // Construct all components in order (from source to patient)
    ConstructTarget(worldLogical);
    ConstructPrimaryCollimator(worldLogical);
    ConstructFlatteningFilter(worldLogical);
    ConstructShielding(worldLogical);
    ConstructMLC(worldLogical);
    ConstructJaws(worldLogical);
    
    G4cout << "LINAC Infinity geometry construction complete.\n" << G4endl;
}

//==============================================================================
// Target
//==============================================================================
void LinacInfinityGeometry::ConstructTarget(G4LogicalVolume* worldLogical)
{
    G4double targetRadius = 1.0 * cm;
    G4double targetThickness = 0.09 * cm;
    G4double targetZOffset = 1.1 * cm;
    
    G4Tubs* solidTarget = new G4Tubs("Target",
        0.,                         // inner radius
        targetRadius,               // outer radius
        targetThickness / 2.,       // half thickness
        0. * deg,                   // start angle
        360. * deg);                // spanning angle
    
    G4LogicalVolume* logicTarget = new G4LogicalVolume(
        solidTarget, fWreAlloy, "TargetLV");
    
    // Create region for production cuts
    G4Region* targetRegion = new G4Region("TargetRegion");
    targetRegion->AddRootLogicalVolume(logicTarget);
    
    // Position (inverted: source now at Z=0)
    G4ThreeVector posTarget(0, 0, targetZOffset);
    new G4PVPlacement(nullptr, posTarget, logicTarget, 
        "Target", worldLogical, false, 0, true);
    
    // Visualization
    G4VisAttributes* targetVis = new G4VisAttributes(G4Colour::Gray());
    targetVis->SetVisibility(true);
    logicTarget->SetVisAttributes(targetVis);
    
    G4cout << "✓ Target constructed" << G4endl;
}

//==============================================================================
// Primary Collimator
//==============================================================================
void LinacInfinityGeometry::ConstructPrimaryCollimator(G4LogicalVolume* worldLogical)
{
    G4double coneHeight = 11.2 * cm;
    G4double coneBaseRadius = 5.0 * cm;
    G4double coneTopRadius = 1.2 * cm;
    G4double bodyOuterRadius = 7.0 * cm;
    
    // Cone hole
    G4Cons* coneHole = new G4Cons("ConeHole",
        0, coneBaseRadius,          // bottom radii
        0, coneTopRadius,           // top radii
        coneHeight / 2,             // half height
        0, 360 * deg);              // angle
    
    // Collimator body
    G4Tubs* collimatorBody = new G4Tubs("CollimatorBody",
        0, bodyOuterRadius,         // radii
        coneHeight / 2,             // half height
        0, 360 * deg);              // angle
    
    // Subtract hole from body
    G4SubtractionSolid* collimatorHollowed = new G4SubtractionSolid(
        "CollimatorWithHole", collimatorBody, coneHole,
        nullptr, G4ThreeVector(0, 0, 0));
    
    G4LogicalVolume* collimatorLogical = new G4LogicalVolume(
        collimatorHollowed, fTungsten, "CollimatorLogical");
    
    // Position (inverted: source now at Z=0)
    G4double zPosition = coneHeight / 2;
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, zPosition),
        collimatorLogical, "PrimaryCollimator", worldLogical, false, 0, true);
    
    // Visualization
    G4VisAttributes* collimatorVis = new G4VisAttributes(G4Colour::Magenta());
    collimatorVis->SetForceSolid(true);
    collimatorLogical->SetVisAttributes(collimatorVis);
    
    G4cout << "✓ Primary Collimator constructed" << G4endl;
}

//==============================================================================
// Flattening Filter (includes aluminum disc)
//==============================================================================
void LinacInfinityGeometry::ConstructFlatteningFilter(G4LogicalVolume* worldLogical)
{
    // Aluminum disc
    G4double discRadius = 4.0 * cm;
    G4double discThickness = 0.5 * cm;
    
    // Flattening filter (cone) - inverted for upward beam
    G4double filterHeight = 2.77 * cm;
    G4double filterRmax1 = 0.0 * cm;  // tip (bottom, toward source)
    G4double filterRmax2 = 4.0 * cm;  // base (top, toward patient)
    
    // Positions (inverted: source now at Z=0)
    G4double filterZ = 15.9 * cm;
    G4double discZ = filterZ + (discThickness / 2 + filterHeight / 2);
    
    // Create disc
    G4Tubs* solidDisc = new G4Tubs("AlDisc", 
        0, discRadius, discThickness / 2, 0, 360 * deg);
    G4LogicalVolume* logicDisc = new G4LogicalVolume(
        solidDisc, fAluminum, "AlDiscLV");
    new G4PVPlacement(0, G4ThreeVector(0, 0, discZ),
        logicDisc, "AlDisc", worldLogical, false, 0, true);
    
    // Create filter (cone inverted)
    G4Cons* solidFilter = new G4Cons("FlatteningFilter",
        0, filterRmax1,             // bottom (tip toward source)
        0, filterRmax2,             // top (base toward patient)
        filterHeight / 2,
        0, 360 * deg);
    G4LogicalVolume* logicFilter = new G4LogicalVolume(
        solidFilter, fStainlessSteel, "FlatteningFilterLV");
    new G4PVPlacement(0, G4ThreeVector(0, 0, filterZ),
        logicFilter, "FlatteningFilter", worldLogical, false, 0, true);
    
    // Visualization
    G4VisAttributes* discVis = new G4VisAttributes(G4Colour::Blue());
    discVis->SetForceSolid(true);
    logicDisc->SetVisAttributes(discVis);
    
    G4VisAttributes* filterVis = new G4VisAttributes(G4Colour::Cyan());
    filterVis->SetForceSolid(true);
    logicFilter->SetVisAttributes(filterVis);
    
    G4cout << "✓ Flattening Filter constructed" << G4endl;
}

//==============================================================================
// Shielding (Revestimiento + Hollow Cylinder)
//==============================================================================
void LinacInfinityGeometry::ConstructShielding(G4LogicalVolume* worldLogical)
{
    // Main shielding cylinder
    G4Tubs* revestimiento = new G4Tubs("Revestimiento",
        63.26 * cm,                 // inner radius
        68.34 * cm,                 // outer radius
        59.625 * cm / 2,            // half height
        0, 360 * deg);
    
    G4LogicalVolume* logicRevestimiento = new G4LogicalVolume(
        revestimiento, fShieldingMaterial, "LogicRevestimiento");
    
    new G4PVPlacement(0, G4ThreeVector(0, 0, 20.6325 * cm),
        logicRevestimiento, "Revestimiento", worldLogical, false, 0, true);
    
    // Upper hollow cylinder
    G4Tubs* cilindroHueco = new G4Tubs("CilindroHueco",
        19 * cm,                    // inner radius
        68.34 * cm,                 // outer radius
        2.0 * cm / 2,               // half height
        0, 360 * deg);
    
    G4LogicalVolume* logicCilindroHueco = new G4LogicalVolume(
        cilindroHueco, fShieldingMaterial, "LogicCilindroHueco");
    
    G4double zPosCilindro = 20.6325 * cm + 59.625 * cm / 2 + 2.0 * cm / 2;
    new G4PVPlacement(0, G4ThreeVector(0, 0, zPosCilindro),
        logicCilindroHueco, "CilindroHueco", worldLogical, false, 0, true);
    
    // Visualization
    G4VisAttributes* revestVis = new G4VisAttributes(G4Colour::Green());
    revestVis->SetForceSolid(true);
    logicRevestimiento->SetVisAttributes(revestVis);
    
    G4VisAttributes* cilindroVis = new G4VisAttributes(G4Colour::Yellow());
    cilindroVis->SetForceSolid(true);
    logicCilindroHueco->SetVisAttributes(cilindroVis);
    
    G4cout << "✓ Shielding constructed" << G4endl;
}

//==============================================================================
// MLC (Multi-Leaf Collimator)
//==============================================================================
void LinacInfinityGeometry::ConstructMLC(G4LogicalVolume* worldLogical)
{
    // MLC parameters
    fMLCParams.leafWidth = 5.0 * mm;
    fMLCParams.leafHeight = 90.0 * mm;
    fMLCParams.leafZPosition = 30.9 * cm;
    fMLCParams.numLeaves = 80;
    fMLCParams.interleafGap = 0.09 * mm;
    fMLCParams.blockX = 34.0 * cm;
    fMLCParams.blockY = 9.0 * cm;
    fMLCParams.blockZ = 9.0 * cm;
    fMLCParams.diskRadius = 17.0 * cm;
    fMLCParams.diskHeight = 5.0 * mm;
    
    // Calculate leaf positions based on field size
    CalculateMLCPositions();
    
    // Create leaf geometry (intersection of block and disk)
    G4Box* blockSolid = new G4Box("Block", 
        fMLCParams.blockX / 2, fMLCParams.blockY / 2, fMLCParams.blockZ / 2);
    
    G4Tubs* diskSolid = new G4Tubs("Disk", 
        0, fMLCParams.diskRadius, fMLCParams.diskHeight / 2, 0, 360.0 * deg);
    
    G4ThreeVector diskOffset(0, -7.5 * mm, 0);
    G4Transform3D transformDisk(G4RotationMatrix(), diskOffset);
    
    G4IntersectionSolid* intersectSolid = new G4IntersectionSolid(
        "LeafSolid", blockSolid, diskSolid, transformDisk);
    
    G4LogicalVolume* leafLogic = new G4LogicalVolume(
        intersectSolid, fTungsten, "LeafLogic");
    
    // Rotation for leaves
    G4RotationMatrix* rotX = new G4RotationMatrix();
    rotX->rotateX(90.0 * deg);
    rotX->rotateY(9 * mrad);
    
    // Place leaves
    G4double pitch = fMLCParams.leafWidth + fMLCParams.interleafGap;
    
    for (int i = 0; i < fMLCParams.numLeaves; ++i) {
        G4double y = -((fMLCParams.numLeaves - 1) * pitch / 2.0) + i * pitch;
        
        // Positive X leaves
        G4double xp = fMLCParams.blockX / 2 + fMLCParams.leafXPShift[i];
        G4ThreeVector posP(xp, y, fMLCParams.leafZPosition);
        new G4PVPlacement(rotX, posP, leafLogic, 
            ("LeafXP_" + std::to_string(i)).c_str(),
            worldLogical, false, i, true);
        
        // Negative X leaves
        G4double xn = -fMLCParams.blockX / 2 + fMLCParams.leafXNShift[i];
        G4ThreeVector posN(xn, y, fMLCParams.leafZPosition);
        new G4PVPlacement(rotX, posN, leafLogic,
            ("LeafXN_" + std::to_string(i)).c_str(),
            worldLogical, false, i + fMLCParams.numLeaves, true);
    }
    
    // Visualization
    G4VisAttributes* leafVis = new G4VisAttributes(G4Colour::Red());
    leafVis->SetForceSolid(true);
    leafLogic->SetVisAttributes(leafVis);
    
    G4cout << "✓ MLC constructed (" << fMLCParams.numLeaves << " leaves)" << G4endl;
}

//==============================================================================
// Calculate MLC Positions
//==============================================================================
void LinacInfinityGeometry::CalculateMLCPositions()
{
    G4double L = fFieldSize / 2.0;  // Half field size
    
    // With source at Z=0 and isocenter at Z=SSD:
    // Distance from source to MLC
    G4double sourceToMLC = fMLCParams.leafZPosition;
    
    // Aperture by geometric projection
    G4double mlcHalfOpening = L * sourceToMLC / fSSD;
    
    G4double pitch = fMLCParams.leafWidth + fMLCParams.interleafGap;
    G4double yHalfOpening = L * sourceToMLC / fSSD;
    
    fMLCParams.leafXPShift.resize(fMLCParams.numLeaves, 0.0);
    fMLCParams.leafXNShift.resize(fMLCParams.numLeaves, 0.0);
    
    G4int extraLeaves = 1;
    G4int numActiveLeaves = 0;
    
    for (int i = 0; i < fMLCParams.numLeaves; ++i) {
        G4double yLeaf = -((fMLCParams.numLeaves - 1) * pitch / 2.0) + i * pitch;
        
        if (std::abs(yLeaf) <= yHalfOpening + extraLeaves * pitch) {
            numActiveLeaves++;
            fMLCParams.leafXPShift[i] = mlcHalfOpening;
            fMLCParams.leafXNShift[i] = -mlcHalfOpening;
        } else {
            fMLCParams.leafXPShift[i] = 0.0;
            fMLCParams.leafXNShift[i] = 0.0;
        }
    }
    
    G4cout << "  MLC active leaves: " << numActiveLeaves 
           << " / " << fMLCParams.numLeaves << G4endl;
}

//==============================================================================
// Jaws (Secondary Collimators)
//==============================================================================
void LinacInfinityGeometry::ConstructJaws(G4LogicalVolume* worldLogical)
{
    // Jaw parameters
    fJawParams.lengthX = 20.0 * cm;
    fJawParams.widthY = 20.0 * cm;
    fJawParams.heightZ = 7.80 * cm;
    fJawParams.zPosition = 43.2 * cm;
    
    // Calculate jaw positions
    CalculateJawPositions();
    
    // Create jaw boxes
    G4Box* solidJawYP = new G4Box("JawYP", 
        fJawParams.lengthX / 2, fJawParams.widthY / 2, fJawParams.heightZ / 2);
    G4Box* solidJawYN = new G4Box("JawYN",
        fJawParams.lengthX / 2, fJawParams.widthY / 2, fJawParams.heightZ / 2);
    
    G4LogicalVolume* logicJawYP = new G4LogicalVolume(
        solidJawYP, fTungsten, "LogicJawYP");
    G4LogicalVolume* logicJawYN = new G4LogicalVolume(
        solidJawYN, fTungsten, "LogicJawYN");
    
    // Rotations
    G4RotationMatrix* jawRotationP = new G4RotationMatrix();
    G4RotationMatrix* jawRotationN = new G4RotationMatrix();
    jawRotationP->rotateX(-fJawParams.rotationAngle);
    jawRotationN->rotateX(fJawParams.rotationAngle);
    
    // Place jaws
    new G4PVPlacement(jawRotationP,
        G4ThreeVector(0, fJawParams.displacement, fJawParams.zPosition),
        logicJawYP, "PhysJawYP", worldLogical, false, 0, true);
    
    new G4PVPlacement(jawRotationN,
        G4ThreeVector(0, -fJawParams.displacement, fJawParams.zPosition),
        logicJawYN, "PhysJawYN", worldLogical, false, 1, true);
    
    // Visualization
    G4VisAttributes* jawVis = new G4VisAttributes(G4Colour(1.0, 0.5, 0.0));
    jawVis->SetForceSolid(true);
    logicJawYP->SetVisAttributes(jawVis);
    logicJawYN->SetVisAttributes(jawVis);
    
    G4cout << "✓ Jaws constructed (displacement: " 
           << fJawParams.displacement/cm << " cm)" << G4endl;
}

//==============================================================================
// Calculate Jaw Positions
//==============================================================================
void LinacInfinityGeometry::CalculateJawPositions()
{
    G4double L = fFieldSize / 2.0;
    G4double a = fJawParams.heightZ;
    G4double b = fJawParams.widthY;
    
    // With source at Z=0: distance from jaw to isocenter
    G4double z0 = fSSD - fJawParams.zPosition;
    G4double D1 = 0.5 * std::sqrt(a * a + b * b);
    
    const G4double PI = 3.14159265358979323846;
    
    // Rotation angle (inverted for upward beam)
    G4double theta = PI / 2 - std::atan(fSSD / L);
    G4double beta = std::atan(fSSD / L);
    G4double alpha = std::atan(a / b);
    
    fJawParams.displacement = z0 * L / fSSD + 
                              D1 * std::cos(alpha + theta) + 
                              D1 * std::sin(alpha + theta) / std::tan(beta);
    
    // Rotation angle (negated for inverted orientation)
    fJawParams.rotationAngle = -theta * rad;
    
    G4cout << "  Jaw rotation: " << -theta * 180 / PI << " deg" << G4endl;
}

//==============================================================================
// Set Field Size
//==============================================================================
void LinacInfinityGeometry::SetFieldSize(G4double fieldSize)
{
    fFieldSize = fieldSize;
    G4cout << "Field size updated to: " << fFieldSize/cm << " cm" << G4endl;
    G4cout << "Note: Geometry must be reconstructed for changes to take effect." << G4endl;
}
