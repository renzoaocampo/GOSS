#ifndef TRACKINGACTION_HH
#define TRACKINGACTION_HH

#include "G4UserTrackingAction.hh"
#include "G4Track.hh"
#include "globals.hh"

class TrackingAction : public G4UserTrackingAction {
public:
    TrackingAction();
    virtual ~TrackingAction();

    // Se ejecuta al inicio de la trayectoria de una partícula
    virtual void PreUserTrackingAction(const G4Track* track);

    // Se ejecuta al final de la trayectoria de una partícula
    virtual void PostUserTrackingAction(const G4Track* track);
};

#endif
