/* --------------------------------------------------
   Copyright (C): OpenGATE Collaboration
   This software is distributed under the terms
   of the GNU Lesser General  Public Licence (LGPL)
   See LICENSE.md for further details
   -------------------------------------------------- */

#ifndef GateTrackingAction_h
#define GateTrackingAction_h

#include "G4Track.hh"
#include "G4UserTrackingAction.hh"
#include "GateVActor.h"

// @BD
#include "QMC/CurrentTrackInformationAction.h"

class GateTrackingAction : public G4UserTrackingAction {

public:
  GateTrackingAction();

  virtual ~GateTrackingAction() {}

  void RegisterActor(GateVActor *actor);

  virtual void PreUserTrackingAction(const G4Track *Track);

  virtual void PostUserTrackingAction(const G4Track *Track);

protected:
  // @BD TODO: Find better place for this.
  // @BD : But taking the Gate dependancy of GateVActor seems
  // @BD : heavy and the dependancy to py::dict is odd...
  CurrentTrackInformationAction qmcTrackActor;

  std::vector<GateVActor *> fPreUserTrackingActionActors;
  std::vector<GateVActor *> fPostUserTrackingActionActors;
};

#endif // GateTrackingAction_h
