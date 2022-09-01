/*
 * TrackV0ArmCut.cxx
 *
 *  Created on: 01-09-2022
 *      Author: Malgorzata Karabowicz
 *		        Warsaw University of Technology, Faculty of Physics
 *		E-mail: karabowiczmalgorzata@gmail.com
 */

#include "TrackV0ArmCut.h"

#include "Track.h"

namespace Hal
{
  TrackV0ArmCut::TrackV0ArmCut() : TrackCut(2)
  {
    SetUnitName("#alpha", 0);
    SetUnitName("P_{T} [GeV/c]", 1);
  }

  Bool_t TrackV0ArmCut::Pass(Track* track)
  {
    Double_t alpha = track->GetV0Info()->GetAlphaArm();
    Double_t pt    = track->GetV0Info()->GetPtArm();
    SetValue(alpha, 0);
    SetValue(pt, 1);
    return Validate();
  }

  TrackV0ArmCut::~TrackV0ArmCut() {}
}  // namespace Hal