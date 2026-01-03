#include "GOSSMessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "Randomize.hh"
#include <chrono>
#include <cstdlib>

// Static member initialization
G4int GOSSMessenger::fSaveInterval = 1000000;  // Default: 1M events
G4String GOSSMessenger::fOutputFileName = "output";  // Default name
G4long GOSSMessenger::fSeed = 0;
G4bool GOSSMessenger::fSeedSet = false;
G4bool GOSSMessenger::fMergeEnabled = true;  // Default: merge enabled

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GOSSMessenger::GOSSMessenger()
{
  fGOSSDir = new G4UIdirectory("/goss/");
  fGOSSDir->SetGuidance("GOSS dose scoring configuration.");

  // Save interval command
  fSaveIntervalCmd = new G4UIcmdWithAnInteger("/goss/saveInterval", this);
  fSaveIntervalCmd->SetGuidance("Set the save interval for dose output (number of events).");
  fSaveIntervalCmd->SetGuidance("Default: 1000000 (1M events)");
  fSaveIntervalCmd->SetParameterName("interval", false);
  fSaveIntervalCmd->SetRange("interval>0");
  fSaveIntervalCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  // Output file name command
  fOutputFileCmd = new G4UIcmdWithAString("/goss/outputFile", this);
  fOutputFileCmd->SetGuidance("Set the output file name (without .csv extension).");
  fOutputFileCmd->SetGuidance("Default: 'output' -> creates 'output.csv'");
  fOutputFileCmd->SetParameterName("filename", false);
  fOutputFileCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  // Seed command
  fSeedCmd = new G4UIcmdWithAnInteger("/goss/seed", this);
  fSeedCmd->SetGuidance("Set the random seed for the simulation.");
  fSeedCmd->SetGuidance("If not set, a random seed based on system time will be used.");
  fSeedCmd->SetGuidance("Use any integer value.");
  fSeedCmd->SetParameterName("seed", false);
  fSeedCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  // Merge CSV command
  fMergeCmd = new G4UIcmdWithABool("/goss/mergeCSV", this);
  fMergeCmd->SetGuidance("Enable/disable automatic CSV merge at end of run.");
  fMergeCmd->SetGuidance("When enabled, thread CSV files are merged into goss_sd_dose_merged.csv");
  fMergeCmd->SetGuidance("Default: true");
  fMergeCmd->SetParameterName("enable", false);
  fMergeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  // Initialize with automatic random seed (can be overridden by /goss/seed)
  auto now = std::chrono::high_resolution_clock::now();
  auto rawSeed = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
  fSeed = std::abs(rawSeed % 1000000000);  // Limit to 9 digits, always positive
  G4Random::setTheSeed(fSeed);
  G4cout << "GOSS: Auto seed = " << fSeed << " (use /goss/seed to override)" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GOSSMessenger::~GOSSMessenger()
{
  delete fGOSSDir;
  delete fSaveIntervalCmd;
  delete fOutputFileCmd;
  delete fSeedCmd;
  delete fMergeCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GOSSMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fSaveIntervalCmd) {
    fSaveInterval = fSaveIntervalCmd->GetNewIntValue(newValue);
    G4cout << "GOSS: Save interval set to " << fSaveInterval << " events" << G4endl;
  }
  else if (command == fOutputFileCmd) {
    fOutputFileName = newValue;
    G4cout << "GOSS: Output file set to " << fOutputFileName << ".csv" << G4endl;
  }
  else if (command == fSeedCmd) {
    fSeed = fSeedCmd->GetNewIntValue(newValue);
    fSeedSet = true;
    G4Random::setTheSeed(fSeed);
    G4cout << "GOSS: Random seed set to " << fSeed << G4endl;
  }
  else if (command == fMergeCmd) {
    fMergeEnabled = fMergeCmd->GetNewBoolValue(newValue);
    G4cout << "GOSS: CSV merge " << (fMergeEnabled ? "ENABLED" : "DISABLED") << G4endl;
  }
}
