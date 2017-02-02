// Created on: 1994-03-10
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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


#include <Bisector.hxx>
#include <Bisector_BisecPC.hxx>
#include <ElCLib.hxx>
#include <GccEnt_Position.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Geometry.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom2dGcc.hxx>
#include <Geom2dGcc_Circ2d2TanRad.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <Precision.hxx>
#include <Standard_DivideByZero.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Bisector_BisecPC,Bisector_Curve)

//=============================================================================
//function :
// purpose :
//=============================================================================
Bisector_BisecPC::Bisector_BisecPC()
{
}

//=============================================================================
//function :
// purpose :
//=============================================================================
Bisector_BisecPC::Bisector_BisecPC(const Handle(Geom2d_Curve)& Cu, 
				   const gp_Pnt2d&             P,
				   const Standard_Real         Side,
				   const Standard_Real         DistMax)
{
  Perform (Cu,P,Side,DistMax);
}

//=============================================================================
//function :
// purpose :
//=============================================================================
Bisector_BisecPC::Bisector_BisecPC(const Handle(Geom2d_Curve)& Cu, 
				   const gp_Pnt2d&             P,
				   const Standard_Real         Side,
				   const Standard_Real         UMin,
				   const Standard_Real         UMax)

{
  curve    = Handle (Geom2d_Curve)::DownCast(Cu->Copy());
  point    = P;
  sign     = Side;
  startIntervals.Append(UMin);
  endIntervals  .Append(UMax);  
  bisInterval    = 1;
  extensionStart = Standard_False;
  extensionEnd   = Standard_False; 
  pointStartBis  = Value(UMin);
  pointEndBis    = Value(UMax);
  isConvex = Bisector::IsConvex(curve,sign);
}

//=============================================================================
//function : Perform
// purpose :
//=============================================================================
void  Bisector_BisecPC::Perform(const Handle(Geom2d_Curve)& Cu, 
				const gp_Pnt2d&             P,
				const Standard_Real         Side,
				const Standard_Real         DistMax)
{
  curve    = Handle (Geom2d_Curve)::DownCast(Cu->Copy());
  point    = P;
  distMax  = DistMax;
  sign     = Side;
  isConvex = Bisector::IsConvex(curve,sign);
  //--------------------------------------------
  // Calculate interval of definition.
  //--------------------------------------------
  ComputeIntervals();
  if (isEmpty) return;

  //-------------------------
  // Construction extensions.
  //-------------------------  
  bisInterval    = 1;
  extensionStart = Standard_False;
  extensionEnd   = Standard_False;
  pointStartBis  = Value(startIntervals.First());
  pointEndBis    = Value(endIntervals  .Last());

  if (!isConvex) {
    if (point.IsEqual(curve->Value(curve->FirstParameter()),
		      Precision::Confusion())               ) {
      extensionStart = Standard_True;
      Standard_Real UFirst = startIntervals.First() - P.Distance(pointStartBis);
      startIntervals.InsertBefore(1,UFirst);
      endIntervals  .InsertBefore(1,startIntervals.Value(2));
      bisInterval = 2;
    }
    else if (point.IsEqual(curve->Value(curve->LastParameter()),
			   Precision::Confusion())              ) {
      extensionEnd = Standard_True;      
      Standard_Real ULast = endIntervals.Last() + P.Distance(pointEndBis);
      startIntervals.Append(endIntervals.Last());
      endIntervals  .Append(ULast);
      bisInterval = 1;
    }
  }
}

//=============================================================================
//function : IsExtendAtStart
//purpose  :
//=============================================================================
Standard_Boolean Bisector_BisecPC::IsExtendAtStart() const
{
  return extensionStart;
}

//=============================================================================
//function : IsExtendAtEnd
//purpose  :
//=============================================================================
Standard_Boolean Bisector_BisecPC::IsExtendAtEnd() const
{
  return extensionEnd;
}

//=============================================================================
//function : Reverse
//purpose  :
//=============================================================================
void Bisector_BisecPC::Reverse()
{
  throw Standard_NotImplemented();
}

//=============================================================================
//function : ReversedParameter
//purpose  :
//=============================================================================
Standard_Real  Bisector_BisecPC::ReversedParameter(const Standard_Real U) const
{
 return LastParameter() + FirstParameter() - U;
}

//=============================================================================
//function : Copy
//purpose  :
//=============================================================================
Handle(Geom2d_Geometry) Bisector_BisecPC::Copy() const
{
  Handle(Geom2d_Curve) CopyC = Handle(Geom2d_Curve)::DownCast(curve->Copy());
  Handle(Bisector_BisecPC) C = new Bisector_BisecPC();

  C->Init (CopyC,point,sign,
	   startIntervals,endIntervals,bisInterval,currentInterval,
	   shiftParameter,distMax,isEmpty,isConvex,extensionStart,extensionEnd,
	   pointStartBis,pointEndBis);
  return C;  
}

//=============================================================================
//function : Transform
//purpose  :
//=============================================================================
void Bisector_BisecPC::Transform(const gp_Trsf2d& T)
{
  curve        ->Transform(T);
  point        . Transform(T);
  pointStartBis. Transform(T);
  pointEndBis  . Transform(T);
}

//=============================================================================
//function : IsCN
//purpose  :
//=============================================================================
Standard_Boolean Bisector_BisecPC::IsCN(const Standard_Integer N) const 
{
  return curve->IsCN(N+1);
}

//=============================================================================
//function : FirstParameter
//purpose  :
//=============================================================================
Standard_Real Bisector_BisecPC::FirstParameter() const
{
 return startIntervals.First();
}

//=============================================================================
//function : LastParameter
//purpose  :
//=============================================================================
Standard_Real Bisector_BisecPC::LastParameter() const
{
 return endIntervals.Last();
}

//=============================================================================
//function : Continuity
//purpose  :
//=============================================================================
GeomAbs_Shape Bisector_BisecPC::Continuity() const 
{
  GeomAbs_Shape Cont = curve->Continuity();
  switch (Cont) {
  case GeomAbs_C1 : return GeomAbs_C0; 
  case GeomAbs_C2 : return GeomAbs_C1;
  case GeomAbs_C3 : return GeomAbs_C2;
  case GeomAbs_CN : return GeomAbs_CN;  
  default: break;
  }
  return GeomAbs_C0;
}

//=============================================================================
//function : NbIntervals
//purpose  :
//=============================================================================
Standard_Integer Bisector_BisecPC::NbIntervals() const
{
  return startIntervals.Length();
}

//=============================================================================
//function : IntervalFirst
//purpose  :
//=============================================================================
Standard_Real Bisector_BisecPC::IntervalFirst(const Standard_Integer I) const
{
  return startIntervals.Value(I);
}
    
//=============================================================================
//function : IntervalLast
//purpose  :
//=============================================================================
Standard_Real Bisector_BisecPC::IntervalLast(const Standard_Integer I) const
{
  return endIntervals.Value(I);
}

//=============================================================================
//function : IntervalContinuity
//purpose  :
//=============================================================================
GeomAbs_Shape Bisector_BisecPC::IntervalContinuity() const
{
  GeomAbs_Shape Cont = curve->Continuity();
  switch (Cont) {
  case GeomAbs_C1 : return GeomAbs_C0; 
  case GeomAbs_C2 : return GeomAbs_C1;
  case GeomAbs_C3 : return GeomAbs_C2;
  case GeomAbs_CN : return GeomAbs_CN;  
  default: break;
  }
  return GeomAbs_C0; 
}

//=============================================================================
//function : IsClosed
//purpose  :
//=============================================================================
Standard_Boolean Bisector_BisecPC::IsClosed() const
{
  if (curve->IsClosed()) {
    //-----------------------------------------------------------------------
    // The bisectrice is closed if the curve is closed and the bissectrice
    // has only one domain of continuity equal to the one of the curve.
    // -----------------------------------------------------------------------
    if (startIntervals.First() == curve->FirstParameter() &&
	endIntervals  .First() == curve->LastParameter ()    )
      return Standard_True;
  }
  return Standard_False;
}

//=============================================================================
//function : IsPeriodic
//purpose  :
//=============================================================================
Standard_Boolean Bisector_BisecPC::IsPeriodic() const
{
  return Standard_False;
}

//=============================================================================
//function : Extension
// purpose :
//=============================================================================
void Bisector_BisecPC::Extension(const Standard_Real    U,
				       gp_Pnt2d&        P,
				       gp_Vec2d&        V1,
				       gp_Vec2d&        V2,
				       gp_Vec2d&        V3 ) const
{
  Standard_Real dU;

  V1.SetCoord(0., 0.);
  V2.SetCoord(0., 0.);
  V3.SetCoord(0., 0.);
  if      ( U < startIntervals.Value(bisInterval)) {
    if (pointStartBis.IsEqual(point, Precision::PConfusion()))
      P = pointStartBis;
    else {
      dU = U - startIntervals.Value(bisInterval);
      gp_Dir2d DirExt(pointStartBis.X() - point.X(),
                      pointStartBis.Y() - point.Y());
      P.SetCoord(pointStartBis.X() + dU*DirExt.X(),
                 pointStartBis.Y() + dU*DirExt.Y());
      V1.SetCoord(DirExt.X(), DirExt.Y());
    }
  }
  else if ( U > endIntervals.Value(bisInterval)) { 
    if (pointEndBis.IsEqual(point, Precision::PConfusion()))
      P = pointEndBis;
    else {
      dU =  U - endIntervals.Value(bisInterval);
      gp_Dir2d DirExt(point.X() - pointEndBis.X(),
                      point.Y() - pointEndBis.Y()); 
      P.SetCoord(pointEndBis.X() + dU*DirExt.X(),
                 pointEndBis.Y() + dU*DirExt.Y());
      V1.SetCoord(DirExt.X(), DirExt.Y());
    }
  }
}


//=============================================================================
//function : Values
// purpose : To each point of the curve is associated a point on the 
//           bissectrice. The equation of the bissectrice is:
//                              || PP(u)||**2
//           F(u) = P(u) - 1/2* -------------- * N(u)
//                              (N(u)|PP(u))
//
//           N(u) normal to the curve by u.
//           ( | ) designation of the scalar product.
//=============================================================================
void Bisector_BisecPC::Values(const Standard_Real    U,
			      const Standard_Integer N,
			            gp_Pnt2d&        P,
			            gp_Vec2d&        V1,
			            gp_Vec2d&        V2,
				    gp_Vec2d&        V3 ) const
{
  if ( U < startIntervals.Value(bisInterval)) {
    Extension(U,P,V1,V2,V3);
    return;
  }
  else if ( U > endIntervals.Value(bisInterval)) { 
    Extension(U,P,V1,V2,V3);
    return;
  }
  Standard_Real UOnCurve = LinkBisCurve(U);

  gp_Vec2d      Tu,Tuu,T3u;
  gp_Pnt2d      PC;

  switch (N) {
  case 0 :  {curve->D1(UOnCurve,PC,Tu)        ;break;}
  case 1 :  {curve->D2(UOnCurve,PC,Tu,Tuu)    ;break;}
  case 2 :  {curve->D3(UOnCurve,PC,Tu,Tuu,T3u);break;}
  }
  
  gp_Vec2d aPPC(PC.X() - point.X(), PC.Y() - point.Y());
  gp_Vec2d Nor( - Tu.Y(), Tu.X());
  
  Standard_Real SquarePPC = aPPC.SquareMagnitude();
  Standard_Real NorPPC    = Nor.Dot(aPPC);
  Standard_Real A1;

  if (Abs(NorPPC) > gp::Resolution() && (NorPPC*sign) < 0.) {
    A1 = 0.5*SquarePPC/NorPPC;
    P.SetCoord(PC.X() - Nor.X()*A1, PC.Y() - Nor.Y()*A1);
  }
  else {return; }
  
  if (N == 0) return;                                 // End Calculation Point;

  gp_Vec2d      Nu ( - Tuu.Y() , Tuu.X());            // derivative of the normal by U.
  Standard_Real NuPPC     = Nu .Dot(aPPC);
  Standard_Real TuPPC     = Tu .Dot(aPPC);
  Standard_Real NorPPCE2  = NorPPC*NorPPC;
  Standard_Real A2        = TuPPC/NorPPC - 0.5*NuPPC*SquarePPC/NorPPCE2;

//--------------------------
  V1 = Tu - A1*Nu - A2*Nor;
//--------------------------
  if (N == 1) return;                                       // End calculation D1.

  gp_Vec2d Nuu ( - T3u.Y() , T3u.X());
  
  Standard_Real NorPPCE4 = NorPPCE2*NorPPCE2;
  Standard_Real NuuPPC   = Nuu.Dot(aPPC);
  Standard_Real TuuPPC   = Tuu.Dot(aPPC);
  
  Standard_Real A21 = TuuPPC/NorPPC - TuPPC*NuPPC/NorPPCE2;
  Standard_Real A22 = (0.5*NuuPPC*SquarePPC + NuPPC*TuPPC)/NorPPCE2 - 
                      NuPPC*SquarePPC*NorPPC*NuPPC/NorPPCE4;
  Standard_Real A2u = A21 - A22;      
//----------------------------------------
  V2  = Tuu - 2*A2*Nu - A1*Nuu - A2u*Nor;
//----------------------------------------
}

//=============================================================================
//function : Curvature
//purpose  :
//=============================================================================
// Unused :
#ifdef OCCT_DEBUG_CUR
static Standard_Real Curvature (const Handle(Geom2d_Curve)& C,
				      Standard_Real         U,
				      Standard_Real         Tol)
{
  Standard_Real K1;
  gp_Vec2d      D1,D2;
  gp_Pnt2d      P;
  C->D2(U,P,D1,D2);
  Standard_Real Norm2 = D1.SquareMagnitude();;
  if (Norm2 < Tol) {
    K1 = 0.0;
  }
  else {
    K1   = (D1^D2)/(Norm2*sqrt(Norm2));
  }
  return K1;
}
#endif

//=============================================================================
//function : Distance
//purpose  : distance at the square of the point of parameter U to the curve and at point:
//
//            2             ||PP(u)||**4           2
//           d =   1/4* ------------------- ||Nor||
//                         (Nor(u)/PP(u))**2 
//
//           where Nor is the normal to the curve by U. 
//=============================================================================
Standard_Real Bisector_BisecPC::Distance (const Standard_Real U) const
{
  gp_Vec2d Tan;
  gp_Pnt2d PC;

  Standard_Real UOnCurve = LinkBisCurve(U);
  
  curve->D1(UOnCurve,PC,Tan);
  gp_Vec2d aPPC(PC.X() - point.X(), PC.Y() - point.Y());
  gp_Vec2d Nor( - Tan.Y(), Tan.X());

  Standard_Real NorNor       = Nor.SquareMagnitude();
  Standard_Real SquareMagPPC = aPPC.SquareMagnitude();
  Standard_Real Prosca       = Nor.Dot(aPPC);
  
  if (point.IsEqual(PC,Precision::Confusion())) {
    if (isConvex) { return 0.;}
    //----------------------------------------------------
    // the point is on a concave curve. 
    // The required point is not the common point.
    // This can avoid the discontinuity of the bisectrice.
    //----------------------------------------------------
    else { return Precision::Infinite();} 
  }

  if (Abs(Prosca) < Precision::Confusion() || (Prosca*sign) > 0. ) {
    return Precision::Infinite();
  }
  else {    
    Standard_Real A  = 0.5*SquareMagPPC/Prosca;
    Standard_Real Dist = A*A*NorNor;
#ifdef OCCT_DEBUG_CUR
    //----------------------------------------
    // Test Curvature if the curve is concave.
    //----------------------------------------
    if (!isConvex){
      Standard_Real K  = Curvature(curve,UOnCurve,Precision::Confusion());
      if (K  != 0.) {
        if (Dist > 1/(K*K)) { Dist = Precision::Infinite();}
      }
    }
#endif
    return Dist;
  }
}



//=============================================================================
//function : D0
// purpose : 
//=============================================================================
void Bisector_BisecPC::D0(const Standard_Real     U,
			        gp_Pnt2d&         P) const
{
  P = point;
  gp_Vec2d V1,V2,V3;
  Values(U,0,P,V1,V2,V3);
}


//=============================================================================
//function : D1
// purpose : 
//=============================================================================
void Bisector_BisecPC::D1(const Standard_Real     U,
			        gp_Pnt2d&         P,
			        gp_Vec2d&         V ) const
{
  P = point;
  V.SetCoord(0.,0.);
  gp_Vec2d V2,V3;
  Values(U,1,P,V,V2,V3);
}


//=============================================================================
//function : D2
// purpose : 
//=============================================================================
void Bisector_BisecPC::D2(const Standard_Real     U,
			        gp_Pnt2d&         P,
			        gp_Vec2d&         V1,
			        gp_Vec2d&         V2) const
{
  P = point;
  V1.SetCoord(0.,0.);
  V2.SetCoord(0.,0.);
  gp_Vec2d V3;
  Values(U,2,P,V1,V2,V3);
}

//=============================================================================
//function : D3
// purpose : 
//=============================================================================
void Bisector_BisecPC::D3(const Standard_Real     U,
			        gp_Pnt2d&         P,
			        gp_Vec2d&         V1,
			        gp_Vec2d&         V2,
			        gp_Vec2d&         V3) const
{
  P = point;
  V1.SetCoord(0.,0.);
  V2.SetCoord(0.,0.);
  V3.SetCoord(0.,0.);
  Values(U,3,P,V1,V2,V3);
}


//=============================================================================
//function : DN
// purpose : 
//=============================================================================
gp_Vec2d Bisector_BisecPC::DN (const Standard_Real     U,
			       const Standard_Integer  N) const
{
  gp_Pnt2d P = point;
  gp_Vec2d V1(0.,0.);
  gp_Vec2d V2(0.,0.);
  gp_Vec2d V3(0.,0.);
  Values (U,N,P,V1,V2,V3);
  switch (N) {
  case 1 : return V1;
  case 2 : return V2;
  case 3 : return V3;
  default: {
    throw Standard_NotImplemented();
    }
  }
}

//=============================================================================
//function : SearchBound
// purpose : 
//=============================================================================
Standard_Real Bisector_BisecPC::SearchBound (const Standard_Real U1,
					     const Standard_Real U2) const
{
  Standard_Real Dist1,DistMid,U11,U22; 
  Standard_Real UMid = 0.;
  Standard_Real Tol      = Precision::PConfusion();
  Standard_Real DistMax2 = distMax*distMax;
  U11 = U1; U22 = U2;
  Dist1 = Distance(U11);
  
  while ((U22 - U11) > Tol) {
    UMid    = 0.5*( U22 + U11);
    DistMid = Distance(UMid);
    if ((Dist1 > DistMax2) == (DistMid > DistMax2)) {
      U11    = UMid;
      Dist1 = DistMid;
    }
    else {
      U22    = UMid;
    }
  }
  return UMid;
}

//=============================================================================
//function : CuspFilter
// purpose : 
//=============================================================================
void Bisector_BisecPC::CuspFilter()
{
  throw Standard_NotImplemented();
}

//=============================================================================
//function : ComputeIntervals
// purpose : 
//=============================================================================
void Bisector_BisecPC::ComputeIntervals ()
{
  Standard_Real U1 =0.,U2 =0.,UProj =0.;
  Standard_Real UStart = 0., UEnd = 0.;
  Standard_Real Dist1,Dist2,DistProj;
  isEmpty        = Standard_False;     
  shiftParameter = 0.;
  Standard_Boolean YaProj   = Standard_False;
  Standard_Real    DistMax2 = distMax*distMax;

  U1 = curve->FirstParameter();
  U2 = curve->LastParameter();
  Dist1    = Distance(U1);
  Dist2    = Distance(U2);
  DistProj = Precision::Infinite();

  Geom2dAPI_ProjectPointOnCurve Proj(point,curve,U1,U2);
  if (Proj.NbPoints() > 0) {
    UProj    = Proj.LowerDistanceParameter();
    DistProj = Distance(UProj);
    YaProj   = Standard_True;
  } 

  if (Dist1 < DistMax2 && Dist2 < DistMax2) {
    if (DistProj > DistMax2 && YaProj) {
      isEmpty = Standard_True;
    }
    else {
      startIntervals.Append(U1);
      endIntervals  .Append(U2);
    }
    return;
  }
  else if (Dist1 > DistMax2 && Dist2 > DistMax2) {
    if (DistProj < DistMax2) {
      UStart = SearchBound(U1,UProj);
      UEnd   = SearchBound(UProj,U2);
    }
     else {
      isEmpty = Standard_True;
      return;
    }
  }
  else if (Dist1 < DistMax2) {
    UStart = U1;
    UEnd = SearchBound(U1,U2);
  }
  else if (Dist2 < DistMax2) {
    UEnd = U2;
    UStart = SearchBound(U1,U2);
  }
  startIntervals.Append(UStart);
  endIntervals  .Append(UEnd);

  //------------------------------------------------------------------------
  // Eventual offset of the parameter on the curve correspondingly to the one 
  // on the curve. The offset can be done if the curve is periodical and the 
  // point of initial parameter is less then the interval of continuity.
  //------------------------------------------------------------------------
  if (curve->IsPeriodic()) {
    if (startIntervals.Length() > 1) {               // Plusieurs intervals.
      if (endIntervals  .Last()  == curve->LastParameter() &&
	  startIntervals.First() == curve->FirstParameter()   ) {
	//---------------------------------------------------------------
	// the bissectrice is defined at the origin.
	// => Fusion of the first and the last interval.
	// => 0 on the bisectrice becomes the start of the first interval
	// => offset of parameter on all limits of intervals.
	//---------------------------------------------------------------
	startIntervals.Remove(1);
	endIntervals  .Remove(endIntervals.Length());
	
	shiftParameter = Period() - startIntervals.First() ;
	for (Standard_Integer k = 1; k <= startIntervals.Length(); k++) {
	  endIntervals  .ChangeValue(k) += shiftParameter;
	  startIntervals.ChangeValue(k) += shiftParameter;
	}
	startIntervals.ChangeValue(1) = 0.;
      }
    }
  }
}

//=============================================================================
//function : LinkBisCurve
//purpose  : 
//=============================================================================
Standard_Real Bisector_BisecPC::LinkBisCurve(const Standard_Real U) const 
{
  return (U - shiftParameter);
} 

//=============================================================================
//function : LinkCurveBis
//purpose  : 
//=============================================================================
Standard_Real Bisector_BisecPC::LinkCurveBis(const Standard_Real U) const 
{
  return (U + shiftParameter);
} 

//=============================================================================
//function : IsEmpty
//purpose  : 
//=============================================================================
Standard_Boolean Bisector_BisecPC::IsEmpty() const
{
  return isEmpty;
}

//==========================================================================
//function : Parameter
//purpose  :
//==========================================================================
Standard_Real Bisector_BisecPC::Parameter(const gp_Pnt2d& P) const
{
  Standard_Real    Tol     = Precision::Confusion();

  if (P.IsEqual(pointStartBis,Tol)) {return startIntervals.Value(bisInterval);}
  if (P.IsEqual(pointEndBis  ,Tol)) {return endIntervals  .Value(bisInterval);}

  if (extensionStart) {
    gp_Ax2d Axe(pointStartBis,gp_Dir2d(pointStartBis.X() - P.X(),
				       pointStartBis.Y() - P.Y()));
    Standard_Real U    = ElCLib::LineParameter(Axe,P);
    gp_Pnt2d      Proj = ElCLib::LineValue(U,Axe);
    if (Proj.IsEqual(P,Tol) && U < 0.) {
      return U + startIntervals.Value(bisInterval);
    }
  }
  if (extensionEnd)   {    
    gp_Ax2d Axe(pointEndBis,gp_Dir2d(P.X() - pointEndBis.X(),
				     P.Y() - pointEndBis.Y()));
    Standard_Real U    = ElCLib::LineParameter(Axe,P);
    gp_Pnt2d      Proj = ElCLib::LineValue(U,Axe);
    if (Proj.IsEqual(P,Tol) && U > 0.) {
      return U + endIntervals.Value(bisInterval);
    }
  }
  Standard_Real                 UOnCurve = 0.;
  Geom2dAPI_ProjectPointOnCurve Proj(P,curve,
				     curve->FirstParameter(),curve->LastParameter());
  if (Proj.NbPoints() > 0) {
    UOnCurve = Proj.LowerDistanceParameter();
  }
  return LinkCurveBis(UOnCurve);
}


//=============================================================================
//function : Indent
// purpose : 
//=============================================================================
static void Indent(const Standard_Integer Offset) {
  if (Offset > 0) {
    for (Standard_Integer i = 0; i < Offset; i++) {cout << " ";}
  }
}

//=============================================================================
//function : Init
// purpose : 
//=============================================================================
void Bisector_BisecPC::Init (const Handle(Geom2d_Curve)&    Curve, 
			     const gp_Pnt2d&               Point, 
			     const Standard_Real           Sign, 
			     const TColStd_SequenceOfReal& StartIntervals, 
			     const TColStd_SequenceOfReal& EndIntervals, 
			     const Standard_Integer        BisInterval, 
			     const Standard_Integer        CurrentInterval, 
			     const Standard_Real           ShiftParameter, 
			     const Standard_Real           DistMax, 
			     const Standard_Boolean        IsEmpty, 
			     const Standard_Boolean        IsConvex, 
			     const Standard_Boolean        ExtensionStart, 
			     const Standard_Boolean        ExtensionEnd, 
			     const gp_Pnt2d&               PointStartBis, 
			     const gp_Pnt2d&               PointEndBis)
{
  curve           = Curve;
  point           = Point; 
  sign            = Sign ;
  startIntervals  = StartIntervals; 
  endIntervals    = EndIntervals;
  bisInterval     = BisInterval;
  currentInterval = CurrentInterval;
  shiftParameter  = ShiftParameter;
  distMax         = DistMax;
  isEmpty         = IsEmpty;
  isConvex        = IsConvex;
  extensionStart  = ExtensionStart;
  extensionEnd    = ExtensionEnd;
  pointStartBis   = PointStartBis;
  pointEndBis     = PointEndBis;   
}

//=============================================================================
//function : Dump
// purpose : 
//=============================================================================
//void Bisector_BisecPC::Dump(const Standard_Integer Deep, 
void Bisector_BisecPC::Dump(const Standard_Integer , 
			    const Standard_Integer Offset) const 
{
  Indent (Offset);
  cout <<"Bisector_BisecPC :"<<endl;
  Indent (Offset);
  cout <<"Point :"<<endl;
  cout <<" X = "<<point.X()<<endl;
  cout <<" Y = "<<point.Y()<<endl;
  cout <<"Sign  :"<<sign<<endl;
  cout <<"Number Of Intervals :"<<startIntervals.Length()<<endl;
  for (Standard_Integer i = 1; i <= startIntervals.Length(); i++) {
    cout <<"Interval number :"<<i<<"Start :"<<startIntervals.Value(i)
                                 <<"  end :"<<  endIntervals.Value(i)<<endl ;
  }
  cout <<"Index Current Interval :"<<currentInterval<<endl;
}






