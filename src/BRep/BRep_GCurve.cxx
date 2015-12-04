// Created on: 1995-03-15
// Created by: Laurent PAINNOT
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


#include <BRep_GCurve.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRep_GCurve,BRep_CurveRepresentation)

//=======================================================================
//function : BRep_GCurve
//purpose  : 
//=======================================================================
BRep_GCurve::BRep_GCurve(const TopLoc_Location& L, 
			 const Standard_Real First, 
			 const Standard_Real Last):
			 BRep_CurveRepresentation(L),
			 myFirst(First),
			 myLast(Last)
			 
{
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void BRep_GCurve::Update()
{
  
}

