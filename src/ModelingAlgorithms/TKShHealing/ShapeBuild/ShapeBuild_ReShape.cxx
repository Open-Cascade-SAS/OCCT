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

//    abv 28.04.99 S4137: adding method Apply for work on all types of shapes

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Sequence.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend.hxx>
#include <Standard_Type.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeBuild_ReShape, BRepTools_ReShape)

namespace
{

//! Record of a shape replacement to be registered after recursive processing.
struct ReplacementRecord
{
  TopoDS_Shape Original;
  TopoDS_Shape Result;
};

//! Helper structure for recursive Apply implementation.
//! Tracks visited shapes to prevent infinite recursion on shared sub-shapes
//! (e.g., Möbius strip with shared edges) and collects replacements.
struct ApplyHelper
{
  ShapeBuild_ReShape&                     ReShape;
  NCollection_Map<TopoDS_Shape>           Visited;
  NCollection_Sequence<ReplacementRecord> Replacements;
  int                                     Status;

  //! Recursively applies substitutions to a shape.
  //! @param theShape the shape to process
  //! @param theUntil the shape type at which to stop recursion
  //! @return the processed shape (original or replacement)
  TopoDS_Shape Apply(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theUntil)
  {
    Status = ShapeExtend::EncodeStatus(ShapeExtend_OK);
    if (theShape.IsNull())
      return theShape;

    // Apply direct replacement
    TopoDS_Shape aNewShape = ReShape.Value(theShape);

    // If shape removed, return NULL
    if (aNewShape.IsNull())
    {
      Status = ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
      return aNewShape;
    }

    // Check if this shape was already visited to prevent infinite recursion
    if (!Visited.Add(theShape))
    {
      // Already processed this shape, return cached result
      return aNewShape;
    }

    // If shape replaced, apply modifications to the result recursively
    bool aConsLoc = ReShape.ModeConsiderLocation();
    if ((aConsLoc && !aNewShape.IsPartner(theShape)) || (!aConsLoc && !aNewShape.IsSame(theShape)))
    {
      TopoDS_Shape aRes = Apply(aNewShape, theUntil);
      Status |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
      return aRes;
    }

    TopAbs_ShapeEnum aST = theShape.ShapeType();
    if (aST >= theUntil)
      return aNewShape; // stop criterion
    if (aST == TopAbs_VERTEX || aST == TopAbs_SHAPE)
      return theShape;

    BRep_Builder aBuilder;

    TopoDS_Shape       aResult  = theShape.EmptyCopied();
    TopAbs_Orientation anOrient = theShape.Orientation();
    aResult.Orientation(TopAbs_FORWARD); // protect against INTERNAL or EXTERNAL shapes
    bool aModif     = false;
    int  aLocStatus = Status;

    // Apply recorded modifications to subshapes
    for (TopoDS_Iterator anIt(theShape, false); anIt.More(); anIt.Next())
    {
      const TopoDS_Shape& aSh = anIt.Value();
      aNewShape               = Apply(aSh, theUntil);
      if (aNewShape != aSh)
      {
        if (ShapeExtend::DecodeStatus(Status, ShapeExtend_DONE4))
          aLocStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE4);
        aModif = true;
      }
      if (aNewShape.IsNull())
      {
        aLocStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE4);
        continue;
      }
      aLocStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
      if (aST == TopAbs_COMPOUND || aNewShape.ShapeType() == aSh.ShapeType())
      {
        // Fix for SAMTECH bug OCC322 about absence internal vertices after sewing
        aBuilder.Add(aResult, aNewShape);
        continue;
      }
      int aNbItems = 0;
      for (TopoDS_Iterator aSubIt(aNewShape); aSubIt.More(); aSubIt.Next(), aNbItems++)
      {
        const TopoDS_Shape& aSubSh = aSubIt.Value();
        if (aSubSh.ShapeType() == aSh.ShapeType())
          aBuilder.Add(aResult, aSubSh);
        else
          aLocStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
      }
      if (!aNbItems)
        aLocStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    }
    if (!aModif)
      return theShape;

    // Restore range on edge broken by EmptyCopied()
    if (aST == TopAbs_EDGE)
    {
      ShapeBuild_Edge anSBE;
      anSBE.CopyRanges(TopoDS::Edge(aResult), TopoDS::Edge(theShape));
    }
    else if (aST == TopAbs_WIRE || aST == TopAbs_SHELL)
      aResult.Closed(BRep_Tool::IsClosed(aResult));
    aResult.Orientation(anOrient);
    Status = aLocStatus;

    // Record this replacement for later registration
    ReplacementRecord aRecord;
    aRecord.Original = theShape;
    aRecord.Result   = aResult;
    Replacements.Append(aRecord);

    return aResult;
  }
};

} // namespace

//=================================================================================================

ShapeBuild_ReShape::ShapeBuild_ReShape() = default;

//=================================================================================================

TopoDS_Shape ShapeBuild_ReShape::Apply(const TopoDS_Shape&    shape,
                                       const TopAbs_ShapeEnum until,
                                       const int              buildmode)
{
  if (shape.IsNull())
    return shape;
  TopoDS_Shape newsh;
  if (Status(shape, newsh, false) != 0)
    return newsh;

  TopAbs_ShapeEnum st = shape.ShapeType();
  if (st == until)
    return newsh; // critere d arret

  int modif = 0;
  if (st == TopAbs_COMPOUND || st == TopAbs_COMPSOLID)
  {
    BRep_Builder    B;
    TopoDS_Compound C;
    B.MakeCompound(C);
    for (TopoDS_Iterator it(shape); it.More(); it.Next())
    {
      const TopoDS_Shape& sh   = it.Value();
      int                 stat = Status(sh, newsh, false);
      if (stat != 0)
        modif = 1;
      if (stat >= 0)
        B.Add(C, newsh);
    }
    if (modif == 0)
      return shape;
    return C;
  }

  if (st == TopAbs_SOLID)
  {
    BRep_Builder    B;
    TopoDS_Compound C;
    B.MakeCompound(C);
    TopoDS_Solid S;
    B.MakeSolid(S);
    for (TopoDS_Iterator it(shape); it.More(); it.Next())
    {
      const TopoDS_Shape& sh = it.Value();
      newsh                  = Apply(sh, until, buildmode);
      if (newsh.IsNull())
      {
        modif = -1;
      }
      else if (newsh.ShapeType() != TopAbs_SHELL)
      {
        int nbsub = 0;
        for (TopExp_Explorer exh(newsh, TopAbs_SHELL); exh.More(); exh.Next())
        {
          const TopoDS_Shape& onesh = exh.Current();
          B.Add(S, onesh);
          nbsub++;
        }
        if (nbsub == 0)
          modif = -1;
        B.Add(C, newsh); // c est tout
      }
      else
      {
        if (modif == 0 && !sh.IsEqual(newsh))
          modif = 1;
        B.Add(C, newsh);
        B.Add(S, newsh);
      }
    }

    if ((modif < 0 && buildmode < 2) || (modif == 0 && buildmode < 1))
      return C;
    else
      return S;
  }

  if (st == TopAbs_SHELL)
  {
    BRep_Builder    B;
    TopoDS_Compound C;
    B.MakeCompound(C);
    TopoDS_Shell S;
    B.MakeShell(S);
    for (TopoDS_Iterator it(shape); it.More(); it.Next())
    {
      const TopoDS_Shape& sh = it.Value();
      newsh                  = Apply(sh, until, buildmode);
      if (newsh.IsNull())
      {
        modif = -1;
      }
      else if (newsh.ShapeType() != TopAbs_FACE)
      {
        int nbsub = 0;
        for (TopExp_Explorer exf(newsh, TopAbs_FACE); exf.More(); exf.Next())
        {
          const TopoDS_Shape& onesh = exf.Current();
          B.Add(S, onesh);
          nbsub++;
        }
        if (nbsub == 0)
          modif = -1;
        B.Add(C, newsh); // c est tout
      }
      else
      {
        if (modif == 0 && !sh.IsEqual(newsh))
          modif = 1;
        B.Add(C, newsh);
        B.Add(S, newsh);
      }
    }
    if ((modif < 0 && buildmode < 2) || (modif == 0 && buildmode < 1))
      return C;
    else
    {
      S.Closed(BRep_Tool::IsClosed(S));
      return S;
    }
  }
  std::cout << "BRepTools_ReShape::Apply NOT YET IMPLEMENTED" << std::endl;
  return shape;
}

//=================================================================================================

TopoDS_Shape ShapeBuild_ReShape::Apply(const TopoDS_Shape&    theShape,
                                       const TopAbs_ShapeEnum theUntil)
{
  ApplyHelper  aHelper{*this, {}, {}, ShapeExtend::EncodeStatus(ShapeExtend_OK)};
  TopoDS_Shape aResult = aHelper.Apply(theShape, theUntil);

  // Register all collected replacements using public API
  for (NCollection_Sequence<ReplacementRecord>::Iterator anIt(aHelper.Replacements); anIt.More();
       anIt.Next())
  {
    const ReplacementRecord& aRec = anIt.Value();
    if (aRec.Result.IsNull())
      Remove(aRec.Original);
    else
      Replace(aRec.Original, aRec.Result);
  }

  myStatus = aHelper.Status;
  return aResult;
}

//=================================================================================================

int ShapeBuild_ReShape::Status(const TopoDS_Shape& theShape,
                               TopoDS_Shape&       theNewShape,
                               const bool          theLast)
{
  return BRepTools_ReShape::Status(theShape, theNewShape, theLast);
}

//=================================================================================================

bool ShapeBuild_ReShape::Status(const ShapeExtend_Status theStatus) const
{
  return ShapeExtend::DecodeStatus(myStatus, theStatus);
}
