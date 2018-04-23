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

#include <BRepMesh_FastDiscretFace.hxx>

#include <BRepMesh_PairOfPolygon.hxx>
#include <BRepMesh_ShapeTool.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <Adaptor3d_IsoCurve.hxx>
#include <Adaptor3d_HCurve.hxx>

#include <BRep_ListIteratorOfListOfPointRepresentation.hxx>
#include <BRep_PointRepresentation.hxx>
#include <BRep_TVertex.hxx>
#include <BRep_Tool.hxx>

#include <GeomLib.hxx>
#include <Geom_Surface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <GCPnts_AbscissaPoint.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TopTools_DataMapOfShapeReal.hxx>

#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>

#include <NCollection_Map.hxx>
#include <Bnd_Box2d.hxx>

#include <algorithm>

//#define DEBUG_MESH "mesh.tcl"
#ifdef DEBUG_MESH
#include <fstream>
#endif


IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_FastDiscretFace,Standard_Transient)

static Standard_Real FUN_CalcAverageDUV(TColStd_Array1OfReal& P, const Standard_Integer PLen)
{
  Standard_Integer i, j, n = 0;
  Standard_Real p, result = 0.;

  for(i = 1; i <= PLen; i++)
  {
    // Sort
    for(j = i + 1; j <= PLen; j++)
    {
      if(P(i) > P(j))
      {
        p = P(i);
        P(i) = P(j);
        P(j) = p;
      }
    }
    // Accumulate
    if (i != 1)
    {
      p = Abs(P(i) - P(i-1));
      if(p > 1.e-7)
      {
        result += p;
        n++;
      }
    }
  }
  return (n? (result / (Standard_Real) n) : -1.);
}

namespace
{
  Standard_Real deflectionOfSegment (
    const gp_Pnt& theFirstPoint,
    const gp_Pnt& theLastPoint,
    const gp_Pnt& theMidPoint)
  {
    // 23.03.2010 skl for OCC21645 - change precision for comparison
    if (theFirstPoint.SquareDistance (theLastPoint) > Precision::SquareConfusion ())
    {
      gp_Lin aLin (theFirstPoint, gp_Dir (gp_Vec (theFirstPoint, theLastPoint)));
      return aLin.Distance (theMidPoint);
    }

    return theFirstPoint.Distance (theMidPoint);
  }

  Standard_Boolean IsCompexSurface (const GeomAbs_SurfaceType theType)
  {
    return (
      theType != GeomAbs_Sphere   &&
      theType != GeomAbs_Cylinder &&
      theType != GeomAbs_Cone     &&
      theType != GeomAbs_Torus);
  }

  //! Auxiliary class used to extract geometrical parameters of fixed TopoDS_Vertex.
  class FixedVExplorer
  {
  public:

    DEFINE_STANDARD_ALLOC

    FixedVExplorer(const TopoDS_Vertex& theVertex)
      : myVertex(theVertex)
    {
    }

    const TopoDS_Vertex& Vertex() const
    {
      return myVertex;
    }

    Standard_Boolean IsSameUV() const
    {
      return Standard_False;
    }

    TopoDS_Vertex SameVertex() const
    {
      return TopoDS_Vertex();
    }

    gp_Pnt Point() const
    {
      return BRep_Tool::Pnt(myVertex);
    }

  private:

    void operator =(const FixedVExplorer& /*theOther*/)
    {
    }

  private:
    const TopoDS_Vertex& myVertex;
  };

  void ComputeErrFactors (const Standard_Real               theDeflection, 
                          const Handle(Adaptor3d_HSurface)& theFace,
                          Standard_Real&                    theErrFactorU,
                          Standard_Real&                    theErrFactorV)
  {
    theErrFactorU = theDeflection * 10.;
    theErrFactorV = theDeflection * 10.;

    switch (theFace->GetType ())
    {
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
      break;

    case GeomAbs_SurfaceOfExtrusion:
    case GeomAbs_SurfaceOfRevolution:
      {
        Handle (Adaptor3d_HCurve) aCurve = theFace->BasisCurve ();
        if (aCurve->GetType () == GeomAbs_BSplineCurve && aCurve->Degree () > 2)
        {
          theErrFactorV /= (aCurve->Degree () * aCurve->NbKnots ());
        }
        break;
      }
    case GeomAbs_BezierSurface:
      {
        if (theFace->UDegree () > 2)
        {
          theErrFactorU /= (theFace->UDegree ());
        }
        if (theFace->VDegree () > 2)
        {
          theErrFactorV /= (theFace->VDegree ());
        }
        break;
      }
    case GeomAbs_BSplineSurface:
      {
        if (theFace->UDegree () > 2)
        {
          theErrFactorU /= (theFace->UDegree () * theFace->NbUKnots ());
        }
        if (theFace->VDegree () > 2)
        {
          theErrFactorV /= (theFace->VDegree () *  theFace->NbVKnots ());
        }
        break;
      }

    case GeomAbs_Plane:
    default:
      theErrFactorU = theErrFactorV = 1.;
    }
  }

  void AdjustCellsCounts (const Handle(Adaptor3d_HSurface)& theFace,
                          const Standard_Integer            theNbVertices,
                          Standard_Integer&                 theCellsCountU,
                          Standard_Integer&                 theCellsCountV)
  {
    const GeomAbs_SurfaceType aType = theFace->GetType ();
    if (aType == GeomAbs_OtherSurface)
    {
      // fallback to the default behavior
      theCellsCountU = theCellsCountV = -1;
      return;
    }

    Standard_Real aSqNbVert = theNbVertices;
    if (aType == GeomAbs_Plane)
    {
      theCellsCountU = theCellsCountV = (Standard_Integer)Ceiling (Pow (2, Log10 (aSqNbVert)));
    }
    else if (aType == GeomAbs_Cylinder || aType == GeomAbs_Cone)
    {
      theCellsCountV = (Standard_Integer)Ceiling (Pow (2, Log10 (aSqNbVert)));
    }
    else if (aType == GeomAbs_SurfaceOfExtrusion || aType == GeomAbs_SurfaceOfRevolution)
    {
      Handle (Adaptor3d_HCurve) aCurve = theFace->BasisCurve ();
      if (aCurve->GetType () == GeomAbs_Line ||
          (aCurve->GetType () == GeomAbs_BSplineCurve && aCurve->Degree () < 2))
      {
        // planar, cylindrical, conical cases
        if (aType == GeomAbs_SurfaceOfExtrusion)
          theCellsCountU = (Standard_Integer)Ceiling (Pow (2, Log10 (aSqNbVert)));
        else
          theCellsCountV = (Standard_Integer)Ceiling (Pow (2, Log10 (aSqNbVert)));
      }
      if (aType == GeomAbs_SurfaceOfExtrusion)
      {
        // V is always a line
        theCellsCountV = (Standard_Integer)Ceiling (Pow (2, Log10 (aSqNbVert)));
      }
    }
    else if (aType == GeomAbs_BezierSurface || aType == GeomAbs_BSplineSurface)
    {
      if (theFace->UDegree () < 2)
      {
        theCellsCountU = (Standard_Integer)Ceiling (Pow (2, Log10 (aSqNbVert)));
      }
      if (theFace->VDegree () < 2)
      {
        theCellsCountV = (Standard_Integer)Ceiling (Pow (2, Log10 (aSqNbVert)));
      }
    }

    theCellsCountU = Max (theCellsCountU, 2);
    theCellsCountV = Max (theCellsCountV, 2);
  }
}


//=======================================================================
//function : BRepMesh_FastDiscretFace
//purpose  :
//=======================================================================
BRepMesh_FastDiscretFace::BRepMesh_FastDiscretFace(
  const Standard_Real    theAngle,
  const Standard_Real    theMinSize,
  const Standard_Boolean isInternalVerticesMode,
  const Standard_Boolean isControlSurfaceDeflection)
: myAngle(theAngle),
  myInternalVerticesMode(isInternalVerticesMode),
  myMinSize(theMinSize),
  myIsControlSurfaceDeflection(isControlSurfaceDeflection)
{
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscretFace::Perform(const Handle(BRepMesh_FaceAttribute)& theAttribute)
{
  add(theAttribute);
  commitSurfaceTriangulation();
}

//=======================================================================
//function : initDataStructure
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscretFace::initDataStructure()
{
  const Standard_Real aTolU = myAttribute->ToleranceU();
  const Standard_Real aTolV = myAttribute->ToleranceV();
  const Standard_Real uCellSize = 14.0 * aTolU;
  const Standard_Real vCellSize = 14.0 * aTolV;

  const Standard_Real deltaX = myAttribute->GetDeltaX();
  const Standard_Real deltaY = myAttribute->GetDeltaY();

  Handle(NCollection_IncAllocator) aAllocator = 
    new NCollection_IncAllocator(BRepMesh::MEMORY_BLOCK_SIZE_HUGE);
  myStructure = new BRepMesh_DataStructureOfDelaun(aAllocator);
  myStructure->Data()->SetCellSize ( uCellSize / deltaX, vCellSize / deltaY);
  myStructure->Data()->SetTolerance( aTolU     / deltaX, aTolV     / deltaY);

  myAttribute->ChangeStructure() = myStructure;
  myAttribute->ChangeSurfacePoints() = new BRepMesh::DMapOfIntegerPnt(1, aAllocator);
  myAttribute->ChangeSurfaceVertices()= new BRepMesh::DMapOfVertexInteger(1, aAllocator);

  // Check the necessity to fill the map of parameters
  const Handle(BRepAdaptor_HSurface)& gFace = myAttribute->Surface();
  GeomAbs_SurfaceType thetype = gFace->GetType();
  const Standard_Boolean isBSpline = (thetype == GeomAbs_BezierSurface ||
                                      thetype == GeomAbs_BSplineSurface);
  const Standard_Boolean useUVParam = (thetype == GeomAbs_Torus || IsCompexSurface (thetype));

  Handle(NCollection_BaseAllocator) aBaseAlloc = aAllocator;
  myUParam.Clear (aBaseAlloc);
  myVParam.Clear (aBaseAlloc);

  // essai de determination de la longueur vraie:
  // akm (bug OCC16) : We must calculate these measures in non-singular
  //     parts of face. Let`s try to compute average value of three
  //     (umin, (umin+umax)/2, umax), and respectively for v.
  //                 vvvvv
  //Standard_Real longu = 0.0, longv = 0.0; //, last , first;
  //gp_Pnt P11, P12, P21, P22, P31, P32;
  BRepMesh::HVectorOfVertex& aBoundaryNodes = myAttribute->ChangeMeshNodes();
  BRepMesh::VectorOfVertex::Iterator aNodesIt(*aBoundaryNodes);
  for (; aNodesIt.More(); aNodesIt.Next())
  {
    BRepMesh_Vertex& aNode = aNodesIt.ChangeValue();
    gp_XY aPnt2d = aNode.Coord();

    if (useUVParam)
    {
      myUParam.Add(aPnt2d.X());
      myVParam.Add(aPnt2d.Y());
    }

    aNode.ChangeCoord() = myAttribute->Scale(aPnt2d, Standard_True);
    myStructure->AddNode(aNode, Standard_True);
  }
  aBoundaryNodes.Nullify();

  if (isBSpline)
  {
    const Standard_Real aRange[2][2] = {
      {myAttribute->GetUMin(), myAttribute->GetUMax()},
      {myAttribute->GetVMin(), myAttribute->GetVMax()}
    };

    const GeomAbs_Shape aContinuity = GeomAbs_CN;
    for (Standard_Integer i = 0; i < 2; ++i)
    {
      const Standard_Boolean isU = (i == 0);
      const Standard_Integer aIntervalsNb = isU ?
        gFace->NbUIntervals(aContinuity) :
        gFace->NbVIntervals(aContinuity);

      BRepMesh::IMapOfReal& aParams = isU ? myUParam : myVParam;
      if (aIntervalsNb < aParams.Size())
        continue;

      TColStd_Array1OfReal aIntervals(1, aIntervalsNb + 1);
      if (isU)
        gFace->UIntervals(aIntervals, aContinuity);
      else
        gFace->VIntervals(aIntervals, aContinuity);

      for (Standard_Integer j = 1; j <= aIntervals.Upper(); ++j)
      {
        const Standard_Real aParam = aIntervals(j);
        if (aParam > aRange[i][0] && aParam < aRange[i][1])
          aParams.Add(aParam);
      }
    }
  }

  //////////////////////////////////////////////////////////// 
  //add internal vertices after self-intersection check
  if ( myInternalVerticesMode )
  {
    TopExp_Explorer anExplorer(myAttribute->Face(), TopAbs_VERTEX, TopAbs_EDGE);
    for ( ; anExplorer.More(); anExplorer.Next() )
      add(TopoDS::Vertex(anExplorer.Current()));
  }

  const BRepMesh::HDMapOfShapePairOfPolygon& aEdges = myAttribute->ChangeInternalEdges();
  TopExp_Explorer aWireIt(myAttribute->Face(), TopAbs_WIRE);
  for (; aWireIt.More(); aWireIt.Next())
  {
    TopExp_Explorer aEdgeIt(aWireIt.Current(), TopAbs_EDGE);
    for (; aEdgeIt.More(); aEdgeIt.Next())
    {
      const TopoDS_Edge& aEdge = TopoDS::Edge(aEdgeIt.Current());
      BRepMesh_PairOfPolygon aPair;
      if (!aEdges->Find(aEdge, aPair))
        continue;

      TopAbs_Orientation aOri = aEdge.Orientation();
      const Handle(Poly_PolygonOnTriangulation)& aPolygon = 
        aOri == TopAbs_REVERSED ? aPair.Last() : aPair.First();

      const TColStd_Array1OfInteger& aIndices = aPolygon->Nodes();
      const Standard_Integer aNodesNb = aPolygon->NbNodes();

      Standard_Integer aPrevId = aIndices(1);
      for (Standard_Integer i = 2; i <= aNodesNb; ++i)
      {
        const Standard_Integer aCurId = aIndices(i);
        addLinkToMesh(aPrevId, aCurId, aOri);
        aPrevId = aCurId;
      }
    }
  }
}

//=======================================================================
//function : addLinkToMesh
//purpose  :
//=======================================================================
void BRepMesh_FastDiscretFace::addLinkToMesh(
  const Standard_Integer   theFirstNodeId,
  const Standard_Integer   theLastNodeId,
  const TopAbs_Orientation theOrientation)
{
  if (theOrientation == TopAbs_FORWARD)
    myStructure->AddLink(BRepMesh_Edge(theFirstNodeId, theLastNodeId, BRepMesh_Frontier));
  else if (theOrientation == TopAbs_REVERSED)
    myStructure->AddLink(BRepMesh_Edge(theLastNodeId, theFirstNodeId, BRepMesh_Frontier));
  else if (theOrientation == TopAbs_INTERNAL)
    myStructure->AddLink(BRepMesh_Edge(theFirstNodeId, theLastNodeId, BRepMesh_Fixed));
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscretFace::add(const Handle(BRepMesh_FaceAttribute)& theAttribute)
{
  if (!theAttribute->IsValid() || theAttribute->ChangeMeshNodes()->IsEmpty())
    return;

  myAttribute = theAttribute;
  initDataStructure();

  BRepMesh::HIMapOfInteger& aVertexEdgeMap = myAttribute->ChangeVertexEdgeMap();
  Standard_Integer nbVertices = aVertexEdgeMap->Extent();
  BRepMesh::Array1OfInteger tabvert_corr(1, nbVertices);
  for ( Standard_Integer i = 1; i <= nbVertices; ++i )
    tabvert_corr(i) = i;

  Handle (Adaptor3d_HSurface) aSurface (myAttribute->Surface ());
  GeomAbs_SurfaceType aType = aSurface->GetType ();

  while (aType == GeomAbs_OffsetSurface)
  {
    aSurface = aSurface->BasisSurface ();
    aType = aSurface->GetType ();
  }

  // For better meshing performance we try to estimate the acceleration circles grid structure sizes:
  // For each parametric direction (U, V) we estimate firstly an approximate distance between the future points -
  // this estimation takes into account the required face deflection and the complexity of the face.
  // Particularly, the complexity of the faces based on BSpline curves and surfaces requires much more points.
  // At the same time, for planar faces and linear parts of the arbitrary surfaces usually no intermediate points
  // are necessary.
  // The general idea for each parametric direction:
  // cells_count = 2 ^ log10 ( estimated_points_count )
  // For linear parametric direction we fall back to the initial vertex count:
  // cells_count = 2 ^ log10 ( initial_vertex_count )

  Standard_Real anErrFactorU, anErrFactorV;
  ComputeErrFactors(myAttribute->GetDefFace (), aSurface, anErrFactorU, anErrFactorV);

  Standard_Integer aCellsCountU, aCellsCountV;
  if (aType == GeomAbs_Torus)
  {
    aCellsCountU = (Standard_Integer)Ceiling(Pow(2, Log10(
      (myAttribute->GetUMax() - myAttribute->GetUMin()) / myAttribute->GetDeltaX())));
    aCellsCountV = (Standard_Integer)Ceiling(Pow(2, Log10(
      (myAttribute->GetVMax() - myAttribute->GetVMin()) / myAttribute->GetDeltaY())));
  }
  else if (aType == GeomAbs_Cylinder)
  {
    aCellsCountU = (Standard_Integer)Ceiling(Pow(2, Log10(
      (myAttribute->GetUMax() - myAttribute->GetUMin()) / myAttribute->GetDeltaX() /
      (myAttribute->GetVMax() - myAttribute->GetVMin()))));
    aCellsCountV = (Standard_Integer)Ceiling(Pow(2, Log10(
      (myAttribute->GetVMax() - myAttribute->GetVMin()) / anErrFactorV)));
  }
  else
  {
    aCellsCountU = (Standard_Integer)Ceiling(Pow(2, Log10(
      (myAttribute->GetUMax() - myAttribute->GetUMin()) / myAttribute->GetDeltaX() / anErrFactorU)));
    aCellsCountV = (Standard_Integer)Ceiling(Pow(2, Log10(
      (myAttribute->GetVMax() - myAttribute->GetVMin()) / myAttribute->GetDeltaY() / anErrFactorV)));
  }

  AdjustCellsCounts(aSurface, nbVertices, aCellsCountU, aCellsCountV);

  BRepMesh_Delaun trigu(myStructure, tabvert_corr, aCellsCountU, aCellsCountV);

  //removed all free edges from triangulation
  const Standard_Integer nbLinks = myStructure->NbLinks();
  for( Standard_Integer i = 1; i <= nbLinks; i++ ) 
  {
    if( myStructure->ElementsConnectedTo(i).Extent() < 1 )
    {
      BRepMesh_Edge& anEdge = (BRepMesh_Edge&)trigu.GetEdge(i);
      if ( anEdge.Movability() == BRepMesh_Deleted )
        continue;

      anEdge.SetMovability(BRepMesh_Free);
      myStructure->RemoveLink(i);
    }
  }

  Standard_Boolean rajout = 
    (aType == GeomAbs_Sphere || aType == GeomAbs_Torus);

  // Check the necessity to fill the map of parameters
  const Standard_Boolean useUVParam = (aType == GeomAbs_Torus         ||
                                       aType == GeomAbs_BezierSurface ||
                                       aType == GeomAbs_BSplineSurface);

  const Standard_Real umax = myAttribute->GetUMax();
  const Standard_Real umin = myAttribute->GetUMin();
  const Standard_Real vmax = myAttribute->GetVMax();
  const Standard_Real vmin = myAttribute->GetVMin();

  Standard_Boolean isaline = 
    ((umax - umin) < Precision::PConfusion() || 
     (vmax - vmin) < Precision::PConfusion());

  Standard_Real aDef = -1;
  if ( !isaline && myStructure->ElementsOfDomain().Extent() > 0 )
  {
    if (!rajout)
    {
      // compute maximal deflection
      aDef = control(trigu, Standard_True);
      rajout = (aDef > myAttribute->GetDefFace() || aDef < 0.);
    }
    if (aType != GeomAbs_Plane)
    {
      if (!rajout && useUVParam)
      {
        rajout = (myVParam.Extent() > 2 &&
          (aSurface->IsUClosed() || aSurface->IsVClosed()));
      }

      if (rajout)
      {
        insertInternalVertices(trigu);

        //control internal points
        if (myIsControlSurfaceDeflection)
          aDef = control(trigu, Standard_False);
      }
    }
  }

  //modify myStructure back
  BRepMesh::HVectorOfVertex& aMeshNodes = myStructure->Data()->ChangeVertices();
  for ( Standard_Integer i = 1; i <= myStructure->NbNodes(); ++i )
  {
    BRepMesh_Vertex& aNode = aMeshNodes->ChangeValue(i - 1);
    aNode.ChangeCoord() = myAttribute->Scale(aNode.Coord(), Standard_False);

    const BRepMesh::ListOfInteger& alist = myStructure->LinksConnectedTo(i);
    // Register internal nodes used in triangulation
    if (!alist.IsEmpty() && aVertexEdgeMap->FindIndex(i) == 0)
      aVertexEdgeMap->Add(i);
  }

  if (!(aDef < 0.))
    myAttribute->SetDefFace(aDef);
}

//=======================================================================
//function : addVerticesToMesh
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_FastDiscretFace::addVerticesToMesh(
  const BRepMesh::ListOfVertex& theVertices,
  BRepMesh_Delaun&              theMeshBuilder)
{
  if (theVertices.IsEmpty())
    return Standard_False;

  BRepMesh::Array1OfVertexOfDelaun aArrayOfNewVertices(1, theVertices.Extent());
  BRepMesh::ListOfVertex::Iterator aVertexIt(theVertices);
  for (Standard_Integer aVertexId = 0; aVertexIt.More(); aVertexIt.Next())
    aArrayOfNewVertices(++aVertexId) = aVertexIt.Value();

  theMeshBuilder.AddVertices(aArrayOfNewVertices);
  return Standard_True;
}

//=======================================================================
//function : filterParameters
//purpose  : 
//=======================================================================
static void filterParameters(const BRepMesh::IMapOfReal& theParams,
                             const Standard_Real         theMinDist,
                             const Standard_Real         theFilterDist,
                             BRepMesh::SequenceOfReal&   theResult)
{
  // Sort sequence of parameters
  const Standard_Integer anInitLen = theParams.Extent();
    
  TColStd_Array1OfReal aParamArray(1, anInitLen);
  Standard_Integer j;
  for (j = 1; j <= anInitLen; j++)
    aParamArray(j) = theParams(j);

  std::sort (aParamArray.begin(), aParamArray.end());

  // mandatory pre-filtering using the first (minimal) filter value
  Standard_Integer aParamLength = 1;
  for (j = 2; j <= anInitLen; j++) 
  {
    if ((aParamArray(j)-aParamArray(aParamLength)) > theMinDist)
    {
      if (++aParamLength < j)
        aParamArray(aParamLength) = aParamArray(j);
    }
  }
  
  //perform filtering on series
  Standard_Real aLastAdded, aLastCandidate;
  Standard_Boolean isCandidateDefined = Standard_False;
  aLastAdded = aParamArray(1);
  aLastCandidate = aLastAdded;
  theResult.Append(aLastAdded);
  
  for(j=2; j < aParamLength; j++)
  {
    Standard_Real aVal = aParamArray(j);
    if(aVal-aLastAdded > theFilterDist) 
    {
      //adds the parameter
      if(isCandidateDefined) {
        aLastAdded = aLastCandidate;
        isCandidateDefined = Standard_False;
        j--;
      }
      else 
      {
        aLastAdded = aVal;
      }
      theResult.Append(aLastAdded);
      continue;
    }
    
    aLastCandidate = aVal;
    isCandidateDefined = Standard_True;
  }
  theResult.Append(aParamArray(aParamLength));
}

//=======================================================================
//function : insertInternalVertices
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscretFace::insertInternalVertices(BRepMesh_Delaun& theMeshBuilder)
{
  Handle(NCollection_IncAllocator) anAlloc = new NCollection_IncAllocator;
  BRepMesh::ListOfVertex aNewVertices(anAlloc);
  const Handle(BRepAdaptor_HSurface)& gFace = myAttribute->Surface();
  switch (gFace->GetType())
  {
  case GeomAbs_Sphere:
    insertInternalVerticesSphere(aNewVertices);
    break;

  case GeomAbs_Cylinder:
    insertInternalVerticesCylinder(aNewVertices);
    break;

  case GeomAbs_Cone:
    insertInternalVerticesCone(aNewVertices);
    break;

  case GeomAbs_Torus:
    insertInternalVerticesTorus(aNewVertices);
    break;

  default:
    insertInternalVerticesOther(aNewVertices);
    break;
  }
  
  addVerticesToMesh(aNewVertices, theMeshBuilder);
}

//=======================================================================
//function : insertInternalVerticesSphere
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscretFace::insertInternalVerticesSphere(
  BRepMesh::ListOfVertex& theNewVertices)
{
  Standard_Real aRange[] = {
    myAttribute->GetVMin(), myAttribute->GetVMax(),
    myAttribute->GetUMin(), myAttribute->GetUMax()
  };

  gp_Sphere aSphere = myAttribute->Surface()->Sphere();

  // Calculate parameters for iteration in V direction
  Standard_Real aStep = 0.7 * GCPnts_TangentialDeflection::ArcAngularStep(
    aSphere.Radius(), myAttribute->GetDefFace(), myAngle, myMinSize);

  Standard_Real aDd[2] = {aStep, aStep};
  Standard_Real aPasMax[2] = {0., 0.};
  for (Standard_Integer i = 0; i < 2; ++i)
  {
    const Standard_Real aMax = aRange[2 * i + 1];
    const Standard_Real aDiff = aMax - aRange[2 * i + 0];
    aDd[i] = aDiff / ((Standard_Integer)(aDiff / aDd[i]) + 1);
    aPasMax[i] = aMax - Precision::PConfusion();
  }

  const Standard_Real aHalfDu = aDd[1] * 0.5;
  Standard_Boolean Shift = Standard_False;
  Standard_Real aPasV = aRange[0] + aDd[0];
  for (; aPasV < aPasMax[0]; aPasV += aDd[0])
  {
    Shift = !Shift;
    const Standard_Real d = (Shift) ? aHalfDu : 0.;
    Standard_Real aPasU = aRange[2] + d;
    for (; aPasU < aPasMax[1]; aPasU += aDd[1])
    {
      tryToInsertAnalyticVertex(gp_Pnt2d(aPasU, aPasV), aSphere, theNewVertices);
    }
  }
}

//=======================================================================
//function : insertInternalVerticesCylinder
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscretFace::insertInternalVerticesCylinder(
  BRepMesh::ListOfVertex& theNewVertices)
{
  const Standard_Real umax = myAttribute->GetUMax();
  const Standard_Real umin = myAttribute->GetUMin();
  const Standard_Real vmax = myAttribute->GetVMax();
  const Standard_Real vmin = myAttribute->GetVMin();

  gp_Cylinder aCylinder = myAttribute->Surface()->Cylinder();
  const Standard_Real aRadius = aCylinder.Radius();

  Standard_Integer nbU = 0;
  Standard_Integer nbV = 0;
  const Standard_Real su = umax - umin;
  const Standard_Real sv = vmax - vmin;
  const Standard_Real aArcLen = su * aRadius;
  if (aArcLen > myAttribute->GetDefFace ())
  {
    // Calculate parameters for iteration in U direction
    const Standard_Real Du = GCPnts_TangentialDeflection::ArcAngularStep (
      aRadius, myAttribute->GetDefFace (), myAngle, myMinSize);
    nbU = (Standard_Integer)(su / Du);

    // Calculate parameters for iteration in V direction
    const Standard_Real aDv = nbU*sv / aArcLen;
    // Protection against overflow during casting to int in case 
    // of long cylinder with small radius.
    nbV = aDv > static_cast<Standard_Real> (IntegerLast ()) ? 
      0 : (Standard_Integer)(aDv);
    nbV = Min (nbV, 100 * nbU);
  }

  const Standard_Real Du = su / (nbU + 1);
  const Standard_Real Dv = sv / (nbV + 1);

  Standard_Real pasu, pasv, pasvmax = vmax - Dv*0.5, pasumax = umax - Du*0.5;
  for (pasv = vmin + Dv; pasv < pasvmax; pasv += Dv)
  {
    for (pasu = umin + Du; pasu < pasumax; pasu += Du)
    {
      tryToInsertAnalyticVertex(gp_Pnt2d(pasu, pasv), aCylinder, theNewVertices);
    }
  }
}

//=======================================================================
//function : insertInternalVerticesCone
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscretFace::insertInternalVerticesCone(
  BRepMesh::ListOfVertex& theNewVertices)
{
  const Standard_Real umax = myAttribute->GetUMax();
  const Standard_Real umin = myAttribute->GetUMin();
  const Standard_Real vmax = myAttribute->GetVMax();
  const Standard_Real vmin = myAttribute->GetVMin();

  gp_Cone aCone = myAttribute->Surface()->Cone();
  Standard_Real RefR = aCone.RefRadius();
  Standard_Real SAng = aCone.SemiAngle();
  Standard_Real aRadius = Max(Abs(RefR + vmin*Sin(SAng)), Abs(RefR + vmax*Sin(SAng)));

  Standard_Real Du = GCPnts_TangentialDeflection::ArcAngularStep(
    aRadius, myAttribute->GetDefFace(), myAngle, myMinSize);

  Standard_Real Dv, pasu, pasv;
  Standard_Integer nbU = (Standard_Integer)((umax - umin) / Du);
  Standard_Integer nbV = (Standard_Integer)(nbU*(vmax - vmin) / ((umax - umin)*aRadius));
  Du = (umax - umin) / (nbU + 1);
  Dv = (vmax - vmin) / (nbV + 1);

  Standard_Real pasvmax = vmax - Dv*0.5, pasumax = umax - Du*0.5;
  for (pasv = vmin + Dv; pasv < pasvmax; pasv += Dv)
  {
    for (pasu = umin + Du; pasu < pasumax; pasu += Du)
    {
      tryToInsertAnalyticVertex(gp_Pnt2d(pasu, pasv), aCone, theNewVertices);
    }
  }
}

//=======================================================================
//function : insertInternalVerticesTorus
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscretFace::insertInternalVerticesTorus(
  BRepMesh::ListOfVertex& theNewVertices)
{
  const Standard_Real umax     = myAttribute->GetUMax();
  const Standard_Real umin     = myAttribute->GetUMin();
  const Standard_Real vmax     = myAttribute->GetVMax();
  const Standard_Real vmin     = myAttribute->GetVMin();
  const Standard_Real deltaX   = myAttribute->GetDeltaX();
  const Standard_Real deltaY   = myAttribute->GetDeltaY();
  const Standard_Real aDefFace = myAttribute->GetDefFace();

  gp_Torus T = myAttribute->Surface()->Torus();

  Standard_Boolean insert;
  Standard_Integer i, j, ParamULength, ParamVLength;
  Standard_Real pp, pasu, pasv;
  Standard_Real r = T.MinorRadius(), R = T.MajorRadius();

  BRepMesh::SequenceOfReal ParamU, ParamV;

  Standard_Real oldDv = GCPnts_TangentialDeflection::ArcAngularStep(
    r, aDefFace, myAngle, myMinSize);

  Standard_Real Dv = 0.9*oldDv; //TWOTHIRD * oldDv;
  Dv = oldDv;

  Standard_Real Du;
  Standard_Integer nbV = Max((Standard_Integer)((vmax - vmin) / Dv), 2);
  Dv = (vmax - vmin) / (nbV + 1);
  Standard_Real ru = R + r;
  if (ru > 1.e-16)
  {
    Du = GCPnts_TangentialDeflection::ArcAngularStep(
      ru, aDefFace, myAngle, myMinSize);

    Standard_Real aa = sqrt(Du*Du + oldDv*oldDv);
    if (aa < gp::Resolution())
      return;
    Du *= Min(oldDv, Du) / aa;
  }
  else Du = Dv;

  Standard_Integer nbU = Max((Standard_Integer)((umax - umin) / Du), 2);
  nbU = Max(nbU, (int)(nbV*(umax - umin)*R / ((vmax - vmin)*r) / 5.));
  Du = (umax - umin) / (nbU + 1);

  if (R < r)
  {
    // As the points of edges are returned.
    // in this case, the points are not representative.

    //-- Choose DeltaX and DeltaY so that to avoid skipping points on the grid
    for (i = 0; i <= nbU; i++) ParamU.Append(umin + i* Du);
  }//R<r
  else //U if R > r
  {
    //--ofv: U
    // Number of mapped U parameters
    const Standard_Integer LenU = myUParam.Extent();
    // Fill array of U parameters
    TColStd_Array1OfReal Up(1, LenU);
    for (j = 1; j <= LenU; j++) Up(j) = myUParam(j);

    // Calculate DU, leave array of parameters
    Standard_Real aDU = FUN_CalcAverageDUV(Up, LenU);
    aDU = Max(aDU, Abs(umax - umin) / (Standard_Real)nbU / 2.);
    Standard_Real dUstd = Abs(umax - umin) / (Standard_Real)LenU;
    if (aDU > dUstd) dUstd = aDU;
    // Add U parameters
    for (j = 1; j <= LenU; j++)
    {
      pp = Up(j);
      insert = Standard_True;
      ParamULength = ParamU.Length();
      for (i = 1; i <= ParamULength && insert; i++)
      {
        insert = (Abs(ParamU.Value(i) - pp) > (0.5*dUstd));
      }
      if (insert) ParamU.Append(pp);
    }
  }

  //--ofv: V
  // Number of mapped V parameters
  const Standard_Integer LenV = myVParam.Extent();
  // Fill array of V parameters
  TColStd_Array1OfReal Vp(1, LenV);
  for (j = 1; j <= LenV; j++) Vp(j) = myVParam(j);
  // Calculate DV, sort array of parameters
  Standard_Real aDV = FUN_CalcAverageDUV(Vp, LenV);
  aDV = Max(aDV, Abs(vmax - vmin) / (Standard_Real)nbV / 2.);

  Standard_Real dVstd = Abs(vmax - vmin) / (Standard_Real)LenV;
  if (aDV > dVstd) dVstd = aDV;
  // Add V parameters
  for (j = 1; j <= LenV; j++)
  {
    pp = Vp(j);

    insert = Standard_True;
    ParamVLength = ParamV.Length();
    for (i = 1; i <= ParamVLength && insert; i++)
    {
      insert = (Abs(ParamV.Value(i) - pp) > (dVstd*2. / 3.));
    }
    if (insert) ParamV.Append(pp);
  }

  Standard_Integer Lu = ParamU.Length(), Lv = ParamV.Length();
  Standard_Real uminnew = umin + deltaY*0.1;
  Standard_Real vminnew = vmin + deltaX*0.1;
  Standard_Real umaxnew = umax - deltaY*0.1;
  Standard_Real vmaxnew = vmax - deltaX*0.1;

  for (i = 1; i <= Lu; i++)
  {
    pasu = ParamU.Value(i);
    if (pasu >= uminnew && pasu < umaxnew)
    {
      for (j = 1; j <= Lv; j++)
      {
        pasv = ParamV.Value(j);
        if (pasv >= vminnew && pasv < vmaxnew)
        {
          tryToInsertAnalyticVertex(gp_Pnt2d(pasu, pasv), T, theNewVertices);
        }
      }
    }
  }
}

//=======================================================================
//function : insertInternalVerticesOther
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscretFace::insertInternalVerticesOther(
  BRepMesh::ListOfVertex& theNewVertices)
{
  const Standard_Real aRange[2][2] = {
      { myAttribute->GetUMax(), myAttribute->GetUMin() },
      { myAttribute->GetVMax(), myAttribute->GetVMin() }
  };

  const Standard_Real aDelta[2] = { 
    myAttribute->GetDeltaX(),
    myAttribute->GetDeltaY()
  };

  const Standard_Real                 aDefFace = myAttribute->GetDefFace();
  const Handle(BRepAdaptor_HSurface)& gFace    = myAttribute->Surface();

  Handle(NCollection_IncAllocator) anAlloc = new NCollection_IncAllocator;
  BRepMesh::SequenceOfReal aParams[2] = { BRepMesh::SequenceOfReal(anAlloc), 
                                          BRepMesh::SequenceOfReal(anAlloc) };
  for (Standard_Integer i = 0; i < 2; ++i)
  {
    Standard_Boolean isU = (i == 0);
    Standard_Real aRes = isU ?
      gFace->UResolution(aDefFace) :
      gFace->VResolution(aDefFace);

    // Sort and filter sequence of parameters
    Standard_Real aMinDiff = Precision::PConfusion();
    if (aDelta[i] < 1.)
      aMinDiff /= aDelta[i];

    aMinDiff = Max(myMinSize, aMinDiff);

    Standard_Real aRangeDiff = aRange[i][0] - aRange[i][1];
    Standard_Real aDiffMaxLim = 0.1 * aRangeDiff;
    Standard_Real aDiffMinLim = Max(0.005 * aRangeDiff, 2. * aRes);
    Standard_Real aDiff = Max(myMinSize, Min(aDiffMaxLim, aDiffMinLim));
    filterParameters(isU ? myUParam : myVParam, aMinDiff, aDiff, aParams[i]);
  }

  // check intermediate isolines
  Handle (Geom_Surface) aSurface = gFace->ChangeSurface ().Surface ().Surface ();

  BRepMesh::MapOfReal aParamsToRemove[2] = { BRepMesh::MapOfReal(1, anAlloc),
                                             BRepMesh::MapOfReal(1, anAlloc) };
  BRepMesh::MapOfReal aParamsForbiddenToRemove[2] = { BRepMesh::MapOfReal(1, anAlloc),
                                                      BRepMesh::MapOfReal(1, anAlloc) };

  // insert additional points where it is needed to conform criteria.
  // precision for normals calculation
  const Standard_Real aNormPrec = Precision::Confusion();
  for (Standard_Integer k = 0; k < 2; ++k)
  {
    const Standard_Integer aOtherIndex = (k + 1) % 2;
    BRepMesh::SequenceOfReal& aParams1 = aParams[k];
    BRepMesh::SequenceOfReal& aParams2 = aParams[aOtherIndex];
    const Standard_Boolean isU = (k == 0);
    for (Standard_Integer i = 2; i < aParams1.Length(); ++i)
    {
      const Standard_Real aParam1 = aParams1(i);
      GeomAdaptor_Curve aIso(isU ?
        aSurface->UIso(aParam1) : aSurface->VIso(aParam1));

      Standard_Real aPrevParam2 = aParams2(1);
      gp_Pnt aPrevPnt2;
      gp_Vec aPrevVec2;
      aIso.D1(aPrevParam2, aPrevPnt2, aPrevVec2);
      for (Standard_Integer j = 2; j <= aParams2.Length();)
      {
        Standard_Real aParam2 = aParams2(j);
        gp_Pnt aPnt2;
        gp_Vec aVec2;
        aIso.D1(aParam2, aPnt2, aVec2);

        Standard_Real aMidParam = 0.5 * (aPrevParam2 + aParam2);
        gp_Pnt aMidPnt = aIso.Value(aMidParam);

        Standard_Real aDist = deflectionOfSegment(aPrevPnt2, aPnt2, aMidPnt);
        if (aDist > aDefFace && aDist > myMinSize)
        {
          // insertion 
          aParams2.InsertBefore(j, aMidParam);
          continue;
        }
        //put regular grig for normals
        gp_Pnt2d aStPnt1, aStPnt2;
        if (isU)
        {
          aStPnt1 = gp_Pnt2d(aParam1, aPrevParam2);
          aStPnt2 = gp_Pnt2d(aParam1, aMidParam);
        }
        else
        {
          aStPnt1 = gp_Pnt2d(aPrevParam2, aParam1);
          aStPnt2 = gp_Pnt2d(aMidParam, aParam1);
        }

        gp_Dir N1(0, 0, 1), N2(0, 0, 1);
        Standard_Integer aSt1 = GeomLib::NormEstim(aSurface, aStPnt1, aNormPrec, N1);
        Standard_Integer aSt2 = GeomLib::NormEstim(aSurface, aStPnt2, aNormPrec, N2);

        const Standard_Real aAngle = N2.Angle(N1);
        if (aSt1 < 1 && aSt2 < 1 && aAngle > myAngle)
        {
          const Standard_Real aLen = GCPnts_AbscissaPoint::Length(
            aIso, aPrevParam2, aMidParam, aDefFace);

          if (aLen > myMinSize)
          {
            // insertion 
            aParams2.InsertBefore(j, aMidParam);
            continue;
          }
        }
        aPrevParam2 = aParam2;
        aPrevPnt2 = aPnt2;
        aPrevVec2 = aVec2;

        ++j;
      }
    }
  }
#ifdef DEBUG_InsertInternal
  // output numbers of parameters along U and V
  cout << "aParams: " << aParams[0].Length() << " " << aParams[1].Length() << endl;
#endif
  // try to reduce number of points evaluating of isolines sampling
  for (Standard_Integer k = 0; k < 2; ++k)
  {
    const Standard_Integer aOtherIndex = (k + 1) % 2;
    BRepMesh::SequenceOfReal& aParams1 = aParams[k];
    BRepMesh::SequenceOfReal& aParams2 = aParams[aOtherIndex];
    const Standard_Boolean isU = (k == 0);
    BRepMesh::MapOfReal& aToRemove2          = aParamsToRemove[aOtherIndex];
    BRepMesh::MapOfReal& aForbiddenToRemove1 = aParamsForbiddenToRemove[k];
    BRepMesh::MapOfReal& aForbiddenToRemove2 = aParamsForbiddenToRemove[aOtherIndex];
    for (Standard_Integer i = 2; i < aParams1.Length(); ++i)
    {
      const Standard_Real aParam1 = aParams1(i);
      GeomAdaptor_Curve aIso(isU ?
        aSurface->UIso (aParam1) : aSurface->VIso (aParam1));
#ifdef DEBUG_InsertInternal
      // write polyline containing initial parameters to the file
      {
        ofstream ff(DEBUG_InsertInternal, std::ios_base::app);
        ff << "polyline " << (k == 0 ? "u" : "v") << i << " ";
        for (Standard_Integer j = 1; j <= aParams2.Length(); j++)
        {
          gp_Pnt aP;
          aIso.D0(aParams2(j), aP);
          ff << aP.X() << " " << aP.Y() << " " << aP.Z() << " ";
        }
        ff << endl;
      }
#endif

      Standard_Real aPrevParam2 = aParams2(1);
      gp_Pnt aPrevPnt2;
      gp_Vec aPrevVec2;
      aIso.D1 (aPrevParam2, aPrevPnt2, aPrevVec2);
      for (Standard_Integer j = 2; j <= aParams2.Length();)
      {
        Standard_Real aParam2 = aParams2(j);
        gp_Pnt aPnt2;
        gp_Vec aVec2;
        aIso.D1 (aParam2, aPnt2, aVec2);

        // Here we should leave at least 3 parameters as far as
        // we must have at least one parameter related to surface
        // internals in order to prevent movement of triangle body
        // outside the surface in case of highly curved ones, e.g.
        // BSpline springs.
        if (aParams2.Length() > 3 && j < aParams2.Length())
        {
          // Remove too dense points
          const Standard_Real aNextParam = aParams2(j + 1);
          gp_Pnt aNextPnt;
          gp_Vec aNextVec;
          aIso.D1(aNextParam, aNextPnt, aNextVec);

          // Lets check current parameter.
          // If it fits deflection, we can remove it.
          Standard_Real aDist = deflectionOfSegment(aPrevPnt2, aNextPnt, aPnt2);
          if (aDist < aDefFace)
          {
            // Lets check parameters for angular deflection.
            if (aPrevVec2.SquareMagnitude() > gp::Resolution() &&
                aNextVec.SquareMagnitude() > gp::Resolution() &&
                aPrevVec2.Angle(aNextVec) < myAngle)
            {
              // For current Iso line we can remove this parameter.
#ifdef DEBUG_InsertInternal
              // write point of removed parameter
              {
                ofstream ff(DEBUG_InsertInternal, std::ios_base::app);
                ff << "point " << (k == 0 ? "u" : "v") << i << "r_" << j << " "
                  << aPnt2.X() << " " << aPnt2.Y() << " " << aPnt2.Z() << endl;
              }
#endif
              aToRemove2.Add(aParam2);
              aPrevParam2 = aNextParam;
              aPrevPnt2 = aNextPnt;
              aPrevVec2 = aNextVec;
              j += 2;
              continue;
            }
            else {
              // We have found a place on the surface refusing 
              // removement of this parameter.
#ifdef DEBUG_InsertInternal
              // write point of forbidden to remove parameter
              {
                ofstream ff(DEBUG_InsertInternal, std::ios_base::app);
                ff << "vertex " << (k == 0 ? "u" : "v") << i << "f_" << j << " "
                  << aPnt2.X() << " " << aPnt2.Y() << " " << aPnt2.Z() << endl;
              }
#endif
              aForbiddenToRemove1.Add(aParam1);
              aForbiddenToRemove2.Add(aParam2);
            }
          }
        }
        aPrevParam2 = aParam2;
        aPrevPnt2 = aPnt2;
        aPrevVec2 = aVec2;
        ++j;
      }
    }
  }
  // remove filtered out parameters
  for (Standard_Integer k = 0; k < 2; ++k)
  {
    BRepMesh::SequenceOfReal& aParamsk = aParams[k];
    for (Standard_Integer i = 1; i <= aParamsk.Length();)
    {
      const Standard_Real aParam = aParamsk.Value(i);
      if (aParamsToRemove[k].Contains(aParam) &&
        !aParamsForbiddenToRemove[k].Contains(aParam))
      {
        aParamsk.Remove(i);
      }
      else
        i++;
    }
  }
#ifdef DEBUG_InsertInternal
  // write polylines containing remaining parameters
  {
    ofstream ff(DEBUG_InsertInternal, std::ios_base::app);
    for (Standard_Integer k = 0; k < 2; ++k)
    {
      for (Standard_Integer i = 1; i <= aParams[k].Length(); i++)
      {
        ff << "polyline " << (k == 0 ? "uo" : "vo") << i << " ";
        for (Standard_Integer j = 1; j <= aParams[1 - k].Length(); j++)
        {
          gp_Pnt aP;
          if (k == 0)
            gFace->D0(aParams[k](i), aParams[1 - k](j), aP);
          else
            gFace->D0(aParams[1 - k](j), aParams[k](i), aP);
          ff << aP.X() << " " << aP.Y() << " " << aP.Z() << " ";
        }
        ff << endl;
      }
    }
  }
#endif

  // insert nodes of the regular grid
  const BRepMesh::HClassifier& aClassifier = myAttribute->ChangeClassifier();
  for (Standard_Integer i = 1; i <= aParams[0].Length(); ++i)
  {
    const Standard_Real aParam1 = aParams[0].Value (i);
    for (Standard_Integer j = 1; j <= aParams[1].Length(); ++j)
    {
      const Standard_Real aParam2 = aParams[1].Value (j);
      gp_Pnt2d aPnt2d(aParam1, aParam2);

      // Classify intersection point
      if (aClassifier->Perform(aPnt2d) == TopAbs_IN)
      {
        gp_Pnt aPnt;
        gFace->D0(aPnt2d.X(), aPnt2d.Y(), aPnt);
        insertVertex(aPnt, aPnt2d.Coord(), theNewVertices);
      }
    }
  }
}

//=======================================================================
//function : checkDeflectionAndInsert
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_FastDiscretFace::checkDeflectionAndInsert(
  const gp_Pnt&              thePnt3d,
  const gp_XY&               theUV,
  const Standard_Boolean     isDeflectionCheckOnly,
  const Standard_Real        theTriangleDeflection,
  const Standard_Real        theFaceDeflection,
  const BRepMesh_CircleTool& theCircleTool,
  BRepMesh::ListOfVertex&    theVertices,
  Standard_Real&             theMaxTriangleDeflection,
  const Handle(NCollection_IncAllocator)& theTempAlloc)
{
  if (theTriangleDeflection > theMaxTriangleDeflection)
    theMaxTriangleDeflection = theTriangleDeflection;

  if (theTriangleDeflection < theFaceDeflection)
    return Standard_True;

  if (myMinSize > Precision::Confusion())
  {
    // Iterator in the list of indexes of circles containing the node
    BRepMesh::ListOfInteger& aCirclesList = 
      const_cast<BRepMesh_CircleTool&>(theCircleTool).Select(
      myAttribute->Scale(theUV, Standard_True));
    
    BRepMesh::MapOfInteger aUsedNodes(10, theTempAlloc);
    BRepMesh::ListOfInteger::Iterator aCircleIt(aCirclesList);
    for (; aCircleIt.More(); aCircleIt.Next())
    {
      const BRepMesh_Triangle& aTriangle = 
        myStructure->GetElement(aCircleIt.Value());

      Standard_Integer aNodes[3];
      myStructure->ElementNodes(aTriangle, aNodes);

      for (Standard_Integer i = 0; i < 3; ++i)
      {
        const Standard_Integer aNodeId = aNodes[i];
        if (aUsedNodes.Contains(aNodeId))
          continue;

        aUsedNodes.Add(aNodeId);
        const BRepMesh_Vertex& aNode = myStructure->GetNode(aNodeId);
        const gp_Pnt& aPoint = myAttribute->GetPoint(aNode);

        if (thePnt3d.SquareDistance(aPoint) < myMinSize * myMinSize)
          return Standard_True;
      }
    }
  }

  if (isDeflectionCheckOnly)
    return Standard_False;

  insertVertex(thePnt3d, theUV, theVertices);
  return Standard_True;
}

//=======================================================================
//function : control
//purpose  : 
//=======================================================================
Standard_Real BRepMesh_FastDiscretFace::control(
  BRepMesh_Delaun&         theTrigu,
  const Standard_Boolean   theIsFirst)
{
  Standard_Integer aTrianglesNb = myStructure->ElementsOfDomain().Extent();
  if (aTrianglesNb < 1)
    return -1.0;

  //IMPORTANT: Constants used in calculations
  const Standard_Real MinimalArea2d     = 1.e-9;
  const Standard_Real MinimalSqLength3d = 1.e-12;
  const Standard_Real aSqDefFace = myAttribute->GetDefFace() * myAttribute->GetDefFace();

  const Handle(BRepAdaptor_HSurface)& gFace = myAttribute->Surface();

  Handle(Geom_Surface) aBSpline;
  const GeomAbs_SurfaceType aSurfType = gFace->GetType ();
  if (IsCompexSurface (aSurfType) && aSurfType != GeomAbs_SurfaceOfExtrusion)
    aBSpline = gFace->ChangeSurface ().Surface().Surface();

  Handle(NCollection_IncAllocator) anAlloc =
    new NCollection_IncAllocator(BRepMesh::MEMORY_BLOCK_SIZE_HUGE);
  NCollection_DataMap<Standard_Integer, gp_Dir> aNorMap(1, anAlloc);
  BRepMesh::MapOfIntegerInteger                 aStatMap(1, anAlloc);
  NCollection_Map<BRepMesh_Edge>                aCouples(3 * aTrianglesNb, anAlloc);
  const BRepMesh_CircleTool& aCircles = theTrigu.Circles();

  // Perform refinement passes
  // Define the number of iterations
  Standard_Integer       aIterationsNb = 11;
  const Standard_Integer aPassesNb = (theIsFirst ? 1 : aIterationsNb);
  // Initialize stop condition
  Standard_Real aMaxSqDef = -1.;
  Standard_Integer aPass = 1, aInsertedNb = 1;
  Standard_Boolean isAllDegenerated = Standard_False;
  Handle(NCollection_IncAllocator) aTempAlloc =
    new NCollection_IncAllocator(BRepMesh::MEMORY_BLOCK_SIZE_HUGE);
  for (; aPass <= aPassesNb && aInsertedNb && !isAllDegenerated; ++aPass)
  {
    aTempAlloc->Reset(Standard_False);
    BRepMesh::ListOfVertex aNewVertices(aTempAlloc);

    // Reset stop condition
    aInsertedNb      = 0;
    aMaxSqDef        = -1.;
    isAllDegenerated = Standard_True;

    aTrianglesNb = myStructure->ElementsOfDomain().Extent();
    if (aTrianglesNb < 1)
      break;

    // Iterate on current triangles
    const BRepMesh::MapOfInteger& aTriangles = myStructure->ElementsOfDomain();
    BRepMesh::MapOfInteger::Iterator aTriangleIt(aTriangles);
    for (; aTriangleIt.More(); aTriangleIt.Next())
    {
      const Standard_Integer aTriangleId = aTriangleIt.Key();
      const BRepMesh_Triangle& aCurrentTriangle = myStructure->GetElement(aTriangleId);

      if (aCurrentTriangle.Movability() == BRepMesh_Deleted)
        continue;

      Standard_Integer v[3];
      myStructure->ElementNodes(aCurrentTriangle, v);

      Standard_Integer e[3];
      Standard_Boolean o[3];
      aCurrentTriangle.Edges(e, o);

      gp_XY xy[3];
      gp_XYZ p[3];
      Standard_Boolean m[3];
      for (Standard_Integer i = 0; i < 3; ++i)
      {
        m[i] = (myStructure->GetLink(e[i]).Movability() == BRepMesh_Frontier);

        const BRepMesh_Vertex& aVertex = myStructure->GetNode(v[i]);
        xy[i] = myAttribute->Scale(aVertex.Coord(), Standard_False);
        p [i] = myAttribute->GetPoint(aVertex).Coord();
      }

      gp_XYZ aLinkVec[3];
      Standard_Boolean isDegeneratedTri = Standard_False;
      for (Standard_Integer i = 0; i < 3 && !isDegeneratedTri; ++i)
      {
        aLinkVec[i] = p[(i + 1) % 3] - p[i];
        isDegeneratedTri = (aLinkVec[i].SquareModulus() < MinimalSqLength3d);
      }

      if (isDegeneratedTri) 
        continue;

      isAllDegenerated = Standard_False;

      // Check triangle area in 2d
      if (Abs((xy[1]-xy[0])^(xy[2]-xy[1])) < MinimalArea2d)
        continue;

      // Check triangle normal
      gp_Pnt pDef;
      Standard_Real aSqDef = -1.;
      Standard_Boolean isSkipped = Standard_False;
      gp_XYZ normal(aLinkVec[0] ^ aLinkVec[1]);
      if (normal.SquareModulus () < gp::Resolution())
        continue;

      normal.Normalize();

      // Check deflection at triangle centroid
      gp_XY aCenter2d = (xy[0] + xy[1] + xy[2]) / 3.0;
      gFace->D0(aCenter2d.X(), aCenter2d.Y(), pDef);
      aSqDef = Abs(normal * (pDef.XYZ() - p[0]));
      aSqDef *= aSqDef;

      isSkipped = !checkDeflectionAndInsert(pDef, aCenter2d, theIsFirst, 
        aSqDef, aSqDefFace, aCircles, aNewVertices, aMaxSqDef, aTempAlloc);

      if (isSkipped)
        break;

      // Check deflection at triangle links
      for (Standard_Integer i = 0; i < 3 && !isSkipped; ++i)
      {
        if (m[i]) // is a boundary
          continue;

        // Check if this link was already processed
        if (aCouples.Add(myStructure->GetLink(e[i])))
        {
          // Check deflection on edge 1
          Standard_Integer j = (i + 1) % 3;
          gp_XY mi2d = (xy[i] + xy[j]) * 0.5;
          gFace->D0(mi2d.X(), mi2d.Y(), pDef);
          gp_Lin aLin(p[i], gp_Vec(p[i], p[j]));
          aSqDef = aLin.SquareDistance(pDef);

          isSkipped = !checkDeflectionAndInsert(pDef, mi2d, theIsFirst, 
            aSqDef, aSqDefFace, aCircles, aNewVertices, aMaxSqDef, aTempAlloc);
        }
      }

      if (isSkipped)
        break;

      //check normal on bsplines
      if (theIsFirst && !aBSpline.IsNull())
      {
        gp_Dir N[3] = { gp::DZ(), gp::DZ(), gp::DZ() };
        Standard_Integer aSt[3];

        for (Standard_Integer i = 0; i < 3; ++i)
        {
          if (aNorMap.IsBound(v[i]))
          {
            aSt[i] = aStatMap.Find(v[i]);
            N[i] = aNorMap.Find(v[i]);
          }
          else
          {
            aSt[i] = GeomLib::NormEstim(aBSpline, gp_Pnt2d(xy[i]), Precision::Confusion(), N[i]);
            aStatMap.Bind(v[i], aSt[i]);
            aNorMap.Bind(v[i], N[i]);
          }
        }

        Standard_Real aAngle[3];
        for (Standard_Integer i = 0; i < 3; ++i)
          aAngle[i] = N[(i + 1) % 3].Angle(N[i]);

        if (aSt[0] < 1 && aSt[1] < 1 && aSt[2] < 1)
        {
          if (aAngle[0] > myAngle || aAngle[1] > myAngle || aAngle[2] > myAngle)
          {
            aMaxSqDef = -1.;
            break;
          }
        }
      }
    }

    if (theIsFirst)
      continue;

#ifdef DEBUG_MESH
    // append to the file triangles in the form of polyline commands;
    // so the file can be sourced in draw to analyze triangles on each pass of the algorithm.
    // write triangles
    ofstream ftt(DEBUG_MESH, std::ios_base::app);
    Standard_Integer nbElem = myStructure->NbElements();
    for (Standard_Integer i = 1; i <= nbElem; i++)
    {
      const BRepMesh_Triangle& aTri = myStructure->GetElement(i);
      if (aTri.Movability() == BRepMesh_Deleted)
        continue;
      Standard_Integer n[3];
      myStructure->ElementNodes(aTri, n);
      const BRepMesh_Vertex& aV1 = myStructure->GetNode(n[0]);
      const BRepMesh_Vertex& aV2 = myStructure->GetNode(n[1]);
      const BRepMesh_Vertex& aV3 = myStructure->GetNode(n[2]);
      const gp_Pnt& aP1 = myAttribute->GetPoint(aV1);
      const gp_Pnt& aP2 = myAttribute->GetPoint(aV2);
      const gp_Pnt& aP3 = myAttribute->GetPoint(aV3);
      ftt << "polyline t" << aPass << "_" << i << " "
        << aP1.X() << " " << aP1.Y() << " " << aP1.Z() << " "
        << aP2.X() << " " << aP2.Y() << " " << aP2.Z() << " "
        << aP3.X() << " " << aP3.Y() << " " << aP3.Z() << " "
        << aP1.X() << " " << aP1.Y() << " " << aP1.Z() << endl;
    }
    // write points to insert on the current pass
    BRepMesh::ListOfVertex::Iterator it(aNewVertices);
    for (Standard_Integer i=1; it.More(); it.Next(), i++)
    {
      const BRepMesh_Vertex& aVertex = it.Value();
      const gp_Pnt& aP = myAttribute->GetPoint(aVertex);
      ftt << "vertex vt" << aPass << "_" << i << " "
        << aP.X() << " " << aP.Y() << " " << aP.Z() << endl;
    }
#endif

    if (addVerticesToMesh(aNewVertices, theTrigu))
      ++aInsertedNb;
  }

  return (aMaxSqDef < 0) ? aMaxSqDef : Sqrt(aMaxSqDef);
}

//=======================================================================
//function : add
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscretFace::add(const TopoDS_Vertex& theVertex)
{
  if (theVertex.Orientation() != TopAbs_INTERNAL)
    return;

  try
  {
    OCC_CATCH_SIGNALS

    gp_Pnt2d aPnt2d = BRep_Tool::Parameters(theVertex, myAttribute->Face());
    // check UV values for internal vertices
    if (myAttribute->ChangeClassifier()->Perform(aPnt2d) != TopAbs_IN)
      return;

    NCollection_Handle<FixedVExplorer> aFixedVExplorer = new FixedVExplorer(theVertex);
    Standard_Integer aIndex = myAttribute->GetVertexIndex(aFixedVExplorer);
    gp_XY anUV = BRepMesh_ShapeTool::FindUV(aIndex, aPnt2d,
      BRep_Tool::Tolerance(theVertex), myAttribute);

    Standard_Integer aTmpId1, aTmpId2;
    anUV = myAttribute->Scale(anUV, Standard_True);
    myAttribute->AddNode(aIndex, anUV, BRepMesh_Fixed, aTmpId1, aTmpId2);
  }
  catch (Standard_Failure)
  {
  }
}

//=======================================================================
//function : insertVertex
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscretFace::insertVertex(
  const gp_Pnt&           thePnt3d,
  const gp_XY&            theUV,
  BRepMesh::ListOfVertex& theVertices)
{
  Standard_Integer aNbLocat = myAttribute->LastPointId();
  myAttribute->ChangeSurfacePoints()->Bind(++aNbLocat, thePnt3d);

  gp_XY aPnt2d  = myAttribute->Scale(theUV, Standard_True);
  BRepMesh_Vertex aVertex(aPnt2d, aNbLocat, BRepMesh_Free);
  theVertices.Append(aVertex);
}

//=======================================================================
//function : commitSurfaceTriangulation
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscretFace::commitSurfaceTriangulation()
{
  if (myAttribute.IsNull() || !myAttribute->IsValid())
    return;

  const TopoDS_Face& aFace = myAttribute->Face();
  BRepMesh_ShapeTool::NullifyFace(aFace);

  Handle(BRepMesh_DataStructureOfDelaun)& aStructure = myAttribute->ChangeStructure();
  const BRepMesh::MapOfInteger&           aTriangles = aStructure->ElementsOfDomain();

  if (aTriangles.IsEmpty())
  {
    myAttribute->SetStatus(BRepMesh_Failure);
    return;
  }

  BRepMesh::HIMapOfInteger& aVetrexEdgeMap = myAttribute->ChangeVertexEdgeMap();

  // Store triangles
  Standard_Integer aVerticesNb  = aVetrexEdgeMap->Extent();
  Standard_Integer aTrianglesNb = aTriangles.Extent();
  Handle(Poly_Triangulation) aNewTriangulation =
    new Poly_Triangulation(aVerticesNb, aTrianglesNb, Standard_True);

  Poly_Array1OfTriangle& aPolyTrianges = aNewTriangulation->ChangeTriangles();

  Standard_Integer aTriangeId = 1;
  BRepMesh::MapOfInteger::Iterator aTriIt(aTriangles);
  for (; aTriIt.More(); aTriIt.Next())
  {
    const BRepMesh_Triangle& aCurElem = aStructure->GetElement(aTriIt.Key());

    Standard_Integer aNode[3];
    aStructure->ElementNodes(aCurElem, aNode);

    Standard_Integer aNodeId[3];
    for (Standard_Integer i = 0; i < 3; ++i)
      aNodeId[i] = aVetrexEdgeMap->FindIndex(aNode[i]);

    aPolyTrianges(aTriangeId++).Set(aNodeId[0], aNodeId[1], aNodeId[2]);
  }

  // Store mesh nodes
  TColgp_Array1OfPnt&   aNodes   = aNewTriangulation->ChangeNodes();
  TColgp_Array1OfPnt2d& aNodes2d = aNewTriangulation->ChangeUVNodes();

  for (Standard_Integer i = 1; i <= aVerticesNb; ++i)
  {
    Standard_Integer       aVertexId = aVetrexEdgeMap->FindKey(i);
    const BRepMesh_Vertex& aVertex   = aStructure->GetNode(aVertexId);
    const gp_Pnt&          aPoint    = myAttribute->GetPoint(aVertex);

    aNodes(i)   = aPoint;
    aNodes2d(i) = aVertex.Coord();
  }

  aNewTriangulation->Deflection(myAttribute->GetDefFace());
  BRepMesh_ShapeTool::AddInFace(aFace, aNewTriangulation);

  // Delete unused data
  myUParam.Clear(0L);
  myVParam.Clear(0L);
  myAttribute->ChangeStructure().Nullify();
  myAttribute->ChangeSurfacePoints().Nullify();
  myAttribute->ChangeSurfaceVertices().Nullify();

  myAttribute->ChangeClassifier().Nullify();
  myAttribute->ChangeLocation2D().Nullify();
  myAttribute->ChangeVertexEdgeMap().Nullify();
}
