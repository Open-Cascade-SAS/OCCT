// Created on: 2015-11-23
// Created by: Anastasia BORISOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <AIS_RubberBand.hxx>
#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <BRepMesh_Delaun.hxx>
#include <Graphic3d_ArrayOfPolygons.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_TransModeFlags.hxx>
#include <Graphic3d_ZLayerId.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <V3d_View.hxx>

#define MEMORY_BLOCK_SIZE 512 * 7

IMPLEMENT_STANDARD_RTTIEXT(AIS_RubberBand, AIS_InteractiveObject)

//=================================================================================================

AIS_RubberBand::AIS_RubberBand()
    : myIsPolygonClosed(Standard_True)
{
  myDrawer->SetLineAspect(new Prs3d_LineAspect(Quantity_NOC_WHITE, Aspect_TOL_SOLID, 1.0));
  myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
  myDrawer->ShadingAspect()->SetMaterial(Graphic3d_NameOfMaterial_Plastified);
  myDrawer->ShadingAspect()->Aspect()->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
  myDrawer->ShadingAspect()->Aspect()->SetInteriorStyle(Aspect_IS_EMPTY);
  myDrawer->ShadingAspect()->Aspect()->SetAlphaMode(Graphic3d_AlphaMode_Blend);
  myDrawer->ShadingAspect()->SetTransparency(1.0);
  myDrawer->ShadingAspect()->SetColor(Quantity_NOC_WHITE);

  SetTransformPersistence(new Graphic3d_TransformPers(Graphic3d_TMF_2d, Aspect_TOTP_LEFT_LOWER));
  SetZLayer(Graphic3d_ZLayerId_TopOSD);
}

//=================================================================================================

AIS_RubberBand::AIS_RubberBand(const Quantity_Color&   theLineColor,
                               const Aspect_TypeOfLine theLineType,
                               const Standard_Real     theWidth,
                               const Standard_Boolean  theIsPolygonClosed)
    : myIsPolygonClosed(theIsPolygonClosed)
{
  myDrawer->SetLineAspect(new Prs3d_LineAspect(theLineColor, theLineType, theWidth));
  myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
  myDrawer->ShadingAspect()->SetMaterial(Graphic3d_NameOfMaterial_Plastified);
  myDrawer->ShadingAspect()->Aspect()->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
  myDrawer->ShadingAspect()->Aspect()->SetInteriorStyle(Aspect_IS_EMPTY);
  myDrawer->ShadingAspect()->Aspect()->SetAlphaMode(Graphic3d_AlphaMode_Blend);
  myDrawer->ShadingAspect()->SetTransparency(1.0);
  myDrawer->ShadingAspect()->SetColor(Quantity_NOC_WHITE);

  SetTransformPersistence(new Graphic3d_TransformPers(Graphic3d_TMF_2d, Aspect_TOTP_LEFT_LOWER));
  SetZLayer(Graphic3d_ZLayerId_TopOSD);
}

//=================================================================================================

AIS_RubberBand::AIS_RubberBand(const Quantity_Color&   theLineColor,
                               const Aspect_TypeOfLine theLineType,
                               const Quantity_Color    theFillColor,
                               const Standard_Real     theTransparency,
                               const Standard_Real     theLineWidth,
                               const Standard_Boolean  theIsPolygonClosed)
    : myIsPolygonClosed(theIsPolygonClosed)
{
  myDrawer->SetLineAspect(new Prs3d_LineAspect(theLineColor, theLineType, theLineWidth));
  myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
  myDrawer->ShadingAspect()->SetMaterial(Graphic3d_NameOfMaterial_Plastified);
  myDrawer->ShadingAspect()->SetColor(theFillColor);
  myDrawer->ShadingAspect()->Aspect()->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
  myDrawer->ShadingAspect()->Aspect()->SetInteriorStyle(Aspect_IS_SOLID);
  myDrawer->ShadingAspect()->Aspect()->SetAlphaMode(Graphic3d_AlphaMode_Blend);
  myDrawer->ShadingAspect()->SetTransparency(theTransparency);

  SetTransformPersistence(new Graphic3d_TransformPers(Graphic3d_TMF_2d, Aspect_TOTP_LEFT_LOWER));
  SetZLayer(Graphic3d_ZLayerId_TopOSD);
}

//=================================================================================================

AIS_RubberBand::~AIS_RubberBand()
{
  myPoints.Clear();
  myTriangles.Nullify();
  myBorders.Nullify();
}

//=================================================================================================

void AIS_RubberBand::SetRectangle(const Standard_Integer theMinX,
                                  const Standard_Integer theMinY,
                                  const Standard_Integer theMaxX,
                                  const Standard_Integer theMaxY)
{
  myPoints.Clear();
  myPoints.Append(Graphic3d_Vec2i(theMinX, theMinY));
  myPoints.Append(Graphic3d_Vec2i(theMinX, theMaxY));
  myPoints.Append(Graphic3d_Vec2i(theMaxX, theMaxY));
  myPoints.Append(Graphic3d_Vec2i(theMaxX, theMinY));
}

//=================================================================================================

void AIS_RubberBand::AddPoint(const Graphic3d_Vec2i& thePoint)
{
  myPoints.Append(thePoint);
}

//=================================================================================================

void AIS_RubberBand::RemoveLastPoint()
{
  myPoints.Remove(myPoints.Length());
}

//=================================================================================================

const NCollection_Sequence<Graphic3d_Vec2i>& AIS_RubberBand::Points() const
{
  return myPoints;
}

//=================================================================================================

Quantity_Color AIS_RubberBand::LineColor() const
{
  return myDrawer->LineAspect()->Aspect()->Color();
}

//=================================================================================================

void AIS_RubberBand::SetLineColor(const Quantity_Color& theColor)
{
  myDrawer->LineAspect()->SetColor(theColor);
}

//=================================================================================================

Quantity_Color AIS_RubberBand::FillColor() const
{
  return myDrawer->ShadingAspect()->Color();
}

//=================================================================================================

void AIS_RubberBand::SetFillColor(const Quantity_Color& theColor)
{
  myDrawer->ShadingAspect()->SetColor(theColor);
}

//=================================================================================================

void AIS_RubberBand::SetLineWidth(const Standard_Real theWidth) const
{
  myDrawer->LineAspect()->SetWidth(theWidth);
}

//=================================================================================================

Standard_Real AIS_RubberBand::LineWidth() const
{
  return myDrawer->LineAspect()->Aspect()->Width();
}

//=================================================================================================

void AIS_RubberBand::SetLineType(const Aspect_TypeOfLine theType)
{
  myDrawer->LineAspect()->SetTypeOfLine(theType);
}

//=================================================================================================

Aspect_TypeOfLine AIS_RubberBand::LineType() const
{
  return myDrawer->LineAspect()->Aspect()->Type();
}

//=================================================================================================

void AIS_RubberBand::SetFillTransparency(const Standard_Real theValue) const
{
  myDrawer->ShadingAspect()->SetTransparency(theValue);
}

//=================================================================================================

Standard_Real AIS_RubberBand::FillTransparency() const
{
  return myDrawer->ShadingAspect()->Transparency();
}

//=================================================================================================

void AIS_RubberBand::SetFilling(const Standard_Boolean theIsFilling)
{
  myDrawer->ShadingAspect()->Aspect()->SetInteriorStyle(theIsFilling ? Aspect_IS_SOLID
                                                                     : Aspect_IS_EMPTY);
}

//=================================================================================================

void AIS_RubberBand::SetFilling(const Quantity_Color theColor, const Standard_Real theTransparency)
{
  SetFilling(Standard_True);
  SetFillTransparency(theTransparency);
  SetFillColor(theColor);
}

//=================================================================================================

Standard_Boolean AIS_RubberBand::IsFilling() const
{
  Aspect_InteriorStyle aStyle = myDrawer->ShadingAspect()->Aspect()->InteriorStyle();
  return aStyle != Aspect_IS_EMPTY;
}

//=================================================================================================

Standard_Boolean AIS_RubberBand::IsPolygonClosed() const
{
  return myIsPolygonClosed;
}

//=================================================================================================

void AIS_RubberBand::SetPolygonClosed(Standard_Boolean theIsPolygonClosed)
{
  myIsPolygonClosed = theIsPolygonClosed;
}

//=================================================================================================

Standard_Boolean AIS_RubberBand::fillTriangles()
{
  Handle(NCollection_IncAllocator) anAllocator = new NCollection_IncAllocator(MEMORY_BLOCK_SIZE);
  Handle(BRepMesh_DataStructureOfDelaun) aMeshStructure =
    new BRepMesh_DataStructureOfDelaun(anAllocator);
  Standard_Integer           aPtsLower = myPoints.Lower();
  Standard_Integer           aPtsUpper = myPoints.Upper();
  IMeshData::VectorOfInteger anIndexes(myPoints.Length(), anAllocator);
  for (Standard_Integer aPtIdx = aPtsLower; aPtIdx <= aPtsUpper; ++aPtIdx)
  {
    gp_XY aP((Standard_Real)myPoints.Value(aPtIdx).x(), (Standard_Real)myPoints.Value(aPtIdx).y());
    BRepMesh_Vertex aVertex(aP, aPtIdx, BRepMesh_Frontier);
    anIndexes.Append(aMeshStructure->AddNode(aVertex));
  }

  Standard_Real aPtSum = 0;
  for (Standard_Integer aIdx = aPtsLower; aIdx <= aPtsUpper; ++aIdx)
  {
    Standard_Integer aNextIdx = (aIdx % myPoints.Length()) + 1;
    aPtSum += (Standard_Real)(myPoints.Value(aNextIdx).x() - myPoints.Value(aIdx).x())
              * (Standard_Real)(myPoints.Value(aNextIdx).y() + myPoints.Value(aIdx).y());
  }
  Standard_Boolean isClockwiseOrdered = aPtSum < 0;

  for (Standard_Integer aIdx = 0; aIdx < anIndexes.Length(); ++aIdx)
  {
    Standard_Integer aPtIdx     = isClockwiseOrdered ? aIdx : (aIdx + 1) % anIndexes.Length();
    Standard_Integer aNextPtIdx = isClockwiseOrdered ? (aIdx + 1) % anIndexes.Length() : aIdx;
    BRepMesh_Edge anEdge(anIndexes.Value(aPtIdx), anIndexes.Value(aNextPtIdx), BRepMesh_Frontier);
    aMeshStructure->AddLink(anEdge);
  }

  BRepMesh_Delaun                aTriangulation(aMeshStructure, anIndexes);
  const IMeshData::MapOfInteger& aTriangles = aMeshStructure->ElementsOfDomain();
  if (aTriangles.Extent() < 1)
    return Standard_False;

  Standard_Boolean toFill = Standard_False;
  if (myTriangles.IsNull() || myTriangles->VertexNumber() != aTriangles.Extent() * 3)
  {
    toFill      = Standard_True;
    myTriangles = new Graphic3d_ArrayOfTriangles(aTriangles.Extent() * 3, 0, Standard_True);
  }

  Standard_Integer                  aVertexIndex = 1;
  IMeshData::IteratorOfMapOfInteger aTriangleIt(aTriangles);
  for (; aTriangleIt.More(); aTriangleIt.Next())
  {
    const Standard_Integer   aTriangleId      = aTriangleIt.Key();
    const BRepMesh_Triangle& aCurrentTriangle = aMeshStructure->GetElement(aTriangleId);

    if (aCurrentTriangle.Movability() == BRepMesh_Deleted)
      continue;

    Standard_Integer aTriangleVerts[3];
    aMeshStructure->ElementNodes(aCurrentTriangle, aTriangleVerts);

    gp_Pnt2d aPts[3];
    for (Standard_Integer aVertIdx = 0; aVertIdx < 3; ++aVertIdx)
    {
      const BRepMesh_Vertex& aVertex = aMeshStructure->GetNode(aTriangleVerts[aVertIdx]);
      aPts[aVertIdx]                 = aVertex.Coord();
    }

    if (toFill)
    {
      gp_Dir aNorm = gp::DZ();
      for (Standard_Integer anIt = 0; anIt < 3; ++anIt)
      {
        myTriangles
          ->AddVertex(aPts[anIt].X(), aPts[anIt].Y(), 0.0, aNorm.X(), aNorm.Y(), aNorm.Z());
      }
    }
    else
    {
      for (Standard_Integer anIt = 0; anIt < 3; ++anIt)
      {
        myTriangles->SetVertice(aVertexIndex++,
                                (Standard_ShortReal)aPts[anIt].X(),
                                (Standard_ShortReal)aPts[anIt].Y(),
                                0.0f);
      }
    }
  }

  aMeshStructure.Nullify();
  anAllocator.Nullify();

  return Standard_True;
}

//=================================================================================================

void AIS_RubberBand::Compute(const Handle(PrsMgr_PresentationManager)&,
                             const Handle(Prs3d_Presentation)& thePresentation,
                             const Standard_Integer            theMode)
{
  if (theMode != 0)
  {
    return;
  }

  // Draw filling
  if (IsFilling() && fillTriangles())
  {
    Handle(Graphic3d_Group) aGroup1 = thePresentation->NewGroup();
    aGroup1->SetGroupPrimitivesAspect(myDrawer->ShadingAspect()->Aspect());
    aGroup1->AddPrimitiveArray(myTriangles);
  }

  // Draw frame
  if (myBorders.IsNull()
      || myBorders->VertexNumber() != myPoints.Length() + (myIsPolygonClosed ? 1 : 0))
  {
    myBorders = new Graphic3d_ArrayOfPolylines(myPoints.Length() + (myIsPolygonClosed ? 1 : 0));
    for (Standard_Integer anIt = 1; anIt <= myPoints.Length(); anIt++)
    {
      myBorders->AddVertex((Standard_Real)myPoints.Value(anIt).x(),
                           (Standard_Real)myPoints.Value(anIt).y(),
                           0.0);
    }

    if (myIsPolygonClosed)
    {
      myBorders->AddVertex((Standard_Real)myPoints.Value(1).x(),
                           (Standard_Real)myPoints.Value(1).y(),
                           0.0);
    }
  }
  else
  {
    for (Standard_Integer anIt = 1; anIt <= myPoints.Length(); anIt++)
    {
      myBorders->SetVertice(anIt,
                            (Standard_ShortReal)myPoints.Value(anIt).x(),
                            (Standard_ShortReal)myPoints.Value(anIt).y(),
                            0.0f);
    }

    if (myIsPolygonClosed)
    {
      myBorders->SetVertice(myPoints.Length() + 1,
                            (Standard_ShortReal)myPoints.Value(1).x(),
                            (Standard_ShortReal)myPoints.Value(1).y(),
                            0.0f);
    }
  }

  Handle(Graphic3d_Group) aGroup = thePresentation->NewGroup();
  aGroup->SetGroupPrimitivesAspect(myDrawer->LineAspect()->Aspect());
  aGroup->AddPrimitiveArray(myBorders);
}
