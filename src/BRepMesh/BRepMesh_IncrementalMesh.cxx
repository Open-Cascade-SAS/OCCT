// Created on: 1995-06-20
// Created by: Stagiaire Alain JOURDAIN
// Copyright (c) 1995-1999 Matra Datavision
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

#include <BRepMesh_IncrementalMesh.hxx>

#include <OSD_Parallel.hxx>
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>

#include <BRepMesh_ShapeTool.hxx>
#include <BRepMesh_Edge.hxx>
#include <BRepMesh_PluginMacro.hxx>

#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepLib.hxx>
#include <BRepBndLib.hxx>
#include <BRepAdaptor_Curve.hxx>

#include <Poly_Triangulation.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_MapOfTransient.hxx>
#include <TopTools_HArray1OfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

#include <GCPnts_TangentialDeflection.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_IncrementalMesh,BRepMesh_DiscretRoot)

namespace
{
  //! Default flag to control parallelization for BRepMesh_IncrementalMesh
  //! tool returned for Mesh Factory
  static Standard_Boolean IS_IN_PARALLEL = Standard_False;
}


//=======================================================================
//function : Default constructor
//purpose  : 
//=======================================================================
BRepMesh_IncrementalMesh::BRepMesh_IncrementalMesh()
: myMaxShapeSize(0.),
  myModified(Standard_False),
  myStatus(0)
{
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_IncrementalMesh::BRepMesh_IncrementalMesh( const TopoDS_Shape&    theShape,
                                                    const Standard_Real    theLinDeflection,
                                                    const Standard_Boolean isRelative,
                                                    const Standard_Real    theAngDeflection,
                                                    const Standard_Boolean isInParallel,
                                                    const Standard_Boolean adaptiveMin)
: myMaxShapeSize(0.),
  myModified(Standard_False),
  myStatus(0)
{
  myParameters.Deflection = theLinDeflection;
  myParameters.Relative = isRelative;
  myParameters.Angle = theAngDeflection;
  myParameters.InParallel = isInParallel;
  myParameters.AdaptiveMin = adaptiveMin;

  myShape = theShape;
  Perform();
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_IncrementalMesh::BRepMesh_IncrementalMesh(const TopoDS_Shape& theShape,
                                                   const BRepMesh_FastDiscret::Parameters& theParameters)
  : myParameters(theParameters)
{
  myShape       = theShape;
  
  Perform();
}

//=======================================================================
//function : Destructor
//purpose  : 
//=======================================================================
BRepMesh_IncrementalMesh::~BRepMesh_IncrementalMesh()
{
}

//=======================================================================
//function : clear
//purpose  : 
//=======================================================================
void BRepMesh_IncrementalMesh::clear()
{
  // the allocator will be alive while the structures are alive
  Handle(NCollection_BaseAllocator) anAlloc =
    new NCollection_IncAllocator(BRepMesh::MEMORY_BLOCK_SIZE_HUGE);
  myEdges.Clear(anAlloc);
  myEdgeDeflection.Clear(anAlloc);
  myFaces.Clear();
  myMesh.Nullify();
}

//=======================================================================
//function : init
//purpose  : 
//=======================================================================
void BRepMesh_IncrementalMesh::init() 
{
  myStatus   = 0;
  myModified = Standard_False;

  setDone();
  clear();

  collectFaces();

  Bnd_Box aBox;
  if ( myParameters.Relative ) 
  {
    BRepBndLib::Add(myShape, aBox, Standard_False);

    if (aBox.IsVoid())
    {
      // Nothing to mesh.
      return;
    }

    BRepMesh_ShapeTool::BoxMaxDimension(aBox, myMaxShapeSize);
  }

  myMesh = new BRepMesh_FastDiscret (aBox, myParameters);
  
  myMesh->InitSharedFaces(myShape);
}

//=======================================================================
//function : collectFaces
//purpose  : 
//=======================================================================
void BRepMesh_IncrementalMesh::collectFaces()
{
  Handle(NCollection_IncAllocator) anAlloc = new NCollection_IncAllocator;
  TopTools_ListOfShape aFaceList(anAlloc);
  BRepLib::ReverseSortFaces(myShape, aFaceList);
  TColStd_MapOfTransient aTFaceMap(1, anAlloc);

  // make array of faces suitable for processing (excluding faces without surface)
  TopLoc_Location aDummyLoc;
  TopTools_ListIteratorOfListOfShape aFaceIter(aFaceList);
  for (; aFaceIter.More(); aFaceIter.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(aFaceIter.Value());
    const Handle(TopoDS_TShape)& aTFace = aFace.TShape();
    if (!aTFaceMap.Add (aTFace))
      continue; // already processed

    const Handle(Geom_Surface)& aSurf = BRep_Tool::Surface(aFace, aDummyLoc);
    if (aSurf.IsNull())
      continue;

    myFaces.Append(aFace);
  }
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BRepMesh_IncrementalMesh::Perform()
{
  init();

  if (myMesh.IsNull())
    return;

  update();
}

//=======================================================================
//function : update()
//purpose  : 
//=======================================================================
void BRepMesh_IncrementalMesh::update()
{
  // Update edges data
  TopExp_Explorer aExplorer(myShape, TopAbs_EDGE);
  for (; aExplorer.More(); aExplorer.Next())
  {
    const TopoDS_Edge& aEdge = TopoDS::Edge(aExplorer.Current());
    if(!BRep_Tool::IsGeometric(aEdge))
      continue;

    update(aEdge);
  }

  // Update faces data
  NCollection_Vector<TopoDS_Face>::Iterator aFaceIt(myFaces);
  for (; aFaceIt.More(); aFaceIt.Next())
    update(aFaceIt.Value());

  // Mesh faces
  OSD_Parallel::ForEach(myFaces.begin(), myFaces.end(), *myMesh, !myParameters.InParallel);

  commit();
  clear();
}

//=======================================================================
//function : discretizeFreeEdges
//purpose  : 
//=======================================================================
void BRepMesh_IncrementalMesh::discretizeFreeEdges()
{
  TopExp_Explorer aExplorer(myShape ,TopAbs_EDGE, TopAbs_FACE);
  for (; aExplorer.More(); aExplorer.Next())
  {
    const TopoDS_Edge& aEdge = TopoDS::Edge(aExplorer.Current());
    if(!BRep_Tool::IsGeometric(aEdge))
      continue;
    
    TopLoc_Location aLoc;
    Standard_Real aEdgeDeflection  = edgeDeflection(aEdge);
    Handle(Poly_Polygon3D) aPoly3D = BRep_Tool::Polygon3D(aEdge, aLoc);
    if (!aPoly3D.IsNull() && aPoly3D->Deflection() < 1.1 * aEdgeDeflection)
      continue;

    BRepAdaptor_Curve aCurve(aEdge);
    GCPnts_TangentialDeflection aDiscret(aCurve, aCurve.FirstParameter(),
      aCurve.LastParameter(), myParameters.Angle, aEdgeDeflection, 2,
      Precision::PConfusion(), myParameters.MinSize);

    Standard_Integer aNodesNb = aDiscret.NbPoints();
    TColgp_Array1OfPnt   aNodes  (1, aNodesNb);
    TColStd_Array1OfReal aUVNodes(1, aNodesNb);
    for (Standard_Integer i = 1; i <= aNodesNb; ++i)
    {
      aNodes  (i) = aDiscret.Value(i);
      aUVNodes(i) = aDiscret.Parameter(i);
    }
    
    aPoly3D = new Poly_Polygon3D(aNodes, aUVNodes);
    aPoly3D->Deflection(myParameters.Deflection);

    BRep_Builder aBuilder;
    aBuilder.UpdateEdge(aEdge, aPoly3D);
  }
}

//=======================================================================
//function : edgeDeflection
//purpose  : 
//=======================================================================
Standard_Real BRepMesh_IncrementalMesh::edgeDeflection(
  const TopoDS_Edge& theEdge)
{
  const Standard_Real* pDef = myEdgeDeflection.Seek(theEdge);
  if (pDef)
    return *pDef;

  Standard_Real aEdgeDeflection;
  if ( myParameters.Relative ) 
  {
    Standard_Real aScale;
    aEdgeDeflection = BRepMesh_ShapeTool::RelativeEdgeDeflection(theEdge, 
      myParameters.Deflection, myMaxShapeSize, aScale);
  }
  else
    aEdgeDeflection = myParameters.Deflection;

  myEdgeDeflection.Bind(theEdge, aEdgeDeflection);
  return aEdgeDeflection;
}

//=======================================================================
//function : faceDeflection
//purpose  : 
//=======================================================================
Standard_Real BRepMesh_IncrementalMesh::faceDeflection(
  const TopoDS_Face& theFace)
{
  if ( !myParameters.Relative )
    return myParameters.Deflection;

  Standard_Integer aEdgesNb        = 0;
  Standard_Real    aFaceDeflection = 0.;

  TopExp_Explorer aEdgeIt(theFace, TopAbs_EDGE);
  for (; aEdgeIt.More(); aEdgeIt.Next(), ++aEdgesNb)
  {
    const TopoDS_Edge& aEdge = TopoDS::Edge(aEdgeIt.Current());
    aFaceDeflection += edgeDeflection(aEdge);
  }

  return (aEdgesNb == 0) ? myParameters.Deflection : (aFaceDeflection / aEdgesNb);
}

//=======================================================================
//function : update(edge)
//purpose  : 
//=======================================================================
void BRepMesh_IncrementalMesh::update(const TopoDS_Edge& theEdge)
{
  if (!myEdges.IsBound(theEdge))
    myEdges.Bind(theEdge, BRepMesh::DMapOfTriangulationBool(3, myEdges.Allocator()));

  Standard_Real aEdgeDeflection = edgeDeflection(theEdge);
  // Check that triangulation relies to face of the given shape.
  const TopTools_IndexedDataMapOfShapeListOfShape& aMapOfSharedFaces = 
    myMesh->SharedFaces();

  const TopTools_ListOfShape& aSharedFaces = 
    aMapOfSharedFaces.FindFromKey(theEdge);

  TopTools_ListIteratorOfListOfShape aSharedFaceIt(aSharedFaces);
  for (; aSharedFaceIt.More(); aSharedFaceIt.Next())
  {
    TopLoc_Location aLoc;
    const TopoDS_Face& aFace = TopoDS::Face(aSharedFaceIt.Value());
    const Handle(Poly_Triangulation)& aFaceTriangulation = 
      BRep_Tool::Triangulation(aFace, aLoc);

    if (aFaceTriangulation.IsNull())
      continue;

    Standard_Boolean isConsistent = Standard_False;
    const Handle(Poly_PolygonOnTriangulation)& aPolygon =
      BRep_Tool::PolygonOnTriangulation(theEdge, aFaceTriangulation, aLoc);

    if (!aPolygon.IsNull())
    {
      isConsistent = aPolygon->Deflection() < 1.1 * aEdgeDeflection &&
        aPolygon->HasParameters();

      if (!isConsistent)
      {
        myModified = Standard_True;
        BRepMesh_ShapeTool::NullifyEdge(theEdge, aFaceTriangulation, aLoc);
      }
    }

    myEdges(theEdge).Bind(aFaceTriangulation, isConsistent);
  }
}

//=======================================================================
//function : isToBeMeshed
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_IncrementalMesh::toBeMeshed(
  const TopoDS_Face&     theFace,
  const Standard_Boolean isWithCheck)
{
  TopLoc_Location aLoc;
  const Handle(Poly_Triangulation)& aTriangulation = 
    BRep_Tool::Triangulation(theFace, aLoc);

  if (aTriangulation.IsNull())
    return Standard_True;

  if (isWithCheck)
  {
    Standard_Real aFaceDeflection = faceDeflection(theFace);
    if (aTriangulation->Deflection() < 1.1 * aFaceDeflection)
    {
      Standard_Boolean isEdgesConsistent = Standard_True;
      TopExp_Explorer aEdgeIt(theFace, TopAbs_EDGE);
      for (; aEdgeIt.More() && isEdgesConsistent; aEdgeIt.Next())
      {
        const TopoDS_Edge& aEdge = TopoDS::Edge(aEdgeIt.Current());
        if (!myEdges.IsBound(aEdge))
          continue;

        BRepMesh::DMapOfTriangulationBool& aTriMap = myEdges(aEdge);
        isEdgesConsistent &= aTriMap.IsBound(aTriangulation) &&
          aTriMap(aTriangulation);
      }

      if (isEdgesConsistent)
      {
        // #25080: check that indices of links forming triangles are in range.
        Standard_Boolean isTriangulationConsistent = Standard_True;
        const Standard_Integer aNodesNb = aTriangulation->NbNodes();
        const Poly_Array1OfTriangle& aTriangles = aTriangulation->Triangles();
        Standard_Integer i = aTriangles.Lower();
        for (; i <= aTriangles.Upper() && isTriangulationConsistent; ++i)
        {
          const Poly_Triangle& aTriangle = aTriangles(i);
          Standard_Integer n[3];
          aTriangle.Get(n[0], n[1], n[2]);
          for (Standard_Integer j = 0; j < 3 && isTriangulationConsistent; ++j)
            isTriangulationConsistent = (n[j] >= 1 && n[j] <= aNodesNb);
        }

        if (isTriangulationConsistent)
          return Standard_False;
      }
    }
  }

  // Nullify edges
  TopExp_Explorer aEdgeIt(theFace, TopAbs_EDGE);
  for (; aEdgeIt.More(); aEdgeIt.Next())
  {
    const TopoDS_Edge& aEdge = TopoDS::Edge(aEdgeIt.Current());
    BRepMesh_ShapeTool::NullifyEdge(aEdge, aTriangulation, aLoc);
  }

  BRepMesh_ShapeTool::NullifyFace(theFace);
  return Standard_True;
}

//=======================================================================
//function : update(face)
//purpose  : 
//=======================================================================
void BRepMesh_IncrementalMesh::update(const TopoDS_Face& theFace)
{
  if (!toBeMeshed(theFace, Standard_True))
    return;

  myModified = Standard_True;
  Standard_Integer aStatus = myMesh->Add(theFace);

  myStatus |= aStatus;
  if (aStatus != BRepMesh_ReMesh)
    return;

  BRepMesh::MapOfShape aUsedFaces;
  aUsedFaces.Add(theFace);

  const TopTools_IndexedDataMapOfShapeListOfShape& aMapOfSharedFaces = 
    myMesh->SharedFaces();

  TopExp_Explorer aEdgeIt(theFace, TopAbs_EDGE);
  for (; aEdgeIt.More(); aEdgeIt.Next())
  {
    const TopoDS_Edge& aEdge = TopoDS::Edge(aEdgeIt.Current());
    if (aMapOfSharedFaces.FindIndex(aEdge) == 0)
      continue;
     
    const TopTools_ListOfShape& aSharedFaces = aMapOfSharedFaces.FindFromKey(aEdge);
    TopTools_ListIteratorOfListOfShape aSharedFaceIt(aSharedFaces);
    for (; aSharedFaceIt.More(); aSharedFaceIt.Next())
    {
      const TopoDS_Face& aFace = TopoDS::Face(aSharedFaceIt.Value());
      if (aUsedFaces.Contains(aFace))
        continue;

      aUsedFaces.Add(aFace);
      toBeMeshed(aFace, Standard_False);

      myStatus |= myMesh->Add(aFace);
    }
  }
}

//=======================================================================
//function : commit
//purpose  : 
//=======================================================================
void BRepMesh_IncrementalMesh::commit()
{
  NCollection_Vector<TopoDS_Face>::Iterator aFaceIt(myFaces);
  for (; aFaceIt.More(); aFaceIt.Next())
    commitEdges(aFaceIt.Value());

  discretizeFreeEdges();
}

//=======================================================================
//function : commitEdges
//purpose  : 
//=======================================================================
void BRepMesh_IncrementalMesh::commitEdges(const TopoDS_Face& theFace)
{
  TopoDS_Face aFace = theFace;
  aFace.Orientation(TopAbs_FORWARD);

  Handle(BRepMesh_FaceAttribute) aFaceAttribute;
  if (!myMesh->GetFaceAttribute(aFace, aFaceAttribute))
    return;

  if (!aFaceAttribute->IsValid())
  {
    myStatus |= aFaceAttribute->GetStatus();
    return;
  }

  TopLoc_Location aLoc;
  Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation(aFace, aLoc);

  if (aTriangulation.IsNull())
    return;

  try
  {
    OCC_CATCH_SIGNALS

    // Store discretization of edges
    BRepMesh::HDMapOfShapePairOfPolygon& aInternalEdges = aFaceAttribute->ChangeInternalEdges();
    BRepMesh::DMapOfShapePairOfPolygon::Iterator aEdgeIt(*aInternalEdges);
    for (; aEdgeIt.More(); aEdgeIt.Next())
    {
      const TopoDS_Edge& aEdge = TopoDS::Edge(aEdgeIt.Key());
      const BRepMesh_PairOfPolygon& aPolyPair = aEdgeIt.Value();
      const Handle(Poly_PolygonOnTriangulation)& aPolygon1 = aPolyPair.First();
      const Handle(Poly_PolygonOnTriangulation)& aPolygon2 = aPolyPair.Last();

      if (aPolygon1 == aPolygon2)
        BRepMesh_ShapeTool::UpdateEdge(aEdge, aPolygon1, aTriangulation, aLoc);
      else
        BRepMesh_ShapeTool::UpdateEdge(aEdge, aPolygon1, aPolygon2, aTriangulation, aLoc);
    }
  }
  catch (Standard_Failure)
  {
    myStatus |= BRepMesh_Failure;
  }
}

//=======================================================================
//function : Discret
//purpose  :
//=======================================================================
Standard_Integer BRepMesh_IncrementalMesh::Discret(
  const TopoDS_Shape&    theShape,
  const Standard_Real    theDeflection,
  const Standard_Real    theAngle,
  BRepMesh_DiscretRoot* &theAlgo)
{
  BRepMesh_IncrementalMesh* anAlgo = new BRepMesh_IncrementalMesh();
  anAlgo->ChangeParameters().Deflection = theDeflection;
  anAlgo->ChangeParameters().Angle = theAngle;
  anAlgo->ChangeParameters().InParallel = IS_IN_PARALLEL;
  anAlgo->SetShape     (theShape);
  theAlgo = anAlgo;
  return 0; // no error
}

//=======================================================================
//function : IsParallelDefault
//purpose  :
//=======================================================================
Standard_Boolean BRepMesh_IncrementalMesh::IsParallelDefault()
{
  return IS_IN_PARALLEL;
}

//=======================================================================
//function : Discret
//purpose  :
//=======================================================================
void BRepMesh_IncrementalMesh::SetParallelDefault(
  const Standard_Boolean theInParallel)
{
  IS_IN_PARALLEL = theInParallel;
}

//! Export Mesh Plugin entry function
DISCRETPLUGIN(BRepMesh_IncrementalMesh)
