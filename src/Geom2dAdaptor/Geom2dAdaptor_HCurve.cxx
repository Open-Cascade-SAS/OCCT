// Created on: 1995-08-25
// Created by: Remi LEQUETTE
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


#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2dAdaptor_HCurve,Geom2dAdaptor_GHCurve)

//=======================================================================
//function : Geom2dAdaptor_HCurve
//purpose  : 
//=======================================================================
Geom2dAdaptor_HCurve::Geom2dAdaptor_HCurve()
{
}

//=======================================================================
//function : Geom2dAdaptor_HCurve
//purpose  : 
//=======================================================================

Geom2dAdaptor_HCurve::Geom2dAdaptor_HCurve(const Geom2dAdaptor_Curve& AS) :
Geom2dAdaptor_GHCurve(AS)
{
}

//=======================================================================
//function : Geom2dAdaptor_HCurve
//purpose  : 
//=======================================================================

Geom2dAdaptor_HCurve::Geom2dAdaptor_HCurve(const Handle(Geom2d_Curve)& S)
{
  ChangeCurve2d().Load(S);
}

//=======================================================================
//function : Geom2dAdaptor_HCurve
//purpose  : 
//=======================================================================

Geom2dAdaptor_HCurve::Geom2dAdaptor_HCurve(const Handle(Geom2d_Curve)& S, 
						  const Standard_Real UFirst, 
						  const Standard_Real ULast)
{
  ChangeCurve2d().Load(S,UFirst,ULast);
}

