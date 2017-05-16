// Created on: 1993-06-17
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#ifdef DRAW
#include <DBRep.hxx>
static TCollection_AsciiString PRODINS("dins ");
#endif

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_CurveTool.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRep_GettraceSHA(const Standard_Integer i);
extern Standard_Boolean TopOpeBRepBuild_GettraceSS();
extern Standard_Boolean TopOpeBRepBuild_GetcontextSSCONNEX();
extern Standard_Boolean TopOpeBRepBuild_GettraceCHK();
TopOpeBRepBuild_Builder* LOCAL_PBUILDER_DEB = NULL;
void debwesclo(const Standard_Integer) {}
#endif

//=======================================================================
//function : TopOpeBRepBuild_WireEdgeSet
//purpose  : 
//=======================================================================
TopOpeBRepBuild_WireEdgeSet::TopOpeBRepBuild_WireEdgeSet(const TopoDS_Shape& F,
                                                         const Standard_Address /*A*/) :
TopOpeBRepBuild_ShapeSet(TopAbs_VERTEX)
{
  myFace = TopoDS::Face(F);

#ifdef DRAW
  myDEBName = "WES";
  LOCAL_PBUILDER_DEB = (TopOpeBRepBuild_Builder*)((void*)A);
  if (LOCAL_PBUILDER_DEB != NULL) {
    myDEBNumber = LOCAL_PBUILDER_DEB->GdumpSHASETindex();
    Standard_Integer iF; Standard_Boolean tSPS = LOCAL_PBUILDER_DEB->GtraceSPS(F,iF);
    if(tSPS){DumpName(cout,"creation ");cout<<" on ";}
    if(tSPS){LOCAL_PBUILDER_DEB->GdumpSHA(F,NULL);cout<<endl;}
  }
  if (TopOpeBRepBuild_GettraceCHK() && !myCheckShape) {
    DumpName(cout,"no checkshape in creation of ");cout<<endl;
  }
#endif
}

//=======================================================================
//function : AddShape
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_WireEdgeSet::AddShape(const TopoDS_Shape& S)
{
  Standard_Boolean tocheck = Standard_True;
  Standard_Boolean iswire = ( S.ShapeType() == TopAbs_WIRE );
  if ( iswire ) {
    BRepAdaptor_Surface bas(myFace,Standard_False);
    Standard_Boolean uc = bas.IsUClosed();
    Standard_Boolean vc = bas.IsVClosed();
    if ( uc || vc ) tocheck = Standard_False;
  }
  Standard_Boolean chk = Standard_True;
  if ( tocheck ) chk = CheckShape(S);
  
#ifdef DRAW
  if (TopOpeBRepBuild_GettraceCHK() && CheckShape()) {
    if (!tocheck) DumpCheck(cout," AddShape WIRE on closed face",S,chk);
    else DumpCheck(cout," AddShape redefined",S,chk);
  }
#endif

  if (!chk) return;
  ProcessAddShape(S);
}

//=======================================================================
//function : AddStartElement
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_WireEdgeSet::AddStartElement(const TopoDS_Shape& S)
{
#ifdef OCCT_DEBUG
  
#endif
  Standard_Boolean tocheck = Standard_True;
  Standard_Boolean isedge = ( S.ShapeType() == TopAbs_EDGE );
  if ( isedge ) {
    BRepAdaptor_Curve cac(TopoDS::Edge(S));
    GeomAbs_CurveType t = cac.GetType();
    Standard_Boolean b = (t==GeomAbs_BSplineCurve || t==GeomAbs_BezierCurve);
    tocheck = !b;
  }
  Standard_Boolean chk = Standard_True;
  if ( tocheck ) chk = CheckShape(S);

  if (!chk) return;
  ProcessAddStartElement(S);
}

//=======================================================================
//function : AddElement
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_WireEdgeSet::AddElement(const TopoDS_Shape& S)
{
  TopOpeBRepBuild_ShapeSet::AddElement(S);
}

//=======================================================================
//function : Face
//purpose  : 
//=======================================================================
const TopoDS_Face& TopOpeBRepBuild_WireEdgeSet::Face() const 
{
  return myFace;
}

//=======================================================================
//function : InitNeighbours
//purpose  : 
//=======================================================================
void  TopOpeBRepBuild_WireEdgeSet::InitNeighbours(const TopoDS_Shape& E)
{

#ifdef DRAW
  Standard_Boolean traceSS = TopOpeBRepBuild_GettraceSS();
  Standard_Integer ista=myOMSS.FindIndex(E);Standard_Boolean tsh=(ista)?TopOpeBRep_GettraceSHA(ista) : Standard_False;

  if (traceSS || tsh) {
    TCollection_AsciiString str("#**** InitNeighbours");
    if (tsh) str = str + " on WES edge " + SNameori(E);
    str = str + " #****";
    cout<<endl<<str<<endl;
    if (tsh) debwesclo(ista);
  }
#endif

  mySubShapeExplorer.Init(E,mySubShapeType);
  myCurrentShape = E;
  FindNeighbours();
}


//=======================================================================
//function : FindNeighbours
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_WireEdgeSet::FindNeighbours()
{
  while (mySubShapeExplorer.More()) {
    
    // l = list of edges neighbour of edge myCurrentShape trough
    // the vertex mySubShapeExplorer.Current(), which is a vertex of the
    // edge myCurrentShape.
    const TopoDS_Shape& V = mySubShapeExplorer.Current();
    const TopTools_ListOfShape & l = MakeNeighboursList(myCurrentShape,V);

    // myIncidentShapesIter iterates on the neighbour edges of the edge
    // given as InitNeighbours() argument (this edge has been stored 
    // in the field myCurrentShape).

    myIncidentShapesIter.Initialize(l);
    if (myIncidentShapesIter.More()) break;
    else mySubShapeExplorer.Next();
  }
}


//=======================================================================
//function : MakeNeighboursList
//purpose  : recherche des edges connexes a Earg par Varg 
//=======================================================================
const TopTools_ListOfShape & TopOpeBRepBuild_WireEdgeSet::MakeNeighboursList(const TopoDS_Shape& Earg, const TopoDS_Shape& Varg)
{
  const TopoDS_Edge& E = TopoDS::Edge(Earg);
  const TopoDS_Vertex& V = TopoDS::Vertex(Varg);
  const TopTools_ListOfShape& l = mySubShapeMap.FindFromKey(V);

  Standard_Integer nclosing = NbClosingShapes(l);

#ifdef DRAW
  Standard_Boolean traceSS = TopOpeBRepBuild_GettraceSS();
  if ( traceSS ) {
    TCollection_AsciiString svel = SNameVEL(V,E,l);
    cout<<PRODINS<<svel<<"; #---- WES MNL"<<endl;
  }
#endif

  if (nclosing) { 
    // build myCurrentShapeNeighbours = 
    // edge list made of connected shapes to Earg through Varg

    myCurrentShapeNeighbours.Clear();
    
    Standard_Integer iapp = 0;
    for (TopTools_ListIteratorOfListOfShape it(l); it.More(); it.Next()) {
      iapp++;
      const TopoDS_Shape& curn = it.Value(); // current neighbour
      Standard_Boolean k = VertexConnectsEdgesClosing(V,E,curn);
      if (k) {
	myCurrentShapeNeighbours.Append(curn);

#ifdef DRAW
	if ( traceSS ) {
	  Standard_Integer rang = myCurrentShapeNeighbours.Extent();
	  const TopoDS_Edge& EE = TopoDS::Edge(curn);
	  cout <<"+ EE "<<iapp<<" , rang "<<rang<<" ";
	  TCollection_AsciiString svee=SNameVEE(V,E,EE);cout<<svee<<endl;
	}
#endif
      }
    }

    Standard_Integer newn = NbClosingShapes(myCurrentShapeNeighbours);
#ifdef DRAW
    if ( traceSS ) {
      cout<<"#~~~~connexes apres VertexConnectsEdgesClosing : ";
      TCollection_AsciiString svel=SNameVEL(V,E,myCurrentShapeNeighbours);
      cout<<svel<<endl;
    }
#endif

    if (newn >= 2 ) {

      const TopoDS_Face& F = myFace;

      // plusieurs aretes de couture connexes a E par V et telles que :
      // orientation de V dans E # orientation de V dans ces aretes.
      // on ne garde,parmi les aretes de couture connexes,
      // que l'arete A qui verifie tg(E) ^ tg(A) > 0

      gp_Vec2d d1E; gp_Pnt2d pE;
      Standard_Real parE = BRep_Tool::Parameter(V,E);
      Standard_Real fiE,laE,tolpc;
      Standard_Boolean trim3d = Standard_True;
      Handle(Geom2d_Curve) PCE = FC2D_CurveOnSurface(E,F,fiE,laE,tolpc,trim3d);

      if (!PCE.IsNull()) PCE->D1(parE,pE,d1E);
      else               LocalD1(F,E,V,pE,d1E);

      TopAbs_Orientation Eori = E.Orientation();
      if (Eori == TopAbs_REVERSED) d1E.Reverse();

      TopTools_ListIteratorOfListOfShape lclo(myCurrentShapeNeighbours);
      Standard_Integer rang = 0;
      while (lclo.More()) {
	rang++;

	if ( ! IsClosed(lclo.Value()) ) {
	  lclo.Next();
	  continue;
	}

	const TopoDS_Edge& EE = TopoDS::Edge(lclo.Value());
	gp_Vec2d d1EE; gp_Pnt2d pEE;
	Standard_Real parEE = BRep_Tool::Parameter(V,EE);
	Standard_Real fiEE,laEE,tolpc1;
	Handle(Geom2d_Curve) PCEE = FC2D_CurveOnSurface(EE,F,fiEE,laEE,tolpc1,trim3d);
	
	if (!PCEE.IsNull()) PCEE->D1(parEE,pEE,d1EE);
	else                LocalD1(F,EE,V,pEE,d1EE);

	TopAbs_Orientation EEori = EE.Orientation();
	if (EEori == TopAbs_REVERSED) d1EE.Reverse();

	Standard_Real cross = d1E.Crossed(d1EE);
	TopAbs_Orientation oVE,oVEE;
        VertexConnectsEdges(V,E,EE,oVE,oVEE);

	Standard_Boolean t2 = ( (cross > 0) && oVE == TopAbs_REVERSED ) ||
	                      ( (cross < 0) && oVE == TopAbs_FORWARD );

#ifdef DRAW
	if ( traceSS ) {
	  cout<<"#-------- rang "<<rang<<endl;
	  TCollection_AsciiString svee=SNameVEE(V,E,EE);cout<<svee<<endl;
	  
	  cout<<"  fiE,laE : "<<fiE<<" "<<laE<<endl;
	  cout<<" parE "<<parE<<endl;
	  cout<<"  puE,pvE "<<pE.X()<<" "<<pE.Y();
	  cout<<" d1uE,d1vE "<<d1E.X()<<" "<<d1E.Y()<<endl;
	  cout<<endl;
	  cout<<"  fiEE,laEE : "<<fiEE<<" "<<laEE<<endl;
	  cout<<" parEE "<<parEE<<endl;
	  cout<<"  puEE,pvEE "<<pEE.X()<<" "<<pEE.Y();
	  cout<<" d1uEE,d1vEE "<<d1EE.X()<<" "<<d1EE.Y()<<endl;
	  cout<<"  --> cross  "<<cross<<endl;

	  if ( t2 ) cout<<" t2 --> on garde EE"<<endl<<endl;
	  else 	  cout<<" t2 --> on vire EE"<<endl<<endl;
	}
#endif
	
	if ( t2 ) { //-- t1
	  // c'est la bonne IsClosed,on ne garde qu'elle parmi les IsClosed 
	  lclo.Next();
	}
	else {
	  // on vire l'arete IsClosed 
	  myCurrentShapeNeighbours.Remove(lclo);
	}
      }

#ifdef DRAW
      if ( traceSS ) {
	cout<<"#~~~~connexes apres filtre geom : ";
	TCollection_AsciiString svel=SNameVEL(Varg,Earg,myCurrentShapeNeighbours);cout<<svel<<endl;
      }
#endif

    }
    return myCurrentShapeNeighbours;
  }
  else {
    return l;
  }

} // MakeNeighoursList

//=======================================================================
//function : VertexConnectsEdges
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_WireEdgeSet::VertexConnectsEdges(const TopoDS_Shape& V,const TopoDS_Shape& E1,const TopoDS_Shape& E2,TopAbs_Orientation& o1,TopAbs_Orientation& o2) const
{
  TopOpeBRepTool_ShapeExplorer ex1,ex2;
  for(ex1.Init(E1,TopAbs_VERTEX);ex1.More();ex1.Next()) {
    if (V.IsSame(ex1.Current())) {
      for(ex2.Init(E2,TopAbs_VERTEX);ex2.More();ex2.Next()) {
	if (V.IsSame(ex2.Current())) {
	  o1 = ex1.Current().Orientation();
	  o2 = ex2.Current().Orientation();
	  if ( o1 != o2 ) return Standard_True;
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
Standard_Boolean TopOpeBRepBuild_WireEdgeSet::VertexConnectsEdgesClosing(const TopoDS_Shape& V, const TopoDS_Shape& E1, const TopoDS_Shape& E2) const
{

//-----------------------------------------------------------------------
//Standard_Boolean VertexConnectsEdgesClosing :
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

  // SSCONNEX = False ==> on selectionne E2 de facon a creer ulterieurement
  // (defaut)             autant de faces que de composantes connexes en UV.
  // SSCONNEX = True ==> on prend toute arete E2 qui partage V avec E1
  //                     et telle que orientation(V/E1) # orientation(V/E2)
  //                     ==> face de part et d'autre des coutures

#ifdef DRAW
  if ( TopOpeBRepBuild_GetcontextSSCONNEX() ) {
    if (testconnect) resu = VertexConnectsEdges(V,E1,E2,o1,o2);
    return resu;
  }
#endif

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
    // cto 012 O2 arete de couture de face cylindrique
    // chainage des composantes splitees ON et OUT de meme orientation
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
Standard_Integer TopOpeBRepBuild_WireEdgeSet::NbClosingShapes(const TopTools_ListOfShape & L) const
{
  Standard_Integer n = 0;
  for (TopTools_ListIteratorOfListOfShape it(L); it.More(); it.Next()) {
    const TopoDS_Shape& S = it.Value();
    if ( IsClosed(S) ) n++;
  }
  return n;
}

//=======================================================================
//function : LocalD1
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_WireEdgeSet::LocalD1(const TopoDS_Shape& SF,const TopoDS_Shape& SE,const TopoDS_Shape& SV,
		     gp_Pnt2d& pE, gp_Vec2d& d1E) const
{
  const TopoDS_Face&   F = TopoDS::Face(SF); 
  const TopoDS_Edge&   E = TopoDS::Edge(SE);
  const TopoDS_Vertex& V = TopoDS::Vertex(SV);
  Standard_Real parE = BRep_Tool::Parameter(V,E);
  
  TopLoc_Location Loc; Standard_Real fiE,laE;
  Handle(Geom_Curve) CE = BRep_Tool::Curve(E,Loc,fiE,laE);
  CE = Handle(Geom_Curve)::DownCast(CE->Transformed(Loc.Transformation()));
  
  gp_Pnt p3dE; gp_Vec d3dE;
  CE->D1(parE,p3dE,d3dE);
  
  Handle(Geom_Surface) S = BRep_Tool::Surface(F);
  GeomAPI_ProjectPointOnSurf proj(p3dE,S);
  Standard_Real u,v;
  proj.LowerDistanceParameters(u,v);
  pE.SetCoord(u,v);
  gp_Pnt bid; gp_Vec d1u,d1v;
  S->D1(u,v,bid,d1u,d1v);
  u = d3dE.Dot(d1u);
  v = d3dE.Dot(d1v);
  d1E.SetCoord(u,v);
}

//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_WireEdgeSet::IsClosed(const TopoDS_Shape& E) const
{
#ifdef OCCT_DEBUG
  Standard_Integer ista=myOMSS.FindIndex(E);Standard_Boolean tsh=(ista)?TopOpeBRep_GettraceSHA(ista):Standard_False;
  if (tsh) debwesclo(ista);
#endif

  const TopoDS_Edge& EE = TopoDS::Edge(E);
  Standard_Boolean closed = BRep_Tool::IsClosed(EE,myFace);
  if ( closed ) return Standard_True;

  return Standard_False;
}

//=======================================================================
//function : IsUVISO
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_WireEdgeSet::IsUVISO(const TopoDS_Edge& E,const TopoDS_Face& F,Standard_Boolean& uiso,Standard_Boolean& viso) 
{
  uiso = viso = Standard_False;
  Standard_Real fE,lE,tolpc;Handle(Geom2d_Curve) PC;
  Standard_Boolean trim3d = Standard_True;
  PC = FC2D_CurveOnSurface(E,F,fE,lE,tolpc,trim3d);
  if (PC.IsNull()) throw Standard_ProgramError("TopOpeBRepBuild_WireEdgeSet::IsUVISO");

  Handle(Standard_Type) TheType = PC->DynamicType();
  if (TheType == STANDARD_TYPE(Geom2d_Line)) {
    Handle(Geom2d_Line) HL (Handle(Geom2d_Line)::DownCast (PC));
    const gp_Dir2d&  D = HL->Direction();
    Standard_Real    tol = Precision::Angular();

    if      (D.IsParallel(gp_Dir2d(0.,1.),tol)) uiso = Standard_True;
    else if (D.IsParallel(gp_Dir2d(1.,0.),tol)) viso = Standard_True;
  }
}

//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_WireEdgeSet::IsUClosed(const TopoDS_Shape& E) const
{
  const TopoDS_Edge& EE = TopoDS::Edge(E);
  Standard_Boolean bid,closed;
  IsUVISO(EE,myFace,closed,bid);
  return closed;
}


//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_WireEdgeSet::IsVClosed(const TopoDS_Shape& E) const
{
  const TopoDS_Edge& EE = TopoDS::Edge(E);
  Standard_Boolean bid,closed;
  IsUVISO(EE,myFace,bid,closed);
  return closed;
}

//=======================================================================
//function : SNameVEE
//purpose  : 
//=======================================================================
#ifdef DRAW
TCollection_AsciiString TopOpeBRepBuild_WireEdgeSet::SNameVEE(const TopoDS_Shape& VV,
                                                              const TopoDS_Shape& EE1,
                                                              const TopoDS_Shape& EE2) const
{   
  TCollection_AsciiString str;
  const TopoDS_Vertex& V = TopoDS::Vertex(VV);
  const TopoDS_Shape& E1 = TopoDS::Edge(EE1);
  const TopoDS_Shape& E2 = TopoDS::Edge(EE2);
  TopAbs_Orientation oVE1,oVE2; Standard_Boolean conn = VertexConnectsEdges(V,E1,E2,oVE1,oVE2);
  str=SName(VV)+" ";
  str=str+SNameori(E1)+" V/E1 : ";
  TCollection_AsciiString so1 = TopAbs::ShapeOrientationToString (oVE1);
  str=str+so1.SubString(1,1)+" ";
  str=str+SNameori(E2)+" V/E2 : ";
  TCollection_AsciiString so2 = TopAbs::ShapeOrientationToString (oVE2);
  str=str+so2.SubString(1,1)+" ";
  return str;
}
#else
TCollection_AsciiString TopOpeBRepBuild_WireEdgeSet::SNameVEE(const TopoDS_Shape&,
                                                              const TopoDS_Shape&,
                                                              const TopoDS_Shape&) const
{   
  TCollection_AsciiString str;
  return str;
}
#endif

//=======================================================================
//function : SNameVEL
//purpose  : 
//=======================================================================
#ifdef DRAW
TCollection_AsciiString TopOpeBRepBuild_WireEdgeSet::SNameVEL(const TopoDS_Shape& V, const TopoDS_Shape& E,
                                                              const TopTools_ListOfShape& L) const
#else
TCollection_AsciiString TopOpeBRepBuild_WireEdgeSet::SNameVEL(const TopoDS_Shape&, const TopoDS_Shape&,
                                                              const TopTools_ListOfShape&) const
#endif
{
  TCollection_AsciiString str;
#ifdef DRAW
  Standard_Integer nc = NbClosingShapes(L), nl = L.Extent();  
  str=SNameori(E)+" "+SName(V)+" "+SNameori(L);
#endif
  return str;
}

//=======================================================================
//function : DumpSS
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_WireEdgeSet::DumpSS()
{
#ifdef DRAW
  TopOpeBRepBuild_ShapeSet::DumpSS();
#endif
}

//=======================================================================
//function : SName
//purpose  : 
//=======================================================================
#ifdef DRAW
TCollection_AsciiString TopOpeBRepBuild_WireEdgeSet::SName(const TopoDS_Shape& S,
                                                           const TCollection_AsciiString& sb,
                                                           const TCollection_AsciiString& sa) const
#else
TCollection_AsciiString TopOpeBRepBuild_WireEdgeSet::SName(const TopoDS_Shape&,
                                                           const TCollection_AsciiString& sb,
                                                           const TCollection_AsciiString&) const
#endif
{
  TCollection_AsciiString str=sb;
#ifdef DRAW
  str=str+TopOpeBRepBuild_ShapeSet::SName(S);
  str=str+sa;
  DBRep::Set(str.ToCString(),S);
#endif
  return str;
}

//=======================================================================
//function : SNameori
//purpose  : 
//=======================================================================
#ifdef DRAW
TCollection_AsciiString TopOpeBRepBuild_WireEdgeSet::SNameori(const TopoDS_Shape& S,
                                                              const TCollection_AsciiString& sb,
                                                              const TCollection_AsciiString& sa) const
#else
TCollection_AsciiString TopOpeBRepBuild_WireEdgeSet::SNameori(const TopoDS_Shape&,
                                                              const TCollection_AsciiString& sb,
                                                              const TCollection_AsciiString&) const
#endif
{
  TCollection_AsciiString str=sb;
#ifdef DRAW
  str=str+TopOpeBRepBuild_ShapeSet::SNameori(S);
  if ( S.ShapeType() == TopAbs_EDGE ) {
    const TopoDS_Shape& E = TopoDS::Edge(S);
    Standard_Boolean c = IsClosed(E), u = IsUClosed(E), v = IsVClosed(E);
    if (c) str=str+"c";
    if (u) str=str+"u";
    if (v) str=str+"v";
    str=str+sa;
    DBRep::Set(str.ToCString(),S);
  }
#endif
  return str;
}

//=======================================================================
//function : SName
//purpose  : 
//=======================================================================
#ifdef DRAW
TCollection_AsciiString TopOpeBRepBuild_WireEdgeSet::SName(const TopTools_ListOfShape& L,
                                                           const TCollection_AsciiString& sb,
                                                           const TCollection_AsciiString& sa) const
#else
TCollection_AsciiString TopOpeBRepBuild_WireEdgeSet::SName(const TopTools_ListOfShape&,
                                                           const TCollection_AsciiString&,
                                                           const TCollection_AsciiString&) const
#endif
{
  TCollection_AsciiString str;
#ifdef DRAW
  for (TopTools_ListIteratorOfListOfShape it(L);it.More();it.Next())
    str=str+sb+SName(it.Value())+sa+" ";
#endif
  return str;
}

//=======================================================================
//function : SNameori
//purpose  : 
//=======================================================================
#ifdef DRAW
TCollection_AsciiString TopOpeBRepBuild_WireEdgeSet::SNameori(const TopTools_ListOfShape& L,
                                                              const TCollection_AsciiString& sb,
                                                              const TCollection_AsciiString& sa) const
#else
TCollection_AsciiString TopOpeBRepBuild_WireEdgeSet::SNameori(const TopTools_ListOfShape&,
                                                              const TCollection_AsciiString&,
                                                              const TCollection_AsciiString&) const
#endif
{
  TCollection_AsciiString str;
#ifdef DRAW
  for (TopTools_ListIteratorOfListOfShape it(L);it.More();it.Next())
    str=str+sb+SNameori(it.Value())+sa+" ";
#endif
  return str;
}
