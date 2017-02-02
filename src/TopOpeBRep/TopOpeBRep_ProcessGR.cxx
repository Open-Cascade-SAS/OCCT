// Created on: 1997-02-24
// Created by: Prestataire Xuan PHAM PHU
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
#include <TopOpeBRep_DRAW.hxx>
#endif

#include <gp_Vec.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Surface.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
//#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>
#include <TopOpeBRepDS_InterferenceTool.hxx>
#include <TopOpeBRep_FFTransitionTool.hxx>
#include <TopOpeBRep_FacesIntersector.hxx>
#include <TopOpeBRep_LineInter.hxx>
#include <TopOpeBRep_VPointInter.hxx>
#include <TopOpeBRep_Bipoint.hxx>
#include <TopOpeBRep_ListOfBipoint.hxx>
#include <TopOpeBRep_ListIteratorOfListOfBipoint.hxx>
#include <TopOpeBRep_VPointInterIterator.hxx>
#include <TopOpeBRep_GeomTool.hxx>
#include <Precision.hxx>
#include <Standard_CString.hxx>
#include <Standard_ProgramError.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRep.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_SC.hxx>

#ifdef OCCT_DEBUG
#include <TopOpeBRep_FFDumper.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Line.hxx>
extern Standard_Boolean TopOpeBRep_GettraceBIPS(); 
extern Standard_Boolean TopOpeBRep_GettraceDEGEN();
extern Standard_Boolean FUN_debnull(const TopoDS_Shape& s){Standard_Boolean isnull = s.IsNull(); if (isnull) cout <<"***"; return isnull;}
#endif

//Standard_EXPORT extern Standard_Real GLOBAL_tolFF;
Standard_EXPORTEXTERN Standard_Real GLOBAL_tolFF;

//=======================================================================
//function : StBipVPonF
//purpose  : 
//=======================================================================
TopAbs_State TopOpeBRep_FacesFiller::StBipVPonF
(const TopOpeBRep_VPointInter& vpf,const TopOpeBRep_VPointInter& vpl,
 const TopOpeBRep_LineInter& Lrest,const Standard_Boolean isonedge1) const
{
  
#define M_OUT(st) (st == TopAbs_OUT);
#define M_IN(st) (st == TopAbs_IN);
  
  Standard_Integer sind = isonedge1 ? 2 : 1; 
  TopAbs_State stf = vpf.State(sind);
  TopAbs_State stl = vpl.State(sind);
  Standard_Boolean isout = M_OUT(stf); isout = isout || M_OUT(stl);
  Standard_Boolean isin = M_IN(stf); isin = isin || M_IN(stl);   
  if (isout) return TopAbs_OUT;
  if (isin) return TopAbs_IN;

  Standard_Boolean isperiodic;
  const TopoDS_Edge& EArc = TopoDS::Edge(Lrest.Arc());
  BRepAdaptor_Curve BAC(EArc);
  GeomAbs_CurveType CT = BAC.GetType();
  isperiodic = (CT == GeomAbs_Circle);
  isperiodic = isperiodic || (CT == GeomAbs_Ellipse);

  TopOpeBRep_VPointInter vpff = vpf;
  TopOpeBRep_VPointInter vpll = vpl;

  // xpu200798 : CTS21216, restriction edge 7, f8
  // purpose : periodic restriction; vpf, vpl describing restriction bounds
  //      if the Rline is describing the portion on curve (vpl,vpf),
  //      we have to commutate these bounds.
  if (isperiodic) { 
#ifdef OCCT_DEBUG
//    TopOpeBRep_FFDumper FFD(*this); 
//    cout <<"vpf :"; FFD.DumpVP(vpf,cout);
//    cout <<"vpl :"; FFD.DumpVP(vpl,cout);
#endif

    Standard_Integer IArc = 0; 
    if (Lrest.ArcIsEdge(1)) IArc = 1; 
    if (Lrest.ArcIsEdge(2)) IArc = 2; 
    if (IArc == 0) {
#ifdef OCCT_DEBUG
      throw Standard_Failure("StBipVPonF");
#else
      return TopAbs_UNKNOWN;
#endif
    }
    Standard_Integer ISI = (IArc == 1) ? 2 : 1; 
    Standard_Integer sif = vpf.ShapeIndex();
    Standard_Integer sil = vpl.ShapeIndex();
    Standard_Boolean act = ((sif == 3)||(sif == ISI)) && ((sil == 3)||(sil == ISI));
    if (act) {
      TopOpeBRepDS_Transition Tf = TopOpeBRep_FFTransitionTool::ProcessLineTransition(vpf,ISI,vpf.Edge(ISI).Orientation());
      TopOpeBRepDS_Transition Tl = TopOpeBRep_FFTransitionTool::ProcessLineTransition(vpl,ISI,vpl.Edge(ISI).Orientation());
      Standard_Boolean toreverse = (Tf.Orientation(TopAbs_IN) == TopAbs_REVERSED);
      toreverse = toreverse && (Tl.Orientation(TopAbs_IN) == TopAbs_FORWARD);
      if (toreverse) {
	vpff = vpl;
	vpll = vpf;
      }
    } // act
  } // isperiodic

  TopoDS_Shape F;
  if (isonedge1) F = myF2;
  else           F = myF1;
  Standard_Real uf = TopOpeBRep_FacesFiller::VPParamOnER (vpff,Lrest);
  Standard_Real ul = TopOpeBRep_FacesFiller::VPParamOnER (vpll,Lrest);

  // NYI XPU: 16-05-97: INTPATCH -> the parametrization of a point on a 
  // periodized curve is INSUFFICIENT : parVP on line can be either 0. 
  // or period. 
  Standard_Boolean badparametrized = (uf > ul);
  Standard_Real f,l;  f = BAC.FirstParameter(); l = BAC.LastParameter();
  if (badparametrized) {
    if (isperiodic) {
      if (uf == l) uf = f;
      if (ul == f) ul = l;
    }
  }

  const TopoDS_Edge& arc = TopoDS::Edge(Lrest.Arc());
  BRepAdaptor_Curve BC( arc );     
  Standard_Real x = 0.789; Standard_Real parmil = (1-x)*uf + x*ul; //xpu170898
  gp_Pnt pmil = BC.Value(parmil);

#ifdef OCCT_DEBUG
#ifdef DRAW
  Standard_Boolean trc = TopOpeBRep_GettraceBIPS();
  if (trc) {TCollection_AsciiString aa("pmil"); FUN_brep_draw(aa,pmil);}
#endif
#endif
  TopAbs_State st = FSC_StatePonFace (pmil,F,*myPShapeClassifier);
  return st;
}

//=======================================================================
//function : StateVPonFace
//purpose  :
//=======================================================================
TopAbs_State TopOpeBRep_FacesFiller::StateVPonFace(const TopOpeBRep_VPointInter& VP) const
{
  Standard_Integer iVP = VP.ShapeIndex();
  if (iVP == 3) return TopAbs_ON;
  
  Standard_Integer iother =  (iVP == 1) ? 2 : 1;
  TopoDS_Shape F;
  if (iother == 1) F = myF1;
  else             F = myF2;
  Standard_Real u,v;
  if (iother == 1) VP.ParametersOnS1(u,v);
  else VP.ParametersOnS2(u,v);
  
  myPShapeClassifier->SetReference(TopoDS::Face(F));
  myPShapeClassifier->StateP2DReference(gp_Pnt2d(u,v));
  TopAbs_State state = myPShapeClassifier->State();

  return state;
}

// ----------------------------------------------------------------------
//           Class methods
// ----------------------------------------------------------------------

//=======================================================================
//function : Lminmax
//purpose  : Computes <pmin> and <pmax> the upper and lower bounds of <L>
//           enclosing all vpoints.
//=======================================================================
void TopOpeBRep_FacesFiller::Lminmax(const TopOpeBRep_LineInter& L,
				     Standard_Real& pmin,Standard_Real& pmax)
{
  pmin = RealLast();
  pmax = RealFirst();
  TopOpeBRep_VPointInterIterator VPI;
  VPI.Init(L,Standard_False);
  for (; VPI.More(); VPI.Next()) {
    const TopOpeBRep_VPointInter& VP = VPI.CurrentVP();
    Standard_Real p = VP.ParameterOnLine();
    pmin = Min(pmin,p);
    pmax = Max(pmax,p);
  }
  Standard_Real d = Abs(pmin-pmax);
  Standard_Boolean id = (d <= Precision::PConfusion());
  Standard_Boolean isper = L.IsPeriodic();
  Standard_Integer n = L.NbVPoint();
  if (id && isper && n >= 2) {
    Standard_Real per = L.Period();
    pmax = pmin + per;
  }
}

//=======================================================================
//function : LSameDomainERL
//purpose  : Returns <True> if GLine shares a same geometric domain with
//           at least one of the restriction edges of <ERL>.	
//=======================================================================
Standard_Boolean TopOpeBRep_FacesFiller::LSameDomainERL(const TopOpeBRep_LineInter& L,
					   const TopTools_ListOfShape& ERL)
{
  Standard_Boolean isone = Standard_False;
  if(L.TypeLineCurve() == TopOpeBRep_WALKING) return isone;

#ifdef DRAW
  Standard_Boolean trc = Standard_False;
  if (trc) {Handle(Geom_Curve) C = L.Curve(); TCollection_AsciiString aa("line"); FUN_brep_draw(aa,C);}
#endif

  Standard_Real f,l; TopOpeBRep_FacesFiller::Lminmax(L,f,l);
  Standard_Real d = Abs(f-l);

  {
    Standard_Boolean idINL = (L.INL() && (d == 0)); // null length line, made of VPoints only
    if (idINL) return Standard_False;
  } // INL

  Standard_Boolean id = (d <= Precision::PConfusion());
  if (id) return Standard_False;
  
  Handle(Geom_Curve) CL; TopOpeBRep_GeomTool::MakeCurve(f,l,L,CL); 
  Standard_Real t = 0.417789; Standard_Real p = (1-t)*f + t*l;
  gp_Pnt Pm = CL->Value(p);

  TopTools_ListIteratorOfListOfShape it; it.Initialize(ERL);
  for(; it.More(); it.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(it.Value());
    Standard_Real tolE = BRep_Tool::Tolerance(E);
    Standard_Real maxtol = Max(tolE,GLOBAL_tolFF);
    BRepAdaptor_Curve BAC(E);
    f = BAC.FirstParameter(); l = BAC.LastParameter();
    Standard_Boolean pinc = FUN_tool_PinC(Pm,BAC,f,l,maxtol);
    if (pinc) {isone = Standard_True; break;}
  }
  return isone;
}

//=======================================================================
//function : IsVPtransLok
//purpose  : Computes the transition <T> of the VPoint <iVP> on the edge
//           <SI12>. Returns <False> if the status is unknown.
//=======================================================================
Standard_Boolean TopOpeBRep_FacesFiller::IsVPtransLok(const TopOpeBRep_LineInter& L,
					 const Standard_Integer iVP,const Standard_Integer SI12,
					 TopOpeBRepDS_Transition& T)
{
  const TopOpeBRep_VPointInter& VP = L.VPoint(iVP);
  Standard_Boolean is1 = (SI12 == 1);
  Standard_Boolean VPonEd = (is1 && VP.IsOnDomS1());
  VPonEd = VPonEd || (!is1 && VP.IsOnDomS2());
  if (!VPonEd) return Standard_False;

  const TopoDS_Edge& E = TopoDS::Edge(VP.Edge(SI12)); 
  TopAbs_Orientation O = E.Orientation(); 
  T = TopOpeBRep_FFTransitionTool::ProcessLineTransition(VP,SI12,O);
  Standard_Boolean u = T.IsUnknown();
  return (!u);
}

Standard_Boolean TopOpeBRep_FacesFiller::TransvpOK(const TopOpeBRep_LineInter& L,
				      const Standard_Integer ivp,
				      const Standard_Integer SI,
				      const Standard_Boolean isINOUT)
{
#define M_INOUT(stf,stl) ((stf == TopAbs_IN) && (stl == TopAbs_OUT))
#define M_OUTIN(stf,stl) ((stf == TopAbs_OUT) && (stl == TopAbs_IN))

  TopOpeBRepDS_Transition T;
  Standard_Boolean ok = TopOpeBRep_FacesFiller::IsVPtransLok(L,ivp,SI,T);
  if (ok) {
    TopAbs_State stb = T.Before();
    TopAbs_State sta = T.After();
    if (isINOUT) ok = M_INOUT(stb,sta);
    else ok = M_OUTIN(stb,sta);
  }
  return ok;
}

//=======================================================================
//function : VPParamOnER
//purpose  :
//=======================================================================
Standard_Real TopOpeBRep_FacesFiller::VPParamOnER(const TopOpeBRep_VPointInter& vp,
					const TopOpeBRep_LineInter& Lrest)
{
  // If vp(index) is an edge boundary returns the point's parameter.

  const TopoDS_Edge& E = TopoDS::Edge(Lrest.Arc());
  Standard_Boolean isedge1 =  Lrest.ArcIsEdge(1);
  Standard_Boolean isedge2 =  Lrest.ArcIsEdge(2);
  if (isedge1 && vp.IsVertexOnS1()) {
    const TopoDS_Vertex& v1 = TopoDS::Vertex(vp.VertexOnS1());
    Standard_Real rr = BRep_Tool::Parameter(v1,E);
    return rr;
  }
  if (isedge2 && vp.IsVertexOnS2())  {
    const TopoDS_Vertex& v2 = TopoDS::Vertex(vp.VertexOnS2());
    Standard_Real rr = BRep_Tool::Parameter(v2,E);
    return rr;
  }
  // vp is an intersection point,and we get it's parameter.
  if (isedge1 && vp.IsOnDomS1()) return vp.ParameterOnArc1();
  if (isedge2 && vp.IsOnDomS2()) return vp.ParameterOnArc2();
  
  // Else,we have to project the point on the edge restriction
  Standard_Real tolee = BRep_Tool::Tolerance(E);
  tolee = tolee * 1.e2; //xpu290998 : PRO15369
  Standard_Real param, dist; Standard_Boolean projok = FUN_tool_projPonE(vp.Value(),tolee,E,param,dist);
  if (!projok) {
    throw Standard_ProgramError("TopOpeBRep_FacesFiller::VPParamOnER");
  }
  return param;
}

//Standard_EXPORT Standard_Boolean FUN_EqualPonR(const TopOpeBRep_LineInter& Lrest,
Standard_EXPORT Standard_Boolean FUN_EqualPonR(const TopOpeBRep_LineInter& ,
				  const TopOpeBRep_VPointInter& VP1,
				  const TopOpeBRep_VPointInter& VP2)
{
  gp_Pnt P1 = VP1.Value(); gp_Pnt P2 = VP2.Value();
  Standard_Real Ptol1 = VP1.Tolerance(),Ptol2 = VP2.Tolerance();
  Standard_Real Ptol = (Ptol1 > Ptol2) ? Ptol1 : Ptol2;
  Standard_Boolean Pequal = P1.IsEqual(P2,Ptol);
  return Pequal;
}
Standard_EXPORT Standard_Boolean FUN_EqualponR(const TopOpeBRep_LineInter& Lrest,
				  const TopOpeBRep_VPointInter& VP1,
				  const TopOpeBRep_VPointInter& VP2)
{
  Standard_Real p1 = TopOpeBRep_FacesFiller::VPParamOnER(VP1,Lrest);
  Standard_Real p2 = TopOpeBRep_FacesFiller::VPParamOnER(VP2,Lrest);
  Standard_Boolean pequal = fabs(p1-p2) < Precision::PConfusion();
  return pequal;
}

//=======================================================================
//function : EqualpPOnR
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRep_FacesFiller::EqualpPonR(const TopOpeBRep_LineInter& Lrest,
				       const TopOpeBRep_VPointInter& VP1,
				       const TopOpeBRep_VPointInter& VP2)
{
  Standard_Boolean Pequal = ::FUN_EqualPonR(Lrest,VP1,VP2);
  Standard_Boolean pequal = ::FUN_EqualponR(Lrest,VP1,VP2);
  Standard_Boolean pPequal = Pequal && pequal;
  return pPequal;
}
