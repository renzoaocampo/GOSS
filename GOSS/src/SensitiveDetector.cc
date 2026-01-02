#include "G4UnitsTable.hh"
#include "SensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"
#include <chrono>
#include <fstream>
#include <thread>
#include <mutex> 
std::string GetThreadID() {
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    return oss.str();
}

MySensitiveDetector::MySensitiveDetector(const G4String& name,
                     const G4String& hitsCollectionName)
 : G4VSensitiveDetector(name), fHitsCollection(nullptr)
{
  collectionName.insert(hitsCollectionName);
}

void MySensitiveDetector::Initialize(G4HCofThisEvent* hce)
{
  fHitsCollection
    = new TrackerHitsCollection(SensitiveDetectorName, collectionName[0]);

  G4int hcID
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection(hcID, fHitsCollection);
  
  IndepCuadraticEnergyDepositMap.clear();
}

G4bool MySensitiveDetector::ProcessHits(G4Step* aStep,
                                     G4TouchableHistory*)
{
  G4double edep = aStep->GetTotalEnergyDeposit();
  if (edep == 0.) return false;
 
  G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
  const G4TouchableHandle touchable = preStepPoint->GetTouchableHandle();
  int copyNumber = touchable->GetCopyNumber();
 
    // Incrementar la energía depositada en el volumen correspondiente
    energyDepositMap[copyNumber] += edep/eV;
     IndepCuadraticEnergyDepositMap[copyNumber] += edep/eV ;
    // Si la partícula es primaria, incrementar el contador
    posDetector = touchable->GetTranslation();
    // Si la partícula es primaria, incrementar el contador
   xposition[copyNumber]=posDetector.x()/cm;
    yposition[copyNumber]=posDetector.y()/cm;
    zposition[copyNumber]=posDetector.z()/cm;
  return true;
}

void MySensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
   static G4double eventCounter = 0;
   eventCounter++;
     for (const auto& pair :  IndepCuadraticEnergyDepositMap) {
              cuadraticEnergyDepositMap[pair.first]+=pow( IndepCuadraticEnergyDepositMap[pair.first],2); 
            }
    std::string threadID = GetThreadID();
  threadEventCounters[threadID]++; 
  
   if ( threadEventCounters[threadID]% 200 == 0)
   { 
    G4AnalysisManager *man = G4AnalysisManager::Instance();
  
      man-> DeleteNtuple(0,true);

        // Crear el nombre del archivo con la semilla en el título
        std::string fileName = "output.csv";

  man->OpenFile(fileName);
    // man->CreateNtuple(("seed");
    man->CreateNtupleDColumn("Detector_Number");
     man->CreateNtupleDColumn("x");
    man->CreateNtupleDColumn("y");
    man->CreateNtupleDColumn("z");
    man->CreateNtupleDColumn("Total_Energy_Deposited"); 
     man->CreateNtupleDColumn(" Energy_Per_Particle"); 
      man->CreateNtupleDColumn("Energy_Squared_Sum"); 
      man->CreateNtupleDColumn("Quadric_Energy_Per_Particle"); 
      man->CreateNtupleDColumn("3sigma"); 
      man->CreateNtupleDColumn("nEventCounterThread");
    man->FinishNtuple(0);
 
        
     
            
            for (const auto& pair : energyDepositMap) {
                int copyNumber = pair.first;
                double totalEnergy = pair.second; 
                double energyPerParticle = (threadEventCounters[threadID] > 0) ? totalEnergy / threadEventCounters[threadID] : 0.0;
                double eventEnergySquared = cuadraticEnergyDepositMap[copyNumber];
                double quadricEnergyPerParticle = (threadEventCounters[threadID] > 0) ? eventEnergySquared / threadEventCounters[threadID] : 0.0;
                double three_sigma = 3 * sqrt((quadricEnergyPerParticle - pow(energyPerParticle, 2)) / threadEventCounters[threadID]);
                  double x= xposition[copyNumber];
                double y= yposition[copyNumber];
                double z= zposition[copyNumber];
                G4AnalysisManager *man=G4AnalysisManager::Instance();  
              man->FillNtupleDColumn(0,0,copyNumber);
                man->FillNtupleDColumn(0,1,x);
                man->FillNtupleDColumn(0,2,y);
                man->FillNtupleDColumn(0,3,z);
                man->FillNtupleDColumn(0,4,totalEnergy);
                man->FillNtupleDColumn(0,5,energyPerParticle);
                man->FillNtupleDColumn(0,6,eventEnergySquared);
                man->FillNtupleDColumn(0,7,quadricEnergyPerParticle);
                man->FillNtupleDColumn(0,8,three_sigma);
                man->FillNtupleDColumn(0,9,threadEventCounters[threadID]);
                man->AddNtupleRow(0);
                 
                 
    }

    man->Write();
    man->CloseFile();

    // Reset accumulated maps for next interval
 std::cout << "Thread Event Counters:" << std::endl;
    for (const auto& pair : threadEventCounters)
    {
      G4cout << "Eventos: "<<eventCounter << G4endl;
      G4cout << "ThreadID: " << pair.first << " - EventCount: " << pair.second << G4endl;
    }
 }
}
