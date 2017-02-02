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


#include <Plate_LinearXYZConstraint.hxx>
#include <Plate_PinpointConstraint.hxx>
#include <Plate_SampledCurveConstraint.hxx>
#include <Plate_SequenceOfPinpointConstraint.hxx>
#include <Standard_DimensionMismatch.hxx>

static inline Standard_Real B0( Standard_Real t)
{
  Standard_Real s = t;
  if(s<0.) s = -s;
  s = 1. - s;
  if(s<0.) s = 0;
  return s;
}

Plate_SampledCurveConstraint::Plate_SampledCurveConstraint(const Plate_SequenceOfPinpointConstraint &SOPPC,
							   const Standard_Integer n)
:myLXYZC(n,SOPPC.Length())
{
  Standard_Integer m = SOPPC.Length();

  if (n > m)  throw Standard_DimensionMismatch();
  for(Standard_Integer index =1; index <= m;index++)
    myLXYZC.SetPPC(index,SOPPC(index));

  Standard_Real ratio = Standard_Real(n+1) /Standard_Real(m+1); 
  for (Standard_Integer i=1;i<=n;i++)
    for (Standard_Integer j=1;j<=m;j++)
      {
	myLXYZC.SetCoeff(i,j,B0(ratio*j - i));
      }
}
