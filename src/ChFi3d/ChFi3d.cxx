// Created on: 1993-12-21
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


#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <ChFi3d.hxx>
#include <ChFi3d_Builder_0.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <BRepTools.hxx>
#include <IntTools_Tools.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <LocalAnalysis_SurfaceContinuity.hxx>
#include <TopOpeBRepTool_TOOL.hxx>


static void Correct2dPoint(const TopoDS_Face& theF, gp_Pnt2d& theP2d);
//

//=======================================================================
//function : DefineConnectType
//purpose  : 
//=======================================================================
ChFiDS_TypeOfConcavity ChFi3d::DefineConnectType(const TopoDS_Edge&     E,
                                                 const TopoDS_Face&     F1,
                                                 const TopoDS_Face&     F2,
                                                 const Standard_Real    SinTol,
                                                 const Standard_Boolean CorrectPoint)
{
  const Handle(Geom_Surface)& S1 = BRep_Tool::Surface(F1);
  const Handle(Geom_Surface)& S2 = BRep_Tool::Surface(F2);
  //
  Standard_Real   f,l;
  Handle (Geom2d_Curve) C1 = BRep_Tool::CurveOnSurface(E,F1,f,l);
  //For the case of seam edge
  TopoDS_Edge EE = E;
  if (F1.IsSame(F2))
    EE.Reverse();
  Handle (Geom2d_Curve) C2 = BRep_Tool::CurveOnSurface(EE,F2,f,l);
  if (C1.IsNull() || C2.IsNull())
    return ChFiDS_Other;

  BRepAdaptor_Curve C(E);
  f = C.FirstParameter();
  l = C.LastParameter();
//
  Standard_Real ParOnC = 0.5*(f+l);
  gp_Vec T1 = C.DN(ParOnC,1);
  if (T1.SquareMagnitude() <= gp::Resolution())
  {
    ParOnC = IntTools_Tools::IntermediatePoint(f,l);
    T1 = C.DN(ParOnC,1);
  }
  if (T1.SquareMagnitude() > gp::Resolution()) {
    T1.Normalize();
  }
  
  if (BRepTools::OriEdgeInFace(E,F1) == TopAbs_REVERSED) {
    T1.Reverse();
  }
  if (F1.Orientation() == TopAbs_REVERSED) T1.Reverse();

  gp_Pnt2d P  = C1->Value(ParOnC);
  gp_Pnt   P3;
  gp_Vec   D1U,D1V;
  
  if(CorrectPoint) 
    Correct2dPoint(F1, P);
  //
  S1->D1(P.X(),P.Y(),P3,D1U,D1V);
  gp_Vec DN1(D1U^D1V);
  if (F1.Orientation() == TopAbs_REVERSED) DN1.Reverse();
  
  P = C2->Value(ParOnC);
  if(CorrectPoint) 
    Correct2dPoint(F2, P);
  S2->D1(P.X(),P.Y(),P3,D1U,D1V);
  gp_Vec DN2(D1U^D1V);
  if (F2.Orientation() == TopAbs_REVERSED) DN2.Reverse();

  DN1.Normalize();
  DN2.Normalize();

  gp_Vec        ProVec     = DN1^DN2;
  Standard_Real NormProVec = ProVec.Magnitude(); 
  if (NormProVec < SinTol) {
    // plane
    if (DN1.Dot(DN2) > 0) {   
      //Tangent
      return ChFiDS_Tangential;
    }
    else  {                   
      //Mixed not finished!
#ifdef OCCT_DEBUG
      std::cout <<" faces locally mixed"<<std::endl;
#endif
      return ChFiDS_Convex;
    }
  }
  else {  
    if (NormProVec > gp::Resolution())
      ProVec /= NormProVec;
    Standard_Real Prod  = T1.Dot(ProVec);
    if (Prod > 0.) {       
      //
      return ChFiDS_Convex;
    }
    else {                       
      //reenters
      return ChFiDS_Concave;
    }
  }
}

//=======================================================================
//function : IsTangentFaces
//purpose  : 
//=======================================================================
Standard_Boolean ChFi3d::IsTangentFaces(const TopoDS_Edge& theEdge,
                                        const TopoDS_Face& theFace1,
                                        const TopoDS_Face& theFace2,
                                        const GeomAbs_Shape Order)
{
  if (Order == GeomAbs_G1 && BRep_Tool::Continuity(theEdge, theFace1, theFace2) != GeomAbs_C0)
    return Standard_True;

  Standard_Real TolC0 = Max(0.001, 1.5*BRep_Tool::Tolerance(theEdge));

  Standard_Real aFirst;
  Standard_Real aLast;

  // Obtaining of pcurves of edge on two faces.
  const Handle(Geom2d_Curve) aC2d1 = BRep_Tool::CurveOnSurface
    (theEdge, theFace1, aFirst, aLast);
  //For the case of seam edge
  TopoDS_Edge EE = theEdge;
  if (theFace1.IsSame(theFace2))
    EE.Reverse();
  const Handle(Geom2d_Curve) aC2d2 = BRep_Tool::CurveOnSurface
    (EE, theFace2, aFirst, aLast);
  if (aC2d1.IsNull() || aC2d2.IsNull())
    return Standard_False;

  // Obtaining of two surfaces from adjacent faces.
  Handle(Geom_Surface) aSurf1 = BRep_Tool::Surface(theFace1);
  Handle(Geom_Surface) aSurf2 = BRep_Tool::Surface(theFace2);

  if (aSurf1.IsNull() || aSurf2.IsNull())
    return Standard_False;

  // Computation of the number of samples on the edge.
  BRepAdaptor_Surface              aBAS1(theFace1);
  BRepAdaptor_Surface              aBAS2(theFace2);
  Handle(BRepAdaptor_HSurface)     aBAHS1 = new BRepAdaptor_HSurface(aBAS1);
  Handle(BRepAdaptor_HSurface)     aBAHS2 = new BRepAdaptor_HSurface(aBAS2);
  Handle(BRepTopAdaptor_TopolTool) aTool1 = new BRepTopAdaptor_TopolTool(aBAHS1);
  Handle(BRepTopAdaptor_TopolTool) aTool2 = new BRepTopAdaptor_TopolTool(aBAHS2);
  Standard_Integer                 aNbSamples1 = aTool1->NbSamples();
  Standard_Integer                 aNbSamples2 = aTool2->NbSamples();
  Standard_Integer                 aNbSamples = Max(aNbSamples1, aNbSamples2);

  // Computation of the continuity.
  Standard_Real    aPar;
  Standard_Real    aDelta = (aLast - aFirst) / (aNbSamples - 1);
  Standard_Integer i, nbNotDone = 0;

  for (i = 1, aPar = aFirst; i <= aNbSamples; i++, aPar += aDelta) {
    if (i == aNbSamples) aPar = aLast;

    LocalAnalysis_SurfaceContinuity aCont(aC2d1, aC2d2, aPar,
      aSurf1, aSurf2, Order,
      0.001, TolC0, 0.1, 0.1, 0.1);
    if (!aCont.IsDone())
    {
      nbNotDone++;
      continue;
    }

    if (Order == GeomAbs_G1)
    {
      if (!aCont.IsG1())
        return Standard_False;
    }
    else if (!aCont.IsG2())
      return Standard_False;
  }

  if (nbNotDone == aNbSamples)
    return Standard_False;

  //Compare normals of tangent faces in the middle point
  Standard_Real MidPar = (aFirst + aLast) / 2.;
  gp_Pnt2d uv1 = aC2d1->Value(MidPar);
  gp_Pnt2d uv2 = aC2d2->Value(MidPar);
  gp_Dir normal1, normal2;
  TopOpeBRepTool_TOOL::Nt(uv1, theFace1, normal1);
  TopOpeBRepTool_TOOL::Nt(uv2, theFace2, normal2);
  Standard_Real dot = normal1.Dot(normal2);
  if (dot < 0.)
    return Standard_False;
  return Standard_True;
}

//=======================================================================
//function : ConcaveSide
//purpose  : calculate the concave face at the neighborhood of the border of
//           2 faces.
//=======================================================================
Standard_Integer ChFi3d::ConcaveSide(const BRepAdaptor_Surface& S1, 
				     const BRepAdaptor_Surface& S2, 
				     const TopoDS_Edge& E, 
				     TopAbs_Orientation& Or1, 
				     TopAbs_Orientation& Or2)

{
  Standard_Integer ChoixConge;
  Or1 = Or2 = TopAbs_FORWARD;
  BRepAdaptor_Curve CE(E);
  Standard_Real first = CE.FirstParameter();
  Standard_Real last = CE.LastParameter();
  Standard_Real par = 0.691254*first + 0.308746*last;

  gp_Pnt pt, pt1, pt2; gp_Vec tgE, tgE1, tgE2, ns1, ns2, dint1, dint2;
  TopoDS_Face F1 = S1.Face();
  TopoDS_Face F2 = S2.Face();
  //F1.Orientation(TopAbs_FORWARD);
  //F2.Orientation(TopAbs_FORWARD);
  
  CE.D1(par,pt,tgE);
  tgE.Normalize();
  tgE2 = tgE1 = tgE;
  if(E.Orientation() == TopAbs_REVERSED) tgE.Reverse();

  TopoDS_Edge E1 = E, E2 = E;
  E1.Orientation(TopAbs_FORWARD);
  E2.Orientation(TopAbs_FORWARD);

  if(F1.IsSame(F2) && BRep_Tool::IsClosed(E,F1)) {
    E2.Orientation(TopAbs_REVERSED);
    tgE2.Reverse();
  }
  else {
    TopExp_Explorer Exp;
    Standard_Boolean found = 0;
    for (Exp.Init(F1,TopAbs_EDGE); 
	 Exp.More() && !found; 
	 Exp.Next()) {
      if (E.IsSame(TopoDS::Edge(Exp.Current()))){
	if(Exp.Current().Orientation() == TopAbs_REVERSED) tgE1.Reverse();
	found = Standard_True;
      }
    }
    if (!found) { return 0; }
    found = 0;
    for (Exp.Init(F2,TopAbs_EDGE); 
	 Exp.More() && !found; 
	 Exp.Next()) {
      if (E.IsSame(TopoDS::Edge(Exp.Current()))){
	if(Exp.Current().Orientation() == TopAbs_REVERSED) tgE2.Reverse();
	found = Standard_True;
      }
    }
    if (!found) { return 0; }
  }
  BRepAdaptor_Curve2d pc1(E1,F1);
  BRepAdaptor_Curve2d pc2(E2,F2);
  gp_Pnt2d p2d1,p2d2;
  gp_Vec DU1,DV1,DU2,DV2;
  p2d1 = pc1.Value(par);
  p2d2 = pc2.Value(par);
  S1.D1(p2d1.X(),p2d1.Y(),pt1,DU1,DV1);
  ns1 = DU1.Crossed(DV1);
  ns1.Normalize();
  if (F1.Orientation() == TopAbs_REVERSED)
    ns1.Reverse();
  S2.D1(p2d2.X(),p2d2.Y(),pt2,DU2,DV2);
  ns2 = DU2.Crossed(DV2);
  ns2.Normalize();
  if (F2.Orientation() == TopAbs_REVERSED)
    ns2.Reverse();

  dint1 = ns1.Crossed(tgE1);
  dint2 = ns2.Crossed(tgE2);
  Standard_Real ang = ns1.CrossMagnitude(ns2);
  if(ang > 0.0001*M_PI){
    Standard_Real scal = ns2.Dot(dint1);
    if ( scal <= 0. ){
      ns2.Reverse();
      Or2 = TopAbs_REVERSED; 
    }
    scal = ns1.Dot(dint2);
    if ( scal <= 0. ){
      ns1.Reverse();
      Or1 = TopAbs_REVERSED; 
    }
  }
  else { 
    //the faces are locally tangent - this is fake!
    if(dint1.Dot(dint2) < 0.){
      //This is a forgotten regularity
      gp_Vec DDU, DDV, DDUV;
      S1.D2(p2d1.X(),p2d1.Y(),pt1,DU1,DV1,DDU,DDV,DDUV);
      DU1 += ( DU1 * dint1 < 0) ? -DDU : DDU;
      DV1 += ( DV1 * dint1 < 0) ? -DDV : DDV;
      ns1 = DU1.Crossed(DV1);
      ns1.Normalize();
      if (F1.Orientation() == TopAbs_REVERSED)
        ns1.Reverse();
      S2.D2(p2d2.X(),p2d2.Y(),pt2,DU2,DV2,DDU,DDV,DDUV);
      DU2 += ( DU2 * dint2 < 0) ? -DDU : DDU;
      DV2 += ( DV2 * dint2 < 0) ? -DDV : DDV;
      ns2 = DU2.Crossed(DV2);
      ns2.Normalize();
      if (F2.Orientation() == TopAbs_REVERSED)
        ns2.Reverse();
      
      dint1 = ns1.Crossed(tgE1);
      dint2 = ns2.Crossed(tgE2);
      ang = ns1.CrossMagnitude(ns2);
      if(ang > 0.0001*M_PI){
        Standard_Real scal = ns2.Dot(dint1);
        if ( scal <= 0. ){
          ns2.Reverse();
          Or2 = TopAbs_REVERSED; 
        }
        scal = ns1.Dot(dint2);
        if ( scal <= 0. ){
          ns1.Reverse();
          Or1 = TopAbs_REVERSED; 
        }
      }
      else {
#ifdef OCCT_DEBUG
        std::cout<<"ConcaveSide : no concave face"<<std::endl;
#endif
	//This 10 shows that the face at end is in the extension of one of two base faces
	return 10;
      }
    }
    else {
      //here it turns back, the points are taken in faces
      //neither too close nor too far as much as possible.
      Standard_Real u,v;
#ifdef OCCT_DEBUG
//      Standard_Real deport = 1000*BRep_Tool::Tolerance(E);
#endif
      ChFi3d_Coefficient(dint1,DU1,DV1,u,v);
      p2d1.SetX(p2d1.X() + u); p2d1.SetY(p2d1.Y() + v);
      ChFi3d_Coefficient(dint1,DU2,DV2,u,v);
      p2d2.SetX(p2d2.X() + u); p2d2.SetY(p2d2.Y() + v);
      S1.D1(p2d1.X(),p2d1.Y(),pt1,DU1,DV1);
      ns1 = DU1.Crossed(DV1);
      if (F1.Orientation() == TopAbs_REVERSED)
        ns1.Reverse();
      S2.D1(p2d2.X(),p2d2.Y(),pt2,DU2,DV2);
      ns2 = DU2.Crossed(DV2);
      if (F2.Orientation() == TopAbs_REVERSED)
        ns2.Reverse();
      gp_Vec vref(pt1,pt2);
      if(ns1.Dot(vref) < 0.){
	Or1 = TopAbs_REVERSED;
      }
      if(ns2.Dot(vref) > 0.){
	Or2 = TopAbs_REVERSED;
      }
    }
  }


  if (Or1 == TopAbs_FORWARD) {
    if (Or2 == TopAbs_FORWARD) ChoixConge = 1;
    else ChoixConge = 7;
  }
  else {
    if (Or2 == TopAbs_FORWARD) ChoixConge = 3;
    else ChoixConge = 5;
  }
  if ((ns1.Crossed(ns2)).Dot(tgE) >= 0.) ChoixConge++ ;
  return ChoixConge;
}

//=======================================================================
//function : NextSide
//purpose  : 
//           
//=======================================================================

Standard_Integer  ChFi3d::NextSide(TopAbs_Orientation& Or1, 
				   TopAbs_Orientation& Or2,
				   const TopAbs_Orientation OrSave1, 
				   const TopAbs_Orientation OrSave2,
				   const Standard_Integer ChoixSave)
{
  if (Or1 == TopAbs_FORWARD){Or1 = OrSave1;}
  else {
    Or1 = TopAbs::Reverse(OrSave1);
  }
  if (Or2 == TopAbs_FORWARD){Or2 = OrSave2;}
  else {
    Or2 = TopAbs::Reverse(OrSave2);
  }

  Standard_Integer ChoixConge;
  if (Or1 == TopAbs_FORWARD) {
    if (Or2 == TopAbs_FORWARD) ChoixConge = 1;
    else {
      if(ChoixSave < 0) ChoixConge = 3;
      else ChoixConge = 7;
    }
  }
  else {
    if (Or2 == TopAbs_FORWARD) {
      if(ChoixSave < 0) ChoixConge = 7;
      else ChoixConge = 3;
    }
    else ChoixConge = 5;
  }
  if (Abs(ChoixSave)%2 == 0) ChoixConge++;
  return ChoixConge;
}


//=======================================================================
//function : NextSide
//purpose  : 
//           
//=======================================================================

void ChFi3d::NextSide(TopAbs_Orientation& Or, 
		     const TopAbs_Orientation OrSave, 
		     const TopAbs_Orientation OrFace) 
{
  if (Or == OrFace){Or = OrSave;}
  else {
    Or = TopAbs::Reverse(OrSave);
  }
}



//=======================================================================
//function : SameSide
//purpose  : 
//           
//=======================================================================

Standard_Boolean  ChFi3d::SameSide(const TopAbs_Orientation Or, 
				   const TopAbs_Orientation OrSave1, 
				   const TopAbs_Orientation OrSave2,
				   const TopAbs_Orientation OrFace1, 
				   const TopAbs_Orientation OrFace2)
{
  TopAbs_Orientation o1,o2;
  if (Or == OrFace1){o1 = OrSave1;}
  else {
    o1 = TopAbs::Reverse(OrSave1);
  }
  if (Or == OrFace2){o2 = OrSave2;}
  else {
    o2 = TopAbs::Reverse(OrSave2);
  }
  return (o1 == o2);
}

//=======================================================================
//function : Correct2dPoint
//purpose  : 
//=======================================================================
void Correct2dPoint(const TopoDS_Face& theF, gp_Pnt2d& theP2d)
{
  BRepAdaptor_Surface aBAS(theF, Standard_False);
  if (aBAS.GetType() < GeomAbs_BezierSurface) {
    return;
  }
  //
  const Standard_Real coeff = 0.01;
  Standard_Real eps;
  Standard_Real u1, u2, v1, v2;
  //
  aBAS.Initialize(theF, Standard_True);
  u1 = aBAS.FirstUParameter();
  u2 = aBAS.LastUParameter();
  v1 = aBAS.FirstVParameter();
  v2 = aBAS.LastVParameter();
  if (!(Precision::IsInfinite(u1) || Precision::IsInfinite(u2)))
  {
    eps = Max(coeff*(u2 - u1), Precision::PConfusion());
    if (Abs(theP2d.X() - u1) < eps)
    {
      theP2d.SetX(u1 + eps);
    }
    if (Abs(theP2d.X() - u2) < eps)
    {
      theP2d.SetX(u2 - eps);
    }
  }
  if (!(Precision::IsInfinite(v1) || Precision::IsInfinite(v2)))
  {
    eps = Max(coeff*(v2 - v1), Precision::PConfusion());
    if (Abs(theP2d.Y() - v1) < eps)
    {
      theP2d.SetY(v1 + eps);
    }
    if (Abs(theP2d.Y() - v2) < eps)
    {
      theP2d.SetY(v2 - eps);
    }
  }
}
