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


#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <IntSurf_PathPoint.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_UndefinedDerivative.hxx>

IntSurf_PathPoint::IntSurf_PathPoint ():
      ispass(Standard_True), istgt(Standard_True) 
{
}


IntSurf_PathPoint::IntSurf_PathPoint (const gp_Pnt& P,
				      const Standard_Real U,
				      const Standard_Real V):
       pt(P),ispass(Standard_True),istgt(Standard_True) 
{
  sequv = new TColgp_HSequenceOfXY ();
  sequv->Append(gp_XY(U,V));
}


void IntSurf_PathPoint::SetValue (const gp_Pnt& P,
				  const Standard_Real U,
				  const Standard_Real V) 
{
  pt = P;
  sequv = new TColgp_HSequenceOfXY ();
  sequv->Append(gp_XY(U,V));
}
  
