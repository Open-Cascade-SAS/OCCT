// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <PBRep_GCurve.ixx>


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

PBRep_GCurve::PBRep_GCurve(const PTopLoc_Location& L, 
			   const Standard_Real First, 
			   const Standard_Real Last) :
			   PBRep_CurveRepresentation(L),
			   myFirst(First),
			   myLast(Last)
{
}

//=======================================================================
//function : First
//purpose  : 
//=======================================================================

Standard_Real  PBRep_GCurve::First()const 
{
  return myFirst;
}


//=======================================================================
//function : First
//purpose  : 
//=======================================================================

void  PBRep_GCurve::First(const Standard_Real F)
{
  myFirst = F;
}


//=======================================================================
//function : Last
//purpose  : 
//=======================================================================

Standard_Real  PBRep_GCurve::Last()const 
{
  return myLast;
}


//=======================================================================
//function : Last
//purpose  : 
//=======================================================================

void  PBRep_GCurve::Last(const Standard_Real L)
{
  myLast = L;
}


//=======================================================================
//function : IsGCurve
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_GCurve::IsGCurve() const 
{
  return Standard_True;
}

