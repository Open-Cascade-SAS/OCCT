// Created on: 1993-12-15
// Created by: Isabelle GRIGNON
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


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <AppBlend_Approx.hxx>
#include <Blend_CurvPointFuncInv.hxx>
#include <Blend_FuncInv.hxx>
#include <Blend_Function.hxx>
#include <Blend_RstRstFunction.hxx>
#include <Blend_SurfCurvFuncInv.hxx>
#include <Blend_SurfPointFuncInv.hxx>
#include <Blend_SurfRstFunction.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBlend_Line.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepLProp_SLProps.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <ChFi3d.hxx>
#include <ChFi3d_Builder.hxx>
#include <ChFi3d_Builder_0.hxx>
#include <ChFiDS_ChamfSpine.hxx>
#include <ChFiDS_CommonPoint.hxx>
#include <ChFiDS_ElSpine.hxx>
#include <ChFiDS_ErrorStatus.hxx>
#include <ChFiDS_FaceInterference.hxx>
#include <ChFiDS_FilSpine.hxx>
#include <ChFiDS_HData.hxx>
#include <ChFiDS_HElSpine.hxx>
#include <ChFiDS_ListIteratorOfListOfHElSpine.hxx>
#include <ChFiDS_ListOfHElSpine.hxx>
#include <ChFiDS_SequenceOfSurfData.hxx>
#include <ChFiDS_Spine.hxx>
#include <ChFiDS_State.hxx>
#include <ChFiDS_Stripe.hxx>
#include <ChFiDS_SurfData.hxx>
#include <ChFiKPart_ComputeData.hxx>
#include <ElCLib.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_ExtPS.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <Extrema_POnCurv.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <math_Vector.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TopAbs.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#ifdef OCCT_DEBUG
#ifdef DRAW
#include <DrawTrSurf.hxx>
#endif
#include <OSD_Chronometer.hxx>
extern Standard_Real  t_perfsetofkpart,t_perfsetofkgen,t_makextremities,t_performsurf,t_startsol;
extern Standard_Boolean ChFi3d_GettraceCHRON();
extern void ChFi3d_InitChron(OSD_Chronometer& ch);
extern void ChFi3d_ResultChron(OSD_Chronometer & ch, Standard_Real& time);
#endif


//===================================================================
//   Definition by a plane   
//
// It is considered that P1 and P2 are points associated to commonpoints compoint1 and
// compoint2, while E1 and E2 are edges containing P1 and P2.
// The plane containing three directions D12 T1 T2  ou  D12 represente la direction formee 
// par les points P1 et P2, T1 la tangente de E1 en P1 et T2 la tangente de 
// E2 en P2 is found (if exists). 
// Then fillet HConge is intersected by this plane 
// to find associated curve 3d C3d and the curve 2d. 
// 
//====================================================================
static void ChFi3d_CoupeParPlan (const ChFiDS_CommonPoint & compoint1,
                          const ChFiDS_CommonPoint & compoint2,
                          Handle(GeomAdaptor_HSurface)& HConge,
                          const gp_Pnt2d & UV1,
                          const gp_Pnt2d & UV2,
                          const Standard_Real tol3d,
                          const Standard_Real tol2d,
                          Handle(Geom_Curve) &C3d,
                          Handle(Geom2d_Curve) &pcurve,
                          Standard_Real & tolreached,
                          Standard_Real & Pardeb,
                          Standard_Real & Parfin,
                          Standard_Boolean & plane)
{ plane=Standard_True;
  if(compoint1.IsOnArc() && compoint2.IsOnArc() ) {
    gp_Pnt P1,P2;
    BRepAdaptor_Curve BCurv1(compoint1.Arc());
    BRepAdaptor_Curve BCurv2(compoint2.Arc());
    Standard_Real parE1,parE2;
    parE1=compoint1.ParameterOnArc();
    parE2=compoint2.ParameterOnArc();
    gp_Vec t1,t2;
    BCurv1.D1(parE1,P1,t1);
    BCurv2.D1(parE2,P2,t2);
    gp_Dir tgt1(t1);
    gp_Dir tgt2(t2);
    gp_Vec v12(P2.X()-P1.X(),P2.Y()-P1.Y(),P2.Z()-P1.Z());
    gp_Dir d12(v12); 
    gp_Dir nor =tgt1.Crossed(d12);
    Handle (Geom_Plane) Plan=new Geom_Plane(P1,nor);
    Standard_Real scal;
    scal=Abs(nor.Dot(tgt2));
    if (scal<0.01) {
      Handle(GeomAdaptor_HSurface) HPlan=new GeomAdaptor_HSurface(Plan);
      Handle(Geom2d_Curve) C2dint2;
      TColStd_Array1OfReal Pdeb(1,4),Pfin(1,4);
      GeomAdaptor_Surface AS(Plan);
      Extrema_ExtPS ext(P1,AS,1.e-3,1.e-3);
      Extrema_ExtPS ext1 (P2,AS,1.e-3,1.e-3);
      Standard_Real u1,v1;
      ext.Point(1).Parameter(u1,v1);
      Pdeb(1)= UV1.X();Pdeb(2) = UV1.Y();
      Pdeb(3)= u1;Pdeb(4) =v1;
      ext1.Point(1).Parameter(u1,v1);
      Pfin(1)= UV2.X();Pfin(2) = UV2.Y();
      Pfin(3)= u1;Pfin(4) = v1;
      if (ChFi3d_ComputeCurves(HConge,HPlan,Pdeb,Pfin,C3d,
			     pcurve,C2dint2,tol3d,tol2d,tolreached)){
        Pardeb=C3d->FirstParameter();
        Parfin=C3d->LastParameter();
      }
      else  plane=Standard_False;
    }
    else plane=Standard_False;
  }
  else   plane=Standard_False;
}
//=======================================================================
//function : SortieTangente
//purpose  : 
//=======================================================================

static Standard_Boolean SortieTangente(const ChFiDS_CommonPoint& CP,
				       const TopoDS_Face& /*F*/,
				       const Handle(ChFiDS_SurfData)& /*SD*/,
				       const Standard_Integer /*OnS*/,
				       const Standard_Real TolAngular)
{
  if(!CP.HasVector()) return Standard_False;
  gp_Pnt P;
  gp_Vec Darc, Dsurf;
  Handle(Geom_Curve) C;
  Standard_Real Uf, Ul;
  C = BRep_Tool::Curve(CP.Arc(),Uf,Ul);
  C->D1(CP.ParameterOnArc(), P, Darc);
  Dsurf = CP.Vector();
  return Dsurf.IsParallel(Darc, TolAngular);
}

//=======================================================================
//function : BonVoisin
//purpose  : 
//=======================================================================

static Standard_Boolean BonVoisin(const gp_Pnt& Point,
				  Handle(BRepAdaptor_HSurface)& HS,
				  TopoDS_Face& F,
				  Handle(GeomAdaptor_HSurface)& plane, 
				  const TopoDS_Edge& cured,
				  Standard_Real& XDep,
				  Standard_Real& YDep,
				  const ChFiDS_Map& EFMap,
				  const Standard_Real tolesp) 
{
  Standard_Boolean bonvoisin = 1;
  Standard_Real winter, Uf, Ul;
  gp_Pnt papp = HS->Value(XDep, YDep);
  Standard_Real dist = RealLast();
  Handle(BRepAdaptor_HCurve) hc = new BRepAdaptor_HCurve();
  Handle(Geom2d_Curve) PC;
  Standard_Boolean found = 0;

  TopExp_Explorer Ex;
  for(Ex.Init(F,TopAbs_EDGE); Ex.More(); Ex.Next()){
    const TopoDS_Edge& ecur = TopoDS::Edge(Ex.Current());
    if(!ecur.IsSame(cured)){
      hc->ChangeCurve().Initialize(ecur);
      Standard_Real tolc = hc->ChangeCurve().Resolution(tolesp);
      if(ChFi3d_InterPlaneEdge(plane,hc,winter,1,tolc)){
	gp_Pnt np = hc->Value(winter);
	Standard_Real ndist = np.SquareDistance(papp);
	if(ndist<dist){
	  TopTools_ListIteratorOfListOfShape It;
	  TopoDS_Face ff;  
	  Standard_Boolean isclosed = BRep_Tool::IsClosed(ecur, F);
	  Standard_Boolean isreallyclosed = 
	    BRepTools::IsReallyClosed(ecur, F);
	  for(It.Initialize(EFMap(ecur));It.More();It.Next()){  
	    ff = TopoDS::Face(It.Value());
	    Standard_Boolean issame = ff.IsSame(F);
//  Modified by Sergey KHROMOV - Fri Dec 21 17:12:48 2001 Begin
// 	    Standard_Boolean istg = 
// 	      BRep_Tool::Continuity(ecur,ff,F) != GeomAbs_C0;
 	    Standard_Boolean istg = ChFi3d_isTangentFaces(ecur,ff,F);
//  Modified by Sergey KHROMOV - Fri Dec 21 17:12:51 2001 End
	    if((!issame || (issame && isreallyclosed)) && istg) {
	      found = 1;
	      TopoDS_Edge newe = ecur;
	      newe.Orientation(TopAbs_FORWARD);
	      dist = ndist;
	      HS->ChangeSurface().Initialize(ff);
	      if(isclosed && !isreallyclosed){
		TopoDS_Face fff = ff;
		fff.Orientation(TopAbs_FORWARD);
		TopExp_Explorer Ex2;
		for(Ex2.Init(fff,TopAbs_EDGE); 
		    Ex2.More(); Ex2.Next()){
		  if(newe.IsSame(Ex2.Current())){
		    newe = TopoDS::Edge(Ex2.Current());
		    PC = BRep_Tool::CurveOnSurface(newe,fff,Uf,Ul);
		    break;
		  }
		}
	      }
	      else PC = BRep_Tool::CurveOnSurface(newe,ff,Uf,Ul);
	      PC->Value(winter).Coord(XDep,YDep);
	      if(issame){
		gp_Pnt spt; gp_Vec sdu,sdv,nors;
		HS->D1(XDep, YDep, spt, sdu, sdv);
		nors = sdu.Crossed(sdv);
		gp_Pnt cpt; gp_Vec cd;
		hc->D1(winter,cpt,cd);
		gp_Vec vref(Point, cpt);
		TopoDS_Face fff = ff;
		fff.Orientation(TopAbs_FORWARD);
		if(vref.Dot(nors.Crossed(cd)) < 0.){
		  newe.Orientation(TopAbs_REVERSED);
		}
		PC = BRep_Tool::CurveOnSurface(newe,fff,Uf,Ul);
		PC->Value(winter).Coord(XDep, YDep);
	      }
	      break;
	    }
	  }
	}
      }
    }
  }
  if(!found) bonvoisin = 0;
  return bonvoisin;
}

//=======================================================================
//function : Projection
//purpose  : Projects a point on a curve
//=======================================================================

static Standard_Boolean Projection(Extrema_ExtPC&       PExt, 
				   const gp_Pnt&        P,
				   const Adaptor3d_Curve& C,
				   Standard_Real&       W,
				   Standard_Real        Tol)
{
  Standard_Real Dist2, daux2;
  Dist2 =  C.Value(W).SquareDistance(P);

  // It is checked if it is not already a solution
  if (Dist2 < Tol * Tol) 
    return Standard_True;

  Standard_Boolean Ok = Standard_False;

  // On essai une resolution initialise
  Extrema_LocateExtPC ext(P,C,W,Tol/10);
  if(ext.IsDone()) {
    daux2 = C.Value(ext.Point().Parameter()).SquareDistance(P);
    if (daux2 <Dist2 ) {
      W = ext.Point().Parameter();
      Dist2 = daux2;
      Ok = Standard_True;
      if (Dist2 < Tol * Tol) 
	return Standard_True;
    }  
  }
  
  // Global resolution
  PExt.Perform(P);
  if ( PExt.IsDone() ) {
    for (Standard_Integer ii=1; ii<= PExt.NbExt(); ii++) {
      if (PExt.SquareDistance(ii) < Dist2) {
	Dist2 = PExt.SquareDistance(ii);
	W  =  PExt.Point(ii).Parameter();
	Ok = Standard_True;
      }
    }
  }
  return Ok;
}
				   
//=======================================================================
//function : TgtKP
//purpose  : 
//=======================================================================

static void TgtKP(const Handle(ChFiDS_SurfData)& CD,
		  const Handle(ChFiDS_Spine)&    Spine,
		  const Standard_Integer         iedge,
		  const Standard_Boolean         isfirst,
		  gp_Pnt&                        ped,
		  gp_Vec&                        ded) 
{
  Standard_Real wtg = CD->InterferenceOnS1().Parameter(isfirst);
  const BRepAdaptor_Curve& bc = Spine->CurrentElementarySpine(iedge);
  if(Spine->Edges(iedge).Orientation() == TopAbs_FORWARD)
    bc.D1(wtg+bc.FirstParameter(),ped,ded);
  else{
    bc.D1(-wtg+bc.LastParameter(),ped,ded);
    ded.Reverse();
  }
  ded.Normalize();
}

//=======================================================================
//function : IsInput
//purpose  : Checks if a vector belongs to a Face
//=======================================================================

Standard_Boolean IsInput(const gp_Vec&          Vec,
			 const TopoDS_Vertex&   Ve,
			 const TopoDS_Face&     Fa)
{
  TopExp_Explorer FaceExp(Fa, TopAbs_WIRE);
  BRepTools_WireExplorer WireExp;
  Standard_Integer Trouve = 0;
  TopoDS_Wire  W;
  TopoDS_Edge  E;
  TopoDS_Vertex Vf, Vl;
  gp_Vec Vec3d[2];
  gp_Pnt Point;

  // Find edges and compute 3D vectors
  for ( ; (FaceExp.More() && (Trouve<2)); FaceExp.Next()) {
    W = TopoDS::Wire(FaceExp.Current());
    for (Trouve=0, WireExp.Init(W) ; 
	 WireExp.More() && (Trouve<2); WireExp.Next()) {
      E = TopoDS::Edge(WireExp.Current());
      TopExp::Vertices(E, Vf, Vl);
      if (Vf.IsSame(Ve)) {
	BRepAdaptor_Curve Cb(E);
	Cb.D1(BRep_Tool::Parameter(Ve, E), Point, Vec3d[Trouve]);
	Trouve++; 
      }
      else if (Vl.IsSame(Ve)) {
	BRepAdaptor_Curve Cb(E);
	Cb.D1(BRep_Tool::Parameter(Ve, E), Point, Vec3d[Trouve]);
	Vec3d[Trouve].Reverse();
	Trouve++; 
      }
    }
  }
  if (Trouve < 2) return Standard_False;
  // Calculate the normal and the angles in the asssociated vector plane
  gp_Vec Normal;
  Normal = Vec3d[0] ^ Vec3d[1];
  if (Normal.SquareMagnitude() < Precision::Confusion()) {//Colinear case
    return (Vec.IsParallel(Vec3d[0],Precision::Confusion())); 
  }

  Standard_Real amin, amax;
  amax = Vec3d[1].AngleWithRef(Vec3d[0], Normal);
  if (amax <0) {
    amin = amax;
    amax = 0;  
  }
  else amin = 0;

  // Projection of the vector
  gp_Ax3 Axe(Point, Normal, Vec3d[0]);  
  gp_Trsf Transf;
  Transf.SetTransformation (Axe);
  gp_XYZ coord = Vec.XYZ();
  Transf.Transforms(coord);
  coord.SetZ(0);
  Transf.Invert();
  Transf.Transforms(coord);
  gp_Vec theProj(coord); 

  // and finally...
  Standard_Real Angle = theProj.AngleWithRef(Vec3d[0], Normal);
  return ( (Angle >= amin) && (Angle<=amax));
} 

//=======================================================================
//function : IsG1
//purpose  : Find a neighbor G1 by an edge
//=======================================================================

Standard_Boolean IsG1(const ChFiDS_Map&         TheMap,
		      const TopoDS_Edge&        E,
		      const TopoDS_Face&        FRef,
		      TopoDS_Face&              FVoi) 
{
  TopTools_ListIteratorOfListOfShape It;    
  // Find a neighbor of E different from FRef (general case).
  for(It.Initialize(TheMap(E));It.More();It.Next()) {
    if (!TopoDS::Face(It.Value()).IsSame(FRef)) {
      FVoi = TopoDS::Face(It.Value());
//  Modified by Sergey KHROMOV - Fri Dec 21 17:09:32 2001 Begin
//    if (BRep_Tool::Continuity(E,FRef,FVoi) != GeomAbs_C0) {
      if (ChFi3d_isTangentFaces(E,FRef,FVoi)) {
//  Modified by Sergey KHROMOV - Fri Dec 21 17:09:33 2001 End
	return Standard_True;
      }
    }
  }
  // If is was not found it is checked if E is a cutting edge,
  // in which case FVoi = FRef is returned (less frequent case).
  TopExp_Explorer Ex;
  Standard_Boolean orset = Standard_False;
  TopAbs_Orientation orient = TopAbs_FORWARD ;
  TopoDS_Edge ed;
  for(Ex.Init(FRef,TopAbs_EDGE); Ex.More(); Ex.Next()){
    ed = TopoDS::Edge(Ex.Current());
    if(ed.IsSame(E)){
      if(!orset){ orient = ed.Orientation(); orset = Standard_True; }
      else if(ed.Orientation() == TopAbs::Reverse(orient)){
	FVoi = FRef;
//  Modified by Sergey KHROMOV - Fri Dec 21 17:15:12 2001 Begin
// 	if (BRep_Tool::Continuity(E,FRef,FRef) >= GeomAbs_G1) {	  
	if (ChFi3d_isTangentFaces(E,FRef,FRef)) {
//  Modified by Sergey KHROMOV - Fri Dec 21 17:15:16 2001 End
	  return Standard_True;
	}
	return Standard_False;
      }
    }
  }
  return Standard_False;
}

//=======================================================================
//function : SearchFaceOnV
//purpose  : Finds the output face(s) of the path by a vertex
//           The following criteria should be followed
//         -1 : The face shares regular edges with FRef 
//              (too hard condition that should be reconsidered)
//         -2 : The vector starting in CommonPoint "belongs" to the face
//========================================================================
static Standard_Integer SearchFaceOnV(const ChFiDS_CommonPoint&    Pc,
				      const TopoDS_Face&           FRef,
				      const ChFiDS_Map&            VEMap,
				      const ChFiDS_Map&            EFMap,
				      TopoDS_Face&                 F1,
				      TopoDS_Face&                 F2)
{
  // it is checked that it leaves the current face.
  Standard_Boolean FindFace = IsInput(Pc.Vector(), Pc.Vertex(), FRef);
  if (FindFace) {
    FindFace = IsInput(Pc.Vector().Reversed(), Pc.Vertex(), FRef);
  }
  // If it does not leave, it is finished
  if (FindFace) {
    F1 = FRef;
    return 1;
  }
  Standard_Integer Num = 0;
  Standard_Boolean Trouve;
  TopTools_ListIteratorOfListOfShape ItE, ItF;
  TopoDS_Edge E;
  TopoDS_Face FVoi;

  for(ItE.Initialize(VEMap(Pc.Vertex()));
      ItE.More() && (Num < 2); ItE.Next()) {
    E = TopoDS::Edge(ItE.Value());
    for(ItF.Initialize(EFMap(E)), Trouve=Standard_False;
	ItF.More()&&(!Trouve); ItF.Next()) {
      if (TopoDS::Face(ItF.Value()).IsSame(FRef)) {
	Trouve = Standard_True;
      }
    }
    if (Trouve) Trouve = IsG1(EFMap, E, FRef, FVoi);
    if (Trouve) Trouve = IsInput(Pc.Vector(), Pc.Vertex(), FVoi);
    if (Trouve) {
      if (Num == 0) F1 = FVoi;
      else F2 =  FVoi;
      Num++;
    }
  }
  return Num;
}

//=======================================================================
//function : ChangeTransition
//purpose  : Changes the transition of the second common Point, when the surface
//           does not cross the arc
//           As it is supposed that the support Faces are the same, it is enough
//           to examine the cas of cutting edges.
//========================================================================
static void ChangeTransition(const ChFiDS_CommonPoint&    Precedant,
			     ChFiDS_CommonPoint&          Courant,
			     Standard_Integer             FaceIndex,
			     const Handle(TopOpeBRepDS_HDataStructure)& DS)
{
  Standard_Boolean tochange = Standard_True;
  Standard_Real f,l;
  const TopoDS_Face& F = TopoDS::Face(DS->Shape(FaceIndex));
  const TopoDS_Edge& Arc = Precedant.Arc();
  Handle(Geom2d_Curve) PCurve1, PCurve2;
  PCurve1 = BRep_Tool::CurveOnSurface(Arc, F, f, l);
  TopoDS_Shape aLocalShape = Arc.Reversed();
  PCurve2 = BRep_Tool::CurveOnSurface(TopoDS::Edge(aLocalShape), F, f, l);
//  PCurve2 = BRep_Tool::CurveOnSurface(TopoDS::Edge(Arc.Reversed()), F, f, l);
  if (PCurve1 != PCurve2) { 
    // This is a cutting edge, it is necessary to make a small Geometric test
    gp_Vec tgarc;
    gp_Pnt P;
    BRepAdaptor_Curve AC(Arc);
    AC.D1(Precedant.ParameterOnArc(), P, tgarc);
    tochange = tgarc.IsParallel(Precedant.Vector(), Precision::Confusion());
  }

  if (tochange) 
    Courant.SetArc(Precision::Confusion(), 
		   Arc, 
		   Precedant.ParameterOnArc(),
		   TopAbs::Reverse(Precedant.TransitionOnArc()));

}

//=======================================================================
//function : CallPerformSurf
//purpose  : Encapsulates call to PerformSurf/SimulSurf
//========================================================================

void ChFi3d_Builder::
CallPerformSurf(Handle(ChFiDS_Stripe)&              Stripe,
		const Standard_Boolean              Simul,
		ChFiDS_SequenceOfSurfData&          SeqSD,
		Handle(ChFiDS_SurfData)&            SD,
		const Handle(ChFiDS_HElSpine)&      HGuide,
		const Handle(ChFiDS_Spine)&         Spine,
		const Handle(BRepAdaptor_HSurface)& HS1,
		const Handle(BRepAdaptor_HSurface)& HS3,
		const gp_Pnt2d&                     pp1,
		const gp_Pnt2d&                     pp3,
		const Handle(Adaptor3d_TopolTool)&          It1,
		const Handle(BRepAdaptor_HSurface)& HS2,
		const Handle(BRepAdaptor_HSurface)& HS4,
		const gp_Pnt2d&                     pp2,
		const gp_Pnt2d&                     pp4,
		const Handle(Adaptor3d_TopolTool)&          It2,
		const Standard_Real                 MaxStep,
		const Standard_Real                 Fleche,
		const Standard_Real                 /*TolGuide*/,
		Standard_Real&                      First,
		Standard_Real&                      Last,
		const Standard_Boolean              Inside,
		const Standard_Boolean              /*Appro*/,
		const Standard_Boolean              forward,
		const Standard_Boolean              RecOnS1,
		const Standard_Boolean              RecOnS2,
		math_Vector&                        Soldep,
		Standard_Integer&                   intf,
		Standard_Integer&                   intl,
                Handle(BRepAdaptor_HSurface)&       Surf1,
		Handle(BRepAdaptor_HSurface)&       Surf2) 
{
#ifdef OCCT_DEBUG
  OSD_Chronometer ch1;
#endif 
  Handle(BRepAdaptor_HSurface) HSon1, HSon2;
  HSon1 = HS1;
  HSon2 = HS2;
  // Definition of the domain of path It1, It2
  It1->Initialize((const Handle(Adaptor3d_HSurface)&)HSon1);
  It2->Initialize((const Handle(Adaptor3d_HSurface)&)HSon2);


  TopAbs_Orientation Or1 = HS1->ChangeSurface().Face().Orientation();
  TopAbs_Orientation Or2 = HS2->ChangeSurface().Face().Orientation();
  Standard_Integer Choix = 
    ChFi3d::NextSide(Or1,Or2,
		     Stripe->OrientationOnFace1(),
		     Stripe->OrientationOnFace2(),
		     Stripe->Choix());
  Soldep(1) = pp1.X(); Soldep(2) = pp1.Y();
  Soldep(3) = pp2.X(); Soldep(4) = pp2.Y();
      
  Standard_Real thef = First, thel = Last;
  Standard_Boolean isdone;

  if(Simul){
    isdone = SimulSurf(SD,HGuide,Spine,Choix,HS1,It1,HS2,It2,tolesp,First,Last,
                       Inside,Inside,forward,RecOnS1,RecOnS2,Soldep,intf,intl);
  }
  else{
	
#ifdef OCCT_DEBUG
    ChFi3d_InitChron(ch1);//initial perform for PerformSurf
#endif
	
    isdone = PerformSurf(SeqSD,HGuide,Spine,Choix,HS1,It1,HS2,It2,
                         MaxStep,Fleche,tolesp,
                         First,Last,Inside,Inside,forward,
                         RecOnS1,RecOnS2,Soldep,intf,intl);
#ifdef OCCT_DEBUG
    ChFi3d_ResultChron(ch1,t_performsurf);// result perf for PerformSurf   
#endif
  }

 // Case of error
 if (!isdone) {
   First = thef;
   Last = thel;
   Standard_Boolean reprise = Standard_False;
   if (! HS3.IsNull()) {
     HSon1 = HS3;
     It1->Initialize((const Handle(Adaptor3d_HSurface)&)HS3); 
     Or1 = HS3->ChangeSurface().Face().Orientation();
     Soldep(1) = pp3.X(); Soldep(2) = pp3.Y();
     reprise = Standard_True;
   }
   else if (! HS4.IsNull()) {
     HSon2 = HS4;
     It2->Initialize((const Handle(Adaptor3d_HSurface)&)HS4); 
     Or2 = HS4->ChangeSurface().Face().Orientation();
     Soldep(3) = pp4.X(); Soldep(4) = pp4.Y();
     reprise = Standard_True;
   }

   if (reprise) {
     Choix = ChFi3d::NextSide(Or1,Or2,
			      Stripe->OrientationOnFace1(),
			      Stripe->OrientationOnFace2(),
			      Stripe->Choix());
     if(Simul){
       isdone = SimulSurf(SD,HGuide,Spine,Choix,HSon1,It1,HSon2,It2,
                          tolesp,First,Last,
                          Inside,Inside,forward,RecOnS1,RecOnS2,
                          Soldep,intf,intl);
     }
     else{
	
#ifdef OCCT_DEBUG
       ChFi3d_InitChron(ch1);//init perf for PerformSurf
#endif
	
       isdone = PerformSurf(SeqSD,HGuide,Spine,Choix,HSon1,It1,HSon2,It2,
                            MaxStep,Fleche,tolesp,
                            First,Last,Inside,Inside,forward,
                            RecOnS1,RecOnS2,Soldep,intf,intl);
#ifdef OCCT_DEBUG
       ChFi3d_ResultChron(ch1,t_performsurf);// result perf for PerformSurf   
#endif
     }
   }
 }  
  Surf1 = HSon1;
  Surf2 = HSon2;
}

//=======================================================================
//function : StripeOrientation
//purpose  : Calculates the reference orientation determining the
//           concave face for construction of the fillet.
//=======================================================================

Standard_Boolean ChFi3d_Builder::StripeOrientations
(const Handle(ChFiDS_Spine)& Spine,
 TopAbs_Orientation&         Or1,
 TopAbs_Orientation&         Or2,
 Standard_Integer&           ChoixConge) const 
{
  //TopTools_ListIteratorOfListOfShape It;
  BRepAdaptor_Surface Sb1,Sb2;
  TopAbs_Orientation Of1,Of2;
  TopoDS_Face ff1,ff2;
  ChFi3d_conexfaces(Spine->Edges(1),ff1,ff2,myEFMap);
  Of1 = ff1.Orientation();
  ff1.Orientation(TopAbs_FORWARD);
  Sb1.Initialize(ff1);
  Of2 = ff2.Orientation();
  ff2.Orientation(TopAbs_FORWARD);
  Sb2.Initialize(ff2);

  ChoixConge = ChFi3d::ConcaveSide(Sb1,Sb2,Spine->Edges(1),
				   Or1,Or2);
  Or1 = TopAbs::Compose(Or1,Of1);
  Or2 = TopAbs::Compose(Or2,Of2);
  return Standard_True;
}


//=======================================================================
//function : ConexFaces
//purpose  : 
//=======================================================================

void ChFi3d_Builder::ConexFaces (const Handle(ChFiDS_Spine)&   Spine,
				 const Standard_Integer        IEdge,
				 const Standard_Integer        RC,	
				 Handle(BRepAdaptor_HSurface)& HS1,
				 Handle(BRepAdaptor_HSurface)& HS2) const
{
  if(HS1.IsNull()) HS1 = new BRepAdaptor_HSurface ();
  if(HS2.IsNull()) HS2 = new BRepAdaptor_HSurface ();
  BRepAdaptor_Surface& Sb1 = HS1->ChangeSurface();
  BRepAdaptor_Surface& Sb2 = HS2->ChangeSurface();

  TopoDS_Face ff1,ff2;  
  ChFi3d_conexfaces(Spine->Edges(IEdge),ff1,ff2,myEFMap);

  Sb1.Initialize(ff1);
  Sb2.Initialize(ff2);
  
  TopAbs_Orientation Or1,Or2;
  Standard_Integer Choix = ChFi3d::ConcaveSide(Sb1,Sb2,Spine->Edges(IEdge),
					       Or1,Or2);  
  if (RC%2 != Choix%2) {
    Sb1.Initialize(ff2);
    Sb2.Initialize(ff1);
  }
}

//=======================================================================
//function : StartSol
//purpose  : Calculates a starting solution :
//           - one starts by parsing about ten points on the spine,
//           - in case of fail one finds the solution on neighbor faces;
//             section plane of edges of the adjacent face 
//             and identication of the face by connection to that edge.
//=======================================================================

void ChFi3d_Builder::StartSol(const Handle(ChFiDS_Stripe)&      Stripe,
			      const Handle(ChFiDS_HElSpine)&    HGuide,
			      Handle(BRepAdaptor_HSurface)&     HS1,
			      Handle(BRepAdaptor_HSurface)&     HS2,
			      Handle(BRepTopAdaptor_TopolTool)& I1,
			      Handle(BRepTopAdaptor_TopolTool)& I2,
			      gp_Pnt2d&                         P1,
			      gp_Pnt2d&                         P2,
			      Standard_Real&                    First) const 
{
  Handle(ChFiDS_Spine)& Spine = Stripe->ChangeSpine();
  ChFiDS_ElSpine& els = HGuide->ChangeCurve();
  Standard_Integer nbed = Spine->NbEdges();
  Standard_Integer nbessaimax = 3*nbed;
  if (nbessaimax < 10) nbessaimax = 10;
  Standard_Real unsurnbessaimax = 1./nbessaimax;
  Standard_Real wf = 0.9981 * Spine->FirstParameter(1) +
    0.0019 * Spine->LastParameter(1);
  Standard_Real wl = 0.9973 * Spine->LastParameter(nbed) +
    0.0027 * Spine->FirstParameter(nbed);

  Standard_Real TolE = 1.0e-7;
  BRepAdaptor_Surface AS;  

  Standard_Integer nbessai;
  Standard_Integer iedge = 0;
  Standard_Integer RC = Stripe->Choix();
  gp_Vec2d derive;
  gp_Pnt2d P2d;
  TopoDS_Edge cured;
  TopoDS_Face f1,f2;
  TopAbs_Orientation Or1,Or2;
  Standard_Integer Choix = 0;
  math_Vector SolDep(1,4);
  Handle(Geom2d_Curve) PC;
  Extrema_ExtPC PExt;
  PExt.Initialize(els, 
		  Spine->FirstParameter(1),
		  Spine->LastParameter(nbed),
		  Precision::Confusion());
  TopAbs_State Pos1,Pos2;
  for(nbessai = 0; nbessai <= nbessaimax; nbessai++){
    Standard_Real t = nbessai*unsurnbessaimax;
    Standard_Real w = wf * (1. -t) + wl * t;
    Standard_Integer ie = Spine->Index(w);
    if(iedge != ie){
      iedge = ie;
      cured = Spine->Edges(iedge);
      TolE = BRep_Tool::Tolerance(cured);
      ConexFaces(Spine,iedge,RC,HS1,HS2);
      f1 = HS1->ChangeSurface().Face();
      f2 = HS2->ChangeSurface().Face();
      Or1 = f1.Orientation();
      Or2 = f2.Orientation();
      Choix = ChFi3d::NextSide(Or1,Or2,
			       Stripe->OrientationOnFace1(),
			       Stripe->OrientationOnFace2(),
			       RC);
    }

    Standard_Real woned,Uf,Ul, ResU, ResV;
    Spine->Parameter(iedge,w,woned,Standard_True);
    cured.Orientation(TopAbs_FORWARD);
    TopoDS_Face f1forward = f1, f2forward = f2;
    f1forward.Orientation(TopAbs_FORWARD);
    f2forward.Orientation(TopAbs_FORWARD);
    PC = BRep_Tool::CurveOnSurface(cured,f1forward,Uf,Ul);
    I1->Initialize((const Handle(Adaptor3d_HSurface)&)HS1);
    PC->D1(woned, P1, derive);
    // There are ponts on the border, and internal points are found
    if (derive.Magnitude() > Precision::PConfusion()) {
      derive.Normalized();
      derive.Rotate(M_PI/2);
      AS.Initialize(f1);
      ResU = AS.UResolution(TolE);
      ResV = AS.VResolution(TolE);
      derive *= 2*(Abs(derive.X())*ResU + Abs(derive.Y())*ResV);
      P2d = P1.Translated(derive);
      if (I1->Classify(P2d, Min(ResU, ResV), 0)== TopAbs_IN) {
	P1 = P2d;
      }
      else {
	 P2d = P1.Translated(-derive);
	 if (I1->Classify(P2d, Min(ResU, ResV), 0)== TopAbs_IN) {
	   P1 = P2d;
	 }
      }
    }
    if(f1.IsSame(f2)) cured.Orientation(TopAbs_REVERSED);
    PC = BRep_Tool::CurveOnSurface(cured,f2forward,Uf,Ul);
    P2 = PC->Value(woned);
    const Handle(Adaptor3d_HSurface)& HSon2 = HS2; // to avoid ambiguity
    I2->Initialize(HSon2);

    SolDep(1) = P1.X(); SolDep(2) = P1.Y();
    SolDep(3) = P2.X(); SolDep(4) = P2.Y();
    const BRepAdaptor_Curve& Ced = Spine->CurrentElementarySpine(iedge);
    gp_Pnt pnt = Ced.Value(woned);
  
    if (Projection(PExt, pnt, els, w, tolesp) &&
	PerformFirstSection(Spine,HGuide,Choix,HS1,HS2,
			    I1,I2,w,SolDep,Pos1,Pos2)) {
      P1.SetCoord(SolDep(1),SolDep(2));
      P2.SetCoord(SolDep(3),SolDep(4));
      First = w;
      return;
    }
  }
  // No solution was found for the faces adjacent to the trajectory.
  // Now one tries the neighbor faces.
  iedge = 0;
  for(nbessai = 0; nbessai <= nbessaimax; nbessai++){
    Standard_Real t = nbessai*unsurnbessaimax;
    Standard_Real w = wf * (1. -t) + wl * t;
    iedge = Spine->Index(w);
    cured = Spine->Edges(iedge);
    ConexFaces(Spine,iedge,RC,HS1,HS2);
    f1 = HS1->ChangeSurface().Face();
    f2 = HS2->ChangeSurface().Face();
    Or1 = f1.Orientation();
    Or2 = f2.Orientation();
    Choix = ChFi3d::NextSide(Or1,Or2,
			     Stripe->OrientationOnFace1(),
			     Stripe->OrientationOnFace2(),
			     RC);
    Standard_Real woned,Uf,Ul;
    Spine->Parameter(iedge,w,woned,Standard_True);
    TopoDS_Face f1forward = f1, f2forward = f2;
    f1forward.Orientation(TopAbs_FORWARD);
    f2forward.Orientation(TopAbs_FORWARD);
    PC = BRep_Tool::CurveOnSurface(cured,f1forward,Uf,Ul);
    P1 = PC->Value(woned);
    PC = BRep_Tool::CurveOnSurface(cured,f2forward,Uf,Ul);
    P2 = PC->Value(woned);
    const Handle(Adaptor3d_HSurface)& HSon1 = HS1; // to avoid ambiguity
    const Handle(Adaptor3d_HSurface)& HSon2 = HS2; // to avoid ambiguity
    I1->Initialize(HSon1);
    I2->Initialize(HSon2);
    SolDep(1) = P1.X(); SolDep(2) = P1.Y();
    SolDep(3) = P2.X(); SolDep(4) = P2.Y();
    const BRepAdaptor_Curve& Ced = Spine->CurrentElementarySpine(iedge);
    gp_Pnt pnt = Ced.Value(woned);
//    Extrema_LocateExtPC ext(pnt,els,w,1.e-8);
//    if(ext.IsDone()){
//      w = ext.Point().Parameter(); 
    if (Projection(PExt, pnt, els, w, tolesp)) {
      PerformFirstSection(Spine,HGuide,Choix,HS1,HS2,
			  I1,I2,w,SolDep,Pos1,Pos2);
      gp_Pnt P;
      gp_Vec V;
      HGuide->D1(w,P,V);
      Handle(Geom_Plane) pl = new Geom_Plane(P,V);
      Handle(GeomAdaptor_HSurface) plane = new GeomAdaptor_HSurface(pl);

      Standard_Boolean bonvoisin = 1, found = 0;
      Standard_Integer NbChangement;
      for (NbChangement = 1; bonvoisin && (!found) && (NbChangement < 5);
	   NbChangement++) {
	if(Pos1 != TopAbs_IN){
	  bonvoisin = BonVoisin(P, HS1, f1, plane, cured, 
				SolDep(1),SolDep(2), myEFMap, tolesp); 
	}
	if(Pos2 != TopAbs_IN && bonvoisin){
	  bonvoisin = BonVoisin(P, HS2, f2, plane, cured, 
				SolDep(3),SolDep(4), myEFMap, tolesp);
	} 
	if(bonvoisin){
	  f1 = HS1->ChangeSurface().Face();
	  f2 = HS2->ChangeSurface().Face();
	  Or1 = f1.Orientation();
	  Or2 = f2.Orientation();
	  Choix = ChFi3d::NextSide(Or1,Or2,
				   Stripe->OrientationOnFace1(),
				   Stripe->OrientationOnFace2(),
				   RC);
          const Handle(Adaptor3d_HSurface)& HSon1new = HS1; // to avoid ambiguity
          const Handle(Adaptor3d_HSurface)& HSon2new = HS2; // to avoid ambiguity
	  I1->Initialize(HSon1new);
	  I2->Initialize(HSon2new);
	  if(PerformFirstSection(Spine,HGuide,Choix,HS1,HS2,
				 I1,I2,w,SolDep,Pos1,Pos2)){
	    P1.SetCoord(SolDep(1),SolDep(2));
	    P2.SetCoord(SolDep(3),SolDep(4));
	    First = w;
	    found = Standard_True;
	  }
	}
      }
      if (found) return;
    }
  }
  Spine->SetErrorStatus(ChFiDS_StartsolFailure);
  throw Standard_Failure("StartSol echec");
}

//=======================================================================
//function : ChFi3d_BuildPlane
//purpose  : 
//=======================================================================

static void  ChFi3d_BuildPlane (TopOpeBRepDS_DataStructure&    DStr,
				Handle(BRepAdaptor_HSurface)&  HS,
				gp_Pnt2d&                      pons,
				const Handle(ChFiDS_SurfData)& SD,
				const Standard_Boolean         isfirst,
				const Standard_Integer         ons)
{
  Handle(Geom2d_Curve) Hc;      
  TopoDS_Face F = TopoDS::Face(DStr.Shape(SD->Index(ons)));
  Standard_Real u,v;
  gp_Pnt P;
  //gp_Vec V1,V2;
  
  if (SD->Vertex(isfirst,ons).IsOnArc()){
    Hc = BRep_Tool::CurveOnSurface
      (SD->Vertex(isfirst,ons).Arc(),F,u,v);
    Hc->Value(SD->Vertex(isfirst,ons).ParameterOnArc()).Coord(u,v);
    BRepLProp_SLProps theProp(HS->ChangeSurface(), u, v, 1, 1.e-12);
    if  (theProp.IsNormalDefined()) {
      P =  theProp.Value();
      Handle(Geom_Plane) Pln  = new Geom_Plane(P, theProp.Normal());
      TopoDS_Face        NewF = BRepLib_MakeFace(Pln, Precision::Confusion());
      NewF.Orientation(F.Orientation());
      pons.SetCoord(0.,0.);
      HS->ChangeSurface().Initialize(NewF);
      return; // everything is good !
    }
  }
  throw Standard_Failure("ChFi3d_BuildPlane : echec .");
}

//=======================================================================
//function : StartSol
//purpose  : If the commonpoint is not OnArc the input face
//           is returned and 2D point is updated,
//           if it is OnArc
//              if it is detached  the input face
//              is returned and 2D point is updated,
//              otherwise
//                 either there is a neighbor tangent face and it is returned
//                         with recalculated 2D point
//                 or if there is no face 
//                         if the reference arc is Vref (extremity of the spine)
//                            this is the end and the input face is returned 
//                         otherwise this is an obstacle and HC is updated.
//=======================================================================

Standard_Boolean  
ChFi3d_Builder::StartSol(const Handle(ChFiDS_Spine)&    Spine,
			 Handle(BRepAdaptor_HSurface)&  HS, // New face
			 gp_Pnt2d&                      pons,// " Localization
			 Handle(BRepAdaptor_HCurve2d)&  HC, // Representation of the obstacle
			 Standard_Real&                 W,
			 const Handle(ChFiDS_SurfData)& SD,
			 const Standard_Boolean         isfirst,
			 const Standard_Integer         ons,
			 Handle(BRepAdaptor_HSurface)&  HSref, // The other representation
			 Handle(BRepAdaptor_HCurve2d)&  HCref, // of the obstacle	   
			 Standard_Boolean&              RecP,
			 Standard_Boolean&              RecS,
			 Standard_Boolean&              RecRst,
			 Standard_Boolean&              c1obstacle,
			 Handle(BRepAdaptor_HSurface)&  HSBis, // Face of support 	
			 gp_Pnt2d&                      PBis,  // and its point
			 const Standard_Boolean         decroch,
			 const TopoDS_Vertex&           Vref) const 
{
  RecRst = RecS = RecP = c1obstacle = 0;
  TopOpeBRepDS_DataStructure& DStr = myDS->ChangeDS();
  TopoDS_Face Fv,Fref;
  //gp_Pnt2d  pp1,pp2;
  Handle(Geom2d_Curve) pc;      
  Standard_Real Uf,Ul;
  
  TopoDS_Face F = TopoDS::Face(DStr.Shape(SD->Index(ons)));
  if(!HSref.IsNull()) Fref = HSref->ChangeSurface().Face();
  const ChFiDS_CommonPoint& CP = SD->Vertex(isfirst,ons);
  HSBis.Nullify();

  if (CP.IsOnArc()) {
    Standard_Integer notons;
    if (ons == 1)  notons = 2;
    else           notons = 1;
    const ChFiDS_CommonPoint& CPbis = SD->Vertex(isfirst,notons);
    if (CPbis.IsOnArc()) { // It is checked if it is not the extension zone 
                  // In case CP is not at the end of surfdata and it is not necesary to take it into account
                  // except for separate cases (ie pointus) ...
      //ts and tns were earlier CP.Parameter() and CPbis.Parameter, but sometimes they had no values.
      Standard_Real ts=SD->Interference(ons).Parameter(isfirst), tns=SD->Interference(notons).Parameter(isfirst);
      Standard_Boolean isExtend;
      // Arbitrary test (to precise)
      if (isfirst) isExtend = (ts-tns > 100*tolesp); 
      else         isExtend = (tns-ts > 100*tolesp);
      if (isExtend && !CP.Point().IsEqual(CPbis.Point(), 0) ) {
	//  the state is preserved and False is returned (extension by the expected plane).
	HS->ChangeSurface().Initialize(F);
	pc = SD->Interference(ons).PCurveOnFace();
	// The 2nd point is given by its trace on the support surface
        RecS = Standard_False;
	pons = pc->Value(tns);
	return Standard_False;
      }
    }
  }  
  
  if (CP.IsVertex() && !HC.IsNull() && !decroch){
    //The edge is changed, the parameter is updated and
    //eventually the support face and(or) the reference face.
    TopoDS_Vertex VCP = CP.Vertex();
    TopoDS_Edge EHC = HC->ChangeCurve2d().Edge();
    //One starts by searching in Fref another edge referencing VCP.
    TopExp_Explorer ex1,ex2;
    TopoDS_Edge newedge, edgereg;
    TopoDS_Face bidface = Fref, facereg;
    bidface.Orientation(TopAbs_FORWARD);
    for(ex1.Init(bidface,TopAbs_EDGE); ex1.More(); ex1.Next()){
      const TopoDS_Edge& cured = TopoDS::Edge(ex1.Current());
      Standard_Boolean found = 0;
      if(!cured.IsSame(EHC)){
	for(ex2.Init(cured,TopAbs_VERTEX); ex2.More() && !found; ex2.Next()){
	  if(ex2.Current().IsSame(VCP)){
	    if(IsG1(myEFMap,cured,Fref,Fv)){
	      edgereg = cured;
	      facereg = Fv;
	    }
	    else found = 1;
	  }
	}
      }
      if(found) {
	newedge = cured;
	break;
      }
    }
    if(newedge.IsNull()){
      //It is checked if EHC is not a closed edge.
      TopoDS_Vertex V1,V2;
      TopExp::Vertices(EHC,V1,V2);
      if(V1.IsSame(V2)){
	newedge = EHC;
	Standard_Real w1 = BRep_Tool::Parameter(V1,EHC);
	Standard_Real w2 = BRep_Tool::Parameter(V2,EHC);
	const ChFiDS_FaceInterference& fi = SD->Interference(ons);
	const Handle(Geom2d_Curve)& pcf = fi.PCurveOnFace();
	Standard_Real ww = fi.Parameter(isfirst);
	
	gp_Pnt2d pww;
	if(!pcf.IsNull()) pww = pcf->Value(ww);
	else pww = SD->Get2dPoints(isfirst,ons);
	gp_Pnt2d p1 = HC->Value(w1);
	gp_Pnt2d p2 = HC->Value(w2);
	
	if(p1.Distance(pww) >  p2.Distance(pww)){
	  W = w1;
	  pons = p1;
	}
	else {
	  W = w2;
	  pons = p2;
	}
	RecP = c1obstacle = 1;
	return 1;
      }
      else if(!edgereg.IsNull()){
	// the reference edge and face are changed.
	Fref = facereg;
	HSref->ChangeSurface().Initialize(Fref);
	for(ex1.Init(facereg,TopAbs_EDGE); ex1.More() && newedge.IsNull(); ex1.Next()){
	  const TopoDS_Edge& cured = TopoDS::Edge(ex1.Current());
	  if(!cured.IsSame(edgereg)){
	    for(ex2.Init(cured,TopAbs_VERTEX); ex2.More(); ex2.Next()){
	      if(ex2.Current().IsSame(VCP)){
		if(!IsG1(myEFMap,cured,Fref,Fv)){
		  newedge = cured;
		}
	      }
	    }
	  }
	}
      }
    }
    // it is necessary to find the new support face of the fillet : 
    // connected to FRef along the newedge.
    if(newedge.IsNull()) {
      throw Standard_Failure("StartSol : chain is not possible, new obstacle not found");
    }
    if(IsG1(myEFMap,newedge,Fref,Fv)){
      throw Standard_Failure("StartSol : chain is not possible, config non processed");
    }
    else if(Fv.IsNull()){
      throw Standard_Failure("StartSol : chain is not possible, new obstacle not found");
    }
    else{
      HS->ChangeSurface().Initialize(Fv);
      W = BRep_Tool::Parameter(VCP,newedge);
      HCref->ChangeCurve2d().Initialize(newedge,Fref);
      TopoDS_Face newface = Fv;
      newface.Orientation(TopAbs_FORWARD);
      TopExp_Explorer ex;
      for(ex.Init(newface,TopAbs_EDGE); ex.More(); ex.Next()){
	if(ex.Current().IsSame(newedge)){
	  newedge = TopoDS::Edge(ex.Current());
	  break;
	}
      }
      HC->ChangeCurve2d().Initialize(newedge,Fv);
      pons = HC->Value(W);
    }
    RecP = c1obstacle = 1;
    return 1;
  } // End of Case Vertex && Obstacle 

  else if (CP.IsOnArc() && !HC.IsNull() && !decroch){
    //Nothing is changed, the parameter is only updated.
    W = CP.ParameterOnArc();
    c1obstacle = 1;
    return 1;
  }
  
  HC.Nullify();
  
  if (CP.IsOnArc()){
    const TopoDS_Edge& E = CP.Arc();
    if(decroch){
      HS->ChangeSurface().Initialize(Fref);
      W = CP.ParameterOnArc();
      pc = BRep_Tool::CurveOnSurface(E,Fref,Uf,Ul);
      pons = pc->Value(W);
      RecS = 1;
      return 1;
    }
    if (SearchFace(Spine,CP,F,Fv)){
      HS->ChangeSurface().Initialize(Fv);
      RecS = 1;
      if (CP.IsVertex()) { 
	// One goes directly by the Vertex
	Standard_Integer Nb;
	TopoDS_Face aux;
        // And it is checked that there are no other candidates
	Nb = SearchFaceOnV(CP, F, myVEMap, myEFMap, Fv, aux);

	pons = BRep_Tool::Parameters(CP.Vertex(), Fv);
	HS->ChangeSurface().Initialize(Fv);
	if (Nb >=2) {
	  HSBis = new (BRepAdaptor_HSurface)(aux);
	  PBis = BRep_Tool::Parameters(CP.Vertex(), aux);
	}
	return 1;
      }
      // otherwise one passes by the arc...
      if(!Fv.IsSame(F)){
	Fv.Orientation(TopAbs_FORWARD);
	TopoDS_Edge newedge;
	TopExp_Explorer ex;
	for(ex.Init(Fv,TopAbs_EDGE); ex.More(); ex.Next()){
	  if(ex.Current().IsSame(E)){
	    newedge = TopoDS::Edge(ex.Current());
	    break;
	  }
	}
	//gp_Vec Varc, VSurf;
        // In cas of Tangent output, the current face becomes the support face
	if (SortieTangente(CP, F, SD, ons, 0.1)) { 
	  pc = BRep_Tool::CurveOnSurface(CP.Arc(),F,Uf,Ul);
	  HSBis = new (BRepAdaptor_HSurface)(F);
	  PBis = pc->Value(CP.ParameterOnArc());
	}
	

	pc = BRep_Tool::CurveOnSurface(newedge,Fv,Uf,Ul);
      }
      else{
	TopoDS_Edge newedge = E;
	newedge.Reverse();
	Fv.Orientation(TopAbs_FORWARD);
	pc = BRep_Tool::CurveOnSurface(newedge,Fv,Uf,Ul);
      }
      pons = pc->Value(CP.ParameterOnArc());
      return 1;
    }
    else if(!Fv.IsNull()){    
      c1obstacle = 1;
      if(!Vref.IsNull()){
	TopExp_Explorer ex;
	for(ex.Init(E,TopAbs_VERTEX); ex.More(); ex.Next()){
	  if(ex.Current().IsSame(Vref)){
	    c1obstacle = 0;
	    break;
	  }
	}
      }
      if(c1obstacle){
	HS->ChangeSurface().Initialize(Fv);
	HSref->ChangeSurface().Initialize(F);
	W = CP.ParameterOnArc();
	HC = new BRepAdaptor_HCurve2d();
	TopoDS_Edge newedge;
	TopoDS_Face newface = Fv;
	newface.Orientation(TopAbs_FORWARD);
	TopExp_Explorer ex;
	for(ex.Init(newface,TopAbs_EDGE); ex.More(); ex.Next()){
	  if(ex.Current().IsSame(E)){
	    newedge = TopoDS::Edge(ex.Current());
	    break;
	  }
	}
	HC->ChangeCurve2d().Initialize(newedge,Fv);
	pons = HC->Value(W);
	HCref->ChangeCurve2d().Initialize(E,F);
	if(CP.IsVertex()) RecP = 1;
	else RecRst = 1;
	return 1;
      }
      else{
	HS->ChangeSurface().Initialize(F);
	W = CP.ParameterOnArc();
	pc = BRep_Tool::CurveOnSurface(E,F,Uf,Ul);
	pons = pc->Value(W);
	return Standard_False;
      }
    }
    else{ // there is no neighbor face, the state is preserved and False is returned.
      HS->ChangeSurface().Initialize(F);
      W = CP.ParameterOnArc();
      pc = BRep_Tool::CurveOnSurface(E,F,Uf,Ul);
      pons = pc->Value(W);
      return Standard_False;
    }
  }
  else{
    HS->ChangeSurface().Initialize(F);
    const ChFiDS_FaceInterference& FI = SD->Interference(ons);
    if(FI.PCurveOnFace().IsNull()) pons = SD->Get2dPoints(isfirst,ons);
    else pons = FI.PCurveOnFace()->Value(FI.Parameter(isfirst));
  }
  return Standard_True;
}

//=======================================================================
//function : SearchFace
//purpose  : 
//=======================================================================

Standard_Boolean  ChFi3d_Builder::SearchFace
                 (const Handle(ChFiDS_Spine)&  Spine,
		  const ChFiDS_CommonPoint&    Pc,
		  const TopoDS_Face&           FRef,
		  TopoDS_Face&                 FVoi) const
{
  Standard_Boolean Trouve = Standard_False;
  if (! Pc.IsOnArc()) return Standard_False;
  FVoi.Nullify();
  TopoDS_Edge E;
  if (Pc.IsVertex()){
    // attention it is necessary to analyze all faces that turn around of the vertex
#ifdef OCCT_DEBUG
    cout<<"Commonpoint on vertex, the process hangs up"<<endl;
#endif
    if (Pc.HasVector()) { //General processing
      TopoDS_Face Fbis;
      Standard_Integer nb_faces;
      nb_faces = SearchFaceOnV(Pc,  FRef, myVEMap, myEFMap, FVoi, Fbis);
      return ( nb_faces > 0);
    }
    else { // Processing using the spine
      Standard_Boolean  FindFace=Standard_False;
      gp_Pnt Point;
      gp_Vec VecSpine;
      Spine->D1(Pc.Parameter(), Point, VecSpine);
    
      // It is checked if one leaves from the current face.
      FindFace = IsInput(VecSpine, Pc.Vertex(), FRef);
      if (FindFace) {
	VecSpine.Reverse();
	FindFace = IsInput(VecSpine, Pc.Vertex(), FRef);
      }
      // If one does not leave, it is ended
      if (FindFace) {
	FVoi = FRef;
	return Standard_True;
      }
    
      // Otherwise one finds the next among shared Faces 
      // by a common edge G1
      TopTools_ListIteratorOfListOfShape ItE, ItF;
      for(ItE.Initialize(myVEMap(Pc.Vertex()));
	  ItE.More() && (!FindFace); ItE.Next()) {
	E = TopoDS::Edge(ItE.Value());
	Trouve=Standard_False;
	for(ItF.Initialize(myEFMap(E));//, Trouve=Standard_False;           15.11.99 SVV
	    ItF.More()&&(!Trouve); ItF.Next()) {
	  if (TopoDS::Face(ItF.Value()).IsSame(FRef)) {
	    Trouve = Standard_True;
	  }
	}
	if (Trouve) FindFace = IsG1(myEFMap, E, FRef, FVoi);
	if (FindFace) { 
	  FindFace = Standard_False;
	  if (Spine.IsNull()) {
	    //La Spine peut etre nulle (ThreeCorner)
#ifdef OCCT_DEBUG
	    cout << "FindFace sur vertex avec spine nulle! QUEZAKO ?" << endl;
#endif
	    return Standard_False;
	  }
	
	  // It is checked if the selected face actually possesses edges of the spine
	  // containing the vertex on its front
	  // This processing should go only if the Vertex belongs to the spine
	  // This is a single case, for other vertexes it is required to do other things
	  Trouve=Standard_False;
	  for (Standard_Integer IE=1;//, Trouve=Standard_False;                   15.11.99  SVV
	       (IE<=Spine->NbEdges()) && (!Trouve); IE++) {
	    E = Spine->Edges(IE);
	    if (  (TopExp::FirstVertex(E).IsSame(Pc.Vertex())) 
		||(TopExp::LastVertex(E) .IsSame(Pc.Vertex())) ) {
	      for(ItF.Initialize(myEFMap(E)), Trouve=Standard_False;
		  ItF.More()&&(!Trouve); ItF.Next()) {
		if (TopoDS::Face(ItF.Value()).IsSame(FVoi)) {
		  Trouve = Standard_True;
		}
	      }
	    }
	  } 
	  FindFace = Trouve;
	}
      }
    }
  }
  else {
    return IsG1(myEFMap, Pc.Arc(), FRef, FVoi);
  }
  return Standard_False;
}


//=======================================================================
//function : ChFi3d_SingularExtremity
//purpose  : load the vertex in the DS and calculate the pcurve 
//           for an extremity in case of singular freeboundary 
//           or periodic and singular at the cut.
//=======================================================================
static void ChFi3d_SingularExtremity( Handle(ChFiDS_Stripe)&     stripe,
				     TopOpeBRepDS_DataStructure& DStr,
				     const TopoDS_Vertex&        Vtx,	   
				     const Standard_Real         tol3d,
				     const Standard_Real         tol2d)
{
  Handle(ChFiDS_SurfData) Fd;
  Standard_Real tolreached;
  Standard_Real Pardeb, Parfin; 
  gp_Pnt2d VOnS1, VOnS2;
  Handle(Geom_Curve) C3d;
  Handle(Geom2d_Curve) PCurv;
  TopOpeBRepDS_Curve Crv;
  // SurfData and its CommonPoints,
  Standard_Integer Ivtx, Icurv;
  Standard_Boolean isfirst;
  
  if  (stripe->Spine()->IsPeriodic()) {
     isfirst = Standard_True;
     Fd = stripe->SetOfSurfData()->Sequence().First();
   }
  else {
    Standard_Integer sens;
    Standard_Integer num = ChFi3d_IndexOfSurfData(Vtx,stripe,sens);
    Fd =  stripe->SetOfSurfData()->Sequence().Value(num);
    isfirst = (sens == 1);
  }
 
  const ChFiDS_CommonPoint& CV1 = Fd->Vertex(isfirst,1);
  const ChFiDS_CommonPoint& CV2 = Fd->Vertex(isfirst,2);
  // Is it always degenerated ?
  if ( CV1.Point().IsEqual( CV2.Point(), 0) ) { 
    Ivtx = ChFi3d_IndexPointInDS(CV1, DStr);
    if (isfirst) {
      VOnS1 = Fd->InterferenceOnS1().PCurveOnSurf()->
	           Value(Fd->InterferenceOnS1().FirstParameter());
      VOnS2 = Fd->InterferenceOnS2().PCurveOnSurf()->
		     Value(Fd->InterferenceOnS2().FirstParameter());
    }
    else {
      VOnS1 = Fd->InterferenceOnS1().PCurveOnSurf()->
		    Value(Fd->InterferenceOnS1().LastParameter());
      VOnS2 = Fd->InterferenceOnS2().PCurveOnSurf()->
		    Value(Fd->InterferenceOnS2().LastParameter());
    }
      
    ChFi3d_ComputeArete(CV1, VOnS1,
			CV2, VOnS2,
			DStr.Surface(Fd->Surf()).Surface(),
			C3d, PCurv,
			Pardeb,Parfin, tol3d, tol2d, tolreached,0);
    Crv = TopOpeBRepDS_Curve(C3d,tolreached);
    Icurv = DStr.AddCurve(Crv);

    stripe->SetCurve(Icurv, isfirst);
    stripe->SetParameters(isfirst, Pardeb,Parfin);
    stripe->ChangePCurve(isfirst) = PCurv;
    stripe->SetIndexPoint(Ivtx, isfirst, 1);
    stripe->SetIndexPoint(Ivtx, isfirst, 2);

    if (stripe->Spine()->IsPeriodic()) { 
      // periodic case : The operation is renewed
      // the curve 3d is not shared.
      // 2 degenerated edges coinciding in 3d
      isfirst = Standard_False;
      Fd = stripe->SetOfSurfData()->Sequence().Last();
      VOnS1 = Fd->InterferenceOnS1().PCurveOnSurf()->
	      Value(Fd->InterferenceOnS1().LastParameter());
      VOnS2 = Fd->InterferenceOnS2().PCurveOnSurf()->
	      Value(Fd->InterferenceOnS2().LastParameter());

      ChFi3d_ComputeArete(CV1, VOnS1,
			  CV2, VOnS2,
			  DStr.Surface(Fd->Surf()).Surface(),
			  C3d, PCurv,
			  Pardeb,Parfin, tol3d, tol2d, tolreached,0);
      Crv = TopOpeBRepDS_Curve(C3d,tolreached);
      Icurv = DStr.AddCurve(Crv);

      stripe->SetCurve(Icurv, isfirst);
      stripe->SetParameters(isfirst, Pardeb,Parfin);
      stripe->ChangePCurve(isfirst) = PCurv;
      stripe->SetIndexPoint(Ivtx, isfirst, 1);
      stripe->SetIndexPoint(Ivtx, isfirst, 2);    
    }
  }
}

//=======================================================================
//function : ChFi3d_MakeExtremities
//purpose  : calculate Curves3d and pcurves of extremities in
//           periodic and freeboundary cases.
//=======================================================================
static Standard_Boolean IsFree(const TopoDS_Shape& E,
			       const ChFiDS_Map&   EFMap)
{
  if(!EFMap.Contains(E)) return 0;
  TopTools_ListIteratorOfListOfShape It;
  TopoDS_Shape Fref;
  for(It.Initialize(EFMap(E)); It.More(); It.Next()){
    if(Fref.IsNull()) Fref = It.Value();
    else if(!Fref.IsSame(It.Value())) return 0;
  }
  return 1;
}

static void ChFi3d_MakeExtremities(Handle(ChFiDS_Stripe)&      Stripe,
				   TopOpeBRepDS_DataStructure& DStr,
				   const ChFiDS_Map&           EFMap,
				   const Standard_Real         tol3d,
				   const Standard_Real         tol2d)
{
  Handle(ChFiDS_Spine)& sp = Stripe->ChangeSpine();
  Standard_Real Pardeb,Parfin;
  Handle(Geom_Curve) C3d;
  Standard_Real tolreached;
  if(sp->IsPeriodic()){
    Bnd_Box b1,b2;
    const Handle(ChFiDS_SurfData)& 
      SDF = Stripe->SetOfSurfData()->Sequence().First();
    const ChFiDS_CommonPoint& CV1 = SDF->VertexFirstOnS1();
    const ChFiDS_CommonPoint& CV2 = SDF->VertexFirstOnS2();
    if ( !CV1.Point().IsEqual(CV2.Point(), 0) ) { 
      ChFi3d_ComputeArete(CV1,
			  SDF->InterferenceOnS1().PCurveOnSurf()->
			  Value(SDF->InterferenceOnS1().FirstParameter()),
			  CV2,
			  SDF->InterferenceOnS2().PCurveOnSurf()-> 
			  Value(SDF->InterferenceOnS2().FirstParameter()),
			  DStr.Surface(SDF->Surf()).Surface(),C3d,
			  Stripe->ChangeFirstPCurve(),Pardeb,Parfin,
			  tol3d,tol2d,tolreached,0);
      TopOpeBRepDS_Curve Crv(C3d,tolreached);
      Stripe->ChangeFirstCurve(DStr.AddCurve(Crv));
      Stripe->ChangeFirstParameters(Pardeb,Parfin);
      Stripe->ChangeIndexFirstPointOnS1
	(ChFi3d_IndexPointInDS(SDF->VertexFirstOnS1(),DStr));
      Stripe->ChangeIndexFirstPointOnS2
	(ChFi3d_IndexPointInDS(SDF->VertexFirstOnS2(),DStr));
      Standard_Integer ICurv = Stripe->FirstCurve();
      Stripe->ChangeLastParameters(Pardeb,Parfin);
      Stripe->ChangeLastCurve(ICurv);
      Stripe->ChangeIndexLastPointOnS1(Stripe->IndexFirstPointOnS1());
      Stripe->ChangeIndexLastPointOnS2(Stripe->IndexFirstPointOnS2());
      
      const Handle(ChFiDS_SurfData)& 
	SDL = Stripe->SetOfSurfData()->Sequence().Last();
      
      
      ChFi3d_ComputePCurv(C3d,
			  SDL->InterferenceOnS1().PCurveOnSurf()->
			  Value(SDL->InterferenceOnS1().LastParameter()),
			  SDL->InterferenceOnS2().PCurveOnSurf()->
			  Value(SDL->InterferenceOnS2().LastParameter()),
			  Stripe->ChangeLastPCurve(),
			  DStr.Surface(SDL->Surf()).Surface(),
			  Pardeb,Parfin,tol3d,tolreached);
      Standard_Real oldtol = DStr.ChangeCurve(ICurv).Tolerance();
      DStr.ChangeCurve(ICurv).Tolerance(Max(oldtol,tolreached));
      if(CV1.IsOnArc()){
	ChFi3d_EnlargeBox(CV1.Arc(),EFMap(CV1.Arc()),CV1.ParameterOnArc(),b1);
      }
      
      if(CV2.IsOnArc()){
	ChFi3d_EnlargeBox(CV2.Arc(),EFMap(CV2.Arc()),CV2.ParameterOnArc(),b2);
      }
      ChFi3d_EnlargeBox(DStr,Stripe,SDF,b1,b2,1);
      ChFi3d_EnlargeBox(DStr,Stripe,SDL,b1,b2,0);
      if (!CV1.IsVertex())
	ChFi3d_SetPointTolerance(DStr,b1,Stripe->IndexFirstPointOnS1());
      if (!CV2.IsVertex())
	ChFi3d_SetPointTolerance(DStr,b2,Stripe->IndexFirstPointOnS2());
    }
    else {
      // Case of the single extremity
      if (CV1.IsVertex()) { 
	ChFi3d_SingularExtremity(Stripe, DStr, CV1.Vertex(), tol3d, tol2d);
      }
# if CHFI3D_DEB
      else { cout << "MakeExtremities : Singularity out of Vertex !!" << endl; }
# endif
    }
    return;
  }  
  
  const Handle(ChFiDS_SurfData)& 
    SDdeb = Stripe->SetOfSurfData()->Sequence().First();
    
  const ChFiDS_CommonPoint& cpdeb1 = SDdeb->VertexFirstOnS1();
  const ChFiDS_CommonPoint& cpdeb2 = SDdeb->VertexFirstOnS2();
  Standard_Boolean freedeb = sp->FirstStatus() == ChFiDS_FreeBoundary;
  if(!freedeb && cpdeb1.IsOnArc() && cpdeb2.IsOnArc()){
    freedeb = (IsFree(cpdeb1.Arc(),EFMap) && IsFree(cpdeb2.Arc(),EFMap));
  }
  if(freedeb){
    sp->SetFirstStatus(ChFiDS_FreeBoundary);
    Bnd_Box b1,b2;
    if ( !cpdeb1.Point().IsEqual(cpdeb2.Point(), 0) ) { 
      Standard_Boolean plane;  
      gp_Pnt2d UV1,UV2;
      UV1=SDdeb->InterferenceOnS1().PCurveOnSurf()->
			  Value(SDdeb->InterferenceOnS1().FirstParameter());     
      UV2=SDdeb->InterferenceOnS2().PCurveOnSurf()->
			  Value(SDdeb->InterferenceOnS2().FirstParameter());
// The intersection of the fillet by a plane is attempted

      Handle(GeomAdaptor_HSurface) HConge=ChFi3d_BoundSurf(DStr,SDdeb,1,2);
      ChFi3d_CoupeParPlan(cpdeb1,cpdeb2,HConge,UV1,UV2,
                          tol3d,tol2d,C3d,Stripe->ChangeFirstPCurve(),tolreached,
                          Pardeb,Parfin,plane);  
      if (!plane) 
      ChFi3d_ComputeArete(cpdeb1,
			  SDdeb->InterferenceOnS1().PCurveOnSurf()->
			  Value(SDdeb->InterferenceOnS1().FirstParameter()),
			  cpdeb2,
			  SDdeb->InterferenceOnS2().PCurveOnSurf()-> 
			  Value(SDdeb->InterferenceOnS2().FirstParameter()),
			  DStr.Surface(SDdeb->Surf()).Surface(),C3d,
			  Stripe->ChangeFirstPCurve(),Pardeb,Parfin,
			  tol3d,tol2d,tolreached,0);
      TopOpeBRepDS_Curve Crv(C3d,tolreached);
      Stripe->ChangeFirstCurve(DStr.AddCurve(Crv));
      Stripe->ChangeFirstParameters(Pardeb,Parfin);
      Stripe->ChangeIndexFirstPointOnS1
	(ChFi3d_IndexPointInDS(SDdeb->VertexFirstOnS1(),DStr));
      Stripe->ChangeIndexFirstPointOnS2
	(ChFi3d_IndexPointInDS(SDdeb->VertexFirstOnS2(),DStr));
      if(cpdeb1.IsOnArc()){
	ChFi3d_EnlargeBox(cpdeb1.Arc(),EFMap(cpdeb1.Arc()),cpdeb1.ParameterOnArc(),b1);
      }
      if(cpdeb2.IsOnArc()){
	ChFi3d_EnlargeBox(cpdeb2.Arc(),EFMap(cpdeb2.Arc()),cpdeb2.ParameterOnArc(),b2);
      }
      ChFi3d_EnlargeBox(DStr,Stripe,SDdeb,b1,b2,1);
      if (!cpdeb1.IsVertex())
	ChFi3d_SetPointTolerance(DStr,b1,Stripe->IndexFirstPointOnS1());
      if (!cpdeb2.IsVertex())
	ChFi3d_SetPointTolerance(DStr,b2,Stripe->IndexFirstPointOnS2());
    }
    else { // Case of a singular extremity
      if (cpdeb1.IsVertex()) { 
	ChFi3d_SingularExtremity(Stripe, DStr, cpdeb1.Vertex(), tol3d, tol2d);
      }
# if CHFI3D_DEB
      else { cout << "MakeExtremities : Singularity out of Vertex !!" << endl; }
# endif
    }
  }
  const Handle(ChFiDS_SurfData)& 
    SDfin = Stripe->SetOfSurfData()->Sequence().Last();
  const ChFiDS_CommonPoint& cpfin1 = SDfin->VertexLastOnS1();
  const ChFiDS_CommonPoint& cpfin2 = SDfin->VertexLastOnS2();
  Standard_Boolean freefin = sp->LastStatus() == ChFiDS_FreeBoundary;
  if(!freefin && cpfin1.IsOnArc() && cpfin2.IsOnArc()){
    freefin = (IsFree(cpfin1.Arc(),EFMap) && IsFree(cpfin2.Arc(),EFMap));
  }
  if(freefin){
    sp->SetLastStatus(ChFiDS_FreeBoundary);
    Bnd_Box b1,b2;
    if ( !cpfin1.Point().IsEqual(cpfin2.Point(), 0) ) { 
      Standard_Boolean plane;
      gp_Pnt2d UV1,UV2;
      UV1=SDfin->InterferenceOnS1().PCurveOnSurf()->
			  Value(SDfin->InterferenceOnS1().LastParameter());     
      UV2=SDfin->InterferenceOnS2().PCurveOnSurf()->
			  Value(SDfin->InterferenceOnS2().LastParameter());
// Intersection of the fillet by a plane is attempted

      Handle(GeomAdaptor_HSurface) HConge=ChFi3d_BoundSurf(DStr,SDfin,1,2);
      ChFi3d_CoupeParPlan(cpfin1,cpfin2,HConge,UV1,UV2,
                          tol3d,tol2d,C3d,Stripe->ChangeLastPCurve(),tolreached,
                          Pardeb,Parfin,plane);      
      if (!plane)   
      ChFi3d_ComputeArete(cpfin1,
			  SDfin->InterferenceOnS1().PCurveOnSurf()->
			  Value(SDfin->InterferenceOnS1().LastParameter()),
			  cpfin2,
			  SDfin->InterferenceOnS2().PCurveOnSurf()-> 
			  Value(SDfin->InterferenceOnS2().LastParameter()),
			  DStr.Surface(SDfin->Surf()).Surface(),C3d,
			  Stripe->ChangeLastPCurve(),Pardeb,Parfin,
			  tol3d,tol2d,tolreached,0);
      TopOpeBRepDS_Curve Crv(C3d,tolreached);
      Stripe->ChangeLastCurve(DStr.AddCurve(Crv));
      Stripe->ChangeLastParameters(Pardeb,Parfin);
      Stripe->ChangeIndexLastPointOnS1
	(ChFi3d_IndexPointInDS(SDfin->VertexLastOnS1(),DStr));
      Stripe->ChangeIndexLastPointOnS2
	(ChFi3d_IndexPointInDS(SDfin->VertexLastOnS2(),DStr));
      if(cpfin1.IsOnArc()){
	ChFi3d_EnlargeBox(cpfin1.Arc(),EFMap(cpfin1.Arc()),cpfin1.ParameterOnArc(),b1);
      }
      if(cpfin2.IsOnArc()){
	ChFi3d_EnlargeBox(cpfin2.Arc(),EFMap(cpfin2.Arc()),cpfin2.ParameterOnArc(),b2);
      }
      ChFi3d_EnlargeBox(DStr,Stripe,SDfin,b1,b2,0);
      if (!cpfin1.IsVertex())
	ChFi3d_SetPointTolerance(DStr,b1,Stripe->IndexLastPointOnS1());
      if (!cpfin2.IsVertex())
	ChFi3d_SetPointTolerance(DStr,b2,Stripe->IndexLastPointOnS2());
    }
    else { // Case of the single extremity
      if (cpfin1.IsVertex()) { 
	ChFi3d_SingularExtremity(Stripe, DStr, cpfin1.Vertex(), tol3d, tol2d);
      }
# if CHFI3D_DEB
      else { cout << "MakeExtremities : Singularity out of Vertex !!" << endl; }
# endif
    }
  }
}

//=======================================================================
//function : ChFi3d_Purge
//purpose  : 
//=======================================================================

static void ChFi3d_Purge (Handle(ChFiDS_Stripe)&    Stripe,
			  Handle(ChFiDS_SurfData)&  SD,
			  const ChFiDS_CommonPoint& VRef,
			  const Standard_Boolean    isfirst,
			  const Standard_Integer    ons,
			  Standard_Integer&         intf,
			  Standard_Integer&         intl)
{
  if (isfirst) intf = 1; else intl = 1; // End.
  Standard_Integer opp = 3-ons;
  if (!SD->Vertex(isfirst,opp).IsOnArc() || 
      SD->TwistOnS1() || SD->TwistOnS2() ) {
#ifdef OCCT_DEBUG
    cout<<"ChFi3d_Purge : No output on extension."<<endl;
#endif
    ChFiDS_SequenceOfSurfData& Seq = 
      Stripe->ChangeSetOfSurfData()->ChangeSequence();
    if(isfirst) Seq.Remove(1);
    else Seq.Remove(Seq.Length());
    return;
  }
  if (ons == 1) SD->ChangeIndexOfS1(0);
  else          SD->ChangeIndexOfS2(0);
  
  SD->ChangeVertex(!isfirst,ons) = VRef;
  SD->ChangeVertex(isfirst,ons)  = VRef;
  
  ChFiDS_FaceInterference& fi = SD->ChangeInterference(ons);
  if(isfirst) fi.SetFirstParameter(fi.LastParameter());
  else fi.SetLastParameter(fi.FirstParameter());
  fi.SetLineIndex(0);
}

//=======================================================================
//function : InsertAfter
//purpose  : insert Item after ref in Seq. If ref is null, the item is
//           inserted at the beginning.
//=======================================================================

static void InsertAfter (Handle(ChFiDS_Stripe)&   Stripe,
			 Handle(ChFiDS_SurfData)& Ref,
			 Handle(ChFiDS_SurfData)& Item)
{
  if (Ref == Item) 
    throw Standard_Failure("InsertAfter : twice the same surfdata.");
  
  ChFiDS_SequenceOfSurfData& Seq = 
    Stripe->ChangeSetOfSurfData()->ChangeSequence();
  
  if (Seq.IsEmpty() || Ref.IsNull()) {
    Seq.Prepend(Item);
  }
  for (Standard_Integer i = 1; i <= Seq.Length(); i++) {
    if (Seq.Value(i) == Ref) {
      Seq.InsertAfter(i,Item);
      break;
    }
  }
}

//=======================================================================
//function : RemoveSD
//purpose  : 
//=======================================================================

static void RemoveSD (Handle(ChFiDS_Stripe)&   Stripe,
		      Handle(ChFiDS_SurfData)& Prev,
		      Handle(ChFiDS_SurfData)& Next)
{
  ChFiDS_SequenceOfSurfData& Seq = 
    Stripe->ChangeSetOfSurfData()->ChangeSequence();
  if(Seq.IsEmpty()) return;
  Standard_Integer iprev = 0, inext = 0;
  for (Standard_Integer i = 1; i <= Seq.Length(); i++) {
    if (Seq.Value(i) == Prev) iprev = i + 1;
    if (Seq.Value(i) == Next) { inext = i - 1; break; }
  }
  if(Prev.IsNull()) iprev = 1;
  if(Next.IsNull()) inext = Seq.Length();
  if(iprev <= inext) Seq.Remove(iprev,inext);
}

//=======================================================================
//function : InsertBefore
//purpose  : Insert item before ref in Seq. If ref is null, the item is
//           inserted in the queue.
//=======================================================================

static void InsertBefore (Handle(ChFiDS_Stripe)&   Stripe,
			  Handle(ChFiDS_SurfData)& Ref,
			  Handle(ChFiDS_SurfData)& Item)
{
  if (Ref == Item) 
    throw Standard_Failure("InsertBefore : twice the same surfdata.");
  
  ChFiDS_SequenceOfSurfData& Seq = 
    Stripe->ChangeSetOfSurfData()->ChangeSequence();
  
  if (Seq.IsEmpty() || Ref.IsNull()) {
    Seq.Append(Item);
  }
  for (Standard_Integer i = 1; i <= Seq.Length(); i++) {
    if (Seq.Value(i) == Ref) {
      Seq.InsertBefore(i,Item);
      break;
    }
  }
}


//=======================================================================
//function : PerformSetOfSurfOnElSpine
//purpose  : 
//=======================================================================

void ChFi3d_Builder::PerformSetOfSurfOnElSpine
(const Handle(ChFiDS_HElSpine)&    HGuide,
 Handle(ChFiDS_Stripe)&            Stripe,
 Handle(BRepTopAdaptor_TopolTool)& It1,
 Handle(BRepTopAdaptor_TopolTool)& It2,
 const Standard_Boolean            Simul)
{ 
#ifdef OCCT_DEBUG
  OSD_Chronometer ch1;
#endif 
  
  ChFiDS_ElSpine& Guide = HGuide->ChangeCurve();
  Standard_Real wf = Guide.FirstParameter();
  Standard_Real wl = Guide.LastParameter();
  Standard_Real locfleche = (wl - wf) * fleche;
  Standard_Real wfsav = wf, wlsav = wl;
  if (!Guide.IsPeriodic())
  {
    //Now the ElSpine is artificially extended to help rsnld.
    Standard_Real prab = 0.01;
    Guide.FirstParameter(wf-prab*(wl-wf));
    Guide.LastParameter (wl+prab*(wl-wf));
  }
  Handle(ChFiDS_Spine)&  Spine = Stripe->ChangeSpine();
  Standard_Integer ii, nbed = Spine->NbEdges();
  Standard_Real lastedlastp = Spine->LastParameter(nbed);
  
  TopOpeBRepDS_DataStructure& DStr = myDS->ChangeDS();
  
  Handle(ChFiDS_SurfData) ref = Guide.Previous();
  Handle(ChFiDS_SurfData) refbis, SD;
  Handle(ChFiDS_SurfData) raf = Guide.Next();
  RemoveSD(Stripe,ref,raf);
  
  Handle(BRepAdaptor_HSurface) HS1 = new BRepAdaptor_HSurface();
  Handle(BRepAdaptor_HSurface) HS2 = new BRepAdaptor_HSurface();
  Handle(BRepAdaptor_HSurface) HS3, HS4;
  Handle(BRepAdaptor_HSurface) HSref1 = new BRepAdaptor_HSurface();
  Handle(BRepAdaptor_HSurface) HSref2 = new BRepAdaptor_HSurface();
  Handle(BRepAdaptor_HCurve2d) HC1,HC2;
  Handle(BRepAdaptor_HCurve2d) HCref1 = new BRepAdaptor_HCurve2d();
  Handle(BRepAdaptor_HCurve2d) HCref2 = new BRepAdaptor_HCurve2d();
  Standard_Boolean decroch1 = Standard_False, decroch2 = Standard_False;
  Standard_Boolean RecP1 = Standard_False, RecS1 = Standard_False, RecRst1 = Standard_False, obstacleon1 = Standard_False;
  Standard_Boolean RecP2 = Standard_False, RecS2 = Standard_False, RecRst2 = Standard_False, obstacleon2 = Standard_False;
  gp_Pnt2d pp1,pp2,pp3,pp4;
  Standard_Real w1 = 0.,w2 = 0.;
  math_Vector Soldep(1,4);
  math_Vector SoldepCS(1,3);
  math_Vector SoldepCC(1,2);
  
  // Restore a neighboring KPart.
  // If no neighbor calculation start point. 
  Standard_Boolean forward = Standard_True;
  Standard_Boolean Inside  = Standard_False;
  Standard_Real    First   = wf;
  Standard_Real    Last    = wl;
  Standard_Boolean Ok1 = 1,Ok2 = 1;
  // Restore the next KPart if it exists
  TopoDS_Vertex Vref;
  if(ref.IsNull() && raf.IsNull()){
    //sinon solution approchee.
    Inside = Standard_True;
    
#ifdef OCCT_DEBUG
    ChFi3d_InitChron(ch1);// init perf for StartSol 
#endif
    
    StartSol(Stripe,HGuide,HS1,HS2,It1,It2,pp1,pp2,First);
    
#ifdef OCCT_DEBUG
    ChFi3d_ResultChron(ch1,t_startsol); // result perf for StartSol  
#endif 
    
    Last = wf;
    if(Guide.IsPeriodic()) {
      Last = First - Guide.Period();
      Guide.SaveFirstParameter();
      Guide.FirstParameter(Last);
      Guide.SaveLastParameter();
      Guide.LastParameter (First * 1.1);//Extension to help rsnld.
    }
  }
  else{
    if(!Spine->IsPeriodic() && (wl - lastedlastp > -tolesp)){ 
      Vref = Spine->LastVertex(); 
    }
    if (ref.IsNull()) {
      if(!Spine->IsPeriodic() && (wf < tolesp)){ 
	Vref = Spine->FirstVertex(); 
      }
      ref = raf;
      forward = Standard_False;
      First = wl; Last = Guide.FirstParameter();
    }
    
#ifdef OCCT_DEBUG
    ChFi3d_InitChron(ch1);// init perf for startsol 
#endif
    
    
    Ok1 = StartSol(Spine,HS1,pp1,HC1,w1,ref,!forward,1,
		   HSref1,HCref1,RecP1,RecS1,RecRst1,obstacleon1,
		   HS3,pp3,decroch1,Vref);
    Ok2 = StartSol(Spine,HS2,pp2,HC2,w2,ref,!forward,2,
		   HSref2,HCref2, RecP2,RecS2,RecRst2,obstacleon2,
		   HS4,pp4,decroch2,Vref);
    HC1.Nullify(); 
    HC2.Nullify();
    
#ifdef OCCT_DEBUG
    ChFi3d_ResultChron(ch1,t_startsol); // result perf for startsol  
#endif
    
    
    if(Ok1 == 1 && Ok2 == 1) {
      if(forward) Guide.FirstParameter(wf);
      else Guide.LastParameter(wl);
    }
  }
  Standard_Boolean      fini     = Standard_False;
  Standard_Boolean      complete = Inside;
  if(!Guide.IsPeriodic()){
    Standard_Integer indf = Spine->Index(wf);
    Standard_Integer indl = Spine->Index(wl,0);
    if(Spine->IsPeriodic() && (indl < indf)) indl += nbed;
    nbed = indl-indf+1;
  }
  // No Max at the touch : 20 points by edge at average without  
  // counting the extensions.

  Standard_Real bidf = wf, bidl = wl;
  if(!Spine->IsPeriodic()) {
    bidf = Max(0.,wf);
    bidl = Min(wl,Spine->LastParameter(Spine->NbEdges()));
    // PMN 20/07/98 : Attention in case if there is only extension
    if  ((bidl-bidf) < 0.01 * Spine->LastParameter(Spine->NbEdges())) {
       bidf = wf;
       bidl = wl;
    }   
  }
  Standard_Real         MaxStep  = (bidl-bidf)*0.05/nbed;
  Standard_Real         Firstsov = 0.;
  Standard_Integer      intf = 0, intl = 0;
  while(!fini){
    // are these the ends (no extension on periodic).
    Ok1 = 1,Ok2 = 1;
    if(!Spine->IsPeriodic()){
      if(wf < tolesp && (complete == Inside)){
	if(Spine->FirstStatus() == ChFiDS_OnSame) intf = 2;
	else intf = 1;
      }
      if(Spine->IsTangencyExtremity(Standard_True)){
	intf = 4;
	Guide.FirstParameter(wfsav);
      }
      if(wl - lastedlastp > -tolesp){
	if(Spine->LastStatus() == ChFiDS_OnSame) intl = 2;
	else intl = 1;
      }
      if(Spine->IsTangencyExtremity(Standard_False)){
	intl = 4;
	Guide.LastParameter(wlsav);
      }
    }
    if(intf && !forward) Vref = Spine->FirstVertex();
    if(intl && forward) Vref = Spine->LastVertex();
    if(!ref.IsNull()){
      
#ifdef OCCT_DEBUG
      ChFi3d_InitChron(ch1);// init perf for StartSol 
#endif
      
      Ok1 = StartSol(Spine,HS1,pp1,HC1,w1,ref,!forward,1,
		     HSref1,HCref1, RecP1,RecS1,RecRst1,obstacleon1,
		     HS3,pp3,decroch1,Vref);
      Ok2 = StartSol(Spine,HS2,pp2,HC2,w2,ref,!forward,2,
		     HSref2,HCref2, RecP2,RecS2,RecRst2,obstacleon2,
		     HS4,pp4,decroch2,Vref);
      
#ifdef OCCT_DEBUG
      ChFi3d_ResultChron(ch1,t_startsol); // result perf for StartSol  
#endif 
      
    }
    
    // No more connected faces. Construction of the tangent plane to continue the path 
    // till the output on the other face.
    if ((!Ok1 && HC1.IsNull()) || (!Ok2 && HC2.IsNull())) {
      if ((intf && !forward) || (intl && forward)) {
	if (!Ok1) ChFi3d_BuildPlane (DStr,HS1,pp1,ref,!forward,1);
	if (!Ok2) ChFi3d_BuildPlane (DStr,HS2,pp2,ref,!forward,2);	
	if(intf) intf = 5;
	else if(intl) intl = 5;
	if(forward) Guide.FirstParameter(wf);
	else Guide.LastParameter(wl);
      }
      else throw Standard_Failure("PerformSetOfSurfOnElSpine : Chaining is impossible.");
    }
    
    // Definition of the domain of patch It1, It2
    const Handle(Adaptor3d_HSurface)& HSon1 = HS1; // to avoid ambiguity
    const Handle(Adaptor3d_HSurface)& HSon2 = HS2; // to avoid ambiguity
    It1->Initialize(HSon1);
    It2->Initialize(HSon2);
    
    // Calculate one (several if singularity) SurfaData
    SD = new ChFiDS_SurfData();
    ChFiDS_SequenceOfSurfData SeqSD;
    SeqSD.Append(SD);
    
    if(obstacleon1 && obstacleon2){
      TopAbs_Orientation Or1 = HSref1->ChangeSurface().Face().Orientation();
      TopAbs_Orientation Or2 = HSref2->ChangeSurface().Face().Orientation();
      Standard_Integer Choix = ChFi3d::NextSide(Or1,Or2,
						Stripe->OrientationOnFace1(),
						Stripe->OrientationOnFace2(),
						Stripe->Choix());


      // Calculate the criterion of Choice edge / edge
      if (Choix%2 == 0) Choix = 4;
      else Choix = 1;

      SoldepCC(1) = w1; SoldepCC(2) = w2;
      if(Simul){
	SimulSurf(SD,HGuide,Spine,Choix,
		  HS1,It1,HC1,HSref1,HCref1,decroch1,Or1,
		  HS2,It2,HC2,HSref2,HCref2,decroch2,Or2,
		  locfleche,tolesp,First,Last,Inside,Inside,forward,
		  RecP1,RecRst1,RecP2,RecRst2,SoldepCC);
      }
      else{
#ifdef OCCT_DEBUG
	ChFi3d_InitChron(ch1); // init perf for PerformSurf 
#endif
	PerformSurf(SeqSD,HGuide,Spine,Choix,
		    HS1,It1,HC1,HSref1,HCref1,decroch1,Or1,
		    HS2,It2,HC2,HSref2,HCref2,decroch2,Or2,
		    MaxStep,locfleche,tolesp,First,Last,Inside,Inside,forward,
		    RecP1,RecRst1,RecP2,RecRst2,SoldepCC);
#ifdef OCCT_DEBUG
	ChFi3d_ResultChron(ch1,t_performsurf); //result  perf for PerformSurf 
#endif 
      }
      SD->ChangeIndexOfS1(DStr.AddShape(HS1->ChangeSurface().Face()));
      SD->ChangeIndexOfS2(DStr.AddShape(HS2->ChangeSurface().Face()));
    }
    else if (obstacleon1){
      TopAbs_Orientation Or1 = HSref1->ChangeSurface().Face().Orientation();
      TopAbs_Orientation Or2 = HS2->ChangeSurface().Face().Orientation();
      Standard_Integer Choix = ChFi3d::NextSide(Or1,Or2,
						Stripe->OrientationOnFace1(),
						Stripe->OrientationOnFace2(),
						-Stripe->Choix());
      if(Choix%2 == 1) Choix++;
      else Choix--;
      SoldepCS(3) = w1; SoldepCS(1) =  pp2.X(); SoldepCS(2) =  pp2.Y();
      if(Simul){
	SimulSurf(SD,HGuide,Spine,Choix,HS1,It1,HC1,HSref1,HCref1,decroch1,
		  HS2,It2,Or2,locfleche,tolesp,First,Last,
		  Inside,Inside,forward,RecP1,RecS2,RecRst1,SoldepCS);
      }
      else{
#ifdef OCCT_DEBUG
	ChFi3d_InitChron(ch1); // init perf for PerformSurf
#endif
	PerformSurf(SeqSD,HGuide,Spine,Choix,HS1,It1,HC1,HSref1,HCref1,decroch1,
		    HS2,It2,Or2,MaxStep,locfleche,tolesp,First,Last,
		    Inside,Inside,forward,RecP1,RecS2,RecRst1,SoldepCS);
#ifdef OCCT_DEBUG
	ChFi3d_ResultChron(ch1,t_performsurf);//result  perf for PerformSurf  
#endif 
      }
      SD->ChangeIndexOfS1(DStr.AddShape(HS1->ChangeSurface().Face()));
      SD->ChangeIndexOfS2(DStr.AddShape(HS2->ChangeSurface().Face()));
      decroch2 = 0;
    }
    else if (obstacleon2){
      TopAbs_Orientation Or1 = HS1->ChangeSurface().Face().Orientation();
      TopAbs_Orientation Or2 = HSref2->ChangeSurface().Face().Orientation();
      Standard_Integer Choix = ChFi3d::NextSide(Or1,Or2,
						Stripe->OrientationOnFace1(),
						Stripe->OrientationOnFace2(),
						Stripe->Choix());
      SoldepCS(3) = w2; SoldepCS(1) =  pp1.X(); SoldepCS(2) =  pp1.Y();
      if(Simul){
	SimulSurf(SD,HGuide,Spine,Choix,HS1,It1,Or1,
		  HS2,It2,HC2,HSref2,HCref2,decroch2,locfleche,tolesp,
		  First,Last,Inside,Inside,forward,RecP2,RecS1,RecRst2,SoldepCS);
      }
      else{
#ifdef OCCT_DEBUG
	ChFi3d_InitChron(ch1); // init perf for PerformSurf 
#endif
	PerformSurf(SeqSD,HGuide,Spine,Choix,HS1,It1,Or1,
		    HS2,It2,HC2,HSref2,HCref2,decroch2,MaxStep,locfleche,tolesp,
		    First,Last,Inside,Inside,forward,RecP2,RecS1,RecRst2,SoldepCS);
#ifdef OCCT_DEBUG
	ChFi3d_ResultChron(ch1,t_performsurf); //result  perf for PerformSurf 
#endif 
      }
      SD->ChangeIndexOfS1(DStr.AddShape(HS1->ChangeSurface().Face()));
      SD->ChangeIndexOfS2(DStr.AddShape(HS2->ChangeSurface().Face()));
      decroch1 = 0;
    }
    else{ 
      const Handle(Adaptor3d_TopolTool)& aTT1 = It1; // to avoid ambiguity
      const Handle(Adaptor3d_TopolTool)& aTT2 = It2; // to avoid ambiguity
      CallPerformSurf(Stripe, Simul, SeqSD, SD,
		      HGuide,Spine,
		      HS1, HS3, pp1, pp3, aTT1,
		      HS2, HS4, pp2, pp4, aTT2, 
		      MaxStep,locfleche,tolesp,
		      First,Last,Inside,Inside,forward,
		      RecS1,RecS2,Soldep,intf,intl, 
		      HS1, HS2);
      decroch1 = decroch2 = 0;
    }

    if(!done) { // Case of fail
      if ((!Ok1 && !obstacleon1) || (!Ok2 && !obstacleon2)) {
	//Fail in a part of extension is not serious
	//Here one stops.
	done = Standard_True;
	Inside = Standard_False;
	if (forward) intl = 1;
	else         intf = 1;
      }
      else { // Otherwise invalidation of the stripe.
        Spine->SetErrorStatus(ChFiDS_WalkingFailure);
	throw Standard_Failure("CallPerformSurf : Path failed!");
      }
    }
    
    else {
      refbis = ref;
      if(forward) {
	for (ii=1; ii<=SeqSD.Length(); ii++) {
	  SD = SeqSD(ii);
	  SD->ChangeIndexOfS1(DStr.AddShape(HS1->ChangeSurface().Face()));
	  if(obstacleon1) SD->SetIndexOfC1(DStr.AddShape(HC1->ChangeCurve2d().Edge()));
	  SD->ChangeIndexOfS2(DStr.AddShape(HS2->ChangeSurface().Face()));
	  if(obstacleon2) SD->SetIndexOfC2(DStr.AddShape(HC2->ChangeCurve2d().Edge()));
	  InsertAfter (Stripe, refbis, SD);
	  refbis = SD;
	}
      }
      else {
	for (ii=SeqSD.Length(); ii>=1; ii--) {
	  SD = SeqSD(ii);
	  SD->ChangeIndexOfS1(DStr.AddShape(HS1->ChangeSurface().Face()));
	  if(obstacleon1) SD->SetIndexOfC1(DStr.AddShape(HC1->ChangeCurve2d().Edge()));
	  SD->ChangeIndexOfS2(DStr.AddShape(HS2->ChangeSurface().Face()));
	  if(obstacleon2) SD->SetIndexOfC2(DStr.AddShape(HC2->ChangeCurve2d().Edge()));
	  InsertBefore(Stripe,refbis,SD);
	  refbis = SD;
	}
      } 
    
      if (!Ok1 && !obstacleon1) 
	// clean infos on the plane of extension.
	ChFi3d_Purge (Stripe,SD,ref->Vertex(!forward,1),!forward,1,intf,intl);
      
      if (!Ok2 && !obstacleon2) 
	// clean infos on the plane of extension.
	ChFi3d_Purge (Stripe,SD,ref->Vertex(!forward,2),!forward,2,intf,intl);
    
      // The end. The reference is changed.      
      ref = refbis;  
    }  
    
    if(Inside){// There are starting solutions for the next.
      Inside = Standard_False;
      Firstsov = First;
      if(Guide.IsPeriodic()) {
	complete = Standard_False;
	wf  = Guide.FirstParameter();
	wl  = Guide.LastParameter();
      }
    }
    if(forward){
      fini = ((wl - Last) <= 10.*tolesp || 
	      (intl && !(obstacleon1 || obstacleon2))); //General case

      if (!fini && Guide.IsPeriodic() && 
	  ((wl - Last)< Guide.Period()*1.e-3)) {
	// It is tested if reframing of extremes is done at the same edge
        // Loop Condition
	Handle(ChFiDS_SurfData) thefirst, thelast;
	thefirst = Stripe->SetOfSurfData()->Sequence().First();
	thelast =  Stripe->SetOfSurfData()->Sequence().Last();
	
	if (thefirst->VertexFirstOnS1().IsOnArc() && 
	    thelast->VertexLastOnS1().IsOnArc())
	  fini = thefirst->VertexFirstOnS1().Arc().IsSame
	    (thelast->VertexLastOnS1().Arc());
	if (!fini &&
	    thefirst->VertexFirstOnS2().IsOnArc() && 
	    thelast->VertexLastOnS2().IsOnArc())
	  fini = thefirst->VertexFirstOnS2().Arc().IsSame
	    (thelast->VertexLastOnS2().Arc());

	if (fini) 
	  return; //It is ended!
      }

      if(fini && complete) {
	// restart in the opposite direction.
	ref  = Stripe->SetOfSurfData()->Sequence().First();
	forward = Standard_False;
	fini = Standard_False;
	First = Firstsov;
      }
      else {
	First = Last;
	Last  = wl;
      }
    }
    if(!forward){
      fini = ((First - wf) <= 10.*tolesp || 
	      (intf && !(obstacleon1 || obstacleon2)));
      complete = Standard_False;
      Last = wf;
    }
  }
  // The initial state is restored
  if(!Guide.IsPeriodic()){
    Guide.FirstParameter(wfsav);
    Guide.LastParameter (wlsav);
  }

}

//=======================================================================
//function : PerformSetOfKPart
//purpose  : 
//=======================================================================

void ChFi3d_Builder::PerformSetOfKPart(Handle(ChFiDS_Stripe)& Stripe,
				       const Standard_Boolean Simul) 
{
  TopOpeBRepDS_DataStructure&  DStr = myDS->ChangeDS();
  Handle(ChFiDS_Spine)&        Spine = Stripe->ChangeSpine();
  Handle(BRepAdaptor_HSurface) HS1,HS2;
  TopAbs_Orientation           Or1,Or2,RefOr1,RefOr2;
  Standard_Integer             RefChoix;
  
  // initialization of the stripe.
  Stripe->Reset();
  Handle(ChFiDS_HData)&  HData  = Stripe->ChangeSetOfSurfData();
  HData =  new ChFiDS_HData();
  ChFiDS_SequenceOfSurfData& SeqSurf = HData->ChangeSequence();
  
  StripeOrientations(Spine,RefOr1,RefOr2,RefChoix);
  Stripe->OrientationOnFace1(RefOr1);
  Stripe->OrientationOnFace2(RefOr2);
  Stripe->Choix(RefChoix);
  
  Handle(BRepTopAdaptor_TopolTool) It1 = new BRepTopAdaptor_TopolTool();
  Handle(BRepTopAdaptor_TopolTool) It2 = new BRepTopAdaptor_TopolTool();
  
  Standard_Real WFirst,WLast = 0.;
  gp_Vec TFirst,TLast,TEndPeriodic;
  gp_Pnt PFirst,PLast,PEndPeriodic;
  Standard_Boolean intf = Standard_False, intl = Standard_False;
  
  Handle(ChFiDS_HElSpine) CurrentHE = new ChFiDS_HElSpine();
  Spine->D1(Spine->FirstParameter(),PFirst,TFirst);
  CurrentHE->ChangeCurve().FirstParameter(Spine->FirstParameter());
  CurrentHE->ChangeCurve().SetFirstPointAndTgt(PFirst,TFirst);
  
  Standard_Boolean YaKPart = Standard_False;
  Standard_Integer iedgelastkpart = 0;
  
  Standard_Real WStartPeriodic = 0.;
  Standard_Real WEndPeriodic = Spine->LastParameter(Spine->NbEdges());
  Spine->D1(WEndPeriodic,PEndPeriodic,TEndPeriodic);
  
  // Construction of particular cases.
  
  for (Standard_Integer iedge = 1; iedge <= Spine->NbEdges(); iedge++){
    
    ConexFaces(Spine,iedge,RefChoix,HS1,HS2);
    
    if (ChFi3d_KParticular(Spine,iedge,HS1->ChangeSurface(),HS2->ChangeSurface())) {
      intf = ((iedge == 1) && !Spine->IsPeriodic());
      intl = ((iedge == Spine->NbEdges()) && !Spine->IsPeriodic());
      Or1   = HS1->ChangeSurface().Face().Orientation();
      Or2   = HS2->ChangeSurface().Face().Orientation();
      ChFi3d::NextSide(Or1,Or2,RefOr1,RefOr2,RefChoix);      
      const Handle(Adaptor3d_HSurface)& HSon1 = HS1; // to avoid ambiguity
      const Handle(Adaptor3d_HSurface)& HSon2 = HS2; // to avoid ambiguity
      It1->Initialize(HSon1);
      It2->Initialize(HSon2);
      
      Handle(ChFiDS_SurfData)   SD = new ChFiDS_SurfData();
      ChFiDS_SequenceOfSurfData LSD;
      
      if(!ChFiKPart_ComputeData::Compute(DStr,SD,HS1,HS2,Or1,Or2,Spine,iedge)){
#ifdef OCCT_DEBUG
	cout<<"failed calculation KPart"<<endl;
#endif
      }
      else if(!SplitKPart(SD,LSD,Spine,iedge,HS1,It1,HS2,It2,intf,intl)){
#ifdef OCCT_DEBUG
	cout<<"failed calculation KPart"<<endl;
#endif
	LSD.Clear();
      }
      else iedgelastkpart = iedge;
      if(Spine->IsPeriodic()){//debug provisory for SD that arrive in desorder.
	Standard_Integer nbsd = LSD.Length();
	Standard_Real period = Spine->Period();
	Standard_Real wfp = WStartPeriodic, wlp = WEndPeriodic;
//  modified by NIZHNY-EAP Thu Nov 25 12:57:53 1999 ___BEGIN___
	if(!YaKPart && nbsd>0){
//	if(!YaKPart){
//  modified by NIZHNY-EAP Thu Nov 25 12:57:57 1999 ___END___
	  Handle(ChFiDS_SurfData) firstSD = LSD.ChangeValue(1);
	  Standard_Real wwf = firstSD->FirstSpineParam();
	  Standard_Real wwl = firstSD->LastSpineParam();
	  wwf = ChFi3d_InPeriod(wwf,wfp,wlp,tolesp);
	  wwl = ChFi3d_InPeriod(wwl,wfp,wlp,tolesp);
	  if (wwl <= wwf + tolesp) wwl += period;
	  wfp = wwf;
	  wlp = wfp + period;
	}
	for(Standard_Integer j = 1; j < nbsd; j++){
	  Handle(ChFiDS_SurfData) jSD = LSD.Value(j);
	  for(Standard_Integer k = j+1; k <= nbsd; k++){
	    Handle(ChFiDS_SurfData) kSD = LSD.Value(k);
	    Standard_Real jwf = jSD->FirstSpineParam();
	    jwf = ChFi3d_InPeriod(jwf,wfp,wlp,tolesp);
	    Standard_Real kwf = kSD->FirstSpineParam();
	    kwf = ChFi3d_InPeriod(kwf,wfp,wlp,tolesp);
	    if(kwf < jwf){
	      LSD.SetValue(j,kSD);
	      LSD.SetValue(k,jSD);
	    }
	  }
	}
      }
      TColStd_ListOfInteger li;
      for(Standard_Integer j = 1; j <= LSD.Length(); j++){
	Handle(ChFiDS_SurfData)& curSD = LSD.ChangeValue(j);
	if(Simul) SimulKPart(curSD);
	SeqSurf.Append(curSD);
	if(!Simul) li.Append(curSD->Surf());
	WFirst = LSD.Value(j)->FirstSpineParam();
	WLast  = LSD.Value(j)->LastSpineParam();
	if(Spine->IsPeriodic()){
	  WFirst = ChFi3d_InPeriod(WFirst,WStartPeriodic,WEndPeriodic,tolesp);
	  WLast  = ChFi3d_InPeriod(WLast ,WStartPeriodic,WEndPeriodic,tolesp);
	  if (WLast <= WFirst + tolesp) WLast+= Spine->Period();
	}
	TgtKP(LSD.Value(j),Spine,iedge,1,PFirst,TFirst);
	TgtKP(LSD.Value(j),Spine,iedge,0,PLast,TLast);
	
	// Determine the sections to approximate
	if(!YaKPart){
	  if(Spine->IsPeriodic()){
	    WStartPeriodic = WFirst;
	    WEndPeriodic = WStartPeriodic + Spine->Period();
	    WLast = ElCLib::InPeriod(WLast,WStartPeriodic,WEndPeriodic);
	    if (WLast <= WFirst + tolesp) WLast+= Spine->Period();
	    PEndPeriodic = PFirst;
	    TEndPeriodic = TFirst;
	    Spine->SetFirstParameter(WStartPeriodic);
	    Spine->SetLastParameter(WEndPeriodic);
	  }
	  else if(!intf || (iedge > 1)){
	    // start section -> first KPart
	    // update of extension.
	    Spine->SetFirstTgt(Min(0.,WFirst));
	    CurrentHE->ChangeCurve().LastParameter (WFirst);
	    CurrentHE->ChangeCurve().SetLastPointAndTgt(PFirst,TFirst);
	    Spine->AppendElSpine(CurrentHE);
	    CurrentHE->ChangeCurve().ChangeNext() = LSD.Value(j);
	    CurrentHE =  new ChFiDS_HElSpine();
	  }
	  CurrentHE->ChangeCurve().FirstParameter(WLast);
	  CurrentHE->ChangeCurve().SetFirstPointAndTgt(PLast,TLast);
	  CurrentHE->ChangeCurve().ChangePrevious() = LSD.Value(j);
	  YaKPart = Standard_True;
	}
	else {
	  if (WFirst - CurrentHE->FirstParameter() > tolesp) {
	    // section between two KPart
	    CurrentHE->ChangeCurve().LastParameter(WFirst);
	    CurrentHE->ChangeCurve().SetLastPointAndTgt(PFirst,TFirst);
	    Spine->AppendElSpine(CurrentHE);
	    CurrentHE->ChangeCurve().ChangeNext() = LSD.Value(j);
	    CurrentHE = new ChFiDS_HElSpine();
	  }
	  CurrentHE->ChangeCurve().FirstParameter(WLast);
	  CurrentHE->ChangeCurve().SetFirstPointAndTgt(PLast,TLast);
	  CurrentHE->ChangeCurve().ChangePrevious() = LSD.Value(j);
	}
      }
      if(!li.IsEmpty()) myEVIMap.Bind(Spine->Edges(iedge),li);
    }
  }
  
  if (!intl || (iedgelastkpart < Spine->NbEdges())) {
    // section last KPart(or start of the spine) -> End of the spine.
    // update of the extension.
    
    if(Spine->IsPeriodic()){
      if(WEndPeriodic - WLast > tolesp){
	CurrentHE->ChangeCurve().LastParameter(WEndPeriodic);
	CurrentHE->ChangeCurve().SetLastPointAndTgt(PEndPeriodic,TEndPeriodic);
	if(!YaKPart) CurrentHE->ChangeCurve().SetPeriodic(Standard_True);
	Spine->AppendElSpine(CurrentHE);
      }
    }
    else{
      Spine->D1(Spine->LastParameter(),PLast,TLast);
      Spine->SetLastTgt(Max(Spine->LastParameter(Spine->NbEdges()),
			    WLast));
      if (Spine->LastParameter() - WLast > tolesp) {
	CurrentHE->ChangeCurve().LastParameter(Spine->LastParameter());
	CurrentHE->ChangeCurve().SetLastPointAndTgt(PLast,TLast);
	Spine->AppendElSpine(CurrentHE);
      }
    }
  }
  
  ChFiDS_ListOfHElSpine& ll = Spine->ChangeElSpines();
  ChFiDS_ListIteratorOfListOfHElSpine ILES(ll);
  for ( ; ILES.More(); ILES.Next()) {
#ifdef OCCT_DEBUG
    if(ChFi3d_GettraceCHRON()) elspine.Start();
#endif
    ChFi3d_PerformElSpine(ILES.Value(),Spine,myConti,tolesp);
#ifdef OCCT_DEBUG
    if(ChFi3d_GettraceCHRON()) { elspine.Stop(); }
#endif
  }
  Spine->SplitDone(Standard_True);
}

static Standard_Real ChFi3d_BoxDiag(const Bnd_Box& box)
{
  Standard_Real a,b,c,d,e,f;
  box.Get(a,b,c,d,e,f); 
  d-=a; e-=b; f-=c; 
  d*=d; e*=e; f*=f;
  Standard_Real diag = sqrt(d + e + f);
  return diag;
}

//=======================================================================
//function : PerformSetOfKGen
//purpose  : 
//=======================================================================

void ChFi3d_Builder::PerformSetOfKGen(Handle(ChFiDS_Stripe)& Stripe,
				      const Standard_Boolean Simul) 
{
  Handle(BRepTopAdaptor_TopolTool) It1 = new BRepTopAdaptor_TopolTool();
  Handle(BRepTopAdaptor_TopolTool) It2 = new BRepTopAdaptor_TopolTool();
  Handle(ChFiDS_Spine)& Spine = Stripe->ChangeSpine();
  ChFiDS_ListOfHElSpine& ll = Spine->ChangeElSpines();
  ChFiDS_ListIteratorOfListOfHElSpine ILES(ll);
  for ( ; ILES.More(); ILES.Next()) {
#ifdef OCCT_DEBUG
    if(ChFi3d_GettraceCHRON()) { chemine.Start(); }
#endif
    PerformSetOfSurfOnElSpine(ILES.Value(),Stripe,It1,It2,Simul);
#ifdef OCCT_DEBUG
    if(ChFi3d_GettraceCHRON()) chemine.Stop();
#endif
  }
  if(!Simul){
    TopOpeBRepDS_DataStructure& DStr = myDS->ChangeDS();
    Handle(ChFiDS_HData)&  HData  = Stripe->ChangeSetOfSurfData();
    ChFiDS_SequenceOfSurfData& SeqSurf = HData->ChangeSequence();
    Standard_Integer len = SeqSurf.Length();
    Standard_Integer last = len, i;
    Standard_Boolean periodic = Spine->IsPeriodic();
    if(periodic) last++;
    // It is attempted to reprocess the squares that bore.
    for(i = 1; i <= len; i++){
      Handle(ChFiDS_SurfData)& cursd = SeqSurf.ChangeValue(i);
      Standard_Boolean tw1 = cursd->TwistOnS1();
      Standard_Boolean tw2 = cursd->TwistOnS2();
      Handle(ChFiDS_SurfData) prevsd, nextsd;
      Standard_Integer iprev = i-1;
      if(iprev == 0) {
	if(periodic) iprev = len;
      }
      Standard_Integer inext = i + 1;
      if(inext > len) {
	if(periodic) inext = 1;
	else inext = 0;
      }

      // For the moment only the surfaces where the twist is 
      // detected at the path are corrected, it is necessary to control  
      // more subtly the ugly traces (size, curvature, inflexion... ) 
      if(!tw1 && !tw2) continue;
      
      // It is decided (fairly at random) if the extended surface is ready for the filling.
      ChFiDS_FaceInterference& intf1 = cursd->ChangeInterferenceOnS1();
      ChFiDS_FaceInterference& intf2 = cursd->ChangeInterferenceOnS2();
      Standard_Integer cursurf1 = cursd->IndexOfS1();
      Standard_Integer cursurf2 = cursd->IndexOfS2();
      ChFiDS_CommonPoint& cpd1 = cursd->ChangeVertexFirstOnS1();
      ChFiDS_CommonPoint& cpd2 = cursd->ChangeVertexFirstOnS2();
      ChFiDS_CommonPoint& cpf1 = cursd->ChangeVertexLastOnS1();
      ChFiDS_CommonPoint& cpf2 = cursd->ChangeVertexLastOnS2();
      const gp_Pnt& pd1 = cpd1.Point();
      const gp_Pnt& pd2 = cpd2.Point();
      const gp_Pnt& pf1 = cpf1.Point();
      const gp_Pnt& pf2 = cpf2.Point();
      Standard_Real ddeb = pd1.Distance(pd2);
      Standard_Real dfin = pf1.Distance(pf2);
      Standard_Real don1 = pd1.Distance(pf1);
      Standard_Real don2 = pd2.Distance(pf2);
      Standard_Boolean possibleon1 = (don1 < 2*(ddeb + dfin));
      Standard_Boolean possibleon2 = (don2 < 2*(ddeb + dfin));
      if((tw1 && !possibleon1) || (tw2 && !possibleon2)) {
        Spine->SetErrorStatus(ChFiDS_TwistedSurface);
	throw Standard_Failure("adjustment by reprocessing the non-written points");
      }
      
      // It is checked if there are presentable neighbors
      Standard_Boolean yaprevon1 = 0, yaprevon2 = 0;
      Standard_Boolean samesurfon1 = 0, samesurfon2 = 0;
      if(iprev){
	prevsd = SeqSurf.ChangeValue(iprev);
	yaprevon1 = !prevsd->TwistOnS1();
	samesurfon1 = (prevsd->IndexOfS1() == cursurf1);
	yaprevon2 = !prevsd->TwistOnS2();
	samesurfon2 = (prevsd->IndexOfS2() == cursurf2);
      }
      Standard_Boolean yanexton1 = 0, yanexton2 = 0;
      if(inext){
	nextsd = SeqSurf.ChangeValue(inext);
	yanexton1 = !nextsd->TwistOnS1();
	if(samesurfon1) samesurfon1 = (nextsd->IndexOfS1() == cursurf1);
	yanexton2 = !nextsd->TwistOnS2();
	if(samesurfon2) samesurfon2 = (nextsd->IndexOfS2() == cursurf2);
      }
      // A contour of filling is constructed
      Handle(Geom2d_Curve) PC1 = intf1.PCurveOnFace();
      Handle(Geom2d_Curve) PC2 = intf2.PCurveOnFace();
      Handle(BRepAdaptor_HSurface) S1 = new BRepAdaptor_HSurface();
      TopoDS_Face F1 = TopoDS::Face(DStr.Shape(cursurf1));
      S1->ChangeSurface().Initialize(F1);
      Handle(BRepAdaptor_HSurface) S2 = new BRepAdaptor_HSurface();
      TopoDS_Face F2 = TopoDS::Face(DStr.Shape(cursurf2));
      S2->ChangeSurface().Initialize(F2);
      Handle(GeomFill_Boundary) Bdeb,Bfin,Bon1,Bon2;
      Standard_Boolean pointuon1 = 0, pointuon2 = 0;
      if(tw1){
	if(!yaprevon1 || !yanexton1){
          Spine->SetErrorStatus(ChFiDS_TwistedSurface);
	  throw Standard_Failure("adjustment by reprocessing the non-written points: no neighbor");
	}
	ChFiDS_FaceInterference& previntf1 = prevsd->ChangeInterferenceOnS1();
	ChFiDS_FaceInterference& nextintf1 = nextsd->ChangeInterferenceOnS1();
	Standard_Real prevpar1 = previntf1.LastParameter();
	Standard_Real nextpar1 = nextintf1.FirstParameter();
	if(samesurfon1){
	  // It is checked if it is possible to intersect traces of neighbors
	  // to create a sharp end.
	  Handle(Geom2d_Curve) pcprev1 = previntf1.PCurveOnFace();
	  Handle(Geom2d_Curve) pcnext1 = nextintf1.PCurveOnFace();
	  Standard_Real nprevpar1,nnextpar1;
	  gp_Pnt2d p2d;
//  Modified by Sergey KHROMOV - Wed Feb  5 12:03:17 2003 Begin
// 	  if(ChFi3d_IntTraces(prevsd,prevpar1,nprevpar1,1,1,
// 			      nextsd,nextpar1,nnextpar1,1,-1,p2d)){
	  if(ChFi3d_IntTraces(prevsd,prevpar1,nprevpar1,1,1,
			      nextsd,nextpar1,nnextpar1,1,-1,p2d,
			      Standard_False, Standard_True)){
//  Modified by Sergey KHROMOV - Wed Feb  5 12:03:17 2003 End
	    previntf1.SetLastParameter(nprevpar1);
	    nextintf1.SetFirstParameter(nnextpar1);
	    pointuon1 = 1;
	    PC1.Nullify();
	  }
	  else{
	    gp_Pnt2d pdeb1,pfin1;
	    gp_Vec2d vdeb1,vfin1;
	    pcprev1->D1(prevpar1,pdeb1,vdeb1);
	    pcnext1->D1(nextpar1,pfin1,vfin1);
	    Bon1 = ChFi3d_mkbound(S1,PC1,-1,pdeb1,vdeb1,1,
				  pfin1,vfin1,tolesp,2.e-4);
	  }
	}
	else{
	  //here the base is on 3D tangents of neighbors.
	  const Handle(Geom_Curve)& c3dprev1 = 
	    DStr.Curve(previntf1.LineIndex()).Curve();
	  const Handle(Geom_Curve)& c3dnext1 = 
	    DStr.Curve(nextintf1.LineIndex()).Curve();
	  gp_Pnt Pdeb1, Pfin1;
	  gp_Vec Vdeb1, Vfin1;
	  c3dprev1->D1(prevpar1,Pdeb1,Vdeb1);
	  c3dnext1->D1(nextpar1,Pfin1,Vfin1);
	  gp_Pnt2d pdeb1,pfin1;
	  Standard_Real pardeb1 = intf1.FirstParameter();
	  Standard_Real parfin1 = intf1.LastParameter();
	  pdeb1 = PC1->Value(pardeb1);
	  pfin1 = PC1->Value(parfin1);
	  Bon1 = ChFi3d_mkbound(S1,PC1,-1,pdeb1,Vdeb1,1,
				pfin1,Vfin1,tolesp,2.e-4);
	}
      }
      else{
	Bon1 = ChFi3d_mkbound(S1,PC1,tolesp,2.e-4);
      }
      if(tw2){
	if(!yaprevon2 || !yanexton2){
	  throw Standard_Failure("adjustment by reprocessing the non-written points: no neighbor");
	}
	ChFiDS_FaceInterference& previntf2 = prevsd->ChangeInterferenceOnS2();
	ChFiDS_FaceInterference& nextintf2 = nextsd->ChangeInterferenceOnS2();
	Standard_Real prevpar2 = previntf2.LastParameter();
	Standard_Real nextpar2 = nextintf2.FirstParameter();
	if(samesurfon2){
	  // It is checked if it is possible to intersect traces of neighbors
	  // to create a sharp end.
	  Handle(Geom2d_Curve) pcprev2 = previntf2.PCurveOnFace();
	  Handle(Geom2d_Curve) pcnext2 = nextintf2.PCurveOnFace();
	  Standard_Real nprevpar2,nnextpar2;
	  gp_Pnt2d p2d;
//  Modified by Sergey KHROMOV - Wed Feb  5 12:03:17 2003 Begin
// 	  if(ChFi3d_IntTraces(prevsd,prevpar2,nprevpar2,2,1,
// 			      nextsd,nextpar2,nnextpar2,2,-1,p2d)){
	  if(ChFi3d_IntTraces(prevsd,prevpar2,nprevpar2,2,1,
			      nextsd,nextpar2,nnextpar2,2,-1,p2d,
			      Standard_False, Standard_True)){
//  Modified by Sergey KHROMOV - Wed Feb  5 12:03:17 2003 End
	    previntf2.SetLastParameter(nprevpar2);
	    nextintf2.SetFirstParameter(nnextpar2);
	    pointuon2 = 1;
	    PC2.Nullify();
	  }
	  else{
	    gp_Pnt2d pdeb2,pfin2;
	    gp_Vec2d vdeb2,vfin2;
	    pcprev2->D1(prevpar2,pdeb2,vdeb2);
	    pcnext2->D1(nextpar2,pfin2,vfin2);
	    Bon2 = ChFi3d_mkbound(S2,PC2,-1,pdeb2,vdeb2,1,
				  pfin2,vfin2,tolesp,2.e-4);
	  }
	}
	else{
	 //here the base is on 3D tangents of neighbors.
	  const Handle(Geom_Curve)& c3dprev2 = 
	    DStr.Curve(previntf2.LineIndex()).Curve();
	  const Handle(Geom_Curve)& c3dnext2 = 
	    DStr.Curve(nextintf2.LineIndex()).Curve();
	  gp_Pnt Pdeb2, Pfin2;
	  gp_Vec Vdeb2, Vfin2;
	  c3dprev2->D1(prevpar2,Pdeb2,Vdeb2);
	  c3dnext2->D1(nextpar2,Pfin2,Vfin2);
	  gp_Pnt2d pdeb2,pfin2;
	  Standard_Real pardeb2 = intf2.FirstParameter();
	  Standard_Real parfin2 = intf2.LastParameter();
	  pdeb2 = PC2->Value(pardeb2);
	  pfin2 = PC2->Value(parfin2);
	  Bon2 = ChFi3d_mkbound(S2,PC2,-1,pdeb2,Vdeb2,1,
				pfin2,Vfin2,tolesp,2.e-4);
	}
      }
      else{
	Bon2 = ChFi3d_mkbound(S2,PC2,tolesp,2.e-4);
      }
      // The parameters of neighbor traces are updated, so 
      // straight lines uv are pulled.
      const Handle(Geom_Surface)& 
	sprev = DStr.Surface(prevsd->Surf()).Surface();
      const Handle(Geom_Surface)& 
	snext = DStr.Surface(nextsd->Surf()).Surface();
      ChFiDS_FaceInterference& previntf1 = prevsd->ChangeInterferenceOnS1();
      ChFiDS_FaceInterference& nextintf1 = nextsd->ChangeInterferenceOnS1();
      ChFiDS_FaceInterference& previntf2 = prevsd->ChangeInterferenceOnS2();
      ChFiDS_FaceInterference& nextintf2 = nextsd->ChangeInterferenceOnS2();
      Handle(Geom2d_Curve) pcsprev1 = previntf1.PCurveOnSurf();
      Handle(Geom2d_Curve) pcsnext1 = nextintf1.PCurveOnSurf();
      Standard_Real prevpar1 = previntf1.LastParameter();
      Standard_Real nextpar1 = nextintf1.FirstParameter();
      Handle(Geom2d_Curve) pcsprev2 = previntf2.PCurveOnSurf();
      Handle(Geom2d_Curve) pcsnext2 = nextintf2.PCurveOnSurf();
      Standard_Real prevpar2 = previntf2.LastParameter();
      Standard_Real nextpar2 = nextintf2.FirstParameter();
      gp_Pnt2d pdebs1 = pcsprev1->Value(prevpar1);
      gp_Pnt2d pdebs2 = pcsprev2->Value(prevpar2);
      gp_Pnt2d pfins1 = pcsnext1->Value(nextpar1);
      gp_Pnt2d pfins2 = pcsnext2->Value(nextpar2);
      Bdeb = ChFi3d_mkbound(sprev,pdebs1,pdebs2,tolesp,2.e-4);
      Bfin = ChFi3d_mkbound(snext,pfins1,pfins2,tolesp,2.e-4);

      GeomFill_ConstrainedFilling fil(11,20);
      if(pointuon1) fil.Init(Bon2,Bfin,Bdeb,1);
      else if(pointuon2) fil.Init(Bon1,Bfin,Bdeb,1);
      else fil.Init(Bon1,Bfin,Bon2,Bdeb,1);
      
      ChFi3d_ReparamPcurv(0.,1.,PC1);
      ChFi3d_ReparamPcurv(0.,1.,PC2);
      Handle(Geom_Surface) newsurf = fil.Surface();
#ifdef OCCT_DEBUG
#ifdef DRAW
      //POP for NT
      char* pops = "newsurf";
      DrawTrSurf::Set(pops,newsurf);
#endif
#endif
      if(pointuon1) {
	newsurf->VReverse(); // we return to direction 1 from  2;
	done = CompleteData(cursd,newsurf,S1,PC1,S2,PC2,
			    F2.Orientation(),0,0,0,0,0);
	cursd->ChangeIndexOfS1(0);
      }
      else{
	done = CompleteData(cursd,newsurf,S1,PC1,S2,PC2,
			    F1.Orientation(),1,0,0,0,0);
	if(pointuon2) cursd->ChangeIndexOfS2(0);
      }
      if(tw1){
	prevsd->ChangeVertexLastOnS1().SetPoint(cpd1.Point());
	nextsd->ChangeVertexFirstOnS1().SetPoint(cpf1.Point());
      }
      if(tw2){
	prevsd->ChangeVertexLastOnS2().SetPoint(cpd2.Point());
	nextsd->ChangeVertexFirstOnS2().SetPoint(cpf2.Point());
      }
    }
    // The tolerance of points is updated.
    for(i = 1; i < last; i++){
      Standard_Integer j = i%len + 1;
      Standard_Integer curs1, curs2;
      Standard_Integer nexts1, nexts2;
      Handle(ChFiDS_SurfData)& cursd = SeqSurf.ChangeValue(i);
      Handle(ChFiDS_SurfData)& nextsd = SeqSurf.ChangeValue(j);
      ChFiDS_CommonPoint& curp1 = cursd->ChangeVertexLastOnS1();
      ChFiDS_CommonPoint& nextp1 = nextsd->ChangeVertexFirstOnS1();
      if (cursd->IsOnCurve1()) curs1 = cursd->IndexOfC1();
      else                     curs1 = cursd->IndexOfS1();
      if (cursd->IsOnCurve2()) curs2 = cursd->IndexOfC2();
      else                     curs2 = cursd->IndexOfS2();
      Standard_Real tol1 = Max(curp1.Tolerance(),nextp1.Tolerance());
      ChFiDS_CommonPoint& curp2 = cursd->ChangeVertexLastOnS2();
      ChFiDS_CommonPoint& nextp2 = nextsd->ChangeVertexFirstOnS2();
      Standard_Real tol2 = Max(curp2.Tolerance(),nextp2.Tolerance());
      if (nextsd->IsOnCurve1()) nexts1 = nextsd->IndexOfC1();
      else                     nexts1 = nextsd->IndexOfS1();
      if (nextsd->IsOnCurve2()) nexts2 = nextsd->IndexOfC2();
      else                     nexts2 = nextsd->IndexOfS2();

      if(!curp1.IsOnArc() && nextp1.IsOnArc()){ 
	curp1 = nextp1;
	if ( (curs1 == nexts1) && !nextsd->IsOnCurve1()) 
	  // Case when it is not possible to pass along the border without leaving
	  ChangeTransition(nextp1, curp1, nexts1, myDS);
      }
      else if(curp1.IsOnArc() && !nextp1.IsOnArc()) { 
	nextp1 = curp1;
	if ( (curs1 == nexts1) && !cursd->IsOnCurve1())
	  ChangeTransition(curp1, nextp1, curs1, myDS);
      }
	
      if(!curp2.IsOnArc() && nextp2.IsOnArc()) {
	curp2 = nextp2;
	if ( (curs2 == nexts2) && !nextsd->IsOnCurve2()) 
	  ChangeTransition(nextp2, curp2, curs2, myDS);
      }
      else if(curp2.IsOnArc() && !nextp2.IsOnArc()){
	nextp2 = curp2;
	if ( (curs2 == nexts2) && !cursd->IsOnCurve2()) 
	  ChangeTransition(curp2, nextp2, curs2, myDS);
      }

      curp1.SetTolerance(tol1); nextp1.SetTolerance(tol1); 
      curp2.SetTolerance(tol2); nextp2.SetTolerance(tol2); 
      
      Bnd_Box b1,b2;
      if(curp1.IsOnArc()){
	ChFi3d_EnlargeBox(curp1.Arc(),myEFMap(curp1.Arc()),curp1.ParameterOnArc(),b1);
      }
      if(curp2.IsOnArc()){
	ChFi3d_EnlargeBox(curp2.Arc(),myEFMap(curp2.Arc()),curp2.ParameterOnArc(),b2);
      }
      Handle(ChFiDS_Stripe) bidst;
      ChFi3d_EnlargeBox(DStr,bidst,cursd,b1,b2,0);
      ChFi3d_EnlargeBox(DStr,bidst,nextsd,b1,b2,1);
      tol1 = ChFi3d_BoxDiag(b1);
      tol2 = ChFi3d_BoxDiag(b2);
      curp1.SetTolerance(tol1); nextp1.SetTolerance(tol1); 
      curp2.SetTolerance(tol2); nextp2.SetTolerance(tol2); 
    }
    // The connections edge/new faces are updated.
    for (ILES.Initialize(ll) ; ILES.More(); ILES.Next()) {
      const Handle(ChFiDS_HElSpine)& curhels = ILES.Value();
      const ChFiDS_ElSpine& curels = curhels->ChangeCurve();
      Standard_Real WF = curels.FirstParameter();
      Standard_Real WL = curels.LastParameter();
      Standard_Integer IF,IL;
      Standard_Real nwf = WF, nwl = WL;
      Standard_Real period = 0.;
      Standard_Integer nbed = Spine->NbEdges();
      if(periodic){
	period = Spine->Period();
	nwf = ElCLib::InPeriod(WF,-tolesp,period-tolesp);
	IF = Spine->Index(nwf,1);
	nwl = ElCLib::InPeriod(WL,tolesp,period+tolesp);
	IL = Spine->Index(nwl,0);
	if(nwl<nwf+tolesp) IL += nbed;
      }
      else{
	IF = Spine->Index(WF,1);
	IL = Spine->Index(WL,0);
      }
      if(IF == IL) {
	//fast processing
	Standard_Integer IFloc = IF;
	if(periodic) IFloc = (IF - 1)%nbed + 1;
	const TopoDS_Edge& Ej = Spine->Edges(IFloc);
	for(i = 1; i <= len; i++){
	  Handle(ChFiDS_SurfData)& cursd = SeqSurf.ChangeValue(i);
	  Standard_Real fp = cursd->FirstSpineParam();
	  Standard_Real lp = cursd->LastSpineParam();
	  if(lp < WF+tolesp || fp > WL-tolesp) continue;
	  if(!myEVIMap.IsBound(Ej)) {
	    TColStd_ListOfInteger li;
	    myEVIMap.Bind(Ej,li);
	  }
	  myEVIMap.ChangeFind(Ej).Append(cursd->Surf());
	}
      }
      else if(IF < IL){
	TColStd_Array1OfReal wv(IF,IL - 1);
#ifdef OCCT_DEBUG
	cout<<"length of the trajectory : "<<(WL-WF)<<endl;
#endif
	for(i = IF; i < IL; i++){
	  Standard_Integer iloc = i;
	  if(periodic) iloc = (i - 1)%nbed + 1;
	  Standard_Real wi = Spine->LastParameter(iloc);
	  if(periodic) wi = ElCLib::InPeriod(wi,WF,WF+period);
	  gp_Pnt pv = Spine->Value(wi);
#ifdef OCCT_DEBUG
	  gp_Pnt pelsapp = curels.Value(wi);
	  Standard_Real distinit = pv.Distance(pelsapp);
	  cout<<"distance psp/papp : "<<distinit<<endl;
#endif
	  Extrema_LocateExtPC ext(pv,curels,wi,1.e-8);
	  wv(i) = wi;
	  if(ext.IsDone()){
	    wv(i) = ext.Point().Parameter(); 
	  }
	  else {
#ifdef OCCT_DEBUG
	    cout<<"fail of projection vertex ElSpine!!!"<<endl;
#endif
	  }
	}
	for(i = 1; i <= len; i++){
	  Handle(ChFiDS_SurfData)& cursd = SeqSurf.ChangeValue(i);
	  Standard_Real fp = cursd->FirstSpineParam();
	  Standard_Real lp = cursd->LastSpineParam();
	  Standard_Integer j;
	  Standard_Integer jf = 0, jl = 0;
	  if(lp < WF+tolesp || fp > WL-tolesp) continue;
	  for(j = IF; j < IL; j++){
	    jf = j;
	    if(fp < wv(j) - tolesp) break;
	  }
	  for(j = IF; j < IL; j++){
	    jl = j;
	    if(lp < wv(j) + tolesp) break;
	  }
	  for(j = jf; j <= jl; j++){
	    Standard_Integer jloc = j;
	    if(periodic) jloc = (j - 1)%nbed + 1;
	    const TopoDS_Edge& Ej = Spine->Edges(jloc);
	    if(!myEVIMap.IsBound(Ej)) {
	      TColStd_ListOfInteger li;
	      myEVIMap.Bind(Ej,li);
	    }
	    myEVIMap.ChangeFind(Ej).Append(cursd->Surf());
	  }
	}
      }
    }
  }
}

//=======================================================================
//function : PerformSetOfSurf
//purpose  : 
//=======================================================================

void ChFi3d_Builder::PerformSetOfSurf(Handle(ChFiDS_Stripe)& Stripe,
				      const Standard_Boolean Simul) 
{
  TopOpeBRepDS_DataStructure& DStr = myDS->ChangeDS();
  
#ifdef OCCT_DEBUG
  OSD_Chronometer ch;
  ChFi3d_InitChron(ch);// init perf for PerformSetOfKPart
#endif
  
  const Handle(ChFiDS_Spine)& sp = Stripe->Spine();
  Standard_Integer SI = ChFi3d_SolidIndex(sp,DStr,myESoMap,myEShMap);
  Stripe->SetSolidIndex(SI);
  if(!sp->SplitDone()) PerformSetOfKPart(Stripe,Simul);
  
#ifdef OCCT_DEBUG
  ChFi3d_ResultChron(ch ,t_perfsetofkpart); // result perf PerformSetOfKPart(
  ChFi3d_InitChron(ch); // init perf for  PerformSetOfKGen
#endif
  
  PerformSetOfKGen(Stripe,Simul);
  
#ifdef OCCT_DEBUG
  ChFi3d_ResultChron(ch, t_perfsetofkgen);//result perf PerformSetOfKGen 
  ChFi3d_InitChron(ch); // init perf for ChFi3d_MakeExtremities
#endif
  
  if(!Simul) ChFi3d_MakeExtremities(Stripe,DStr,myEFMap,tolesp,tol2d);
  
#ifdef OCCT_DEBUG
  ChFi3d_ResultChron(ch, t_makextremities); // result perf t_makextremities
#endif
}
