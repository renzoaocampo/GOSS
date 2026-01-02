#include "G4UnitsTable.hh"
#include "SensitiveDetector.hh"
#include "GOSSMessenger.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"
#include "G4LogicalVolume.hh"
#include <cmath>
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MySensitiveDetector::MySensitiveDetector(const G4String& name,
                     const G4String& hitsCollectionName)
 : G4VSensitiveDetector(name),
   fDetectorMass(0.0),
   fMassInitialized(false),
   fEventCounter(0)
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
  
  // Get detector mass ONCE (same for all detectors sharing logical volume)
  if (!fMassInitialized) {
    G4LogicalVolume* logVol = touchable->GetVolume()->GetLogicalVolume();
    fDetectorMass = logVol->GetMass() / kg;  // Store in kg for Gy calculation
    fMassInitialized = true;
    G4cout << "=== Detector Mass Initialized ===" << G4endl;
    G4cout << "  Mass: " << fDetectorMass * 1e6 << " mg" << G4endl;
    G4cout << "  Material: " << logVol->GetMaterial()->GetName() << G4endl;
    G4cout << "=================================" << G4endl;
  }
  
  // Store detector position ONCE per detector (position is constant)
  if (xposition.find(copyNumber) == xposition.end()) {
    G4ThreeVector pos = touchable->GetTranslation();
    xposition[copyNumber] = pos.x() / cm;
    yposition[copyNumber] = pos.y() / cm;
    zposition[copyNumber] = pos.z() / cm;
  }
 
  // Accumulate total energy deposited per detector (in Joules for Gy)
  G4double edep_J = edep / joule;
  energyDepositMap[copyNumber] += edep_J;
  
  // Accumulate per-event energy for E^2 calculation
  IndepCuadraticEnergyDepositMap[copyNumber] += edep_J;
  
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MySensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
  // Accumulate E^2 from this event's energy deposits
  for (const auto& pair : IndepCuadraticEnergyDepositMap) {
    cuadraticEnergyDepositMap[pair.first] += std::pow(pair.second, 2);
  }
  
  fEventCounter++;
  
  // Write output at configured interval
  G4int saveInterval = GOSSMessenger::GetSaveInterval();
  if (fEventCounter % saveInterval == 0) {
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    
    // Delete previous ntuple and recreate (intentional overwrite)
    man->DeleteNtuple(0, true);
    
    std::string fileName = GOSSMessenger::GetOutputFileName() + ".csv";
    man->OpenFile(fileName);
    
    // Create ntuple structure - DOSE in Gy
    man->CreateNtuple("dose", "Dose Scoring Results");
    man->CreateNtupleDColumn("Detector_Number");
    man->CreateNtupleDColumn("x_cm");
    man->CreateNtupleDColumn("y_cm");
    man->CreateNtupleDColumn("z_cm");
    man->CreateNtupleDColumn("Total_Dose_Gy");
    man->CreateNtupleDColumn("Dose_Per_Particle_Gy");
    man->CreateNtupleDColumn("Dose_Squared_Sum");
    man->CreateNtupleDColumn("Mean_Dose_Squared_Gy2");
    man->CreateNtupleDColumn("Uncertainty_3sigma_Gy");
    man->CreateNtupleDColumn("nEvents");
    man->FinishNtuple(0);
    
    // Fill data for each detector
    const int nEvents = fEventCounter;
    double maxDose = 0.0;
    double maxDosePerParticle = 0.0;
    double maxDose3sigma = 0.0;
    
    for (const auto& pair : energyDepositMap) {
      int copyNumber = pair.first;
      
      // Energy in Joules -> Dose in Gy (J/kg)
      double totalEnergy_J = pair.second;
      double totalDose_Gy = totalEnergy_J / fDetectorMass;
      double dosePerParticle_Gy = (nEvents > 0) ? totalDose_Gy / nEvents : 0.0;
      
      // Variance calculation for dose
      double energySquaredSum = cuadraticEnergyDepositMap[copyNumber];
      double doseSquaredSum = energySquaredSum / (fDetectorMass * fDetectorMass);
      double meanDoseSquared = (nEvents > 0) ? doseSquaredSum / nEvents : 0.0;
      
      // Standard deviation of the mean: σ_mean = sqrt((⟨D²⟩ - ⟨D⟩²) / N)
      double variance = meanDoseSquared - std::pow(dosePerParticle_Gy, 2);
      double three_sigma_Gy = 3.0 * std::sqrt(std::max(0.0, variance) / nEvents);
      
      // Track max dose and its statistics
      if (totalDose_Gy > maxDose) {
        maxDose = totalDose_Gy;
        maxDosePerParticle = dosePerParticle_Gy;
        maxDose3sigma = three_sigma_Gy;
      }
      
      man->FillNtupleDColumn(0, 0, copyNumber);
      man->FillNtupleDColumn(0, 1, xposition[copyNumber]);
      man->FillNtupleDColumn(0, 2, yposition[copyNumber]);
      man->FillNtupleDColumn(0, 3, zposition[copyNumber]);
      man->FillNtupleDColumn(0, 4, totalDose_Gy);
      man->FillNtupleDColumn(0, 5, dosePerParticle_Gy);
      man->FillNtupleDColumn(0, 6, doseSquaredSum);
      man->FillNtupleDColumn(0, 7, meanDoseSquared);
      man->FillNtupleDColumn(0, 8, three_sigma_Gy);
      man->FillNtupleDColumn(0, 9, nEvents);
      man->AddNtupleRow(0);
    }
    
    man->Write();
    man->CloseFile();
    
    // Calculate relative error percentage for max dose
    double relativeError = (maxDosePerParticle > 0) ? (maxDose3sigma / maxDosePerParticle) * 100.0 : 0.0;
    
    // Compact, informative progress log (single output for MT safety)
    std::ostringstream log;
    log << "\n+--------------------------------------------------------------------+\n"
        << "|  GOSS  |  " << std::scientific << std::setprecision(0) << (double)nEvents << " events  |  " 
        << std::fixed << energyDepositMap.size() << " detectors  |  output.csv  |\n"
        << "+--------------------------------------------------------------------+\n"
        << "|  Dose/particle: " << std::scientific << std::setprecision(2) << maxDosePerParticle << " Gy"
        << "  |  Error: " << std::fixed << std::setprecision(2) << relativeError << "%  |\n"
        << "+--------------------------------------------------------------------+\n";
    G4cout << log.str() << G4endl;
  }
}

