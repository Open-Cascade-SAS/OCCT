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

#include <Adaptor3d_IsoCurve.hxx>
#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
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
#include <IVtkOCC_ShapeMesher.hxx>
#include <NCollection_Array1.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <Prs3d.hxx>
#include <Prs3d_Drawer.hxx>
#include <Standard_ErrorHandler.hxx>
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
    myDeflection = Prs3d::GetDeflection (GetShapeObj()->GetShape(), aDefDrawer);
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
  catch (Standard_Failure)
  { }
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

  TopExp_Explorer anEdgeIter (GetShapeObj()->GetShape(), TopAbs_EDGE);
  for (; anEdgeIter.More(); anEdgeIter.Next())
  {
    const TopoDS_Edge& anOcctEdge = TopoDS::Edge (anEdgeIter.Current());
    aNbFaces = anEdgesMap.FindFromKey (anOcctEdge).Extent();
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
      aType = MT_SharedEdge;
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
    catch (Standard_Failure)
    { }
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
// Function : FindLimits
// Purpose  : Static internal function, finds parametrical limits of the curve.
//! @param [in] theCurve 3D curve adaptor used to retrieve the curve geometry
//! @param [in] theLimit maximum allowed absolute parameter value
//! @param [out] theFirst minimum parameter value for the curve
//! @param [out] theLast maximum parameter value for the curve
//================================================================
static void FindLimits (const Adaptor3d_Curve& theCurve,
                        const Standard_Real&   theLimit,
                        Standard_Real&         theFirst,
                        Standard_Real&         theLast)
{
  theFirst = Max(theCurve.FirstParameter(), theFirst);
  theLast  = Min(theCurve.LastParameter(), theLast);
  Standard_Boolean isFirstInf = Precision::IsNegativeInfinite (theFirst);
  Standard_Boolean isLastInf  = Precision::IsPositiveInfinite (theLast);

  if (isFirstInf || isLastInf)
  {
    gp_Pnt aP1, aP2;
    Standard_Real aDelta = 1;
    if (isFirstInf && isLastInf)
    {
      do
      {
        aDelta *= 2;
        theFirst = - aDelta;
        theLast  =   aDelta;
        theCurve.D0 (theFirst, aP1);
        theCurve.D0 (theLast, aP2);
      } while (aP1.Distance (aP2) < theLimit);
    }
    else if (isFirstInf)
    {
      theCurve.D0 (theLast, aP2);
      do {
        aDelta *= 2;
        theFirst = theLast - aDelta;
        theCurve.D0 (theFirst, aP1);
      } while (aP1.Distance(aP2) < theLimit);
    }
    else if (isLastInf)
    {
      theCurve.D0 (theFirst, aP1);
      do
      {
        aDelta *= 2;
        theLast = theFirst + aDelta;
        theCurve.D0 (theLast, aP2);
      } while (aP1.Distance (aP2) < theLimit);
    }
  }
}

//================================================================
// Function : FindLimits
// Purpose  :Static helper function, builds a discrete representation
//! (sequence of points) for the given curve.
//!
//! @param [in] theCurve 3D curve adaptor used to retrieve the curve geometry
//! @param [in] theDeflection absolute deflection value
//! @param [in] theAngle deviation angle value
//! @param [in] theU1 minimal curve parameter value
//! @param [in] theU2 maximal curve parameter value
//! @param [out] thePoints the container for generated polyline
//================================================================
static void DrawCurve (Adaptor3d_Curve&    theCurve,
                       const Standard_Real theDeflection,
                       const Standard_Real theAngle,
                       const Standard_Real theU1,
                       const Standard_Real theU2,
                       IVtk_Polyline&      thePoints)
{
  switch (theCurve.GetType())
  {
  case GeomAbs_Line:
    {
      gp_Pnt aPnt = theCurve.Value(theU1);
      thePoints.Append (aPnt);

      aPnt = theCurve.Value(0.5 * (theU1 + theU2));
      thePoints.Append (aPnt);

      aPnt = theCurve.Value (theU2);
      thePoints.Append(aPnt);
   }
    break;
  default:
    {
      Standard_Integer aNbInter = theCurve.NbIntervals(GeomAbs_C1);
      Standard_Integer anI, aJ;
      TColStd_Array1OfReal aParams(1, aNbInter+1);
      theCurve.Intervals(aParams, GeomAbs_C1);
      Standard_Real anU1, anU2;
      Standard_Integer NumberOfPoints;

      for (aJ = 1; aJ <= aNbInter; aJ++)
      {
        anU1 = aParams (aJ); anU2 = aParams (aJ + 1);
        if (anU2 > anU1 && anU1 < anU2)
        {
          anU1 = Max(anU1, anU1);
          anU2 = Min(anU2, anU2);

          GCPnts_TangentialDeflection anAlgo (theCurve, anU1, anU2, theAngle, theDeflection);
          NumberOfPoints = anAlgo.NbPoints();

          if (NumberOfPoints > 0)
          {
            for (anI = 1; anI < NumberOfPoints; anI++)
            {
              thePoints.Append(anAlgo.Value (anI));
            }
            if (aJ == aNbInter)
            {
              thePoints.Append (anAlgo.Value (NumberOfPoints));
            }
          }
        }
      }
    }
  }
}

//================================================================
// Function : buildIsoLines
// Purpose  : 
//================================================================
void IVtkOCC_ShapeMesher::buildIsoLines (const Handle(BRepAdaptor_HSurface)& theFace,
                                         const Standard_Boolean          theIsDrawUIso,
                                         const Standard_Boolean          theIsDrawVIso,
                                         const Standard_Integer          theNBUiso,
                                         const Standard_Integer          theNBViso,
                                         IVtk_PolylineList&              thePolylines)
{
  Standard_Real anUF, anUL, aVF, aVL;
  anUF = theFace->FirstUParameter();
  anUL = theFace->LastUParameter();
  aVF = theFace->FirstVParameter();
  aVL = theFace->LastVParameter();

  // Restrict maximal parameter value
  // in OCCT it's 5e+5 by default
  const Standard_Real aLimit = 5e+5;

  // compute bounds of the restriction
  Standard_Real anUMin, anUMax, aVMin, aVMax;
  Standard_Integer anI;

  anUMin = Max (anUF, -aLimit);
  anUMax = Min (anUL, aLimit);
  aVMin = Max (aVF, -aLimit);
  aVMax = Min (aVL, aLimit);

  // update min max for the hatcher.
  gp_Pnt2d aP1,aP2;
  gp_Pnt aDummyPnt;

  Standard_Real aDdefle = Max (anUMax - anUMin, aVMax - aVMin) * GetDeviationCoeff();
  TColgp_SequenceOfPnt2d aTabPoints;

  anUMin = aVMin = 1.e100;
  anUMax = aVMax = -1.e100;

  // Process the edges
  TopExp_Explorer aToolRst;
  TopoDS_Face aTopoFace (((BRepAdaptor_Surface*)&(theFace->Surface()))->Face());
  for (aToolRst.Init (aTopoFace, TopAbs_EDGE); aToolRst.More(); aToolRst.Next())
  {
    TopAbs_Orientation anOrient = aToolRst.Current().Orientation();
    // Skip INTERNAL and EXTERNAL edges
    if (anOrient == TopAbs_FORWARD || anOrient == TopAbs_REVERSED)
    {
      Standard_Real anU1, anU2;
      const Handle(Geom2d_Curve)& aCurve = 
        BRep_Tool::CurveOnSurface (TopoDS::Edge (aToolRst.Current()),
                                   aTopoFace,
                                   anU1, anU2);
      if (aCurve.IsNull())
      {
        continue;
      }

      Geom2dAdaptor_Curve aRCurve;
      aRCurve.Load (aCurve, anU1, anU2);
      if (aRCurve.GetType() != GeomAbs_Line)
      {
        GCPnts_QuasiUniformDeflection aUDP(aRCurve, aDdefle);
        if (aUDP.IsDone())
        {
          Standard_Integer NumberOfPoints = aUDP.NbPoints();
          if ( NumberOfPoints >= 2 )
          {
            aDummyPnt = aUDP.Value (1);
            aP2.SetCoord (aDummyPnt.X(), aDummyPnt.Y());
            anUMin = Min (aP2.X(), anUMin);
            anUMax = Max (aP2.X(), anUMax);
            aVMin = Min (aP2.Y(), aVMin);
            aVMax = Max (aP2.Y(), aVMax);
            for (anI = 2; anI <= NumberOfPoints; anI++)
            {
              aP1 = aP2;
              aDummyPnt = aUDP.Value (anI);
              aP2.SetCoord (aDummyPnt.X(), aDummyPnt.Y());
              anUMin = Min(aP2.X(), anUMin);
              anUMax = Max(aP2.X(), anUMax);
              aVMin = Min(aP2.Y(), aVMin);
              aVMax = Max(aP2.Y(), aVMax);

              if(anOrient == TopAbs_FORWARD )
              {
                //isobuild.Trim(P1,P2);
                aTabPoints.Append (aP1);
                aTabPoints.Append (aP2);
              }
              else
              {
                //isobuild.Trim(P2,P1);
                aTabPoints.Append (aP2);
                aTabPoints.Append (aP1);
              }
            }
          }
        }
        else
        {
          cout << "Cannot evaluate curve on surface"<<endl;
        }
      }
      else
      {
        anU1 = aRCurve.FirstParameter();
        anU2 = aRCurve.LastParameter();
        // MSV 17.08.06 OCC13144: U2 occured less than U1, to overcome it
        // ensure that distance U2-U1 is not greater than aLimit*2,
        // if greater then choose an origin and use aLimit to define
        // U1 and U2 anew
        Standard_Real aOrigin = 0.;
        if (!Precision::IsNegativeInfinite(anU1) || !Precision::IsPositiveInfinite (anU2))
        {
          if (Precision::IsNegativeInfinite (anU1))
          {
            aOrigin = anU2 - aLimit;
          }
          else if (Precision::IsPositiveInfinite (anU2))
          {
            aOrigin = anU1 + aLimit;
          }
          else
          {
            aOrigin = (anU1 + anU2) * 0.5;
          }
        }

        anU1 = Max (aOrigin - aLimit, anU1);
        anU2 = Min (aOrigin + aLimit, anU2);
        aP1 = aRCurve.Value (anU1);
        aP2 = aRCurve.Value (anU2);
        anUMin = Min(aP1.X(), anUMin);
        anUMax = Max(aP1.X(), anUMax);
        aVMin = Min(aP1.Y(), aVMin);
        aVMax = Max(aP1.Y(), aVMax);
        anUMin = Min(aP2.X(), anUMin);
        anUMax = Max(aP2.X(), anUMax);
        aVMin = Min(aP2.Y(), aVMin);
        aVMax = Max(aP2.Y(), aVMax);
        if(anOrient == TopAbs_FORWARD )
        {
         // isobuild.Trim(P1,P2);
          aTabPoints.Append (aP1);
          aTabPoints.Append (aP2);
        }
        else
        {
          //isobuild.Trim(P2,P1);
          aTabPoints.Append (aP2);
          aTabPoints.Append (aP1);
        }
      }
    }
  }

  // load the isos
  const Standard_Real anIntersectionTolerance = 1.e-5;
  Hatch_Hatcher anIsoBuild (anIntersectionTolerance, Standard_True );

  Standard_Boolean isUClosed = theFace->IsUClosed();
  Standard_Boolean isVClosed = theFace->IsVClosed();

  if (!isUClosed)
  {
    anUMin = anUMin + (anUMax - anUMin) / 1000.0;
    anUMax = anUMax - (anUMax - anUMin) /1000.0;
  }

  if (!isVClosed)
  {
    aVMin = aVMin + (aVMax - aVMin) /1000.0;
    aVMax = aVMax - (aVMax - aVMin) /1000.0;
  }

  if (theIsDrawUIso)
  {
    if (theNBUiso > 0)
    {
      isUClosed = Standard_False;
      Standard_Real aDu= isUClosed ? (anUMax - anUMin) / theNBUiso : (anUMax - anUMin) / (1 + theNBUiso);
      for (anI = 1; anI <= theNBUiso; anI++)
      {
        anIsoBuild.AddXLine (anUMin + aDu*anI);
      }
    }
  }
  if (theIsDrawVIso)
  {
    if (theNBViso > 0)
    {
      isVClosed = Standard_False;
      Standard_Real aDv= isVClosed ? (aVMax - aVMin) / theNBViso : (aVMax - aVMin) / (1 + theNBViso);
      for (anI = 1; anI <= theNBViso; anI++)
      {
        anIsoBuild.AddYLine (aVMin + aDv*anI);
      }
    }
  }

  Standard_Integer aLength = aTabPoints.Length();
  for (anI = 1; anI <= aLength; anI += 2)
  {
    anIsoBuild.Trim (aTabPoints (anI),aTabPoints (anI + 1));
  }

  // Create the polylines for isos
  Adaptor3d_IsoCurve anIso;
  anIso.Load(theFace);
  Handle(Geom_Curve) aBCurve;
  const BRepAdaptor_Surface& aBSurf = *(BRepAdaptor_Surface*)&(theFace->Surface());
  GeomAbs_SurfaceType aType = theFace->GetType();

  Standard_Integer aNumberOfLines = anIsoBuild.NbLines();
  Handle(Geom_Surface) aGeomSurf;
  if (aType == GeomAbs_BezierSurface)
  {
    aGeomSurf = aBSurf.Bezier();
  }
  else if (aType == GeomAbs_BSplineSurface)
  {
    aGeomSurf = aBSurf.BSpline();
  }

  Standard_Real aDeflection = GetDeflection();
  Standard_Real anAngle     = GetDeviationAngle();
  for (anI = 1; anI <= aNumberOfLines; anI++)
  {
    Standard_Integer aNumberOfIntervals = anIsoBuild.NbIntervals(anI);
    Standard_Real aCoord = anIsoBuild.Coordinate(anI);
    for (Standard_Integer aJ = 1; aJ <= aNumberOfIntervals; aJ++)
    {
      Standard_Real aB1 = anIsoBuild.Start (anI, aJ);
      Standard_Real aB2 = anIsoBuild.End(anI, aJ);

      if (!aGeomSurf.IsNull())
      {
        if (anIsoBuild.IsXLine (anI))
        {
          aBCurve = aGeomSurf->UIso (aCoord);
        }
        else
        {
          aBCurve = aGeomSurf->VIso (aCoord);
        }

        GeomAdaptor_Curve aGeomCurve (aBCurve);
        FindLimits (aGeomCurve, aLimit, aB1, aB2);
        if (aB2 - aB1 > Precision::Confusion())
        {
          IVtk_Polyline aPoints;
          DrawCurve (aGeomCurve, aDeflection, anAngle, aB1, aB2, aPoints);
          thePolylines.Append (aPoints);
        }
      }
      else
      {
        if (anIsoBuild.IsXLine (anI))
        {
          anIso.Load (GeomAbs_IsoU, aCoord, aB1, aB2);
        }
        else
        {
          anIso.Load (GeomAbs_IsoV, aCoord, aB1, aB2);
        }
        FindLimits (anIso, aLimit, aB1, aB2);
        if (aB2 - aB1>Precision::Confusion())
        {
          IVtk_Polyline aPoints;
          DrawCurve (anIso, aDeflection, anAngle, aB1, aB2, aPoints);
          thePolylines.Append (aPoints);
        }
      }
    }
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
  TopExp_Explorer anEdgeIter (aFaceToMesh, TopAbs_EDGE );
  for (; anEdgeIter.More(); anEdgeIter.Next())
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

  BRepAdaptor_Surface aSurf;
  aSurf.Initialize (aFaceToMesh);
  Handle(BRepAdaptor_HSurface) aSurfAdaptor = new BRepAdaptor_HSurface (aSurf);

  IVtk_PolylineList aPolylines;
  gp_Trsf aDummyTrsf;

  // Building U isolines
  // Introducing a local scope here to simplify variable naming
  {
    buildIsoLines (aSurfAdaptor,
                   myNbIsos[0] != 0,
                   Standard_False,
                   myNbIsos[0],
                   0,
                   aPolylines);

    IVtk_PolylineList::Iterator anIt (aPolylines);
    for (; anIt.More(); anIt.Next())
    {
      const IVtk_Polyline& aPntSeq = anIt.Value();
      Standard_Integer aNbNodes = aPntSeq.Length();
      TColgp_Array1OfPnt aPoints (1, aNbNodes);
      for (Standard_Integer aJ = 1; aJ <= aNbNodes; aJ++)
      {
        aPoints.SetValue (aJ, aPntSeq.Value(aJ));
      }

      TColStd_Array1OfInteger aPointIds (1, aNbNodes);
      for (Standard_Integer anI = 1; anI <= aNbNodes; anI++)
      {
        aPointIds.SetValue (anI, anI);
      }

      processPolyline (aNbNodes,
                       aPoints,
                       aPointIds,
                       theShapeId,
                       true,
                       aDummyTrsf, 
                       MT_IsoLine);
    }
  }

  // Building V isolines
  {
    aPolylines.Clear();
    buildIsoLines (aSurfAdaptor,
                   Standard_False,
                   myNbIsos[1] != 0,
                   0,
                   myNbIsos[1],
                   aPolylines);

    IVtk_PolylineList::Iterator anIt (aPolylines);
    for (; anIt.More(); anIt.Next())
    {
      const IVtk_Polyline& aPntSeq = anIt.Value();
      Standard_Integer aNbNodes = aPntSeq.Length();
      TColgp_Array1OfPnt aPoints (1, aNbNodes);
      for (int aJ = 1; aJ <= aNbNodes; aJ++)
      {
        aPoints.SetValue (aJ, aPntSeq.Value (aJ));
      }

      TColStd_Array1OfInteger aPointIds (1, aNbNodes);
      for (Standard_Integer anI = 1; anI <= aNbNodes; anI++)
      {
        aPointIds.SetValue (anI, anI);
      }

      processPolyline (aNbNodes,
                       aPoints,
                       aPointIds,
                       theShapeId,
                       true,
                       aDummyTrsf, 
                       MT_IsoLine);
    }
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
