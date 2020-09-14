// Created on: 2011-10-14 
// Created by: Roman KOZLOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS 
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

#include <IVtkOCC_ShapeMesher.hxx>

#include <Adaptor3d_IsoCurve.hxx>
#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepMesh_DiscretFactory.hxx>
#include <BRepMesh_DiscretRoot.hxx>
#include <BRepTools.hxx>
#include <Hatch_Hatcher.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Message.hxx>
#include <NCollection_Array1.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <Prs3d.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Standard_ErrorHandler.hxx>
#include <StdPrs_Isolines.hxx>
#include <StdPrs_ToolTriangulatedShape.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IVtkOCC_ShapeMesher,IVtk_IShapeMesher)

// Handle implementation


//================================================================
// Function : internalBuild
// Purpose  : 
//================================================================
void IVtkOCC_ShapeMesher::internalBuild()
{
  // TODO: do we need any protection here so as not to triangualte
  // the shape twice??? This can be done e.g. by checking if
  // triangulation exists for TopoDS_Shape..
  meshShape();

  // Free vertices and free edges should always be shown.
  // Shared edges are needed in WF representation only.
  // TODO: how to filter free edges at visualization level????
  addFreeVertices();
  addEdges();

  // Build wireframe points and cells (lines for isolines)
  addWireFrameFaces();

  // Build shaded representation (based on Poly_Triangulation)
  addShadedFaces();
}

//================================================================
// Function : GetShapeObj
// Purpose  : 
//================================================================
const IVtkOCC_Shape::Handle IVtkOCC_ShapeMesher::GetShapeObj() const
{
  return (IVtkOCC_Shape::Handle::DownCast(myShapeObj));
}

//================================================================
// Function : GetDeflection
// Purpose  : Returns absolute deflection used by this algorithm.
//================================================================
Standard_Real IVtkOCC_ShapeMesher::GetDeflection() const
{
  if (myDeflection < Precision::Confusion()) // if not yet initialized
  {
    Handle(Prs3d_Drawer) aDefDrawer = new Prs3d_Drawer();
    aDefDrawer->SetTypeOfDeflection (Aspect_TOD_RELATIVE);
    aDefDrawer->SetDeviationCoefficient (GetDeviationCoeff());
    myDeflection = StdPrs_ToolTriangulatedShape::GetDeflection (GetShapeObj()->GetShape(), aDefDrawer);
  }

  return myDeflection;
}

//================================================================
// Function : meshShape
// Purpose  : 
//================================================================
void IVtkOCC_ShapeMesher::meshShape()
{
  const TopoDS_Shape& anOcctShape = GetShapeObj()->GetShape();
  if (anOcctShape.IsNull())
  {
    return;
  }

  //Clean triangulation before compute incremental mesh
  BRepTools::Clean (anOcctShape);

  //Compute triangulation
  Standard_Real aDeflection = GetDeflection();
  if (aDeflection < Precision::Confusion())
  {
    return;
  }

  try
  {
    OCC_CATCH_SIGNALS

    Handle(BRepMesh_DiscretRoot) anAlgo;
    anAlgo = BRepMesh_DiscretFactory::Get().Discret (anOcctShape,
                                                     aDeflection,
                                                     GetDeviationAngle());
    if (!anAlgo.IsNull())
    {
      anAlgo->Perform();
    }
  }
  catch (const Standard_Failure& anException)
  {
    Message::SendFail (TCollection_AsciiString("Error: IVtkOCC_ShapeMesher::meshShape() triangulation builder has failed (")
                     + anException.GetMessageString() + ")");
  }
}

//================================================================
// Function : addFreeVertices
// Purpose  : 
//================================================================
void IVtkOCC_ShapeMesher::addFreeVertices()
{
  TopTools_IndexedDataMapOfShapeListOfShape aVertexMap;
  TopExp::MapShapesAndAncestors (GetShapeObj()->GetShape(),
                                 TopAbs_VERTEX,
                                 TopAbs_EDGE,
                                 aVertexMap);

  Standard_Integer aVertNum = aVertexMap.Extent();
  IVtk_MeshType aType;
  for (Standard_Integer anIt = 1; anIt <= aVertNum; anIt++)
  {
    if (aVertexMap.FindFromIndex(anIt).IsEmpty())
    {
      aType = MT_FreeVertex;
    }
    else
    {
      aType = MT_SharedVertex;
    }
    const TopoDS_Vertex& aVertex = TopoDS::Vertex (aVertexMap.FindKey (anIt));
    addVertex (aVertex, GetShapeObj()->GetSubShapeId (aVertex), aType);
  }
}

//================================================================
// Function : addEdges
// Purpose  : 
//================================================================
void IVtkOCC_ShapeMesher::addEdges()
{
  TopTools_IndexedDataMapOfShapeListOfShape anEdgesMap;
  TopExp::MapShapesAndAncestors (GetShapeObj()->GetShape(), 
                                 TopAbs_EDGE, 
                                 TopAbs_FACE,
                                 anEdgesMap);
  int aNbFaces;
  IVtk_MeshType aType;
  myEdgesTypes.Clear();

  TopTools_IndexedDataMapOfShapeListOfShape::Iterator aEdgeIt(anEdgesMap);
  for (; aEdgeIt.More(); aEdgeIt.Next())
  {
    const TopoDS_Edge& anOcctEdge = TopoDS::Edge (aEdgeIt.Key());
    const TopTools_ListOfShape& aFaceList = aEdgeIt.Value();
    aNbFaces = aFaceList.Extent();
    if (aNbFaces == 0)
    {
      aType = MT_FreeEdge;
    }
    else if (aNbFaces == 1)
    {
      aType = MT_BoundaryEdge;
    }
    else
    {
      aType = (aNbFaces >= 2) && (BRep_Tool::MaxContinuity(anOcctEdge) > GeomAbs_G2) ?
        MT_SeamEdge : MT_SharedEdge;
    }
    addEdge (anOcctEdge, GetShapeObj()->GetSubShapeId (anOcctEdge), aType);
    myEdgesTypes.Bind (anOcctEdge, aType);
  }
}

//================================================================
// Function : addWireFrameFaces
// Purpose  : 
//================================================================
void IVtkOCC_ShapeMesher::addWireFrameFaces()
{
  // Check the deflection value once for all faces
  if (GetDeflection() < Precision::Confusion())
  {
    return;
  }

  TopExp_Explorer aFaceIter (GetShapeObj()->GetShape(), TopAbs_FACE);
  for (; aFaceIter.More(); aFaceIter.Next())
  {
    const TopoDS_Face& anOcctFace = TopoDS::Face (aFaceIter.Current());
    try
    {
      OCC_CATCH_SIGNALS
      addWFFace (anOcctFace, 
                 GetShapeObj()->GetSubShapeId (anOcctFace));
    }
    catch (const Standard_Failure& anException)
    {
      Message::SendFail (TCollection_AsciiString("Error: addWireFrameFaces() wireframe presentation builder has failed (")
                       + anException.GetMessageString() + ")");
    }
  }
}

//================================================================
// Function : addShadedFaces
// Purpose  : 
//================================================================
void IVtkOCC_ShapeMesher::addShadedFaces()
{
  TopExp_Explorer aFaceIter (GetShapeObj()->GetShape(), TopAbs_FACE);
  for (; aFaceIter.More(); aFaceIter.Next())
  {
    const TopoDS_Face& anOcctFace = TopoDS::Face (aFaceIter.Current());
    addShadedFace (anOcctFace,
                   GetShapeObj()->GetSubShapeId (anOcctFace));
  }
}

//================================================================
// Function : addVertex
// Purpose  : 
//================================================================
void IVtkOCC_ShapeMesher::addVertex (const TopoDS_Vertex& theVertex,
                                     const IVtk_IdType    theShapeId,
                                     const IVtk_MeshType  theMeshType)
{
  if (theVertex.IsNull())
  {
    return;
  }

  gp_Pnt aPnt3d = BRep_Tool::Pnt (theVertex);

  IVtk_PointId anId = 
    myShapeData->InsertCoordinate (aPnt3d.X(), aPnt3d.Y(), aPnt3d.Z());
  myShapeData->InsertVertex (theShapeId, anId, theMeshType);

}

//================================================================
// Function : processPolyline
// Purpose  : 
//================================================================
void IVtkOCC_ShapeMesher::processPolyline (Standard_Integer          theNbNodes,
                                      const TColgp_Array1OfPnt&      thePoints,
                                      const TColStd_Array1OfInteger& thePointIds,
                                      const IVtk_IdType              theOcctId,
                                      bool                           theNoTransform,
                                      gp_Trsf                        theTransformation,
                                      const IVtk_MeshType            theMeshType)
{
  if (theNbNodes < 2)
  {
    return;
  }

  IVtk_PointIdList aPolyPointIds;

  IVtk_PointId anId;
  for (Standard_Integer aJ = 0; aJ < theNbNodes; aJ++)
  {
    Standard_Integer aPntId = thePointIds (aJ + 1);
    gp_Pnt point = thePoints (aPntId);

    if (!theNoTransform)
    {
      // Apply the transformation to points
      point.Transform (theTransformation);
    }

    anId = myShapeData->InsertCoordinate (point.X(), point.Y(), point.Z());
    aPolyPointIds.Append (anId);
  }

  myShapeData->InsertLine (theOcctId, &aPolyPointIds, theMeshType);
}

//================================================================
// Function : addEdge
// Purpose  : 
//================================================================
void IVtkOCC_ShapeMesher::addEdge (const TopoDS_Edge&  theEdge,
                                   const IVtk_IdType   theShapeId,
                                   const IVtk_MeshType theMeshType)
{
  if (theEdge.IsNull() || BRep_Tool::Degenerated (theEdge))
  {
    return;
  }

  // Two discrete representations of an OCCT edge are possible:
  // 1. Polygon on trinagulation - holds Ids of points
  // contained in Poly_Triangulation object
  Handle(Poly_PolygonOnTriangulation) aPolyOnTriangulation;
  Handle(Poly_Triangulation) aTriangulation;
  TopLoc_Location aLocation;
  BRep_Tool::PolygonOnTriangulation (theEdge,
                                     aPolyOnTriangulation,
                                     aTriangulation,
                                     aLocation,
                                     1);

  // 2. 3D polygon - holds 3D points
  Handle(Poly_Polygon3D) aPoly3d;
  if (aPolyOnTriangulation.IsNull())
  {
    aPoly3d = BRep_Tool::Polygon3D (theEdge, aLocation);
  }

  if (aPoly3d.IsNull() && aPolyOnTriangulation.IsNull())
  {
    return;
  }

  // Handle a non-identity transofmation applied to the edge
  gp_Trsf anEdgeTransf;
  bool noTransform = true;
  if (!aLocation.IsIdentity())
  {
    noTransform = false;
    anEdgeTransf = aLocation.Transformation();
  }

  if (!aPoly3d.IsNull())
  {
    Standard_Integer aNbNodes = aPoly3d->NbNodes();
    const TColgp_Array1OfPnt& aPoints = aPoly3d->Nodes();
    TColStd_Array1OfInteger aPointIds (1, aNbNodes);

    for (Standard_Integer anI = 1; anI <= aNbNodes; anI++)
    {
      aPointIds.SetValue (anI, anI);
    }

    processPolyline (aNbNodes,
                     aPoints,
                     aPointIds,
                     theShapeId,
                     noTransform,
                     anEdgeTransf,
                     theMeshType);
  }
  else
  {
    Standard_Integer aNbNodes = aPolyOnTriangulation->NbNodes();
    const TColStd_Array1OfInteger& aPointIds = aPolyOnTriangulation->Nodes();
    const TColgp_Array1OfPnt& aPoints = aTriangulation->Nodes();

    processPolyline (aNbNodes,
                     aPoints,
                     aPointIds,
                     theShapeId,
                     noTransform,
                     anEdgeTransf,
                     theMeshType);
  }
}

//================================================================
// Function : addWFFace
// Purpose  : 
//================================================================
void IVtkOCC_ShapeMesher::addWFFace (const TopoDS_Face& theFace,
                                     const IVtk_IdType  theShapeId)
{
  if (theFace.IsNull())
  {
    return;
  }

  TopoDS_Face aFaceToMesh = theFace;
  aFaceToMesh.Orientation (TopAbs_FORWARD);

  // The code that builds wireframe representation for a TopoDS_Face
  // has been adapted from some OCCT 6.5.1 methods:
  // - Prs3d_WFShape::Add()
  // - StdPrs_WFDeflectionRestrictedFace::Add()
  // - StdPrs_DeflectionCurve::Add()

  // Add face's edges here but with the face ID
  for (TopExp_Explorer anEdgeIter (aFaceToMesh, TopAbs_EDGE); anEdgeIter.More(); anEdgeIter.Next())
  {
    const TopoDS_Edge& anOcctEdge = TopoDS::Edge (anEdgeIter.Current());
    addEdge (anOcctEdge, theShapeId, myEdgesTypes (anOcctEdge));
  }

  TopLoc_Location aLoc;
  const Handle(Geom_Surface)& aGeomSurf = BRep_Tool::Surface (aFaceToMesh, aLoc);
  if (aGeomSurf.IsNull())
  {
    return;
  }

  const Standard_Real aDeflection = GetDeflection();
  Handle(Prs3d_Drawer) aDrawer = new Prs3d_Drawer();
  aDrawer->SetUIsoAspect (new Prs3d_IsoAspect (Quantity_NOC_WHITE, Aspect_TOL_SOLID, 1.0f, myNbIsos[0]));
  aDrawer->SetVIsoAspect (new Prs3d_IsoAspect (Quantity_NOC_WHITE, Aspect_TOL_SOLID, 1.0f, myNbIsos[1]));
  aDrawer->SetDeviationAngle (myDevAngle);
  aDrawer->SetDeviationCoefficient (myDevCoeff);
  aDrawer->SetMaximalChordialDeviation (aDeflection);

  Prs3d_NListOfSequenceOfPnt aPolylines;
  StdPrs_Isolines::Add (theFace, aDrawer, aDeflection, aPolylines, aPolylines);
  for (Prs3d_NListOfSequenceOfPnt::Iterator aPolyIter (aPolylines); aPolyIter.More(); aPolyIter.Next())
  {
    const Handle(TColgp_HSequenceOfPnt)& aPoints = aPolyIter.Value();
    const Standard_Integer theNbNodes = aPoints->Length();
    if (theNbNodes < 2)
    {
      continue;
    }

    IVtk_PointIdList aPolyPointIds;
    for (TColgp_HSequenceOfPnt::Iterator aNodeIter (*aPoints); aNodeIter.More(); aNodeIter.Next())
    {
      const gp_Pnt& aPnt = aNodeIter.Value();
      const IVtk_PointId anId = myShapeData->InsertCoordinate (aPnt.X(), aPnt.Y(), aPnt.Z());
      aPolyPointIds.Append (anId);
    }

    myShapeData->InsertLine (theShapeId, &aPolyPointIds, MT_IsoLine);
  }
}

//================================================================
// Function : addShadedFace
// Purpose  : 
//================================================================
void IVtkOCC_ShapeMesher::addShadedFace (const TopoDS_Face& theFace,
                                         const IVtk_IdType  theShapeId)
{
  if (theFace.IsNull())
  {
    return;
  }

  // Build triangulation of the face.
  TopLoc_Location aLoc;
  Handle(Poly_Triangulation) anOcctTriangulation = BRep_Tool::Triangulation (theFace, aLoc);
  if (anOcctTriangulation.IsNull())
  {
    return;
  }

  gp_Trsf aPntTransform;
  Standard_Boolean noTransform = Standard_True;
  if (!aLoc.IsIdentity())
  {
    noTransform = Standard_False;
    aPntTransform = aLoc.Transformation();
  }

  // Get triangulation points.
  const TColgp_Array1OfPnt& aPoints = anOcctTriangulation->Nodes();
  Standard_Integer aNbPoints = anOcctTriangulation->NbNodes();

  // Keep inserted points id's of triangulation in an array.
  NCollection_Array1<IVtk_PointId> aPointIds (1, aNbPoints);
  IVtk_PointId anId;

  Standard_Integer anI;
  for (anI = 1; anI <= aNbPoints; anI++)
  {
    gp_Pnt aPoint = aPoints (anI);

    if (!noTransform)
    {
      aPoint.Transform (aPntTransform);
    }

    // Add a point into output shape data and keep its id in the array.
    anId = myShapeData->InsertCoordinate (aPoint.X(), aPoint.Y(), aPoint.Z());
    aPointIds.SetValue (anI, anId);
  }

  // Create triangles on the created triangulation points.
  const Poly_Array1OfTriangle& aTriangles = anOcctTriangulation->Triangles();
  Standard_Integer aNbTriangles = anOcctTriangulation->NbTriangles();
  Standard_Integer aN1, aN2, aN3;
  for (anI = 1; anI <= aNbTriangles; anI++)
  {
    aTriangles(anI).Get (aN1, aN2, aN3); // get indexes of triangle's points
    // Insert new triangle on these points into output shape data.
    myShapeData->InsertTriangle (
      theShapeId, aPointIds(aN1), aPointIds(aN2), aPointIds(aN3), MT_ShadedFace);
  }
}
