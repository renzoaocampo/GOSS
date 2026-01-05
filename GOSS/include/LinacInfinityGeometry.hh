#ifndef LinacInfinityGeometry_h
#define LinacInfinityGeometry_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4SystemOfUnits.hh"
#include <vector>

/// LINAC Infinity Geometry
/// Complete geometry definition for Infinity LINAC
/// Self-contained and easily portable to other Geant4 projects
class LinacInfinityGeometry
{
public:
    LinacInfinityGeometry();
    ~LinacInfinityGeometry();

    /// Construct the complete LINAC geometry
    /// @param worldLogical The world logical volume to place components in
    /// @param fieldSize Field size at isocenter (cm)
    void ConstructLinac(G4LogicalVolume* worldLogical, G4double fieldSize = 15.0 * cm);

    /// Get SSD value
    G4double GetSSD() const { return fSSD; }

    /// Set field size and update collimation
    void SetFieldSize(G4double fieldSize);

private:
    // Construction methods for each component
    void DefineMaterials();
    void ConstructTarget(G4LogicalVolume* worldLogical);
    void ConstructFlatteningFilter(G4LogicalVolume* worldLogical);
    void ConstructPrimaryCollimator(G4LogicalVolume* worldLogical);
    void ConstructShielding(G4LogicalVolume* worldLogical);
    void ConstructMLC(G4LogicalVolume* worldLogical);
    void ConstructJaws(G4LogicalVolume* worldLogical);

    // Helper methods
    void CalculateMLCPositions();
    void CalculateJawPositions();

    // Materials
    G4Material* fWreAlloy;
    G4Material* fAluminum;
    G4Material* fStainlessSteel;
    G4Material* fTungsten;
    G4Material* fShieldingMaterial;

    // Geometry parameters
    G4double fSSD;
    G4double fFieldSize;

    // MLC parameters
    struct MLCParameters {
        G4double leafWidth;
        G4double leafHeight;
        G4double leafZPosition;
        G4int numLeaves;
        G4double interleafGap;
        G4double blockX, blockY, blockZ;
        G4double diskRadius, diskHeight;
        std::vector<G4double> leafXPShift;
        std::vector<G4double> leafXNShift;
    } fMLCParams;

    // Jaw parameters
    struct JawParameters {
        G4double lengthX;
        G4double widthY;
        G4double heightZ;
        G4double zPosition;
        G4double displacement;
        G4double rotationAngle;
    } fJawParams;
};

#endif
