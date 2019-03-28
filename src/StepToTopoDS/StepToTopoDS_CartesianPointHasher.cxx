// Created on: 1993-08-30
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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


#include <StepGeom_CartesianPoint.hxx>
#include <StepToTopoDS_CartesianPointHasher.hxx>

//=======================================================================
// function : HashCode
// purpose  :
//=======================================================================
Standard_Integer StepToTopoDS_CartesianPointHasher::HashCode (const Handle (StepGeom_CartesianPoint)
                                                                & theCartesianPoint,
                                                              const Standard_Integer theUpperBound)
{
  return ::HashCode (theCartesianPoint, theUpperBound);
}

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================

Standard_Boolean StepToTopoDS_CartesianPointHasher::IsEqual
  (const Handle(StepGeom_CartesianPoint)& K1,
   const Handle(StepGeom_CartesianPoint)& K2)
{
  return (K1 == K2);
}
