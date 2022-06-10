/* --------------------------------------------------
   Copyright (C): OpenGATE Collaboration
   This software is distributed under the terms
   of the GNU Lesser General  Public Licence (LGPL)
   See LICENSE.md for further details
   -------------------------------------------------- */

#ifndef GamTrackingAction_h
#define GamTrackingAction_h

#include "G4UserTrackingAction.hh"
#include "G4Track.hh"
#include "GamVActor.h"


#include "QMC/CurrentTrackInformationAction.h"

class GamTrackingAction : public G4UserTrackingAction {

public:

    GamTrackingAction();

    virtual ~GamTrackingAction() {}

    void RegisterActor(GamVActor *actor);

    virtual void PreUserTrackingAction(const G4Track *Track);

    virtual void PostUserTrackingAction(const G4Track *Track);

protected:
    CurrentTrackInformationAction trackInformation; 
    std::vector<GamVActor *> fPreUserTrackingActionActors;
    std::vector<GamVActor *> fPostUserTrackingActionActors;
};

#endif // GamTrackingAction_h
