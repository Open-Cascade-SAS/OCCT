// Created by: Ekaterina SMIRNOVA
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#include <BRepMesh_FaceAttribute.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepMesh_ShapeTool.hxx>
#include <BRepMesh_Classifier.hxx>
#include <BRepTools.hxx>

IMPLEMENT_STANDARD_HANDLE (BRepMesh_FaceAttribute, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_FaceAttribute, Standard_Transient)

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_FaceAttribute::BRepMesh_FaceAttribute()
  : myDefFace         (0.),
    myUMin            (0.),
    myUMax            (0.),
    myVMin            (0.),
    myVMax            (0.),
    myDeltaX          (1.),
    myDeltaY          (1.),
    myStatus          (BRepMesh_NoError)
{
  init();
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_FaceAttribute::BRepMesh_FaceAttribute(
  const TopoDS_Face&                    theFace,
  const BRepMesh::HDMapOfVertexInteger& theBoundaryVertices,
  const BRepMesh::HDMapOfIntegerPnt&    theBoundaryPoints)
  : myDefFace         (0.),
    myUMin            (0.),
    myUMax            (0.),
    myVMin            (0.),
    myVMax            (0.),
    myDeltaX          (1.),
    myDeltaY          (1.),
    myStatus          (BRepMesh_NoError),
    myBoundaryVertices(theBoundaryVertices),
    myBoundaryPoints  (theBoundaryPoints),
    myFace            (theFace)
{
  init();
}

//=======================================================================
//function : Destructor
//purpose  : 
//=======================================================================
BRepMesh_FaceAttribute::~BRepMesh_FaceAttribute()
{
}

//=======================================================================
//function : init
//purpose  : 
//=======================================================================
void BRepMesh_FaceAttribute::init()
{
  myVertexEdgeMap = new BRepMesh::IMapOfInteger;
  myInternalEdges = new BRepMesh::DMapOfShapePairOfPolygon;
  myLocation2D    = new BRepMesh::DMapOfIntegerListOfXY;
  myClassifier    = new BRepMesh_Classifier;

  if (myFace.IsNull())
    return;

  BRepTools::Update(myFace);
  myFace.Orientation(TopAbs_FORWARD);
  BRepTools::UVBounds(myFace, myUMin, myUMax, myVMin, myVMax);

  BRepAdaptor_Surface aSurfAdaptor(myFace, Standard_False);
  mySurface = new BRepAdaptor_HSurface(aSurfAdaptor);
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BRepMesh_FaceAttribute::Clear()
{
  myStructure.Nullify();
  myLocation2D->Clear();
  myInternalEdges->Clear();
  myVertexEdgeMap->Clear();
}

//=======================================================================
//function : computeParametricTolerance
//purpose  : 
//=======================================================================
Standard_Real BRepMesh_FaceAttribute::computeParametricTolerance(
  const Standard_Real theFirstParam,
  const Standard_Real theLastParam) const
{
  const Standard_Real aDeflectionUV = 1.e-05;
  return Max(Precision::PConfusion(), (theLastParam - theFirstParam) * aDeflectionUV);
}

//=======================================================================
//function : getVertexIndex
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_FaceAttribute::getVertexIndex(
  const TopoDS_Vertex& theVertex,
  Standard_Integer&    theVertexIndex) const
{
  if (!myBoundaryVertices.IsNull() && myBoundaryVertices->IsBound(theVertex))
    theVertexIndex = myBoundaryVertices->Find(theVertex);
  else if (!mySurfaceVertices.IsNull() && mySurfaceVertices->IsBound(theVertex))
    theVertexIndex = mySurfaceVertices->Find(theVertex);
  else
    return Standard_False;

  return Standard_True;
}

//=======================================================================
//function : AddNode
//purpose  : 
//=======================================================================
void BRepMesh_FaceAttribute::AddNode(
  const Standard_Integer         theIndex,
  const gp_XY&                   theUV,
  const BRepMesh_DegreeOfFreedom theMovability,
  Standard_Integer&              theNodeIndex,
  Standard_Integer&              theNodeOnEdgeIndex)
{
  BRepMesh_Vertex aNode(theUV, theIndex, theMovability);
  theNodeIndex = myStructure->AddNode(aNode);
  theNodeOnEdgeIndex = myVertexEdgeMap->FindIndex(theNodeIndex);
  if (theNodeOnEdgeIndex == 0)
    theNodeOnEdgeIndex = myVertexEdgeMap->Add(theNodeIndex);
}

//=======================================================================
//function : Scale
//purpose  : 
//=======================================================================
gp_XY BRepMesh_FaceAttribute::Scale(const gp_XY&           thePoint2d,
                                    const Standard_Boolean isToFaceBasis)
{
  return isToFaceBasis ?
    gp_XY((thePoint2d.X() - myUMin) / myDeltaX, (thePoint2d.Y() - myVMin) / myDeltaY) :
    gp_XY(thePoint2d.X() * myDeltaX + myUMin, thePoint2d.Y() * myDeltaY + myVMin);
}

//=======================================================================
//function : ToleranceU
//purpose  : 
//=======================================================================
Standard_Real BRepMesh_FaceAttribute::ToleranceU() const
{
  return computeParametricTolerance(myUMin, myUMax);
}

//=======================================================================
//function : ToleranceV
//purpose  : 
//=======================================================================
Standard_Real BRepMesh_FaceAttribute::ToleranceV() const
{
  return computeParametricTolerance(myVMin, myVMax);
}
