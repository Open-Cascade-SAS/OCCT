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

// #71 rln 09.03.99: S4135: new class _TrsfModification and method ::ScaleShape
//     abv 08.05.99: S4190: new class and method ConvertToRevolution
//     gka 01.08.99 S4188 : new class and method LimitDegreeShape
//     abv 16.06.99 general function ApplyModifier() implemented
//     gka 21.06.99 general LimitDegreeShape renamed to BSplineRestriction
//     szv 03.01.01 PositiveCones merged with DirectFaces
#include <ShapeCustom.hxx>
#include <ShapeCustom_DirectModification.hxx>
#include <ShapeCustom_TrsfModification.hxx>
#include <ShapeCustom_ConvertToRevolution.hxx>
#include <ShapeCustom_BSplineRestriction.hxx>
#include <ShapeCustom_ConvertToBSpline.hxx>
#include <ShapeCustom_SweptToElementary.hxx>

#include <gp_Trsf.hxx>
#include <GeomAbs_Shape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Iterator.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools_Modifier.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <Standard_ErrorHandler.hxx>

#include <Message_ProgressScope.hxx>

namespace
{
//=======================================================================
// function : UpdateHistoryShape
// purpose  : Updates ShapeBuild_ReShape by the info of the given shape
//=======================================================================
bool UpdateHistoryShape(const TopoDS_Shape&               theShape,
                        const BRepTools_Modifier&         theModifier,
                        const Handle(ShapeBuild_ReShape)& theReShape)
{
  TopoDS_Shape aResult;
  try
  {
    OCC_CATCH_SIGNALS
    aResult = theModifier.ModifiedShape(theShape);
  }
  catch (Standard_NoSuchObject const&)
  {
    // the sub shape isn't in the map
    aResult.Nullify();
  }

  if (!aResult.IsNull() && !theShape.IsSame(aResult))
  {
    theReShape->Replace(theShape, aResult);
    return true;
  }

  return false;
}

//=======================================================================
// function : UpdateHistory
// purpose  : Recursively updates ShapeBuild_ReShape to add information of all sub-shapes
//=======================================================================
void UpdateHistory(const TopoDS_Shape&               theShape,
                   const BRepTools_Modifier&         theModifier,
                   const Handle(ShapeBuild_ReShape)& theReShape)
{
  for (TopoDS_Iterator theIterator(theShape, Standard_False); theIterator.More();
       theIterator.Next())
  {
    const TopoDS_Shape& aCurrent = theIterator.Value();
    if (UpdateHistoryShape(aCurrent, theModifier, theReShape))
    {
      UpdateHistory(aCurrent, theModifier, theReShape);
    }
  }
}

//=======================================================================
// function : UpdateShapeBuild
// purpose  : Recursively updates ShapeBuild_ReShape to add information of all sub-shapes
//=======================================================================
void UpdateShapeBuild(const TopoDS_Shape&               theShape,
                      const BRepTools_Modifier&         theModifier,
                      const Handle(ShapeBuild_ReShape)& theReShape)
{
  UpdateHistoryShape(theShape, theModifier, theReShape);
  UpdateHistory(theShape, theModifier, theReShape);
}
} // namespace

//=================================================================================================

TopoDS_Shape ShapeCustom::ApplyModifier(const TopoDS_Shape&                   S,
                                        const Handle(BRepTools_Modification)& M,
                                        TopTools_DataMapOfShapeShape&         context,
                                        BRepTools_Modifier&                   MD,
                                        const Message_ProgressRange&          theProgress,
                                        const Handle(ShapeBuild_ReShape)&     aReShape)
{
  // protect against INTERNAL/EXTERNAL shapes
  TopoDS_Shape SF = S.Oriented(TopAbs_FORWARD);

  // Process COMPOUNDs separately in order to handle sharing in assemblies
  if (SF.ShapeType() == TopAbs_COMPOUND)
  {
    Standard_Boolean locModified = Standard_False;
    TopoDS_Compound  C;
    BRep_Builder     B;
    B.MakeCompound(C);

    SF.Location(TopLoc_Location());
    Standard_Integer      aShapeCount = SF.NbChildren();
    Message_ProgressScope aPS(theProgress, "Applying Modifier For Solids", aShapeCount);
    for (TopoDS_Iterator it(SF); it.More() && aPS.More(); it.Next())
    {
      Message_ProgressRange aRange = aPS.Next();

      TopoDS_Shape shape       = it.Value();
      TopoDS_Shape aShapeNoLoc = it.Value();
      aShapeNoLoc.Location(TopLoc_Location());

      TopoDS_Shape res;
      if (context.Find(aShapeNoLoc, res))
      {
        res.Orientation(shape.Orientation());
        res.Location(shape.Location(), Standard_False);
      }
      else
      {
        res = ApplyModifier(shape, M, context, MD, aRange, aReShape);
      }

      locModified |= !res.IsSame(shape);

      B.Add(C, res);
    }

    if (!aPS.More())
    {
      // Was cancelled
      return S;
    }

    if (!locModified)
    {
      return S;
    }

    SF.Orientation(S.Orientation());
    C.Orientation(S.Orientation());

    context.Bind(SF, C);

    C.Location(S.Location(), Standard_False);

    if (!aReShape.IsNull())
    {
      aReShape->Replace(S, C);
    }

    return C;
  }

  Message_ProgressScope aPS(theProgress, "Modify the Shape", 1);
  // Modify the shape
  MD.Init(SF);
  MD.Perform(M, aPS.Next());

  if (!aPS.More() || !MD.IsDone())
    return S;

  TopoDS_Shape aResult = MD.ModifiedShape(SF);
  aResult.Orientation(S.Orientation());

  if (!SF.IsSame(aResult))
  {
    context.Bind(S.Located(TopLoc_Location()), aResult.Located(TopLoc_Location()));
  }

  if (!aReShape.IsNull())
  {
    UpdateShapeBuild(SF, MD, aReShape);
  }

  return aResult;
}

//=================================================================================================

TopoDS_Shape ShapeCustom::DirectFaces(const TopoDS_Shape& S)
{
  // Create a modification description
  Handle(ShapeCustom_DirectModification) DM = new ShapeCustom_DirectModification();
  TopTools_DataMapOfShapeShape           context;
  BRepTools_Modifier                     MD;
  return ApplyModifier(S, DM, context, MD);
}

//=================================================================================================

TopoDS_Shape ShapeCustom::ScaleShape(const TopoDS_Shape& S, const Standard_Real scale)
{
  // Create a modification description
  gp_Trsf T;
  T.SetScale(gp_Pnt(0, 0, 0), scale);
  Handle(ShapeCustom_TrsfModification) TM = new ShapeCustom_TrsfModification(T);
  TopTools_DataMapOfShapeShape         context;
  BRepTools_Modifier                   MD;
  return ShapeCustom::ApplyModifier(S, TM, context, MD);
}

//=================================================================================================

TopoDS_Shape ShapeCustom::BSplineRestriction(
  const TopoDS_Shape&                              S,
  const Standard_Real                              Tol3d,
  const Standard_Real                              Tol2d,
  const Standard_Integer                           MaxDegree,
  const Standard_Integer                           MaxNbSegment,
  const GeomAbs_Shape                              Continuity3d,
  const GeomAbs_Shape                              Continuity2d,
  const Standard_Boolean                           Degree,
  const Standard_Boolean                           Rational,
  const Handle(ShapeCustom_RestrictionParameters)& aParameters)
{
  // Create a modification description

  Handle(ShapeCustom_BSplineRestriction) BSR = new ShapeCustom_BSplineRestriction();
  BSR->SetTol3d(Tol3d);
  BSR->SetTol2d(Tol2d);
  BSR->SetMaxDegree(MaxDegree);
  BSR->SetMaxNbSegments(MaxNbSegment);
  BSR->SetContinuity3d(Continuity3d);
  BSR->SetContinuity2d(Continuity2d);
  BSR->SetPriority(Degree);
  BSR->SetConvRational(Rational);
  BSR->SetRestrictionParameters(aParameters);
  // Modify the shape
  TopTools_DataMapOfShapeShape context;
  BRepTools_Modifier           MD;
  return ShapeCustom::ApplyModifier(S, BSR, context, MD);
}

//=================================================================================================

TopoDS_Shape ShapeCustom::ConvertToRevolution(const TopoDS_Shape& S)
{
  // Create a modification description
  Handle(ShapeCustom_ConvertToRevolution) CRev = new ShapeCustom_ConvertToRevolution();
  TopTools_DataMapOfShapeShape            context;
  BRepTools_Modifier                      MD;
  return ShapeCustom::ApplyModifier(S, CRev, context, MD);
}

//=================================================================================================

TopoDS_Shape ShapeCustom::SweptToElementary(const TopoDS_Shape& S)
{
  // Create a modification description
  Handle(ShapeCustom_SweptToElementary) SE = new ShapeCustom_SweptToElementary();
  TopTools_DataMapOfShapeShape          context;
  BRepTools_Modifier                    MD;
  return ShapeCustom::ApplyModifier(S, SE, context, MD);
}

//=================================================================================================

TopoDS_Shape ShapeCustom::ConvertToBSpline(const TopoDS_Shape&    S,
                                           const Standard_Boolean extrMode,
                                           const Standard_Boolean revolMode,
                                           const Standard_Boolean offsetMode,
                                           const Standard_Boolean planeMode)
{
  // Create a modification description
  Handle(ShapeCustom_ConvertToBSpline) BSRev = new ShapeCustom_ConvertToBSpline();
  BSRev->SetExtrusionMode(extrMode);
  BSRev->SetRevolutionMode(revolMode);
  BSRev->SetOffsetMode(offsetMode);
  BSRev->SetPlaneMode(planeMode);
  TopTools_DataMapOfShapeShape context;
  BRepTools_Modifier           MD;
  return ShapeCustom::ApplyModifier(S, BSRev, context, MD);
}
