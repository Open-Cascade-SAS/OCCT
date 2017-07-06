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


#include <Blend_Point.hxx>
#include <BRepBlend_Extremity.hxx>
#include <BRepBlend_Line.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepBlend_Line,Standard_Transient)

BRepBlend_Line::BRepBlend_Line ():
  tras1(IntSurf_Undecided),tras2(IntSurf_Undecided),
  hass1(Standard_False),hass2(Standard_False)
{}


void BRepBlend_Line::Clear ()
{
  seqpt.Clear();
  hass1 = Standard_False;
  hass2 = Standard_False;
  tras1 = IntSurf_Undecided;
  tras2 = IntSurf_Undecided;
}

void BRepBlend_Line::Set(const IntSurf_TypeTrans TranS1,
			    const IntSurf_TypeTrans TranS2)
{
  hass1 = Standard_True;
  hass2 = Standard_True;
  tras1 = TranS1;
  tras2 = TranS2;
}

void BRepBlend_Line::Set(const IntSurf_TypeTrans Trans)
{
  hass1 = Standard_True;
  hass2 = Standard_False;
  tras1 = Trans;
}

