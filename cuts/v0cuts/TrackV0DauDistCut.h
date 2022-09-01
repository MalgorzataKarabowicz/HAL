/*
 * TrackV0DauDistCut.h
 *
 *  Created on: 01-09-2022
 *      Author: Malgorzata Karabowicz
 *		        Warsaw University of Technology, Faculty of Physics
 *		E-mail: karabowiczmalgorzata@gmail.com
 */

#ifndef HALTrackV0DAUDISTCUT_H_
#define HALTrackV0DAUDISTCUT_H_

#include "TrackCut.h"

namespace Hal
{
  class TrackV0DauDistCut : public TrackCut {
  public:
    TrackV0DauDistCut();
    Bool_t Pass(Track* track);
    virtual ~TrackV0DauDistCut();
    ClassDef(TrackV0DauDistCut, 1)
  };
}  // namespace Hal
#endif /* HALTrackV0DAUDISTCUT_H_ */