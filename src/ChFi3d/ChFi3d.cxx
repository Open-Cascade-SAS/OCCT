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
        cout<<"ConcaveSide : no concave face"<<endl;
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
