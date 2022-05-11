/*
 * EventVirtualCut.cxx
 *
 *  Created on: 07-08-2013
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */

#include "EventVirtualCut.h"

namespace Hal {
  EventVirtualCut::EventVirtualCut() : EventCut(1) { SetUnitName("Virtual Unit [VU]"); }

  EventVirtualCut::~EventVirtualCut() {}
}  // namespace Hal
