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
#include <BRepMesh_ShapeTool.hxx>
#include <IMeshTools_Parameters.hxx>
#include <IMeshData_Edge.hxx>
#include <IMeshData_Wire.hxx>
#include <IMeshData_Face.hxx>
#include <BRep_Tool.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>

//=======================================================================
//function : RelativeEdgeDeflection
//purpose  : 
//=======================================================================
Standard_Real BRepMesh_Deflection::RelativeEdgeDeflection(
  const TopoDS_Edge&  theEdge,
  const Standard_Real theDeflection,
  const Standard_Real theMaxShapeSize,
  Standard_Real&      theAdjustmentCoefficient)
{
  theAdjustmentCoefficient = 1.;
  Standard_Real aEdgeDeflection = theDeflection;
  if (theEdge.IsNull())
  {
    return aEdgeDeflection;
  }

  Bnd_Box aBox;
  BRepBndLib::Add (theEdge, aBox, Standard_False);
  BRepMesh_ShapeTool::BoxMaxDimension (aBox, aEdgeDeflection);

  // Adjust resulting value in relation to the total size
  theAdjustmentCoefficient = theMaxShapeSize / (2 * aEdgeDeflection);
  if (theAdjustmentCoefficient < 0.5)
  {
    theAdjustmentCoefficient = 0.5;
  }
  else if (theAdjustmentCoefficient > 2.)
  {
    theAdjustmentCoefficient = 2.;
  }

  return (theAdjustmentCoefficient * aEdgeDeflection * theDeflection);
}

//=======================================================================
// Function: ComputeDeflection (edge)
// Purpose : 
//=======================================================================
void BRepMesh_Deflection::ComputeDeflection (
  const IMeshData::IEdgeHandle& theDEdge,
  const Standard_Real           theMaxShapeSize,
  const IMeshTools_Parameters&  theParameters)
{
  Standard_Real aLinDeflection;
  Standard_Real aAngDeflection;
  if (theParameters.Relative)
  {
    Standard_Real aScale;
    aLinDeflection = RelativeEdgeDeflection (theDEdge->GetEdge (),
      theParameters.Deflection, theMaxShapeSize, aScale);

    // Is it OK?
    aAngDeflection = theParameters.Angle * aScale;
  }
  else
  {
    aLinDeflection = theParameters.Deflection;
    aAngDeflection = theParameters.Angle;
  }

  const TopoDS_Edge& anEdge = theDEdge->GetEdge();

  TopoDS_Vertex aFirstVertex, aLastVertex;
  TopExp::Vertices(anEdge, aFirstVertex, aLastVertex);

  Handle(Geom_Curve) aCurve;
  Standard_Real aFirstParam, aLastParam;
  if (BRepMesh_ShapeTool::Range(anEdge, aCurve, aFirstParam, aLastParam))
  {
    const Standard_Real aDistF = aFirstVertex.IsNull() ? -1.0 : 
                        BRep_Tool::Pnt(aFirstVertex).Distance(aCurve->Value(aFirstParam));
    const Standard_Real aDistL = aLastVertex.IsNull()  ? -1.0 :
                        BRep_Tool::Pnt(aLastVertex).Distance(aCurve->Value(aLastParam));

    const Standard_Real aVertexAdjustDistance = Max(aDistF, aDistL);

    aLinDeflection = Max(aVertexAdjustDistance, aLinDeflection);
  }

  theDEdge->SetDeflection        (aLinDeflection);
  theDEdge->SetAngularDeflection (aAngDeflection);
}

//=======================================================================
// Function: ComputeDeflection (wire)
// Purpose : 
//=======================================================================
void BRepMesh_Deflection::ComputeDeflection (
  const IMeshData::IWireHandle& theDWire,
  const IMeshTools_Parameters&  theParameters)
{
  Standard_Real aWireDeflection = 0.;
  if (theDWire->EdgesNb () > 0)
  {
    for (Standard_Integer aEdgeIt = 0; aEdgeIt < theDWire->EdgesNb(); ++aEdgeIt)
    {
      aWireDeflection += theDWire->GetEdge(aEdgeIt)->GetDeflection();
    }

    aWireDeflection /= theDWire->EdgesNb ();
  }
  else
  {
    aWireDeflection = theParameters.Deflection;
  }

  theDWire->SetDeflection (aWireDeflection);
}

//=======================================================================
// Function: ComputeDeflection (face)
// Purpose : 
//=======================================================================
void BRepMesh_Deflection::ComputeDeflection (
  const IMeshData::IFaceHandle& theDFace,
  const IMeshTools_Parameters&  theParameters)
{
  Standard_Real aFaceDeflection = 0.;
  if (theDFace->WiresNb () > 0)
  {
    for (Standard_Integer aWireIt = 0; aWireIt < theDFace->WiresNb(); ++aWireIt)
    {
      aFaceDeflection += theDFace->GetWire(aWireIt)->GetDeflection();
    }

    aFaceDeflection /= theDFace->WiresNb ();
  }
  else
  {
    aFaceDeflection = theParameters.Deflection;
  }

  theDFace->SetDeflection (Max(2.* BRepMesh_ShapeTool::MaxFaceTolerance(
    theDFace->GetFace()), aFaceDeflection));
}
