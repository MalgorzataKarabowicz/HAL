/*
 * TrackV0DecLengthCut.h
 *
 *  Created on: 01-09-2022
 *      Author: Malgorzata Karabowicz
 *		        Warsaw University of Technology, Faculty of Physics
 *		E-mail: karabowiczmalgorzata@gmail.com
 */

#ifndef HALTrackV0DECLENGTHCUT_H_
#define HALTrackV0DECLENGTHCUT_H_

#include "TrackCut.h"

namespace Hal
{
  class TrackV0DecLengthCut : public TrackCut {
  public:
    TrackV0DecLengthCut();
    Bool_t Pass(Track* track);
    virtual ~TrackV0DecLengthCut();
    ClassDef(TrackV0DecLengthCut, 1)
  };
}  // namespace Hal
#endif /* HALTrackV0DECLENGTHCUT_H_ */