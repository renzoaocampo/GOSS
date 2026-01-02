#include "TrackingAction.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"

TrackingAction::TrackingAction() : G4UserTrackingAction() {}

TrackingAction::~TrackingAction() {}

void TrackingAction::PreUserTrackingAction(const G4Track* track) {
     
}

void TrackingAction::PostUserTrackingAction(const G4Track* track) {
  
}
