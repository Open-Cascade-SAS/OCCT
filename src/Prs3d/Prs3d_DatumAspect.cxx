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

#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>

#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_ToolSphere.hxx>
#include <Prs3d_Arrow.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Prs3d_DatumAspect, Prs3d_BasicAspect)

#define PATCH_TO_OCCT_710

// =======================================================================
// function : Prs3d_DatumAspect
// purpose  :
// =======================================================================
Prs3d_DatumAspect::Prs3d_DatumAspect()
: myAxes (Prs3d_DA_XYZAxis),
  myToDrawLabels (Standard_True)
{
  myAttributes.Bind(Prs3d_DA_XAxisLength, 10.0);
  myAttributes.Bind(Prs3d_DA_YAxisLength, 10.0);
  myAttributes.Bind(Prs3d_DA_ZAxisLength, 10.0);
  myAttributes.Bind(Prs3d_DP_ShadingTubeRadiusPercent, 0.02);
  myAttributes.Bind(Prs3d_DP_ShadingConeRadiusPercent, 0.04);
  myAttributes.Bind(Prs3d_DP_ShadingConeLengthPercent, 0.1);
  myAttributes.Bind(Prs3d_DP_ShadingOriginRadiusPercent, 0.015);
  myAttributes.Bind(Prs3d_DP_ShadingNumberOfFacettes, 12.);

  myDefaultColors.Bind(Prs3d_DP_Origin, Quantity_Color(Quantity_NOC_BLUE1));
  myDefaultColors.Bind(Prs3d_DP_XAxis,  Quantity_Color(Quantity_NOC_RED));
  myDefaultColors.Bind(Prs3d_DP_YAxis,  Quantity_Color(Quantity_NOC_GREEN));
  myDefaultColors.Bind(Prs3d_DP_ZAxis,  Quantity_Color(Quantity_NOC_BLUE1));

  Aspect_TypeOfLine aLineType = Aspect_TOL_SOLID;
  Standard_Real aWidth = 1.0;
  for (int i = Prs3d_DP_Origin; i <= Prs3d_DP_XOZAxis; i++)
  {
    Prs3d_DatumParts aPart = (Prs3d_DatumParts)i;
    Quantity_Color aDefaultColor;
    if (!myDefaultColors.Find(aPart, aDefaultColor))
      aDefaultColor = Quantity_NOC_PEACHPUFF;
    if (aPart != Prs3d_DP_Origin) // origin point is used only in shading mode
      myLineAspects.Bind(aPart, new Prs3d_LineAspect (aDefaultColor, aLineType, aWidth));
    Handle(Prs3d_ShadingAspect) aShadingAspect = new Prs3d_ShadingAspect();
    aShadingAspect->SetColor(aDefaultColor);
    myShadedAspects.Bind(aPart, aShadingAspect);
  }
  myTextAspect = new Prs3d_TextAspect ();
#ifdef PATCH_TO_OCCT_710
  myPointAspect = new Prs3d_PointAspect(Aspect_TOM_POINT/*Aspect_TOM_EMPTY*/,
#else
  myPointAspect = new Prs3d_PointAspect(Aspect_TOM_EMPTY,
#endif
                                        myDefaultColors.Find(Prs3d_DP_Origin), 1.0);
  myArrowAspect = new Prs3d_ArrowAspect();
}

Handle(Graphic3d_ArrayOfPrimitives) Prs3d_DatumAspect::ArrayOfPrimitives(
                                                        const Prs3d_DatumParts& theDatumPart) const
{
  Handle(Graphic3d_ArrayOfPrimitives) anArray;
  myPrimitives.Find(theDatumPart, anArray);
  return anArray;
}

void Prs3d_DatumAspect::UpdatePrimitives(const Prs3d_DatumMode& theMode, const gp_Pnt& theOrigin,
                                         const gp_Dir& theXDirection,
                                         const gp_Dir& theYDirection, const gp_Dir& theZDirection)
{
  myPrimitives.Clear();

  NCollection_DataMap<Prs3d_DatumParts, gp_Dir> anAxisDirs;
  anAxisDirs.Bind(Prs3d_DP_XAxis, theXDirection);
  anAxisDirs.Bind(Prs3d_DP_YAxis, theYDirection);
  anAxisDirs.Bind(Prs3d_DP_ZAxis, theZDirection);

  NCollection_DataMap<Prs3d_DatumParts, gp_Pnt> anAxisPoints;
  gp_XYZ anXYZOrigin = theOrigin.XYZ();
  for (int i = Prs3d_DP_XAxis; i <= Prs3d_DP_ZAxis; i++)
  {
    Prs3d_DatumParts aPart = (Prs3d_DatumParts)i;
    anAxisPoints.Bind(aPart, gp_Pnt(anXYZOrigin + anAxisDirs.Find(aPart).XYZ()*AxisLength(aPart)));
  }

  if (theMode == Prs3d_DM_WireFrame)
  {
    // origin
    Prs3d_DatumParts aPart = Prs3d_DP_Origin;
    if (DrawDatumPart(aPart))
    {
      Handle(Graphic3d_ArrayOfPrimitives) aPrims = new Graphic3d_ArrayOfPoints(1);
      aPrims->AddVertex(theOrigin);
      myPrimitives.Bind(aPart, aPrims);
    }
    // axes
    for (int i = Prs3d_DP_XAxis; i <= Prs3d_DP_ZAxis; i++)
    {
      Prs3d_DatumParts aPart = (Prs3d_DatumParts)i;
      if (DrawDatumPart(aPart))
      {
        Handle(Graphic3d_ArrayOfPrimitives) aPrims = new Graphic3d_ArrayOfSegments(2);
        aPrims->AddVertex(theOrigin);
        aPrims->AddVertex(anAxisPoints.Find(aPart));
        myPrimitives.Bind(aPart, aPrims);
      }

      Prs3d_DatumParts anArrowPart = ArrowPartForAxis(aPart);
      if (DrawDatumPart(anArrowPart))
      {
        myPrimitives.Bind(anArrowPart,
          Prs3d_Arrow::DrawSegments(anAxisPoints.Find(aPart), anAxisDirs.Find(aPart),
                          ArrowAspect()->Angle(),
                          AxisLength(aPart)*Attribute(Prs3d_DP_ShadingConeLengthPercent),
                          (Standard_Integer)Attribute(Prs3d_DP_ShadingNumberOfFacettes)));
      }
    }
  }
  else { // shading mode
    // origin
    Prs3d_DatumParts aPart = Prs3d_DP_Origin;
    if (DrawDatumPart(aPart))
    {
      Standard_Real aSphereRadius = AxisLength(Prs3d_DP_XAxis)*
                                    Attribute(Prs3d_DP_ShadingOriginRadiusPercent);
      Standard_Integer aNbOfFacettes = (Standard_Integer)Attribute(Prs3d_DP_ShadingNumberOfFacettes);
      gp_Trsf aSphereTransform;
      aSphereTransform.SetTranslationPart(gp_Vec(gp_Pnt(0.0, 0.0, 0.0), theOrigin));
      myPrimitives.Bind(aPart, Prs3d_ToolSphere::Create(aSphereRadius, aNbOfFacettes,
                                                        aNbOfFacettes, aSphereTransform));
    }
    // axes
    {
      Standard_Integer aNbOfFacettes =
                        (Standard_Integer)Attribute(Prs3d_DP_ShadingNumberOfFacettes);
      Standard_Real aTubeRadiusPercent = Attribute(Prs3d_DP_ShadingTubeRadiusPercent);
      Standard_Real aConeLengthPercent = Attribute(Prs3d_DP_ShadingConeLengthPercent);
      Standard_Real aConeRadiusPercent = Attribute(Prs3d_DP_ShadingConeRadiusPercent);
      for (Standard_Integer anAxisIter = Prs3d_DP_XAxis; anAxisIter <= Prs3d_DP_ZAxis;
           ++anAxisIter)
      {
        Prs3d_DatumParts aPart = (Prs3d_DatumParts)anAxisIter;
        Prs3d_DatumParts anArrowPart = ArrowPartForAxis(aPart);
        bool aDrawArrow = DrawDatumPart(anArrowPart);
        Standard_Real anAxisLength = AxisLength(aPart);
        gp_Ax1 anAxis (theOrigin, anAxisDirs.Find(aPart));

        if (DrawDatumPart(aPart))
          // draw cylinder
          myPrimitives.Bind(aPart, Prs3d_Arrow::DrawShaded (anAxis, anAxisLength*aTubeRadiusPercent,
                          aDrawArrow ? anAxisLength - anAxisLength*aConeLengthPercent: anAxisLength,
                          0.0, 0.0, aNbOfFacettes));

        // draw arrow
        if (aDrawArrow)
          myPrimitives.Bind(anArrowPart, Prs3d_Arrow::DrawShaded (anAxis,
                            0.0, anAxisLength, anAxisLength*aConeRadiusPercent,
                            anAxisLength*aConeLengthPercent, aNbOfFacettes));
      }
    }
  }
  // planes
  for (Standard_Integer aPlaneIter = Prs3d_DP_XOYAxis; aPlaneIter <= Prs3d_DP_XOZAxis;
       ++aPlaneIter)
  {
    Prs3d_DatumParts aPart = (Prs3d_DatumParts)aPlaneIter;
    if (!DrawDatumPart(aPart))
      continue;

    Handle(Graphic3d_ArrayOfPrimitives) aPrims = new Graphic3d_ArrayOfPolylines(4);
    aPrims->AddVertex(theOrigin);

    Prs3d_DatumParts aPart1, aPart2;
    switch (aPart) {
      case Prs3d_DP_XOYAxis: { aPart1 = Prs3d_DP_XAxis;     aPart2 = Prs3d_DP_YAxis; } break;
      case Prs3d_DP_YOZAxis: { aPart1 = Prs3d_DP_YAxis;     aPart2 = Prs3d_DP_ZAxis; } break;
      case Prs3d_DP_XOZAxis: { aPart1 = Prs3d_DP_XAxis;     aPart2 = Prs3d_DP_ZAxis; } break;
      default: break;
    }
    aPrims->AddVertex(anAxisPoints.Find(aPart1));
    aPrims->AddVertex(anAxisPoints.Find(aPart2));

    aPrims->AddVertex(theOrigin);
    myPrimitives.Bind(aPart, aPrims);
  }
}

void Prs3d_DatumAspect::SetDrawFirstAndSecondAxis (const Standard_Boolean theToDraw)
{
  if (theToDraw)
    myAxes = Prs3d_DatumAxes(myAxes | Prs3d_DA_XAxis | Prs3d_DA_YAxis);
  else
    myAxes = Prs3d_DatumAxes(myAxes & !Prs3d_DA_XAxis & !Prs3d_DA_YAxis);
}

void Prs3d_DatumAspect::SetDrawThirdAxis (const Standard_Boolean theToDraw)
{
  if (theToDraw)
    myAxes = Prs3d_DatumAxes(myAxes | Prs3d_DA_ZAxis);
  else
    myAxes = Prs3d_DatumAxes(myAxes & !Prs3d_DA_ZAxis);
}

bool Prs3d_DatumAspect::DrawDatumPart(const Prs3d_DatumParts& thePart) const
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
    case Prs3d_DP_XOYAxis: return DrawDatumPart(Prs3d_DP_XAxis) && DrawDatumPart(Prs3d_DP_YAxis);
    case Prs3d_DP_YOZAxis: return DrawDatumPart(Prs3d_DP_YAxis) && DrawDatumPart(Prs3d_DP_ZAxis);
    case Prs3d_DP_XOZAxis: return DrawDatumPart(Prs3d_DP_XAxis) && DrawDatumPart(Prs3d_DP_ZAxis);
    default: break;
  }
  return false;
}

Quantity_Length Prs3d_DatumAspect::AxisLength(const Prs3d_DatumParts& thePart) const
{
  switch (thePart)
  {
    case Prs3d_DP_XAxis: return (Quantity_Length)myAttributes.Find(Prs3d_DA_XAxisLength);
    case Prs3d_DP_YAxis: return (Quantity_Length)myAttributes.Find(Prs3d_DA_YAxisLength);
    case Prs3d_DP_ZAxis: return (Quantity_Length)myAttributes.Find(Prs3d_DA_ZAxisLength);
    default: break;
  }
  return Quantity_Length(0);
}

Prs3d_DatumParts Prs3d_DatumAspect::ArrowPartForAxis(const Prs3d_DatumParts& thePart) const
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
