// Copyright (c) 2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef ANDROIDQT_USERINTERACTIONPARAMETERS_H
#define ANDROIDQT_USERINTERACTIONPARAMETERS_H

#include <Quantity_Color.hxx>

namespace AndroidQt_UserInteractionParameters
{
  const double RotationThreshold  = 2;    // [pixel]
  const double PanThreshold       = 4;    // [pixel]
  const double ZoomThreshold      = 6;    // [pixel]
  const double ZoomRatio          = 0.13; // distance ratio
  const Quantity_Color BgColor    = Quantity_Color(0.145, 0.145, 0.145, Quantity_TOC_RGB); // color of viewer's background
}

#endif // USERINTERACTIONPARAMETERS_H
