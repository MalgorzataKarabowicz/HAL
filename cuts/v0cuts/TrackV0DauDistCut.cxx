/*
 * TrackV0DauDistCut.cxx
 *
 *  Created on: 01-09-2022
 *      Author: Malgorzata Karabowicz
 *		        Warsaw University of Technology, Faculty of Physics
 *		E-mail: karabowiczmalgorzata@gmail.com
 */

#include "TrackV0DauDistCut.h"

#include "Track.h"

namespace Hal
{
  TrackV0DauDistCut::TrackV0DauDistCut() : TrackCut(1) { SetUnitName("distance [cm]"); }

  Bool_t TrackV0DauDistCut::Pass(Track* track)
  {
    Double_t distance = track->GetV0Info()->GetDauDist();
    SetValue(distance);
    return Validate();
  }

  TrackV0DauDistCut::~TrackV0DauDistCut() {}
}  // namespace Hal