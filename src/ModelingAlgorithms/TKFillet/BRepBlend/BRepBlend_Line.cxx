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

#include <BRepBlend_Line.hxx>
#include <Standard_Type.hxx>
#include <memory>

IMPLEMENT_STANDARD_RTTIEXT(BRepBlend_Line, Standard_Transient)

BRepBlend_Line::BRepBlend_Line()
    : tras1(IntSurf_Undecided),
      tras2(IntSurf_Undecided),
      hass1(false),
      hass2(false)
{
}

void BRepBlend_Line::Clear()
{
  myPoints.Clear(false);
  hass1 = false;
  hass2 = false;
  tras1 = IntSurf_Undecided;
  tras2 = IntSurf_Undecided;
}

//==================================================================================================

BRepBlend_Line::BRepBlend_Line(const BRepBlend_Line& theOther)
    : BRepBlend_Line()
{
  *this = theOther;
}

//==================================================================================================

BRepBlend_Line& BRepBlend_Line::operator=(const BRepBlend_Line& theOther)
{
  if (this != &theOther)
  {
    myPoints.Clear(false);
    myPoints.Reserve(theOther.myPoints.Size());
    for (const auto& aPoint : theOther.myPoints)
    {
      myPoints.Append(std::make_unique<Blend_Point>(*aPoint));
    }
    tras1 = theOther.tras1;
    tras2 = theOther.tras2;
    stp1  = theOther.stp1;
    stp2  = theOther.stp2;
    endp1 = theOther.endp1;
    endp2 = theOther.endp2;
    hass1 = theOther.hass1;
    hass2 = theOther.hass2;
  }
  return *this;
}

void BRepBlend_Line::Set(const IntSurf_TypeTrans TranS1, const IntSurf_TypeTrans TranS2)
{
  hass1 = true;
  hass2 = true;
  tras1 = TranS1;
  tras2 = TranS2;
}

void BRepBlend_Line::Set(const IntSurf_TypeTrans Trans)
{
  hass1 = true;
  hass2 = false;
  tras1 = Trans;
}
