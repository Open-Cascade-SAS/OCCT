// Created on: 1993-08-06
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
#include <StepToTopoDS_PointPair.hxx>
#include <StepToTopoDS_PointPairHasher.hxx>

//=======================================================================
//function : HashCode
//purpose  : 
//=======================================================================
Standard_Integer StepToTopoDS_PointPairHasher::HashCode
  (const StepToTopoDS_PointPair& P, const Standard_Integer Upper)
{
  return (::HashCode(P.myP1,Upper) + ::HashCode(P.myP2,Upper)) % Upper;
}

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================

Standard_Boolean StepToTopoDS_PointPairHasher::IsEqual
  (const StepToTopoDS_PointPair& P1,
   const StepToTopoDS_PointPair& P2)
{
  return (((P1.myP1 == P2.myP1) && (P1.myP2 == P2.myP2)) ||
	  ((P1.myP1 == P2.myP2) && (P1.myP2 == P2.myP1)));
}
