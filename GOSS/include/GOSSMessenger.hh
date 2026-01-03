#ifndef GOSSMessenger_h
#define GOSSMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWithAString;
class G4UIcmdWithABool;

/// Messenger class for GOSS dose scoring configuration
/// Provides UI commands for:
/// - /goss/saveInterval <N>     : Set save interval in events
/// - /goss/outputFile <name>    : Set output file name (without extension)
/// - /goss/seed <value>         : Set random seed
/// - /goss/mergeCSV <true/false>: Enable/disable automatic CSV merge at end of run

class GOSSMessenger : public G4UImessenger
{
public:
  GOSSMessenger();
  virtual ~GOSSMessenger() override;
  
  void SetNewValue(G4UIcommand* command, G4String newValue) override;
  
  // Static getters
  static G4int GetSaveInterval() { return fSaveInterval; }
  static G4String GetOutputFileName() { return fOutputFileName; }
  static G4long GetSeed() { return fSeed; }
  static G4bool IsSeedSet() { return fSeedSet; }
  static G4bool IsMergeEnabled() { return fMergeEnabled; }

private:
  G4UIdirectory* fGOSSDir;
  G4UIcmdWithAnInteger* fSaveIntervalCmd;
  G4UIcmdWithAString* fOutputFileCmd;
  G4UIcmdWithAnInteger* fSeedCmd;
  G4UIcmdWithABool* fMergeCmd;
  
  // Static configuration values
  static G4int fSaveInterval;
  static G4String fOutputFileName;
  static G4long fSeed;
  static G4bool fSeedSet;
  static G4bool fMergeEnabled;
};

#endif
