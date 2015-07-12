// Created on: 1993-11-03
// Created by: Jean Marc LACHAUME
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

//=======================================================================
// Function : Geom2dHatch_Element
// Purpose  : Magic Constructor.
//=======================================================================

#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dHatch_Element.hxx>

Geom2dHatch_Element::Geom2dHatch_Element (const Geom2dHatch_Element& Other) 
: myCurve(Other.myCurve), myOrientation(Other.myOrientation) { 
}
  
//=======================================================================
// Function : Geom2dHatch_Element
// Purpose  : Empty Constructor.
//=======================================================================

Geom2dHatch_Element::Geom2dHatch_Element () { 
}
  
//=======================================================================
// Function : Geom2dHatch_Element
// Purpose  : Constructor.
//=======================================================================

Geom2dHatch_Element::Geom2dHatch_Element (const Geom2dAdaptor_Curve& Curve,
					  const TopAbs_Orientation Orientation) :
       myCurve       (Curve),
       myOrientation (Orientation)
{
}
  
//=======================================================================
// Function : Curve
// Purpose  : Returns the curve associated to the hatching.
//=======================================================================

const Geom2dAdaptor_Curve& Geom2dHatch_Element::Curve () const
{
  return myCurve ;
}

//=======================================================================
// Function : ChangeCurve
// Purpose  : Returns the curve associated to the hatching.
//=======================================================================

Geom2dAdaptor_Curve& Geom2dHatch_Element::ChangeCurve ()
{
  return myCurve ;
}

//=======================================================================
// Function : Orientation
// Purpose  : Sets the orientation of the element.
//=======================================================================

void Geom2dHatch_Element::Orientation (const TopAbs_Orientation Orientation)
{
  myOrientation = Orientation ;
}

//=======================================================================
// Function : Orientation
// Purpose  : Returns the orientation of the element.
//=======================================================================

TopAbs_Orientation Geom2dHatch_Element::Orientation () const
{
  return myOrientation ;
}



