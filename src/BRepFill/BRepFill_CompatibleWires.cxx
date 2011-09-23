// File:	BRepFill_CompatibleWires.cxx
// Created:	Thu Jul  2 16:47:25 1998
// Author:	Joelle CHAUVET
//		<jct@sgi64>


#include <BRepFill_CompatibleWires.ixx>

#include <BRepAdaptor_Curve.hxx>

#include <BRepFill.hxx>
#include <BRepLib.hxx>
#include <BRepLib_FindSurface.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepCheck_Wire.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepLProp.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <GProp_PrincipalProps.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Precision.hxx>
#include <TopAbs.hxx> 
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopTools_ListOfShape.hxx>    

#include <gp.hxx>
#include <gp_Vec.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pln.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <TColgp_HArray1OfVec.hxx>
#include <TColgp_HArray1OfPnt.hxx>

#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_SequenceOfReal.hxx>


static void EdgesFromVertex (const TopoDS_Wire&   W,
			     const TopoDS_Vertex& V, 
			     TopoDS_Edge& E1, 
			     TopoDS_Edge& E2)
{
  TopTools_IndexedDataMapOfShapeListOfShape Map;
  TopExp::MapShapesAndAncestors(W,TopAbs_VERTEX,TopAbs_EDGE,Map);

  const TopTools_ListOfShape& List = Map.FindFromKey(V);
  TopoDS_Edge          e1   = TopoDS::Edge(List.First());
  TopoDS_Edge          e2   = TopoDS::Edge(List. Last());

  BRepTools_WireExplorer anExp;
  Standard_Integer I1=0, I2=0, NE=0;

  for(anExp.Init(W); anExp.More(); anExp.Next()) {
    NE++;
    const TopoDS_Edge& ECur = anExp.Current();
    if (e1.IsSame(ECur)) {
      I1 = NE;
    }
    if (e2.IsSame(ECur)) {
      I2 = NE;
    }
  }

  if (Abs(I2-I1)==1) {
    // numeros consecutifs
    if (I2==I1+1) {
      E1 = e1;
      E2 = e2;
    }
    else {
      E1 = e2;
      E2 = e1;
    }
  }
  else {
    // numeros non consecutifs sur un wire ferme
    if (I1==1&&I2==NE) {
      E1 = e2;
      E2 = e1;
    }
    else {
      E1 = e1;
      E2 = e2;
    }
  }
}
				      

static void SeqOfVertices (const TopoDS_Wire&   W,
			   TopTools_SequenceOfShape& S)
{
  S.Clear();
  Standard_Integer jj, cpt = 0;
  TopExp_Explorer PE;
  for (PE.Init(W,TopAbs_VERTEX); PE.More(); PE.Next()) {
    cpt++;
    Standard_Boolean trouve=Standard_False;
    for (jj=1;jj<=S.Length() && (!trouve);jj++) {
      if (S.Value(jj).IsSame(PE.Current())) trouve = Standard_True; 
      }
      if (!trouve) S.Append(PE.Current());
    }
}
				      

static Standard_Boolean PlaneOfWire (const TopoDS_Wire& W, gp_Pln& P)
{
  Standard_Boolean isplane = Standard_True;
  BRepLib_FindSurface findPlanarSurf;
  Handle(Geom_Surface) S;
  TopLoc_Location      L;
  
  GProp_GProps GP;
  BRepGProp::LinearProperties(W,GP);
  gp_Pnt Bary = GP.CentreOfMass();

// blindage pour les cas particuliers : 1 seule edge cercle ou ellipse
// sur un wire ferme !
  Standard_Integer nbEdges = 0;
  BRepTools_WireExplorer anExp;
  anExp.Init(W);
  Standard_Boolean wClosed = W.Closed();
  if (!wClosed) {
    // on regarde quand meme si les vertex sont les memes.
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(W,V1,V2);
    if ( V1.IsSame(V2)) wClosed = Standard_True;
  }
  TopoDS_Edge Edge = TopoDS::Edge(anExp.Current());
  Standard_Real first, last;
  TopLoc_Location loc;
  Handle(Geom_Curve) curv;
  curv = BRep_Tool::Curve(Edge, loc, first, last);
  curv = 
      Handle(Geom_Curve)::DownCast(curv->Transformed(loc.Transformation()));
  if (wClosed) {
    GeomAdaptor_Curve AdC;
    AdC.Load(curv);
    for(; anExp.More(); anExp.Next()) {
      nbEdges ++;
    }
    if ( nbEdges==1 && AdC.GetType() == GeomAbs_Circle ) {
      Bary = AdC.Circle().Location();
    }
    if ( nbEdges==1 && AdC.GetType() == GeomAbs_Ellipse ) {
      Bary = AdC.Ellipse().Location();
    }
  }

  findPlanarSurf.Init(W, -1, Standard_True);
  if ( findPlanarSurf.Found()) {
    S = findPlanarSurf.Surface();
    L = findPlanarSurf.Location();
    if (!L.IsIdentity()) S = Handle(Geom_Surface)::
      DownCast(S->Transformed(L.Transformation()));
    P = (Handle(Geom_Plane)::DownCast(S))->Pln();
    P.SetLocation(Bary);
  }
  else {
    // wire non plan !
    GProp_PrincipalProps Pp  = GP.PrincipalProperties();
    gp_Vec Vec;
    Standard_Real R1, R2, R3,Tol = Precision::Confusion();
    Pp.RadiusOfGyration(R1,R2,R3);
    Standard_Real RMax = Max(Max(R1,R2),R3);
    if ( ( Abs(RMax-R1)<Tol && Abs(RMax-R2)<Tol )
	|| ( Abs(RMax-R1)<Tol && Abs(RMax-R3)<Tol ) 
	|| ( Abs(RMax-R2)<Tol && Abs(RMax-R3)<Tol ) )
      isplane = Standard_False;
    else {
      if (R1>=R2 && R1>=R3) {
	Vec = Pp.FirstAxisOfInertia();
      }
      else if (R2>=R1 && R2>=R3) {
	Vec = Pp.SecondAxisOfInertia();
      }
      else if (R3>=R1 && R3>=R2) {
	Vec = Pp.ThirdAxisOfInertia();
      }
      gp_Dir NDir(Vec);
      if (R3<=R2 && R3<=R1) {
	Vec = Pp.ThirdAxisOfInertia();
      }
      else if (R2<=R1 && R2<=R3) {
	Vec = Pp.SecondAxisOfInertia();
      }
      else if (R1<=R2 && R1<=R3) {
	Vec = Pp.FirstAxisOfInertia();
      }
      gp_Dir XDir(Vec);
      gp_Ax3 repere(Bary,NDir,XDir);
      Geom_Plane GPlan(repere);
      P = GPlan.Pln();
    }
  }

  return isplane;

}
				      

static void WireContinuity (const TopoDS_Wire& W,
			    GeomAbs_Shape& contW)
{
  contW = GeomAbs_CN;
  GeomAbs_Shape cont;
  Standard_Boolean IsDegenerated = Standard_False;

  BRepTools_WireExplorer anExp;
  Standard_Integer nbEdges=0;
  Handle(TopTools_HSequenceOfShape) Edges = new TopTools_HSequenceOfShape();
  for(anExp.Init(W); anExp.More(); anExp.Next()) {
    nbEdges++;
    Edges->Append(anExp.Current());
    if (BRep_Tool::Degenerated(anExp.Current())) IsDegenerated = Standard_True;
  }
  
  if (!IsDegenerated) {

    Standard_Boolean testconti = Standard_True;

    for (Standard_Integer j=1;j<=nbEdges;j++) {
      
      TopoDS_Edge Edge1, Edge2;
      
      if (j == nbEdges) {
	Edge1 = TopoDS::Edge (Edges->Value(nbEdges));
	Edge2 = TopoDS::Edge (Edges->Value(1));
      }
      else {
	Edge1 = TopoDS::Edge (Edges->Value(j));
	Edge2 = TopoDS::Edge (Edges->Value(j+1));
      } 
      
      TopoDS_Vertex V1,V2,Vbid;
      TopExp::Vertices(Edge1,Vbid,V1,Standard_True);
      TopExp::Vertices(Edge2,V2,Vbid,Standard_True);
      Standard_Real U1 = BRep_Tool::Parameter(V1,Edge1);
      Standard_Real U2 = BRep_Tool::Parameter(V2,Edge2);
      BRepAdaptor_Curve Curve1(Edge1);
      BRepAdaptor_Curve Curve2(Edge2);
      Standard_Real Eps = BRep_Tool::Tolerance(V2) + BRep_Tool::Tolerance(V1);
      
      if(j == nbEdges) 
	testconti = Curve1.Value(U1).IsEqual(Curve2.Value(U2), Eps);
      
      if(testconti) {
	cont = BRepLProp::Continuity(Curve1,Curve2,U1,U2,
				     Eps, Precision::Angular()); 
	if (cont <= contW) contW = cont;
      }
    }
  }
  
}

static void TrimEdge (const TopoDS_Edge&              CurrentEdge,
		      const TColStd_SequenceOfReal&   CutValues,
		      const Standard_Real   t0, const Standard_Real   t1,
		      const Standard_Boolean          SeqOrder,
		      TopTools_SequenceOfShape& S)

{
  S.Clear();
  Standard_Integer j, ndec=CutValues.Length();
  Standard_Real first,last,m0,m1;
  Handle(Geom_Curve) C = BRep_Tool::Curve(CurrentEdge,first,last);

  TopoDS_Vertex Vf,Vl,Vbid,V0,V1;
  TopAbs_Orientation CurrentOrient = CurrentEdge.Orientation();
  TopExp::Vertices(CurrentEdge,Vf,Vl);
  Vbid.Nullify();

  if (SeqOrder) {
    // de first vers last
    m0 = first;
    V0 = Vf;
    for (j=1; j<=ndec; j++) {
      // morceau d'edge  
      m1 = (CutValues.Value(j)-t0)*(last-first)/(t1-t0)+first;
      TopoDS_Edge CutE = BRepLib_MakeEdge(C,V0,Vbid,m0,m1);
      CutE.Orientation(CurrentOrient);
      S.Append(CutE);
      m0 = m1;
      V0 = TopExp::LastVertex(CutE);
      if (j==ndec) {
	// dernier morceau
	TopoDS_Edge LastE = BRepLib_MakeEdge(C,V0,Vl,m0,last);
	LastE.Orientation(CurrentOrient);
	S.Append(LastE);
      }
    }
  }
  else {
    // de last vers first
    m1 = last;
    V1 = Vl;
    for (j=ndec; j>=1; j--) {
      // morceau d'edge  
      m0 = (CutValues.Value(j)-t0)*(last-first)/(t1-t0)+first;
      TopoDS_Edge CutE = BRepLib_MakeEdge(C,Vbid,V1,m0,m1);
      CutE.Orientation(CurrentOrient);
      S.Append(CutE);
      m1 = m0;
      V1 = TopExp::FirstVertex(CutE);
      if (j==1) {
	// dernier morceau
	TopoDS_Edge LastE = BRepLib_MakeEdge(C,Vf,V1,first,m1);
	LastE.Orientation(CurrentOrient);
	S.Append(LastE);
      }
    }
  }
}



static Standard_Boolean SearchRoot (const TopoDS_Vertex& V,
				    const TopTools_DataMapOfShapeListOfShape& Map,
				    TopoDS_Vertex& VRoot)
{
  Standard_Boolean trouve = Standard_False;
  VRoot.Nullify();
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape it;
  for (it.Initialize(Map); it.More(); it.Next()) {
    const TopTools_ListOfShape & List = it.Value();
    TopTools_ListIteratorOfListOfShape itL;
    Standard_Boolean ilyest = Standard_False;
    for (itL.Initialize(List); itL.More(); itL.Next()) {
      TopoDS_Vertex Vcur = TopoDS::Vertex(itL.Value());
      if (Vcur.IsSame(V)) {
	ilyest = Standard_True;
      }
      if (ilyest) break;
    }
    if (ilyest) {
      trouve = Standard_True;
      VRoot = TopoDS::Vertex(it.Key());
    }
    if (trouve) break;
  }
  return trouve;
}

static Standard_Boolean SearchVertex (const TopTools_ListOfShape& List,
				      const TopoDS_Wire&   W,
				      TopoDS_Vertex& VonW)
{
  Standard_Boolean trouve = Standard_False;
  VonW.Nullify();
  TopTools_SequenceOfShape SeqV;
  SeqOfVertices(W,SeqV);
  for (Standard_Integer ii=1;ii<=SeqV.Length();ii++) {
    TopoDS_Vertex Vi = TopoDS::Vertex(SeqV.Value(ii));
    TopTools_ListIteratorOfListOfShape itL;
    Standard_Boolean ilyest = Standard_False;
    for (itL.Initialize(List); itL.More(); itL.Next()) {
      TopoDS_Vertex Vcur = TopoDS::Vertex(itL.Value());
      if (Vcur.IsSame(Vi)) {
	ilyest = Standard_True;
      }
      if (ilyest) break;
    }
    if (ilyest) {
      trouve = Standard_True;
      VonW = Vi;
    }
    if (trouve) break;
  }
  return trouve;
}


static Standard_Boolean EdgeIntersectOnWire (const gp_Pnt& P1,
					     const gp_Pnt& P2,
					     Standard_Real percent,
					     const TopTools_DataMapOfShapeListOfShape& Map,
					     const TopoDS_Wire&   W,
					     TopoDS_Vertex& Vsol,
					     TopoDS_Wire&   newW)
{

  BRepTools_WireExplorer anExp;

  // construction de l'edge d'intersection
  Standard_Boolean NewVertex = Standard_False;
  gp_Lin droite(P1,gp_Dir(gp_Vec(P1,P2)));
  // ATTENTION : en toute rigueur, il faudrait construire une demi-droite
  //             mais il y a un bug dans BRepExtrema_DistShapeShape
  //             on se contente de 100 * la distance entre P1 et P2
  //             en esperant que ce soit suffisant jusqu'a ce que le bug
  //             soit corrige
  //  Standard_Real dernierparam = Precision::Infinite();
  // ATTENTION : le retour !!
  //             100 c'est mieux que 10 mais quelquefois c'est trop !
  //             finalement, rien ne vaut une bonne boite d'encombrement
  //  Standard_Real dernierparam = 100 * P1.Distance(P2);
  Bnd_Box B;
  BRepBndLib::Add(W,B);
  Standard_Real x1,x2,y1,y2,z1,z2;
  B.Get(x1,y1,z1,x2,y2,z2);
  gp_Pnt BP1(x1,y1,z1), BP2(x2,y2,z2);
  Standard_Real diag = BP1.Distance(BP2);
  Standard_Real dernierparam = diag;
  BRepLib_MakeEdge ME(droite,0.,dernierparam);
  TopoDS_Edge ECur = BRepLib_MakeEdge(droite,0.,P1.Distance(P2));

  // calcul de l'intersection par BRepExtrema (point de distance mini)
  BRepExtrema_DistShapeShape DSS(ME.Edge(),W);
  if (DSS.IsDone()) {
    // on choisit la solution la plus proche de P2
    Standard_Integer isol = 1;
    Standard_Real dss = P2.Distance(DSS.PointOnShape2(isol));
    for (Standard_Integer iss=2; iss<=DSS.NbSolution(); iss++) {
      if (dss>P2.Distance(DSS.PointOnShape2(iss))) {
	dss = P2.Distance(DSS.PointOnShape2(iss));
	isol = iss;
      }
    }
#ifdef DEB
    gp_Pnt Psol = 
#endif
      DSS.PointOnShape2(isol);
    // la solution est-elle un nouveau vertex ?
    NewVertex = (DSS.SupportTypeShape2(isol) != BRepExtrema_IsVertex);
    if (NewVertex) {
      TopoDS_Shape aLocalShape = DSS.SupportOnShape2(isol);
      TopoDS_Edge E = TopoDS::Edge(aLocalShape);
//      TopoDS_Edge E = TopoDS::Edge(DSS.SupportOnShape2(isol));
      Standard_Real tol = Precision::PConfusion();
      Standard_Real first,last,param;
      BRep_Tool::Range(E,first,last);
      tol = Max(tol,percent*Abs(last-first));
      DSS.ParOnEdgeS2(isol,param);
      if (Abs(first-param)<tol) {
	NewVertex = Standard_False;
	Vsol = TopExp::FirstVertex(E);
      }
      else if (Abs(last-param)<tol) {
	NewVertex = Standard_False;
	Vsol = TopExp::LastVertex(E);
      }
      // verif
      if (!NewVertex) {
	TopoDS_Vertex VRoot;
	if (SearchRoot(Vsol,Map,VRoot)) NewVertex = Standard_True;
      }
    }
    else {
      TopoDS_Shape aLocalShape = DSS.SupportOnShape2(isol);
      Vsol = TopoDS::Vertex(aLocalShape);
//      Vsol = TopoDS::Vertex(DSS.SupportOnShape2(isol));
    }

    // il faut decouper l'edge
    if (NewVertex) {
      TopoDS_Shape aLocalShape = DSS.SupportOnShape2(isol);
      TopoDS_Edge E = TopoDS::Edge(aLocalShape);
//      TopoDS_Edge E = TopoDS::Edge(DSS.SupportOnShape2(isol));
#ifdef DEB
      Standard_Real tol = 
#endif
	Precision::PConfusion();
      Standard_Real first,last,param;
      DSS.ParOnEdgeS2(isol,param);
      BRep_Tool::Range(E,first,last);
      BRepLib_MakeWire MW;
      for (anExp.Init(W); anExp.More(); anExp.Next()) {
	if (E.IsSame(anExp.Current())) {
	  Standard_Boolean SO 
	    = (anExp.CurrentVertex().IsSame(TopExp::FirstVertex(E)));
	  TopTools_SequenceOfShape SE;
	  SE.Clear();
	  TColStd_SequenceOfReal SR;
	  SR.Clear();
	  SR.Append(param);
	  TrimEdge(E,SR,first,last,SO,SE);
	  TopoDS_Vertex VV1,VV2;
	  TopExp::Vertices(TopoDS::Edge(SE.Value(1)),VV1,VV2);
	  if (TopExp::FirstVertex(E).IsSame(VV1)
	      || TopExp::LastVertex(E).IsSame(VV1)) {
	    Vsol = VV2;
	  }
	  if (TopExp::FirstVertex(E).IsSame(VV2)
	      || TopExp::LastVertex(E).IsSame(VV2)) {
	    Vsol = VV1;
	  }
	  for (Standard_Integer k=1; k<=SE.Length(); k++) {
	    MW.Add(TopoDS::Edge(SE.Value(k)));
	  }
	}
	else {
	  MW.Add(anExp.Current());
	}
      }
      newW = MW.Wire();
    }
    else {
      newW = W;
    }
    
    
  }

  return NewVertex;

}


static void Transform (const Standard_Boolean WithRotation,
		       const gp_Pnt& P,
		       const gp_Pnt& Pos1,
		       const gp_Vec& Ax1,
		       const gp_Pnt& Pos2,
		       const gp_Vec& Ax2,
		       gp_Pnt& Pnew)
{

  Pnew = P.Translated (Pos1,Pos2);
  gp_Vec axe1 = Ax1, axe2 = Ax2; 
  if (!axe1.IsParallel(axe2,1.e-4)) {
    gp_Vec Vtrans(Pos1,Pos2),Vsign;
    Standard_Real alpha,beta,sign=1;
    alpha = Vtrans.Dot(axe1);
    beta = Vtrans.Dot(axe2);
    if (alpha<-1.e-7) axe1 *=-1;
    if (beta<1.e-7) axe2 *=-1;
    alpha = Vtrans.Dot(axe1);
    beta = Vtrans.Dot(axe2);
    gp_Vec norm2 = axe1 ^ axe2;
    Vsign.SetLinearForm(Vtrans.Dot(axe1),axe2,-Vtrans.Dot(axe2),axe1);
    alpha = Vsign.Dot(axe1);
    beta = Vsign.Dot(axe2);
    Standard_Boolean pasnul = (Abs(alpha)>1.e-4 && Abs(beta)>1.e-4);
    if ( alpha*beta>0.0 && pasnul ) sign=-1;
    gp_Ax1 Norm(Pos2,norm2);
    Standard_Real ang = axe1.AngleWithRef(axe2,norm2);
    if (!WithRotation) {
      if (ang>PI/2) ang = ang - PI;
      if (ang<-PI/2) ang = ang + PI;
    }
    ang *= sign;
    Pnew = Pnew.Rotated (Norm,ang);
  }
}

static void BuildConnectedEdges(const TopoDS_Wire& aWire,
				const TopoDS_Edge& StartEdge,
				const TopoDS_Vertex& StartVertex,
				TopTools_ListOfShape& ConnectedEdges)
{
  TopTools_IndexedDataMapOfShapeListOfShape MapVE;
  TopExp::MapShapesAndAncestors(aWire, TopAbs_VERTEX, TopAbs_EDGE, MapVE);
  TopoDS_Edge CurEdge = StartEdge;
  TopoDS_Vertex CurVertex = StartVertex;
  TopoDS_Vertex Origin, V1, V2;
  TopExp::Vertices(StartEdge, V1, V2);
  Origin = (V1.IsSame(StartVertex))? V2 : V1;

  for (;;)
    {
      TopTools_ListIteratorOfListOfShape itE( MapVE.FindFromKey(CurVertex) );
      for (; itE.More(); itE.Next())
	{
	  TopoDS_Edge anEdge = TopoDS::Edge(itE.Value());
	  if (!anEdge.IsSame(CurEdge))
	    {
	      ConnectedEdges.Append(anEdge);
	      TopExp::Vertices(anEdge, V1, V2);
	      CurVertex = (V1.IsSame(CurVertex))? V2 : V1;
	      CurEdge = anEdge;
	      break;
	    }
	}
      if (CurVertex.IsSame(Origin))
	break;
    }
}
				      
//=======================================================================
//function : BRepFill_CompatibleWires
//purpose  : 
//=======================================================================

BRepFill_CompatibleWires::BRepFill_CompatibleWires() 
:myIsDone(Standard_False)
{
}


//=======================================================================
//function : BRepFill_CompatibleWires
//purpose  : 
//=======================================================================

BRepFill_CompatibleWires::BRepFill_CompatibleWires(const TopTools_SequenceOfShape& Sections)
{
  Init(Sections);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepFill_CompatibleWires::Init(const TopTools_SequenceOfShape& Sections)
{
  myInit = Sections;
  myWork = Sections;
  myPercent = 0.01;
  myIsDone = Standard_False;
  myMap.Clear();

}


//=======================================================================
//function : SetPercent
//purpose  : 
//=======================================================================

void BRepFill_CompatibleWires::SetPercent(const Standard_Real Percent)
{
  if (0.<Percent && Percent<1.) myPercent = Percent;

}


//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepFill_CompatibleWires::IsDone() const 
{
  return myIsDone;
}


//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopTools_SequenceOfShape& BRepFill_CompatibleWires::Shape() const 
{
  return myWork;
}


//=======================================================================
//function : GeneratedShapes
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFill_CompatibleWires::GeneratedShapes
(const TopoDS_Edge& SubSection) const
{  

  if (myMap.IsBound(SubSection)) {
    return myMap(SubSection);
  }
  else {
    static TopTools_ListOfShape Empty;
    return Empty;
  }
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepFill_CompatibleWires::Perform (const Standard_Boolean WithRotation)
{
  // compute origin and orientation on wires to avoid twisted results
  // and update wires to have same number of edges

  // determination de report:
  // si le nombre d'elements est identique  et si les wires ont des discontinuites
  // en tangence, on n'effectue pas le report par abscisse curviligne, ni
  Standard_Integer nbSects = myWork.Length(), i;
  BRepTools_WireExplorer anExp;
  Standard_Integer nbmax=0, nbmin=0;
  TColStd_Array1OfInteger nbEdges(1,nbSects);
  Standard_Boolean report;
  GeomAbs_Shape contS=GeomAbs_CN;
  GeomAbs_Shape cont;
  for (i=1; i<=nbSects; i++) {
    TopoDS_Shape aLocalShape = myWork(i).Oriented(TopAbs_FORWARD);
    myWork(i) = TopoDS::Wire(aLocalShape);
//    myWork(i) = TopoDS::Wire(myWork(i).Oriented(TopAbs_FORWARD));
    TopoDS_Wire W = TopoDS::Wire(myWork(i));
    WireContinuity(W,cont);
    if (cont<contS) contS=cont;
    nbEdges(i) = 0;
    for(anExp.Init(W); anExp.More(); anExp.Next() ) nbEdges(i)++;
    if (i==1) nbmin = nbEdges(i);
    if (nbmax<nbEdges(i)) nbmax = nbEdges(i);
    if (nbmin>nbEdges(i)) nbmin = nbEdges(i);
  } 
  // si on n'a pas le meme nombre d'elements ou si tous les wires sont au moins
  // C1, on effectue le report par abscisse curviligne des decoupes sinon, on se
  // fait un report vertex / Vertex
  report = (nbmax != nbmin || contS >= GeomAbs_C1 );
  
  // initialisation de la map
  Standard_Integer nbE = 0;
  TopTools_ListOfShape Empty;
  for (i=1; i<=nbSects; i++) {
    TopoDS_Wire W = TopoDS::Wire(myWork(i));
    for(anExp.Init(W); anExp.More(); anExp.Next() ) {
      TopoDS_Edge E = TopoDS::Edge(anExp.Current());
      myMap.Bind(E,Empty);
      myMap(E).Append(E);
      nbE++;
    }
  } 
  
  // sections ouvertes / sections fermees
  // initialisation de myDegen1, myDegen2
  Standard_Integer ideb=1, ifin=myWork.Length();
  // on regarde si le premier wire est ponctuel
  myDegen1 = Standard_True;
  for(anExp.Init(TopoDS::Wire(myWork(ideb))); anExp.More(); anExp.Next()) {
    myDegen1 = myDegen1 && (BRep_Tool::Degenerated(anExp.Current()));
  }
  if (myDegen1) ideb++;
  // on regarde si le dernier wire est ponctuel
  myDegen2 = Standard_True;
  for(anExp.Init(TopoDS::Wire(myWork(ifin))); anExp.More(); anExp.Next()) {
    myDegen2 = myDegen2 && (BRep_Tool::Degenerated(anExp.Current()));
  }
  if (myDegen2) ifin--;
  
  Standard_Boolean wClosed, allClosed = Standard_True, allOpen = Standard_True;
  for (i=ideb; i<=ifin; i++) {
    wClosed = myWork(i).Closed();
    if (!wClosed) {
      // on regarde quand meme si les vertex sont les memes.
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(TopoDS::Wire(myWork(i)),V1,V2);
      if ( V1.IsSame(V2)) wClosed = Standard_True;
    }
    allClosed = (allClosed && wClosed);
    allOpen = (allOpen && !wClosed);
  }
  
  if (allClosed) {
    // Toutes les sections sont fermees 
    if (report) {
      // same number of elements  
      SameNumberByPolarMethod(WithRotation);
    }
    else {
      // origine
      ComputeOrigin(Standard_False);
    }
    myIsDone = Standard_True;
  }
  else if (allOpen) {
    // Toutes les sections sont ouvertes
    // origine
    SearchOrigin();
    // same number of elements
    if (report) {
      SameNumberByACR(report);
    }
    myIsDone = Standard_True;
  }
  else {
    // Il y a des sections ouvertes et des sections fermees :
    // on ne traite pas
    Standard_DomainError::Raise("Sections must be all closed or all open");
  }
  
}




//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================

const TopTools_DataMapOfShapeListOfShape&  BRepFill_CompatibleWires::Generated() const 
{
  return myMap;
}


//=======================================================================
//function : SameNumberByPolarMethod
//purpose  : 
//=======================================================================

void BRepFill_CompatibleWires::
              SameNumberByPolarMethod(const Standard_Boolean WithRotation)
{

  // initialisation 
  Standard_Integer NbSects=myWork.Length();
  BRepTools_WireExplorer anExp;
  TopoDS_Vertex V1, V2;
  
  Standard_Boolean allClosed = Standard_True;
  Standard_Integer i,ii,ideb=1,ifin=NbSects;
  for (i=1; i<=NbSects; i++) {
    Handle(BRepCheck_Wire) Checker = new BRepCheck_Wire(TopoDS::Wire(myWork(i)));
    allClosed = (allClosed && (Checker->Closed() == BRepCheck_NoError));
    //allClosed = (allClosed && myWork(i).Closed());
  }
  if (!allClosed)
    Standard_NoSuchObject::Raise("BRepFill_CompatibleWires::SameNumberByPolarMethod : the wires must be closed");
  
  // sections ponctuelles, sections bouclantes ?
  if (myDegen1) ideb++;
  if (myDegen2) ifin--;
  Standard_Boolean vClosed = (!myDegen1) && (!myDegen2)
                                && (myWork(ideb).IsSame(myWork(ifin)));

  //Removing degenerated edges
  for (i = ideb; i <= ifin; i++)
  {
    Standard_Boolean hasDegEdge = Standard_False;
    TopoDS_Iterator anItw(myWork(i));
    for (; anItw.More(); anItw.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(anItw.Value());
      if (BRep_Tool::Degenerated(anEdge))
      {
        hasDegEdge = Standard_True;
        break;
      }
    }
    if (hasDegEdge)
    {
      TopoDS_Wire aNewWire;
      BRep_Builder aBBuilder;
      aBBuilder.MakeWire(aNewWire);
      for (anItw.Initialize(myWork(i)); anItw.More(); anItw.Next())
      {
        const TopoDS_Edge& anEdge = TopoDS::Edge(anItw.Value());
        if (!BRep_Tool::Degenerated(anEdge))
          aBBuilder.Add(aNewWire, anEdge);
      }
      myWork(i) = aNewWire;
    }
  }
  
  // Nombre max de decoupes possibles
  Standard_Integer NbMaxV = 0;
  for (i=1; i<=NbSects; i++) {
    for(anExp.Init(TopoDS::Wire(myWork(i))); anExp.More(); anExp.Next()) {
      NbMaxV++;
    }
  }
  
  // construction des tableaux de plans des wires 
  gp_Pln P;
  Handle(TColgp_HArray1OfPnt) Pos
    = new (TColgp_HArray1OfPnt) (1,NbSects);
  Handle(TColgp_HArray1OfVec) Axe
    = new (TColgp_HArray1OfVec) (1,NbSects);
  for (i=ideb;i<=ifin;i++) {
    if (PlaneOfWire(TopoDS::Wire(myWork(i)),P)) {
      Pos->SetValue(i,P.Location());
      Axe->SetValue(i,gp_Vec(P.Axis().Direction()));
    }
  }
  TopTools_SequenceOfShape SeqV;
  if (myDegen1) {
    SeqOfVertices(TopoDS::Wire(myWork(1)),SeqV);
    Pos->SetValue(1,BRep_Tool::Pnt(TopoDS::Vertex(SeqV.Value(1))));
    Axe->SetValue(1,Axe->Value(ideb));
  }
  if (myDegen2) {
    SeqOfVertices(TopoDS::Wire(myWork(NbSects)),SeqV);
    Pos->SetValue(NbSects,BRep_Tool::Pnt(TopoDS::Vertex(SeqV.Value(1))));
    Axe->SetValue(NbSects,Axe->Value(ifin));
  }
  
  // construction de RMap, map des reports du wire i vers le wire i-1
  TopTools_DataMapOfShapeListOfShape RMap;
  RMap.Clear();
  
  // boucle sur i
  for (i=ifin; i>ideb; i--) {
    
    const TopoDS_Wire& wire1 = TopoDS::Wire(myWork(i));
    
    // sequence des vertex du premier wire
    SeqOfVertices(wire1,SeqV);
    if (SeqV.Length()>NbMaxV) 
      Standard_NoSuchObject::Raise("BRepFill::SameNumberByPolarMethod failed");
    
    // extremite du premier wire
    V1 = TopoDS::Vertex(SeqV.Value(1));	
    // wire precedent
#ifdef DEB
    const TopoDS_Wire& wire2 = 
#endif
      TopoDS::Wire(myWork(i-1));
    // boucle sur les vertex de wire1
    for (ii=1;ii<=SeqV.Length();ii++) {
      
      TopoDS_Vertex Vi = TopoDS::Vertex(SeqV.Value(ii));
      
      // init de RMap pour Vi
      TopTools_ListOfShape Init;
      Init.Clear();
      RMap.Bind(Vi,Init);
      
      // il faut chercher l'intersection Vi - wire2
      gp_Pnt Pi = BRep_Tool::Pnt(Vi);
      
      // on ramene Pi dans le plan courant
      gp_Pnt Pnew;
      Transform(WithRotation,Pi,
		Pos->Value(i),Axe->Value(i), 
		Pos->Value(i-1),Axe->Value(i-1),Pnew);
      
      // calcul de l'intersection
      TopoDS_Shape Support;
      Standard_Boolean NewVertex;
      TopoDS_Vertex Vsol;
      TopoDS_Wire newwire;
      if (Pnew.Distance(Pos->Value(i-1))>Precision::Confusion()) {
	Standard_Real percent = myPercent;
	NewVertex = EdgeIntersectOnWire(Pos->Value(i-1),Pnew,percent,
				    RMap,TopoDS::Wire(myWork(i-1)),
				    Vsol,newwire);
	if (NewVertex) myWork(i-1) = newwire;
	RMap(Vi).Append(Vsol);
      }
      
    } // boucle sur ii
  }   // boucle sur i
  
  // initialisation de MapVLV, map des correspondances vertex - liste de vertex
  TopTools_DataMapOfShapeListOfShape MapVLV;
  SeqOfVertices(TopoDS::Wire(myWork(ideb)),SeqV);
  Standard_Integer SizeMap = SeqV.Length();
  MapVLV.Clear();
  for (ii=1;ii<=SizeMap;ii++) {
    TopoDS_Vertex Vi = TopoDS::Vertex(SeqV.Value(ii));
    TopTools_ListOfShape Init;
    Init.Clear();
    Init.Append(Vi);
    MapVLV.Bind(Vi,Init);
    Standard_Integer NbV = 1;
    TopoDS_Vertex V0,V1;
    V0 = Vi;
    Standard_Boolean tantque = SearchRoot(V0,RMap,V1);
    while (tantque) {
      MapVLV(Vi).Append(V1);
      NbV++;
      // test sur NbV necessaire pour les sections bouclantes
      if (V1.IsSame(Vi) || NbV >= myWork.Length()) {
	tantque = Standard_False;
      }
      else {
	V0 = V1;
	tantque = SearchRoot(V0,RMap,V1);
      }
    }
  }
  
  // boucle sur i
  for (i=ideb; i<ifin; i++) {
    
    const TopoDS_Wire& wire1 = TopoDS::Wire(myWork(i));
    
    // sequence des vertex du premier wire
    SeqOfVertices(wire1,SeqV);
    if ( SeqV.Length()>NbMaxV || SeqV.Length()>SizeMap ) 
      Standard_NoSuchObject::Raise("BRepFill::SameNumberByPolarMethod failed");
    
    // extremite du premier wire
    V1 = TopoDS::Vertex(SeqV.Value(1));

    // wire suivant
    const TopoDS_Wire& wire2 = TopoDS::Wire(myWork(i+1));
    
    // boucle sur les vertex de wire1
    for (ii=1;ii<=SeqV.Length();ii++) {
      
      TopoDS_Vertex Vi = TopoDS::Vertex(SeqV.Value(ii));
      TopoDS_Vertex VRoot;
      VRoot.Nullify();
      Standard_Boolean intersect = Standard_True;
      if (SearchRoot(Vi,MapVLV,VRoot)) {
	const TopTools_ListOfShape& LVi = MapVLV(VRoot);
	TopoDS_Vertex VonW;
	VonW.Nullify();
	intersect = (!SearchVertex(LVi,wire2,VonW));
      }
      
      if (intersect) {
	// il faut chercher l'intersection Vi - wire2
	gp_Pnt Pi = BRep_Tool::Pnt(Vi);
	
	// on ramene Pi dans le plan courant
	gp_Pnt Pnew;
	Transform(WithRotation,Pi,
		  Pos->Value(i),Axe->Value(i), 
		  Pos->Value(i+1),Axe->Value(i+1),Pnew);
	
	// calcul de l'intersection
	TopoDS_Shape Support;
	Standard_Boolean NewVertex;
	TopoDS_Vertex Vsol;
	TopoDS_Wire newwire;
	if (Pnew.Distance(Pos->Value(i+1))>Precision::Confusion()) {
	  Standard_Real percent = myPercent;
	  NewVertex = EdgeIntersectOnWire(Pos->Value(i+1),Pnew,percent,
				      MapVLV,TopoDS::Wire(myWork(i+1)),
				      Vsol,newwire);
	  MapVLV(VRoot).Append(Vsol);
	  if (NewVertex) myWork(i+1) = newwire;
	}
	
      }
    } // boucle sur ii
  }   // boucle sur i
  
  // mise en ordre des wires en suivant MapVLV
  TopoDS_Wire wire = TopoDS::Wire(myWork(ideb));

  // sauf le dernier si les sections sont bouclantes
  Standard_Integer ibout = ifin;
  if (vClosed) ibout--;

  for ( i=ideb+1; i<=ibout; i++) {

    BRepLib_MakeWire MW;

    anExp.Init(wire);
    TopoDS_Edge ECur = anExp.Current();
    TopoDS_Vertex VF,VL;
    TopExp::Vertices(ECur,VF,VL,Standard_True);
    Standard_Real U1 = BRep_Tool::Parameter(VF,ECur);
    Standard_Real U2 = BRep_Tool::Parameter(VL,ECur);
    BRepAdaptor_Curve Curve(ECur);
    gp_Pnt PPs = Curve.Value(0.1*(U1+9*U2));
    TopTools_ListIteratorOfListOfShape itF(MapVLV(VF)),itL(MapVLV(VL));
    Standard_Integer rang = ideb;
    while (rang < i) {
      itF.Next();
      itL.Next();
      rang++;
    }
    TopoDS_Vertex V1 = TopoDS::Vertex(itF.Value()), V2 = TopoDS::Vertex(itL.Value());
    TopoDS_Edge Esol;
    Standard_Real scalmax=0.;
    TopoDS_Iterator itW( myWork(i) );
    
    for(; itW.More(); itW.Next())
      {
	TopoDS_Edge E = TopoDS::Edge(itW.Value());
	TopoDS_Vertex VVF,VVL;
	TopExp::Vertices(E,VVF,VVL,Standard_True);
	
	// tri des edges candidates
	Standard_Real scal1,scal2;
	if ( (V1.IsSame(VVF)&&V2.IsSame(VVL)) || (V2.IsSame(VVF)&&V1.IsSame(VVL)) ) {
	  Standard_Real U1 = BRep_Tool::Parameter(VVF,E);
	  Standard_Real U2 = BRep_Tool::Parameter(VVL,E);
	  BRepAdaptor_Curve Curve(E);
	  gp_Pnt PP1 = Curve.Value(0.1*(U1+9*U2));
	  gp_Pnt PP2 = Curve.Value(0.1*(9*U1+U2));
  
	  for (rang=i;rang>ideb;rang--) {
	    Transform(WithRotation, PP1,
		      Pos->Value(rang), Axe->Value(rang),
		      Pos->Value(rang-1), Axe->Value(rang-1), PP1);
	    Transform(WithRotation, PP2,
		      Pos->Value(rang), Axe->Value(rang),
		      Pos->Value(rang-1), Axe->Value(rang-1), PP2);
	  }
	  gp_Vec Ns(Pos->Value(ideb),PPs);
	  Ns = Ns.Normalized();
	  gp_Vec N1(Pos->Value(ideb),PP1);
	  N1 = N1.Normalized();
	  gp_Vec N2(Pos->Value(ideb),PP2);
	  N2 = N2.Normalized();
	  scal1 = N1.Dot(Ns);
	  if (scal1>scalmax) {
	    scalmax = scal1;
	    Esol = E;
	  }
	  scal2 = N2.Dot(Ns);
	  if (scal2>scalmax) {
	    scalmax = scal2;
	    TopoDS_Shape aLocalShape = E.Reversed();
	    Esol = TopoDS::Edge(aLocalShape);
	  }
	}
      } //end of for(; itW.More(); itW.Next())
    MW.Add(Esol);

    TopTools_ListOfShape ConnectedEdges;
    BuildConnectedEdges( TopoDS::Wire(myWork(i)), Esol, V2, ConnectedEdges );

    TopTools_ListIteratorOfListOfShape itCE(ConnectedEdges);
    for(; anExp.More(), itCE.More(); anExp.Next(), itCE.Next())
      {
	ECur = anExp.Current();
	TopExp::Vertices(ECur,VF,VL,Standard_True);
	U1 = BRep_Tool::Parameter(VF,ECur);
	U2 = BRep_Tool::Parameter(VL,ECur);
	Curve.Initialize(ECur);
	PPs = Curve.Value(0.1*(U1+9*U2));
	
	TopoDS_Edge E = TopoDS::Edge(itCE.Value());
	TopoDS_Vertex VVF,VVL;
	TopExp::Vertices(E,VVF,VVL,Standard_True);

	// tri des edges candidates
	Standard_Real scal1,scal2;
	U1 = BRep_Tool::Parameter(VVF,E);
	U2 = BRep_Tool::Parameter(VVL,E);
	Curve.Initialize(E);
	gp_Pnt PP1 = Curve.Value(0.1*(U1+9*U2));
	gp_Pnt PP2 = Curve.Value(0.1*(9*U1+U2));
	
	for (rang=i;rang>ideb;rang--) {
	  Transform(WithRotation, PP1,
		    Pos->Value(rang), Axe->Value(rang),
		    Pos->Value(rang-1), Axe->Value(rang-1), PP1);
	  Transform(WithRotation, PP2,
		    Pos->Value(rang), Axe->Value(rang),
		    Pos->Value(rang-1), Axe->Value(rang-1), PP2);
	}
	gp_Vec Ns(Pos->Value(ideb),PPs);
	Ns = Ns.Normalized();
	gp_Vec N1(Pos->Value(ideb),PP1);
	N1 = N1.Normalized();
	gp_Vec N2(Pos->Value(ideb),PP2);
	N2 = N2.Normalized();
	scal1 = N1.Dot(Ns);
	scal2 = N2.Dot(Ns);
	if (scal2>scal1)
	  E.Reverse();
	MW.Add(E);
      }
    myWork(i) = MW.Wire();
  }
  
  // sections bouclantes ?
  if (vClosed) myWork(myWork.Length()) = myWork(1);

  // verification du nombre d'edges pour debug
  Standard_Integer nbmax=0, nbmin=0;
  for ( i=ideb; i<=ifin; i++) {
    Standard_Integer nbEdges=0;
    for(anExp.Init(TopoDS::Wire(myWork(i))); anExp.More(); anExp.Next()) {
      nbEdges++;
    }
    if (i==ideb) nbmin = nbEdges;
    if (nbmax<nbEdges) nbmax = nbEdges;
    if (nbmin>nbEdges) nbmin = nbEdges;
  }
  if (nbmin!=nbmax) {
    Standard_NoSuchObject::Raise("BRepFill_CompatibleWires::SameNumberByPolarMethod failed");
  }

}

//=======================================================================
//function : SameNumberByACR
//purpose  : 
//=======================================================================

void BRepFill_CompatibleWires::SameNumberByACR(const  Standard_Boolean  report)
{
  // find the dimension
  Standard_Integer ideb=1, ifin=myWork.Length();
  BRepTools_WireExplorer anExp;

  // sections ponctuelles, sections bouclantes ?
  if (myDegen1) ideb++;
  if (myDegen2) ifin--;
  Standard_Boolean vClosed = (!myDegen1) && (!myDegen2)
                                && (myWork(ideb).IsSame(myWork(ifin)));

  Standard_Integer nbSects = myWork.Length(), i;
  Standard_Integer nbmax=0, nbmin=0;
  TColStd_Array1OfInteger nbEdges(1,nbSects);
  for (i=1; i<=nbSects; i++) {
    nbEdges(i) = 0;
    for(anExp.Init(TopoDS::Wire(myWork(i))); anExp.More(); anExp.Next()) {
      nbEdges(i)++;
    }
    if (i==1) nbmin = nbEdges(i);
    if (nbmax<nbEdges(i)) nbmax = nbEdges(i);
    if (nbmin>nbEdges(i)) nbmin = nbEdges(i);
  }

  if (nbmax>1) {
    // plusieurs edges

    if (report || nbmin<nbmax) {
      // insertion des decoupes
      Standard_Integer nbdec=(nbmax-1)*nbSects+1;
      Standard_Real tol = 0.01;
      TColStd_Array1OfReal dec(1,nbdec);
      dec.Init(0);
      dec(2)=1;
      // calcul du tableau des decoupes
      Standard_Integer j,k,l;
      for (i=1; i<=nbSects; i++) {
	// wire courant
	const TopoDS_Wire& wire1 = TopoDS::Wire(myWork(i));
	Standard_Integer nbE = 0;
	for(anExp.Init(wire1); anExp.More(); anExp.Next()) {
	  nbE++;
	}
	// longueur et ACR du wire 
	TColStd_Array1OfReal ACR(0,nbE);
	ACR.Init(0);
	BRepFill::ComputeACR(wire1, ACR);
	// insertion des ACR du wire dans le tableau des decoupes
	for (j=1; j<ACR.Length()-1; j++) {
	  k=1;
	  while (dec(k)<ACR(j)) {
	    k++;
	    if (k>nbdec) break;
	  }
	  if (dec(k-1)+tol<ACR(j)&& ACR(j)+tol<dec(k)) {
	    for (l=nbdec-1;l>=k;l--) {
	      dec(l+1)=dec(l);
	    }
	    dec(k) = ACR(j);
	  }
	}
      }
      
      // tableau effectif des decoupes
      k=1;
      while (dec(k)<1) {
	k++;
	if (k>nbdec) break;
      }
      nbdec = k-1;
      TColStd_Array1OfReal dec2(1,nbdec);
      for (k=1;k<=nbdec;k++) {
	dec2(k) = dec(k);
      }
      
      // insertion des decoupes dans chaque wire
      for (i=1; i<=nbSects; i++) {
	const TopoDS_Wire& oldwire = TopoDS::Wire(myWork(i));
	TopoDS_Wire newwire = BRepFill::InsertACR(oldwire, dec2, tol);
	BRepTools_WireExplorer anExp1,anExp2;
	anExp1.Init(oldwire);
	anExp2.Init(newwire);
	for (;anExp1.More();anExp1.Next()) {
	  const TopoDS_Edge& Ecur = anExp1.Current();
	  if (!Ecur.IsSame(TopoDS::Edge(anExp2.Current()))) {
	    TopTools_ListOfShape LE;
	    LE.Clear();
	    gp_Pnt P1,P2;
	    const TopoDS_Vertex& V1 = anExp1.CurrentVertex();
	    TopoDS_Vertex VF,VR;
	    TopExp::Vertices(Ecur,VF,VR,Standard_True);
	    if (V1.IsSame(VF)) P1 = BRep_Tool::Pnt(VR);
	    if (V1.IsSame(VR)) P1 = BRep_Tool::Pnt(VF);
	    TopoDS_Vertex V2 = anExp2.CurrentVertex();
	    TopExp::Vertices(TopoDS::Edge(anExp2.Current()),
			     VF,VR,Standard_True);
	    if (V2.IsSame(VF)) P2 = BRep_Tool::Pnt(VR);
	    if (V2.IsSame(VR)) P2 = BRep_Tool::Pnt(VF);
	    while (P1.Distance(P2)>1.e-3) {
	      LE.Append(anExp2.Current());
	      anExp2.Next();
	      V2 = anExp2.CurrentVertex();
	      TopExp::Vertices(TopoDS::Edge(anExp2.Current()),
			       VF,VR,Standard_True);
	      if (V2.IsSame(VF)) P2 = BRep_Tool::Pnt(VR);
	      if (V2.IsSame(VR)) P2 = BRep_Tool::Pnt(VF);
	      if (P1.Distance(P2)<=1.e-3) {
		LE.Append(anExp2.Current());
		anExp2.Next();
	      }
	    }

	    TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itmap;
	    //TopTools_ListIteratorOfListOfShape itlist;
	    TopoDS_Edge Ancestor;
	    Standard_Integer nbedge, nblist=0;
	    Standard_Boolean found = Standard_False;

	    for (itmap.Initialize(myMap);itmap.More()&&(!found);itmap.Next()) {
	      nblist++;
	      TopTools_ListIteratorOfListOfShape itlist(itmap.Value());
	      nbedge = 0;
	      while (itlist.More()&&(!found)) {
		nbedge++;
		TopoDS_Edge ECur = TopoDS::Edge(itlist.Value());
		    
		if (Ecur.IsSame(ECur)) {
		  Ancestor = TopoDS::Edge(itmap.Key());
		  found = Standard_True;
		  myMap(Ancestor).InsertBefore(LE,itlist);
		  myMap(Ancestor).Remove(itlist);
		}
		if (itlist.More()) itlist.Next();
	      }
	      
	    }

	  }
	  else {
	    anExp2.Next();
	  }
	  
	}
	myWork(i) = newwire;
      }
      
    }
  }
  
  // sections bouclantes ?
  if (vClosed) myWork(myWork.Length()) = myWork(1);

  // verification du nombre d'edges pour debug
  nbmax = 0;
  for (i=ideb; i<=ifin; i++) {
    nbEdges(i) = 0;
    for(anExp.Init(TopoDS::Wire(myWork(i))); anExp.More(); anExp.Next()) {
      nbEdges(i)++;
    }
    if (i==ideb) nbmin = nbEdges(i);
    if (nbmax<nbEdges(i)) nbmax = nbEdges(i);
    if (nbmin>nbEdges(i)) nbmin = nbEdges(i);
  }
  if (nbmax!=nbmin) 
    Standard_NoSuchObject::Raise("BRepFill_CompatibleWires::SameNumberByACR failed");
}

//=======================================================================
//function : ComputeOrigin
//purpose  : 
//=======================================================================

void BRepFill_CompatibleWires::ComputeOrigin(const  Standard_Boolean polar )
{
  // reorganize the wires respecting orientation and origin
  
  TopoDS_Vertex Vdeb, Vfin;
  gp_Pnt Pdeb, Psuiv, PPs;

  BRepTools_WireExplorer anExp;

  Standard_Boolean wClosed, allClosed = Standard_True;

  Standard_Integer NbSects = myWork.Length();
  Standard_Integer i, ideb=1,ifin=NbSects;

  // sections ponctuelles, sections bouclantes
  if (myDegen1) ideb++;
  if (myDegen2) ifin--;
  Standard_Boolean vClosed = (!myDegen1) && (!myDegen2)
                                && (myWork(ideb).IsSame(myWork(ifin)));
  
  
  for (i=ideb; i<=ifin; i++) {
    wClosed = myWork(i).Closed();
    if (!wClosed) {
      // on regarde quand meme si les vertex sont les memes.
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(TopoDS::Wire(myWork(i)),V1,V2);
      if ( V1.IsSame(V2)) wClosed = Standard_True;
    }
    allClosed = (allClosed && wClosed);
  }
/*
  for (i=ideb; i<=ifin; i++) {
    allClosed = (allClosed && myWork(i).Closed());
  }
*/
  if (!allClosed) 
    Standard_NoSuchObject::Raise("BRepFill_CompatibleWires::ComputeOrigin : the wires must be closed");

/*  
  // Nombre max de decoupes possibles
  Standard_Integer NbMaxV = 0;
  for (i=1; i<=NbSects; i++) {
    for(anExp.Init(TopoDS::Wire(myWork(i))); anExp.More(); anExp.Next()) {
      NbMaxV++;
    }
  }
  
  // construction des tableaux de plans des wires 
  gp_Pln P;  
  Handle(TColgp_HArray1OfPnt) Pos
    = new (TColgp_HArray1OfPnt) (1,NbSects);
  Handle(TColgp_HArray1OfVec) Axe
    = new (TColgp_HArray1OfVec) (1,NbSects);
  for (i=ideb;i<=ifin;i++) {
    if (PlaneOfWire(TopoDS::Wire(myWork(i)),P)) {
      Pos->SetValue(i,P.Location());
      Axe->SetValue(i,gp_Vec(P.Axis().Direction()));
    }
  }
  TopTools_SequenceOfShape SeqV;
  if (myDegen1) {
    SeqOfVertices(TopoDS::Wire(myWork(1)),SeqV);
    Pos->SetValue(1,BRep_Tool::Pnt(TopoDS::Vertex(SeqV.Value(1))));
    Axe->SetValue(1,Axe->Value(ideb));
  }
  if (myDegen2) {
    SeqOfVertices(TopoDS::Wire(myWork(NbSects)),SeqV);
    Pos->SetValue(NbSects,BRep_Tool::Pnt(TopoDS::Vertex(SeqV.Value(1))));
    Axe->SetValue(NbSects,Axe->Value(ifin));
  }
*/

  //Consider that all wires have same number of edges (polar==Standard_False)
  TopTools_SequenceOfShape PrevSeq;
  TopTools_SequenceOfShape PrevEseq;
  Standard_Integer theLength = 0;
  const TopoDS_Wire& wire = TopoDS::Wire( myWork(ideb) );
  for (anExp.Init(wire); anExp.More(); anExp.Next())
    {
      PrevSeq.Append(anExp.CurrentVertex());
      PrevEseq.Append(anExp.Current());
      theLength++;
    }

  Standard_Integer nbs, NbSamples = 0;
  if (theLength <= 2)
    NbSamples = 4;
  for (i = ideb+1; i <= ifin; i++)
    {
      const TopoDS_Wire& wire = TopoDS::Wire(myWork(i));
      TopoDS_Wire newwire;
      BRep_Builder BB;
      BB.MakeWire(newwire);
      
      TopTools_SequenceOfShape SeqVertices, SeqEdges;
      for (anExp.Init(wire); anExp.More(); anExp.Next())
	{
	  SeqVertices.Append( anExp.CurrentVertex() );
	  SeqEdges.Append( anExp.Current() );
	}
      
      Standard_Real MinSumDist = Precision::Infinite();
      Standard_Integer jmin, j, k, n;
      Standard_Boolean forward;
      if (i == myWork.Length() && myDegen2)
	{
	  // derniere section ponctuelle
	  jmin = 1;
	  forward = Standard_True;
	}
      else
	for (j = 1; j <= theLength; j++)
	  {
	    //Forward
	    Standard_Real SumDist = 0.;
	    for (k = j, n = 1; k <= theLength; k++, n++)
	      {
		const TopoDS_Vertex& Vprev = TopoDS::Vertex( PrevSeq(n) );
		gp_Pnt Pprev = BRep_Tool::Pnt(Vprev);
		const TopoDS_Vertex& V = TopoDS::Vertex( SeqVertices(k) );
		gp_Pnt P = BRep_Tool::Pnt(V);
		SumDist += Pprev.Distance(P);
                if (NbSamples > 0)
                {
                  const TopoDS_Edge& PrevEdge = TopoDS::Edge(PrevEseq(n));
                  const TopoDS_Edge& CurEdge = TopoDS::Edge(SeqEdges(k));
                  BRepAdaptor_Curve PrevEcurve(PrevEdge);
                  BRepAdaptor_Curve Ecurve(CurEdge);
                  Standard_Real SampleOnPrev = (PrevEcurve.LastParameter()-PrevEcurve.FirstParameter())/NbSamples;
                  Standard_Real SampleOnCur = (Ecurve.LastParameter()-Ecurve.FirstParameter())/NbSamples;
                  for (nbs = 1; nbs <= NbSamples-1; nbs++)
                  {
                    Standard_Real ParOnPrev = (PrevEdge.Orientation() == TopAbs_FORWARD)?
                      (PrevEcurve.FirstParameter() + nbs*SampleOnPrev) :
                      (PrevEcurve.FirstParameter() + (NbSamples-nbs)*SampleOnPrev);
                    Standard_Real ParOnCur = (CurEdge.Orientation() == TopAbs_FORWARD)?
                      (Ecurve.FirstParameter() + nbs*SampleOnCur) :
                      (Ecurve.FirstParameter() + (NbSamples-nbs)*SampleOnCur);
                    gp_Pnt PonPrev = PrevEcurve.Value(ParOnPrev);
                    gp_Pnt PonCur = Ecurve.Value(ParOnCur);
                    SumDist += PonPrev.Distance(PonCur);
                  }
                }
	      }
	    for (k = 1; k < j; k++, n++)
	      {
		const TopoDS_Vertex& Vprev = TopoDS::Vertex( PrevSeq(n) );
		gp_Pnt Pprev = BRep_Tool::Pnt(Vprev);
		const TopoDS_Vertex& V = TopoDS::Vertex( SeqVertices(k) );
		gp_Pnt P = BRep_Tool::Pnt(V);
		SumDist += Pprev.Distance(P);
                if (NbSamples > 0)
                {
                  const TopoDS_Edge& PrevEdge = TopoDS::Edge(PrevEseq(n));
                  const TopoDS_Edge& CurEdge = TopoDS::Edge(SeqEdges(k));
                  BRepAdaptor_Curve PrevEcurve(PrevEdge);
                  BRepAdaptor_Curve Ecurve(CurEdge);
                  Standard_Real SampleOnPrev = (PrevEcurve.LastParameter()-PrevEcurve.FirstParameter())/NbSamples;
                  Standard_Real SampleOnCur = (Ecurve.LastParameter()-Ecurve.FirstParameter())/NbSamples;
                  for (nbs = 1; nbs <= NbSamples-1; nbs++)
                  {
                    Standard_Real ParOnPrev = (PrevEdge.Orientation() == TopAbs_FORWARD)?
                      (PrevEcurve.FirstParameter() + nbs*SampleOnPrev) :
                      (PrevEcurve.FirstParameter() + (NbSamples-nbs)*SampleOnPrev);
                    Standard_Real ParOnCur = (CurEdge.Orientation() == TopAbs_FORWARD)?
                      (Ecurve.FirstParameter() + nbs*SampleOnCur) :
                      (Ecurve.FirstParameter() + (NbSamples-nbs)*SampleOnCur);
                    gp_Pnt PonPrev = PrevEcurve.Value(ParOnPrev);
                    gp_Pnt PonCur = Ecurve.Value(ParOnCur);
                    SumDist += PonPrev.Distance(PonCur);
                  }
                }
	      }
	    if (SumDist < MinSumDist)
	      {
		MinSumDist = SumDist;
		jmin = j;
		forward = Standard_True;
	      }
	    
	    //Backward
	    SumDist = 0.;
	    for (k = j, n = 1; k >= 1; k--, n++)
	      {
		const TopoDS_Vertex& Vprev = TopoDS::Vertex( PrevSeq(n) );
		gp_Pnt Pprev = BRep_Tool::Pnt(Vprev);
		const TopoDS_Vertex& V = TopoDS::Vertex( SeqVertices(k) );
		gp_Pnt P = BRep_Tool::Pnt(V);
		SumDist += Pprev.Distance(P);
                if (NbSamples > 0)
                {
                  Standard_Integer k_cur = k-1;
                  if (k_cur == 0)
                    k_cur = theLength;
                  const TopoDS_Edge& PrevEdge = TopoDS::Edge(PrevEseq(n));
                  const TopoDS_Edge& CurEdge = TopoDS::Edge(SeqEdges(k_cur));
                  BRepAdaptor_Curve PrevEcurve(PrevEdge);
                  BRepAdaptor_Curve Ecurve(CurEdge);
                  Standard_Real SampleOnPrev = (PrevEcurve.LastParameter()-PrevEcurve.FirstParameter())/NbSamples;
                  Standard_Real SampleOnCur = (Ecurve.LastParameter()-Ecurve.FirstParameter())/NbSamples;
                  for (nbs = 1; nbs <= NbSamples-1; nbs++)
                  {
                    Standard_Real ParOnPrev = (PrevEdge.Orientation() == TopAbs_FORWARD)?
                      (PrevEcurve.FirstParameter() + nbs*SampleOnPrev) :
                      (PrevEcurve.FirstParameter() + (NbSamples-nbs)*SampleOnPrev);
                    Standard_Real ParOnCur = (CurEdge.Orientation() == TopAbs_FORWARD)?
                      (Ecurve.FirstParameter() + (NbSamples-nbs)*SampleOnCur) :
                      (Ecurve.FirstParameter() + nbs*SampleOnCur);
                    gp_Pnt PonPrev = PrevEcurve.Value(ParOnPrev);
                    gp_Pnt PonCur = Ecurve.Value(ParOnCur);
                    SumDist += PonPrev.Distance(PonCur);
                  }
                }
	      }
	    for (k = theLength; k > j; k--, n++)
	      {
		const TopoDS_Vertex& Vprev = TopoDS::Vertex( PrevSeq(n) );
		gp_Pnt Pprev = BRep_Tool::Pnt(Vprev);
		const TopoDS_Vertex& V = TopoDS::Vertex( SeqVertices(k) );
		gp_Pnt P = BRep_Tool::Pnt(V);
		SumDist += Pprev.Distance(P);
                if (NbSamples > 0)
                {
                  const TopoDS_Edge& PrevEdge = TopoDS::Edge(PrevEseq(n));
                  const TopoDS_Edge& CurEdge = TopoDS::Edge(SeqEdges(k-1));
                  BRepAdaptor_Curve PrevEcurve(PrevEdge);
                  BRepAdaptor_Curve Ecurve(CurEdge);
                  Standard_Real SampleOnPrev = (PrevEcurve.LastParameter()-PrevEcurve.FirstParameter())/NbSamples;
                  Standard_Real SampleOnCur = (Ecurve.LastParameter()-Ecurve.FirstParameter())/NbSamples;
                  for (nbs = 1; nbs <= NbSamples-1; nbs++)
                  {
                    Standard_Real ParOnPrev = (PrevEdge.Orientation() == TopAbs_FORWARD)?
                      (PrevEcurve.FirstParameter() + nbs*SampleOnPrev) :
                      (PrevEcurve.FirstParameter() + (NbSamples-nbs)*SampleOnPrev);
                    Standard_Real ParOnCur = (CurEdge.Orientation() == TopAbs_FORWARD)?
                      (Ecurve.FirstParameter() + (NbSamples-nbs)*SampleOnCur) :
                      (Ecurve.FirstParameter() + nbs*SampleOnCur);
                    gp_Pnt PonPrev = PrevEcurve.Value(ParOnPrev);
                    gp_Pnt PonCur = Ecurve.Value(ParOnCur);
                    SumDist += PonPrev.Distance(PonCur);
                  }
                }
	      }
	    if (SumDist < MinSumDist)
	      {
		MinSumDist = SumDist;
		jmin = j;
		forward = Standard_False;
	      }
	  }
      
      PrevSeq.Clear();
      PrevEseq.Clear();
      if (forward)
	{
	  for (j = jmin; j <= theLength; j++)
	    {
	      BB.Add( newwire, TopoDS::Edge(SeqEdges(j)) );
	      PrevSeq.Append( SeqVertices(j) );
              PrevEseq.Append( SeqEdges(j) );
	    }
	  for (j = 1; j < jmin; j++)
	    {
	      BB.Add( newwire, TopoDS::Edge(SeqEdges(j)) );
	      PrevSeq.Append( SeqVertices(j) );
              PrevEseq.Append( SeqEdges(j) );
	    }
	}
      else
	{
	  for (j = jmin-1; j >= 1; j--)
	    {
	      TopoDS_Shape aLocalShape = SeqEdges(j).Reversed();
	      BB.Add( newwire, TopoDS::Edge(aLocalShape) );
	      //PrevSeq.Append( SeqVertices(j) );
              PrevEseq.Append( SeqEdges(j).Reversed() );
	    }
	  for (j = theLength; j >= jmin; j--)
	    {
	      TopoDS_Shape aLocalShape = SeqEdges(j).Reversed();
	      BB.Add( newwire, TopoDS::Edge(aLocalShape) );
	      //PrevSeq.Append( SeqVertices(j) );
              PrevEseq.Append( SeqEdges(j).Reversed() );
	    }
	  for (j = jmin; j >= 1; j--)
	    PrevSeq.Append( SeqVertices(j) );
	  for (j = theLength; j > jmin; j--)
	    PrevSeq.Append( SeqVertices(j) );
	}
      
      newwire.Closed( Standard_True );
      newwire.Orientation( TopAbs_FORWARD );
      myWork(i) = newwire;
    }

/*  
  for ( i=ideb; i<=myWork.Length(); i++) {
    
    const TopoDS_Wire& wire = TopoDS::Wire(myWork(i));
    
    Standard_Integer nbEdges=0;
    for(anExp.Init(TopoDS::Wire(myWork(i))); anExp.More(); anExp.Next())
      nbEdges++;
    TopExp::Vertices(wire,Vdeb,Vfin);
    Standard_Boolean wClosed = wire.Closed();
    if (!wClosed) {
      // on regarde quand meme si les vertex sont les memes.
      if ( Vdeb.IsSame(Vfin)) wClosed = Standard_True;
    }
    
    
    TopoDS_Vertex Vsuiv, VF, VR;
    TopoDS_Wire newwire;
    BRep_Builder BW;
    BW.MakeWire(newwire);
    if (i==ideb) {
      anExp.Init(wire);
      const TopoDS_Edge Ecur = TopoDS::Edge(anExp.Current());
      TopExp::Vertices(Ecur,VF,VR);
      if (Vdeb.IsSame(VF)) Vsuiv=VR;
      else if (Vdeb.IsSame(VR)) Vsuiv=VF;
      else {
	// par defaut on prend l'origine sur cette arete
	if (VR.IsSame(TopoDS::Vertex(anExp.CurrentVertex()))) {
	  Vdeb = VR;
	  Vsuiv = VF;
	}
	else {
	  Vdeb = VF;
	  Vsuiv = VR;
	}
      }
      Pdeb=BRep_Tool::Pnt(Vdeb);
      Psuiv=BRep_Tool::Pnt(Vsuiv);
      Standard_Real U1 = BRep_Tool::Parameter(Vdeb,Ecur);
      Standard_Real U2 = BRep_Tool::Parameter(Vsuiv,Ecur);
      BRepAdaptor_Curve Curve(Ecur);
      PPs = Curve.Value(0.25*(U1+3*U2));
      myWork(ideb) = wire;
    }
    else {
      // on ramene Pdeb, Psuiv et PPs dans le plan courant
      gp_Pnt Pnew,Pnext,PPn; 
      Transform(Standard_True,Pdeb,Pos->Value(i-1),Axe->Value(i-1), 
			      Pos->Value(i),Axe->Value(i),Pnew);
      Transform(Standard_True,Psuiv,Pos->Value(i-1),Axe->Value(i-1), 
			      Pos->Value(i),Axe->Value(i),Pnext);
      Transform(Standard_True,PPs,Pos->Value(i-1),Axe->Value(i-1), 
			      Pos->Value(i),Axe->Value(i),PPn);
      
      Standard_Real distmini,dist;
      Standard_Integer rang=0,rangdeb=0;
      TopoDS_Vertex Vmini;
      gp_Pnt Pmini,P1,P2;
      SeqOfVertices(wire,SeqV);
      if (SeqV.Length()>NbMaxV) 
	Standard_NoSuchObject::Raise("BRepFill::ComputeOrigin failed");
      if (!polar) {
	// choix du vertex le plus proche comme origine
	distmini = Precision::Infinite();
	for (Standard_Integer ii=1;ii<=SeqV.Length();ii++) {
	  P1 = BRep_Tool::Pnt(TopoDS::Vertex(SeqV.Value(ii)));
	  dist = P1.Distance(Pnew);
	  if (dist<distmini) {
	    distmini = dist;
	    Vmini = TopoDS::Vertex(SeqV.Value(ii));
	  }
	}
	if (!Vmini.IsNull()) Pmini = BRep_Tool::Pnt(Vmini);
      }
      else {
	
	// recherche du vertex correspondant a la projection conique
	Standard_Real angmin, angV, eta = Precision::Angular();
	TopoDS_Vertex Vopti;
	angmin = PI/2;
	distmini = Precision::Infinite();
	gp_Dir dir0(gp_Vec(Pnew,P.Location()));
	for (Standard_Integer ii=1;ii<=SeqV.Length();ii++) {
	  P1 = BRep_Tool::Pnt(TopoDS::Vertex(SeqV.Value(ii)));
	  dist = Pnew.Distance(P1);
	  if (dist<Precision::Confusion()) {
	    angV = 0.0;
	  }
	  else {
	    gp_Dir dir1(gp_Vec(Pnew,P1));
	    angV = dir1.Angle(dir0);
	  }
	  if (angV>PI/2) angV = PI - angV;
	  if (angmin>angV+eta) {
	    distmini = dist;
	    angmin = angV;
	    Vopti = TopoDS::Vertex(SeqV.Value(ii));
	  }
	  else if (Abs(angmin-angV)<eta) {
	    if (dist<distmini) {
	      distmini = dist;
	      angmin = angV;
	      Vopti = TopoDS::Vertex(SeqV.Value(ii));
	    }
	  }
	}
	gp_Pnt Popti;
	if (!Vopti.IsNull()) Popti = BRep_Tool::Pnt(Vopti);
	Vmini = Vopti;
	
      }

      distmini = Precision::Infinite();
      for (anExp.Init(wire); anExp.More(); anExp.Next()) {
	TopoDS_Edge Ecur = anExp.Current();
	TopoDS_Vertex Vcur = anExp.CurrentVertex();
	TopExp::Vertices(Ecur,VF,VR);
	if (VF.IsSame(Vmini)) {
	  P1 = BRep_Tool::Pnt(VR);
	  dist = P1.Distance(Pnext);
	  if (dist<=distmini) {
	    distmini = dist;
	    Vsuiv = VR;
	  }
	}
	if (VR.IsSame(Vmini)) {
	  P1 = BRep_Tool::Pnt(VF);
	  dist = P1.Distance(Pnext);
	  if (dist<distmini) {
	    distmini = dist;
	    Vsuiv = VF;
	  }
	}
      }
      
      // choix du sens de parcours en fonction de Pnext
      Standard_Boolean parcours = Standard_False;
      if (i==myWork.Length() && myDegen2) {
	// derniere section ponctuelle
	rangdeb = 1;
	parcours = Standard_True;
      }
      else {
	// cas general
	gp_Pnt Pbout = Pnext;
	TopoDS_Edge E1,E2;
	TopoDS_Vertex V1,V2;
	EdgesFromVertex(wire,Vmini,E1,E2);
	
	TopExp::Vertices(E1,V1,V2,Standard_True);
#ifndef DEB
	Standard_Real U1=0, U2=0;
#else
	Standard_Real U1, U2;
#endif
	if (Vmini.IsSame(V1)) { 
	  P1 = BRep_Tool::Pnt(V2);
	  U1 = 0.25*(BRep_Tool::Parameter(V1,E1)+3*BRep_Tool::Parameter(V2,E1));
	}
	if (Vmini.IsSame(V2)) {
	  P1 = BRep_Tool::Pnt(V1);
	  U1 = 0.25*(3*BRep_Tool::Parameter(V1,E1)+BRep_Tool::Parameter(V2,E1));
	}
	
	TopExp::Vertices(E2,V1,V2,Standard_True);
	if (Vmini.IsSame(V1)) { 
	  P2 = BRep_Tool::Pnt(V2);
	  U2 = 0.25*(BRep_Tool::Parameter(V1,E2)+3*BRep_Tool::Parameter(V2,E2));
	}
	if (Vmini.IsSame(V2)) {
	  P2 = BRep_Tool::Pnt(V1);
	  U2 = 0.25*(3*BRep_Tool::Parameter(V1,E2)+BRep_Tool::Parameter(V2,E2));
	}
	
	if (Abs(Pbout.Distance(P1)-Pbout.Distance(P2))<Precision::Confusion()) {
	  // cas limite ; on se decale un peu
	  Pbout = PPn;
	  BRepAdaptor_Curve Curve1(E1);
	  P1 = Curve1.Value(U1);
	  BRepAdaptor_Curve Curve2(E2);
	  P2 = Curve2.Value(U2);
	}
	
	// calcul de rangdeb
	rangdeb = 0;
	if (Pbout.Distance(P1)<Pbout.Distance(P2)){
	  // P1 est plus proche; parcours = False
	  parcours = Standard_False;
	  rang = 0;
	  for (anExp.Init(wire); anExp.More(); anExp.Next()) {
	    rang++;
	    TopoDS_Edge Ecur = anExp.Current();
	    if (E1.IsSame(Ecur)) {
	      rangdeb = rang;
	    }
	  }
	  BRepAdaptor_Curve Curve(E1);
	  PPs = Curve.Value(U1);
	}
	else {
	  // P2 est plus proche; parcours = True
	  parcours = Standard_True;
	  rang = 0;
	  for (anExp.Init(wire); anExp.More(); anExp.Next()) {
	    rang++;
	    TopoDS_Edge Ecur = anExp.Current();
	    if (E2.IsSame(Ecur)) {
	      rangdeb = rang;
	    }
	  }
	  BRepAdaptor_Curve Curve(E2);
	  PPs = Curve.Value(U2);
	}
      }

      // reconstruction du wire a partir de rangdeb
      TopTools_SequenceOfShape SeqEdges;
      SeqEdges.Clear();
      for (anExp.Init(TopoDS::Wire(wire)); anExp.More(); anExp.Next()) {
	SeqEdges.Append(anExp.Current());
      }
      if (parcours) {
	for (rang=rangdeb;rang<=nbEdges;rang++) {
	  BW.Add(newwire,TopoDS::Edge(SeqEdges.Value(rang)));
	}
	for (rang=1;rang<rangdeb;rang++) {
	  BW.Add(newwire,TopoDS::Edge(SeqEdges.Value(rang)));
	}
      }
      else {
	for (rang=rangdeb;rang>=1;rang--) {
	  TopoDS_Shape aLocalShape = SeqEdges.Value(rang).Reversed();
	  BW.Add(newwire,TopoDS::Edge(aLocalShape));
//	  BW.Add(newwire,TopoDS::Edge(SeqEdges.Value(rang).Reversed()));
	}
	for (rang=nbEdges;rang>rangdeb;rang--) {
	  TopoDS_Shape aLocalShape = SeqEdges.Value(rang).Reversed();
	  BW.Add(newwire,TopoDS::Edge(aLocalShape));
//	  BW.Add(newwire,TopoDS::Edge(SeqEdges.Value(rang).Reversed()));
	}
      }
      
      myWork(i) = newwire.Oriented(TopAbs_FORWARD);
      
      // on passe au wire suivant
      if (!Vmini.IsNull()) Pdeb=BRep_Tool::Pnt(Vmini);
      if (!Vsuiv.IsNull()) Psuiv=BRep_Tool::Pnt(Vsuiv);
    }
  }
*/
  
  // sections bouclantes ?
  if (vClosed) myWork(myWork.Length()) = myWork(1);
}

//=======================================================================
//function : SearchOrigin
//purpose  : 
//=======================================================================

void BRepFill_CompatibleWires::SearchOrigin()
{
  // reorganize the open wires respecting orientation and origin
  
  gp_Pln P0,P;  

  TopoDS_Vertex Vdeb, Vfin;
  gp_Pnt Pdeb,  Pfin;//,Psuiv;

  BRepTools_WireExplorer anExp;

  Standard_Boolean allOpen = Standard_True;
  Standard_Integer ideb=1, ifin=myWork.Length();
  if (myDegen1) ideb++;
  if (myDegen2) ifin--;
  Standard_Boolean vClosed = (!myDegen1) && (!myDegen2)
                                && (myWork(ideb).IsSame(myWork(ifin)));
  
//  for (Standard_Integer i=ideb; i<=ifin; i++) {
  Standard_Integer i;
  for  (i=ideb; i<=ifin; i++) {
    allOpen = (allOpen && !myWork(i).Closed());
  }
  if (!allOpen)
    Standard_NoSuchObject::Raise("BRepFill_CompatibleWires::SearchOrigin : the wires must be open");

  // init

  TopoDS_Wire wire1 = TopoDS::Wire(myWork(ideb));
  wire1.Orientation(TopAbs_FORWARD);
  TopExp::Vertices(wire1,Vdeb,Vfin);
  Pdeb = BRep_Tool::Pnt(Vdeb);
  Pfin = BRep_Tool::Pnt(Vfin);
  Standard_Boolean isline0 = (!PlaneOfWire(wire1,P0)), isline;
  myWork(ideb) = wire1;
  //OCC86
  anExp.Init(wire1);
  TopoDS_Edge E0 = anExp.Current(), E;

  for ( i=ideb+1; i<=ifin; i++) {

    TopoDS_Wire wire = TopoDS::Wire(myWork(i));
    wire.Orientation(TopAbs_FORWARD);

    TopTools_SequenceOfShape SeqEdges;
    SeqEdges.Clear();
    Standard_Integer nbEdges=0;
    //OCC86  for(anExp.Init(wire); anExp.More(); anExp.Next()) {
    for(anExp.Init(wire), E = anExp.Current(); anExp.More(); anExp.Next()) {
      SeqEdges.Append(anExp.Current());
      nbEdges++;
    }
    TopExp::Vertices(wire,Vdeb,Vfin);
    isline = (!PlaneOfWire(wire,P));

    TopoDS_Vertex Vmini;
    TopoDS_Wire newwire;
    BRep_Builder BW;
    BW.MakeWire(newwire);
    Standard_Boolean parcours = Standard_True;

    if (isline0 || isline) {
      
      // cas particulier des segments de droite
      gp_Pnt P1 = BRep_Tool::Pnt(Vdeb),
             P2 = BRep_Tool::Pnt(Vfin);
      Standard_Real dist1, dist2;
      dist1 = Pdeb.Distance(P1)+Pfin.Distance(P2);
      dist2 = Pdeb.Distance(P2)+Pfin.Distance(P1);
      parcours = (dist2>=dist1);
    }
    
    else {
      //OCC86
      gp_Pnt P1 = BRep_Tool::Pnt(Vdeb), P1o = Pdeb,
             P2 = BRep_Tool::Pnt(Vfin), P2o = Pfin;
/*    // on ramene Pdeb dans le plan courant
      gp_Pnt Pnew = Pdeb.Translated (P0.Location(),P.Location());
      gp_Ax1 A0 = P0.Axis();
      gp_Ax1 A1 = P.Axis();
      
      if (!A0.IsParallel(A1,1.e-4)) {
	gp_Vec vec1(A0.Direction()), vec2(A1.Direction()), 
	norm = vec1 ^ vec2;
	gp_Ax1 Norm(P.Location(),norm);
	Standard_Real ang = vec1.AngleWithRef(vec2,norm);
	if (ang > PI/2.0)
	  ang = PI - ang;
	if (ang < -PI/2.0)
	  ang = -PI - ang;
	if (Abs(ang-PI/2.0)<Precision::Angular()) {
	  // cas d'ambiguite
	  gp_Vec Vtrans(P0.Location(),P.Location()),Vsign;
	  Standard_Real alpha,beta,sign=1;
	  Vsign.SetLinearForm(Vtrans.Dot(vec1),vec2,-Vtrans.Dot(vec2),vec1);
	  alpha = Vsign.Dot(vec1);
	  beta = Vsign.Dot(vec2);
	  Standard_Boolean pasnul = (Abs(alpha)>1.e-4 && Abs(beta)>1.e-4);
	  if ( alpha*beta>0.0 && pasnul ) sign=-1;
	  ang *= sign;
        }
	Pnew = Pnew.Rotated (Norm,ang);
      }
      // choix entre Vdeb et Vfin
      Standard_Real dist = Pnew.Distance(P1);
      parcours = (dist<Pnew.Distance(P2));
*/      
      if(P1.IsEqual(P2,Precision::Confusion()) || P1o.IsEqual(P2o,Precision::Confusion())){
	BRepAdaptor_Curve Curve0(E0), Curve(E);
	Curve0.D0(Curve0.FirstParameter() + Precision::Confusion(), P2o);
	Curve.D0(Curve.FirstParameter() + Precision::Confusion(), P2);
      };
      gp_Vec VDebFin0(P1o,P2o), VDebFin(P1,P2);
      Standard_Real AStraight = VDebFin0.Angle(VDebFin);
      parcours = (AStraight < PI/2.0? Standard_True: Standard_False);
    }
    
    // reconstruction du wire
    Standard_Integer rang;
    if (parcours) {
      for (rang=1;rang<=nbEdges;rang++) {
	TopoDS_Shape alocalshape = SeqEdges.Value(rang);
	BW.Add(newwire,TopoDS::Edge(alocalshape));
//	BW.Add(newwire,TopoDS::Edge(SeqEdges.Value(rang)));
      }
    }
    else {
      for (rang=nbEdges;rang>=1;rang--) {
	TopoDS_Shape alocalshape = SeqEdges.Value(rang).Reversed();
	BW.Add(newwire,TopoDS::Edge(alocalshape));
//	BW.Add(newwire,TopoDS::Edge(SeqEdges.Value(rang).Reversed()));
      }
    }

    // orientation du wire
    newwire.Oriented(TopAbs_FORWARD);
    myWork(i) = newwire;

    // on passe au wire suivant
    if (parcours) {
      Pdeb = BRep_Tool::Pnt(Vdeb);
      Pfin = BRep_Tool::Pnt(Vfin);
    }
    else {
      Pfin = BRep_Tool::Pnt(Vdeb);
      Pdeb = BRep_Tool::Pnt(Vfin);
    }
    P0 = P;
    isline0 = isline;
    //OCC86
    E0 = E;
  }
  
  // sections bouclantes ?
  if (vClosed) myWork(myWork.Length()) = myWork(1);
}
