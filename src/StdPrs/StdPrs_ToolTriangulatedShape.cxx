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

//=======================================================================
//function : IsTriangulated
//purpose  :
//=======================================================================
Standard_Boolean StdPrs_ToolTriangulatedShape::IsTriangulated (const TopoDS_Shape& theShape)
{
  TopLoc_Location aLocDummy;
  for (TopExp_Explorer aFaceIter (theShape, TopAbs_FACE); aFaceIter.More(); aFaceIter.Next())
  {
    const TopoDS_Face&                aFace = TopoDS::Face (aFaceIter.Current());
    const Handle(Poly_Triangulation)& aTri  = BRep_Tool::Triangulation (aFace, aLocDummy);
    if (aTri.IsNull())
    {
      return Standard_False;
    }
  }
  return Standard_True;
}

//=======================================================================
//function : IsClosed
//purpose  :
//=======================================================================
Standard_Boolean StdPrs_ToolTriangulatedShape::IsClosed (const TopoDS_Shape& theShape)
{
  if (theShape.IsNull())
  {
    return Standard_True;
  }

  switch (theShape.ShapeType())
  {
    case TopAbs_COMPOUND:
    case TopAbs_COMPSOLID:
    default:
    {
      // check that compound consists of closed solids
      for (TopoDS_Iterator anIter (theShape); anIter.More(); anIter.Next())
      {
        const TopoDS_Shape& aShape = anIter.Value();
        if (!IsClosed (aShape))
        {
          return Standard_False;
        }
      }
      return Standard_True;
    }
    case TopAbs_SOLID:
    {
      // Check for non-manifold topology first of all:
      // have to use BRep_Tool::IsClosed() because it checks the face connectivity
      // inside the shape
      if (!BRep_Tool::IsClosed (theShape))
        return Standard_False;

      for (TopoDS_Iterator anIter (theShape); anIter.More(); anIter.Next())
      {
        const TopoDS_Shape& aShape = anIter.Value();
        if (aShape.IsNull())
        {
          continue;
        }

        if (aShape.ShapeType() == TopAbs_FACE)
        {
          // invalid solid
          return Standard_False;
        }
        else if (!IsTriangulated (aShape))
        {
          // mesh contains holes
          return Standard_False;
        }
      }
      return Standard_True;
    }
    case TopAbs_SHELL:
    case TopAbs_FACE:
    {
      // free faces / shell are not allowed
      return Standard_False;
    }
    case TopAbs_WIRE:
    case TopAbs_EDGE:
    case TopAbs_VERTEX:
    {
      // ignore
      return Standard_True;
    }
  }
}

//=======================================================================
//function : GetDeflection
//purpose  :
//=======================================================================
Standard_Real StdPrs_ToolTriangulatedShape::GetDeflection (const TopoDS_Shape& theShape,
                                                           const Handle(Prs3d_Drawer)& theDrawer)
{
  if (theDrawer->TypeOfDeflection() != Aspect_TOD_RELATIVE)
  {
    return theDrawer->MaximalChordialDeviation();
  }

  Bnd_Box aBndBox;
  BRepBndLib::Add (theShape, aBndBox, Standard_False);
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

  // store computed relative deflection of shape as absolute deviation coefficient in case relative type to use it later on for sub-shapes
  const Standard_Real aDeflection = Prs3d::GetDeflection (aBndBox, theDrawer->DeviationCoefficient(), theDrawer->MaximalChordialDeviation());
  theDrawer->SetMaximalChordialDeviation (aDeflection);
  return aDeflection;
}

//=======================================================================
//function : IsTessellated
//purpose  :
//=======================================================================
Standard_Boolean StdPrs_ToolTriangulatedShape::IsTessellated (const TopoDS_Shape&         theShape,
                                                              const Handle(Prs3d_Drawer)& theDrawer)
{
  return BRepTools::Triangulation (theShape, GetDeflection (theShape, theDrawer), true);
}

// =======================================================================
// function : Tessellate
// purpose  :
// =======================================================================
Standard_Boolean StdPrs_ToolTriangulatedShape::Tessellate (const TopoDS_Shape&         theShape,
                                                           const Handle(Prs3d_Drawer)& theDrawer)
{
  Standard_Boolean wasRecomputed = Standard_False;
  // Check if it is possible to avoid unnecessary recomputation of shape triangulation
  if (IsTessellated (theShape, theDrawer))
  {
    return wasRecomputed;
  }

  const Standard_Real aDeflection = GetDeflection (theShape, theDrawer);

  // retrieve meshing tool from Factory
  Handle(BRepMesh_DiscretRoot) aMeshAlgo = BRepMesh_DiscretFactory::Get().Discret (theShape,
                                                                                   aDeflection,
                                                                                   theDrawer->DeviationAngle());
  if (!aMeshAlgo.IsNull())
  {
    aMeshAlgo->Perform();
    wasRecomputed = Standard_True;
  }

  return wasRecomputed;
}

// =======================================================================
// function : ClearOnOwnDeflectionChange
// purpose  :
// =======================================================================
void StdPrs_ToolTriangulatedShape::ClearOnOwnDeflectionChange (const TopoDS_Shape&         theShape,
                                                               const Handle(Prs3d_Drawer)& theDrawer,
                                                               const Standard_Boolean      theToResetCoeff)
{
  if (!theDrawer->IsAutoTriangulation()
    || theShape.IsNull())
  {
    return;
  }

  const Standard_Boolean isOwnDeviationAngle       = theDrawer->HasOwnDeviationAngle();
  const Standard_Boolean isOwnDeviationCoefficient = theDrawer->HasOwnDeviationCoefficient();
  const Standard_Real anAngleNew  = theDrawer->DeviationAngle();
  const Standard_Real anAnglePrev = theDrawer->PreviousDeviationAngle();
  const Standard_Real aCoeffNew   = theDrawer->DeviationCoefficient();
  const Standard_Real aCoeffPrev  = theDrawer->PreviousDeviationCoefficient();
  if ((!isOwnDeviationAngle       || Abs (anAngleNew - anAnglePrev) <= Precision::Angular())
   && (!isOwnDeviationCoefficient || Abs (aCoeffNew  - aCoeffPrev)  <= Precision::Confusion()))
  {
    return;
  }

  BRepTools::Clean (theShape);
  if (theToResetCoeff)
  {
    theDrawer->UpdatePreviousDeviationAngle();
    theDrawer->UpdatePreviousDeviationCoefficient();
  }
}
