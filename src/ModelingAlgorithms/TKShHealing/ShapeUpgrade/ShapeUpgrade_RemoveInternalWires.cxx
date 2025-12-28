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

#include <Precision.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeUpgrade_RemoveInternalWires.hxx>
#include <Standard_Type.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_RemoveInternalWires, ShapeUpgrade_Tool)

//=================================================================================================

ShapeUpgrade_RemoveInternalWires::ShapeUpgrade_RemoveInternalWires()
{
  myMinArea                           = 0.;
  myRemoveFacesMode                   = true;
  myStatus                            = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  occ::handle<ShapeBuild_ReShape> aContext = new ShapeBuild_ReShape;
  SetContext(aContext);
}

//=================================================================================================

ShapeUpgrade_RemoveInternalWires::ShapeUpgrade_RemoveInternalWires(const TopoDS_Shape& theShape)
{
  occ::handle<ShapeBuild_ReShape> aContext = new ShapeBuild_ReShape;
  SetContext(aContext);
  Init(theShape);
}

//=================================================================================================

void ShapeUpgrade_RemoveInternalWires::Init(const TopoDS_Shape& theShape)
{
  myShape = theShape;
  Context()->Apply(theShape);
  TopExp::MapShapesAndAncestors(myShape, TopAbs_EDGE, TopAbs_FACE, myEdgeFaces);
  myStatus          = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  myMinArea         = 0.;
  myRemoveFacesMode = true;
}

//=================================================================================================

bool ShapeUpgrade_RemoveInternalWires::Perform()
{
  Clear();
  if (myShape.IsNull())
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    return false;
  }
  TopExp_Explorer aExpF(myShape, TopAbs_FACE);
  for (; aExpF.More(); aExpF.Next())
  {
    TopoDS_Face aF = TopoDS::Face(aExpF.Current());
    removeSmallWire(aF, TopoDS_Wire());
  }
  if (myRemoveFacesMode)
    removeSmallFaces();

  myResult = Context()->Apply(myShape);
  return Status(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeUpgrade_RemoveInternalWires::Perform(
  const NCollection_Sequence<TopoDS_Shape>& theSeqShapes)
{
  if (myShape.IsNull())
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    return false;
  }
  Clear();
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> aWireFaces;
  int                          i = 1, nb = theSeqShapes.Length();
  for (; i <= nb; i++)
  {
    const TopoDS_Shape& aS = theSeqShapes.Value(i);
    if (aS.ShapeType() == TopAbs_FACE)
      removeSmallWire(aS, TopoDS_Wire());
    else if (aS.ShapeType() == TopAbs_WIRE)
    {
      if (!aWireFaces.Extent())
        TopExp::MapShapesAndAncestors(myShape, TopAbs_WIRE, TopAbs_FACE, aWireFaces);
      if (aWireFaces.Contains(aS))
      {
        const NCollection_List<TopoDS_Shape>&        alfaces = aWireFaces.FindFromKey(aS);
        NCollection_List<TopoDS_Shape>::Iterator liter(alfaces);
        for (; liter.More(); liter.Next())
          removeSmallWire(liter.Value(), aS);
      }
    }
  }
  if (myRemoveFacesMode)
    removeSmallFaces();
  myResult = Context()->Apply(myShape);
  return Status(ShapeExtend_DONE);
}

//=================================================================================================

void ShapeUpgrade_RemoveInternalWires::removeSmallWire(const TopoDS_Shape& theFace,
                                                       const TopoDS_Shape& theWire)
{
  TopoDS_Face     aF     = TopoDS::Face(theFace);
  TopoDS_Wire     anOutW = ShapeAnalysis::OuterWire(aF);
  TopoDS_Iterator aIt(aF);
  for (; aIt.More(); aIt.Next())
  {
    if (aIt.Value().ShapeType() != TopAbs_WIRE || aIt.Value().IsSame(anOutW))
      continue;
    // occ::handle<ShapeExtend_WireData> asewd = new  ShapeExtend_WireData();
    TopoDS_Wire aW = TopoDS::Wire(aIt.Value());
    if (!theWire.IsNull() && !theWire.IsSame(aW))
      continue;
    double anArea = ShapeAnalysis::ContourArea(aW);
    if (anArea < myMinArea - Precision::Confusion())
    {
      Context()->Remove(aW);
      myRemoveWires.Append(aW);
      myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
      if (!myRemoveFacesMode)
        continue;

      TopoDS_Iterator aIte(aW, false);
      for (; aIte.More(); aIte.Next())
      {
        const TopoDS_Shape& aE = aIte.Value();
        if (myRemoveEdges.IsBound(aE))
          myRemoveEdges.ChangeFind(aE).Append(aF);
        else
        {
          NCollection_List<TopoDS_Shape> alfaces;
          alfaces.Append(aF);
          myRemoveEdges.Bind(aE, alfaces);
        }
      }
    }
  }
}

//=================================================================================================

void ShapeUpgrade_RemoveInternalWires::removeSmallFaces()
{

  int i = 1;
  for (; i <= myRemoveWires.Length(); i++)
  {
    TopoDS_Shape               aWire = myRemoveWires.Value(i);
    TopoDS_Iterator            aIte(aWire, false);
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aFaceCandidates;
    // collecting all faces containing edges from removed wire
    for (; aIte.More(); aIte.Next())
    {

      const TopoDS_Shape& aEdge = aIte.Value();
      if (!myEdgeFaces.Contains(aEdge))
      {
        myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
        continue;
      }
      const NCollection_List<TopoDS_Shape>&        aLface1 = myEdgeFaces.FindFromKey(aEdge);
      const NCollection_List<TopoDS_Shape>&        aLface2 = myRemoveEdges.Find(aEdge);
      NCollection_List<TopoDS_Shape>::Iterator aliter(aLface1);
      NCollection_List<TopoDS_Shape>::Iterator aliter2(aLface2);
      for (; aliter.More(); aliter.Next())
      {
        TopoDS_Shape aF = Context()->Apply(aliter.Value());
        if (aF.IsNull())
          continue;
        bool isFind = false;
        for (; aliter2.More() && !isFind; aliter2.Next())
        {
          TopoDS_Shape aF2 = Context()->Apply(aliter2.Value());
          isFind           = aF.IsSame(aF2);
        }

        if (!isFind)
        {
          TopoDS_Wire      aWout   = ShapeAnalysis::OuterWire(TopoDS::Face(aF));
          bool isOuter = false;
          TopoDS_Iterator  aIter(aWout, false);
          for (; aIter.More() && !isOuter; aIter.Next())
            isOuter = aEdge.IsSame(aIter.Value());
          if (isOuter)
            aFaceCandidates.Add(aF);
        }
      }
    }

    // remove faces which have outer wire consist of only
    // edges from removed wires and
    // seam edges for faces based on conic surface or
    // in the case of a few faces based on the same conic surface
    // the edges belogining these faces.
    int k = 1;
    for (; k <= aFaceCandidates.Extent(); k++)
    {
      TopoDS_Shape                 aF     = aFaceCandidates.FindKey(k);
      TopoDS_Wire                  anOutW = ShapeAnalysis::OuterWire(TopoDS::Face(aF));
      occ::handle<ShapeExtend_WireData> asewd  = new ShapeExtend_WireData(anOutW);
      int             n = 1, nbE = asewd->NbEdges();
      int             nbNotRemoved = 0;
      for (; n <= nbE; n++)
      {
        if (asewd->IsSeam(n))
          continue;
        TopoDS_Edge aE = asewd->Edge(n);
        if (!myRemoveEdges.IsBound(aE))
        {
          const NCollection_List<TopoDS_Shape>&        aLface3 = myEdgeFaces.FindFromKey(aE);
          NCollection_List<TopoDS_Shape>::Iterator aliter3(aLface3);
          for (; aliter3.More(); aliter3.Next())
          {
            TopoDS_Shape aF2 = Context()->Apply(aliter3.Value());
            if (aF2.IsNull())
              continue;
            if (!aF.IsSame(aF2) && !aFaceCandidates.Contains(aF2))
              nbNotRemoved++;
          }
        }
      }

      if (!nbNotRemoved)
      {
        Context()->Remove(aF);
        myRemovedFaces.Append(aF);
      }
    }
  }

  if (myRemovedFaces.Length())
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
}

//=================================================================================================

void ShapeUpgrade_RemoveInternalWires::Clear()
{
  myRemoveEdges.Clear();
  myRemovedFaces.Clear();
  myRemoveWires.Clear();
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
}
