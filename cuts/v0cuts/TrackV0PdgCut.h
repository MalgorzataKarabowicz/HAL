/*
 * TrackV0PdgCut.h
 *
 *  Created on: 01-09-2022
 *      Author: Malgorzata Karabowicz
 *		        Warsaw University of Technology, Faculty of Physics
 *		E-mail: karabowiczmalgorzata@gmail.com
 */

#ifndef HALTrackV0PDGCUT_H_
#define HALTrackV0PDGCUT_H_

#include "TrackCut.h"

namespace Hal
{
  class TrackV0PdgCut : public TrackCut {
  public:
    TrackV0PdgCut();
    Bool_t Pass(Track* track);
    virtual ~TrackV0PdgCut();
    ClassDef(TrackV0PdgCut, 1)
  };
}  // namespace Hal
#endif /* HALTrackV0PDGCUT_H_ */