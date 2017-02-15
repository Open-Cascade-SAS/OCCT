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

IMPLEMENT_STANDARD_RTTIEXT(Prs3d_DatumAspect, Prs3d_BasicAspect)

// =======================================================================
// function : Prs3d_DatumAspect
// purpose  :
// =======================================================================
Prs3d_DatumAspect::Prs3d_DatumAspect()
: myAxes (Prs3d_DA_XYZAxis),
  myToDrawLabels (Standard_True)
{
  Standard_Real aDefaultLength = 100.0; // default axis lenght, the same as in context
  Quantity_Color aDefaultColor(Quantity_NOC_LIGHTSTEELBLUE4); // default axis color

  myAttributes.Bind (Prs3d_DA_XAxisLength, aDefaultLength);
  myAttributes.Bind (Prs3d_DA_YAxisLength, aDefaultLength);
  myAttributes.Bind (Prs3d_DA_ZAxisLength, aDefaultLength);
  myAttributes.Bind (Prs3d_DP_ShadingTubeRadiusPercent,   0.02);
  myAttributes.Bind (Prs3d_DP_ShadingConeRadiusPercent,   0.04);
  myAttributes.Bind (Prs3d_DP_ShadingConeLengthPercent,   0.1);
  myAttributes.Bind (Prs3d_DP_ShadingOriginRadiusPercent, 0.015);
  myAttributes.Bind (Prs3d_DP_ShadingNumberOfFacettes,    12.0);

  Aspect_TypeOfLine aLineType = Aspect_TOL_SOLID;
  Standard_Real aWidth = 1.0;
  for (int aPartIter = Prs3d_DP_Origin; aPartIter <= Prs3d_DP_XOZAxis; ++aPartIter)
  {
    const Prs3d_DatumParts aPart = (Prs3d_DatumParts )aPartIter;
    if (aPart != Prs3d_DP_Origin) // origin point is used only in shading mode
    {
      myLineAspects.Bind (aPart, new Prs3d_LineAspect (aDefaultColor, aLineType, aWidth));
    }

    Handle(Prs3d_ShadingAspect) aShadingAspect = new Prs3d_ShadingAspect();
    aShadingAspect->SetColor (aDefaultColor);
    myShadedAspects.Bind (aPart, aShadingAspect);
  }
  myTextAspect  = new Prs3d_TextAspect();
  myPointAspect = new Prs3d_PointAspect (Aspect_TOM_EMPTY, aDefaultColor, 1.0);
  myArrowAspect = new Prs3d_ArrowAspect();
}

// =======================================================================
// function : LineAspect
// purpose  :
// =======================================================================
Handle(Prs3d_LineAspect) Prs3d_DatumAspect::LineAspect (Prs3d_DatumParts thePart) const
{
  Handle(Prs3d_LineAspect) aLineAspect;
  myLineAspects.Find (thePart, aLineAspect);
  return aLineAspect;
}

// =======================================================================
// function : ShadingAspect
// purpose  :
// =======================================================================
Handle(Prs3d_ShadingAspect) Prs3d_DatumAspect::ShadingAspect (Prs3d_DatumParts thePart) const
{
  Handle(Prs3d_ShadingAspect) aShadingAspect;
  myShadedAspects.Find (thePart, aShadingAspect);
  return aShadingAspect;
}

// =======================================================================
// function : SetDrawFirstAndSecondAxis
// purpose  :
// =======================================================================
void Prs3d_DatumAspect::SetDrawFirstAndSecondAxis (Standard_Boolean theToDraw)
{
  if (theToDraw)
  {
    myAxes = Prs3d_DatumAxes(myAxes | Prs3d_DA_XAxis | Prs3d_DA_YAxis);
  }
  else
  {
    myAxes = Prs3d_DatumAxes(myAxes & !Prs3d_DA_XAxis & !Prs3d_DA_YAxis);
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
    myAxes = Prs3d_DatumAxes(myAxes | Prs3d_DA_ZAxis);
  }
  else
  {
    myAxes = Prs3d_DatumAxes(myAxes & !Prs3d_DA_ZAxis);
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
    case Prs3d_DP_Origin:  return true;
    case Prs3d_DP_XAxis:
    case Prs3d_DP_XArrow:  return (myAxes & Prs3d_DA_XAxis) != 0;
    case Prs3d_DP_YAxis:
    case Prs3d_DP_YArrow:  return (myAxes & Prs3d_DA_YAxis) != 0;
    case Prs3d_DP_ZAxis:
    case Prs3d_DP_ZArrow:  return (myAxes & Prs3d_DA_ZAxis) != 0;
    case Prs3d_DP_XOYAxis: return DrawDatumPart (Prs3d_DP_XAxis)
                               && DrawDatumPart (Prs3d_DP_YAxis);
    case Prs3d_DP_YOZAxis: return DrawDatumPart (Prs3d_DP_YAxis)
                               && DrawDatumPart (Prs3d_DP_ZAxis);
    case Prs3d_DP_XOZAxis: return DrawDatumPart (Prs3d_DP_XAxis)
                               && DrawDatumPart (Prs3d_DP_ZAxis);
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
    case Prs3d_DP_XAxis: return myAttributes.Find (Prs3d_DA_XAxisLength);
    case Prs3d_DP_YAxis: return myAttributes.Find (Prs3d_DA_YAxisLength);
    case Prs3d_DP_ZAxis: return myAttributes.Find (Prs3d_DA_ZAxisLength);
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
    case Prs3d_DP_XAxis: return Prs3d_DP_XArrow;
    case Prs3d_DP_YAxis: return Prs3d_DP_YArrow;
    case Prs3d_DP_ZAxis: return Prs3d_DP_ZArrow;
    default: break;
  }
  return Prs3d_DP_None;
}
