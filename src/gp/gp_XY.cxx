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


#include <gp_Mat2d.hxx>
#include <gp_XY.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>

Standard_Boolean gp_XY::IsEqual (const gp_XY& Other,
				 const Standard_Real Tolerance) const
{
  Standard_Real val;
  val = x - Other.x;
  if (val < 0) val = - val;
  if (val > Tolerance) return Standard_False;
  val = y - Other.y;
  if (val < 0) val = - val;
  if (val > Tolerance) return Standard_False;
  return Standard_True;
}

