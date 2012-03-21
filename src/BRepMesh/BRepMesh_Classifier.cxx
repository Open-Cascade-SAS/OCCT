// Created on: 1997-06-26
// Created by: Laurent PAINNOT
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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

#ifdef DEB_MESH
static Standard_Integer debwire;
static Standard_Integer debedge;
static Standard_Integer debclass = 0;
#endif
static const Standard_Real MIN_DIST = 2.E-5; //EPA: real mesh is created in the grid 10E5x10E5, so intersection should be cheched
                                             //     with double of discretization.
static const Standard_Real PARALL_COND = Sin(M_PI/3.0);
static const Standard_Real RESOLUTION = 1.0E-16; //OCC319


//=======================================================================
//function : IsLine
//purpose  : 
//=======================================================================

static Standard_Boolean IsLine(const Handle(Geom2d_Curve)& C2d)
{
  Standard_Boolean IsALine = Standard_False;
  if ( C2d->IsKind(STANDARD_TYPE(Geom2d_Line)) )
  {
    IsALine = Standard_True;
  }
  else if ( C2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)) )
  {
    Handle(Geom2d_BSplineCurve) BS = *((Handle(Geom2d_BSplineCurve)*)&C2d);
    IsALine = (BS->NbPoles() == 2);
  }
  else if ( C2d->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)) )
  {
    Handle(Geom2d_BezierCurve) Bz = *((Handle(Geom2d_BezierCurve)*)&C2d);
    IsALine = (Bz->NbPoles() == 2);
  }
  else if ( C2d->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)) )
  {
    Handle(Geom2d_TrimmedCurve) Curv = *((Handle(Geom2d_TrimmedCurve)*)&C2d);
    IsALine = IsLine(Curv->BasisCurve());
  }
  return IsALine;
}

//=======================================================================
//function : AnalizeWire
//purpose  : 
//=======================================================================

void BRepMesh_Classifier::AnalizeWire (const TColgp_SequenceOfPnt2d&  theSeqPnt2d,
                                       const Standard_Real Umin,  const Standard_Real Umax,
                                       const Standard_Real Vmin,  const Standard_Real Vmax)
{
  const Standard_Integer nbpnts = theSeqPnt2d.Length();
  if (nbpnts < 2) return;

  // Accumulate angle
  TColgp_Array1OfPnt2d PClass(1,nbpnts);
  Standard_Integer i, ii;
  Standard_Real theangle = 0.0;
  gp_Pnt2d p1 = theSeqPnt2d(1), p2 = theSeqPnt2d(2), p3;
  PClass(1) = p1;
  PClass(2) = p2;
  for (i = 1; i <= nbpnts; i++)
  { 
    ii = i + 2;
    if (ii > nbpnts)
    {
      p3 = PClass(ii-nbpnts);
    }
    else
    {
      p3 = theSeqPnt2d.Value(ii);
      PClass(ii) = p3;
    }
    gp_Vec2d A(p1,p2), B(p2,p3);
    if (A.SquareMagnitude() > 1.e-16 && B.SquareMagnitude() > 1.e-16)
    {
      const Standard_Real a = A.Angle(B);
      const Standard_Real aa = Abs(a);
      // Check if vectors are opposite
      if (aa > Precision::Angular() && (M_PI - aa) > Precision::Angular())
      {
        theangle += a;
        p1 = p2;
      }
    }
    p2 = p3;
  }
  // Check for zero angle - treat self intersecting wire as outer
  if (Abs(theangle) < Precision::Angular()) theangle = 0.0;

  TabClass.Append((void *)new CSLib_Class2d(PClass,Toluv,Toluv,Umin,Vmin,Umax,Vmax));
  TabOrien.Append((theangle < 0.0) ? 0 : 1);
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

static Standard_Boolean checkWiresIntersection(const Standard_Integer            theFirstWireId,
                                              const Standard_Integer            theSecondWireId,
                                              Standard_Integer* const           theFirstOuterSegmentId,
                                              Standard_Integer                  theLastOuterSegmentId,
                                              const TColStd_SequenceOfInteger&  theWireLength,
                                              const BRepMesh_Array1OfBiPoint&   theBiPoints,
                                              const Standard_Boolean            findNextIntersection   = Standard_False,
                                              const Standard_Boolean            isFirstSegment         = Standard_False,
                                              Standard_Integer* const           theFirstInnerSegmentId = 0)
{
  Standard_Real A1, B1, C1, A2, B2, C2, AB, BC, CA, xc, yc;
  Standard_Real  mu1, d, mu2;
  Standard_Integer ik = *theFirstOuterSegmentId, jk;
  Standard_Real x11, x12, y11, y12, x21, x22, y21, y22;

  // Calculate bounds for first wire
  Standard_Integer ikEnd = theLastOuterSegmentId;
  Standard_Boolean isFirst = Standard_True;
  if ( findNextIntersection )
  {
    isFirst = isFirstSegment;
  }

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
    {
      jk = *theFirstInnerSegmentId;
    }
    else if (theSecondWireId == theFirstWireId)
    {
      jk = ik + 2;
    }
    else
    {
      jk = jkStart;
    }

    // Explore second wire
    Standard_Boolean aFirstPass = Standard_True;
    for (; jk <= jkEnd; jk++)
    {
      // don't check end's segment of the wire on selfrestriction
      if ( theSecondWireId == theFirstWireId && isFirst && jk == ikEnd ) continue;

      mu2 = getSegmentParams(theBiPoints, jk, x21, y21, x22, y22, A2, B2, C2);
      gp_XY p2(x21, y21), p3(x22, y22);

      //different segments may have common vertex (see OCC287 bug for example)
      AB = A1*B2 - A2*B1;
      //check on minimal of distance between current segment and points of another linear segments - OCC319
      d = A1*x22 + B1*y22 + C1;
      Standard_Real dTol = MIN_DIST*MIN_DIST;
      if(theFirstWireId != theSecondWireId       && // if compared wires are different &&
         AB*AB > PARALL_COND*PARALL_COND*mu1*mu2 && // angle between two segments greater then PARALL_COND &&
         d*d   < dTol*mu1 &&             // distance between vertex of the segment and other one's less then MIN_DIST
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
      {
        aFirstTriangleArea = triangle2Area(aStartPoint, p2);
      }
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

      Standard_Integer i = 0;
      for(; i < 2; i++ )
      {
        if ( dd[i][0]*dd[i][1] > dTol || dd[i][2]*dd[i][3] > dTol)
        {
          isOnLines = Standard_False;
          break;
        }
      }

      // check the intersection point is on the ends of segments
      if ( isOnLines )
      {
        for( i = 0; i < 2; i++ )
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
      {
        aFirstPass = Standard_False;
      }

      aLoopArea += aTmpArea;
    }
    
    if ( isFirst )
    {
      isFirst = Standard_False;
    }
  }
  return Standard_False;
}


//=======================================================================
//function : BRepMesh_Classifier
//purpose  : 
//=======================================================================

BRepMesh_Classifier::BRepMesh_Classifier(const TopoDS_Face& aFace,
                                         const Standard_Real TolUV,
                                         const BRepMesh_DataMapOfShapePairOfPolygon& edges,
                                         const TColStd_IndexedMapOfInteger& themap,
                                         const Handle(BRepMesh_DataStructureOfDelaun)& Str,
                                         const Standard_Real Umin,
                                         const Standard_Real Umax,
                                         const Standard_Real Vmin,
                                         const Standard_Real Vmax):
                                         Toluv(TolUV), Face(aFace),
                                         myState(BRepMesh_NoError)
{
  //-- impasse sur les surfs definies sur plus d une periode

  //-- once definition
  Face.Orientation(TopAbs_FORWARD);

  TopoDS_Edge  edge;
  BRepTools_WireExplorer WireExplorer;
  //TopExp_Explorer FaceExplorer;
  TopoDS_Iterator FaceExplorer;

  TColgp_SequenceOfPnt2d aWirePoints, aWire;
  TColStd_SequenceOfInteger aWireLength;


  //-- twice definitions
  TopAbs_Orientation anOr = TopAbs_FORWARD;
  Standard_Boolean falsewire = Standard_False;
  Standard_Integer i, index, firstindex = 0, lastindex = 0, nbedges = 0;
#ifdef DEB_MESH
  debwire = 0;
#endif

  for(FaceExplorer.Initialize(Face); FaceExplorer.More(); FaceExplorer.Next())
  {
    if( FaceExplorer.Value().ShapeType()!= TopAbs_WIRE)
      continue;
#ifdef DEB_MESH
    if (debclass) { debwire++;  cout <<endl;  cout << "#wire no "<<debwire; debedge = 0;}
#endif
    // For each wire we create a data map, linking vertices (only
    // the ends of edges) with their positions in the sequence of
    // all 2d points from this wire.
    // When we meet some vertex for the second time - the piece
    // of sequence is treated for a HOLE and quits the sequence.
    // Actually, we must unbind the vertices belonging to the
    // loop from the map, but since they can't appear twice on the
    // valid wire, leave them for a little speed up.
    nbedges = 0;
    TColgp_SequenceOfPnt2d SeqPnt2d;
    TColStd_DataMapOfIntegerInteger NodeInSeq;
    // Start traversing the wire
    for (WireExplorer.Init(TopoDS::Wire(FaceExplorer.Value()),Face); WireExplorer.More(); WireExplorer.Next())
    {
      edge = WireExplorer.Current();
#ifdef DEB_MESH
      if (debclass) { debedge++; cout << endl; cout << "#edge no "<<debedge <<endl;}
#endif
      anOr = edge.Orientation();
      if (anOr != TopAbs_FORWARD && anOr != TopAbs_REVERSED) continue;
      if (edges.IsBound(edge))
      {
        // Retrieve polygon
        // Define the direction for adding points to SeqPnt2d
        Standard_Integer iFirst,iLast,iIncr;
        const BRepMesh_PairOfPolygon& pair = edges.Find(edge);
        Handle(Poly_PolygonOnTriangulation) NOD;
        if (anOr == TopAbs_FORWARD)
        {
          NOD = pair.First();
          iFirst = 1;
          iLast  = NOD->NbNodes();
          iIncr  = 1;
        }
        else
        {
          NOD = pair.Last();
          iFirst = NOD->NbNodes();
          iLast  = 1;
          iIncr  = -1;
        }
        const TColStd_Array1OfInteger& indices = NOD->Nodes();

        // indexFirst and indexLast are the indices of first and last
        // vertices of the edge in IndexedMap <Str>
        const Standard_Integer indexFirst = themap.FindKey(indices(iFirst));
        const Standard_Integer indexLast = themap.FindKey(indices(iLast));

        // Skip degenerated edge : OCC481(apo)
        if (indexLast == indexFirst && (iLast-iFirst) == iIncr) continue;

        // If there's a gap between edges -> raise <falsewire> flag
        if (nbedges)
        {
          if (indexFirst != lastindex)
          {
            falsewire = Standard_True;
            break;
          }
        }
        else firstindex = indexFirst;
              lastindex = indexLast;

        // Record first vertex (to detect loops)
        NodeInSeq.Bind(indexFirst,SeqPnt2d.Length()+1);

        // Add vertices in sequence
        for (i = iFirst; i != iLast; i += iIncr)
        {
          index = (i == iFirst)? indexFirst : themap.FindKey(indices(i));

          gp_Pnt2d vp(Str->GetNode(index).Coord());
          SeqPnt2d.Append(vp);
#ifdef DEB_MESH
          if (debclass) cout<<"point p"<<index<<" "<<vp.X()<<" "<< vp.Y()<<endl;
#endif
        }

        // Now, is there a loop?
        if (NodeInSeq.IsBound(indexLast))
        {
          // Yes, treat it separately as a hole
          // 1. Divide points into main wire and a loop
          const Standard_Integer iWireStart = NodeInSeq(indexLast);
          if(iWireStart < SeqPnt2d.Length()) {
            SeqPnt2d.Split(iWireStart, aWire);
            //OCC319->  the operation will be done later
            // 2. Proceed the loop
            //AnalizeWire(aLoop, Umin, Umax, Vmin, Vmax, aWirePoints, aWireLength, NbBiPoint);
            aWireLength.Append(aWire.Length());
            aWirePoints.Append(aWire);
            //<-OCC319
          }
        }
        nbedges++;
      }
    }

    if (nbedges)
    {
      // Isn't it open?
      if (falsewire || (firstindex != lastindex) || SeqPnt2d.Length() > 1)
      {
        myState = BRepMesh_OpenWire;
        return;
      }
    }
    else
    {
#ifdef DEB_MESH
      cout <<"Warning : empty wire" <<endl;
#endif
    }
  }

  const Standard_Integer nbwires = aWireLength.Length();
  Standard_Integer NbBiPoint = aWirePoints.Length();
  BRepMesh_Array1OfBiPoint BiPoints(0,NbBiPoint);

  BRepMesh_BiPoint *BP;
  Standard_Real *Coordinates1;
  Standard_Real x1, y1, x2, y2, xstart, ystart;
  Standard_Integer j, l = 1;
  BP = &(BiPoints.ChangeValue(1));

  // Fill array of segments (bi-points)
  for (i = 1; i <= nbwires; i++)
  {
    const Standard_Integer len = aWireLength(i) + 1;
    for (j = 1; j <= len; j++)
    {
      // Obtain last point of the segment
      if (j == len)
      {
        x2 = xstart;
        y2 = ystart;
      }
      else
      {
        const gp_Pnt2d& PT = aWirePoints(l); l++;
        x2 = PT.X();
        y2 = PT.Y();
      }
      // Build segment (bi-point)
      if (j == 1)
      {
        xstart = x2;
        ystart = y2;
      }
      else
      {
        Coordinates1 = ((Standard_Real*)(BP->Coordinates())); BP++;
        Coordinates1[0] = x1;
        Coordinates1[1] = y1;
        Coordinates1[2] = x2;
        Coordinates1[3] = y2;
        Coordinates1[4] = x2 - x1;
        Coordinates1[5] = y2 - y1;
      }
      x1 = x2;
      y1 = y2;
    }
  }

  // Search the intersection
  // Explore first wire
  Standard_Integer ik, ikEnd = 0;
  for(i = 1; i <= nbwires; i++)
  {
    ik = ikEnd + 1;  ikEnd += aWireLength(i);
    // Explore second wire
    for (j = i; j <= nbwires; j++)
    {
      if ( checkWiresIntersection(i, j, &ik, ikEnd, aWireLength, BiPoints) )
      {
        myState = BRepMesh_SelfIntersectingWire; return;
      }
    }
  }

  // Find holes
  for (i = nbwires; i >= 1; i--)
  {
    NbBiPoint = aWirePoints.Length() - aWireLength(i) + 1;
    aWirePoints.Split(NbBiPoint, aWire);
    AnalizeWire(aWire, Umin, Umax, Vmin, Vmax);
  }
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

TopAbs_State BRepMesh_Classifier::Perform(const gp_Pnt2d& aPoint) const
{
  Standard_Boolean isOut = Standard_False;

  Standard_Integer cur, i, nb = TabClass.Length();
  
  for (i = 1; i <= nb; i++)
  {
    cur = ((CSLib_Class2d*)TabClass(i))->SiDans(aPoint);
    if (cur == 0)
    {
      // Point is ON, but mark it as OUT
      isOut = Standard_True;
    }
    else
    {
      isOut = TabOrien(i)? (cur == -1) : (cur == 1);
    }
    if (isOut) return TopAbs_OUT;
  }

  return TopAbs_IN;
}


//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================

void BRepMesh_Classifier::Destroy()
{
  Standard_Integer i, nb = TabClass.Length();
  for (i = 1; i <= nb; i++)
  {
    if (TabClass(i))
    {
      delete ((CSLib_Class2d*)TabClass(i));
      TabClass(i) = NULL;
    }
  }
}
