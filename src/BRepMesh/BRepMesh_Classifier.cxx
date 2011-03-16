// File:	BRepMesh_Classifier.cxx
// Created:	Thu Jun 26 14:54:02 1997
// Author:	Laurent PAINNOT
//		<lpa@penox.paris1.matra-dtv.fr>


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
static const Standard_Real PARALL_COND = Sin(PI/3.0);
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
      if (aa > Precision::Angular() && (PI - aa) > Precision::Angular())
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
//function : BRepMesh_Classifier
//purpose  : 
//=======================================================================

BRepMesh_Classifier::BRepMesh_Classifier(const TopoDS_Face& aFace,
					 const Standard_Real TolUV,
					 const BRepMesh_DataMapOfShapePairOfPolygon& edges,
					 const TColStd_IndexedMapOfInteger& themap,
					 const BRepMesh_IndexedMapOfVertex& Str,
					 const Standard_Real Umin,
					 const Standard_Real Umax,
					 const Standard_Real Vmin,
					 const Standard_Real Vmax):
					 Toluv(TolUV), Face(aFace),  
					 myState(BRepMesh_NoError),
					 isnatural(Standard_False)
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
#ifdef DEB_MESH
    if (debclass) { debwire++;  cout <<endl;  cout << "#wire no "<<debwire; debedge = 0;}
#endif
    if(FaceExplorer.Value().ShapeType() != TopAbs_WIRE)
      continue;
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

	// indexFirst and nodeLast are the indices of first and last
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

          gp_Pnt2d vp(Str(index).Coord());
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

  // Check natural restriction
  const Standard_Integer nbwires = aWireLength.Length();
  if (nbwires == 1 && nbedges == 4)
  {
    Handle(Geom2d_Curve) C2d;
    Standard_Real pfbid, plbid;
    isnatural = Standard_True;
    for(FaceExplorer.Initialize(Face); FaceExplorer.More(); FaceExplorer.Next())
    {
      if(FaceExplorer.Value().ShapeType() != TopAbs_WIRE)
        continue;
      TopoDS_Iterator aEdgeIt(FaceExplorer.Value());
      for( ; aEdgeIt.More(); aEdgeIt.Next())
      {
        edge = TopoDS::Edge(aEdgeIt.Value());
        if(anOr == TopAbs_FORWARD || anOr == TopAbs_REVERSED)
        {
          C2d = BRep_Tool::CurveOnSurface(edge,Face,pfbid,plbid);
          //OCC316(APO): if(!IsLine(C2d)) { isnatural = Standard_False; break; }
          if(!C2d.IsNull() && !IsLine(C2d)) { isnatural = Standard_False; break; }
          else
          { // sont-ce des isos:
            gp_Pnt2d P1, P2;
            C2d->D0(pfbid, P1);
            C2d->D0(plbid, P2);
            if ((Abs(P1.X()-P2.X()) > 1.e-04) && (Abs(P1.Y()-P2.Y()) > 1.e-04)) { isnatural = Standard_False; break; }
          }
        }
      }
    }
  }

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

  Standard_Real *Coordinates2;
  Standard_Real A1, B1, C1, A2, B2, C2, AB, BC, CA, xc, yc;
  Standard_Real  mu1, d, mu2;
  Standard_Integer ik, ikEnd = 0, jk, jkEnd;
  Standard_Real x11, x12, y11, y12, x21, x22, y21, y22;
  for(i = 1; i <= nbwires; i++)
  {
    ik = ikEnd + 1;  ikEnd += aWireLength(i);
    // Explore first wire
    for (; ik <= ikEnd; ik++)
    {
      Coordinates1 = ((Standard_Real*)(BiPoints.ChangeValue(ik).Coordinates()));
      x11 = Coordinates1[0];
      y11 = Coordinates1[1];
      x12 = Coordinates1[2];
      y12 = Coordinates1[3];
      A1 =  Coordinates1[5];
      B1 = -Coordinates1[4];
      C1 = - x11*A1 - y11*B1;
      //mu1 = Sqrt(A1*A1+B1*B1);
      mu1 = A1*A1+B1*B1;
      for (j = i; j <= nbwires; j++)
      {
        //for i==j the algorithm check current wire on selfintersection
	if (j == i)
        {
          jk = ik + 2;  jkEnd = ikEnd;
        }
        else
        {
          jk = jkEnd + 1;  jkEnd = jk + aWireLength(j) - 1;
        }
        // Explore second wire
	for (; jk <= jkEnd; jk++)
        {
          // don't check end's segment of the wire on selfrestriction
	  if (jk == ikEnd) continue;
	  Coordinates2 = ((Standard_Real*)(BiPoints.ChangeValue(jk).Coordinates()));
	  x21 = Coordinates2[0];
          y21 = Coordinates2[1];
          x22 = Coordinates2[2];
          y22 = Coordinates2[3];
	  A2 =  Coordinates2[5];
          B2 = -Coordinates2[4];
          C2 = - x21*A2 - y21*B2;
	  //mu2 = Sqrt(A2*A2+B2*B2);
          mu2 = A2*A2+B2*B2;
	  //different segments may have common vertex (see OCC287 bug for example)
	  //if(x22 == x11 && y22 == y11){ myState = BRepMesh_OpenWire;  return;}
	  AB = A1*B2 - A2*B1;
	  //check on minimal of distance between current segment and points of another linear segments - OCC319
	  //d = Abs(A1*x22 + B1*y22 + C1);
          d = A1*x22 + B1*y22 + C1;
	  if(i != j &&                        // if compared wires are different &&
	     AB*AB > PARALL_COND*PARALL_COND*mu1*mu2 && // angle between two segments greater then PARALL_COND &&
	     d*d < MIN_DIST*MIN_DIST*mu1 &&              // distance between vertex of the segment and other one's less then MIN_DIST
	     (x22-x11)*(x22-x12) < 0.0 && (y22-y11)*(y22-y12) < 0.0)
          {
	    myState = BRepMesh_SelfIntersectingWire;  return;
	  }
	  //look for intersection of two linear segments
	  if(Abs(AB) <= RESOLUTION) continue;  //current segments seem parallel - no intersection
	  //calculate coordinates of point of the intersection
	  BC = B1*C2 - B2*C1;  xc = BC/AB;
	  CA = C1*A2 - C2*A1;  yc = CA/AB;
          if( Abs(xc-x11) > RESOLUTION && Abs(xc-x12) > RESOLUTION &&
              Abs(yc-y11) > RESOLUTION && Abs(yc-y12) > RESOLUTION &&
              Abs(xc-x21) > RESOLUTION && Abs(xc-x22) > RESOLUTION &&
              Abs(yc-y21) > RESOLUTION && Abs(yc-y22) > RESOLUTION )
          {
	    //check on belonging of intersection point to the both of segments
	    if((xc-x11)*(xc-x12) < 0.0 && (yc-y11)*(yc-y12) < 0.0 &&
	       (xc-x21)*(xc-x22) < 0.0 && (yc-y21)*(yc-y22) < 0.0)
            {
	      //different segments may have common vertex (why "<" but "<=")
	      myState = BRepMesh_SelfIntersectingWire;  return;
            }
	  }
        }
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


//Wind code duplication

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
					 myState(BRepMesh_NoError),
					 isnatural(Standard_False)
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

	// indexFirst and nodeLast are the indices of first and last
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

  // Check natural restriction
  const Standard_Integer nbwires = aWireLength.Length();
  if (nbwires == 1 && nbedges == 4)
  {
    Handle(Geom2d_Curve) C2d;
    Standard_Real pfbid, plbid;
    isnatural = Standard_True;
    for(FaceExplorer.Initialize(Face); FaceExplorer.More(); FaceExplorer.Next())
    {
      if(FaceExplorer.Value().ShapeType() != TopAbs_WIRE)
        continue;
      TopoDS_Iterator aEdgeIt(FaceExplorer.Value());
      for( ; aEdgeIt.More(); aEdgeIt.Next())
      {
        edge = TopoDS::Edge(aEdgeIt.Value());
        if(anOr == TopAbs_FORWARD || anOr == TopAbs_REVERSED)
        {
          C2d = BRep_Tool::CurveOnSurface(edge,Face,pfbid,plbid);
          //OCC316(APO): if(!IsLine(C2d)) { isnatural = Standard_False; break; }
          if(!C2d.IsNull() && !IsLine(C2d)) { isnatural = Standard_False; break; }
          else
          { // sont-ce des isos:
            gp_Pnt2d P1, P2;
            C2d->D0(pfbid, P1);
            C2d->D0(plbid, P2);
            if ((Abs(P1.X()-P2.X()) > 1.e-04) && (Abs(P1.Y()-P2.Y()) > 1.e-04)) { isnatural = Standard_False; break; }
          }
        }
      }
    }
  }

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

  Standard_Real *Coordinates2;
  Standard_Real A1, B1, C1, A2, B2, C2, AB, BC, CA, xc, yc;
  Standard_Real  mu1, d, mu2;
  Standard_Integer ik, ikEnd = 0, jk, jkEnd;
  Standard_Real x11, x12, y11, y12, x21, x22, y21, y22;
  for(i = 1; i <= nbwires; i++)
  {
    ik = ikEnd + 1;  ikEnd += aWireLength(i);
    // Explore first wire
    for (; ik <= ikEnd; ik++)
    {
      Coordinates1 = ((Standard_Real*)(BiPoints.ChangeValue(ik).Coordinates()));
      x11 = Coordinates1[0];
      y11 = Coordinates1[1];
      x12 = Coordinates1[2];
      y12 = Coordinates1[3];
      A1 =  Coordinates1[5];
      B1 = -Coordinates1[4];
      C1 = - x11*A1 - y11*B1;
      //mu1 = Sqrt(A1*A1+B1*B1);
      mu1 = A1*A1+B1*B1;
      for (j = i; j <= nbwires; j++)
      {
        //for i==j the algorithm check current wire on selfintersection
	if (j == i)
        {
          jk = ik + 2;  jkEnd = ikEnd;
        }
        else
        {
          jk = jkEnd + 1;  jkEnd = jk + aWireLength(j) - 1;
        }
        // Explore second wire
	for (; jk <= jkEnd; jk++)
        {
          // don't check end's segment of the wire on selfrestriction
	  if (jk == ikEnd) continue;
	  Coordinates2 = ((Standard_Real*)(BiPoints.ChangeValue(jk).Coordinates()));
	  x21 = Coordinates2[0];
          y21 = Coordinates2[1];
          x22 = Coordinates2[2];
          y22 = Coordinates2[3];
	  A2 =  Coordinates2[5];
          B2 = -Coordinates2[4];
          C2 = - x21*A2 - y21*B2;
	  //mu2 = Sqrt(A2*A2+B2*B2);
          mu2 = A2*A2+B2*B2;
	  //different segments may have common vertex (see OCC287 bug for example)
	  //if(x22 == x11 && y22 == y11){ myState = BRepMesh_OpenWire;  return;}
	  AB = A1*B2 - A2*B1;
	  //check on minimal of distance between current segment and points of another linear segments - OCC319
	  //d = Abs(A1*x22 + B1*y22 + C1);
          d = A1*x22 + B1*y22 + C1;
	  if(i != j &&                        // if compared wires are different &&
	     AB*AB > PARALL_COND*PARALL_COND*mu1*mu2 && // angle between two segments greater then PARALL_COND &&
	     d*d < MIN_DIST*MIN_DIST*mu1 &&              // distance between vertex of the segment and other one's less then MIN_DIST
	     (x22-x11)*(x22-x12) < 0.0 && (y22-y11)*(y22-y12) < 0.0)
          {
	    myState = BRepMesh_SelfIntersectingWire;  return;
	  }
	  //look for intersection of two linear segments
	  if(Abs(AB) <= RESOLUTION) continue;  //current segments seem parallel - no intersection
	  //calculate coordinates of point of the intersection
	  BC = B1*C2 - B2*C1;  xc = BC/AB;
	  CA = C1*A2 - C2*A1;  yc = CA/AB;
	  //check on belonging of intersection point to the both of segments
          if( Abs(xc-x11) > RESOLUTION && Abs(xc-x12) > RESOLUTION &&
              Abs(yc-y11) > RESOLUTION && Abs(yc-y12) > RESOLUTION &&
              Abs(xc-x21) > RESOLUTION && Abs(xc-x22) > RESOLUTION &&
              Abs(yc-y21) > RESOLUTION && Abs(yc-y22) > RESOLUTION )
          {
            if((xc-x11)*(xc-x12) < 0.0 && (yc-y11)*(yc-y12) < 0.0 &&
	      (xc-x21)*(xc-x22) < 0.0 && (yc-y21)*(yc-y22) < 0.0)
            {
	      //different segments may have common vertex (why "<" but "<=")
	      myState = BRepMesh_SelfIntersectingWire;  return;
	    }
          }
	}
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
