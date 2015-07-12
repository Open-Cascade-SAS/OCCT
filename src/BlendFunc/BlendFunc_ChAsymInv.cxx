// Created on: 1998-06-04
// Created by: Philippe NOUAILLE
// Copyright (c) 1998-1999 Matra Datavision
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
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <BlendFunc.hxx>
#include <BlendFunc_ChAsymInv.hxx>
#include <math_Matrix.hxx>
#include <Precision.hxx>

//=======================================================================
//function : BlendFunc_ChAsymInv
//purpose  : 
//=======================================================================
BlendFunc_ChAsymInv::BlendFunc_ChAsymInv(const Handle(Adaptor3d_HSurface)& S1,
                                         const Handle(Adaptor3d_HSurface)& S2,
                                         const Handle(Adaptor3d_HCurve)&   C) :
    surf1(S1),surf2(S2),curv(C),
    FX(1, 4),
    DX(1, 4, 1, 4)
{
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BlendFunc_ChAsymInv::Set(const Standard_Real Dist1,
			      const Standard_Real Angle,
			      const Standard_Integer Choix)
{
  dist1 = Abs(Dist1);
  angle = Angle;
  tgang = Tan(Angle);  
  choix = Choix;
}

//=======================================================================
//function : NbEquations
//purpose  : 
//=======================================================================

Standard_Integer BlendFunc_ChAsymInv::NbEquations () const
{
  return 4;
}

//=======================================================================
//function : GetTolerance
//purpose  : 
//=======================================================================

void BlendFunc_ChAsymInv::Set(const Standard_Boolean OnFirst,
			      const Handle(Adaptor2d_HCurve2d)& C)
{
  first = OnFirst;
  csurf = C;
}

//=======================================================================
//function : GetTolerance
//purpose  : 
//=======================================================================

void BlendFunc_ChAsymInv::GetTolerance(math_Vector& Tolerance, const Standard_Real Tol) const
{
  Tolerance(1) = csurf->Resolution(Tol);
  Tolerance(2) = curv->Resolution(Tol);
  if (first) {
    Tolerance(3) = surf2->UResolution(Tol);
    Tolerance(4) = surf2->VResolution(Tol);
  }
  else {
    Tolerance(3) = surf1->UResolution(Tol);
    Tolerance(4) = surf1->VResolution(Tol);
  }
}


//=======================================================================
//function : GetBounds
//purpose  : 
//=======================================================================

void BlendFunc_ChAsymInv::GetBounds(math_Vector& InfBound, math_Vector& SupBound) const
{
  InfBound(1) = csurf->FirstParameter();
  InfBound(2) = curv->FirstParameter();
  SupBound(1) = csurf->LastParameter();
  SupBound(2) = curv->LastParameter();

  if (first) {
    InfBound(3) = surf2->FirstUParameter();
    InfBound(4) = surf2->FirstVParameter();
    SupBound(3) = surf2->LastUParameter();
    SupBound(4) = surf2->LastVParameter();
    if(!Precision::IsInfinite(InfBound(3)) &&
       !Precision::IsInfinite(SupBound(3))) {
      const Standard_Real range = (SupBound(3) - InfBound(3));
      InfBound(3) -= range;
      SupBound(3) += range;
    }
    if(!Precision::IsInfinite(InfBound(4)) &&
       !Precision::IsInfinite(SupBound(4))) {
      const Standard_Real range = (SupBound(4) - InfBound(4));
      InfBound(4) -= range;
      SupBound(4) += range;
    }
  }
  else {
    InfBound(3) = surf1->FirstUParameter();
    InfBound(4) = surf1->FirstVParameter();
    SupBound(3) = surf1->LastUParameter();
    SupBound(4) = surf1->LastVParameter();
    if(!Precision::IsInfinite(InfBound(3)) &&
       !Precision::IsInfinite(SupBound(3))) {
      const Standard_Real range = (SupBound(3) - InfBound(3));
      InfBound(3) -= range;
      SupBound(3) += range;
    }
    if(!Precision::IsInfinite(InfBound(4)) &&
       !Precision::IsInfinite(SupBound(4))) {
      const Standard_Real range = (SupBound(4) - InfBound(4));
      InfBound(4) -= range;
      SupBound(4) += range;
    }
  }    
}

//=======================================================================
//function : IsSolution
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ChAsymInv::IsSolution(const math_Vector& Sol,
						 const Standard_Real Tol)
{
  math_Vector valsol(1, 4);
  gp_Pnt pts1, pts2, ptgui;
  gp_Vec nplan, d1gui, Nsurf1, tsurf1;
  gp_Vec d1u1, d1v1;
  
  curv->D1(Sol(2), ptgui, d1gui);
  nplan = d1gui.Normalized();

  gp_Pnt2d pt2d(csurf->Value(Sol(1)));

  if (first) {
    surf1->D1(pt2d.X(), pt2d.Y(), pts1, d1u1, d1v1);
    pts2 = surf2->Value(Sol(3), Sol(4));
  }
  else {
    surf1->D1(Sol(3), Sol(4), pts1, d1u1, d1v1);
    pts2 = surf2->Value(pt2d.X(), pt2d.Y());
  }

  Nsurf1   = d1u1.Crossed(d1v1);
  tsurf1   = Nsurf1.Crossed(nplan);

  gp_Vec s1s2(pts1, pts2);
  Standard_Real PScaInv = 1. / tsurf1.Dot(s1s2),  temp;// ,F4;   
  Standard_Real Nordu1 = d1u1.Magnitude(),
                Nordv1 = d1v1.Magnitude();

  temp = 2. * (Nordu1 + Nordv1) * s1s2.Magnitude() + 2. * Nordu1 * Nordv1;

  Value(Sol, valsol);

  if (Abs(valsol(1)) < Tol &&
      Abs(valsol(2)) < Tol &&
      Abs(valsol(3)) < 2. * dist1 * Tol  &&
      Abs(valsol(4)) < Tol * (1. + tgang) * Abs(PScaInv) * temp) {

    return Standard_True;
  }

  return Standard_False;  

}


//=======================================================================
//function : ComputeValues
//purpose  : 
//=======================================================================
Standard_Boolean BlendFunc_ChAsymInv::ComputeValues(const math_Vector& X,
						    const Standard_Integer DegF,
						    const Standard_Integer DegL)
{
  if (DegF > DegL) return Standard_False;

  gp_Vec nplan, dnplan, d1gui, d2gui, d1u1, d1v1, d2u1, d2v1, d2uv1, d1u2, d1v2;
  gp_Vec  Nsurf1,  tsurf1;
  gp_Pnt pts1, pts2, ptgui;
  Standard_Real PScaInv,  F4;
  Standard_Real Normg = 0.;
  gp_Pnt2d pt2d;
  gp_Vec2d v2d;

  if ( (DegF == 0) && (DegL == 0) ) {
    curv->D1(X(2), ptgui, d1gui);
    nplan  = d1gui.Normalized();

    if (choix%2 != 0) nplan.Reverse();
    pt2d = csurf->Value(X(1));

    if (first) {
      surf1->D1(pt2d.X(), pt2d.Y(), pts1, d1u1, d1v1);
      pts2 = surf2->Value(X(3), X(4));
    }
    else {
      surf1->D1(X(3), X(4), pts1, d1u1, d1v1);
      pts2 = surf2->Value(pt2d.X(), pt2d.Y());
    }
  }
  else {
    curv->D2(X(2), ptgui, d1gui, d2gui);
    nplan  = d1gui.Normalized();
    Normg  = d1gui.Magnitude(); 
    dnplan = (d2gui - nplan.Dot(d2gui) * nplan) / Normg;
    
    if (choix%2 != 0) {
      nplan.Reverse();
      dnplan.Reverse();
      Normg = - Normg;
    }

    csurf->D1(X(1), pt2d, v2d);

    if (first) {
      surf1->D2(pt2d.X(), pt2d.Y(), pts1, d1u1, d1v1, d2u1, d2v1, d2uv1);
      surf2->D1(X(3), X(4), pts2, d1u2, d1v2);
    }
    else {
      surf1->D2(X(3), X(4), pts1, d1u1, d1v1, d2u1, d2v1, d2uv1);
      surf2->D1(pt2d.X(), pt2d.Y(), pts2, d1u2, d1v2);
     }
  }

  gp_Vec nps1(ptgui, pts1), s1s2(pts1, pts2); 
  Nsurf1  = d1u1.Crossed(d1v1);
  tsurf1  = Nsurf1.Crossed(nplan);
  PScaInv = 1. / s1s2.Dot(tsurf1);
  F4      = nplan.Dot(tsurf1.Crossed(s1s2)) * PScaInv;
  
  if (DegF == 0) { 
    Standard_Real Dist;  
    Dist  = ptgui.XYZ().Dot(nplan.XYZ());
    FX(1) = pts1.XYZ().Dot(nplan.XYZ()) - Dist;
    FX(2) = pts2.XYZ().Dot(nplan.XYZ()) - Dist;
    FX(3) = dist1 * dist1 - nps1.SquareMagnitude(); 
    FX(4) = tgang - F4;

  }

  if (DegL == 1) {   
    gp_Vec dwtsurf1, tempVec;
    Standard_Real temp; 
    gp_Vec nps2(ptgui, pts2);

    if (first) {
      gp_Vec dw1du1, dw1dv1, dw1csurf, dw1pts1; 
      dw1pts1  = v2d.X() * d1u1 + v2d.Y() * d1v1;
      dw1du1   = v2d.X() * d2u1 + v2d.Y() * d2uv1;
      dw1dv1   = v2d.X() * d2uv1 + v2d.Y() * d2v1;
      dw1csurf = (dw1du1.Crossed(d1v1) + d1u1.Crossed(dw1dv1)).Crossed(nplan);
      dwtsurf1 = Nsurf1.Crossed(dnplan);

      DX(1, 1) = nplan.Dot(dw1pts1);
      DX(1, 2) = dnplan.Dot(nps1) - Normg;
      DX(1, 3) = 0.;
      DX(1, 4) = 0.;
      
      DX(2, 1) = 0.;
      DX(2, 2) = dnplan.Dot(nps2) - Normg;
      DX(2, 3) = nplan.Dot(d1u2);
      DX(2, 4) = nplan.Dot(d1v2);

      tempVec  = 2. * nps1;      
      DX(3, 1) = -dw1pts1.Dot(tempVec);
      DX(3, 2) = d1gui.Dot(tempVec);
      DX(3, 3) = 0.;
      DX(3, 4) = 0.;
 
      temp     = F4 * (dw1csurf.Dot(s1s2) - tsurf1.Dot(dw1pts1));
      temp    += nplan.Dot(tsurf1.Crossed(dw1pts1) - dw1csurf.Crossed(s1s2));
      DX(4, 1) = PScaInv * temp;
      
      temp     = F4 * dwtsurf1.Dot(s1s2);
      temp    -= dnplan.Dot(tempVec) + nplan.Dot(dwtsurf1.Crossed(s1s2));
      DX(4, 2) = PScaInv * temp;
      temp     = F4 * tsurf1.Dot(d1u2) - nplan.Dot(tsurf1.Crossed(d1u2));
      DX(4, 3) = PScaInv * temp;
      
      temp     = F4 * tsurf1.Dot(d1v2) - nplan.Dot(tsurf1.Crossed(d1v2));
      DX(4, 4) = PScaInv * temp;
    }
    else {
      gp_Vec d1utsurf1, d1vtsurf1, dw2pts2; 
      d1utsurf1 = (d2u1.Crossed(d1v1) + d1u1.Crossed(d2uv1)).Crossed(nplan);
      d1vtsurf1 = (d2uv1.Crossed(d1v1) + d1u1.Crossed(d2v1)).Crossed(nplan);
      dw2pts2   = v2d.X() * d1u2 + v2d.Y() * d1v2;
      dwtsurf1  = Nsurf1.Crossed(dnplan);

      DX(1, 1) = 0.;
      DX(1, 2) = dnplan.Dot(nps1) - Normg;
      DX(1, 3) = nplan.Dot(d1u1);
      DX(1, 4) = nplan.Dot(d1v1);
      
      DX(2, 1) = nplan.Dot(dw2pts2);
      DX(2, 2) = dnplan.Dot(nps2) - Normg;
      DX(2, 3) = 0.;
      DX(2, 4) = 0.;
      
      tempVec  = 2. * nps1;
      DX(3, 1) = 0.;
      DX(3, 2) = d1gui.Dot(tempVec);
      
      tempVec.Reverse();
      DX(3, 3) = d1u1.Dot(tempVec);
      DX(3, 4) = d1v1.Dot(tempVec);
      
      temp     = F4 * tsurf1.Dot(dw2pts2) - nplan.Dot(tsurf1.Crossed(dw2pts2));
      DX(4, 1) = PScaInv * temp;
      
      temp     = F4 * dwtsurf1.Dot(s1s2);
      temp    -= dnplan.Dot(tempVec) + nplan.Dot(dwtsurf1.Crossed(s1s2));
      DX(4, 2) = PScaInv * temp;
      
      temp     = F4 * (d1utsurf1.Dot(s1s2) - tsurf1.Dot(d1u1));
      temp    += nplan.Dot(tsurf1.Crossed(d1u1) - d1utsurf1.Crossed(s1s2));
      DX(4, 3) = PScaInv * temp;
      
      temp     = F4 * (d1vtsurf1.Dot(s1s2) - tsurf1.Dot(d1v1));
      temp    += nplan.Dot(tsurf1.Crossed(d1v1) - d1vtsurf1.Crossed(s1s2));
      DX(4, 4) = PScaInv * temp;
    }
  }
      
  return Standard_True;
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ChAsymInv::Value(const math_Vector& X, math_Vector& F)
{
  const Standard_Boolean Error = ComputeValues(X, 0, 0);
  F = FX;
  return Error;

}

//=======================================================================
//function : Derivatives
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ChAsymInv::Derivatives(const math_Vector& X, math_Matrix& D)
{
  const Standard_Boolean Error = ComputeValues(X, 1, 1);
  D = DX;
  return Error;
} 

//=======================================================================
//function : Values
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ChAsymInv::Values(const math_Vector& X,
					     math_Vector& F,
					     math_Matrix& D)
{
  const Standard_Boolean Error = ComputeValues(X, 0, 1);
  F = FX;
  D = DX;
  return Error;
/*  cout<<endl;
  cout<<" test ChAsymInv"<<endl;
  cout<<"calcul exact  <-->  approche"<<endl;

  math_Vector X1(1,4);
  math_Vector F1(1,4); 
  X1 = X; X1(1) += 1.e-10;
  Value(X1,F1);
  cout<<"D(1,1) : "<<D(1,1)<<" "<<(F1(1) - F(1)) * 1.e10<<endl;
  cout<<"D(2,1) : "<<D(2,1)<<" "<<(F1(2) - F(2)) * 1.e10<<endl;
  cout<<"D(3,1) : "<<D(3,1)<<" "<<(F1(3) - F(3)) * 1.e10<<endl;
  cout<<"D(4,1) : "<<D(4,1)<<" "<<(F1(4) - F(4)) * 1.e10<<endl;
  X1 = X; X1(2) += 1.e-10;
  Value(X1,F1);
  cout<<"D(1,2) : "<<D(1,2)<<" "<<(F1(1) - F(1)) * 1.e10<<endl;
  cout<<"D(2,2) : "<<D(2,2)<<" "<<(F1(2) - F(2)) * 1.e10<<endl;
  cout<<"D(3,2) : "<<D(3,2)<<" "<<(F1(3) - F(3)) * 1.e10<<endl;
  cout<<"D(4,2) : "<<D(4,2)<<" "<<(F1(4) - F(4)) * 1.e10<<endl;
  X1 = X; X1(3) += 1.e-10;
  Value(X1,F1);
  cout<<"D(1,3) : "<<D(1,3)<<" "<<(F1(1) - F(1)) * 1.e10<<endl;
  cout<<"D(2,3) : "<<D(2,3)<<" "<<(F1(2) - F(2)) * 1.e10<<endl;
  cout<<"D(3,3) : "<<D(3,3)<<" "<<(F1(3) - F(3)) * 1.e10<<endl;
  cout<<"D(4,3) : "<<D(4,3)<<" "<<(F1(4) - F(4)) * 1.e10<<endl;
  X1 = X; X1(4) += 1.e-10;
  Value(X1,F1);
  cout<<"D(1,4) : "<<D(1,4)<<" "<<(F1(1) - F(1)) * 1.e10<<endl;
  cout<<"D(2,4) : "<<D(2,4)<<" "<<(F1(2) - F(2)) * 1.e10<<endl;
  cout<<"D(3,4) : "<<D(3,4)<<" "<<(F1(3) - F(3)) * 1.e10<<endl;
  cout<<"D(4,4) : "<<D(4,4)<<" "<<(F1(4) - F(4)) * 1.e10<<endl;*/
}
