#ifndef SENSITIVEDETECTOR_HH
#define SENSITIVEDETECTOR_HH
 
#include "G4VSensitiveDetector.hh"

#include "TrackerHit.hh"
#include <map>
#include <vector>

class G4Step;
class G4HCofThisEvent;

 
/// Tracker sensitive detector class
///
/// The hits are accounted in hits in ProcessHits() function which is called
/// by Geant4 kernel at each step. A hit is created with each step with non zero
/// energy deposit.

class MySensitiveDetector : public G4VSensitiveDetector
{
  public:
    MySensitiveDetector(const G4String& name,
                const G4String& hitsCollectionName);
    ~MySensitiveDetector() override = default;

    // methods from base class
    void   Initialize(G4HCofThisEvent* hitCollection) override;
    G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
    void   EndOfEvent(G4HCofThisEvent* hitCollection) override;

  private:
    TrackerHitsCollection* fHitsCollection;
           std::map<int, G4double> energyDepositMap;
           std::map<int, G4double> cuadraticEnergyDepositMap;
           std::map<int, G4double> IndepCuadraticEnergyDepositMap;
    std::map<int, G4int> primaryParticleCountMap;
    std::unordered_map<std::string, int> threadEventCounters;
        std::map<int, G4double> xposition;
     std::map<int, G4double> yposition;
     std::map<int, G4double> zposition;
    G4ThreeVector posDetector;
};
 
 
#endif