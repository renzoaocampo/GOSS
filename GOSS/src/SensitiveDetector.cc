#include "G4UnitsTable.hh"
#include "SensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"
#include <fstream>
#include <thread>
#include <sstream>
#include <cmath>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

namespace {
  std::string GetThreadID() {
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    return oss.str();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MySensitiveDetector::MySensitiveDetector(const G4String& name,
                     const G4String& hitsCollectionName)
 : G4VSensitiveDetector(name)
{
  collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MySensitiveDetector::Initialize(G4HCofThisEvent*)
{
  // Clear per-event energy map for E^2 calculation
  IndepCuadraticEnergyDepositMap.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool MySensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  G4double edep = aStep->GetTotalEnergyDeposit();
  if (edep == 0.) return false;
 
  G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
  const G4TouchableHandle touchable = preStepPoint->GetTouchableHandle();
  int copyNumber = touchable->GetCopyNumber();
 
  // Accumulate total energy deposited per detector
  energyDepositMap[copyNumber] += edep/eV;
  
  // Accumulate per-event energy for E^2 calculation
  IndepCuadraticEnergyDepositMap[copyNumber] += edep/eV;
  
  // Store detector position (overwrites each step, but position is constant)
  posDetector = touchable->GetTranslation();
  xposition[copyNumber] = posDetector.x()/cm;
  yposition[copyNumber] = posDetector.y()/cm;
  zposition[copyNumber] = posDetector.z()/cm;
  
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MySensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
  // Accumulate E^2 from this event's energy deposits
  for (const auto& pair : IndepCuadraticEnergyDepositMap) {
    cuadraticEnergyDepositMap[pair.first] += std::pow(pair.second, 2);
  }
  
  std::string threadID = GetThreadID();
  threadEventCounters[threadID]++;
  
  // Write output every 100k events
  if (threadEventCounters[threadID] % 100000 == 0) {
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    
    // Delete previous ntuple and recreate (intentional overwrite)
    man->DeleteNtuple(0, true);
    
    std::string fileName = "output.csv";
    man->OpenFile(fileName);
    
    // Create ntuple structure
    man->CreateNtuple("dose", "Dose Scoring Results");
    man->CreateNtupleDColumn("Detector_Number");
    man->CreateNtupleDColumn("x");
    man->CreateNtupleDColumn("y");
    man->CreateNtupleDColumn("z");
    man->CreateNtupleDColumn("Total_Energy_Deposited");
    man->CreateNtupleDColumn("Energy_Per_Particle");
    man->CreateNtupleDColumn("Energy_Squared_Sum");
    man->CreateNtupleDColumn("Quadric_Energy_Per_Particle");
    man->CreateNtupleDColumn("3sigma");
    man->CreateNtupleDColumn("nEventCounterThread");
    man->FinishNtuple(0);
    
    // Fill data for each detector
    const int nEvents = threadEventCounters[threadID];
    for (const auto& pair : energyDepositMap) {
      int copyNumber = pair.first;
      double totalEnergy = pair.second;
      double energyPerParticle = (nEvents > 0) ? totalEnergy / nEvents : 0.0;
      double eventEnergySquared = cuadraticEnergyDepositMap[copyNumber];
      double quadricEnergyPerParticle = (nEvents > 0) ? eventEnergySquared / nEvents : 0.0;
      
      // Standard deviation of the mean: σ_mean = sqrt((⟨E²⟩ - ⟨E⟩²) / N)
      double variance = quadricEnergyPerParticle - std::pow(energyPerParticle, 2);
      double three_sigma = 3.0 * std::sqrt(std::max(0.0, variance) / nEvents);
      
      double x = xposition[copyNumber];
      double y = yposition[copyNumber];
      double z = zposition[copyNumber];
      
      man->FillNtupleDColumn(0, 0, copyNumber);
      man->FillNtupleDColumn(0, 1, x);
      man->FillNtupleDColumn(0, 2, y);
      man->FillNtupleDColumn(0, 3, z);
      man->FillNtupleDColumn(0, 4, totalEnergy);
      man->FillNtupleDColumn(0, 5, energyPerParticle);
      man->FillNtupleDColumn(0, 6, eventEnergySquared);
      man->FillNtupleDColumn(0, 7, quadricEnergyPerParticle);
      man->FillNtupleDColumn(0, 8, three_sigma);
      man->FillNtupleDColumn(0, 9, nEvents);
      man->AddNtupleRow(0);
    }
    
    man->Write();
    man->CloseFile();
    
    // Log progress
    G4cout << "=== Dose Scoring Progress ===" << G4endl;
    G4cout << "  ThreadID: " << threadID << G4endl;
    G4cout << "  Events processed: " << nEvents << G4endl;
    G4cout << "  Detectors with hits: " << energyDepositMap.size() << G4endl;
    G4cout << "=============================" << G4endl;
  }
}
