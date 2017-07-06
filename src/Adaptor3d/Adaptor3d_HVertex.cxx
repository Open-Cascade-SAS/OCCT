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


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HVertex.hxx>
#include <ElCLib.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Adaptor3d_HVertex,Standard_Transient)

Adaptor3d_HVertex::Adaptor3d_HVertex ()
{}


Adaptor3d_HVertex::Adaptor3d_HVertex (const gp_Pnt2d& P,
				  const TopAbs_Orientation Or,
				  const Standard_Real Resolution):
       myPnt(P),myTol(Resolution),myOri(Or)
{}


gp_Pnt2d Adaptor3d_HVertex::Value ()
{
  return myPnt;
}

Standard_Real Adaptor3d_HVertex::Parameter (const Handle(Adaptor2d_HCurve2d)& C)
{
  return ElCLib::Parameter(C->Line(),myPnt);
}

Standard_Real Adaptor3d_HVertex::Resolution (const Handle(Adaptor2d_HCurve2d)&)
{
  return myTol;
}

TopAbs_Orientation Adaptor3d_HVertex::Orientation ()
{
  return myOri;
}


Standard_Boolean Adaptor3d_HVertex::IsSame(const Handle(Adaptor3d_HVertex)& Other)
{
  return (myPnt.Distance(Other->Value())<= Precision::Confusion());
}

