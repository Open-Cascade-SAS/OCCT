// Created on: 1997-06-26
// Created by: Laurent PAINNOT
// Copyright (c) 1997-1999 Matra Datavision
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

#include <BRepMesh_Classifier.ixx>

// Kernel
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TColStd_ListOfTransient.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <ElCLib.hxx>
// Geometry
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Geom2dInt_Geom2dCurveTool.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
// Topology
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <CSLib_Class2d.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
// BRepMesh
#include <BRepMesh_Vertex.hxx>
#include <BRepMesh_Array1OfBiPoint.hxx>
#include <BRepMesh_PairOfPolygon.hxx>

static const Standard_Real PARALL_COND = Sin(M_PI/3.0);
static const Standard_Real RESOLUTION = 1.0E-16;

// Real mesh is created in the grid 10E5x10E5, so intersection
// should be cheched with double of discretization.
static const Standard_Real MIN_DIST = 2.E-5;

//=======================================================================
//function : AnalizeWire
//purpose  : 
//=======================================================================
void BRepMesh_Classifier::AnalizeWire (const TColgp_SequenceOfPnt2d&  theSeqPnt2d,
                                       const Standard_Real theUmin,  const Standard_Real theUmax,
                                       const Standard_Real theVmin,  const Standard_Real theVmax)
{
  const Standard_Integer aNbPnts = theSeqPnt2d.Length();
  if (aNbPnts < 2)
    return;

  // Accumulate angle
  TColgp_Array1OfPnt2d aPClass(1, aNbPnts);
  Standard_Real anAngle = 0.0;
  gp_Pnt2d p1 = theSeqPnt2d(1), p2 = theSeqPnt2d(2), p3;
  aPClass(1) = p1;
  aPClass(2) = p2;
  for (Standard_Integer i = 1; i <= aNbPnts; i++)
  { 
    Standard_Integer ii = i + 2;
    if (ii > aNbPnts)
    {
      p3 = aPClass(ii - aNbPnts);
    }
    else
    {
      p3 = theSeqPnt2d.Value(ii);
      aPClass(ii) = p3;
    }

    gp_Vec2d A(p1,p2), B(p2,p3);
    if (A.SquareMagnitude() > 1.e-16 && B.SquareMagnitude() > 1.e-16)
    {
      const Standard_Real aCurAngle    = A.Angle(B);
      const Standard_Real aCurAngleAbs = Abs(aCurAngle);
      // Check if vectors are opposite
      if (aCurAngleAbs > Precision::Angular() && (M_PI - aCurAngleAbs) > Precision::Angular())
      {
        anAngle += aCurAngle;
        p1 = p2;
      }
    }
    p2 = p3;
  }
  // Check for zero angle - treat self intersecting wire as outer
  if (Abs(anAngle) < Precision::Angular())
    anAngle = 0.0;

  myTabClass.Append( (void *)new CSLib_Class2d(aPClass, myTolUV, myTolUV,
                                               theUmin, theVmin, theUmax, theVmax) );
  myTabOrient.Append( ((anAngle < 0.0) ? 0 : 1) );
}

//=======================================================================
//function : triangle2Area
//purpose  : calculating area under triangle
//=======================================================================
inline static Standard_Real triangle2Area(const gp_XY& p1, const gp_XY& p2)
{
  return p1.Crossed(p2);
}

//=======================================================================
//function : getSegmentParams
//purpose  : extracting segment attributes 
//=======================================================================
static Standard_Real getSegmentParams(const BRepMesh_Array1OfBiPoint& theBiPoints,
                                      const Standard_Integer Index,
                                      Standard_Real& x11,
                                      Standard_Real& y11,
                                      Standard_Real& x12,
                                      Standard_Real& y12,
                                      Standard_Real& A,
                                      Standard_Real& B,
                                      Standard_Real& C)
{
  Standard_Real *aCoordinates;
  aCoordinates = ((Standard_Real*)(theBiPoints(Index).Coordinates()));
  x11 =  aCoordinates[0];
  y11 =  aCoordinates[1];
  x12 =  aCoordinates[2];
  y12 =  aCoordinates[3];
  A   =  aCoordinates[5];
  B   = -aCoordinates[4];
  C   = - x11*A - y11*B;
  return A*A+B*B;
}

//=======================================================================
//function : checkWiresIntersection
//purpose  : finding intersection.
//           If the intersection is found return Standard_True
//=======================================================================
static Standard_Boolean checkWiresIntersection(const Standard_Integer           theFirstWireId,
                                               const Standard_Integer           theSecondWireId,
                                               Standard_Integer* const          theFirstOuterSegmentId,
                                               Standard_Integer                 theLastOuterSegmentId,
                                               const TColStd_SequenceOfInteger& theWireLength,
                                               const BRepMesh_Array1OfBiPoint&  theBiPoints,
                                               const Standard_Boolean           findNextIntersection   = Standard_False,
                                               const Standard_Boolean           isFirstSegment         = Standard_False,
                                               Standard_Integer* const          theFirstInnerSegmentId = 0)
{
  Standard_Real A1, B1, C1, A2, B2, C2, AB, BC, CA, xc, yc;
  Standard_Real mu1, d, mu2;
  Standard_Integer ik = *theFirstOuterSegmentId, jk;
  Standard_Real x11, x12, y11, y12, x21, x22, y21, y22;

  // Calculate bounds for first wire
  Standard_Integer ikEnd = theLastOuterSegmentId;
  Standard_Boolean isFirst = Standard_True;
  if ( findNextIntersection )
    isFirst = isFirstSegment;

  // Calculate bounds for second wire
  Standard_Integer jkStart = 0, jkEnd = 0;
  for (jk = 1; jk <= theSecondWireId; jk++)
  {
    jkStart = jkEnd + 1;
    jkEnd  += theWireLength(jk);
  }

  // total area under polygon (area of loop)
  Standard_Real aLoopArea          = 0.0;
  // area under first triangles of polygon
  Standard_Real aFirstTriangleArea = 0.0;
  // contains coordinates of the end point of segment if first intersection point is finding
  // or coordinates of the intersecting point if second intersection point is finding
  gp_XY aStartPoint;

  for (; ik <= ikEnd; ik++)
  {
    mu1 = getSegmentParams(theBiPoints, ik, x11, y11, x12, y12, A1, B1, C1);
    // for second intersection point we must count the area from first intersection point 
    if ( !findNextIntersection )
    {
      aLoopArea = 0.0;
      aStartPoint.SetCoord(x12, y12);
    }

    //for theFirstWireId == theSecondWireId the algorithm check current wire on selfintersection
    if ( findNextIntersection && theFirstInnerSegmentId && isFirst)
      jk = *theFirstInnerSegmentId;
    else if (theSecondWireId == theFirstWireId)
      jk = ik + 2;
    else
      jk = jkStart;

    // Explore second wire
    Standard_Boolean aFirstPass = Standard_True;
    for (; jk <= jkEnd; jk++)
    {
      // don't check end's segment of the wire on selfrestriction
      if ( theSecondWireId == theFirstWireId && isFirst && jk == ikEnd )
        continue;

      mu2 = getSegmentParams(theBiPoints, jk, x21, y21, x22, y22, A2, B2, C2);
      gp_XY p2(x21, y21), p3(x22, y22);

      //different segments may have common vertex (see OCC287 bug for example)
      AB = A1*B2 - A2*B1;
      //check on minimal of distance between current segment and points of another linear segments - OCC319
      d = A1*x22 + B1*y22 + C1;
      Standard_Real dTol = MIN_DIST*MIN_DIST;
      if(theFirstWireId != theSecondWireId       && // if compared wires are different &&
         AB*AB > PARALL_COND*PARALL_COND*mu1*mu2 && // angle between two segments greater then PARALL_COND &&
         d*d   < dTol*mu1 &&                        // distance between vertex of the segment and other one's less then MIN_DIST
         (x22-x11)*(x22-x12) < 0.0 && (y22-y11)*(y22-y12) < 0.0)
      {
        // if we finding the second intersection we must return Standard_False for setting
        // self-intersection result flag
        if ( findNextIntersection )
          return Standard_False;

        // we can step here when finding first intersection, return self-intersection flag
        return Standard_True;
      }

      if( aFirstPass )
        aFirstTriangleArea = triangle2Area(aStartPoint, p2);
      
      Standard_Real aTmpArea = triangle2Area(p2, p3);

      //look for intersection of two linear segments
      if(Abs(AB) <= RESOLUTION)
      {
        aLoopArea += aTmpArea;
        continue;  //current segments seem parallel - no intersection
      }
      
      //calculate coordinates of point of the intersection
      BC = B1*C2 - B2*C1;  xc = BC/AB;
      CA = C1*A2 - C2*A1;  yc = CA/AB;

      // remember current intersection point and area of first triangle
      if( findNextIntersection && ik == *theFirstOuterSegmentId && jk == *theFirstInnerSegmentId )
      {
        aStartPoint.SetCoord(xc, yc);
        continue;
      }

      //check on belonging of intersection point to the both of segments
      Standard_Boolean isOnLines = Standard_True;

      Standard_Real dd[2][4] = { {(xc-x11), (xc-x12), (xc-x21), (xc-x22)},   //dX
                                 {(yc-y11), (yc-y12), (yc-y21), (yc-y22)} }; //dY

      for( Standard_Integer i = 0; i < 2; i++ )
      {
        if ( dd[i][0] * dd[i][1] > RESOLUTION || dd[i][2] * dd[i][3] > RESOLUTION )
        {
          isOnLines = Standard_False;
          break;
        }
      }

      // check the intersection point is on the ends of segments
      if ( isOnLines )
      {
        for( Standard_Integer i = 0; i < 2; i++ )
        {
          //     if it's the last segment and intersection point lies at the end
          if ( ( jk == jkEnd                                              ||
          //     dX                        && dY
          //     or when the start or the end point of the first segment
                (Abs(dd[0][0])  < MIN_DIST && Abs(dd[1][0])   < MIN_DIST) ||
                (Abs(dd[0][1])  < MIN_DIST && Abs(dd[1][1])   < MIN_DIST)) &&
          //     is equal to one of the end points of the second
               (Abs(dd[0][i+2]) < MIN_DIST && Abs(dd[1][i+2]) < MIN_DIST))
          {
            // no intersection
            isOnLines = Standard_False;
            aLoopArea = aTmpArea = 0.0;
            aFirstPass = Standard_True;
            break;
          }
        }
      }


      if( isOnLines )
      {
        p3.SetX(xc); p3.SetY(yc); 
        aLoopArea += aFirstTriangleArea;             // First triangle area
        aLoopArea += triangle2Area(p2, p3); 
        aLoopArea += triangle2Area(p3, aStartPoint); // Last triangle area

        if( Abs(aLoopArea)/2 > M_PI*MIN_DIST )
        {
          if ( findNextIntersection )
          {
            // intersection is found, but Standard_False returns, because area is too much
            return Standard_False;
          }

          if ( checkWiresIntersection(theFirstWireId, theSecondWireId, &ik, ikEnd, theWireLength,
                           theBiPoints, Standard_True, isFirst, &jk) )
          {
            // small crossing is not intersection, continue cheching
            aLoopArea = aTmpArea = 0.0;
            aFirstPass = Standard_True;
          }
          else
          {
            // if we found only one intersection
            return Standard_True;
          }
        }
        else if ( findNextIntersection )
        {
          // small intersection, skip double checking
          *theFirstOuterSegmentId = ik;
          *theFirstInnerSegmentId = jk + 1;
          return Standard_True;
        }
      }
      if ( aFirstPass )
        aFirstPass = Standard_False;

      aLoopArea += aTmpArea;
    }
    
    if ( isFirst )
      isFirst = Standard_False;
  }
  return Standard_False;
}


//=======================================================================
//function : BRepMesh_Classifier
//purpose  : 
//=======================================================================
BRepMesh_Classifier::BRepMesh_Classifier(const TopoDS_Face& theFace,
                                         const Standard_Real theTolUV,
                                         const BRepMesh_DataMapOfShapePairOfPolygon& theEdges,
                                         const TColStd_IndexedMapOfInteger& theMap,
                                         const Handle(BRepMesh_DataStructureOfDelaun)& theStructure,
                                         const Standard_Real theUmin,
                                         const Standard_Real theUmax,
                                         const Standard_Real theVmin,
                                         const Standard_Real theVmax)
: myTolUV( theTolUV ),
  myFace ( theFace ),
  myState( BRepMesh_NoError )
{
  //-- impasse sur les surfs definies sur plus d une periode
  //-- once definition
  myFace.Orientation(TopAbs_FORWARD);
  
  TColgp_SequenceOfPnt2d    aWirePoints, aWire;
  TColStd_SequenceOfInteger aWireLength;

  TopoDS_Iterator aFaceExplorer;
  for(aFaceExplorer.Initialize(myFace); aFaceExplorer.More(); aFaceExplorer.Next())
  {
    if(aFaceExplorer.Value().ShapeType() != TopAbs_WIRE)
      continue;

    // For each wire we create a data map, linking vertices (only
    // the ends of edges) with their positions in the sequence of
    // all 2d points from this wire.
    // When we meet some vertex for the second time - the piece
    // of sequence is treated for a HOLE and quits the sequence.
    // Actually, we must unbind the vertices belonging to the
    // loop from the map, but since they can't appear twice on the
    // valid wire, leave them for a little speed up.
    Standard_Integer aNbEdges = 0;
    Standard_Integer aFirstIndex = 0, aLastIndex = 0;
    Standard_Boolean isFalseWire = Standard_False;

    TColgp_SequenceOfPnt2d aSeqPnt2d;
    TColStd_DataMapOfIntegerInteger aNodeInSeq;

    // Start traversing the wire
    BRepTools_WireExplorer aWireExplorer;
    for (aWireExplorer.Init(TopoDS::Wire( aFaceExplorer.Value() ), myFace); aWireExplorer.More(); aWireExplorer.Next())
    {
      TopoDS_Edge        anEdge   = aWireExplorer.Current();
      TopAbs_Orientation anOrient = anEdge.Orientation();
      if (anOrient != TopAbs_FORWARD && anOrient != TopAbs_REVERSED)
        continue;

      if (theEdges.IsBound(anEdge))
      {
        // Retrieve polygon
        // Define the direction for adding points to aSeqPnt2d
        Standard_Integer aIdxFirst, aIdxLast, aIdxIncr;

        const BRepMesh_PairOfPolygon& aPair = theEdges.Find(anEdge);
        Handle(Poly_PolygonOnTriangulation) aNOD;
        if (anOrient == TopAbs_FORWARD)
        {
          aNOD = aPair.First();
          aIdxFirst = 1;
          aIdxLast  = aNOD->NbNodes();
          aIdxIncr  = 1;
        }
        else
        {
          aNOD = aPair.Last();
          aIdxFirst = aNOD->NbNodes();
          aIdxLast  = 1;
          aIdxIncr  = -1;
        }
        const TColStd_Array1OfInteger& anIndices = aNOD->Nodes();

        // anIndexFirst and anIndexLast are the indices of first and last
        // vertices of the edge in IndexedMap <Str>
        const Standard_Integer anIndexFirst = theMap.FindKey( anIndices(aIdxFirst) );
        const Standard_Integer anIndexLast  = theMap.FindKey( anIndices(aIdxLast) );

        if (anIndexLast == anIndexFirst && (aIdxLast - aIdxFirst) == aIdxIncr)
        {
          // case of continuous set of degenerated edges
          aLastIndex = anIndexLast;
          continue;
        }

        // If there's a gap between edges -> raise <isFalseWire> flag
        if (aNbEdges)
        {
          if (anIndexFirst != aLastIndex)
          {
            isFalseWire = Standard_True;
            break;
          }
        }
        else
          aFirstIndex = anIndexFirst;

        aLastIndex = anIndexLast;

        // Record first vertex (to detect loops)
        aNodeInSeq.Bind(anIndexFirst, (aSeqPnt2d.Length() + 1));

        // Add vertices in sequence
        for (Standard_Integer i = aIdxFirst; i != aIdxLast; i += aIdxIncr)
        {
          Standard_Integer anIndex = ((i == aIdxFirst) ? anIndexFirst : theMap.FindKey( anIndices(i) ));

          gp_Pnt2d aPnt( theStructure->GetNode(anIndex).Coord() );
          aSeqPnt2d.Append(aPnt);
        }

        // Now, is there a loop?
        if (aNodeInSeq.IsBound(anIndexLast))
        {
          // Yes, treat it separately as a hole
          // 1. Divide points into main wire and a loop
          const Standard_Integer aIdxWireStart = aNodeInSeq(anIndexLast);
          if(aIdxWireStart < aSeqPnt2d.Length())
          {
            aSeqPnt2d.Split(aIdxWireStart, aWire);
            // 2. Proceed the loop
            //AnalizeWire(aLoop, Umin, Umax, Vmin, Vmax, aWirePoints, aWireLength, NbBiPoint);
            aWireLength.Append( aWire.Length() );
            aWirePoints.Append( aWire );
          }
        }
        aNbEdges++;
      }
    }

    if (aNbEdges)
    {
      // Isn't it open?
      if (isFalseWire || (aFirstIndex != aLastIndex) || aSeqPnt2d.Length() > 1)
      {
        myState = BRepMesh_OpenWire;
        return;
      }
    }
  }

  const Standard_Integer aNbWires = aWireLength.Length();
  Standard_Integer aNbBiPoint = aWirePoints.Length();
  BRepMesh_Array1OfBiPoint aBiPoints(0, aNbBiPoint);
  BRepMesh_BiPoint *aBiPoint = &(aBiPoints.ChangeValue(1));

  // Fill array of segments (bi-points)
  Standard_Integer k = 1;
  for (Standard_Integer i = 1; i <= aNbWires; i++)
  {
    Standard_Real x1 = 0., y1 = 0., x2, y2, aXstart = 0., aYstart = 0.;
    const Standard_Integer aLen = aWireLength(i) + 1;
    for (Standard_Integer j = 1; j <= aLen; j++)
    {
      // Obtain last point of the segment
      if (j == aLen)
      {
        x2 = aXstart;
        y2 = aYstart;
      }
      else
      {
        const gp_Pnt2d& aPnt = aWirePoints(k);
        k++;

        x2 = aPnt.X();
        y2 = aPnt.Y();
      }
      // Build segment (bi-point)
      if (j == 1)
      {
        aXstart = x2;
        aYstart = y2;
      }
      else
      {
        Standard_Real *aCoordinates1 = ((Standard_Real*)(aBiPoint->Coordinates()));
        aBiPoint++;

        aCoordinates1[0] = x1;
        aCoordinates1[1] = y1;
        aCoordinates1[2] = x2;
        aCoordinates1[3] = y2;
        aCoordinates1[4] = x2 - x1;
        aCoordinates1[5] = y2 - y1;
      }
      x1 = x2;
      y1 = y2;
    }
  }

  // Search the intersection
  // Explore first wire
  Standard_Integer ikEnd = 0;
  for(Standard_Integer i = 1; i <= aNbWires; i++)
  {
    Standard_Integer ik = ikEnd + 1;
    ikEnd += aWireLength(i);

    // Explore second wire
    for (Standard_Integer j = i; j <= aNbWires; j++)
    {
      if ( checkWiresIntersection(i, j, &ik, ikEnd, aWireLength, aBiPoints) )
      {
        myState = BRepMesh_SelfIntersectingWire;
        return;
      }
    }
  }

  // Find holes
  for (Standard_Integer i = aNbWires; i >= 1; i--)
  {
    aNbBiPoint = aWirePoints.Length() - aWireLength(i) + 1;
    aWirePoints.Split(aNbBiPoint, aWire);
    AnalizeWire(aWire, theUmin, theUmax, theVmin, theVmax);
  }
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
TopAbs_State BRepMesh_Classifier::Perform(const gp_Pnt2d& thePoint) const
{
  Standard_Boolean isOut = Standard_False;
  Standard_Integer aNb   = myTabClass.Length();
  
  for (Standard_Integer i = 1; i <= aNb; i++)
  {
    Standard_Integer aCur = ((CSLib_Class2d*)myTabClass(i))->SiDans(thePoint);
    if (aCur == 0)
    {
      // Point is ON, but mark it as OUT
      isOut = Standard_True;
    }
    else
      isOut = myTabOrient(i)? (aCur == -1) : (aCur == 1);
    
    if (isOut)
      return TopAbs_OUT;
  }

  return TopAbs_IN;
}


//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================
void BRepMesh_Classifier::Destroy()
{
  Standard_Integer aNb = myTabClass.Length();
  for (Standard_Integer i = 1; i <= aNb; i++)
  {
    if (myTabClass(i))
    {
      delete ((CSLib_Class2d*)myTabClass(i));
      myTabClass(i) = NULL;
    }
  }
}
