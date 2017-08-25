// Created on: 1992-03-13
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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


#include <BRepAdaptor_Curve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <HLRAlgo.hxx>
#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_CLProps.hxx>
#include <HLRBRep_Curve.hxx>
#include <Precision.hxx>
#include <ProjLib.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_UndefinedDerivative.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopoDS_Edge.hxx>

//OCC155 // jfa 05.03.2002 // bad vectors projection
//=======================================================================
//function : HLRBRep_Curve
//purpose  : 
//=======================================================================
HLRBRep_Curve::HLRBRep_Curve ()
{}

//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

void HLRBRep_Curve::Curve (const TopoDS_Edge& E)
{ myCurve.Initialize(E); }

//=======================================================================
//function : Parameter2d
//purpose  : 
//=======================================================================

Standard_Real 
HLRBRep_Curve::Parameter2d (const Standard_Real P3d) const
{
  // Mathematical formula for lines

  //        myOF P3d (myOF myVX - myOZ myVX + myOX myVZ)
  // Res -> --------------------------------------------
  //        (-myOF + myOZ) (-myOF + myOZ + P3d myVZ)

  switch (myType)
  {
    case GeomAbs_Line:
      if (((HLRAlgo_Projector*) myProj)->Perspective()) {
        const Standard_Real FmOZ = myOF - myOZ;
        return myOF * P3d * (myVX * FmOZ + myOX * myVZ) / (FmOZ * (FmOZ - P3d * myVZ));
      }
      return P3d * myVX;

    case GeomAbs_Ellipse:
      return P3d + myOX;

    default: // implemented to avoid gcc compiler warnings
      break;
  }
  return P3d;
}

//=======================================================================
//function : Parameter3d
//purpose  : 
//=======================================================================

Standard_Real
HLRBRep_Curve::Parameter3d (const Standard_Real P2d) const
{
  // Mathematical formula for lines

  //                                 2   
  //                   (-myOF + myOZ)  P2d
  // P3d -> -----------------------------------------------------
  //        (myOF - myOZ) (myOF myVX + P2d myVZ) + myOF myOX myVZ

  if (myType == GeomAbs_Line) {
    if (((HLRAlgo_Projector*) myProj)->Perspective()) {
      const Standard_Real FmOZ = myOF - myOZ;
      return P2d * FmOZ * FmOZ / (FmOZ * (myOF * myVX + P2d * myVZ) + myOF * myOX * myVZ);
    }
    return ((myVX <= gp::Resolution())? P2d : (P2d / myVX));
  }

  else if (myType == GeomAbs_Ellipse) {
    return P2d - myOX;
  }

  return P2d;
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

Standard_Real HLRBRep_Curve::Update(
  Standard_Real TotMin[16], Standard_Real TotMax[16])
{
  GeomAbs_CurveType typ = HLRBRep_BCurveTool::GetType(myCurve);
  myType = GeomAbs_OtherCurve;

  switch (typ) {

  case GeomAbs_Line:
    myType = typ;
    break;

  case GeomAbs_Circle:
    if (!((HLRAlgo_Projector*) myProj)->Perspective()) {
      gp_Dir D1 = HLRBRep_BCurveTool::Circle(myCurve).Axis().Direction();
      D1.Transform(((HLRAlgo_Projector*) myProj)->Transformation());
      if (D1.IsParallel(gp::DZ(),Precision::Angular()))
        myType = GeomAbs_Circle;
      else if (Abs(D1.Dot(gp::DZ())) < Precision::Angular()*10) //*10: The minor radius of ellipse should not be too small.
        myType = GeomAbs_OtherCurve;
      else {
        myType = GeomAbs_Ellipse;
        // compute the angle offset
        gp_Dir D3 = D1.Crossed(gp::DZ());
        gp_Dir D2 = HLRBRep_BCurveTool::Circle(myCurve).XAxis().Direction();
        D2.Transform(((HLRAlgo_Projector*) myProj)->Transformation());
        myOX = D3.AngleWithRef(D2,D1);
      }
    }
    break;

  case GeomAbs_Ellipse:
    if (!((HLRAlgo_Projector*) myProj)->Perspective()) {
      gp_Dir D1 = HLRBRep_BCurveTool::Ellipse(myCurve).Axis().Direction();
      D1.Transform(((HLRAlgo_Projector*) myProj)->Transformation());
      if (D1.IsParallel(gp::DZ(),Precision::Angular())) {
        myOX = 0.;    // no offset on the angle
        myType = GeomAbs_Ellipse;
      }
    }
    break;

  case GeomAbs_BezierCurve:
    if (HLRBRep_BCurveTool::Degree(myCurve) == 1)
      myType = GeomAbs_Line;
    else if (!((HLRAlgo_Projector*) myProj)->Perspective())
      myType = typ;
    break;

  case GeomAbs_BSplineCurve:
    if (!((HLRAlgo_Projector*) myProj)->Perspective())
      myType = typ;
    break;

  default:
    break;
  }

  if (myType == GeomAbs_Line) {
    // compute the values for a line
    gp_Lin L;
    Standard_Real l3d = 1.; // length of the 3d bezier curve
    if (HLRBRep_BCurveTool::GetType(myCurve) == GeomAbs_Line) {
      L = HLRBRep_BCurveTool::Line(myCurve);
    }
    else {  // bezier degree 1
      gp_Pnt PL;
      gp_Vec VL;
      HLRBRep_BCurveTool::D1(myCurve,0,PL,VL);
      L = gp_Lin(PL,VL);
      l3d = PL.Distance(HLRBRep_BCurveTool::Value(myCurve,1.));
    }
    gp_Pnt P = L.Location();
    gp_Vec V = L.Direction();
    P.Transform(((HLRAlgo_Projector*) myProj)->Transformation());
    V.Transform(((HLRAlgo_Projector*) myProj)->Transformation());
    if (((HLRAlgo_Projector*) myProj)->Perspective()) {
      gp_Pnt2d F;
      gp_Vec2d VFX;
      D1(0.,F,VFX);
      VFX.Normalize();
      myVX = (VFX.X()*V.X()+VFX.Y()*V.Y()) * l3d;
      Standard_Real l = - (VFX.X()*F.X() + VFX.Y()*F.Y());
      F.SetCoord(F.X()+VFX.X()*l,F.Y()+VFX.Y()*l);
      myOX = VFX.X()*(P.X()-F.X()) + VFX.Y()*(P.Y()-F.Y());
      gp_Vec VFZ(-F.X(),-F.Y(),((HLRAlgo_Projector*) myProj)->Focus());
      myOF = VFZ.Magnitude();
      VFZ /= myOF;
      myVZ = VFZ * V;
      myVZ *= l3d;
      myOZ = VFZ * gp_Vec(P.X()-F.X(),P.Y()-F.Y(),P.Z());
    }
    else myVX = Sqrt(V.X() * V.X() + V.Y() * V.Y()) * l3d;
  }
  return(UpdateMinMax(TotMin,TotMax));
}

//=======================================================================
//function : UpdateMinMax
//purpose  : 
//=======================================================================

Standard_Real HLRBRep_Curve::UpdateMinMax(
  Standard_Real TotMin[16], Standard_Real TotMax[16])
{
  Standard_Real a = HLRBRep_BCurveTool::FirstParameter(myCurve);
  Standard_Real b = HLRBRep_BCurveTool::LastParameter(myCurve);
  Standard_Real x,y,z,tolMinMax = 0;
  ((HLRAlgo_Projector*) myProj)->Project(Value3D(a),x,y,z);
  HLRAlgo::UpdateMinMax(x,y,z,TotMin,TotMax);

  if (myType != GeomAbs_Line) {
    Standard_Integer nbPnt = 30;
    Standard_Integer i;
    Standard_Real step = (b-a)/(nbPnt+1);
    Standard_Real xa,ya,za,xb =0.,yb =0.,zb =0.;
    Standard_Real dx1,dy1,dz1,dd1;
    Standard_Real dx2,dy2,dz2,dd2;

    for (i = 1; i <= nbPnt; i++) {
      a += step;
      xa = xb; ya = yb; za = zb;
      xb = x ; yb = y ; zb = z ;
      ((HLRAlgo_Projector*) myProj)->Project(Value3D(a),x,y,z);
      HLRAlgo::UpdateMinMax(x,y,z,TotMin,TotMax);
      if (i >= 2) {
        dx1 = x - xa; dy1 = y - ya; dz1 = z - za;
        dd1 = sqrt (dx1 * dx1 + dy1 * dy1 + dz1 * dz1);
        if (dd1 > 0) {
          dx2 = xb - xa; dy2 = yb - ya; dz2 = zb - za;
          dd2 = sqrt (dx2 * dx2 + dy2 * dy2 + dz2 * dz2);
          if (dd2 > 0) {
            Standard_Real p = (dx1 * dx2 + dy1 * dy2 + dz1 * dz2) / (dd1 * dd2);
            dx1 = xa + p * dx1 - xb;
            dy1 = ya + p * dy1 - yb;
            dz1 = za + p * dz1 - zb;
            dd1 = sqrt (dx1 * dx1 + dy1 * dy1 + dz1 * dz1);
            if (dd1 > tolMinMax) tolMinMax = dd1;
          }
        }
      }
    }
  }
  ((HLRAlgo_Projector*) myProj)->Project(Value3D(b),x,y,z);
  HLRAlgo::UpdateMinMax(x,y,z,TotMin,TotMax);
  return tolMinMax;
}

//=======================================================================
//function : Z
//purpose  : 
//=======================================================================

Standard_Real HLRBRep_Curve::Z (const Standard_Real U) const 
{
  gp_Pnt P3d;
  HLRBRep_BCurveTool::D0(myCurve,U,P3d);
  P3d.Transform(((HLRAlgo_Projector*) myProj)->Transformation());
  return P3d.Z();
}

//=======================================================================
//function : Tangent
//purpose  : 
//=======================================================================

void HLRBRep_Curve::Tangent (const Standard_Boolean AtStart,
                             gp_Pnt2d& P, gp_Dir2d& D) const
{
  Standard_Real U = AtStart? HLRBRep_BCurveTool::FirstParameter(myCurve) :
                             HLRBRep_BCurveTool::LastParameter (myCurve);

  D0(U,P);
  HLRBRep_CLProps CLP(2,Epsilon(1.));
  const HLRBRep_Curve* aCurve = this;
  CLP.SetCurve(aCurve);
  CLP.SetParameter(U);
  StdFail_UndefinedDerivative_Raise_if
    (!CLP.IsTangentDefined(), "HLRBRep_Curve::Tangent");
  CLP.Tangent(D); 
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void HLRBRep_Curve::D0 (const Standard_Real U, gp_Pnt2d& P) const
{
  /* gp_Pnt P3d;
  HLRBRep_BCurveTool::D0(myCurve,U,P3d);
  P3d.Transform(((HLRAlgo_Projector*) myProj)->Transformation());
  if (((HLRAlgo_Projector*) myProj)->Perspective()) {
    Standard_Real R = 1.-P3d.Z()/((HLRAlgo_Projector*) myProj)->Focus();
    P.SetCoord(P3d.X()/R,P3d.Y()/R);
  }
  else P.SetCoord(P3d.X(),P3d.Y()); */
  gp_Pnt P3d;
  HLRBRep_BCurveTool::D0(myCurve,U,P3d); 
  ((HLRAlgo_Projector*) myProj)->Project(P3d,P);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void HLRBRep_Curve::D1 (const Standard_Real U,
                        gp_Pnt2d& P, gp_Vec2d& V) const
{
  // Mathematical formula for lines

  //        X'[t]      X[t] Z'[t]                                     
  // D1 =  -------- + -------------                                   
  //           Z[t]          Z[t] 2                                   
  //       1 - ----   f (1 - ----)                                    
  //            f             f                                       

  gp_Pnt P3D;
  gp_Vec V13D;
  HLRBRep_BCurveTool::D1(myCurve,U,P3D,V13D);
  if (myProj->Perspective())
  {
    P3D .Transform(myProj->Transformation());
    V13D.Transform(myProj->Transformation());

    Standard_Real f = myProj->Focus();
    Standard_Real R = 1. - P3D.Z()/f;
    Standard_Real e = V13D.Z()/(f*R*R);
    P.SetCoord(P3D .X()/R            , P3D .Y()/R            );
    V.SetCoord(V13D.X()/R + P3D.X()*e, V13D.Y()/R + P3D.Y()*e);
  }
  else {
    //OCC155
    myProj->Project(P3D,V13D,P,V);
  }
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void HLRBRep_Curve::D2 (const Standard_Real U,
                        gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const
{
  // Mathematical formula for lines
  
  //                                   2
  //       2 X'[t] Z'[t]   2 X[t] Z'[t]      X''[t]     X[t] Z''[t]   
  // D2 =  ------------- + -------------- + -------- + -------------  
  //              Z[t] 2    2      Z[t] 3       Z[t]          Z[t] 2  
  //       f (1 - ----)    f  (1 - ----)    1 - ----   f (1 - ----)   
  //               f                f            f             f      

  gp_Pnt P3D;
  gp_Vec V13D,V23D;
  HLRBRep_BCurveTool::D2(myCurve,U,P3D,V13D,V23D);
  P3D .Transform(myProj->Transformation());
  V13D.Transform(myProj->Transformation());
  V23D.Transform(myProj->Transformation());
  if (myProj->Perspective())
  {
    Standard_Real f = myProj->Focus();
    Standard_Real R = 1. - P3D.Z() / f;
    Standard_Real q = f*R*R;
    Standard_Real e = V13D.Z()/q;
    Standard_Real c = e*V13D.Z()/(f*R);
    P .SetCoord(P3D .X()/R            , P3D .Y()/R            );
    V1.SetCoord(V13D.X()/R + P3D.X()*e,	V13D.Y()/R + P3D.Y()*e);
    V2.SetCoord(V23D.X()/R + 2*V13D.X()*e + P3D.X()*V23D.Z()/q + 2*P3D.X()*c,
		V23D.Y()/R + 2*V13D.Y()*e + P3D.Y()*V23D.Z()/q + 2*P3D.Y()*c);
  }
  else {
    P .SetCoord(P3D .X(),P3D .Y());
    V1.SetCoord(V13D.X(),V13D.Y());
    V2.SetCoord(V23D.X(),V23D.Y());
  }
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void HLRBRep_Curve::D3 (const Standard_Real,
                        gp_Pnt2d&, gp_Vec2d&, gp_Vec2d&, gp_Vec2d&) const
{
}

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec2d HLRBRep_Curve::DN (const Standard_Real, const Standard_Integer) const
{ return gp_Vec2d(); }

//=======================================================================
//function : Line
//purpose  : 
//=======================================================================

gp_Lin2d HLRBRep_Curve::Line () const
{
  gp_Pnt2d P;
  gp_Vec2d V;
  D1(0.,P,V);
  return gp_Lin2d(P,V);
}

//=======================================================================
//function : Circle
//purpose  : 
//=======================================================================

gp_Circ2d HLRBRep_Curve::Circle () const
{
  gp_Circ C = HLRBRep_BCurveTool::Circle(myCurve);
  C.Transform(myProj->Transformation());
  return ProjLib::Project(gp_Pln(gp::XOY()),C);
}

//=======================================================================
//function : Ellipse
//purpose  : 
//=======================================================================

gp_Elips2d HLRBRep_Curve::Ellipse () const
{
  if (HLRBRep_BCurveTool::GetType(myCurve) == GeomAbs_Ellipse) {
    gp_Elips E = HLRBRep_BCurveTool::Ellipse(myCurve);
    E.Transform(myProj->Transformation());
    return ProjLib::Project(gp_Pln(gp::XOY()),E);
  }
  // this is a circle
  gp_Circ C = HLRBRep_BCurveTool::Circle(myCurve);
  C.Transform(myProj->Transformation());
  const gp_Dir& D1 = C.Axis().Direction();
  const gp_Dir& D3 = D1.Crossed(gp::DZ());
  const gp_Dir& D2 = D1.Crossed(D3);
  Standard_Real rap = sqrt( D2.X()*D2.X() + D2.Y()*D2.Y() );
  gp_Dir2d d(D1.Y(),-D1.X());
  gp_Pnt2d p(C.Location().X(),C.Location().Y());
  gp_Elips2d El(gp_Ax2d(p,d),C.Radius(),C.Radius()*rap);
  if ( D1.Z() < 0 ) El.Reverse();
  return El;
}

//=======================================================================
//function : Hyperbola
//purpose  : 
//=======================================================================

gp_Hypr2d HLRBRep_Curve::Hyperbola () const
{ return gp_Hypr2d(); }

//=======================================================================
//function : Parabola
//purpose  : 
//=======================================================================
gp_Parab2d HLRBRep_Curve::Parabola () const
{ return gp_Parab2d(); }

//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void  HLRBRep_Curve::Poles (TColgp_Array1OfPnt2d& TP) const
{
  Standard_Integer i1 = TP.Lower();
  Standard_Integer i2 = TP.Upper();
  TColgp_Array1OfPnt TP3(i1,i2);
  //-- HLRBRep_BCurveTool::Poles(myCurve,TP3);
  if(HLRBRep_BCurveTool::GetType(myCurve) == GeomAbs_BSplineCurve) { 
    (HLRBRep_BCurveTool::BSpline(myCurve))->Poles(TP3);
  }
  else { 
    (HLRBRep_BCurveTool::Bezier(myCurve))->Poles(TP3);
  }
  for (Standard_Integer i = i1; i <= i2; i++) {
    myProj->Transform(TP3(i));
    TP(i).SetCoord(TP3(i).X(),TP3(i).Y());
  }
}

//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void  HLRBRep_Curve::Poles (const Handle(Geom_BSplineCurve)& aCurve,
			    TColgp_Array1OfPnt2d& TP) const
{
  Standard_Integer i1 = TP.Lower();
  Standard_Integer i2 = TP.Upper();
  TColgp_Array1OfPnt TP3(i1,i2);
  //-- HLRBRep_BCurveTool::Poles(myCurve,TP3);
  aCurve->Poles(TP3);

  for (Standard_Integer i = i1; i <= i2; i++) {
    ((HLRAlgo_Projector*) myProj)->Transform(TP3(i));
    TP(i).SetCoord(TP3(i).X(),TP3(i).Y());
  }
}

//=======================================================================
//function : PolesAndWeights
//purpose  : 
//=======================================================================

void  HLRBRep_Curve::PolesAndWeights (TColgp_Array1OfPnt2d& TP,
					       TColStd_Array1OfReal& TW) const
{
  Standard_Integer i1 = TP.Lower();
  Standard_Integer i2 = TP.Upper();
  TColgp_Array1OfPnt TP3(i1,i2);
  //-- HLRBRep_BCurveTool::PolesAndWeights(myCurve,TP3,TW);
  
  if(HLRBRep_BCurveTool::GetType(myCurve) == GeomAbs_BSplineCurve) {
    Handle(Geom_BSplineCurve) HB = (HLRBRep_BCurveTool::BSpline(myCurve));
    HB->Poles(TP3);
    HB->Weights(TW);  
    //-- (HLRBRep_BCurveTool::BSpline(myCurve))->PolesAndWeights(TP3,TW);
  }
  else { 
    Handle(Geom_BezierCurve) HB = (HLRBRep_BCurveTool::Bezier(myCurve));
    HB->Poles(TP3);
    HB->Weights(TW);
    //-- (HLRBRep_BCurveTool::Bezier(myCurve))->PolesAndWeights(TP3,TW);
  }
  for (Standard_Integer i = i1; i <= i2; i++) {
    ((HLRAlgo_Projector*) myProj)->Transform(TP3(i));
    TP(i).SetCoord(TP3(i).X(),TP3(i).Y());
  }
}

//=======================================================================
//function : PolesAndWeights
//purpose  : 
//=======================================================================

void  HLRBRep_Curve::PolesAndWeights (const Handle(Geom_BSplineCurve)& aCurve,
				      TColgp_Array1OfPnt2d& TP,
				      TColStd_Array1OfReal& TW) const
{
  Standard_Integer i1 = TP.Lower();
  Standard_Integer i2 = TP.Upper();
  TColgp_Array1OfPnt TP3(i1,i2);
  //-- HLRBRep_BCurveTool::PolesAndWeights(myCurve,TP3,TW);
  
  aCurve->Poles(TP3);
  aCurve->Weights(TW);  
    //-- (HLRBRep_BCurveTool::BSpline(myCurve))->PolesAndWeights(TP3,TW);

  for (Standard_Integer i = i1; i <= i2; i++) {
    ((HLRAlgo_Projector*) myProj)->Transform(TP3(i));
    TP(i).SetCoord(TP3(i).X(),TP3(i).Y());
  }
}

//=======================================================================
//function : Knots
//purpose  : 
//=======================================================================

void  HLRBRep_Curve::Knots (TColStd_Array1OfReal& kn) const
{
  if(HLRBRep_BCurveTool::GetType(myCurve) == GeomAbs_BSplineCurve) {
    Handle(Geom_BSplineCurve) HB = (HLRBRep_BCurveTool::BSpline(myCurve));
    HB->Knots(kn);
  }
}

//=======================================================================
//function : Multiplicities
//purpose  : 
//=======================================================================

void  HLRBRep_Curve::Multiplicities (TColStd_Array1OfInteger& mu) const
{
  if(HLRBRep_BCurveTool::GetType(myCurve) == GeomAbs_BSplineCurve) {
    Handle(Geom_BSplineCurve) HB = (HLRBRep_BCurveTool::BSpline(myCurve));
    HB->Multiplicities(mu);
  }
}
