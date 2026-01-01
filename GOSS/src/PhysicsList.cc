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
//
//----------------------------------------------------------------------------//
// This physics list *must* be set with a *reference* physics list.
//   These provide a full set of models (both electromagnetic and hadronic).
//
//   The reference physics list must be set by issuing its command in a macro
//   file (see messenger class). No other action is required.
//   Examples of physics list names: QGSP_BIC_HP_EMZ or QGSP_BERT_HP
//----------------------------------------------------------------------------//
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


#include "PhysicsList.hh"
#include "PhysicsListMessenger.hh"

#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4VPhysicsConstructor.hh"
#include "G4VModularPhysicsList.hh"

// Particle constructors
#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

// Electromagnetic physics lists
#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"

PhysicsList::PhysicsList(): G4VModularPhysicsList()
{
    SetDefaultCutValue(1.0*mm);
    fPhysListIsSet = false;
    fVerbose = 1;
    fMessenger = new PhysicsListMessenger(this);
}

PhysicsList::~PhysicsList()
{
    delete fMessenger;
}

void PhysicsList::ConstructParticle()
{
    if(fVerbose > 0) G4cout << "### PhysicsList Construct Particles" << G4endl;

    G4BosonConstructor  pBosonConstructor;  pBosonConstructor.ConstructParticle();
    G4LeptonConstructor pLeptonConstructor; pLeptonConstructor.ConstructParticle();
    G4MesonConstructor  pMesonConstructor;  pMesonConstructor.ConstructParticle();
    G4BaryonConstructor pBaryonConstructor; pBaryonConstructor.ConstructParticle();
    G4IonConstructor    pIonConstructor;    pIonConstructor.ConstructParticle();
    G4ShortLivedConstructor pShortLivedConstructor; pShortLivedConstructor.ConstructParticle();
}

void PhysicsList::ConstructProcess()
{
    if(fVerbose > 0) G4cout << "### PhysicsList Construct Processes" << G4endl;

    if (fPhysListIsSet)
        G4VModularPhysicsList::ConstructProcess();
    else
        G4Exception("PhysicsList::ConstructProcess()", "PhysList001",
                    FatalException, "No PHYSICS LIST has been set!");
}

void PhysicsList::SetPhysicsList(const G4String& name)
{
    if(fVerbose > 0)
        G4cout << "### PhysicsList set physics list <" << name << "> " << G4endl;

    if (!fPhysListIsSet)
    {
        G4VPhysicsConstructor* emPhysics = nullptr;

        if (name == "EMStandardPhysics")          emPhysics = new G4EmStandardPhysics();
        else if (name == "EMStandardPhysics_option1") emPhysics = new G4EmStandardPhysics_option1();
        else if (name == "EMStandardPhysics_option2") emPhysics = new G4EmStandardPhysics_option2();
        else if (name == "EMStandardPhysics_option3") emPhysics = new G4EmStandardPhysics_option3();
        else if (name == "EMStandardPhysics_option4") emPhysics = new G4EmStandardPhysics_option4();
        else if (name == "EMLivermore")           emPhysics = new G4EmLivermorePhysics();
        else if (name == "EMPenelope")            emPhysics = new G4EmPenelopePhysics();
        else
        {
            G4Exception("PhysicsList::SetPhysicsList()", "PhysList001",
                        JustWarning, "Unknown EM physics list, defaulting to EMStandardPhysics");
            emPhysics = new G4EmStandardPhysics();
        }

        RegisterPhysics(emPhysics);
        fPhysListIsSet = true;

        G4cout << name << " EM physics list has been ACTIVATED." << G4endl;
    }
}