/*
 * TrackV0ArmCut.h
 *
 *  Created on: 01-09-2022
 *      Author: Malgorzata Karabowicz
 *		        Warsaw University of Technology, Faculty of Physics
 *		E-mail: karabowiczmalgorzata@gmail.com
 */

#ifndef HALTRACKV0ARMCUT_H_
#define HALTRACKV0ARMCUT_H_

#include "TrackCut.h"

namespace Hal
{
  class TrackV0ArmCut : public TrackCut {
  public:
    TrackV0ArmCut();
    Bool_t Pass(Track* track);
    virtual ~TrackV0ArmCut();
    ClassDef(TrackV0ArmCut, 1)
  };
}  // namespace Hal
#endif /* HALTRACKV0ARMCUT_H_ */