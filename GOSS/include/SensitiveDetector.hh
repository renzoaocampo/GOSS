#ifndef SENSITIVEDETECTOR_HH
#define SENSITIVEDETECTOR_HH
 
#include "G4VSensitiveDetector.hh"
#include "G4THitsCollection.hh"
#include <map>
#include <unordered_map>
#include <vector>

class G4Step;
class G4HCofThisEvent;

 
/// Sensitive detector class for dose scoring
///
/// Accumulates energy deposits per detector volume and periodically
/// writes results to CSV file with statistical analysis.

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
    // Energy accumulation maps (thread-local by Geant4 design)
    std::map<int, G4double> energyDepositMap;           // Total energy per detector
    std::map<int, G4double> cuadraticEnergyDepositMap;  // Sum of E^2 for variance
    std::map<int, G4double> IndepCuadraticEnergyDepositMap;  // Per-event energy for E^2
    
    // Position maps
    std::map<int, G4double> xposition;
    std::map<int, G4double> yposition;
    std::map<int, G4double> zposition;
    G4ThreeVector posDetector;
    
    // Thread event counter
    std::unordered_map<std::string, int> threadEventCounters;
};
 
 
#endif