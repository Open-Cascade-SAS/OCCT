// Created on: 1998-05-18
// Created by: Andre LIEUTIER
// Copyright (c) 1998-1999 Matra Datavision
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


#include <gp_Lin.hxx>
#include <gp_XY.hxx>
#include <Plate_LinearScalarConstraint.hxx>
#include <Plate_LineConstraint.hxx>

Plate_LineConstraint::Plate_LineConstraint(const gp_XY& point2d,
					     const gp_Lin& lin,
					     const Standard_Integer iu,
					     const Standard_Integer iv)
:myLSC(2,1)
{
  gp_XYZ point = lin.Location().XYZ();
  myLSC.SetPPC(1,Plate_PinpointConstraint(point2d,point,iu,iv));

  gp_XYZ dir = lin.Direction().XYZ();
  // one builds two directions orthogonal to dir
  gp_XYZ dX(1,0,0);
  gp_XYZ dY(0,1,0);

  gp_XYZ d1 = dX ^ dir;
  gp_XYZ d2 = dY ^ dir;
  if(d2.SquareModulus() > d1.SquareModulus()) d1 = d2;
  d1.Normalize();
  d2 = dir ^ d1;
  d2.Normalize();
  myLSC.SetCoeff(1,1,d1);
  myLSC.SetCoeff(2,1,d2);
}
