/*
 * TrackV0PdgCut.cxx
 *
 *  Created on: 01-09-2022
 *      Author: Malgorzata Karabowicz
 *		        Warsaw University of Technology, Faculty of Physics
 *		E-mail: karabowiczmalgorzata@gmail.com
 */

#include "TrackV0PdgCut.h"

#include "Track.h"

namespace Hal
{
  TrackV0PdgCut::TrackV0PdgCut() : TrackCut(1) { SetUnitName("PDG"); }

  Bool_t TrackV0PdgCut::Pass(Track* track)
  {
    Double_t pdg = track->GetV0Info()->GetPdg();
    SetValue(pdg);
    return Validate();
  }

  TrackV0PdgCut::~TrackV0PdgCut() {}
}  // namespace Hal