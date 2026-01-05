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

#include "globals.hh"
#include "Randomize.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"
#include "GOSSMessenger.hh"
#include "GOSSMerger.hh"

#include <string>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc, char** argv) {

  // Check for --merge command
  if (argc >= 2 && std::string(argv[1]) == "--merge") {
    G4cout << "\n========================================" << G4endl;
    G4cout << "  GOSS CSV Merger (Standalone Mode)" << G4endl;
    G4cout << "========================================\n" << G4endl;
    
    std::string directory = (argc >= 3) ? argv[2] : ".";
    GOSSMerger::MergeThreadOutputs(directory);
    
    G4cout << "\nMerge complete!" << G4endl;
    return 0;
  }

  // Show help if no arguments
  if (argc == 1) {
    G4cout << "\n========================================" << G4endl;
    G4cout << "  GOSS - Geant4 Dose Scoring System" << G4endl;
    G4cout << "========================================\n" << G4endl;
    G4cout << "Usage:" << G4endl;
    G4cout << "  ./goss <macro_file>     Run simulation with macro" << G4endl;
    G4cout << "  ./goss --merge [dir]    Merge CSV files from threads\n" << G4endl;
    G4cout << "Examples:" << G4endl;
    G4cout << "  ./goss macros/my_simulation.mac" << G4endl;
    G4cout << "  ./goss --merge" << G4endl;
    G4cout << "  ./goss --merge ../results" << G4endl;
    G4cout << "\n========================================\n" << G4endl;
    return 0;
  }

  // Choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  // Construct the run manager
#ifdef G4MULTITHREADED
  auto runManager = new G4MTRunManager;
#else
  auto runManager = new G4RunManager;
#endif

  // Set mandatory initialization classes
  runManager->SetUserInitialization(new DetectorConstruction());
  runManager->SetUserInitialization(new PhysicsList());
  runManager->SetUserInitialization(new ActionInitialization());
  
  // GOSS Messenger for dose scoring configuration
  GOSSMessenger* gossMessenger = new GOSSMessenger();

  // UI and Visualization - ALWAYS initialize
  G4UIExecutive* ui = new G4UIExecutive(argc, argv);
  G4VisManager* visManager = new G4VisExecutive();
  visManager->Initialize();
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Execute macro if provided
  if (argc > 1) {
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command + fileName);
  }

  // Start UI session (allows visualization commands in macro)
  ui->SessionStart();

  // Job termination
  delete ui;
  delete visManager;
  delete gossMessenger;
  delete runManager;
  
  return 0;
}
