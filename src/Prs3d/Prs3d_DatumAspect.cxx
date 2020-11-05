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

#include <Prs3d_DatumAspect.hxx>

#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Prs3d_DatumAspect, Prs3d_BasicAspect)

// =======================================================================
// function : Prs3d_DatumAspect
// purpose  :
// =======================================================================
Prs3d_DatumAspect::Prs3d_DatumAspect()
: myAxes (Prs3d_DatumAxes_XYZAxes),
  myToDrawLabels (Standard_True),
  myToDrawArrows (Standard_True)
{
  const Standard_Real  aDefaultLength = 100.0; // default axis length, the same as in context
  const Quantity_Color aDefaultColor (Quantity_NOC_LIGHTSTEELBLUE4); // default axis color

  myAttributes[Prs3d_DatumAttribute_XAxisLength] = aDefaultLength;
  myAttributes[Prs3d_DatumAttribute_YAxisLength] = aDefaultLength;
  myAttributes[Prs3d_DatumAttribute_ZAxisLength] = aDefaultLength;
  myAttributes[Prs3d_DatumAttribute_ShadingTubeRadiusPercent]   = 0.02;
  myAttributes[Prs3d_DatumAttribute_ShadingConeRadiusPercent]   = 0.04;
  myAttributes[Prs3d_DatumAttribute_ShadingConeLengthPercent]   = 0.1;
  myAttributes[Prs3d_DatumAttribute_ShadingOriginRadiusPercent] = 0.015;
  myAttributes[Prs3d_DatumAttribute_ShadingNumberOfFacettes]    = 12.0;

  for (int aPartIter = Prs3d_DatumParts_Origin; aPartIter <= Prs3d_DatumParts_XOZAxis; ++aPartIter)
  {
    const Prs3d_DatumParts aPart = (Prs3d_DatumParts )aPartIter;
    if (aPart != Prs3d_DatumParts_Origin) // origin point is used only in shading mode
    {
      myLineAspects[aPart] = new Prs3d_LineAspect (aDefaultColor, Aspect_TOL_SOLID, 1.0);
    }

    Handle(Prs3d_ShadingAspect) aShadingAspect = new Prs3d_ShadingAspect();
    aShadingAspect->SetColor (aDefaultColor);
    myShadedAspects[aPart] = aShadingAspect;
  }
  myTextAspect  = new Prs3d_TextAspect();
  myPointAspect = new Prs3d_PointAspect (Aspect_TOM_EMPTY, aDefaultColor, 1.0);
  myArrowAspect = new Prs3d_ArrowAspect();
}

// =======================================================================
// function : SetDrawFirstAndSecondAxis
// purpose  :
// =======================================================================
void Prs3d_DatumAspect::SetDrawFirstAndSecondAxis (Standard_Boolean theToDraw)
{
  if (theToDraw)
  {
    myAxes = ((myAxes & Prs3d_DatumAxes_ZAxis) != 0 ? Prs3d_DatumAxes_XYZAxes : Prs3d_DatumAxes_XYAxes);
  }
  else
  {
    myAxes = Prs3d_DatumAxes_ZAxis;
  }
}

// =======================================================================
// function : SetDrawThirdAxis
// purpose  :
// =======================================================================
void Prs3d_DatumAspect::SetDrawThirdAxis (Standard_Boolean theToDraw)
{
  if (theToDraw)
  {
    myAxes = ((myAxes & Prs3d_DatumAxes_XYAxes) != 0 ? Prs3d_DatumAxes_XYZAxes : Prs3d_DatumAxes_ZAxis);
  }
  else
  {
    myAxes = Prs3d_DatumAxes_XYAxes;
  }
}

// =======================================================================
// function : DrawDatumPart
// purpose  :
// =======================================================================
bool Prs3d_DatumAspect::DrawDatumPart (Prs3d_DatumParts thePart) const
{
  switch (thePart)
  {
    case Prs3d_DatumParts_Origin:  return true;
    case Prs3d_DatumParts_XAxis:   return (myAxes & Prs3d_DatumAxes_XAxis) != 0;
    case Prs3d_DatumParts_XArrow:  return (myAxes & Prs3d_DatumAxes_XAxis) != 0 && myToDrawArrows;
    case Prs3d_DatumParts_YAxis:   return (myAxes & Prs3d_DatumAxes_YAxis) != 0;
    case Prs3d_DatumParts_YArrow:  return (myAxes & Prs3d_DatumAxes_YAxis) != 0 && myToDrawArrows;
    case Prs3d_DatumParts_ZAxis:   return (myAxes & Prs3d_DatumAxes_ZAxis) != 0;
    case Prs3d_DatumParts_ZArrow:  return (myAxes & Prs3d_DatumAxes_ZAxis) != 0 && myToDrawArrows;
    case Prs3d_DatumParts_XOYAxis: return DrawDatumPart (Prs3d_DatumParts_XAxis)
                                       && DrawDatumPart (Prs3d_DatumParts_YAxis);
    case Prs3d_DatumParts_YOZAxis: return DrawDatumPart (Prs3d_DatumParts_YAxis)
                                       && DrawDatumPart (Prs3d_DatumParts_ZAxis);
    case Prs3d_DatumParts_XOZAxis: return DrawDatumPart (Prs3d_DatumParts_XAxis)
                                       && DrawDatumPart (Prs3d_DatumParts_ZAxis);
    default: break;
  }
  return false;
}

// =======================================================================
// function : AxisLength
// purpose  :
// =======================================================================
Standard_Real Prs3d_DatumAspect::AxisLength (Prs3d_DatumParts thePart) const
{
  switch (thePart)
  {
    case Prs3d_DatumParts_XAxis: return myAttributes[Prs3d_DatumAttribute_XAxisLength];
    case Prs3d_DatumParts_YAxis: return myAttributes[Prs3d_DatumAttribute_YAxisLength];
    case Prs3d_DatumParts_ZAxis: return myAttributes[Prs3d_DatumAttribute_ZAxisLength];
    default: break;
  }
  return 0.0;
}

// =======================================================================
// function : ArrowPartForAxis
// purpose  :
// =======================================================================
Prs3d_DatumParts Prs3d_DatumAspect::ArrowPartForAxis (Prs3d_DatumParts thePart) const
{
  switch (thePart)
  {
    case Prs3d_DatumParts_XAxis: return Prs3d_DatumParts_XArrow;
    case Prs3d_DatumParts_YAxis: return Prs3d_DatumParts_YArrow;
    case Prs3d_DatumParts_ZAxis: return Prs3d_DatumParts_ZArrow;
    default: break;
  }
  return Prs3d_DatumParts_None;
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void Prs3d_DatumAspect::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myTextAspect.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myPointAspect.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myArrowAspect.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myAxes)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myToDrawLabels)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myToDrawArrows)
}
