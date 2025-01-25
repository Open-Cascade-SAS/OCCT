// Created on: 2016-04-19
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#include <BRepMesh_Deflection.hxx>

#include <BRepBndLib.hxx>
#include <BRepMesh_ShapeTool.hxx>
#include <IMeshData_Edge.hxx>
#include <IMeshData_Wire.hxx>
#include <IMeshTools_Parameters.hxx>
#include <TopExp.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_Deflection, Standard_Transient)

//=================================================================================================

Standard_Real BRepMesh_Deflection::ComputeAbsoluteDeflection(
  const TopoDS_Shape& theShape,
  const Standard_Real theRelativeDeflection,
  const Standard_Real theMaxShapeSize)
{
  if (theShape.IsNull())
  {
    return theRelativeDeflection;
  }

  Bnd_Box aBox;
  BRepBndLib::Add(theShape, aBox, Standard_False);

  Standard_Real aShapeSize = theRelativeDeflection;
  BRepMesh_ShapeTool::BoxMaxDimension(aBox, aShapeSize);

  // Adjust resulting value in relation to the total size

  Standard_Real aX1, aY1, aZ1, aX2, aY2, aZ2;
  aBox.Get(aX1, aY1, aZ1, aX2, aY2, aZ2);
  const Standard_Real aMaxShapeSize =
    (theMaxShapeSize > 0.0) ? theMaxShapeSize : Max(aX2 - aX1, Max(aY2 - aY1, aZ2 - aZ1));

  Standard_Real anAdjustmentCoefficient = aMaxShapeSize / (2 * aShapeSize);
  if (anAdjustmentCoefficient < 0.5)
  {
    anAdjustmentCoefficient = 0.5;
  }
  else if (anAdjustmentCoefficient > 2.)
  {
    anAdjustmentCoefficient = 2.;
  }

  return (anAdjustmentCoefficient * aShapeSize * theRelativeDeflection);
}

//=================================================================================================

void BRepMesh_Deflection::ComputeDeflection(const IMeshData::IEdgeHandle& theDEdge,
                                            const Standard_Real           theMaxShapeSize,
                                            const IMeshTools_Parameters&  theParameters)
{
  const Standard_Real aAngDeflection = theParameters.Angle;
  Standard_Real       aLinDeflection =
    !theParameters.Relative
            ? theParameters.Deflection
            : ComputeAbsoluteDeflection(theDEdge->GetEdge(), theParameters.Deflection, theMaxShapeSize);

  const TopoDS_Edge& anEdge = theDEdge->GetEdge();

  TopoDS_Vertex aFirstVertex, aLastVertex;
  TopExp::Vertices(anEdge, aFirstVertex, aLastVertex);

  Handle(Geom_Curve) aCurve;
  Standard_Real      aFirstParam, aLastParam;
  if (BRepMesh_ShapeTool::Range(anEdge, aCurve, aFirstParam, aLastParam))
  {
    const Standard_Real aDistF =
      aFirstVertex.IsNull() ? -1.0
                            : BRep_Tool::Pnt(aFirstVertex).Distance(aCurve->Value(aFirstParam));
    const Standard_Real aDistL =
      aLastVertex.IsNull() ? -1.0 : BRep_Tool::Pnt(aLastVertex).Distance(aCurve->Value(aLastParam));

    const Standard_Real aVertexAdjustDistance = Max(aDistF, aDistL);

    aLinDeflection = Max(aVertexAdjustDistance, aLinDeflection);
  }

  theDEdge->SetDeflection(aLinDeflection);
  theDEdge->SetAngularDeflection(aAngDeflection);
}

//=================================================================================================

void BRepMesh_Deflection::ComputeDeflection(const IMeshData::IWireHandle& theDWire,
                                            const IMeshTools_Parameters&  theParameters)
{
  Standard_Real aWireDeflection = 0.;
  if (theDWire->EdgesNb() > 0)
  {
    for (Standard_Integer aEdgeIt = 0; aEdgeIt < theDWire->EdgesNb(); ++aEdgeIt)
    {
      aWireDeflection += theDWire->GetEdge(aEdgeIt)->GetDeflection();
    }

    aWireDeflection /= theDWire->EdgesNb();
  }
  else
  {
    aWireDeflection = theParameters.Deflection;
  }

  theDWire->SetDeflection(aWireDeflection);
}

//=================================================================================================

void BRepMesh_Deflection::ComputeDeflection(const IMeshData::IFaceHandle& theDFace,
                                            const IMeshTools_Parameters&  theParameters)
{
  Standard_Real aDeflection = theParameters.DeflectionInterior;
  if (theParameters.Relative)
  {
    aDeflection = ComputeAbsoluteDeflection(theDFace->GetFace(), aDeflection, -1.0);
  }

  Standard_Real aFaceDeflection = 0.0;
  if (!theParameters.ForceFaceDeflection)
  {
    if (theDFace->WiresNb() > 0)
    {
      for (Standard_Integer aWireIt = 0; aWireIt < theDFace->WiresNb(); ++aWireIt)
      {
        aFaceDeflection += theDFace->GetWire(aWireIt)->GetDeflection();
      }

      aFaceDeflection /= theDFace->WiresNb();
    }

    aFaceDeflection =
      Max(2. * BRepMesh_ShapeTool::MaxFaceTolerance(theDFace->GetFace()), aFaceDeflection);
  }
  aFaceDeflection = Max(aDeflection, aFaceDeflection);

  theDFace->SetDeflection(aFaceDeflection);
}

//=================================================================================================

Standard_Boolean BRepMesh_Deflection::IsConsistent(const Standard_Real    theCurrent,
                                                   const Standard_Real    theRequired,
                                                   const Standard_Boolean theAllowDecrease,
                                                   const Standard_Real    theRatio)
{
  // Check if the deflection of existing polygonal representation
  // fits the required deflection.
  Standard_Boolean isConsistent =
    theCurrent < (1. + theRatio) * theRequired
    && (!theAllowDecrease || theCurrent > (1. - theRatio) * theRequired);
  return isConsistent;
}
