// Created on: 1995-10-20
// Created by: Andre LIEUTIER
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <Plate_PinpointConstraint.hxx>

Plate_PinpointConstraint::Plate_PinpointConstraint()
{
  pnt2d = gp_XY(0,0);
  value = gp_XYZ(0,0,0);
  idu = 0;
  idv = 0;
}


Plate_PinpointConstraint::Plate_PinpointConstraint(const gp_XY& point2d, const gp_XYZ& ImposedValue,
						   const Standard_Integer iu, const Standard_Integer iv)
{
  pnt2d = point2d;
  value = ImposedValue;
  idu = iu;
  idv = iv;
}
