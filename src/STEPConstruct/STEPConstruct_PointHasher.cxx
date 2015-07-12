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


#include <gp_Pnt.hxx>
#include <STEPConstruct_PointHasher.hxx>

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================
Standard_Boolean STEPConstruct_PointHasher::IsEqual(const gp_Pnt& point1, 
                                                    const gp_Pnt& point2)
{
  if(Abs(point1.X()-point2.X()) > Epsilon(point1.X())) return Standard_False;
  if(Abs(point1.Y()-point2.Y()) > Epsilon(point1.Y())) return Standard_False;
  if(Abs(point1.Z()-point2.Z()) > Epsilon(point1.Z())) return Standard_False;
  return Standard_True;
}
