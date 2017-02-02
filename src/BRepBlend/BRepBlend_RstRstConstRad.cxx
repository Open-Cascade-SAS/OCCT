// Created on: 1997-02-10
// Created by: Jacques GOUSSARD
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


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Blend_Point.hxx>
#include <BlendFunc.hxx>
#include <BRepBlend_RstRstConstRad.hxx>
#include <ElCLib.hxx>
#include <GeomFill.hxx>
#include <gp.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <math_Gauss.hxx>
#include <math_Matrix.hxx>
#include <math_SVD.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>

#define Eps 1.e-15

static void t3dto2d(Standard_Real& a,
		    Standard_Real& b,
		    const gp_Vec& A,
		    const gp_Vec& B,
		    const gp_Vec& C)
{
  Standard_Real AB = A.Dot(B);
  Standard_Real AC = A.Dot(C);
  Standard_Real BC = B.Dot(C);
  Standard_Real BB = B.Dot(B);
  Standard_Real CC = C.Dot(C);
  Standard_Real deno = (BB*CC-BC*BC);
  a = (AB*CC-AC*BC)/deno;
  b = (AC*BB-AB*BC)/deno;
}

//=======================================================================
//function : BRepBlend_RstRstConstRad
//purpose  : 
//=======================================================================

BRepBlend_RstRstConstRad::BRepBlend_RstRstConstRad
(const Handle(Adaptor3d_HSurface)& Surf1,
 const Handle(Adaptor2d_HCurve2d)& Rst1,
 const Handle(Adaptor3d_HSurface)& Surf2,
 const Handle(Adaptor2d_HCurve2d)& Rst2,
 const Handle(Adaptor3d_HCurve)&   CGuide):
 surf1(Surf1), surf2(Surf2), rst1(Rst1), rst2(Rst2),
 cons1(Rst1, Surf1), cons2(Rst2, Surf2), 
 guide(CGuide), tguide(CGuide),
 istangent(Standard_True), maxang(RealFirst()), minang(RealLast()),
 distmin(RealLast()),
 mySShape(BlendFunc_Rational)
{}

//=======================================================================
//function : NbVariables
//purpose  : 
//=======================================================================

Standard_Integer BRepBlend_RstRstConstRad::NbVariables() const
{
  return 2;
}

//=======================================================================
//function : NbEquations
//purpose  : 
//=======================================================================

Standard_Integer BRepBlend_RstRstConstRad::NbEquations() const
{
  return 2;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_RstRstConstRad::Value(const math_Vector& X,
						 math_Vector&       F)
{
  ptrst1 = cons1.Value(X(1));
  ptrst2 = cons2.Value(X(2));
  
  F(1)   = nplan.XYZ().Dot(ptrst1.XYZ()) + theD;  
  F(2)   = nplan.XYZ().Dot(ptrst2.XYZ()) + theD;
  
  return Standard_True;
}

//=======================================================================
//function : Derivatives
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_RstRstConstRad::Derivatives(const math_Vector& X,
						       math_Matrix&       D)
{
  gp_Vec d11, d21;

  cons1.D1(X(1), ptrst1, d11);
  cons2.D1(X(2), ptrst2, d21);
  
  D(1,1) = nplan.Dot(d11);
  D(1,2) = 0.;
  
  D(2,1) = 0.;
  D(2,2) = nplan.Dot(d21);
    
  return Standard_True;
}

//=======================================================================
//function : Values
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_RstRstConstRad::Values(const math_Vector& X,
						  math_Vector&       F,
						  math_Matrix&       D)
{
  Value(X, F);  
  Derivatives(X, D);
 
  return Standard_True;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BRepBlend_RstRstConstRad::Set(const Handle(Adaptor3d_HSurface)& SurfRef1,
				   const Handle(Adaptor2d_HCurve2d)& RstRef1,
                                   const Handle(Adaptor3d_HSurface)& SurfRef2,
                                   const Handle(Adaptor2d_HCurve2d)& RstRef2)
{
  surfref1 = SurfRef1;
  surfref2 = SurfRef2;
  rstref1  = RstRef1;
  rstref2  = RstRef2;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BRepBlend_RstRstConstRad::Set(const Standard_Real Param)
{
  tguide->D2(Param, ptgui, d1gui, d2gui);
  normtg = d1gui.Magnitude();
  nplan  = d1gui.Normalized();
  theD   = - (nplan.XYZ().Dot(ptgui.XYZ()));
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BRepBlend_RstRstConstRad::Set(const Standard_Real First,
				   const Standard_Real Last)
{ 
 tguide = guide->Trim(First, Last, 1.e-12);
}

//=======================================================================
//function : GetTolerance
//purpose  : 
//=======================================================================

void BRepBlend_RstRstConstRad::GetTolerance(math_Vector&        Tolerance,
					    const Standard_Real Tol) const
{
  Tolerance(1) = cons1.Resolution(Tol);
  Tolerance(2) = cons2.Resolution(Tol);
}

//=======================================================================
//function : GetBounds
//purpose  : 
//=======================================================================

void BRepBlend_RstRstConstRad::GetBounds(math_Vector& InfBound,
					 math_Vector& SupBound) const
{
  InfBound(1) = cons1.FirstParameter();
  InfBound(2) = cons2.FirstParameter();
  SupBound(1) = cons1.LastParameter();
  SupBound(2) = cons2.LastParameter();
  
}

//=======================================================================
//function : IsSolution
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_RstRstConstRad::IsSolution(const math_Vector&  Sol,
						      const Standard_Real Tol)
     
     
{
  math_Vector valsol(1, 2), secmember(1, 2);
  math_Matrix gradsol(1, 2, 1, 2);
  
  gp_Vec dnplan, d1urst1, d1vrst1, d1urst2, d1vrst2, d11, d21, temp;
  gp_Pnt bid;

  Standard_Real Cosa, Sina, Angle;
  
  Values(Sol, valsol, gradsol);

  if (Abs(valsol(1)) <= Tol &&
      Abs(valsol(2)) <= Tol ) {
    
    // Calculation of tangents
    prmrst1  = Sol(1);    
    pt2drst1 = rst1->Value(prmrst1);
    prmrst2  = Sol(2);
    pt2drst2 = rst2->Value(prmrst2);

    cons1.D1(Sol(1), ptrst1, d11);
    cons2.D1(Sol(2), ptrst2, d21);

    dnplan.SetLinearForm(1./normtg, d2gui,
			 -1./normtg * (nplan.Dot(d2gui)), nplan);
    
    temp.SetXYZ(ptrst1.XYZ() - ptgui.XYZ());
    secmember(1) = normtg - dnplan.Dot(temp);
    
    temp.SetXYZ(ptrst2.XYZ() - ptgui.XYZ());
    secmember(2) = normtg - dnplan.Dot(temp);
    
    math_Gauss Resol(gradsol);

    if (Resol.IsDone()) {    
      Resol.Solve(secmember);
      istangent = Standard_False;
    }
    else {
      math_SVD SingRS (gradsol);
      if (SingRS.IsDone()) {
        math_Vector DEDT(1,3);
        DEDT = secmember;
        SingRS.Solve(DEDT, secmember, 1.e-6);
        istangent = Standard_False;
      }
      else istangent = Standard_True;
    }


    if (!istangent) {      
      tgrst1 = secmember(1) * d11;
      tgrst2 = secmember(2) * d21;

      Standard_Real a, b;
      surf1->D1(pt2drst1.X(), pt2drst1.Y(), bid, d1urst1, d1vrst1);
      t3dto2d(a, b, tgrst1, d1urst1, d1vrst1);
      tg2drst1.SetCoord(a, b);
      surf2->D1(pt2drst2.X(), pt2drst2.Y(), bid, d1urst2, d1vrst2);
      t3dto2d(a, b, tgrst1, d1urst2, d1vrst2);
      tg2drst2.SetCoord(a, b);
    }
 
    gp_Pnt Center;
    gp_Vec NotUsed;
    Standard_Boolean IsCenter;

    IsCenter = CenterCircleRst1Rst2(ptrst1, ptrst2, nplan, Center, NotUsed);

    if (!IsCenter) return Standard_False;

    gp_Vec n1(Center, ptrst1) , n2(Center, ptrst2);

    n1.Normalize();
    n2.Normalize();
    
    Cosa = n1.Dot(n2);
    Sina = nplan.Dot(n1.Crossed(n2));

    if (choix%2 != 0) {
      Sina = -Sina;  //nplan is changed into -nplan
    }
    
    Angle = ACos(Cosa);
    if (Sina < 0.) {
      Angle = 2.*M_PI - Angle;
    }
    
    if (Angle > maxang) {maxang = Angle;}
    if (Angle < minang) {minang = Angle;}
    distmin = Min( distmin, ptrst1.Distance(ptrst2));

    return Standard_True;
  }
  istangent = Standard_True;
  return Standard_False;
}

//=======================================================================
//function : GetMinimalDistance
//purpose  : 
//=======================================================================

Standard_Real BRepBlend_RstRstConstRad::GetMinimalDistance() const
{
  return distmin;
}

//=======================================================================
//function : PointOnRst1
//purpose  : 
//=======================================================================

const gp_Pnt& BRepBlend_RstRstConstRad::PointOnRst1() const
{
  return ptrst1;
}

//=======================================================================
//function : PointOnRst2
//purpose  : 
//=======================================================================

const gp_Pnt& BRepBlend_RstRstConstRad::PointOnRst2() const
{
  return ptrst2;
}

//=======================================================================
//function : Pnt2dOnRst1
//purpose  : 
//=======================================================================

const gp_Pnt2d& BRepBlend_RstRstConstRad::Pnt2dOnRst1() const
{
  return pt2drst1;
}

//=======================================================================
//function : Pnt2dOnRst2
//purpose  : 
//=======================================================================

const gp_Pnt2d& BRepBlend_RstRstConstRad::Pnt2dOnRst2() const
{
  return pt2drst2;
}

//=======================================================================
//function : ParameterOnRst1
//purpose  : 
//=======================================================================

Standard_Real BRepBlend_RstRstConstRad::ParameterOnRst1() const
{
  return prmrst1;
}

//=======================================================================
//function : ParameterOnRst2
//purpose  : 
//=======================================================================

Standard_Real BRepBlend_RstRstConstRad::ParameterOnRst2() const
{
  return prmrst2;
}
//=======================================================================
//function : IsTangencyPoint
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_RstRstConstRad::IsTangencyPoint() const
{
  return istangent;
}

//=======================================================================
//function : TangentOnRst1
//purpose  : 
//=======================================================================

const gp_Vec& BRepBlend_RstRstConstRad::TangentOnRst1() const
{
  if (istangent) {throw Standard_DomainError();}
  return tgrst1;
}

//=======================================================================
//function : Tangent2dOnRst1
//purpose  : 
//=======================================================================

const gp_Vec2d& BRepBlend_RstRstConstRad::Tangent2dOnRst1() const
{
  if (istangent) {throw Standard_DomainError();}
  return tg2drst1;
}

//=======================================================================
//function : TangentOnRst2
//purpose  : 
//=======================================================================

const gp_Vec& BRepBlend_RstRstConstRad::TangentOnRst2() const
{
  if (istangent) {throw Standard_DomainError();}
  return tgrst2;
}

//=======================================================================
//function : Tangent2dOnRst2
//purpose  : 
//=======================================================================

const gp_Vec2d& BRepBlend_RstRstConstRad::Tangent2dOnRst2() const
{
  if (istangent) {throw Standard_DomainError();}
  return tg2drst2;
}

//=======================================================================
//function : Decroch
//purpose  : 
//=======================================================================

Blend_DecrochStatus BRepBlend_RstRstConstRad::Decroch(const math_Vector& Sol,
						      gp_Vec&            NRst1,
						      gp_Vec&            TgRst1,
						      gp_Vec&            NRst2,
						      gp_Vec&            TgRst2)const
{
  gp_Vec NRst1InPlane, NRst2InPlane;
  gp_Pnt PtTmp1, PtTmp2, Center;
  gp_Vec d1u, d1v, centptrst, NotUsed;
  Standard_Real norm, unsurnorm;
  Standard_Real u,v;

  rstref1->Value(Sol(1)).Coord(u, v);
  surfref1->D1(u, v,PtTmp1,d1u,d1v);
  // Normal to the reference surface 1
  NRst1     = d1u.Crossed(d1v);  
  rstref2->Value(Sol(2)).Coord(u, v);
  surfref2->D1(u, v, PtTmp2, d1u, d1v);
  // Normal to the reference surface 2
  NRst2     = d1u.Crossed(d1v);

  CenterCircleRst1Rst2(PtTmp1, PtTmp2, nplan, Center, NotUsed);

  norm      = nplan.Crossed(NRst1).Magnitude();
  unsurnorm = 1. / norm;

  NRst1InPlane.SetLinearForm(nplan.Dot(NRst1) * unsurnorm, nplan, -unsurnorm, NRst1);

  centptrst.SetXYZ(PtTmp1.XYZ() - Center.XYZ());

  if (centptrst.Dot(NRst1InPlane) < 0.) NRst1InPlane.Reverse();

  TgRst1    = nplan.Crossed(centptrst);

  norm      = nplan.Crossed(NRst2).Magnitude();
  unsurnorm = 1./ norm;
  NRst2InPlane.SetLinearForm(nplan.Dot(NRst2) * unsurnorm, nplan, -unsurnorm, NRst2);
  centptrst.SetXYZ(PtTmp2.XYZ() - Center.XYZ());


  if (centptrst.Dot(NRst2InPlane) < 0.) NRst2InPlane.Reverse();

  TgRst2 = nplan.Crossed(centptrst);

  if (choix %2 != 0) {
    TgRst1.Reverse();
    TgRst2.Reverse();
  }

  // The vectors are returned 
  if (NRst1InPlane.Dot(TgRst1) > -1.e-10) {
    if (NRst2InPlane.Dot(TgRst2) < 1.e-10) {
      return Blend_DecrochBoth;
    }        
    else {
      return Blend_DecrochRst1;  
    }
  }
  else {
    if (NRst2InPlane.Dot(TgRst2) < 1.e-10) {
      return Blend_DecrochRst2;
    }        
    else {
      return Blend_NoDecroch;
    }
  }
  
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BRepBlend_RstRstConstRad::Set(const Standard_Real    Radius,
				   const Standard_Integer Choix)
{
  choix = Choix;
  ray   = Abs(Radius);

}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BRepBlend_RstRstConstRad::Set(const BlendFunc_SectionShape TypeSection)
{
  mySShape = TypeSection;
}



//=======================================================================
//function : CenterCircleRst1Rst2
//purpose  : Calculate the center of the circle passing by two points of restrictions
//=======================================================================
Standard_Boolean  BRepBlend_RstRstConstRad::CenterCircleRst1Rst2(const gp_Pnt&       PtRst1,
								 const gp_Pnt&       PtRst2,
								 const gp_Vec&       np,
								 gp_Pnt&             Center,
								 gp_Vec&             VdMed) const
{  
  
  gp_Vec rst1rst2(PtRst1, PtRst2);
  gp_Vec   vdmedNor; //,NRst1;    vdmedNor directong vector of the perpendicular bisector  
  Standard_Real norm2;
  Standard_Real Dist;// distance between the middle of PtRst1,PtRst2 and Center

  // Calculate the center of the circle 
  VdMed = rst1rst2.Crossed(np); 
  norm2 = rst1rst2.SquareMagnitude();
  Dist  = ray * ray - 0.25 * norm2;

  if (choix > 2) { 
    VdMed.Reverse();
  }

  if (Dist < - 1.E-07) return Standard_False;

  if (Dist > 1.E-07) {
    Dist = sqrt(Dist); 
    vdmedNor = VdMed.Normalized();
    Center.SetXYZ(0.5 * rst1rst2.XYZ() + PtRst1.XYZ() + Dist * vdmedNor.XYZ());
  }
  else
  {
    Center.SetXYZ(0.5 * rst1rst2.XYZ() + PtRst1.XYZ());    
  }

  return Standard_True;

}






//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

void BRepBlend_RstRstConstRad::Section(const Standard_Real Param,
				       const Standard_Real U,
				       const Standard_Real V,
				       Standard_Real&      Pdeb,
				       Standard_Real&      Pfin,
				       gp_Circ&               C)
{
  gp_Vec ns, np, NotUsed;
  gp_Pnt Center;
  
  tguide->D1(Param, ptgui, d1gui);
  np       = d1gui.Normalized();
  ptrst1   = cons1.Value(U);
  ptrst2   = cons2.Value(V);

  CenterCircleRst1Rst2(ptrst1, ptrst2, np, Center, NotUsed);

  C.SetRadius(Abs(ray));
  ns = gp_Vec(Center, ptrst1).Normalized(); 
 
  if (choix%2 != 0) {
    np.Reverse();
  }

  C.SetPosition(gp_Ax2(Center, np, ns));
  Pdeb = 0; //ElCLib::Parameter(C, pts);
  Pfin = ElCLib::Parameter(C, ptrst2);

  // Test of angles negative and almost null : Special Case
  if (Pfin > 1.5 * M_PI) {
    np.Reverse();
    C.SetPosition(gp_Ax2(Center, np, ns));
    Pfin = ElCLib::Parameter(C, ptrst2);
  }
  if (Pfin < Precision::PConfusion()) Pfin += Precision::PConfusion();
}

//=======================================================================
//function : IsRational
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_RstRstConstRad::IsRational () const
{
  return  (mySShape==BlendFunc_Rational || mySShape==BlendFunc_QuasiAngular);
}

//=======================================================================
//function : GetSectionSize
//purpose  :
//=======================================================================

Standard_Real BRepBlend_RstRstConstRad::GetSectionSize() const 
{
  return maxang * Abs(ray);
}

//=======================================================================
//function : GetMinimalWeight
//purpose  : 
//=======================================================================

void BRepBlend_RstRstConstRad::GetMinimalWeight(TColStd_Array1OfReal& Weights) const 
{
  BlendFunc::GetMinimalWeights(mySShape, myTConv, minang, maxang, Weights );
  // It is supposed that it does not depend on the Radius! 
}

//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================

Standard_Integer BRepBlend_RstRstConstRad::NbIntervals (const GeomAbs_Shape S) const
{
  return guide->NbIntervals(BlendFunc::NextShape(S));
}

//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================

void BRepBlend_RstRstConstRad::Intervals (TColStd_Array1OfReal& T,
					  const GeomAbs_Shape S) const
{
  guide->Intervals(T, BlendFunc::NextShape(S));
}

//=======================================================================
//function : GetShape
//purpose  : 
//=======================================================================

void BRepBlend_RstRstConstRad::GetShape (Standard_Integer& NbPoles,
					 Standard_Integer& NbKnots,
					 Standard_Integer& Degree,
					 Standard_Integer& NbPoles2d)
{
  NbPoles2d = 2;
  BlendFunc::GetShape(mySShape, maxang, NbPoles, NbKnots, Degree, myTConv);
}

//=======================================================================
//function : GetTolerance
//purpose  : Determine Tolerances to be used in approximations.
//=======================================================================

void BRepBlend_RstRstConstRad::GetTolerance(const Standard_Real BoundTol, 
					    const Standard_Real SurfTol, 
					    const Standard_Real AngleTol, 
					    math_Vector& Tol3d, 
					    math_Vector& Tol1d) const
{
  Standard_Integer low = Tol3d.Lower(), up = Tol3d.Upper();
  Standard_Real Tol;
  Tol= GeomFill::GetTolerance(myTConv, minang, Abs(ray), 
			       AngleTol, SurfTol);
  Tol1d.Init(SurfTol);
  Tol3d.Init(SurfTol);
  Tol3d(low+1) = Tol3d(up-1) = Min(Tol, SurfTol);
  Tol3d(low)   = Tol3d(up)   = Min(Tol, BoundTol);
}

//=======================================================================
//function : Knots
//purpose  : 
//=======================================================================

void BRepBlend_RstRstConstRad::Knots(TColStd_Array1OfReal& TKnots)
{
  GeomFill::Knots(myTConv, TKnots);
}

//=======================================================================
//function : Mults
//purpose  : 
//=======================================================================

void BRepBlend_RstRstConstRad::Mults(TColStd_Array1OfInteger& TMults)
{
  GeomFill::Mults(myTConv, TMults);
}

//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

void BRepBlend_RstRstConstRad::Section(const Blend_Point& P,
				       TColgp_Array1OfPnt& Poles,
				       TColgp_Array1OfPnt2d& Poles2d,
				       TColStd_Array1OfReal& Weights)
{
  gp_Vec ns, ns2, NotUsed;
  gp_Pnt Center;
  Standard_Real u, v;
  
  Standard_Real prm    = P.Parameter();
  Standard_Integer low = Poles.Lower();
  Standard_Integer upp = Poles.Upper();
  
  tguide->D1(prm,ptgui, d1gui);
  nplan  = d1gui.Normalized();
  
  u = P.ParameterOnC1(); 
  v = P.ParameterOnC2();

  gp_Pnt2d  pt2d1 = rst1->Value(u);
  gp_Pnt2d  pt2d2 = rst2->Value(v);

  ptrst1 = cons1.Value(u); 
  ptrst2 = cons2.Value(v);
  distmin = Min (distmin, ptrst1.Distance(ptrst2)); 

  Poles2d(Poles2d.Lower()).SetCoord(pt2d1.X(),pt2d1.Y());
  Poles2d(Poles2d.Upper()).SetCoord(pt2d2.X(),pt2d2.Y());
  
  // Linear case
  if (mySShape == BlendFunc_Linear) {
    Poles(low)   = ptrst1;
    Poles(upp)   = ptrst2;
    Weights(low) = 1.0;
    Weights(upp) = 1.0;
    return;
  }

  // Calculate the center of the circle
  CenterCircleRst1Rst2(ptrst1, ptrst2, nplan, Center, NotUsed);

  // normals to the section with points 
  ns  = gp_Vec(Center, ptrst1).Normalized();  
  ns2 = gp_Vec(Center, ptrst2).Normalized();

  if (choix%2 != 0) {
    nplan.Reverse();
  }
  
  GeomFill::GetCircle(myTConv,
		      ns, ns2, 
		      nplan, ptrst1, ptrst2,
		      Abs(ray), Center, 
		      Poles, Weights);
}

//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_RstRstConstRad::Section(const Blend_Point& P,
						   TColgp_Array1OfPnt& Poles,
						   TColgp_Array1OfVec& DPoles,
						   TColgp_Array1OfPnt2d& Poles2d,
						   TColgp_Array1OfVec2d& DPoles2d,
						   TColStd_Array1OfReal& Weights,
						   TColStd_Array1OfReal& DWeights)
{
  
  gp_Vec d11, d21;
  gp_Vec  dnplan, d1n1, d1n2;//,np2, dnp2;
  gp_Vec temp, tgct;
  gp_Vec d1urst, d1vrst;
  gp_Pnt Center, NotUsed;
  
  Standard_Real norm2, normmed, Dist;
  
  math_Vector sol(1, 2), valsol(1, 2), secmember(1, 2);
  math_Matrix gradsol(1, 2, 1, 2);
  
  Standard_Real prm       = P.Parameter();
  Standard_Integer low    = Poles.Lower();
  Standard_Integer upp    = Poles.Upper();
  Standard_Boolean istgt;
  
  tguide->D2(prm, ptgui, d1gui, d2gui);
  normtg = d1gui.Magnitude();
  nplan  = d1gui.Normalized();
  dnplan.SetLinearForm(1./normtg, d2gui,
		       -1./normtg * (nplan.Dot(d2gui)), nplan);
  
  sol(1)   = prmrst1 = P.ParameterOnC1();
  sol(2)   = prmrst2 = P.ParameterOnC2();
  pt2drst1 = rst1->Value(prmrst1);
  pt2drst2 = rst2->Value(prmrst2);
  
  Values(sol, valsol, gradsol);
  
  cons1.D1(sol(1), ptrst1, d11);
  cons2.D1(sol(2), ptrst2, d21);
  
  temp.SetXYZ(ptrst1.XYZ() - ptgui.XYZ());
  secmember(1) = normtg - dnplan.Dot(temp);
  
  temp.SetXYZ(ptrst2.XYZ() - ptgui.XYZ());
  secmember(2) = normtg - dnplan.Dot(temp);
  
  math_Gauss Resol(gradsol, 1.e-9);
  
  if (Resol.IsDone()) {
    istgt = Standard_False;
    Resol.Solve(secmember);
  }
  else {
    math_SVD SingRS (gradsol);
    if (SingRS.IsDone()) {
      math_Vector DEDT(1,2);
      DEDT = secmember;
      SingRS.Solve(DEDT, secmember, 1.e-6);
      istgt = Standard_False;
    }
    else istgt = Standard_True;
  }

  gp_Vec med;
  gp_Vec rst1rst2(ptrst1, ptrst2);
  Standard_Boolean IsCenter;

  IsCenter = CenterCircleRst1Rst2(ptrst1, ptrst2, nplan, Center, med);
  if (!IsCenter) return Standard_False;
    
  normmed = med.Magnitude();
  med.Normalize();
  gp_Vec n1(Center, ptrst1), n2(Center, ptrst2);
  n1.Normalize();
  n2.Normalize();

  if (!istgt) {
    // secmember contains derivatives of parameters on curves
    // compared to t  
    tgrst1 = secmember(1) * d11;
    tgrst2 = secmember(2) * d21;

    gp_Vec d1rst1rst2;

    norm2      = rst1rst2.SquareMagnitude();
    d1rst1rst2 = tgrst2 - tgrst1;        
    Dist       = ray * ray - 0.25 * norm2;

    if (Dist >  1.E-07) { 
      gp_Vec d1P1P2CrosNp, dmed;
      d1P1P2CrosNp = d1rst1rst2.Crossed(nplan) + rst1rst2.Crossed(dnplan);
      // derivative of the perpendicular bisector
      dmed = d1P1P2CrosNp - med.Dot(d1P1P2CrosNp) * med;
      dmed /= normmed; 
      Dist = sqrt(Dist);
      Standard_Real d1Dist = - (0.25 / Dist) * rst1rst2.Dot(d1rst1rst2);

      if  (choix > 2) {
        dmed.Reverse();
      }

      // the derivative of coefficient Dist is located in dmed
      dmed.SetLinearForm(Dist, dmed, d1Dist, med);
      d1rst1rst2 *= 0.5;   
      // derivative of the Normal to the curve in P1    
      d1n1 = - (dmed + d1rst1rst2) / ray;

      // derivative of the Normal to the curve in P2
      d1n2 = (d1rst1rst2 - dmed) / ray; 
    }
    else {
      d1rst1rst2 *= 0.5;
      // Normal to the curve in P1    
      d1n1 = - d1rst1rst2 / ray;

      // Normal to the curve in P2
      d1n2 = d1rst1rst2 / ray;       
    }   
  }
  
  // Tops 2d
  
  Poles2d(Poles2d.Lower()).SetCoord(pt2drst1.X(), pt2drst1.Y());
  Poles2d(Poles2d.Upper()).SetCoord(pt2drst2.X(), pt2drst2.Y());
  if (!istgt) {
    Standard_Real a, b;
    surf1->D1(pt2drst1.X(), pt2drst1.Y(), NotUsed, d1urst, d1vrst);
    t3dto2d(a,b,tgrst1, d1urst, d1vrst);
    DPoles2d(Poles2d.Lower()).SetCoord(a, b);

    surf2->D1(pt2drst2.X(), pt2drst2.Y(), NotUsed, d1urst, d1vrst);
    t3dto2d(a, b, tgrst2, d1urst, d1vrst);
    DPoles2d(Poles2d.Upper()).SetCoord(a, b);
  }
  
  // Linear case
  if (mySShape == BlendFunc_Linear) {
    Poles(low)   = ptrst1;
    Poles(upp)   = ptrst2;
    Weights(low) = 1.0;
    Weights(upp) = 1.0;
    if (!istgt) {
      DPoles(low)   = tgrst1;
      DPoles(upp)   = tgrst2;
      DWeights(low) = 0.0;
      DWeights(upp) = 0.0;
    }
    return (!istgt);
  }
  
  // Case of the circle
  // tangent to the center of the circle
  if (!istgt) {
    tgct.SetLinearForm(-ray, d1n1, tgrst1);
  }

  
  if (choix%2 != 0) {
    nplan.Reverse();
    dnplan.Reverse();
  }

  if (!istgt) {
    return GeomFill::GetCircle(myTConv, 
			      n1, n2, 
			      d1n1, d1n2, 
			      nplan, dnplan, 
			      ptrst1, ptrst2, 
			      tgrst1, tgrst2, 
			      Abs(ray), 0, 
			      Center, tgct, 
			      Poles, 
			      DPoles,
			      Weights, 
			      DWeights); 
  }
  else {
    GeomFill::GetCircle(myTConv,
		       n1, n2, 
		       nplan, ptrst1, ptrst2,
		       Abs(ray), Center, 
		       Poles, Weights);
    return Standard_False;
  }
}

//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_RstRstConstRad::Section
(const Blend_Point&,
 TColgp_Array1OfPnt&,
 TColgp_Array1OfVec&,
 TColgp_Array1OfVec&,
 TColgp_Array1OfPnt2d&,
 TColgp_Array1OfVec2d&,
 TColgp_Array1OfVec2d&,
 TColStd_Array1OfReal&,
 TColStd_Array1OfReal&,
 TColStd_Array1OfReal&)
{
  return Standard_False;
}


void BRepBlend_RstRstConstRad::Resolution(const Standard_Integer IC2d,
					  const Standard_Real Tol,
					  Standard_Real& TolU,
					  Standard_Real& TolV) const
{
  if(IC2d == 1){
    TolU = surf1->UResolution(Tol);
    TolV = surf1->VResolution(Tol);
  }
  else {
    TolU = surf2->UResolution(Tol);
    TolV = surf2->VResolution(Tol);
  }
}

