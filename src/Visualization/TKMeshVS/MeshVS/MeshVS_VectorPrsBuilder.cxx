// Created on: 2003-09-19
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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
#include <Graphic3d_AspectLine3d.hxx>
#include <MeshVS_Buffer.hxx>
#include <MeshVS_DataSource.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_EntityType.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_VectorPrsBuilder.hxx>
#include <Precision.hxx>
#include <Prs3d_Presentation.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <NCollection_PackedMapAlgo.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MeshVS_VectorPrsBuilder, MeshVS_PrsBuilder)

//=================================================================================================

MeshVS_VectorPrsBuilder::MeshVS_VectorPrsBuilder(const occ::handle<MeshVS_Mesh>&       Parent,
                                                 const double                          MaxLength,
                                                 const Quantity_Color&                 VectorColor,
                                                 const MeshVS_DisplayModeFlags&        Flags,
                                                 const occ::handle<MeshVS_DataSource>& DS,
                                                 const int                             Id,
                                                 const MeshVS_BuilderPriority&         Priority,
                                                 const bool                            IsSimplePrs)
    : MeshVS_PrsBuilder(Parent, Flags, DS, Id, Priority),
      myIsSimplePrs(IsSimplePrs),
      mySimpleWidthPrm(2.5),
      mySimpleStartPrm(0.85),
      mySimpleEndPrm(0.95)
{
  occ::handle<MeshVS_Drawer> aDrawer = GetDrawer();
  if (!aDrawer.IsNull())
  {
    aDrawer->SetDouble(MeshVS_DA_VectorMaxLength, MaxLength);
    aDrawer->SetColor(MeshVS_DA_VectorColor, VectorColor);
    aDrawer->SetDouble(MeshVS_DA_VectorArrowPart, 0.1);
  }
}

//=================================================================================================

const NCollection_DataMap<int, gp_Vec>& MeshVS_VectorPrsBuilder::GetVectors(
  const bool IsElements) const
{
  if (IsElements)
    return myElemVectorMap;
  else
    return myNodeVectorMap;
}

//=================================================================================================

void MeshVS_VectorPrsBuilder::SetVectors(const bool                              IsElements,
                                         const NCollection_DataMap<int, gp_Vec>& theMap)
{
  if (IsElements)
    myElemVectorMap = theMap;
  else
    myNodeVectorMap = theMap;
}

//=================================================================================================

bool MeshVS_VectorPrsBuilder::HasVectors(const bool IsElement) const
{
  bool aRes = (myNodeVectorMap.Extent() > 0);
  if (IsElement)
    aRes = (myElemVectorMap.Extent() > 0);
  return aRes;
}

//=================================================================================================

bool MeshVS_VectorPrsBuilder::GetVector(const bool IsElement, const int ID, gp_Vec& Vect) const
{
  const NCollection_DataMap<int, gp_Vec>* aMap = &myNodeVectorMap;
  if (IsElement)
    aMap = &myElemVectorMap;

  bool aRes = aMap->IsBound(ID);
  if (aRes)
    Vect = aMap->Find(ID);

  return aRes;
}

//=================================================================================================

void MeshVS_VectorPrsBuilder::SetVector(const bool IsElement, const int ID, const gp_Vec& Vect)
{
  NCollection_DataMap<int, gp_Vec>* aMap = &myNodeVectorMap;
  if (IsElement)
    aMap = &myElemVectorMap;

  bool aRes = aMap->IsBound(ID);
  if (aRes)
    aMap->ChangeFind(ID) = Vect;
  else
    aMap->Bind(ID, Vect);
}

//=================================================================================================

void MeshVS_VectorPrsBuilder::GetMinMaxVectorValue(const bool IsElement,
                                                   double&    MinValue,
                                                   double&    MaxValue) const
{
  const NCollection_DataMap<int, gp_Vec>* aMap = &myNodeVectorMap;
  if (IsElement)
    aMap = &myElemVectorMap;

  NCollection_DataMap<int, gp_Vec>::Iterator anIt(*aMap);
  if (anIt.More())
    MinValue = MaxValue = anIt.Value().Magnitude();

  double aCurValue;

  for (; anIt.More(); anIt.Next())
  {
    aCurValue = anIt.Value().Magnitude();
    if (MinValue > aCurValue)
      MinValue = aCurValue;
    if (MaxValue < aCurValue)
      MaxValue = aCurValue;
  }
}

//=================================================================================================

#define NB_VERTICES 2
#define NB_BOUNDS 1
#define NB_TRIANGLES 6
#define NB_FANS 1

void MeshVS_VectorPrsBuilder::Build(const occ::handle<Prs3d_Presentation>& Prs,
                                    const TColStd_PackedMapOfInteger&      IDs,
                                    TColStd_PackedMapOfInteger&            IDsToExclude,
                                    const bool                             IsElement,
                                    const int                              theDisplayMode) const
{
  occ::handle<MeshVS_Drawer>     aDrawer = GetDrawer();
  occ::handle<MeshVS_DataSource> aSource = GetDataSource();
  if (aSource.IsNull() || aDrawer.IsNull() || !HasVectors(IsElement)
      || (theDisplayMode & GetFlags()) == 0)
    return;

  int    aMaxFaceNodes;
  double aMaxLen, anArrowPart = 0.1;

  if (!aDrawer->GetInteger(MeshVS_DA_MaxFaceNodes, aMaxFaceNodes) || aMaxFaceNodes <= 0
      || !aDrawer->GetDouble(MeshVS_DA_VectorMaxLength, aMaxLen) || aMaxLen <= 0
      || !aDrawer->GetDouble(MeshVS_DA_VectorArrowPart, anArrowPart) || anArrowPart <= 0)
    return;

  MeshVS_Buffer              aCoordsBuf(3 * aMaxFaceNodes * sizeof(double));
  NCollection_Array1<double> aCoords(aCoordsBuf, 1, 3 * aMaxFaceNodes);
  int                        NbNodes;
  MeshVS_EntityType          aType;

  // DECLARE ARRAYS OF PRIMITIVES
  const NCollection_DataMap<int, gp_Vec>& aMap       = GetVectors(IsElement);
  int                                     aNbVectors = aMap.Extent();

  if (aNbVectors <= 0)
    return;

  // polylines
  int aNbVertices = aNbVectors * NB_VERTICES;

  occ::handle<Graphic3d_ArrayOfPrimitives> aLineArray = new Graphic3d_ArrayOfSegments(aNbVertices);
  occ::handle<Graphic3d_ArrayOfPrimitives> aArrowLineArray =
    new Graphic3d_ArrayOfSegments(aNbVertices);

  occ::handle<Graphic3d_ArrayOfPrimitives> aTriangleArray = new Graphic3d_ArrayOfSegments(
    aNbVectors * 8 /* vertices per arrow */,
    aNbVectors * 12 /* segments per arrow */ * 2 /* indices per segment */);

  NCollection_Array1<gp_Pnt> anArrowPnt(1, 8);
  double                     k, b, aMaxValue, aMinValue, aValue, X, Y, Z;

  double  aMinLength = calculateArrow(anArrowPnt, aMaxLen, anArrowPart);
  gp_Vec  aVec;
  gp_Trsf aTrsf;

  GetMinMaxVectorValue(IsElement, aMinValue, aMaxValue);

  if (aMaxValue - aMinValue > Precision::Confusion())
  {
    k = 0.8 * aMaxLen / (aMaxValue - aMinValue);
    b = aMaxLen - k * aMaxValue;
  }
  else
  {
    k = 0;
    b = aMaxLen;
  }

  TColStd_PackedMapOfInteger aCustomElements;

  // subtract the hidden elements and ids to exclude (to minimize allocated memory)
  TColStd_PackedMapOfInteger anIDs;
  anIDs.Assign(IDs);
  if (IsElement)
  {
    occ::handle<TColStd_HPackedMapOfInteger> aHiddenElems = myParentMesh->GetHiddenElems();
    if (!aHiddenElems.IsNull())
      NCollection_PackedMapAlgo::Subtract(anIDs, aHiddenElems->Map());
  }
  NCollection_PackedMapAlgo::Subtract(anIDs, IDsToExclude);

  TColStd_PackedMapOfInteger::Iterator it(anIDs);
  for (; it.More(); it.Next())
  {
    int aKey = it.Key();
    if (GetVector(IsElement, aKey, aVec))
    {
      aValue = aVec.Magnitude();

      if (std::abs(aValue) < Precision::Confusion())
        continue;

      if (aSource->GetGeom(aKey, IsElement, aCoords, NbNodes, aType))
      {
        if (aType == MeshVS_ET_Node)
        {
          X = aCoords(1);
          Y = aCoords(2);
          Z = aCoords(3);
        }
        else if (aType == MeshVS_ET_Link || aType == MeshVS_ET_Face || aType == MeshVS_ET_Volume)
        {
          if (IsElement && IsExcludingOn())
            IDsToExclude.Add(aKey);
          X = Y = Z = 0;
          for (int i = 1; i <= NbNodes; i++)
          {
            X += aCoords(3 * i - 2);
            Y += aCoords(3 * i - 1);
            Z += aCoords(3 * i);
          }
          X /= double(NbNodes);
          Y /= double(NbNodes);
          Z /= double(NbNodes);
        }
        else
        {
          aCustomElements.Add(aKey);
          continue;
        }

        aTrsf.SetDisplacement(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z)),
                              gp_Ax3(gp_Pnt(X, Y, Z), aVec));

        DrawVector(aTrsf,
                   std::max(k * fabs(aValue) + b, aMinLength),
                   aMaxLen,
                   anArrowPnt,
                   aLineArray,
                   aArrowLineArray,
                   aTriangleArray);
      }
    }
  }

  occ::handle<Graphic3d_Group> aVGroup = Prs->NewGroup();

  Quantity_Color aColor;
  aDrawer->GetColor(MeshVS_DA_VectorColor, aColor);

  // Add primitive arrays to group
  occ::handle<Graphic3d_AspectLine3d> aLinAspect =
    new Graphic3d_AspectLine3d(aColor, Aspect_TOL_SOLID, 1.5);

  aVGroup->SetPrimitivesAspect(aLinAspect);
  aVGroup->AddPrimitiveArray(aLineArray);

  if (!myIsSimplePrs)
  {
    occ::handle<Graphic3d_AspectLine3d> anArrowLinAspect =
      new Graphic3d_AspectLine3d(aColor, Aspect_TOL_SOLID, mySimpleWidthPrm);

    aVGroup->SetPrimitivesAspect(anArrowLinAspect);
    aVGroup->AddPrimitiveArray(aTriangleArray);
  }
  else
  {
    occ::handle<Graphic3d_AspectLine3d> anArrowLinAspect =
      new Graphic3d_AspectLine3d(aColor, Aspect_TOL_SOLID, mySimpleWidthPrm * 1.5);

    aVGroup->SetPrimitivesAspect(anArrowLinAspect);
    aVGroup->AddPrimitiveArray(aArrowLineArray);
  }

  if (!aCustomElements.IsEmpty())
    CustomBuild(Prs, aCustomElements, IDsToExclude, theDisplayMode);
}

//=======================================================================
// name    : DrawVector
// Purpose : Fill arrays of primitives for drawing force
//=======================================================================
void MeshVS_VectorPrsBuilder::DrawVector(
  const gp_Trsf&                                  theTrsf,
  const double                                    theLength,
  const double                                    theMaxLength,
  const NCollection_Array1<gp_Pnt>&               theArrowPoints,
  const occ::handle<Graphic3d_ArrayOfPrimitives>& theLines,
  const occ::handle<Graphic3d_ArrayOfPrimitives>& theArrowLines,
  const occ::handle<Graphic3d_ArrayOfPrimitives>& theTriangles) const
{
  const int PntNum = 8;

  const double aMinLength   = theMaxLength * (1 - mySimpleStartPrm);
  const double aLocalLength = (!myIsSimplePrs || theLength > aMinLength ? theLength : aMinLength);
  // draw line
  gp_Pnt aLinePnt[2] = {gp_Pnt(0, 0, 0), gp_Pnt(0, 0, aLocalLength)};
  theTrsf.Transforms(aLinePnt[0].ChangeCoord());
  theTrsf.Transforms(aLinePnt[1].ChangeCoord());

  theLines->AddVertex(aLinePnt[0]);
  theLines->AddVertex(aLinePnt[1]);

  // draw arrow
  if (!myIsSimplePrs)
  {
    int aLower = theArrowPoints.Lower(), aUpper = theArrowPoints.Upper();

    if (aUpper - aLower < PntNum - 1)
      return;

    NCollection_Array1<gp_Pnt> anArrowPnt(aLower, aUpper);
    for (int aPntIdx = aLower; aPntIdx < aLower + PntNum; ++aPntIdx)
    {
      anArrowPnt(aPntIdx).ChangeCoord() =
        theArrowPoints(aPntIdx).Coord() + gp_XYZ(0, 0, aLocalLength);
      theTrsf.Transforms(anArrowPnt(aPntIdx).ChangeCoord());
    }

    const int aVrtOffset = theTriangles->VertexNumber() + 1;

    for (int aPntIdx = 0; aPntIdx < PntNum; ++aPntIdx)
    {
      theTriangles->AddVertex(anArrowPnt(aLower + aPntIdx));
    }

    for (int aPntIdx = 1; aPntIdx <= PntNum - 2; ++aPntIdx)
    {
      theTriangles->AddEdge(aVrtOffset);
      theTriangles->AddEdge(aVrtOffset + aPntIdx);

      theTriangles->AddEdge(aVrtOffset + aPntIdx);
      theTriangles->AddEdge(aVrtOffset + aPntIdx + 1);
    }
  }
  else
  {
    const double aEndPos      = aLocalLength - theMaxLength * (1 - mySimpleEndPrm);
    const double aArrowLength = theMaxLength * (mySimpleEndPrm - mySimpleStartPrm);
    gp_Pnt       aArrowPnt[2] = {gp_Pnt(0, 0, aEndPos - aArrowLength), gp_Pnt(0, 0, aEndPos)};
    theTrsf.Transforms(aArrowPnt[0].ChangeCoord());
    theTrsf.Transforms(aArrowPnt[1].ChangeCoord());

    theArrowLines->AddVertex(aArrowPnt[0]);
    theArrowLines->AddVertex(aArrowPnt[1]);
  }
}

//=======================================================================
// name    : calculateArrow
// Purpose : Calculate points of arrow ( 8 pnts )
//=======================================================================
double MeshVS_VectorPrsBuilder::calculateArrow(NCollection_Array1<gp_Pnt>& Points,
                                               const double                Length,
                                               const double                ArrowPart)
{
  double h = Length * ArrowPart;
  double w = h / 5.;

  int f         = Points.Lower();
  Points(f)     = gp_Pnt(0, 0, 0);
  Points(f + 1) = gp_Pnt(0, -w, -h);
  Points(f + 2) = gp_Pnt(w * 0.866, -w * 0.5, -h);
  Points(f + 3) = gp_Pnt(w * 0.866, w * 0.5, -h);
  Points(f + 4) = gp_Pnt(0, w, -h);
  Points(f + 5) = gp_Pnt(-w * 0.866, w * 0.5, -h);
  Points(f + 6) = gp_Pnt(-w * 0.866, -w * 0.5, -h);
  Points(f + 7) = gp_Pnt(0, -w, -h);

  return h;
}

//=================================================================================================

void MeshVS_VectorPrsBuilder::SetSimplePrsMode(const bool IsSimpleArrow)
{
  myIsSimplePrs = IsSimpleArrow;
}

//=================================================================================================

void MeshVS_VectorPrsBuilder::SetSimplePrsParams(const double theLineWidthParam,
                                                 const double theStartParam,
                                                 const double theEndParam)
{
  mySimpleWidthPrm = theLineWidthParam;
  mySimpleStartPrm = theStartParam;
  mySimpleEndPrm   = theEndParam;
}
