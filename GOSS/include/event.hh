#ifndef EVENT_HH
#define EVENT_HH
#include "G4UserEventAction.hh" 
#include "G4AnalysisManager.hh" 
#include "RunAction.hh"
#include "G4UnitsTable.hh"
#include "SensitiveDetector.hh"
class MyEventAction: public G4UserEventAction{
    public: 
        MyEventAction();
        ~MyEventAction();
        void BeginOfEventAction(const G4Event*);
        void EndOfEventAction(const G4Event*);

     
 
    private:    
};
#endif