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
#include <Bisector_BisecCC.hxx>
#include <Bisector_BisecPC.hxx>
#include <Bisector_Curve.hxx>
#include <Bisector_FunctionH.hxx>
#include <Bisector_PointOnBis.hxx>
#include <Bisector_PolyBis.hxx>
#include <Geom2d_Circle.hxx>
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
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <math_BissecNewton.hxx>
#include <math_FunctionRoot.hxx>
#include <math_FunctionRoots.hxx>
#include <Precision.hxx>
#include <Standard_DivideByZero.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Bisector_BisecCC,Bisector_Curve)

static Standard_Boolean ProjOnCurve (const gp_Pnt2d& P,
  const Handle(Geom2d_Curve)& C,
  Standard_Real& theParam);

static Standard_Real    Curvature (const Handle(Geom2d_Curve)& C,
  Standard_Real         U,
  Standard_Real         Tol) ;

static Standard_Boolean TestExtension (const Handle(Geom2d_Curve)& C1,
  const Handle(Geom2d_Curve)& C2,
  const Standard_Integer      Start_End);

static Standard_Boolean DiscretPar(const Standard_Real     DU, 
  const Standard_Real     EpsMin, 
  const Standard_Real     EpsMax,
  const Standard_Integer  NbMin,
  const Standard_Integer  NbMax,
  Standard_Real&    Eps,
  Standard_Integer& Nb);

//=============================================================================
//function :
//purpose  :
//=============================================================================
Bisector_BisecCC::Bisector_BisecCC()
{
  shiftParameter = 0;
  isEmpty        = Standard_False;
}

//=============================================================================
//function :
//purpose  :
//=============================================================================
Bisector_BisecCC::Bisector_BisecCC(const Handle(Geom2d_Curve)& Cu1,
  const Handle(Geom2d_Curve)& Cu2,
  const Standard_Real         Side1,
  const Standard_Real         Side2,
  const gp_Pnt2d&             Origin,
  const Standard_Real         DistMax)
{
  Perform (Cu1,Cu2,Side1,Side2,Origin,DistMax);
}

//=============================================================================
//function : Perform
//purpose  :
//=============================================================================
void  Bisector_BisecCC::Perform(const Handle(Geom2d_Curve)& Cu1,
  const Handle(Geom2d_Curve)& Cu2,
  const Standard_Real         Side1,
  const Standard_Real         Side2,
  const gp_Pnt2d&             Origin,				
  const Standard_Real         DistMax)
{
  isEmpty = Standard_False;
  distMax = DistMax;

  curve1  = Handle (Geom2d_Curve)::DownCast(Cu1->Copy());
  curve2  = Handle (Geom2d_Curve)::DownCast(Cu2->Copy());

  sign1 = Side1;
  sign2 = Side2;

  isConvex1 = Bisector::IsConvex(curve1,sign1);
  isConvex2 = Bisector::IsConvex(curve2,sign2);

  Standard_Real    U,UC1,UC2,Dist,dU,USol;  
  gp_Pnt2d         P;
  Standard_Integer NbPnts    = 21;
  Standard_Real    EpsMin    = 10*Precision::Confusion();
  Standard_Boolean YaPoly    = Standard_True;
  Standard_Boolean OriInPoly = Standard_False;
  //---------------------------------------------
  // Calculate first point of the polygon.
  //---------------------------------------------
  Standard_Boolean isProjDone = ProjOnCurve (Origin,curve1, U);

  if(!isProjDone)
  {
    isEmpty = Standard_True;
    return;
  }

  P = ValueByInt  (U,UC1,UC2,Dist);
  if(Dist < Precision::Confusion())
  {
    gp_Pnt2d aP1 = curve1->Value(UC1);
    gp_Pnt2d aP2 = curve2->Value(UC2);
    Standard_Real dp = (aP1.Distance(P)+aP2.Distance(P));
    Standard_Real dorig = (aP1.Distance(Origin)+aP2.Distance(Origin));
    if(dp < dorig)
    {
      isEmpty = Standard_True;
      return;
    }
  }

  if (Dist < Precision::Infinite()) {
    //----------------------------------------------------
    // the parameter of the origin point gives a point 
    // on the polygon.
    //----------------------------------------------------
    myPolygon.Append(Bisector_PointOnBis(UC1,UC2,U,Dist,P));     
    startIntervals.Append(U);
    if (P.IsEqual(Origin,Precision::Confusion())) {    
      //----------------------------------------
      // test if the first point is the origin.
      //----------------------------------------
      OriInPoly = Standard_True;
    }
  }
  else {
    //-------------------------------------------------------
    // The origin point is on the extension.
    // Find the first point of the polygon by dichotomy.
    //-------------------------------------------------------
    dU     = (curve1->LastParameter() - U)/(NbPnts - 1); 
    U += dU;
    for (Standard_Integer i = 1; i <= NbPnts - 1; i++) {
      P    = ValueByInt(U,UC1,UC2,Dist);
      if (Dist < Precision::Infinite()) {
        USol = SearchBound(U - dU,U);    
        P    = ValueByInt(USol,UC1,UC2,Dist);	
        startIntervals.Append(USol);
        myPolygon.Append(Bisector_PointOnBis(UC1,UC2,USol,Dist,P));
        break;
      }
      U += dU;
    }
  }

  if ( myPolygon.Length() != 0 ) {
    SupLastParameter();
    //----------------------------------------------
    // Construction of the polygon of the bissectrice.
    //---------------------------------------------
    U                = FirstParameter();
    Standard_Real DU = LastParameter() - U;

    if (DU < EpsMin) {NbPnts = 3;} 
    dU = DU/(NbPnts - 1);

    U += dU;
    //  modified by NIZHNY-EAP Fri Jan 21 09:33:20 2000 ___BEGIN___
    //  prevent addition of the same point
    gp_Pnt2d prevPnt = P;
    for (Standard_Integer i = 1; i <= NbPnts - 1; i++) {
      P    = ValueByInt(U,UC1,UC2,Dist);
      if (Dist < Precision::Infinite()) {
        if (P.Distance (prevPnt) > Precision::Confusion())
          myPolygon.Append(Bisector_PointOnBis(UC1,UC2,U,Dist,P));
      }
      else {
        USol = SearchBound(U - dU,U);
        P    = ValueByInt(USol,UC1,UC2,Dist);      
        endIntervals.SetValue(1,USol);
        if (P.Distance (prevPnt) > Precision::Confusion())
          myPolygon.Append(Bisector_PointOnBis(UC1,UC2,USol,Dist,P));
        break;
      }
      U += dU;
      prevPnt=P;
      //  modified by NIZHNY-EAP Fri Jan 21 09:33:24 2000 ___END___
    }  
  }
  else {
    //----------------
    // Empty Polygon.    
    //----------------
    YaPoly = Standard_False;
  }

  extensionStart = Standard_False;
  extensionEnd   = Standard_False;
  pointStart     = Origin;

  if (isConvex1 && isConvex2) {
    if (YaPoly) pointEnd = myPolygon.Last().Point();
  }
  else {
    //-----------------------------------------------------------------------------
    // Extension : The curve is extended at the beginning and/or the end if
    //                - one of two curves is concave.
    //                - the curves have a common point at the beginning and/or the end
    //                - the angle of opening at the common point between two curves
    //                  values M_PI.
    // the extension at the beginning  is taken into account if the origin is found above.
    // ie : the origin is not the in the polygon.
    //-----------------------------------------------------------------------------

    //---------------------------------
    // Do the extensions exist ?
    //---------------------------------
    if (OriInPoly) {
      extensionStart = Standard_False;
    }
    else {
      extensionStart = TestExtension(curve1,curve2,1);
    }
    extensionEnd     = TestExtension(curve1,curve2,2);

    //-----------------
    // Calculate pointEnd.
    //-----------------
    if (extensionEnd) {
      pointEnd = curve1->Value(curve1->LastParameter());
    }
    else if (YaPoly) {
      pointEnd = myPolygon.Last().Point();
    }
    else {
      ComputePointEnd();
    }
    //------------------------------------------------------
    // Update the Limits of intervals of  definition.
    //------------------------------------------------------
    if (YaPoly) {
      if (extensionStart) {
        gp_Pnt2d       P1     = myPolygon.First().Point();
        Standard_Real  UFirst = startIntervals.First() - pointStart.Distance(P1);
        startIntervals.InsertBefore(1,UFirst);
        endIntervals  .InsertBefore(1,startIntervals.Value(2));
      }
      if (extensionEnd) {
        gp_Pnt2d       P1;
        Standard_Real  UFirst,ULast;
        P1     = myPolygon.Last().Point();
        UFirst = endIntervals.Last();      
        ULast  = UFirst + pointEnd.Distance(P1);
        startIntervals.Append(UFirst);
        endIntervals  .Append(ULast );
      }
    }
    else {
      //--------------------------------------------------
      // No polygon => the bissectrice is a segment.
      //--------------------------------------------------
      startIntervals.Append(0.);
      endIntervals  .Append(pointEnd.Distance(pointStart));
    }
  }
  if (!YaPoly && !extensionStart && !extensionEnd) 
    isEmpty = Standard_True;
  //  modified by NIZHNY-EAP Mon Jan 17 17:32:40 2000 ___BEGIN___
  if (myPolygon.Length() <= 2)
    isEmpty = Standard_True;
  //  modified by NIZHNY-EAP Mon Jan 17 17:32:42 2000 ___END___
}

//=============================================================================
//function : IsExtendAtStart
//purpose  :
//=============================================================================
Standard_Boolean Bisector_BisecCC::IsExtendAtStart() const
{
  return extensionStart;
}

//=============================================================================
//function : IsExtendAtEnd
//purpose  :
//=============================================================================
Standard_Boolean Bisector_BisecCC::IsExtendAtEnd() const
{
  return extensionEnd;
}

//=============================================================================
//function : IsEmpty
//purpose :
//=============================================================================
Standard_Boolean Bisector_BisecCC::IsEmpty() const
{
  return isEmpty;
}

//=============================================================================
//function : Reverse
//purpose  :
//=============================================================================
void Bisector_BisecCC::Reverse()
{
  throw Standard_NotImplemented();
}

//=============================================================================
//function : ReversedParameter
// purpose :
//=============================================================================
Standard_Real  Bisector_BisecCC::ReversedParameter(const Standard_Real U) const
{ 
  return LastParameter() + FirstParameter() - U;
}

//=============================================================================
//function : Copy
//purpose  :
//=============================================================================
Handle(Geom2d_Geometry) Bisector_BisecCC::Copy() const
{
  Handle(Geom2d_Curve) CopyCurve1
    = Handle(Geom2d_Curve)::DownCast(curve1->Copy());
  Handle(Geom2d_Curve) CopyCurve2
    = Handle(Geom2d_Curve)::DownCast(curve2->Copy());

  Handle(Bisector_BisecCC) C = new Bisector_BisecCC();

  C -> Curve    (1, CopyCurve1)   ; C -> Curve    (2, CopyCurve2);
  C -> Sign     (1, sign1 )       ; C -> Sign     (2, sign2 );
  C -> IsConvex (1, isConvex1)    ; C -> IsConvex (2, isConvex2);
  C -> Polygon  (myPolygon);
  C -> IsEmpty  (isEmpty)  ;
  C -> DistMax  (distMax)  ;
  C -> StartIntervals (startIntervals); C -> EndIntervals (endIntervals);
  C -> ExtensionStart (extensionStart); C -> ExtensionEnd (extensionEnd);
  C -> PointStart     (pointStart)    ; C -> PointEnd     (pointEnd)    ;

  return C;  
}

//=============================================================================
//function : ChangeGuide
//purpose  : Changet of the guideline for the parameters of the bissectrice
//           ATTENTION : - This can invert the direction  of parameterization.
//                       - This concerns only the part of the curve
//                         corresponding to the polygon.
//=============================================================================
Handle(Bisector_BisecCC) Bisector_BisecCC::ChangeGuide() const
{
  Handle(Bisector_BisecCC) C = new Bisector_BisecCC();

  C -> Curve    (1, curve2)   ; C -> Curve    (2, curve1);
  C -> Sign     (1, sign2 )   ; C -> Sign     (2, sign1 );
  C -> IsConvex (1, isConvex2); C -> IsConvex (2, isConvex1); 

  //-------------------------------------------------------------------------
  // Construction of the new polygon from the initial one.
  // inversion of PointOnBis and Calculation of new parameters on the bissectrice.
  //-------------------------------------------------------------------------
  Bisector_PolyBis Poly;
  if (sign1 == sign2 ) {
    //---------------------------------------------------------------
    // elements of the new polygon are ranked in the other direction.
    //---------------------------------------------------------------
    for (Standard_Integer i = myPolygon.Length(); i >=1; i--) {
      Bisector_PointOnBis P = myPolygon.Value(i);
      Bisector_PointOnBis NewP (P.ParamOnC2(), P.ParamOnC1(), 
        P.ParamOnC2(), P.Distance (),
        P.Point());
      Poly.Append (NewP);
    }
  }
  else {
    for (Standard_Integer i = 1; i <= myPolygon.Length(); i ++) {      
      Bisector_PointOnBis P = myPolygon.Value(i);
      Bisector_PointOnBis NewP (P.ParamOnC2(), P.ParamOnC1(), 
        P.ParamOnC2(), P.Distance (),
        P.Point());
      Poly.Append (NewP);
    }
  }
  C -> Polygon        (Poly);
  C -> FirstParameter (Poly.First().ParamOnBis());
  C -> LastParameter  (Poly.Last() .ParamOnBis());

  return C;  
}

//=============================================================================
//function : Transform
//purpose  :
//=============================================================================
void Bisector_BisecCC::Transform (const gp_Trsf2d& T)
{
  curve1    ->Transform(T);
  curve2    ->Transform(T);
  myPolygon . Transform(T);
  pointStart. Transform(T);
  pointEnd  . Transform(T);
}

//=============================================================================
//function : IsCN
//purpose  :
//=============================================================================
Standard_Boolean Bisector_BisecCC::IsCN (const Standard_Integer N) const 
{
  return (curve1->IsCN(N+1) && curve2->IsCN(N+1));
}

//=============================================================================
//function : FirstParameter
//purpose :
//=============================================================================
Standard_Real Bisector_BisecCC::FirstParameter() const
{
  return startIntervals.First();
}

//=============================================================================
//function : LastParameter
//purpose  :
//=============================================================================
Standard_Real Bisector_BisecCC::LastParameter() const
{
  return endIntervals.Last();
}

//=============================================================================
//function : Continuity
//purpose  :
//=============================================================================
GeomAbs_Shape Bisector_BisecCC::Continuity() const 
{
  GeomAbs_Shape Cont = curve1->Continuity();
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
Standard_Integer Bisector_BisecCC::NbIntervals() const
{
  return startIntervals.Length();
}

//=============================================================================
//function : IntervalFirst
//purpose  :
//=============================================================================
Standard_Real Bisector_BisecCC::IntervalFirst(const Standard_Integer Index) const
{
  return startIntervals.Value(Index);
}

//=============================================================================
//function : IntervalLast
//purpose  :
//=============================================================================
Standard_Real Bisector_BisecCC::IntervalLast(const Standard_Integer Index) const
{
  return endIntervals.Value(Index);
}

//=============================================================================
//function : IntervalContinuity
//purpose  :
//=============================================================================
GeomAbs_Shape Bisector_BisecCC::IntervalContinuity() const
{
  GeomAbs_Shape Cont = curve1->Continuity();
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
Standard_Boolean Bisector_BisecCC::IsClosed() const
{
  if (curve1->IsClosed()) {
    if (startIntervals.First() == curve1->FirstParameter() &&
      endIntervals  .Last () == curve1->LastParameter ()    )
      return Standard_True;
  }
  return Standard_False;
}

//=============================================================================
//function : IsPeriodic
//purpose  :
//=============================================================================
Standard_Boolean Bisector_BisecCC::IsPeriodic() const
{
  return Standard_False;
}


//=============================================================================
//function : Curvature
//purpose  :
//=============================================================================
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

//=============================================================================
//function : Value
//purpose  : CALCULATE THE CURRENT POINT BY ITERATIVE METHOD.
//           ----------------------------------------------
//           Calculate the current point, the distance from the current point to
//           both curves, the parameters on each curve of the projection  
//           of the current point.
//
//method  : - Find start parameter by using <myPolygon>.
//          - Calculate parameter U2 on curve C2 solution of H(U,V)= 0
//          - P(U) = F(U,U2)
//
//          or :
//                                 ||P2(v0)P1(u)||**2
//           F(u,v) = P1(u) - 1/2 *----------------* N(u)
//                                (N(u).P2(v0)P1(u))
//
//           H(u,v) = (Tu.P1(u)P2(v))**2||Tv||**2 - (Tv.P1(u)P2(v))**2||Tu||**2
//
//=============================================================================
gp_Pnt2d Bisector_BisecCC::ValueAndDist (const Standard_Real  U,
  Standard_Real& U1,
  Standard_Real& U2,
  Standard_Real& Dist) const
{	
  gp_Vec2d T;  

  //-----------------------------------------------
  // is the polygon reduced to a point or empty?
  //-----------------------------------------------
  if (myPolygon.Length() <= 1) {
    return Extension(U,U1,U2,Dist,T);
  }

  //-----------------------------------------------
  // test U out of the limits of the polygon.
  //-----------------------------------------------
  if (U  <  myPolygon.First().ParamOnBis()) {
    return Extension(U,U1,U2,Dist,T);
  }
  if (U >  myPolygon.Last().ParamOnBis()) {
    return Extension(U,U1,U2,Dist,T);
  }

  //-------------------------------------------------------
  // Find start parameter by using <myPolygon>.
  //-------------------------------------------------------
  Standard_Integer IntervalIndex = myPolygon.Interval(U);
  Standard_Real    UMin = myPolygon.Value(IntervalIndex    ).ParamOnBis();
  Standard_Real    UMax = myPolygon.Value(IntervalIndex + 1).ParamOnBis();
  Standard_Real    VMin = myPolygon.Value(IntervalIndex    ).ParamOnC2();
  Standard_Real    VMax = myPolygon.Value(IntervalIndex + 1).ParamOnC2(); 
  Standard_Real    Alpha,VInit;

  if (Abs(UMax - UMin) < gp::Resolution()) {
    VInit = VMin;
  }
  else {
    Alpha = (U - UMin)/(UMax - UMin);
    VInit = VMin + Alpha*(VMax - VMin);
  }

  U1 = LinkBisCurve(U);
  Standard_Real   VTemp = Min(VMin,VMax); 
  VMax = Max(VMin,VMax); VMin = VTemp;
  Standard_Boolean Valid = Standard_True; 
  //---------------------------------------------------------------
  // Calculate parameter U2 on curve C2 solution of H(u,v)=0     
  //---------------------------------------------------------------
  gp_Pnt2d P1;
  gp_Vec2d T1;
  Standard_Real    EpsH     = 1.E-9;
  Standard_Real    EpsH100  = 1.E-7; 
  curve1->D1 (U1,P1,T1);
  gp_Vec2d N1(T1.Y(), - T1.X());

  if ((VMax - VMin) < Precision::PConfusion()) {
    U2 = VInit;
  }
  else {
    Bisector_FunctionH H  (curve2,P1,sign1*sign2*T1);
    Standard_Real FInit;
    H.Value(VInit,FInit);
    if (Abs(FInit) < EpsH) {
      U2 = VInit;
    }
    else
    {
      math_BissecNewton aNewSolution(EpsH);
      aNewSolution.Perform(H, VMin - EpsH100, VMax + EpsH100, 10);

      if (aNewSolution.IsDone())
      {
        U2 = aNewSolution.Root();
      }
      else
      {
        math_FunctionRoot SolRoot (H,VInit,EpsH,VMin - EpsH100,VMax + EpsH100);
        
        if (SolRoot.IsDone())
          U2    = SolRoot.Root();
        else
          Valid = Standard_False;
      }
    }
  }

  gp_Pnt2d PBis = pointStart;
  //----------------
  // P(U) = F(U1,U2)
  //----------------
  if (Valid) {
    gp_Pnt2d P2 = curve2->Value(U2);
    gp_Vec2d P2P1(P1.X() - P2.X(),P1.Y() - P2.Y());
    Standard_Real SquareP2P1 = P2P1.SquareMagnitude();
    Standard_Real N1P2P1     = N1.Dot(P2P1);
    const Standard_Real anEps = Epsilon(1);

    if (P1.IsEqual(P2,Precision::Confusion())) {
      PBis = P1 ;
      Dist = 0.0;
    }
    else if (N1P2P1*sign1 < anEps) {
      Valid = Standard_False;
    }
    else {	
      PBis = P1.Translated(- (0.5*SquareP2P1/N1P2P1)*N1);
      Dist = P1.SquareDistance(PBis);
    }
  }

  //----------------------------------------------------------------
  // If the point is not valid 
  // calculate by intersection.
  //----------------------------------------------------------------
  if (!Valid) {
    //--------------------------------------------------------------------
    // Construction of the bisectrice point curve and of the straight line passing
    // by P1 and carried by the normal. curve2 is limited by VMin and VMax.
    //--------------------------------------------------------------------
    Standard_Real DMin = Precision::Infinite();
    gp_Pnt2d      P;
    Handle(Bisector_BisecPC) BisPC 
      = new Bisector_BisecPC(curve2, P1, sign2, VMin, VMax);
    Handle(Geom2d_Line)      NorLi = new Geom2d_Line (P1,N1);

    Geom2dAdaptor_Curve ABisPC(BisPC);
    Geom2dAdaptor_Curve ANorLi(NorLi);    
    //-------------------------------------------------------------------------
    Geom2dInt_GInter  Intersect(ABisPC,ANorLi,
      Precision::Confusion(),Precision::Confusion());
    //-------------------------------------------------------------------------

    if (Intersect.IsDone() && !Intersect.IsEmpty()) {
      for (Standard_Integer i = 1; i <= Intersect.NbPoints(); i++) {
        if (Intersect.Point(i).ParamOnSecond()*sign1 < Precision::PConfusion()) {
          P  = Intersect.Point(i).Value();
          if (P.SquareDistance(P1) < DMin) {
            DMin = P.SquareDistance(P1);
            PBis = P;
            U2   = BisPC->LinkBisCurve(Intersect.Point(i).ParamOnFirst());
            Dist = DMin;
          }
        }
      }
    }
  } 
  return   PBis;  
}

//=============================================================================
//function : ValueByInt
//purpose  : CALCULATE THE CURRENT POINT BY INTERSECTION.
//           -------------------------------------------
//           Calculate the current point, the distance from the current point
//           to two curves, the parameters on each curve of the projection of the 
//           current point.
//           the current point with parameter U is the intersection of the 
//           bissectrice point curve (P1,curve2) and of the straight line 
//           passing through P1 of director vector N1.
//           P1 is the current point of parameter U on curve1 and N1 the  
//           normal at this point.
//=============================================================================
gp_Pnt2d Bisector_BisecCC::ValueByInt (const Standard_Real  U,
  Standard_Real& U1,
  Standard_Real& U2,
  Standard_Real& Dist) const
{
  //------------------------------------------------------------------
  // Return point, tangent, normal on C1 at parameter U.
  //-------------------------------------------------------------------
  U1 = LinkBisCurve(U);

  gp_Pnt2d P1,P2,P,PSol;
  gp_Vec2d Tan1,Tan2;
  curve1->D1(U1,P1,Tan1);
  gp_Vec2d N1(  Tan1.Y(), - Tan1.X());

  //--------------------------------------------------------------------------
  // test confusion of P1 with extremity of curve2.
  //--------------------------------------------------------------------------
  if (P1.Distance(curve2->Value(curve2->FirstParameter())) < Precision::Confusion()) {
    U2   = curve2->FirstParameter();
    curve2->D1(U2,P2,Tan2);	  
    if ( isConvex1 && isConvex2 ) {
      Dist = 0.;
      return P1;
    }
    if (! Tan1.IsParallel(Tan2,Precision::Angular())) {
      Dist = 0.;
      return P1;
    }
  }
  if (P1.Distance(curve2->Value(curve2->LastParameter())) < Precision::Confusion()) {
    U2   = curve2->LastParameter();  
    curve2->D1(U2,P2,Tan2);    
    if ( isConvex1 && isConvex2 ) {
      Dist = 0.;
      return P1;
    }
    if (! Tan1.IsParallel(Tan2,Precision::Angular())) {  
      Dist = 0.;
      return P1;
    }
  }

  Standard_Boolean YaSol = Standard_False;
  Standard_Real    DMin  = Precision::Infinite();
  Standard_Real    USol;
  Standard_Real    EpsMax = 1.E-6;
  Standard_Real    EpsX;
  Standard_Real    EpsH   = 1.E-8;
  Standard_Real    DistPP1;
  Standard_Integer NbSamples =20;
  Standard_Real    UFirstOnC2 = curve2->FirstParameter();
  Standard_Real    ULastOnC2  = curve2->LastParameter();

  if (!myPolygon.IsEmpty()){
    if (sign1 == sign2) { ULastOnC2  = myPolygon.Last().ParamOnC2();}
    else                { UFirstOnC2 = myPolygon.Last().ParamOnC2();} 
  }

  if (Abs(ULastOnC2 - UFirstOnC2) < Precision::PConfusion()/100.) {
    Dist = Precision::Infinite();
    return P1;
  }

  DiscretPar(Abs(ULastOnC2 - UFirstOnC2),EpsH,EpsMax,2,20,EpsX,NbSamples);

  Bisector_FunctionH H  (curve2,P1,sign1*sign2*Tan1);
  math_FunctionRoots SolRoot (H,
    UFirstOnC2,
    ULastOnC2 ,
    NbSamples,
    EpsX,EpsH,EpsH);
  if (SolRoot.IsDone()) {
    for (Standard_Integer j = 1; j <= SolRoot.NbSolutions(); j++) {
      USol        = SolRoot.Value(j);
      gp_Pnt2d P2Curve2 = curve2->Value(USol);
      gp_Vec2d P2P1(P1.X() - P2Curve2.X(),P1.Y() - P2Curve2.Y());
      Standard_Real SquareP2P1 = P2P1.SquareMagnitude();
      Standard_Real N1P2P1     = N1.Dot(P2P1);

      // Test if the solution is at the proper side of the curves.
      if (N1P2P1*sign1 > 0 ) {
        P       = P1.Translated(- (0.5*SquareP2P1/N1P2P1)*N1);
        DistPP1 = P1.SquareDistance(P);
        if (DistPP1  < DMin) {
          DMin  = DistPP1;
          PSol  = P;
          U2    = USol;
          YaSol = Standard_True;
        }
      }
    }
  }

  /*
  if (!YaSol) {
  //--------------------------------------------------------------------
  // Construction de la bisectrice point courbe et de la droite passant
  // par P1 et portee par la normale.
  //--------------------------------------------------------------------
  Handle(Bisector_BisecPC) BisPC 
  = new Bisector_BisecPC(curve2,P1,sign2,2*distMax);
  //-------------------------------
  // Test si la bissectrice existe.
  //-------------------------------
  if (BisPC->IsEmpty()) {
  Dist = Precision::Infinite();
  PSol = P1;
  return PSol;
  }

  Handle(Geom2d_Line)      NorLi  = new Geom2d_Line (P1,N1);
  Geom2dAdaptor_Curve      NorLiAd;
  if (sign1 < 0.) {NorLiAd.Load(NorLi,0.       ,distMax);}
  else            {NorLiAd.Load(NorLi,- distMax,0.     );}

  //-------------------------------------------------------------------------
  Geom2dInt_GInter  Intersect(BisPC,NorLiAd,
  Precision::Confusion(),Precision::Confusion());
  //-------------------------------------------------------------------------
  if (Intersect.IsDone() && !Intersect.IsEmpty()) {
  for (Standard_Integer i = 1; i <= Intersect.NbPoints(); i++) {
  if (Intersect.Point(i).ParamOnSecond()*sign1< Precision::PConfusion()) {
  P       = Intersect.Point(i).Value();
  DistPP1 = P.SquareDistance(P1);
  if (DistPP1  < DMin) {
  DMin  = DistPP1;
  PSol  = P;
  U2   = Intersect.Point(i).ParamOnFirst();
  YaSol = Standard_True;
  }
  }
  }
  }
  }
  */

  if (YaSol) {
    Dist = DMin;
    //--------------------------------------------------------------
    // Point found => Test curve distance + Angular Test 
    //---------------------------------------------------------------
    P2 = curve2->Value(U2);
    gp_Vec2d PP1(P1.X() - PSol.X(),P1.Y() - PSol.Y());
    gp_Vec2d PP2(P2.X() - PSol.X(),P2.Y() - PSol.Y());

    //-----------------------------------------------
    // Dist = product of norms = distance at the square.
    //-----------------------------------------------
    if (PP1.Dot(PP2) > (1. - Precision::Angular())*Dist) {
      YaSol = Standard_False;
    }
    else {
      if ( !isConvex1 ) {
        Standard_Real K1 = Curvature(curve1,U1,Precision::Confusion());
        if (K1 != 0.) {
          if (Dist > 1/(K1*K1)) YaSol = Standard_False;
        }
      }
      if (YaSol) {
        if ( !isConvex2 ) {
          Standard_Real K2 = Curvature(curve2,U2,Precision::Confusion());
          if (K2 != 0.) {
            if (Dist > 1/(K2*K2)) YaSol = Standard_False;
          }
        }
      }
    }
  }
  if (!YaSol) {	
    Dist = Precision::Infinite();
    PSol = P1;
  }
  return PSol;
}

//=============================================================================
//function : D0
//purpose : 
//=============================================================================
void Bisector_BisecCC::D0(const Standard_Real     U,
  gp_Pnt2d&         P) const
{
  Standard_Real U1,U2,Dist;

  P = ValueAndDist(U,U1,U2,Dist);
}

//=============================================================================
//function : D1
//purpose : 
//=============================================================================
void Bisector_BisecCC::D1(const Standard_Real     U,
  gp_Pnt2d&         P,
  gp_Vec2d&         V ) const
{
  V.SetCoord(0.,0.);
  gp_Vec2d V2,V3;
  Values(U,1,P,V,V2,V3);
} 

//=============================================================================
//function : D2
//purpose : 
//=============================================================================
void Bisector_BisecCC::D2(const Standard_Real     U,
  gp_Pnt2d&         P,
  gp_Vec2d&         V1,
  gp_Vec2d&         V2) const
{
  V1.SetCoord(0.,0.);
  V2.SetCoord(0.,0.);
  gp_Vec2d V3;
  Values(U,2,P,V1,V2,V3);
}

//=============================================================================
//function : D3
//purpose : 
//=============================================================================
void Bisector_BisecCC::D3(const Standard_Real     U,
  gp_Pnt2d&         P,
  gp_Vec2d&         V1,
  gp_Vec2d&         V2,
  gp_Vec2d&         V3) const
{
  V1.SetCoord(0.,0.);
  V2.SetCoord(0.,0.);
  V3.SetCoord(0.,0.);
  Values(U,3,P,V1,V2,V3);
}

//=============================================================================
//function : DN
//purpose : 
//=============================================================================
gp_Vec2d Bisector_BisecCC::DN(const Standard_Real    U,
  const Standard_Integer N) const
{
  gp_Pnt2d P;
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
//function : Values
// purpose : the curve can be described by the following equations:
//
//           B(u) = F(u,v0)
//           where v0 = Phi(u) is given by H (u,v) = 0.
//   
//           with :
//                                 ||P2(v0)P1(u)||**2
//           F(u,v) = P1(u) - 1/2 *----------------* N(u)
//                                (N(u).P2(v0)P1(u))
//
//           H(u,v) = (Tu.P1(u)P2(v))**2||Tv||**2 - (Tv.P1(u)P2(v))**2||Tu||**2
//
//           => dB(u)/du = dF/du + dF/dv(- dH/du:dH/dv)
//
//           Note : tangent to the bisectrice is bissectrice at 
//                      tangents  T1(u) and T2(v0)
//
//=============================================================================
void  Bisector_BisecCC::Values (const Standard_Real     U,
  const Standard_Integer  N,
  gp_Pnt2d&         P,
  gp_Vec2d&         V1,
  gp_Vec2d&         V2,
  gp_Vec2d&         V3) const
{
  V1 = gp_Vec2d(0.,0.);
  V2 = gp_Vec2d(0.,0.);
  V3 = gp_Vec2d(0.,0.);
  //-------------------------------------------------------------------------
  // Calculate the current point on the bisectrice and the parameters on each  
  // curve.
  //-------------------------------------------------------------------------
  Standard_Real U0,V0,Dist;  

  //-----------------------------------------------
  // is the polygon reduced to a point or empty?
  //-----------------------------------------------
  if (myPolygon.Length() <= 1) {
    P = Extension(U,U0,V0,Dist,V1);
  }
  if (U  <  myPolygon.First().ParamOnBis()) {
    P = Extension(U,U0,V0,Dist,V1);
    return;
  }
  if (U >  myPolygon.Last().ParamOnBis()) {
    P = Extension(U,U0,V0,Dist,V1);
    return;
  }
  P = ValueAndDist(U,U0,V0,Dist);

  if (N == 0) return;
  //------------------------------------------------------------------
  // Return point, tangent, normal to C1 by parameter U0.
  //-------------------------------------------------------------------
  gp_Pnt2d P1  ;      // point on C1. 
  gp_Vec2d Tu  ;      // tangent to C1 by U0.
  gp_Vec2d Tuu ;      // second derivative to C1 by U0.
  curve1->D2(U0,P1,Tu,Tuu);  
  gp_Vec2d Nor( - Tu .Y() , Tu .X());           // Normal by U0.
  gp_Vec2d Nu ( - Tuu.Y() , Tuu.X());           // derivative of the normal by U0.

  //-------------------------------------------------------------------
  // Return point, tangent, normale to C2 by parameter V0.
  //-------------------------------------------------------------------
  gp_Pnt2d P2  ;      // point on C2. 
  gp_Vec2d Tv  ;      // tangent to C2 by V.
  gp_Vec2d Tvv ;      // second derivative to C2 by V.
  curve2->D2(V0,P2,Tv,Tvv);  

  gp_Vec2d PuPv(P2.X() - P1.X(), P2.Y() - P1.Y());

  //-----------------------------
  // Calculate dH/du and dH/dv.
  //-----------------------------
  Standard_Real TuTu,TvTv,TuTv;
  Standard_Real TuPuPv,TvPuPv  ;
  Standard_Real TuuPuPv,TuTuu  ;
  Standard_Real TvvPuPv,TvTvv  ;

  TuTu    = Tu.Dot(Tu)    ; TvTv   = Tv.Dot(Tv)  ; TuTv = Tu.Dot(Tv);
  TuPuPv  = Tu.Dot(PuPv)  ; TvPuPv = Tv.Dot(PuPv);
  TuuPuPv = Tuu.Dot(PuPv) ; TuTuu  = Tu.Dot(Tuu) ;
  TvvPuPv = Tvv.Dot(PuPv) ; TvTvv  = Tv.Dot(Tvv) ;

  Standard_Real dHdu = 2*(TuPuPv*(TuuPuPv - TuTu)*TvTv +
    TvPuPv*TuTv*TuTu  -TuTuu*TvPuPv*TvPuPv);
  Standard_Real dHdv = 2*(TuPuPv*TuTv*TvTv + TvTvv*TuPuPv*TuPuPv -
    TvPuPv*(TvvPuPv + TvTv)*TuTu);

  //-----------------------------
  // Calculate dF/du and dF/dv.
  //-----------------------------
  Standard_Real NorPuPv,NuPuPv,NorTv;
  Standard_Real A,B,dAdu,dAdv,dBdu,dBdv,BB;

  NorPuPv = Nor.Dot(PuPv); 
  NuPuPv  = Nu .Dot(PuPv);  
  NorTv   = Nor.Dot(Tv)  ;

  A    = 0.5*PuPv.SquareMagnitude();
  B    = - NorPuPv;
  BB   = B*B;
  dAdu = - TuPuPv;
  dBdu = - NuPuPv ;
  dAdv = TvPuPv;
  dBdv = - NorTv;

  //---------------------------------------
  // F(u,v) = Pu - (A(u,v)/B(u,v))*Nor(u)
  //----------------------------------------
  if (BB < gp::Resolution()) {
    V1 = Tu.Normalized() + Tv.Normalized();
    V1 = 0.5*Tu.SquareMagnitude()*V1;
  }
  else {
    gp_Vec2d dFdu = Tu - (dAdu/B - dBdu*A/BB)*Nor - (A/B)*Nu; 
    gp_Vec2d dFdv = ( - dAdv/B + dBdv*A/BB)*Nor ;

    if (Abs(dHdv) > gp::Resolution()) {
      V1 = dFdu + dFdv*( - dHdu / dHdv );
    }
    else {
      V1 = Tu;
    }
  }
  if (N == 1) return;
}

//=============================================================================
//function : Extension
// purpose : Calculate the current point on the  extensions 
//           by tangence of the curve.
//============================================================================
gp_Pnt2d Bisector_BisecCC::Extension (const Standard_Real  U,
  Standard_Real& U1,
  Standard_Real& U2,
  Standard_Real& Dist,
  gp_Vec2d&      T   ) const
{
  Bisector_PointOnBis PRef;
  gp_Pnt2d            P,P1,P2,PBis;
  gp_Vec2d            T1,Tang;
  Standard_Real       dU = 0.;
  Standard_Boolean    ExtensionTangent = Standard_False;

  if (myPolygon.Length() == 0) {
    //---------------------------------------------
    // Empty Polygon => segment (pointStart,pointEnd)
    //---------------------------------------------
    dU = U - startIntervals.First();
    P  = pointStart;
    P1 = pointEnd;
    U1 = curve1->LastParameter();
    if (sign1 == sign2) { U2 = curve2->FirstParameter();}
    else                { U2 = curve2->LastParameter() ;}
    Tang.SetCoord(P1.X() - P.X(),P1.Y() - P.Y());
  }
  else if (U  < myPolygon.First().ParamOnBis()) {
    PRef = myPolygon.First();    
    P    = PRef.Point();    
    dU   = U - PRef.ParamOnBis();
    if (extensionStart) {
      //------------------------------------------------------------
      // extension = segment (pointstart, first point of the polygon.)
      //------------------------------------------------------------
      P1 = pointStart;
      U1 = curve1->FirstParameter();
      if (sign1 == sign2) { U2 = curve2->LastParameter();}
      else                { U2 = curve2->FirstParameter();} 
      Tang.SetCoord(P.X() - P1.X(),P.Y() - P1.Y());
    }
    else {
      ExtensionTangent = Standard_True;
    }
  }
  else if (U >  myPolygon.Last().ParamOnBis()) {
    PRef = myPolygon.Last();    
    P    = PRef.Point();    
    dU   = U - PRef.ParamOnBis();
    if (extensionEnd) {      
      //------------------------------------------------------------
      // extension = segment (last point of the polygon.pointEnd)
      //------------------------------------------------------------
      P1 = pointEnd;
      U1 = curve1->LastParameter();
      if (sign1 == sign2) { U2 = curve2->LastParameter();}
      else                { U2 = curve2->FirstParameter();} 
      Tang.SetCoord(P1.X() - P.X(),P1.Y() - P.Y());
    }
    else {
      ExtensionTangent = Standard_True;     
    }
  }

  if (ExtensionTangent) {
    //-----------------------------------------------------------
    // If the la curve has no a extension, it is extended by tangency      
    //------------------------------------------------------------
    U1 = PRef.ParamOnC1();
    U2 = PRef.ParamOnC2(); 
    P2 = curve2->Value(U2);
    curve1->D1(U1,P1,T1);
    Tang.SetCoord(2*P.X() - P1.X() - P2.X(), 2*P.Y() - P1.Y() - P2.Y());
    if (Tang.Magnitude() < Precision::Confusion()) {
      Tang = T1;
    }
    if (T1.Dot(Tang) < 0.) Tang = - Tang;
  } 

  T = Tang.Normalized();
  PBis.SetCoord(P.X() + dU*T.X(),P.Y() + dU*T.Y());        
  Dist = P1.Distance(PBis);
  return PBis;
}

//=============================================================================
//function : PointByInt
// purpose : 
//=============================================================================
static Standard_Boolean PointByInt(const Handle(Geom2d_Curve)& CA,
  const Handle(Geom2d_Curve)& CB,
  const Standard_Real         SignA,
  const Standard_Real         SignB,
  const Standard_Real         UOnA,
  Standard_Real&        UOnB,
  Standard_Real&        Dist)
{  
  //------------------------------------------------------------------
  // Return point,tangent, normal on CA with parameter UOnA.
  //-------------------------------------------------------------------
  gp_Pnt2d P1,P2,P,PSol;
  gp_Vec2d Tan1,Tan2;
  Standard_Boolean IsConvexA = Bisector::IsConvex(CA,SignA);
  Standard_Boolean IsConvexB = Bisector::IsConvex(CB,SignB);

  CA->D1(UOnA,P1,Tan1);
  gp_Vec2d N1(Tan1.Y(), - Tan1.X());

  //--------------------------------------------------------------------------
  // test of confusion of P1 with extremity of curve2.
  //--------------------------------------------------------------------------
  if (P1.Distance(CB->Value(CB->FirstParameter())) < Precision::Confusion()) {
    UOnB = CB->FirstParameter();
    CB->D1(UOnB,P2,Tan2);	  
    if ( IsConvexA && IsConvexB ) {
      Dist = 0.;
      return Standard_True;
    }
    if (! Tan1.IsParallel(Tan2,Precision::Angular())) {
      Dist = 0.;
      return Standard_False;
    }
  }
  if (P1.Distance(CB->Value(CB->LastParameter())) < Precision::Confusion()) {
    UOnB   = CB->LastParameter();  
    CB->D1(UOnB,P2,Tan2);    
    if ( IsConvexA && IsConvexB ) {
      Dist = 0.;
      return Standard_True;
    }
    if (! Tan1.IsParallel(Tan2,Precision::Angular())) {  
      Dist = 0.;
      return Standard_False;
    }
  }

  Standard_Real    DMin = Precision::Infinite();
  Standard_Real    UPC;
  Standard_Boolean YaSol = Standard_False; 
  //--------------------------------------------------------------------
  // Construction of the bisectrice point curve and of the straight line passing
  // through P1 and carried by the normal. 
  //--------------------------------------------------------------------
  Handle(Bisector_BisecPC) BisPC 
    = new Bisector_BisecPC(CB,P1,SignB );
  //-------------------------------
  // Test if the bissectrice exists.
  //-------------------------------  
  if (BisPC->IsEmpty()) {
    Dist = Precision::Infinite();
    PSol = P1;
    return Standard_False;
  }

  Handle(Geom2d_Line)      NorLi = new Geom2d_Line (P1,N1);

  Geom2dAdaptor_Curve ABisPC(BisPC);
  Geom2dAdaptor_Curve ANorLi(NorLi);    
  //-------------------------------------------------------------------------
  Geom2dInt_GInter  Intersect(ABisPC,ANorLi,
    Precision::Confusion(),Precision::Confusion());
  //-------------------------------------------------------------------------

  if (Intersect.IsDone() && !Intersect.IsEmpty()) {
    for (Standard_Integer i = 1; i <= Intersect.NbPoints(); i++) {
      if (Intersect.Point(i).ParamOnSecond()*SignA < Precision::PConfusion()) {
        P  = Intersect.Point(i).Value();
        if (P.SquareDistance(P1) < DMin) {
          DMin  = P.SquareDistance(P1);
          PSol  = P;
          UPC   = Intersect.Point(i).ParamOnFirst();
          UOnB  = BisPC->LinkBisCurve(UPC);
          Dist  = DMin;
          YaSol = Standard_True;
        }
      }
    }
  }  
  if (YaSol) {    
    //--------------------------------------------------------------
    // Point found => Test distance curvature + Angular test 
    //---------------------------------------------------------------
    P2 = CB->Value(UOnB);
    if(P1.SquareDistance(PSol) < 1.e-32)
    {
      YaSol = Standard_False;
      return YaSol;
    }
    if(P2.SquareDistance(PSol) < 1.e-32)
    {
      YaSol = Standard_False;
      return YaSol;
    }

    gp_Dir2d PP1Unit(P1.X() - PSol.X(),P1.Y() - PSol.Y());
    gp_Dir2d PP2Unit(P2.X() - PSol.X(),P2.Y() - PSol.Y());

    if (PP1Unit*PP2Unit > 1. - Precision::Angular()) {
      YaSol = Standard_False;
    }
    else {
      Dist = sqrt(Dist);
      if ( !IsConvexA ) {
        Standard_Real K1 = Curvature(CA,UOnA,Precision::Confusion());
        if (K1 != 0.) {
          if (Dist > Abs(1/K1)) YaSol = Standard_False;
        }
      }
      if (YaSol) {
        if ( !IsConvexB ) {
          Standard_Real K2 = Curvature(CB,UOnB,Precision::Confusion());
          if (K2 != 0.) {
            if (Dist > Abs(1/K2)) YaSol = Standard_False;
          }
        }
      }
    }
  }
  return YaSol;
}

//=============================================================================
//function : SupLastParameter
// purpose : 
//=============================================================================
void Bisector_BisecCC::SupLastParameter()
{
  endIntervals.Append(curve1->LastParameter());
  // ----------------------------------------------------------------------
  // Calculate parameter on curve1 associated to one or the other of the extremities
  // of curve2 following the values of sign1 and sign2.
  // the bissectrice is limited by the obtained parameters.
  //------------------------------------------------------------------------
  Standard_Real    UOnC1,UOnC2,Dist;
  if (sign1 == sign2) {
    UOnC2 =  curve2->FirstParameter();
  }
  else {
    UOnC2 = curve2->LastParameter();
  }
  Standard_Boolean YaSol  = PointByInt(curve2,curve1,sign2,sign1,UOnC2,UOnC1,Dist);
  if (YaSol) {
    if (UOnC1 > startIntervals.First() && UOnC1 < endIntervals.Last()) {
      endIntervals.SetValue(1,UOnC1);
    }
  }
}

//=============================================================================
//function : Curve
// purpose : 
//=============================================================================
Handle(Geom2d_Curve) Bisector_BisecCC::Curve(const Standard_Integer I) const 
{
  if      (I == 1) return curve1;
  else if (I == 2) return curve2;
  else             throw Standard_OutOfRange();
}

//=============================================================================
//function : LinkBisCurve
//purpose : 
//=============================================================================
Standard_Real Bisector_BisecCC::LinkBisCurve(const Standard_Real U) const 
{
  return (U - shiftParameter);
} 

//=============================================================================
//function : LinkCurveBis
//purpose : 
//=============================================================================
Standard_Real Bisector_BisecCC::LinkCurveBis(const Standard_Real U) const 
{
  return (U + shiftParameter);
}

//=============================================================================
//function : Indent
//purpose  : 
//=============================================================================
static void Indent(const Standard_Integer Offset) {
  if (Offset > 0) {
    for (Standard_Integer i = 0; i < Offset; i++) {cout << " ";}
  }
}

//=============================================================================
//function : Polygon
// purpose : 
//=============================================================================
const Bisector_PolyBis&  Bisector_BisecCC::Polygon() const
{
  return myPolygon;
}

//==========================================================================
//function : Parameter
//purpose  :
//==========================================================================
Standard_Real Bisector_BisecCC::Parameter(const gp_Pnt2d& P) const
{
  Standard_Real UOnCurve;

  if (P.IsEqual(Value(FirstParameter()),Precision::Confusion())) {
    UOnCurve = FirstParameter();
  }
  else if (P.IsEqual(Value(LastParameter()),Precision::Confusion())) {
    UOnCurve = LastParameter();
  }
  else
  {
    ProjOnCurve(P, curve1, UOnCurve);
  }

  return UOnCurve;
}


//=============================================================================
//function : Dump
// purpose : 
//=============================================================================
//void Bisector_BisecCC::Dump(const Standard_Integer Deep, 
void Bisector_BisecCC::Dump(const Standard_Integer , 
  const Standard_Integer Offset) const 
{
  Indent (Offset);
  cout <<"Bisector_BisecCC :"<<endl;
  Indent (Offset);
  //  cout <<"Curve1 :"<<curve1<<endl;
  //  cout <<"Curve2 :"<<curve2<<endl;
  cout <<"Sign1  :"<<sign1<<endl;
  cout <<"Sign2  :"<<sign2<<endl;

  cout <<"Number Of Intervals :"<<startIntervals.Length()<<endl;
  for (Standard_Integer i = 1; i <= startIntervals.Length(); i++) {
    cout <<"Interval number :"<<i<<"Start :"<<startIntervals.Value(i)
      <<"  end :"<<  endIntervals.Value(i)<<endl ;
  }
  cout <<"Index Current Interval :"<<currentInterval<<endl;
}

//=============================================================================
//function : Curve
// purpose : 
//=============================================================================
void Bisector_BisecCC::Curve(const Standard_Integer      I,
  const Handle(Geom2d_Curve)& C)  
{
  if      (I == 1) curve1 = C;
  else if (I == 2) curve2 = C;
  else             throw Standard_OutOfRange();
}

//=============================================================================
//function : Sign
// purpose : 
//=============================================================================
void Bisector_BisecCC::Sign(const Standard_Integer      I,
  const Standard_Real         S)
{
  if      (I == 1) sign1 = S;
  else if (I == 2) sign2 = S;
  else             throw Standard_OutOfRange();
}

//=============================================================================
//function : Polygon
// purpose : 
//=============================================================================
void Bisector_BisecCC::Polygon(const Bisector_PolyBis& P)  
{
  myPolygon = P;
}

//=============================================================================
//function : DistMax
// purpose : 
//=============================================================================
void Bisector_BisecCC::DistMax(const Standard_Real D)  
{
  distMax = D;
}

//=============================================================================
//function : IsConvex
// purpose : 
//=============================================================================
void Bisector_BisecCC::IsConvex(const Standard_Integer     I,
  const Standard_Boolean     IsConvex)  
{
  if      (I == 1) isConvex1 = IsConvex;
  else if (I == 2) isConvex2 = IsConvex;
  else             throw Standard_OutOfRange();
}

//=============================================================================
//function : IsEmpty
// purpose : 
//=============================================================================
void Bisector_BisecCC::IsEmpty ( const Standard_Boolean     IsEmpty)  
{
  isEmpty = IsEmpty;
}

//=============================================================================
//function : ExtensionStart
// purpose : 
//=============================================================================
void Bisector_BisecCC::ExtensionStart( const Standard_Boolean  ExtensionStart)  
{
  extensionStart = ExtensionStart;
}

//=============================================================================
//function : ExtensionEnd
// purpose : 
//=============================================================================
void Bisector_BisecCC::ExtensionEnd( const Standard_Boolean  ExtensionEnd)  
{
  extensionEnd = ExtensionEnd;
}

//=============================================================================
//function : PointStart
// purpose : 
//=============================================================================
void Bisector_BisecCC::PointStart( const gp_Pnt2d& Point)  
{
  pointStart = Point;
}

//=============================================================================
//function : PointEnd
// purpose : 
//=============================================================================
void Bisector_BisecCC::PointEnd( const gp_Pnt2d& Point)  
{
  pointEnd = Point;
}

//=============================================================================
//function : StartIntervals
// purpose : 
//=============================================================================
void Bisector_BisecCC::StartIntervals 
  (const TColStd_SequenceOfReal& StartIntervals)  
{
  startIntervals = StartIntervals;
}

//=============================================================================
//function : EndIntervals
// purpose : 
//=============================================================================
void Bisector_BisecCC::EndIntervals 
  (const TColStd_SequenceOfReal& EndIntervals)  
{
  endIntervals = EndIntervals;
}

//=============================================================================
//function : FirstParameter
// purpose : 
//=============================================================================
void Bisector_BisecCC::FirstParameter (const Standard_Real U)  
{
  startIntervals.Append(U);
}

//=============================================================================
//function : LastParameter
// purpose : 
//=============================================================================
void Bisector_BisecCC::LastParameter (const Standard_Real U)  
{
  endIntervals.Append(U);
}

//=============================================================================
//function : SearchBound
// purpose : 
//=============================================================================
Standard_Real Bisector_BisecCC::SearchBound (const Standard_Real U1,
  const Standard_Real U2) const
{
  Standard_Real UMid,Dist1,Dist2,DistMid,U11,U22;
  Standard_Real UC1,UC2;
  gp_Pnt2d PBis,PBisPrec;
  Standard_Real TolPnt   = Precision::Confusion();
  Standard_Real TolPar   = Precision::PConfusion();
  U11 = U1; U22 = U2;
  PBisPrec = ValueByInt(U11,UC1,UC2,Dist1);
  PBis     = ValueByInt(U22,UC1,UC2,Dist2);

  while ((U22 - U11) > TolPar || 
    ((Dist1 < Precision::Infinite() && 
    Dist2 < Precision::Infinite() &&
    !PBis.IsEqual(PBisPrec,TolPnt)))) { 
      PBisPrec = PBis;
      UMid     = 0.5*( U22 + U11);
      PBis     = ValueByInt(UMid,UC1,UC2,DistMid);
      if ((Dist1 < Precision::Infinite()) == (DistMid < Precision::Infinite())) {
        U11    = UMid;
        Dist1 = DistMid;
      }
      else {
        U22    = UMid;
        Dist2 = DistMid;
      }
  }
  PBis = ValueByInt(U11,UC1,UC2,Dist1);
  if (Dist1 < Precision::Infinite()) {
    UMid = U11;
  }
  else {
    UMid = U22;
  }
  return UMid;
}

//=============================================================================
//function : ProjOnCurve
// purpose :
//=============================================================================
static Standard_Boolean ProjOnCurve (const gp_Pnt2d& P,
  const Handle(Geom2d_Curve)& C,
  Standard_Real& theParam)
{
  //Standard_Real UOnCurve =0.;
  theParam = 0.0;
  gp_Pnt2d      PF,PL;
  gp_Vec2d      TF,TL;

  C->D1(C->FirstParameter(),PF,TF);
  C->D1(C->LastParameter() ,PL,TL);

  if (P.IsEqual(PF ,Precision::Confusion()))
  {
    theParam =  C->FirstParameter();
    return Standard_True;
  }

  if (P.IsEqual(PL ,Precision::Confusion()))
  {
    theParam = C->LastParameter();
    return Standard_True;
  }

  gp_Vec2d PPF(PF.X() - P.X(), PF.Y() - P.Y());
  TF.Normalize();

  if ( Abs (PPF.Dot(TF)) < Precision::Confusion())
  {
    theParam = C->FirstParameter();
    return Standard_True;
  }
  gp_Vec2d PPL (PL.X() - P.X(), PL.Y() - P.Y());
  TL.Normalize();
  if ( Abs (PPL.Dot(TL)) < Precision::Confusion())
  {
    theParam = C->LastParameter();
    return Standard_True;
  }
  Geom2dAPI_ProjectPointOnCurve Proj(P,C,
    C->FirstParameter(),
    C->LastParameter());
  if (Proj.NbPoints() > 0) {
    theParam = Proj.LowerDistanceParameter();
  }
  else {
    return Standard_False;
  }

  return Standard_True;
}

//=============================================================================
//function : TestExtension
// purpose : 
//=============================================================================
static Standard_Boolean TestExtension (const Handle(Geom2d_Curve)& C1,
  const Handle(Geom2d_Curve)& C2,
  const Standard_Integer      Start_End)
{
  gp_Pnt2d         P1,P2;
  gp_Vec2d         T1,T2;
  Standard_Boolean Test = Standard_False;
  if (Start_End == 1) {
    C1->D1(C1->FirstParameter(),P1,T1);
  }
  else {
    C1->D1(C1->LastParameter(),P1,T1); 
  }
  C2->D1(C2->FirstParameter(),P2,T2);
  if (P1.IsEqual(P2,Precision::Confusion())) {
    T1.Normalize(); T2.Normalize();
    if (T1.Dot(T2) > 1.0 - Precision::Confusion()) {
      Test = Standard_True;
    }
  }
  else {
    C2->D1(C2->LastParameter(),P2,T2);
    if (P1.IsEqual(P2,Precision::Confusion())) {
      T2.Normalize();
      if (T1.Dot(T2) > 1.0 - Precision::Confusion()) {
        Test = Standard_True;
      }
    }
  }
  return Test;
}

//=============================================================================
//function : ComputePointEnd
// purpose : 
//=============================================================================
void  Bisector_BisecCC::ComputePointEnd  ()
{  
  Standard_Real U1,U2;  
  Standard_Real KC,RC;
  U1 = curve1->FirstParameter();
  if (sign1 == sign2) {
    U2 = curve2->LastParameter();
  }
  else {
    U2 = curve2->FirstParameter();
  }
  Standard_Real K1 = Curvature(curve1,U1,Precision::Confusion());
  Standard_Real K2 = Curvature(curve2,U2,Precision::Confusion());    
  if (!isConvex1 && !isConvex2) {
    if (K1 < K2) {KC = K1;} else {KC = K2;}
  }
  else if (!isConvex1) {KC = K1;}
  else                 {KC = K2;}

  gp_Pnt2d      PF; 
  gp_Vec2d      TF;
  curve1->D1(U1,PF,TF);
  TF.Normalize();
  if (KC != 0.) { RC = Abs(1/KC);}
  else          { RC = Precision::Infinite();}
  pointEnd.SetCoord(PF.X() - sign1*RC*TF.Y(), PF.Y() + sign1*RC*TF.X());

}

//=============================================================================
//function : DiscretPar
// purpose :
//=============================================================================
static Standard_Boolean DiscretPar(const Standard_Real     DU, 
  const Standard_Real     EpsMin, 
  const Standard_Real     EpsMax,
  const Standard_Integer  NbMin,
  const Standard_Integer  NbMax,
  Standard_Real&    Eps,
  Standard_Integer& Nb) 
{
  if (DU <= NbMin*EpsMin) {
    Eps = DU/(NbMin + 1) ;
    Nb  = NbMin;  
    return Standard_False;
  }

  Eps = Min (EpsMax,DU/NbMax);

  if (Eps < EpsMin) {
    Eps = EpsMin;
    Nb  = Standard_Integer(DU/EpsMin);
  }  
  else { Nb = NbMax;}

  return Standard_True;
}


