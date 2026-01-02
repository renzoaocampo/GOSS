#ifndef GOSSMessenger_h
#define GOSSMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWithAString;

/// Messenger class for GOSS dose scoring configuration
/// Provides UI commands for:
/// - /goss/saveInterval <N>     : Set save interval in events
/// - /goss/outputFile <name>    : Set output file name (without extension)
/// - /goss/seed <value>         : Set random seed (-1 for random)

class GOSSMessenger : public G4UImessenger
{
public:
  GOSSMessenger();
  virtual ~GOSSMessenger() override;
  
  void SetNewValue(G4UIcommand* command, G4String newValue) override;
  
  // Static getters for use by SensitiveDetector
  static G4int GetSaveInterval() { return fSaveInterval; }
  static G4String GetOutputFileName() { return fOutputFileName; }
  static G4long GetSeed() { return fSeed; }
  static G4bool IsSeedSet() { return fSeedSet; }

private:
  G4UIdirectory* fGOSSDir;
  G4UIcmdWithAnInteger* fSaveIntervalCmd;
  G4UIcmdWithAString* fOutputFileCmd;
  G4UIcmdWithAnInteger* fSeedCmd;
  
  // Static configuration values
  static G4int fSaveInterval;
  static G4String fOutputFileName;
  static G4long fSeed;
  static G4bool fSeedSet;
};

#endif
