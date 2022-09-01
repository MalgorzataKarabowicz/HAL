/*
 * TrackV0DecLengthCut.cxx
 *
 *  Created on: 01-09-2022
 *      Author: Malgorzata Karabowicz
 *		        Warsaw University of Technology, Faculty of Physics
 *		E-mail: karabowiczmalgorzata@gmail.com
 */

#include "TrackV0DecLengthCut.h"

#include "Track.h"

namespace Hal
{
  TrackV0DecLengthCut::TrackV0DecLengthCut() : TrackCut(1) { SetUnitName("l [cm]"); }

  Bool_t TrackV0DecLengthCut::Pass(Track* track)
  {
    Double_t l = track->GetV0Info()->GetDecLength();
    SetValue(l);
    return Validate();
  }

  TrackV0DecLengthCut::~TrackV0DecLengthCut() {}
}  // namespace Hal