// File:	BOP_WireEdgeSet.cxx
// Created:	Thu Jun 17 17:21:05 1993
// Author:	Jean Yves LEBEY
//		<jyl@zerox>
// modified by NIZNHY-PKV 

#include <BOP_WireEdgeSet.ixx>

#include <Standard_ProgramError.hxx>

#include <Precision.hxx>

#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>

#include <TopoDS.hxx>
#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>

#include <BOPTools_Tools2D.hxx>

// modified by NIZHNY-MKK  Fri May 30 10:18:01 2003.BEGIN
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <BOPTools_Tools3D.hxx>

static void GetOrientationVOnE(const TopoDS_Shape& V,
			       const TopoDS_Shape& E1,
			       const TopoDS_Shape& E2,
			       TopAbs_Orientation& o1,
			       TopAbs_Orientation& o2);
// modified by NIZHNY-MKK  Fri May 30 10:18:03 2003.END

//=======================================================================
//function : BOP_WireEdgeSet::BOP_WireEdgeSet
//purpose  : 
//=======================================================================
  BOP_WireEdgeSet::BOP_WireEdgeSet()
:
  BOP_ShapeSet(TopAbs_VERTEX)
{
}
//=======================================================================
//function : BOP_WireEdgeSet::BOP_WireEdgeSet
//purpose  : 
//=======================================================================
  BOP_WireEdgeSet::BOP_WireEdgeSet(const TopoDS_Face& F)
: 
  BOP_ShapeSet(TopAbs_VERTEX),
  myFace(F)
{
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================
  void BOP_WireEdgeSet::Initialize(const TopoDS_Face& F)
{
  myFace=F;
}

//=======================================================================
//function : AddShape
//purpose  : 
//=======================================================================
  void BOP_WireEdgeSet::AddShape(const TopoDS_Shape& S)
{
  ProcessAddShape(S);
}

//=======================================================================
//function : AddStartElement
//purpose  : 
//=======================================================================
  void BOP_WireEdgeSet::AddStartElement(const TopoDS_Shape& S)
{
  ProcessAddStartElement(S);
}

//=======================================================================
//function : AddElement
//purpose  : 
//=======================================================================
  void BOP_WireEdgeSet::AddElement(const TopoDS_Shape& S)
{
  BOP_ShapeSet::AddElement(S);
}

//=======================================================================
//function : InitNeighbours
//purpose  : 
//=======================================================================
  void  BOP_WireEdgeSet::InitNeighbours(const TopoDS_Shape& E)
{
  mySubShapeExplorer.Init(E,mySubShapeType);
  myCurrentShape = E;

  FindNeighbours();
}


//=======================================================================
//function : FindNeighbours
//purpose  : 
//=======================================================================
  void BOP_WireEdgeSet::FindNeighbours()
{
  while (mySubShapeExplorer.More()) {
    
    // l = list of edges neighbour of edge myCurrentShape trough
    // the vertex mySubShapeExplorer.Current(), which is a vertex of the
    // edge myCurrentShape.
    const TopoDS_Shape& V = mySubShapeExplorer.Current();
    
    const TopTools_ListOfShape & l = 
      MakeNeighboursList(myCurrentShape, V);

    // myIncidentShapesIter iterates on the neighbour edges of the edge
    // given as InitNeighbours() argument (this edge has been stored 
    // in the field myCurrentShape).

    myIncidentShapesIter.Initialize(l);
    if (myIncidentShapesIter.More()) {
      break;
    }
    else{
      mySubShapeExplorer.Next();
    }
  }
}

//=======================================================================
//function : MakeNeighboursList
//purpose  : find edges connected to Earg by Varg 
//=======================================================================
  const TopTools_ListOfShape & BOP_WireEdgeSet::MakeNeighboursList(const TopoDS_Shape& Earg,
								   const TopoDS_Shape& Varg)
{
  const TopoDS_Edge&   E = TopoDS::Edge(Earg);
  const TopoDS_Vertex& V = TopoDS::Vertex(Varg);

  const TopTools_ListOfShape& l = mySubShapeMap.FindFromKey(Varg);

  Standard_Integer nclosing = NbClosingShapes(l);
  if (nclosing) { 
    // build myCurrentShapeNeighbours = 
    // edge list made of connected shapes to Earg through Varg
    myCurrentShapeNeighbours.Clear();
    
    Standard_Integer iapp = 0;
    for (TopTools_ListIteratorOfListOfShape it(l); it.More(); it.Next()) {
      iapp++;
      const TopoDS_Shape& curn = it.Value(); // current neighbour
      Standard_Boolean k = VertexConnectsEdgesClosing(Varg, Earg, curn);
      if (k) {
	myCurrentShapeNeighbours.Append(curn);
      }
    }

    Standard_Integer newn = NbClosingShapes(myCurrentShapeNeighbours);
    if (newn >= 2 ) {

      const TopoDS_Face& F = myFace;
      // several sewing edges connected to E by V such as :
      // orientation of V in E # orientation of V in its edges.
      // one leaves among the connected sewing edges,
      // only edge A that checks tg(E) ^ tg(A) > 0

      TopAbs_Orientation Eori;
      gp_Vec2d d1E; gp_Pnt2d pE;
      Standard_Real fiE, laE, tolpc, parE;
      
      parE = BRep_Tool::Parameter(V,E);
      
      Handle(Geom2d_Curve) PCE;
      BOPTools_Tools2D::CurveOnSurface(E, F, PCE, fiE, laE, tolpc, Standard_False);

      if (!PCE.IsNull()){
	PCE->D1(parE, pE, d1E);
      }
      else  {
	LocalD1 (F, E, V, pE, d1E);
      }

      Eori = E.Orientation();
      if (Eori == TopAbs_REVERSED) {
	d1E.Reverse();
      }

      TopTools_ListIteratorOfListOfShape lclo(myCurrentShapeNeighbours);
      Standard_Integer rang = 0;
      while (lclo.More()) {
	rang++;

	if ( ! IsClosed(lclo.Value()) ) {
	  lclo.Next();
	  continue;
	}

	const TopoDS_Edge& EE = TopoDS::Edge(lclo.Value());
	Standard_Real parEE,  fiEE, laEE, tolpcc;
	gp_Vec2d d1EE; gp_Pnt2d pEE;
	
	parEE = BRep_Tool::Parameter(V,EE);
	
	Handle(Geom2d_Curve) PCEE;
	BOPTools_Tools2D::CurveOnSurface (EE, F, PCEE, fiEE, laEE, tolpcc, Standard_False);//
	//
	if (!PCEE.IsNull()) {
	  PCEE->D1(parEE,pEE,d1EE);
	
	}
	else {
	  LocalD1(F, EE, V, pEE, d1EE);
	}

	TopAbs_Orientation EEori = EE.Orientation();
	if (EEori == TopAbs_REVERSED) d1EE.Reverse();

	Standard_Real cross = d1E.Crossed(d1EE);
	TopAbs_Orientation oVE,oVEE;

	// modified by NIZHNY-MKK  Fri May 30 10:17:10 2003.BEGIN
	GetOrientationVOnE(V,E,EE,oVE,oVEE);
// 	VertexConnectsEdges(V,E,EE,oVE,oVEE);
	// modified by NIZHNY-MKK  Fri May 30 10:17:13 2003.END

	Standard_Boolean t2 = ( (cross > 0) && oVE == TopAbs_REVERSED ) ||
	                      ( (cross < 0) && oVE == TopAbs_FORWARD );

	if ( t2 ) { //-- t1
	  // this is proper IsClosed, only it is preserved among IsClosed 
	  lclo.Next();
	}
	else {
	  // edge IsClosed is suspended
	  myCurrentShapeNeighbours.Remove(lclo);
	}
      }// end of while (lclo.More())
    }// end of if (newn >= 2 )
    return myCurrentShapeNeighbours;
  }// end of if (nclosing)

  else {
    return l;
  }
} // MakeNeighoursList

//=======================================================================
//function : VertexConnectsEdges
//purpose  : 
//=======================================================================
  Standard_Boolean BOP_WireEdgeSet::VertexConnectsEdges(const TopoDS_Shape& V,
							const TopoDS_Shape& E1,
							const TopoDS_Shape& E2,
							TopAbs_Orientation& o1,
							TopAbs_Orientation& o2) const
{
  TopExp_Explorer ex1, ex2;
  ex1.Init(E1,TopAbs_VERTEX);
  for(; ex1.More(); ex1.Next()) {
    
    if (V.IsSame(ex1.Current())) {
      ex2.Init(E2,TopAbs_VERTEX);
      for(; ex2.More(); ex2.Next()) {
	
	if (V.IsSame(ex2.Current())) {
	  o1 = ex1.Current().Orientation();
	  o2 = ex2.Current().Orientation();
	  if ( o1 != o2 ) {
	    return Standard_True;
	  }
	}
      }
    }
  }
  return Standard_False;
}


//=======================================================================
//function : VertexConnectEdgesClosing
//purpose  : 
//=======================================================================
  Standard_Boolean BOP_WireEdgeSet::VertexConnectsEdgesClosing(const TopoDS_Shape& V, 
							       const TopoDS_Shape& E1, 
							       const TopoDS_Shape& E2) const
{
//  Let S the set of incident edges on vertex V.
//  S contains at least one closed edge on the periodic face to build.
// (the face implied in closing test of edge is myFace)
//  E1,E2 are S shapes (sharing V).
//  
//  if E1 and E2 are not closed : edges are NOT connected
//  if E1 or E2 is/are closed :
//    if V changes of relative orientation between E1,E2 : edges are connected
//    else : edges are NOT connected
//  
//  example with E1 NOT closed, E2 closed :
//
//  E1 FORWARD, V REVERSED on E1 
//  E2 FORWARD, V FORWARD on E2       --> edges are connected
//
//  E1 FORWARD, V REVERSED on E1 
//  E2 REVERSED, V FORWARD on E2      --> edges are NOT connected
//-----------------------------------------------------------------------

  Standard_Boolean c1 = IsClosed(E1);
  Standard_Boolean c2 = IsClosed(E2);

  Standard_Boolean testconnect = c1 || c2;
  Standard_Boolean resu = Standard_False;
  TopAbs_Orientation o1,o2;

  // SSCONNEX = False ==> E2 is  selected to create as many 
  // (default)           faces as there are components connected in UV.
  // SSCONNEX = True ==> the entire edge E2 sharing V with E1 is taken
  //                     so that orientation(V/E1) # orientation(V/E2)
  //                     ==> face of part and other sewings
  if ((c1 && c2)) {
    Standard_Boolean u1 = c1 ? IsUClosed(E1) : Standard_False; 
    Standard_Boolean v1 = c1 ? IsVClosed(E1) : Standard_False; 
    Standard_Boolean u2 = c2 ? IsUClosed(E2) : Standard_False; 
    Standard_Boolean v2 = c2 ? IsVClosed(E2) : Standard_False; 
    Standard_Boolean uvdiff = (u1 && v2) || (u2 && v1);
    testconnect = uvdiff;
  }

  if (testconnect) {
    resu = VertexConnectsEdges(V,E1,E2,o1,o2);
  }
  else { 
    // cto 012 O2 sewing edge of cylindric face
    // chain of components split ON and OUT of the same orientation
    TopAbs_Orientation oe1 = E1.Orientation();
    TopAbs_Orientation oe2 = E2.Orientation();
    Standard_Boolean iseq = E1.IsEqual(E2);
    if ( (c1 && c2) && (oe1 == oe2) && (!iseq) ) {
      resu = VertexConnectsEdges(V,E1,E2,o1,o2);
    }
  }
  return resu;
}

//=======================================================================
//function : NbClosingShapes
//purpose  : 
//=======================================================================
  Standard_Integer BOP_WireEdgeSet::NbClosingShapes(const TopTools_ListOfShape & L) const
{
  Standard_Integer n = 0;
  TopTools_ListIteratorOfListOfShape it(L);
  for ( ; it.More(); it.Next()) {
    const TopoDS_Shape& S = it.Value();
    if ( IsClosed(S) ) {
      n++;
    }
  }
  return n;
}

//=======================================================================
//function : LocalD1
//purpose  : 
//=======================================================================
  void BOP_WireEdgeSet::LocalD1(const TopoDS_Face&  F,
				const TopoDS_Edge&  E,
				const TopoDS_Vertex& V,
				gp_Pnt2d& ,//pE, 
				gp_Vec2d& d1E) const
{
  TopLoc_Location Loc;
  Standard_Real parE, fiE, laE, u, v;
  
  parE = BRep_Tool::Parameter(V,E);
   
  Handle(Geom_Curve) CE = BRep_Tool::Curve(E, Loc, fiE, laE);
  CE = Handle(Geom_Curve)::DownCast(CE->Transformed(Loc.Transformation()));
  
  gp_Pnt p3dE; gp_Vec d3dE;
  CE->D1(parE, p3dE, d3dE);
  
  Handle(Geom_Surface) S = BRep_Tool::Surface(F);
  GeomAPI_ProjectPointOnSurf proj(p3dE,S);
 
  proj.LowerDistanceParameters(u,v);
  gp_Pnt bid; 
  gp_Vec d1u,d1v;
  S->D1(u,v,bid,d1u,d1v);
  u = d3dE.Dot(d1u);
  v = d3dE.Dot(d1v);
  d1E.SetCoord(u,v);
}

//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================
  Standard_Boolean BOP_WireEdgeSet::IsClosed(const TopoDS_Shape& E) const
{
  const TopoDS_Edge& EE = TopoDS::Edge(E);
  Standard_Boolean closed = BRep_Tool::IsClosed(EE,myFace);

  // modified by NIZHNY-MKK  Wed May 12 15:44:10 2004.BEGIN
  if(closed) {
    TopoDS_Edge aE1, aE2;
    BOPTools_Tools3D::GetSeam (myFace, aE1, aE2);

    if(aE1.IsNull() || aE2.IsNull()) {
      closed = Standard_False;
    }
  }
  // modified by NIZHNY-MKK  Wed May 12 15:44:14 2004.END

  return closed;
}

//=======================================================================
//function : IsUVISO
//purpose  : 
//=======================================================================
  void BOP_WireEdgeSet::IsUVISO(const TopoDS_Edge& E,
				const TopoDS_Face& F,
				Standard_Boolean& uiso,
				Standard_Boolean& viso) 
{
  uiso = viso = Standard_False;
  Standard_Real fE, lE,tolpc;
  
  Handle(Geom2d_Curve) PC;
  BOPTools_Tools2D::CurveOnSurface (E, F, PC, fE, lE, tolpc, Standard_False);//
  
  if (PC.IsNull()) {
    Standard_ProgramError::Raise("BOP_WireEdgeSet::IsUVISO");
  }
  
  Handle(Standard_Type) TheType = PC->DynamicType();
  if (TheType == STANDARD_TYPE(Geom2d_Line)) {
    const Handle(Geom2d_Line)& HL = *((Handle(Geom2d_Line)*)&PC);
    const gp_Dir2d&  D = HL->Direction();
    Standard_Real    tol = Precision::Angular();
    if      (D.IsParallel(gp_Dir2d(0.,1.),tol)) {
      uiso = Standard_True;
    }    
    else if (D.IsParallel(gp_Dir2d(1.,0.),tol)) {
      viso = Standard_True;
    }
  }
}

//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================
  Standard_Boolean BOP_WireEdgeSet::IsUClosed(const TopoDS_Shape& E) const
{
  const TopoDS_Edge& EE = TopoDS::Edge(E);

  Standard_Boolean bid, closed;
  IsUVISO(EE, myFace, closed, bid);
  return closed;
}


//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================
  Standard_Boolean BOP_WireEdgeSet::IsVClosed(const TopoDS_Shape& E) const
{
  const TopoDS_Edge& EE = TopoDS::Edge(E);

  Standard_Boolean bid, closed;
  IsUVISO (EE, myFace, bid, closed);
  return closed;
}

//=======================================================================
//function : Face
//purpose  : 
//=======================================================================
  const TopoDS_Face& BOP_WireEdgeSet::Face() const 
{
  return myFace;
}
//=======================================================================
//function : BOP_Tools::KeptTwice
//purpose  : 
//=======================================================================
  Standard_Boolean BOP_WireEdgeSet::KeptTwice (const TopoDS_Edge& aSpEF2) const
					 

{
  Standard_Integer iCnt=0;

  const TopTools_ListOfShape& aWESList=StartElements();

  TopTools_ListIteratorOfListOfShape anIt(aWESList);
  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Shape& aSS=anIt.Value();
    if (aSpEF2.IsSame(aSS)) {
      iCnt++;
      if (iCnt > 1) {
	return Standard_True;
      }
    }
  }
 return Standard_False;
}
//=======================================================================
//function : BOP_Tools::RemoveEdgeFromWES
//purpose  : 
//=======================================================================
  void BOP_WireEdgeSet::RemoveEdgeFromWES(const TopoDS_Edge& aSpEF2)
				    
{
  TopTools_ListOfShape aTmpList;
  aTmpList=StartElements();
  
  ClearContents();

  TopTools_ListIteratorOfListOfShape anIt(aTmpList);
  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Shape& aSS=anIt.Value();
    if (!aSpEF2.IsEqual(aSS)) {
      AddStartElement(aSS);
    }
  }
}


// modified by NIZHNY-MKK  Fri May 30 10:18:38 2003.BEGIN
void GetOrientationVOnE(const TopoDS_Shape& V,
			const TopoDS_Shape& E1,
			const TopoDS_Shape& E2,
			TopAbs_Orientation& o1,
			TopAbs_Orientation& o2) {
  TopTools_IndexedMapOfShape aMap;

  TopExp::MapShapes(E1, TopAbs_VERTEX, aMap);
  TopExp::MapShapes(E2, TopAbs_VERTEX, aMap);

  if(aMap.Extent() > 1) {
    TopExp_Explorer ex1, ex2;
    ex1.Init(E1,TopAbs_VERTEX);

    for(; ex1.More(); ex1.Next()) {
    
      if (V.IsSame(ex1.Current())) {
	ex2.Init(E2,TopAbs_VERTEX);

	for(; ex2.More(); ex2.Next()) {

	  if (V.IsSame(ex2.Current())) {
	    o1 = ex1.Current().Orientation();
	    o2 = ex2.Current().Orientation();

	    if ( o1 != o2 ) {
	      return;
	    }
	  }
	}
      }
    }
  }
  else {
    TopoDS_Shape VV = V;

    if(E1.Orientation() == TopAbs_REVERSED)
      VV.Reverse();
    o1 = VV.Orientation();
    VV.Reverse();
    o2 = VV.Orientation();
  }
  return;
}
// modified by NIZHNY-MKK  Fri May 30 10:18:40 2003.END
