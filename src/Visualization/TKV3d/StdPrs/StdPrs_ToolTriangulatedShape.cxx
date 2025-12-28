// Created on: 1993-10-27
// Created by: Jean-LOuis FRENKEL
// Copyright (c) 1993-1999 Matra Datavision
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

#include <StdPrs_ToolTriangulatedShape.hxx>

#include <BRepBndLib.hxx>
#include <BRepMesh_DiscretFactory.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <Prs3d.hxx>
#include <Prs3d_Drawer.hxx>
#include <TopLoc_Location.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

//=================================================================================================

bool StdPrs_ToolTriangulatedShape::IsTriangulated(const TopoDS_Shape& theShape)
{
  TopLoc_Location aLocDummy;
  for (TopExp_Explorer aFaceIter(theShape, TopAbs_FACE); aFaceIter.More(); aFaceIter.Next())
  {
    const TopoDS_Face&                     aFace = TopoDS::Face(aFaceIter.Current());
    const occ::handle<Poly_Triangulation>& aTri  = BRep_Tool::Triangulation(aFace, aLocDummy);
    if (aTri.IsNull())
    {
      return false;
    }
  }
  return true;
}

//=================================================================================================

bool StdPrs_ToolTriangulatedShape::IsClosed(const TopoDS_Shape& theShape)
{
  if (theShape.IsNull())
  {
    return true;
  }

  switch (theShape.ShapeType())
  {
    case TopAbs_COMPOUND:
    case TopAbs_COMPSOLID:
    default: {
      // check that compound consists of closed solids
      for (TopoDS_Iterator anIter(theShape); anIter.More(); anIter.Next())
      {
        const TopoDS_Shape& aShape = anIter.Value();
        if (!IsClosed(aShape))
        {
          return false;
        }
      }
      return true;
    }
    case TopAbs_SOLID: {
      // Check for non-manifold topology first of all:
      // have to use BRep_Tool::IsClosed() because it checks the face connectivity
      // inside the shape
      if (!BRep_Tool::IsClosed(theShape))
        return false;

      for (TopoDS_Iterator anIter(theShape); anIter.More(); anIter.Next())
      {
        const TopoDS_Shape& aShape = anIter.Value();
        if (aShape.IsNull())
        {
          continue;
        }

        if (aShape.ShapeType() == TopAbs_FACE)
        {
          // invalid solid
          return false;
        }
        else if (!IsTriangulated(aShape))
        {
          // mesh contains holes
          return false;
        }
      }
      return true;
    }
    case TopAbs_SHELL:
    case TopAbs_FACE: {
      // free faces / shell are not allowed
      return false;
    }
    case TopAbs_WIRE:
    case TopAbs_EDGE:
    case TopAbs_VERTEX: {
      // ignore
      return true;
    }
  }
}

//=================================================================================================

double StdPrs_ToolTriangulatedShape::GetDeflection(const TopoDS_Shape&              theShape,
                                                   const occ::handle<Prs3d_Drawer>& theDrawer)
{
  if (theDrawer->TypeOfDeflection() != Aspect_TOD_RELATIVE)
  {
    return theDrawer->MaximalChordialDeviation();
  }

  Bnd_Box aBndBox;
  BRepBndLib::Add(theShape, aBndBox, false);
  if (aBndBox.IsVoid())
  {
    return theDrawer->MaximalChordialDeviation();
  }
  else if (aBndBox.IsOpen())
  {
    if (!aBndBox.HasFinitePart())
    {
      return theDrawer->MaximalChordialDeviation();
    }
    aBndBox = aBndBox.FinitePart();
  }

  // store computed relative deflection of shape as absolute deviation coefficient in case relative
  // type to use it later on for sub-shapes
  const double aDeflection = Prs3d::GetDeflection(aBndBox,
                                                  theDrawer->DeviationCoefficient(),
                                                  theDrawer->MaximalChordialDeviation());
  theDrawer->SetMaximalChordialDeviation(aDeflection);
  return aDeflection;
}

//=================================================================================================

bool StdPrs_ToolTriangulatedShape::IsTessellated(const TopoDS_Shape&              theShape,
                                                 const occ::handle<Prs3d_Drawer>& theDrawer)
{
  return BRepTools::Triangulation(theShape, GetDeflection(theShape, theDrawer), true);
}

//=================================================================================================

bool StdPrs_ToolTriangulatedShape::Tessellate(const TopoDS_Shape&              theShape,
                                              const occ::handle<Prs3d_Drawer>& theDrawer)
{
  bool wasRecomputed = false;
  // Check if it is possible to avoid unnecessary recomputation of shape triangulation
  if (IsTessellated(theShape, theDrawer))
  {
    return wasRecomputed;
  }

  const double aDeflection = GetDeflection(theShape, theDrawer);

  // retrieve meshing tool from Factory
  occ::handle<BRepMesh_DiscretRoot> aMeshAlgo =
    BRepMesh_DiscretFactory::Get().Discret(theShape, aDeflection, theDrawer->DeviationAngle());
  if (!aMeshAlgo.IsNull())
  {
    aMeshAlgo->Perform();
    wasRecomputed = true;
  }

  return wasRecomputed;
}

//=================================================================================================

void StdPrs_ToolTriangulatedShape::ClearOnOwnDeflectionChange(
  const TopoDS_Shape&              theShape,
  const occ::handle<Prs3d_Drawer>& theDrawer,
  const bool                       theToResetCoeff)
{
  if (!theDrawer->IsAutoTriangulation() || theShape.IsNull())
  {
    return;
  }

  const bool   isOwnDeviationAngle       = theDrawer->HasOwnDeviationAngle();
  const bool   isOwnDeviationCoefficient = theDrawer->HasOwnDeviationCoefficient();
  const double anAngleNew                = theDrawer->DeviationAngle();
  const double anAnglePrev               = theDrawer->PreviousDeviationAngle();
  const double aCoeffNew                 = theDrawer->DeviationCoefficient();
  const double aCoeffPrev                = theDrawer->PreviousDeviationCoefficient();
  if ((!isOwnDeviationAngle || std::abs(anAngleNew - anAnglePrev) <= Precision::Angular())
      && (!isOwnDeviationCoefficient || std::abs(aCoeffNew - aCoeffPrev) <= Precision::Confusion()))
  {
    return;
  }

  BRepTools::Clean(theShape);
  if (theToResetCoeff)
  {
    theDrawer->UpdatePreviousDeviationAngle();
    theDrawer->UpdatePreviousDeviationCoefficient();
  }
}
