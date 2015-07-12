// Created on: 1995-08-04
// Created by: Jean Yves LEBEY
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


#include <gp_Pnt.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep_FacesFiller.hxx>
#include <TopOpeBRep_FacesIntersector.hxx>
#include <TopOpeBRep_FFDumper.hxx>
#include <TopOpeBRep_LineInter.hxx>
#include <TopOpeBRep_PointClassifier.hxx>
#include <TopOpeBRep_VPointInter.hxx>
#include <TopOpeBRep_VPointInterClassifier.hxx>
#include <TopOpeBRep_VPointInterIterator.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_Transition.hxx>

#ifdef DRAW
#include <TopOpeBRepDS_DRAW.hxx>
#endif

#include <Geom_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Curve.hxx>

#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>
//#include <BRepAdaptor_Curve2d.hxx>
#include <gp_Vec.hxx>
#include <BRep_Tool.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <Precision.hxx>

#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRep.hxx>
#include <TopOpeBRep_define.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_mkTondgE.hxx>

#define M_ON(st)       (st == TopAbs_ON) 
#define M_REVERSED(st) (st == TopAbs_REVERSED) 

// modified by NIZHNY-MKK  Tue Nov 21 17:30:23 2000.BEGIN
static TopTools_DataMapOfShapeListOfShape aMapOfTreatedVertexListOfEdge;
static TopOpeBRep_PLineInter localCurrentLine=NULL;

static Standard_Boolean local_FindTreatedEdgeOnVertex(const TopoDS_Edge& theEdge,
						      const TopoDS_Vertex& theVertex);

static void local_ReduceMapOfTreatedVertices(const TopOpeBRep_PLineInter& theCurrentLine);

static Standard_Boolean local_FindVertex(const TopOpeBRep_VPointInter& theVP,
					 const TopTools_IndexedDataMapOfShapeListOfShape& theMapOfVertexEdges,
					 TopoDS_Vertex& theVertex);
// modified by NIZHNY-MKK  Tue Nov 21 17:30:27 2000.END

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepDS_GettraceDEGEN();
extern Standard_Boolean TopOpeBRepDS_GettraceDSF();
Standard_EXPORT Standard_Boolean FUN_debnull(const TopoDS_Shape& s);
#endif

Standard_EXPORT  Handle(TopOpeBRepDS_Interference) MakeEPVInterference
(const TopOpeBRepDS_Transition& T, // transition
 const Standard_Integer S, // curve/edge index
 const Standard_Integer G, // point/vertex index
 const Standard_Real P, // parameter of G on S
 const TopOpeBRepDS_Kind GK,
 const Standard_Boolean B); // G is a vertex (or not) of the interference master

Standard_EXPORT Handle(TopOpeBRepDS_Interference) MakeEPVInterference
(const TopOpeBRepDS_Transition& T, // transition
 const Standard_Integer S, // curve/edge index
 const Standard_Integer G, // point/vertex index
 const Standard_Real P, // parameter of G on S
 const TopOpeBRepDS_Kind GK, // POINT/VERTEX
 const TopOpeBRepDS_Kind SK,
 const Standard_Boolean B); // G is a vertex (or not) of the interference master


#define M_FINDVP  (0) // only look for new vp
#define M_MKNEWVP (1) // only make newvp
#define M_GETVP   (2) // steps (0) [+(1) if (O) fails]
Standard_EXPORT void FUN_VPIndex
(TopOpeBRep_FacesFiller& FF,
 const TopOpeBRep_LineInter& L,
 const TopOpeBRep_VPointInter& VP,
 const Standard_Integer ShapeIndex,
 const Handle(TopOpeBRepDS_HDataStructure)& HDS,
 const TopOpeBRepDS_ListOfInterference& DSCIL,
 TopOpeBRepDS_Kind& PVKind, Standard_Integer& PVIndex, // out
 Standard_Boolean& EPIfound, Handle(TopOpeBRepDS_Interference)& IEPI, // out 
 Standard_Boolean& CPIfound, Handle(TopOpeBRepDS_Interference)& ICPI, // out 
 const Standard_Integer mkVP);


/*
// *******************   methods for the compute   **********************
// ******************   of interferences on degenerated edge   **********

// UVonEofF(par) = 2d point of parameter par on edge E in F's  2d representation.
// UVonF(E)      = E's pcurve on F
// UVonF(V)      = V's UV on F's 2d representation.

#define UNDEF (0)
#define FIRST (1)
#define LAST  (2)

static void FUN_GetVectors(const TopoDS_Face& Fe, const TopoDS_Face& Fi,
			   const TopoDS_Edge& Ec,
			   const gp_Pnt2d& p2di, const gp_Pnt2d& p2de,
			   const TopoDS_Vertex& ve,
			   gp_Vec& ngFe, gp_Vec& ngFi,
			   gp_Vec& a, gp_Vec& x)
{
   // geometric normals to Fe and Fi : ngFe,ngFi
  ngFi = FUN_tool_nggeomF(p2di, Fi);
  ngFe = FUN_tool_nggeomF(p2de, Fe);
  // tgEc
  gp_Vec tgEc;
//  Standard_Integer orivine = FUN_tool_orientVinE(ve,Ec);
//  Standard_Real parOnEc = BRep_Tool::Parameter(ve,Ec);
//  tgEc = FUN_tool_tggeomE(parOnEc,Ec);
//  a = tgEc; if (orivine == LAST) a.Reverse();
  Standard_Integer ovine; Standard_Boolean ok = TopOpeBRepTool_TOOL::TgINSIDE(ve,Ec,a,ovine);  

  x = ngFe^ngFi; 
}

static void FUN_getEofFwithV(const TopoDS_Vertex& v, const TopoDS_Face& Fi, TopTools_ListOfShape& los)
{
  // get <los>, the edges of <Fi> binding vertex <V>
  los.Clear(); Standard_Integer ned = 0;

  TopExp_Explorer exe(Fi, TopAbs_EDGE);
  for (; exe.More(); exe.Next()) {
    const TopoDS_Shape& edge = exe.Current();
    TopExp_Explorer exv(edge, TopAbs_VERTEX);
    for (; exv.More(); exv.Next()) {
      if (exv.Current().IsSame(v)) {
	los.Append(edge);
	ned++; break;
      }
    }
    if (ned == 2) return;
  }
}
 
static Standard_Real FUN_getpar(const gp_Dir& a, const gp_Dir& x, const gp_Dir& y, 
		      const Standard_Boolean& complement)
{
  // Computes the parameter on degenerated edge (on circle) :
  // <Fe> on spherical surface
  // <Fi> the incident face
  // watching our system in a XY space normal to ngFe :
  // <a> : tangent with tgEc, going OUT the sphere, while
  //       following the geometry of <Ec>
  // <x> : normal to ngFi, oriented INSIDE Fi
  // <y> : normal to <x>, oriented OUTSIDE the matter delimited
  //       by the oriented face <Fi>.
  // <par> is the oriented angle (<a>,<x>), computed in the 
  // anti-trigonometric sense (defined by RONd (<x>,<y>,<z>).

  // If <complement>, nrotation has a direction opposite to z,
  // parameter on sphere = 2PI - parameter computed.

  Standard_Real x1 = a.Dot(x);
  Standard_Real x2 = a.Dot(y);
  Standard_Real par;  
  Standard_Real tol = Precision::Angular();
  Standard_Boolean x1null = (Abs(x1) <= tol);
  Standard_Boolean x2null = (Abs(x2) <= tol);

  if      (x1null) par = (x2 > 0.) ? 3.*M_PI*.5 : M_PI*.5;
  else if (x2null) par = (x1 > 0.) ? 2.*M_PI    : M_PI;
  else {
    Standard_Real ac = ACos(Abs(x1));
    Standard_Boolean x1pos = (x1 > tol);
    Standard_Boolean x2pos = (x2 > tol);
    if (x1pos && x2pos)   par = 2.*M_PI-ac;
    if (x1pos && !x2pos)  par = ac;
    if (!x1pos && x2pos)  par = M_PI+ac;
    if (!x1pos && !x2pos) par = M_PI-ac;
  }

  if (complement) par = 2.*M_PI - par;
  return par;
}

static void FUN_getloEi(const gp_Pnt2d& p2d, const TopoDS_Shape& F, TopTools_ListOfShape& loE)
{
  loE.Clear(); Standard_Integer nsol = 0;
  // looking for restrictions of <F> / <p2d> is ON these restrictions.
  // if <onlyoneE>, looks get the first restriction solution,
  //                else looks among all the restrictions for edges solutions
  Standard_Real tol = Precision::Parametric(Precision::Confusion());
  TopExp_Explorer ex(F, TopAbs_EDGE);
  for (; ex.More(); ex.Next()){
    const TopoDS_Edge& E = TopoDS::Edge(ex.Current());
    if ( E.Orientation() == TopAbs_INTERNAL )     continue;
    if ( BRep_Tool::IsClosed(E,TopoDS::Face(F)) ) continue;

    Standard_Real par,dist;
    Standard_Boolean ok = TopOpeBRepTool_TOOL::ParE2d(p2d,E,TopoDS::Face(F), par,dist);
    if (!ok) continue;
    Standard_Real tolp = TopOpeBRepTool_TOOL::TolP(E,TopoDS::Face(F));
    if (dist < tolp) {loE.Append(E); nsol++;}
    if (nsol == 2) return;
  }
}

static Standard_Boolean FUN_paronE(const gp_Pnt2d& p2d, const TopoDS_Shape& E, const TopoDS_Shape& F,
		      Standard_Real& paronE)
{
  // <p2d> on UVon<F>(<E>)
  // Purpose : finding out paronE / UVon<E>(paronE) = <p2d>
  Standard_Real par,dist;
  Standard_Boolean ok = TopOpeBRepTool_TOOL::ParE2d(p2d,TopoDS::Edge(E),TopoDS::Face(F), par,dist);
  if (!ok) return Standard_False;
  Standard_Real tolp = TopOpeBRepTool_TOOL::TolP(TopoDS::Edge(E),TopoDS::Face(F));
  if (dist < tolp) {paronE = par; return Standard_True;}
  else             return Standard_False;
}

static Standard_Boolean FUN_nullx(const gp_Vec& x)
{
  Standard_Real tola = Precision::Confusion();
  Standard_Real mag = x.Magnitude();
  Standard_Boolean isnull = (mag < tola);
  return isnull;
}

static Standard_Boolean FUN_OOEi(const gp_Pnt2d& p2di, const TopoDS_Face& Fi,
		    TopoDS_Edge& OOEi, Standard_Real& paronOOEi)     
{
  // ngFe // ngFi : <Fi> is tangent to the sphere on its
  // degenerated edge.
  // vertex is on <Ec> (sphere's closing edge) and <OOEi> (on <Fi>).
  // <p2di> = UVon<Fi>(<ve>) 
  Standard_Boolean isplane = FUN_tool_plane(Fi); if (isplane) return Standard_False;
  
  // --------------------------------------------------
  // SUPPLYING INTPATCH :
  // The geometry of <ve> is on 2 edges <Ec> and <OOEi>
  // and <OOEi> is not found touched.
  // --------------------------------------------------
  // <loEi> is the list of edges of <Fi> not INTERNAL
  // and non-closing.
  TopTools_ListOfShape loEi; FUN_getloEi(p2di,Fi,loEi);
  if (loEi.Extent() != 1) return Standard_False;
  
  OOEi = TopoDS::Edge(loEi.First());
  Standard_Boolean done = FUN_paronE(p2di,OOEi,Fi,paronOOEi); 
  return done;
}

static void FUN_transitiononedge
(const TopAbs_State& staB, const TopAbs_State& staA, TopOpeBRepDS_Transition& T)
{
  T = TopOpeBRepDS_Transition(staB,staA,TopAbs_EDGE,TopAbs_EDGE);
} 

static Standard_Boolean FUN_IEcOOEi
(const TopoDS_Vertex& ve, const Standard_Real& paronOOEi, const TopoDS_Edge& OOEi, const TopoDS_Edge& Ec, 
 TopOpeBRepDS_Transition& TOOEi)
{
  FUN_transitiononedge(TopAbs_UNKNOWN,TopAbs_UNKNOWN,TOOEi);
  // ------------------------------------------------------------
  // SUPPLYING INTPATCH :
  // when tg<OOEi> and tg<Ec> are tangent,
  // the interference <OOEi> with <Ec> at vertex <v> is not found 
  // => we have to compute the transition <T> :
  // following <OOEi>'s geometry, we cross <Ec> at vertex <ve>
  // and describe the transition <T>.
  // ------------------------------------------------------------
  gp_Vec dirOOEi = FUN_tool_tggeomE(paronOOEi,OOEi);
  
  Standard_Real paronEc; 
  Standard_Boolean ok = FUN_tool_parVonE(ve,Ec,paronEc); if (!ok) return Standard_False;
  gp_Vec dirEc = FUN_tool_tggeomE(paronEc,Ec);

  Standard_Real prod = dirOOEi.Dot(dirEc);
  Standard_Real tola = Precision::Angular();
  Standard_Boolean dEctgdOOEi = (Abs(1-Abs(prod)) < tola);
  if (!dEctgdOOEi) return Standard_False;
  
  // get <dparonOOEi>, 
  // compute <paronOOEi> = point of <OOEi> at param=paronOOEi+dparonOOEi
  Standard_Real f,l; Handle(Geom_Curve) C = BRep_Tool::Curve(OOEi,f,l);
  Standard_Real dparonOOEi = (l-f)*0.05; // NYI : find a better value
  Standard_Real tol = Precision::Parametric( Precision::Confusion());
  Standard_Boolean islast  = (Abs(paronOOEi-l) < tol);
  Standard_Boolean isfirst = (Abs(paronOOEi-f) < tol);
  if (islast || isfirst) return Standard_False;
  Standard_Real param = paronOOEi+dparonOOEi;
  gp_Pnt ptonOOEi = C->Value(param);
  
  // classify <paronOOEi> IN <Ec>
  TopAbs_State sta = FUN_tool_staPinE(ptonOOEi,Ec);
  if (sta == TopAbs_IN)  FUN_transitiononedge(TopAbs_OUT,TopAbs_IN,TOOEi);
  if (sta == TopAbs_OUT) FUN_transitiononedge(TopAbs_IN,TopAbs_OUT,TOOEi);
  return Standard_True;
}

#define s_NOTdgE (0) // do NOT compute any transition
#define s_TdgE   (1) // compute transition on dgE
#define s_TOOEi  (2) // compute transition on OOEi

// case VP is ON the boundary of <Fi> (on edge <Ei>)
static Standard_Integer FUN_parondgEONFi
(const TopOpeBRep_VPointInter& VP,
 const Standard_Boolean visvon12,      
 const TopoDS_Vertex& ve, 
 const TopoDS_Vertex& vi,   // dummy if !visvon12
 const Standard_Integer is,    // rank of <Fe>
 const TopoDS_Face& Fe,   // contains Ed, Ec
 const TopoDS_Face& Fi,   // face of shape is, contains Ei
 const TopoDS_Edge& Ed,   // degenerated edge
 const TopoDS_Edge& Ei,   // incident edge
 const TopoDS_Edge& Ec,   // couture edge
 Standard_Real& paronEd,TopOpeBRepDS_Transition& T,                   // !s_NOTdgE  
 TopoDS_Edge& OOEi, Standard_Real& paronOOEi, TopOpeBRepDS_Transition& TOOEi) // s_TOOEi
{
  // <Fe>, <Ed>, <ve> are of rank <is>, <Ed> is on face <Fe>
  // <Fi>, <Ei>, <vi> are of rank <ioo>, <Ei> is on face <Fi>
  Standard_Integer ioo = (is == 1) ? 2 : 1;
  Standard_Integer sind = VP.ShapeIndex();
  
  // p2di and p2de :
  gp_Pnt2d p2de = VP.SurfaceParameters(is);
  
  gp_Pnt2d p2di = VP.SurfaceParameters(ioo);
  Standard_Real parOnEi;
  Standard_Boolean ok = VP.ParonE(Ei,parOnEi);
  if (!ok) {
    TopoDS_Vertex voo;
    if (visvon12) voo = vi;
    else          voo = ve; // is of rank <is> or <ioo>!
    ok = FUN_tool_parVonE(voo,Ei,parOnEi);
  }
  if (!ok) return s_NOTdgE;
  
  // Getting caracteristic vectors describing our system (a,x,y). 
  // The system is in the plane normal to ngFe :
  // * <a> is the direction of the tangent vector tgEc, going OUT 
  // the sphere while following <Ec>
  // * <x> is normal to ngFi.   
  //   <x> is oriented INSIDE <Fi> (the matter limited by <Fi>'s boundaries)
  // * (<x>,<y>,<z>) describe a RONd such that
  // the geometry of the degenerated edge <Ed> follows (<x>,<y>,<z>),
  // <y> is oriented OUTSIDE the matter limited by the oriented face <Fi>.
  // (<Fi> is a plane : <z> is parallel to ngFe)

  gp_Vec ngFe,ngFi,x,a;
  FUN_GetVectors(Fe,Fi,Ec,p2di,p2de,ve,ngFe,ngFi,a,x);

  // xpu : 24-10-97
  Standard_Boolean nullx =  FUN_nullx(x);
  if (nullx) {
    Standard_Boolean mTOOEi = Standard_False;
    Standard_Boolean getOOEi = FUN_OOEi(p2di,Fi,OOEi,paronOOEi);
    if (getOOEi && !OOEi.IsSame(Ei)) mTOOEi = FUN_IEcOOEi(ve,paronOOEi,OOEi,Ec,TOOEi);

    if (!mTOOEi) return s_NOTdgE;
    else         return s_TOOEi;
  }
  // xpu : 24-10-97

  gp_Vec xx = FUN_tool_getgeomxx(Fi,Ei,parOnEi,ngFi);
  TopAbs_Orientation oriEi; ok = FUN_tool_orientEinFFORWARD(Ei,Fi,oriEi);
  if (!ok) return s_NOTdgE; // NYI : Raise Error
  if (oriEi == TopAbs_REVERSED) xx.Reverse();
  
  if (x.Dot(xx) < 0.) x.Reverse();

  // when the edge <Ei> is tangent to <Fe> at vertex <v>,
  // the degenerated edge is not splitted.
  Standard_Boolean EitangenttoFe = FUN_tool_EitangenttoFe(ngFe,Ei,parOnEi);
  if (EitangenttoFe) return s_NOTdgE;

  gp_Vec y;
  if (Fi.Orientation() == TopAbs_FORWARD) y = ngFi;
  else                                    y = ngFi.Reversed();
  gp_Vec z(x^y);

#ifdef OCCT_DEBUG
  Standard_Boolean trc = Standard_False;
  if (trc) {
    gp_Pnt p = BRep_Tool::Pnt(ve);
#ifdef DRAW
    TCollection_AsciiString aax("x");     FUN_brep_draw(aax,p,x);
    TCollection_AsciiString aay("y");     FUN_brep_draw(aay,p,y);
    TCollection_AsciiString aang("ngFi"); FUN_brep_draw(aang,p,ngFi);
    TCollection_AsciiString aaa("a");     FUN_brep_draw(aaa,p,a);
#endif
  }
#endif
  
  // nrotation  = axis describing the sphere's parametrization 
  Standard_Integer orivine = FUN_tool_orientVinE(ve,Ec);
  gp_Vec nrotation;
  if (orivine == LAST) nrotation = ngFe;
  else                 nrotation = ngFe.Reversed();
  Standard_Boolean complement = (z.Dot(nrotation) < 0.);
  paronEd = FUN_getpar(gp_Dir(a),gp_Dir(x),gp_Dir(y),complement);

  // T :
  // in referential (x,y), following trigonometric sense, while 
  // crossing axis x (describing Fi), the transition is IN/OUT
  // (y = ntFi). 
  // if parametrization follows the trigonometric sense: transition IN/OUT
  // else, it is OUT/IN.
  
  Standard_Boolean inout = !complement;
  if (inout) T.Set(TopAbs_IN,TopAbs_OUT);
  else T.Set(TopAbs_OUT,TopAbs_IN);
  return s_TdgE;
}

static Standard_Boolean FUN_0or2PI(Standard_Real& paronEd, const Standard_Boolean& inout)
{
  Standard_Real tol = Precision::Parametric(Precision::Confusion());
  Standard_Boolean extre = (Abs(paronEd) < tol);
  extre = extre && (Abs(2.*M_PI-paronEd) < tol);
  if (!extre) return Standard_False;
  paronEd = (inout) ? 2.*M_PI : 0.;
  return Standard_True;
}

// case VP is IN <Fi>
static Standard_Integer FUN_parondgEINFi(const TopOpeBRep_VPointInter& VP,
			    const TopoDS_Face& Fe,
			    const TopoDS_Face& Fi,
			    const TopoDS_Edge& Ed, 
			    const TopoDS_Edge& Ec,
			    Standard_Real& par1OnEd,
			    Standard_Real& par2OnEd,
			    TopOpeBRepDS_Transition& T1, 
			    TopOpeBRepDS_Transition& T2,                               // !s_NOTdgE  
			    TopoDS_Edge& OOEi, Standard_Real& paronOOEi, TopOpeBRepDS_Transition& TOOEi) // s_TdgEandTOOEi 
{
  Standard_Integer is = VP.ShapeIndex();
  Standard_Integer ioo = (is == 1) ? 2 : 1;
  Standard_Boolean iis1 = (is == 1) ? Standard_True : Standard_False;

  // VP is on the restriction of shape <i>
  // and IN the face <Fi>.
  Standard_Boolean isVon1 = VP.IsVertexOnS1();
  Standard_Boolean isVon2 = VP.IsVertexOnS2();
  if (iis1 && !isVon1)  return s_NOTdgE; 
  if (!iis1 && !isVon2) return s_NOTdgE;
  TopoDS_Vertex v;
  if (iis1) v = TopoDS::Vertex(VP.VertexOnS1());
  else      v = TopoDS::Vertex(VP.VertexOnS2());

  // p2di and p2de
  gp_Pnt2d p2de = VP.SurfaceParameters(is);
  gp_Pnt2d p2di = VP.SurfaceParameters(ioo);
  
  gp_Vec ngFe,ngFi,x,a;
  FUN_GetVectors(Fe,Fi,Ec,p2di,p2de,v,ngFe,ngFi,a,x);  

  // xpu : 24-10-97
  Standard_Boolean nullx =  FUN_nullx(x);
  if (nullx) {
    Standard_Boolean mTOOEi = Standard_False;
    Standard_Boolean getOOEi = FUN_OOEi(p2di,Fi,OOEi,paronOOEi);
    if (getOOEi) mTOOEi = FUN_IEcOOEi(v,paronOOEi,OOEi,Ec,TOOEi);

    if (!mTOOEi) return s_NOTdgE;
    else         return s_TOOEi;
  }
  // xpu : 24-10-97

  gp_Vec y;
  if (Fi.Orientation() == TopAbs_FORWARD) y = ngFi;
  else                                    y = ngFi.Reversed();
  gp_Vec z(x^y);

#ifdef OCCT_DEBUG
  Standard_Boolean trc = Standard_False;
  if (trc) {
    gp_Pnt p = BRep_Tool::Pnt(v);
#ifdef DRAW
    TCollection_AsciiString aax("x");     FUN_brep_draw(aax,p,x);
    TCollection_AsciiString aay("y");     FUN_brep_draw(aay,p,y);
    TCollection_AsciiString aang("ngFi"); FUN_brep_draw(aang,p,ngFi);
    TCollection_AsciiString aaa("a");     FUN_brep_draw(aaa,p,a);
#endif
  }
#endif

  // parameters on <Ed>
  Standard_Integer orivine = FUN_tool_orientVinE(v,Ec);
  gp_Vec nrotation;
  if (orivine == LAST) nrotation = ngFe;
  else                 nrotation = ngFe.Reversed();
  Standard_Boolean complement = (z.Dot(nrotation) < 0.);

  Standard_Boolean T1inout = !complement;
  if (T1inout) T1.Set(TopAbs_IN,TopAbs_OUT);
  else T1.Set(TopAbs_OUT,TopAbs_IN); 
  T2 = T1.Complement();

  Standard_Real par = FUN_getpar(a,x,y,complement);
  par1OnEd = par;
  par2OnEd = (par > M_PI) ? par-M_PI : par+M_PI;

  // kpart Ec on Fi :
  Standard_Boolean changedpar1 = FUN_0or2PI(par1OnEd,T1inout);
  Standard_Boolean changedpar2 = FUN_0or2PI(par2OnEd,!T1inout);
  
  return s_TdgE;
}

static Standard_Boolean FUN_PInDegenEd(const Standard_Real& paronEd, const TopoDS_Edge& Ec)
{
  TopoDS_Vertex vf,vl;
  TopExp::Vertices(Ec,vf,vl);
  Standard_Real pf = BRep_Tool::Parameter(vf,Ec);
  Standard_Real pl = BRep_Tool::Parameter(vl,Ec);
  Standard_Real tol = Precision::Parametric(Precision::Confusion());

  Standard_Boolean kept = (paronEd < pl-tol) ||  (paronEd > pl+tol);
  kept = kept || (paronEd < pf-tol) ||  (paronEd > pf+tol);
  return kept;
}

static Standard_Integer FUN_putInterfonDegenEd
(const TopOpeBRep_VPointInter& VP,
 const TopoDS_Face& F1,
 const TopoDS_Face& F2,
 TopTools_DataMapOfShapeListOfShape& DataforDegenEd, // const but for copy &
 const Handle(TopOpeBRepDS_HDataStructure) HDS,
 Standard_Integer& is,
 Standard_Integer& makeinterf, // 1,2,3 : compute interf1, or2 or the 2 interfs
 TopOpeBRepDS_Transition& Trans1, Standard_Real& param1, 
 TopOpeBRepDS_Transition& Trans2, Standard_Real& param2,
 TopoDS_Edge& OOEi, Standard_Real& paronOOEi, TopOpeBRepDS_Transition& TOOEi)
{
  // IMPORTANT : NYI : xpu :
  // -----------------------
  // The following process is valid when ngFe is normal to ngFi.
  // It may be unsufficient elsewhere.

  // (kpart : sphere/box, with one sphere's degenerated edge lying on one boxe's 
  //  face, IN or ON the face)
  // 1_ if on2edges :
  //   Ed append EVI of transition(FACE Fi) on G=(VERTEX,V), S=(EDGE,Ei) par = paronEd.
  // 2_ else :
  //   Ed append EVI of transition(FACE Fi) on G=(VERTEX,V), S=(FACE,Fi) par = paronEd1  
  //   Ed append EVI of transition(FACE Fi) on G=(VERTEX,V), S=(FACE,Fi) par = paronEd2  
  // with Ed the degenerated edge, Ei of Fi interfers with Ed at vertex V.
  
#ifdef OCCT_DEBUG
  Standard_Boolean traceDSF = TopOpeBRepDS_GettraceDSF();
  Standard_Boolean traceDEGEN = TopOpeBRepDS_GettraceDEGEN();
  Standard_Boolean trace = traceDSF || traceDEGEN;
#endif
 
  TopOpeBRepDS_DataStructure& DS = HDS->ChangeDS();
  Standard_Boolean isvon1 = VP.IsVertexOnS1();
  Standard_Boolean isvon2 = VP.IsVertexOnS2();
  Standard_Boolean isvon12 = isvon1 && isvon2;
  if (!isvon1 && !isvon2) return s_NOTdgE;
  Standard_Boolean ison1 = VP.IsOnDomS1();
  Standard_Boolean ison2 = VP.IsOnDomS2();

  TopoDS_Vertex v1, v2;
  if (isvon1) v1 = TopoDS::Vertex(VP.VertexOnS1());
  if (isvon2) v2 = TopoDS::Vertex(VP.VertexOnS2());

#ifdef DRAW
  Standard_Boolean trcd = Standard_False;
  if (trcd && isvon1) {TCollection_AsciiString aa("v1"); FUN_brep_draw(aa,v1);}
  if (trcd && isvon2) {TCollection_AsciiString aa("v2"); FUN_brep_draw(aa,v2);}
  if (trcd) FUN_DrawMap(DataforDegenEd);
#endif

  // VP is lying on at least one vertex vi
  // !!! : even if isvon1 && isvon2, v1 and v2 are NOT SAME !
  Standard_Boolean visvon12 = isvon1 ? HDS->HasSameDomain(v1) : HDS->HasSameDomain(v2);
  if (visvon12 && !isvon12) {
    TopoDS_Shape oov;
    if(isvon1) { 
      Standard_Boolean ok = FUN_ds_getoov(v1,HDS,oov);
      if (!ok) return s_NOTdgE;
      v2 = TopoDS::Vertex(oov); 
    }
    else { 
      Standard_Boolean ok = FUN_ds_getoov(v2,HDS,oov); 
      if (!ok) return s_NOTdgE;
      v1 = TopoDS::Vertex(oov);
    }
    // now, if visvon12, v1 and v2 are defined
  }

  TopoDS_Vertex v;
  Standard_Boolean hasdegened;
  if (isvon1) v = v1;
  else        v = v2;
  hasdegened = DataforDegenEd.IsBound(v);  
  if (!hasdegened && visvon12) {
    // DataforDegenEd can either bind v1 or v2.
    if (isvon1) hasdegened = DataforDegenEd.IsBound(v2);
    else        hasdegened = DataforDegenEd.IsBound(v1);
    if (hasdegened) {
      if (isvon1) v = v2;
      else        v = v1;
    }
  }
  if (!hasdegened) return s_NOTdgE;
  
  Standard_Boolean on2edges = (VP.ShapeIndex() == 3);
  on2edges = on2edges || visvon12;

  TopTools_ListIteratorOfListOfShape itoflos(DataforDegenEd.Find(v));
  if (!itoflos.More()) return s_NOTdgE;
  TopoDS_Edge Ec = TopoDS::Edge(itoflos.Value());
  itoflos.Next(); if (!itoflos.More()) return s_NOTdgE;
  TopoDS_Edge Ed = TopoDS::Edge(itoflos.Value());
    
  // -----------------------------------------
  // Fe, Ec, Ed and v are on shape <is>,
  // Fi, Ei are on shape <ioo> = (is == 1) ? 2 :1
  // -----------------------------------------
  TopoDS_Edge e1,e2;
  Standard_Boolean e1isEd = Standard_False, e2isEd = Standard_False;
  if (ison1) { e1 = TopoDS::Edge(VP.ArcOnS1()); e1isEd = BRep_Tool::Degenerated(e1); }
  if (ison2) { e2 = TopoDS::Edge(VP.ArcOnS2()); e2isEd = BRep_Tool::Degenerated(e2); } 

  if (!e1isEd && !e2isEd) return s_NOTdgE; // computes transitions only ONCE
  is = (e1isEd) ? 1 : 2;
  Standard_Integer ioo = (is == 1) ? 2 : 1;
  TopoDS_Face Fe;
  if (is == 1) Fe = F1;
  else         Fe = F2;
  TopoDS_Face Fi;
  if (ioo == 1) Fi = F1;
  else          Fi = F2;
  Standard_Integer iv = 0;
  
#ifdef OCCT_DEBUG
  if (FUN_debnull(Fe)) cout<<"Fe is null"<<endl;
  if (FUN_debnull(Fi)) cout<<"Fi is null"<<endl;
  if (FUN_debnull(Ec)) cout<<"Ec is null"<<endl;
  if (FUN_debnull(Ed)) cout<<"Ed is null"<<endl;
  if (trace) {
    TopAbs_Orientation Edori = Ed.Orientation();
  }
  Standard_Boolean trace3d = Standard_False;
#ifdef DRAW
  if (trace3d) {TCollection_AsciiString afe("Fe");TCollection_AsciiString afi("Fi");TCollection_AsciiString aec("Ec");
		FUN_brep_draw(afe,Fe); FUN_brep_draw(afi,Fi); FUN_brep_draw(aec,Ec);}
#endif
#endif

  Standard_Integer makeI = s_NOTdgE;
  // case 1 :
  // -------
  if (on2edges) {
  
    TopTools_ListOfShape loEi;
    TopoDS_Vertex vi,ve; // vertex on shape ioo sharing same domain with v
                         // dummy if !visvon12
    if (visvon12) { 
      // if isvon12, we have two choices for <Ei>,
      // we choose the one for which FUN_parondgEONFi returns True.
      // (recall that prequesitory : ngFi is normal to ngFe)  
      if (is == 1) ve = v1;
      else         ve = v2; 
      if (ioo == 1) vi = v1;
      else          vi = v2;      
      FUN_getEofFwithV(vi,Fi,loEi);
    }
    else {
      if (is == 2) loEi.Append (e1);
      else loEi.Append (e2);
    }
				    
    Standard_Real paronEd;
    TopOpeBRepDS_Transition T; 
    TopTools_ListIteratorOfListOfShape itloei(loEi);
    
    TopoDS_Edge tmpOOEi; Standard_Real tmpparonOOEi; TopOpeBRepDS_Transition tmpTOOEi;
    for (;itloei.More(); itloei.Next()) {

#ifdef OCCT_DEBUG
      if (FUN_debnull(itloei.Value())) cout<<"Ei is null"<<endl;
#ifdef DRAW
      if (trace3d) {TCollection_AsciiString aa("ecur");FUN_brep_draw(aa,itloei.Value());}
#endif
      TopAbs_ShapeEnum typ = itloei.Value().ShapeType();
      if (typ != TopAbs_EDGE) cout<<"shape not edge"<<endl;
#endif

      TopoDS_Edge Ei = TopoDS::Edge(itloei.Value());
      if (visvon12) v = ve;
      makeI = FUN_parondgEONFi (VP,visvon12,v,vi,
				is,Fe,Fi,Ed,Ei,Ec,paronEd,T,
				tmpOOEi,tmpparonOOEi,tmpTOOEi); 
      if (makeI == s_NOTdgE) continue;
      if (makeI == s_TOOEi)  {
	tmpOOEi = OOEi; tmpparonOOEi = paronOOEi; tmpTOOEi = TOOEi;
	continue;
      }

      Standard_Boolean keepI = FUN_PInDegenEd(paronEd,Ed);
      if (keepI) {
	makeinterf = 1;
	param1 = paronEd;
	Trans1.Set(T.Before(), T.After());
      }
    } // itloei
    
  }
  // case 2 :
  // -------
  else {
    Standard_Real paronEd1, paronEd2;
    TopOpeBRepDS_Transition T1, T2;

    makeI = FUN_parondgEINFi (VP,Fe,Fi,Ed,Ec,paronEd1,paronEd2,T1,T2,
			      OOEi,paronOOEi,TOOEi);
    if (makeI == s_NOTdgE) return makeI;
    if (makeI == s_TOOEi)  return makeI;

    Standard_Boolean keepI1 = FUN_PInDegenEd(paronEd1,Ed);
    Standard_Boolean keepI2 = FUN_PInDegenEd(paronEd2,Ed);

    if (keepI1) {
      makeinterf = 1;
      param1 = paronEd1;
      Trans1.Set(T1.Before(), T1.After());
    }
    if (keepI2) {
      makeinterf = (makeinterf == 1) ? 3 : 2;
      param2 = paronEd2;
      Trans2.Set(T2.Before(), T2.After()); 
    }      
  }  
  return makeI;
}


static  Standard_Boolean FUN_getEc
(const TopOpeBRep_VPointInter& VP,
 TopTools_DataMapOfShapeListOfShape& DataforDegenEd, 
 const Handle(TopOpeBRepDS_HDataStructure)& HDS, TopoDS_Shape& Ec)
{
  TopoDS_Vertex v;
  Standard_Boolean isvon1 = VP.IsVertexOnS1();
  Standard_Boolean isvon2 = VP.IsVertexOnS2();
  if (!isvon1 && !isvon2) return Standard_False;
  if (isvon1) v = TopoDS::Vertex(VP.VertexOnS1());
  if (isvon2) v = TopoDS::Vertex(VP.VertexOnS2());
  Standard_Boolean inmap = DataforDegenEd.IsBound(v);
  if  (!inmap) {
    if (HDS->HasSameDomain(v)) 
      {Standard_Boolean ok = FUN_ds_getoov(v,HDS,v); if (!ok) return Standard_False;}
    inmap = DataforDegenEd.IsBound(v);
    if (!inmap) return s_NOTdgE;
  }   
  const TopTools_ListOfShape& los = DataforDegenEd.Find(v);
  if (los.Extent() < 2) return Standard_False;
  Ec = los.First();    
  return Standard_True;
}
*/
Standard_EXPORT void FUN_FillVof12
(const TopOpeBRep_LineInter& L,
 TopOpeBRepDS_PDataStructure pDS)
{
  TopOpeBRep_VPointInterIterator itvp(L);
  for (; itvp.More(); itvp.Next()) {
    const TopOpeBRep_VPointInter& vp = itvp.CurrentVP();
    Standard_Integer sind = vp.ShapeIndex();
    if (sind != 3) continue;
    Standard_Boolean isvon1 = vp.IsVertexOnS1();
    Standard_Boolean isvon2 = vp.IsVertexOnS2();
    Standard_Boolean isvon12 = isvon1 && isvon2;
    if (!isvon12) continue;
    const TopoDS_Shape& v1 = vp.VertexOnS1();
    const TopoDS_Shape& v2 = vp.VertexOnS2();
    pDS->FillShapesSameDomain(v1,v2);
  }      
}

static void FUN_addmapve(TopTools_DataMapOfShapeListOfShape& mapve, const TopoDS_Shape& v, const TopoDS_Shape& e)
{
  Standard_Boolean visb = mapve.IsBound(v);
  Standard_Boolean eisb = mapve.IsBound(e);
  if      (!visb && !eisb) 
    {TopTools_ListOfShape le;le.Append(e);mapve.Bind(v,le); 
     TopTools_ListOfShape lv;lv.Append(v);mapve.Bind(e,lv);}
  else if (visb  && !eisb) 
    {mapve.ChangeFind(v).Append(e); 
     TopTools_ListOfShape lv;lv.Append(v);mapve.Bind(e,lv);}
  else if (!visb &&  eisb) 
    {mapve.ChangeFind(e).Append(v); 
     TopTools_ListOfShape le;le.Append(e);mapve.Bind(v,le);}
  else {
    Standard_Boolean found = Standard_False;
    TopTools_ListIteratorOfListOfShape it(mapve.Find(v));
    for (; it.More(); it.Next())
      if (it.Value().IsSame(e)) {found = Standard_True; break;}
    if (!found) {mapve.ChangeFind(v).Append(e); mapve.ChangeFind(e).Append(v);} 	    
  }
}

Standard_EXPORT void FUN_GetdgData(TopOpeBRepDS_PDataStructure& pDS,const TopOpeBRep_LineInter& L,
				   const TopoDS_Face& F1,const TopoDS_Face& F2, TopTools_DataMapOfShapeListOfShape& datamap)
{
  // purpose : fills up map datamap = {(v, (closinge,degeneratede))}
  //           with shapes with same rank

  TopTools_DataMapOfShapeInteger shaperk;          // rkshape = {shape,rank=1,2}
  TopTools_DataMapOfShapeListOfShape mapvec, mapved; // mapvec = {(v,lec),(ec,lv)}, mapved = {(v,led),(ed,lv)}
  TopTools_DataMapOfShapeShape mapvvsd;          // mapvvsd = {(v,v)}

  TopOpeBRep_VPointInterIterator itvp(L);
  for (; itvp.More(); itvp.Next()) {
    const TopOpeBRep_VPointInter& vp = itvp.CurrentVP();
    Standard_Boolean isv1 = vp.IsVertex(1), isv2 = vp.IsVertex(2);
    Standard_Boolean isv = isv1 || isv2;
    if (!isv) continue;

    Standard_Integer sind = vp.ShapeIndex();        
    TopoDS_Shape v = isv1 ? vp.Vertex(1): vp.Vertex(2);
    for (Standard_Integer i = 1; i <= 2; i++) {
      TopoDS_Face f = (i == 1)? F1: F2;

      Standard_Boolean isvi = vp.IsVertex(i);
      if (isvi) {
	v = vp.Vertex(i);
	shaperk.Bind(v,i);	
      }

      TopoDS_Edge e; Standard_Boolean isdg, iscl; isdg = iscl = Standard_False;
      Standard_Boolean ison = (sind == i)||(sind == 3);
      if (ison) {
	e = TopoDS::Edge( vp.Edge(i) );
	shaperk.Bind(e,i);	

	isdg = BRep_Tool::Degenerated(e);    
	if (!isdg) iscl = TopOpeBRepTool_ShapeTool::Closed(e,f);
	if (isdg) FUN_addmapve(mapved,v,e);
	if (iscl) FUN_addmapve(mapvec,v,e);
      }//ison      
    }//i = 1..2
  }//itvp   

  // filling up map mapvvsd
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itm(mapved);
  for (; itm.More(); itm.Next()){
    const TopoDS_Shape& v = itm.Key();
    if (v.ShapeType() != TopAbs_VERTEX) continue;
    Standard_Integer rkv = shaperk.Find(v);

    TopTools_ListIteratorOfListOfShape ite(itm.Value());
    for (; ite.More(); ite.Next()){
      const TopoDS_Edge& e = TopoDS::Edge( ite.Value() );
      Standard_Integer rke = shaperk.Find(e);
      if (rke != rkv) {
	TopExp_Explorer ex(e, TopAbs_VERTEX);
	const TopoDS_Shape& vsd = ex.Current(); 
	// recall : if vsd is not bound in shaperk,
	//          it is not bound in <L> either
	mapvvsd.Bind(v,vsd); mapvvsd.Bind(vsd,v);
      }      
    }
  }//itm(mapved)
  
  itm.Initialize(mapved);
  for (; itm.More(); itm.Next()){
    const TopoDS_Shape& dge = itm.Key();
    Standard_Integer rk = shaperk.Find(dge);
    TopoDS_Face f = (rk == 1)? F1: F2;
    if (dge.ShapeType() != TopAbs_EDGE) continue;

    TopExp_Explorer ex(dge, TopAbs_VERTEX);
    const TopoDS_Vertex& v = TopoDS::Vertex( ex.Current() ); 
    Standard_Integer rkv = shaperk.Find(v);
    Standard_Boolean hassd = mapvvsd.IsBound(v);
    TopoDS_Vertex vsd; if (hassd) vsd = TopoDS::Vertex( mapvvsd.Find(v) );
    
    Standard_Boolean hasecl = Standard_False; TopoDS_Shape cle;
    Standard_Boolean isbv = mapvec.IsBound(v), isbvsd= hassd ? mapvec.IsBound(vsd) : Standard_False;
    if (!isbv && !isbvsd) {
      // **************************************************
      // interference with closing edge is not found,
      // adding new information to the ds
      // **************************************************
      TopTools_IndexedDataMapOfShapeListOfShape mapve; TopExp::MapShapesAndAncestors(f,TopAbs_VERTEX,TopAbs_EDGE,mapve);
      TopTools_ListIteratorOfListOfShape iteds(mapve.FindFromKey(v));
      for (; iteds.More(); iteds.Next()){
	const TopoDS_Edge& ee = TopoDS::Edge(iteds.Value());
	if (ee.IsSame(dge)) continue;
	Standard_Boolean iscl = TopOpeBRepTool_ShapeTool::Closed(ee,f);
	if (!iscl) continue;
	isbv = Standard_True; cle = ee; hasecl = Standard_True; break;
      }
    }
    if (!hasecl && (isbv || isbvsd)) {
      TopoDS_Vertex vv = isbv ? v : vsd;
      TopTools_ListIteratorOfListOfShape ite;
      if (isbv) ite.Initialize(mapvec.Find(v));
      for (; ite.More(); ite.Next()){
	const TopoDS_Shape& e = ite.Value();
	Standard_Integer rke = shaperk.Find(e);
	if (rke == rk) {cle = e; hasecl = Standard_True; break;}
      }
    }
    if (!hasecl) continue;

    TopoDS_Vertex vv = (rkv == rk) ? v : vsd;
    TopTools_ListOfShape ls; ls.Append(cle); ls.Append(dge); 
    datamap.Bind(vv,ls);
  }//itm(mapved)

  // filling sdm shapes
  TopTools_DataMapIteratorOfDataMapOfShapeShape ittm(mapvvsd);
  for (; ittm.More(); ittm.Next()){
    const TopoDS_Vertex& v  = TopoDS::Vertex( ittm.Value() );
    const TopoDS_Vertex& ov = TopoDS::Vertex( mapvvsd.Find(v) );
    Standard_Integer rkv = shaperk.Find(v);
    TopoDS_Vertex v1 = (rkv == 1) ? v : ov;
    TopoDS_Vertex v2 = (rkv == 2) ? v : ov;
    pDS->FillShapesSameDomain(v1,v2);
  }
}//FUN_GetdgData

#define NOI      (0)
#define MKI1     (1)
#define MKI2     (2)
#define MKI12    (3)

static Standard_Integer FUN_putInterfonDegenEd
(const TopOpeBRep_VPointInter& VP,
 const TopoDS_Face& F1, const TopoDS_Face& F2,
 TopTools_DataMapOfShapeListOfShape& DataforDegenEd, // const but for copy &
 Handle(TopOpeBRepDS_HDataStructure)& HDS,
 Standard_Integer& is, TopoDS_Edge& dgE,
// Standard_Integer& makeinterf, // 1,2,3 : compute interf1, or2 or the 2 interfs
 Standard_Integer& , // 1,2,3 : compute interf1, or2 or the 2 interfs
 TopOpeBRepDS_Transition& Trans1, Standard_Real& param1, 
 TopOpeBRepDS_Transition& Trans2, Standard_Real& param2,
 TopoDS_Edge& OOEi, Standard_Real& paronOOEi, Standard_Boolean hasOOEi,
 Standard_Boolean& isT2d)
{  
  OOEi.Nullify();

  Standard_Boolean on3 = (VP.ShapeIndex() == 3);// <VP> is shared by edge of 1 and edge of 2.
  Standard_Boolean onv12 = VP.IsVertexOnS1() && VP.IsVertexOnS2();

  const TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  TopoDS_Vertex v; Standard_Integer rkv = 0;
//  Standard_Integer iv;
  TopoDS_Vertex ov;
  for (Standard_Integer ShapeIndex = 1; ShapeIndex <= 2; ShapeIndex++) {
    Standard_Boolean isv = (ShapeIndex == 1) ? (VP.IsVertexOnS1()) : (VP.IsVertexOnS2());
    if (!isv) continue;
    v = (ShapeIndex == 1) ? TopoDS::Vertex(VP.VertexOnS1()) : TopoDS::Vertex(VP.VertexOnS2());
    Standard_Boolean hasdegened = DataforDegenEd.IsBound(v);  
    if (!hasdegened) continue;
    rkv = ShapeIndex; break;
  }//ShapeIndex = 1..2
  if (rkv == 0) return NOI;// compute interference once only.
  Standard_Boolean isvsd = HDS->HasSameDomain(v);
    
  // edges dge, cle on shape<rkdg>
  const TopTools_ListOfShape& loe = DataforDegenEd.Find(v);
  const TopoDS_Edge& cle = TopoDS::Edge(loe.First());
  const TopoDS_Edge& dge = TopoDS::Edge(loe.Last()); dgE = dge;
  Standard_Integer rkdg = 0;
  if (BDS.HasShape(dge)) rkdg = BDS.AncestorRank(dge); 
  else {
    Standard_Boolean vindge = FUN_tool_inS(v,dge);
    if (vindge) rkdg = rkv;
    else        rkdg = (rkv == 1) ? 2 : 1;
  }
  is = rkdg;
  Standard_Integer rki = (rkdg == 1) ? 2 : 1;

  gp_Pnt2d uvi; TopoDS_Face fi,f; 
  { 
    //     Standard_Real u,v; 
    //     if (rki == 1) VP.ParametersOnS1(u,v); 
    //     else          VP.ParametersOnS2(u,v);
    //     uvi = gp_Pnt2d(u,v);
    // modified by NIZHNY-MKK  Tue Nov 21 17:44:56 2000.BEGIN
    Standard_Real upar, vpar; 
    if (rki == 1) VP.ParametersOnS1(upar, vpar);
    else          VP.ParametersOnS2(upar, vpar);
    uvi = gp_Pnt2d(upar, vpar);
    // modified by NIZHNY-MKK  Tue Nov 21 17:44:59 2000.END
    fi = (rki == 1)  ? F1 : F2;
    f  = (rkdg == 1) ? F1 : F2;
  }
  TopOpeBRepTool_mkTondgE mktdg;
  Standard_Boolean ok = mktdg.Initialize(dge, f, uvi, fi);
  if (!ok) return NOI;
  ok = mktdg.SetclE(cle);
  if (!ok) return NOI;

  if (onv12 || isvsd) {
    if (onv12)
      ov = (rkv == 2) ? TopoDS::Vertex(VP.VertexOnS1()) : TopoDS::Vertex(VP.VertexOnS2());
    else     {
      // modified by NIZHNY-MKK  Tue Nov 21 17:45:46 2000.BEGIN
      //       Standard_Boolean ok = FUN_ds_getoov(v,HDS,ov); 
      //       if (!ok) return Standard_False;
      Standard_Boolean found = FUN_ds_getoov(v,HDS,ov); 
      if (!found) return NOI;
      // modified by NIZHNY-MKK  Tue Nov 21 17:45:50 2000.END
    }
    if (rkv != rkdg) {TopoDS_Vertex tmp = v; v = ov; ov = tmp; rkv = rkdg;} // ensure v is vertex of dge
  }

  Standard_Integer mkt = 0; Standard_Real par1 = 0.,par2 = 0.;
  if (on3) {
    TopoDS_Edge ei = (rki == 1) ? TopoDS::Edge(VP.ArcOnS1()) : TopoDS::Edge(VP.ArcOnS2());
    Standard_Real pari = (rki == 1) ? VP.ParameterOnArc1() : VP.ParameterOnArc2();
    // if okrest, ei interfers in the compute of transitions for dge
    mktdg.SetRest(pari,ei);
    ok = mktdg.MkTonE(ei,mkt, par1,par2);  
    if ((!ok) || (mkt == NOI)) return NOI;      
    OOEi = ei; paronOOEi = pari; hasOOEi = Standard_True;
  }//on3
  else     {
    // modified by NIZHNY-MKK  Tue Nov 21 17:31:14 2000.BEGIN
    // This search, compute and check the data which was not computed by intersector.
    if((rki == 1 && VP.IsOnDomS1()) || (rki == 2 && VP.IsOnDomS2())) {
      TopoDS_Edge ei = (rki == 1) ? TopoDS::Edge(VP.ArcOnS1()) : TopoDS::Edge(VP.ArcOnS2());
      Standard_Real pari = (rki == 1) ? VP.ParameterOnArc1() : VP.ParameterOnArc2();
      mktdg.SetRest(pari,ei);
      ok = mktdg.MkTonE(ei,mkt, par1,par2);
      if(ok && mkt!=NOI) {
	OOEi = ei; paronOOEi = pari; hasOOEi = Standard_True;
      }
    }
    else {
      Standard_Boolean edgefound = Standard_False;
      TopoDS_Face aFace = (rki == 1) ? F1 : F2;
      TopTools_IndexedDataMapOfShapeListOfShape aMapOfVertexEdges;
      TopExp::MapShapesAndAncestors(aFace, TopAbs_VERTEX, TopAbs_EDGE, aMapOfVertexEdges);
      TopoDS_Vertex aVertex;
      Standard_Boolean vertexfound = local_FindVertex(VP, aMapOfVertexEdges, aVertex);
      
      if(vertexfound && !aVertex.IsNull()) {
	TopTools_ListIteratorOfListOfShape anIt(aMapOfVertexEdges.FindFromKey(aVertex));
	for(; !edgefound && anIt.More(); anIt.Next()) {	  
	  const TopoDS_Edge& ei = TopoDS::Edge(anIt.Value());
	  Standard_Real pari = BRep_Tool::Parameter(aVertex, ei);
	  if(!BRep_Tool::Degenerated(ei)) {
	    edgefound = !local_FindTreatedEdgeOnVertex(ei, aVertex);
	  }
	  if(edgefound) {
	    mktdg.SetRest(pari,ei);
	    ok = mktdg.MkTonE(ei,mkt, par1,par2);
	    if(ok && mkt!=NOI) {
	      OOEi = ei; paronOOEi = pari; hasOOEi = Standard_True;
	    }
	    if(!aMapOfTreatedVertexListOfEdge.IsBound(aVertex)) {
              TopTools_ListOfShape thelist;
	      aMapOfTreatedVertexListOfEdge.Bind(aVertex, thelist);
	    }
	    aMapOfTreatedVertexListOfEdge(aVertex).Append(ei);
	  }
	}
      }
      if(!edgefound) {
	ok = mktdg.MkTonE(mkt, par1,par2);
      }
    }
    // modified by NIZHNY-MKK  Tue Nov 21 17:31:36 2000.END
    if ((!ok) || (mkt == NOI)) return NOI;
  }
  isT2d = mktdg.IsT2d();

  if ((mkt == MKI1)||(mkt == MKI12)) {Trans1.Set(TopAbs_FORWARD); param1 = par1;}
  if ((mkt == MKI2)||(mkt == MKI12)) {Trans2.Set(TopAbs_REVERSED); param2 = par2;}
  return mkt;
//  **********   iterate on restrictions of fi  **********
//  TopTools_ListOfShape lei; mktdg.GetAllRest(lei);
//  TopTools_ListIteratorOfListOfShape ite(lei);
//  for (; ite.More(); ite.Next()){
//    Standard_Boolean oki = mktdg.MkTonE(ei,mkt,par1,par2);
//    ... NYI
//  }
}//FUN_putInterfonDegenEd

//=======================================================================
//function : ProcessVPondgE
//purpose  : SUPPLYING INTPATCH when a degenerated edge is touched.
//=======================================================================

#define s_NOIdgE       (0) // do NOT compute any interference
#define s_IdgE         (1) // compute interference(s) on dgE
#define s_IOOEi        (2) // compute interference(s) on OOEi

Standard_Boolean TopOpeBRep_FacesFiller::ProcessVPondgE
(const TopOpeBRep_VPointInter& VP,
 const Standard_Integer ShapeIndex, 
 TopOpeBRepDS_Kind& PVKind, Standard_Integer& PVIndex, // out
 Standard_Boolean& EPIfound, Handle(TopOpeBRepDS_Interference)& IEPI, // out 
 Standard_Boolean& CPIfound, Handle(TopOpeBRepDS_Interference)& ICPI) // out
{
#ifdef OCCT_DEBUG
  Standard_Boolean traceDSF = TopOpeBRepDS_GettraceDSF();
  Standard_Boolean traceDEGEN = TopOpeBRepDS_GettraceDEGEN();
#endif

  if (PVIndex == 0) FUN_VPIndex ((*this),(*myLine),VP,ShapeIndex,myHDS,myDSCIL, //in
				 PVKind,PVIndex, // out
				 EPIfound,IEPI,  // out
				 CPIfound,ICPI,  // out  
				 M_FINDVP);
 
  // kpart : sphere/box, with one sphere's degenerated edge lying on one boxe's 
  // face, IN or ON the face
  // if (mIdgEorOOEi), adds interferences on degenerated edge

  // If interferences should be added, finds out <VP>'s geometry
  // in existing interferences (see out parameters <EPIfound>..);
  // adds a new point/vertex to the DS if necessary.


  Standard_Boolean hasOOEi=Standard_False; TopoDS_Edge OOEi; Standard_Real parOOEi; 
  TopOpeBRepDS_Transition T1ondg, T2ondg; 
  Standard_Integer rankdg=0, Iiondg=0; 
  Standard_Real par1ondg=0., par2ondg=0.;
  Standard_Boolean hasdgdata = !myDataforDegenEd.IsEmpty();
  if (!hasdgdata) {
    return Standard_False; 
  } 

  // modified by NIZHNY-MKK  Tue Nov 21 17:35:29 2000
  local_ReduceMapOfTreatedVertices(myLine);

  Standard_Boolean isT2d = Standard_False; TopoDS_Edge dgEd;   
  Standard_Integer makeI = FUN_putInterfonDegenEd (VP,myF1,myF2,myDataforDegenEd,myHDS,
				      rankdg,dgEd, Iiondg,T1ondg,par1ondg,T2ondg,par2ondg,
				      OOEi,parOOEi,hasOOEi, isT2d);
  if (makeI == NOI) {
    return Standard_False; 
  }  

  // -------------------------------------------------------------------
  //             --- DS geometry Management --- 
  // -------------------------------------------------------------------
  
  if (PVIndex == 0) FUN_VPIndex ((*this),(*myLine),VP,ShapeIndex,myHDS,myDSCIL, //in
				 PVKind,PVIndex, // out
				 EPIfound,IEPI,  // out
				 CPIfound,ICPI,  // out
				 M_MKNEWVP);

  // -------------------------------------------------------------------
  //             --- EVI on degenerated edge ---
  //             ---       on OOEi           ---
  // -------------------------------------------------------------------    

  Standard_Integer rankFi = (rankdg == 1) ? 2 : 1;
//  TopoDS_Shape dgEd = VP.Edge(rankdg);
  TopoDS_Face Fi;
  if (rankFi == 1) Fi = myF1;
  else             Fi = myF2;
  Standard_Integer iFi = myDS->AddShape(Fi,rankFi);
#ifdef OCCT_DEBUG
  Standard_Integer iEd =
#endif
            myDS->AddShape(dgEd,rankdg);
  Standard_Integer iOOEi = 0;
  if (hasOOEi) iOOEi = myDS->AddShape(OOEi,rankFi);

#ifdef OCCT_DEBUG
  Standard_Boolean trace = traceDSF || traceDEGEN;
  if (trace) cout<<" VP is on degenerated edge "<<iEd<<" :"<<endl;
#endif
  Standard_Integer rkv = myDS->AncestorRank(myDS->Shape(PVIndex));

  if ((makeI == MKI1)||(makeI == MKI12)) {
    T1ondg.Index(iFi);
    Standard_Boolean isvertex1 = (rkv == 1);

    if (hasOOEi) {
      Handle(TopOpeBRepDS_Interference) EVI1i = ::MakeEPVInterference(T1ondg,iOOEi,PVIndex,par1ondg,
					  TopOpeBRepDS_VERTEX,TopOpeBRepDS_EDGE,isvertex1);
      myHDS->StoreInterference(EVI1i,dgEd);
    }
    if (!isT2d) {      
      Handle(TopOpeBRepDS_Interference) EVI1 = ::MakeEPVInterference(T1ondg,iFi,PVIndex,par1ondg,
					 TopOpeBRepDS_VERTEX,TopOpeBRepDS_FACE,isvertex1);
      myHDS->StoreInterference(EVI1,dgEd);
    }
  }
  if ((makeI == MKI2)||(makeI == MKI12)) {
    T2ondg.Index(iFi);
    Standard_Boolean isvertex2 = (rkv == 2);

    if (hasOOEi) {
      Handle(TopOpeBRepDS_Interference) EVI2i = ::MakeEPVInterference(T2ondg,iOOEi,PVIndex,par2ondg,
					  TopOpeBRepDS_VERTEX,TopOpeBRepDS_EDGE,isvertex2);
      myHDS->StoreInterference(EVI2i,dgEd);
    }
    if (!isT2d) {      
      Handle(TopOpeBRepDS_Interference) EVI2 = ::MakeEPVInterference(T2ondg,iFi,PVIndex,par2ondg,
					 TopOpeBRepDS_VERTEX,TopOpeBRepDS_FACE,isvertex2);
      myHDS->StoreInterference(EVI2,dgEd);
    }
  }

  return Standard_True;  
} // ProcessVPondgE


// modified by NIZHNY-MKK  Tue Nov 21 17:32:52 2000.BEGIN
static Standard_Boolean local_FindTreatedEdgeOnVertex(const TopoDS_Edge& theEdge,
						const TopoDS_Vertex& theVertex) {
  Standard_Boolean found = Standard_False;
  if(aMapOfTreatedVertexListOfEdge.IsBound(theVertex)) {
    TopTools_ListIteratorOfListOfShape anIt(aMapOfTreatedVertexListOfEdge(theVertex));
    for(; !found && anIt.More(); anIt.Next()) {
      if(theEdge.IsSame(anIt.Value())) {
	found = Standard_True;
      }
    }
  }
  return found;
}

static Standard_Boolean local_FindVertex(const TopOpeBRep_VPointInter& theVP,
					 const TopTools_IndexedDataMapOfShapeListOfShape& theMapOfVertexEdges,
					 TopoDS_Vertex& theVertex) {
  gp_Pnt aVPPoint = theVP.Value();
  Standard_Real aVPTolerance = theVP.Tolerance();
  Standard_Boolean vertexfound = Standard_False;
  for(Standard_Integer itVertex=1; !vertexfound && itVertex<=theMapOfVertexEdges.Extent(); itVertex++) {
    theVertex = TopoDS::Vertex(theMapOfVertexEdges.FindKey(itVertex));
    gp_Pnt aPoint = BRep_Tool::Pnt(theVertex);
    if(aVPPoint.IsEqual(aPoint, aVPTolerance)) {
	vertexfound = Standard_True;
      }
  }
  return vertexfound;
}

static void local_ReduceMapOfTreatedVertices(const TopOpeBRep_PLineInter& theCurrentLine) {

  if(localCurrentLine==NULL) {
    localCurrentLine = theCurrentLine;
    aMapOfTreatedVertexListOfEdge.Clear();
  }
  else {
    if(localCurrentLine != theCurrentLine) {
      localCurrentLine = theCurrentLine;
      aMapOfTreatedVertexListOfEdge.Clear();
    }
  }
}
// modified by NIZHNY-MKK  Tue Nov 21 17:32:55 2000.END
