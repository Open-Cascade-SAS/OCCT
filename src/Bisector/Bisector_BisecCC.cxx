// File:	Bisector_BisecCC.cxx
// Created:	Thu Mar 10 17:54:52 1994
// Author:	Yves FRICAUD
//		<yfr@phylox>

#include <Bisector_BisecCC.ixx>
#include <Bisector_BisecPC.hxx>
#include <Bisector.hxx>
#include <Bisector_Curve.hxx>
#include <Bisector_FunctionH.hxx>
#include <Bisector_PointOnBis.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <Geom2dGcc.hxx>
#include <Geom2dGcc_Circ2d2TanRad.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>  
#include <gp.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <Precision.hxx>
#include <math_FunctionRoot.hxx>
#include <math_FunctionRoots.hxx>
#include <math_BissecNewton.hxx>

#include <Standard_OutOfRange.hxx>
#include <Standard_DivideByZero.hxx>
#include <Standard_NotImplemented.hxx>


static Standard_Real    ProjOnCurve (const gp_Pnt2d&             P,
				     const Handle(Geom2d_Curve)& C);

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
  // Calcul premier point du polygone.
  //---------------------------------------------
  U    = ProjOnCurve (Origin,curve1);
  P    = ValueByInt  (U,UC1,UC2,Dist);

  if (Dist < Precision::Infinite()) {
    //----------------------------------------------------
    // le parametre du point origine donne un point sur le
    // polygone.
    //----------------------------------------------------
    myPolygon.Append(Bisector_PointOnBis(UC1,UC2,U,Dist,P));     
    startIntervals.Append(U);
    if (P.IsEqual(Origin,Precision::Confusion())) {    
      //----------------------------------------
      // test si le prenier point est l origine.
      //----------------------------------------
      OriInPoly = Standard_True;
    }
  }
  else {
    //-------------------------------------------------------
    // Le point origine est sur un prolongement.
    // Recherche du premier point du polygone par dichotomie.
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

  if ( !myPolygon.Length() == 0) {
    SupLastParameter();
    //----------------------------------------------
    // Construction du polygone de la bissectrice.
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
    // Polygone vide.    
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
    // Prolongement : La courbe est prolongee au debut ou/et a la fin si
    //                - une des deux courbes est concave.
    //                - Les courbes ont un point commun au debut ou/et a la fin
    //                - l angle d ouverture au point commun entre les deux courbes
    //                  vaut PI.
    // le prolongemt au debut est pris en compte si l origine se trouve dessus.
    // ie : l origine n est pas dans le polygone.
    //-----------------------------------------------------------------------------
    
    //---------------------------------
    // Existent ils des prolongemnets ?
    //---------------------------------
    if (OriInPoly) {
      extensionStart = Standard_False;
    }
    else {
      extensionStart = TestExtension(curve1,curve2,1);
    }
    extensionEnd     = TestExtension(curve1,curve2,2);

    //-----------------
    // Calcul pointEnd.
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
    // Mise a jour des Bornes des intervalles de definition.
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
      // Pas de polygone => la bissectrise est un segment.
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
  Standard_NotImplemented::Raise();  
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
//purpose  : Changement de la ligne guide pour le parametrage de la bissectrice
//           ATTENTION : - Ceci peut inverser le sens de parametrage.
//                       - Ceci ne concerne que la partie de la courbe
//                         correspondante au polygone.
//=============================================================================
Handle(Bisector_BisecCC) Bisector_BisecCC::ChangeGuide() const
{
  Handle(Bisector_BisecCC) C = new Bisector_BisecCC();
  
  C -> Curve    (1, curve2)   ; C -> Curve    (2, curve1);
  C -> Sign     (1, sign2 )   ; C -> Sign     (2, sign1 );
  C -> IsConvex (1, isConvex2); C -> IsConvex (2, isConvex1); 
  
  //-------------------------------------------------------------------------
  // Construction du nouveau polygone a partir de celui d origine.
  // inversion des PointOnBis et Calcul nouveau parametre sur la bissectrice.
  //-------------------------------------------------------------------------
  Bisector_PolyBis Poly;
  if (sign1 == sign2 ) {
    //---------------------------------------------------------------
    // les elements du nouveau polygone sont ranges dans l autre sens.
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
#ifndef DEB
  default: break;
#endif
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
#ifndef DEB
  default: break;
#endif
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
//purpose  : CALCUL DU POINT COURANT PAR METHODE ITERATIVE.
//           ----------------------------------------------
//           Calcul du point courant, de la distance du point courant aux deux
//           courbes, des parametres sur chaque courbe de la projection du 
//           point courrant.
//
//method  : - Recheche parametre de depart en utilisant <myPolygon>.
//          - Calcul du parametre U2 sur la courbe C2 solution de H(U,V)= 0
//          - P(U) = F(U,U2)
//
//          ou :
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
  // le polygone est il  reduit a un point ou vide?
  //-----------------------------------------------
  if (myPolygon.Length() <= 1) {
    return Extension(U,U1,U2,Dist,T);
  }

  //-----------------------------------------------
  // test U en dehors des bornes du polygone.
  //-----------------------------------------------
  if (U  <  myPolygon.First().ParamOnBis()) {
    return Extension(U,U1,U2,Dist,T);
  }
  if (U >  myPolygon.Last().ParamOnBis()) {
    return Extension(U,U1,U2,Dist,T);
  }

  //-------------------------------------------------------
  // Recheche parametre de depart en utilisant <myPolygon>.
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
  // Calcul du parametre U2 sur la courbe C2 solution de H(u,v)= 0     
  //---------------------------------------------------------------
  gp_Pnt2d P1;
  gp_Vec2d T1;
  Standard_Real    EpsH     = 1.E-8;
  Standard_Real    EpsH100  = 1.E-6; 
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
    else {
      math_BissecNewton  SolNew (H,VMin - EpsH100,VMax + EpsH100,EpsH,10);
      if (SolNew.IsDone()) {
	U2    = SolNew.Root();
      }
      else {
	math_FunctionRoot SolRoot (H,VInit,EpsH,VMin - EpsH100,VMax + EpsH100);
	if (SolRoot.IsDone()) {
	  U2    = SolRoot.Root();
	}
	else { Valid = Standard_False;}
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

    if (P1.IsEqual(P2,Precision::Confusion())) {
      PBis = P1 ;
      Dist = 0.0;
    }
    else if (N1P2P1*sign1 < 0) {
      Valid = Standard_False;
    }
    else {	
       PBis = P1.Translated(- (0.5*SquareP2P1/N1P2P1)*N1);
       Dist = P1.SquareDistance(PBis);
    }
  }

  //----------------------------------------------------------------
  // Si le point n est pas valide 
  // calcul par intersection.
  //----------------------------------------------------------------
  if (!Valid) {
    //--------------------------------------------------------------------
    // Construction de la bisectrice point courbe et de la droite passant
    // par P1 et portee par la normale. la curve2 est restreinte par VMin et
    // VMax.
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
//purpose  : CALCUL DU POINT COURANT PAR INTERSECTION.
//           -----------------------------------------
//           Calcul du point courant, de la distance du point courant aux deux
//           courbes, des parametres sur chaque courbe de la projection du 
//           point courrant.
//           le point courrant au parametre U est l intersection de la 
//           bissectrice point courbe (P1,curve2) et de la droite passant par
//           P1 de vecteur directeur N1.
//           P1 est le point courrant de parametre U sur la curve1 et N1 la 
//           normale en ce point.
//=============================================================================
gp_Pnt2d Bisector_BisecCC::ValueByInt (const Standard_Real  U,
				             Standard_Real& U1,
				             Standard_Real& U2,
				             Standard_Real& Dist) const
{
  //------------------------------------------------------------------
  // Recuperation des point,tangente,normale sur C1 au parametre U.
  //-------------------------------------------------------------------
  U1 = LinkBisCurve(U);

  gp_Pnt2d P1,P2,P,PSol;
  gp_Vec2d Tan1,Tan2;
  curve1->D1(U1,P1,Tan1);
  gp_Vec2d N1(  Tan1.Y(), - Tan1.X());
 
  //--------------------------------------------------------------------------
  // test de confusion de P1 avec extremite de curve2.
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
      gp_Pnt2d P2 = curve2->Value(USol);
      gp_Vec2d P2P1(P1.X() - P2.X(),P1.Y() - P2.Y());
      Standard_Real SquareP2P1 = P2P1.SquareMagnitude();
      Standard_Real N1P2P1     = N1.Dot(P2P1);

      // Test si la solution est du bon cote des courbes.
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
    // Point trouve => Test distance courbure + Test angulaire
    //---------------------------------------------------------------
    P2 = curve2->Value(U2);
    gp_Vec2d PP1(P1.X() - PSol.X(),P1.Y() - PSol.Y());
    gp_Vec2d PP2(P2.X() - PSol.X(),P2.Y() - PSol.Y());
 
    //-----------------------------------------------
    // Dist = produit des normes = distance au carre.
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
      Standard_NotImplemented::Raise();
    }
  }
  return V1;
}

//=============================================================================
//function : Values
// purpose : la courbe peut etre decrite par les equations suivantes:
//
//           B(u) = F(u,v0)
//           ou v0 = Phi(u) est donne par H (u,v) = 0.
//   
//           avec :
//                                 ||P2(v0)P1(u)||**2
//           F(u,v) = P1(u) - 1/2 *----------------* N(u)
//                                (N(u).P2(v0)P1(u))
//
//           H(u,v) = (Tu.P1(u)P2(v))**2||Tv||**2 - (Tv.P1(u)P2(v))**2||Tu||**2
//
//           => dB(u)/du = dF/du + dF/dv(- dH/du:dH/dv)
//
//           Remarque : la tangente a la bisectrice est bissectrice aux 
//                      tangentes  T1(u) et T2(v0)
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
  // Calcul du point courant sur la bisectrice et des parametres sur chaque 
  // courbe.
  //-------------------------------------------------------------------------
  Standard_Real U0,V0,Dist;  
  
  //-----------------------------------------------
  // le polygone est il  reduit a un point ou vide?
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
  // Recuperation des point,tangente,normale sur C1 au parametre U0.
  //-------------------------------------------------------------------
  gp_Pnt2d P1  ;      // point sur C1. 
  gp_Vec2d Tu  ;      // tangente a C1 en U0.
  gp_Vec2d Tuu ;      // derivee seconde a C1 en U0.
  curve1->D2(U0,P1,Tu,Tuu);  
  gp_Vec2d Nor( - Tu .Y() , Tu .X());           // Normale  en U0.
  gp_Vec2d Nu ( - Tuu.Y() , Tuu.X());           // derivee de la normale en U0.

  //-------------------------------------------------------------------
  // Recuperation des point,tangente,normale sur C2 au parametre V0.
  //-------------------------------------------------------------------
  gp_Pnt2d P2  ;      // point sur C2. 
  gp_Vec2d Tv  ;      // tangente a C2 en V.
  gp_Vec2d Tvv ;      // derivee seconde a C2 en V.
  curve2->D2(V0,P2,Tv,Tvv);  

  gp_Vec2d PuPv(P2.X() - P1.X(), P2.Y() - P1.Y());

  //-----------------------------
  // Calcul de dH/du et de dH/dv.
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
  // Calcul de dF/du et de dF/dv.
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
// purpose : Calcul du point courant sur les extensions ou prolongement en
//           tangence de la courbe.
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
#ifndef DEB
  Standard_Real       dU = 0.;
#else
  Standard_Real       dU;
#endif
  Standard_Boolean    ExtensionTangent = Standard_False;

  if (myPolygon.Length() == 0) {
    //---------------------------------------------
    // Polygone vide => segment (pointStart,pointEnd)
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
      // extension = segment (pointstart,premier point du polygone.)
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
      // extension = segment (dernier point du polygone.pointEnd)
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
    // Si la courbe n a pas d extension, celle ci est prolonge 
    // en tangence.      
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
  // Recuperation des point,tangente,normale sur CA au parametre UOnA.
  //-------------------------------------------------------------------
  gp_Pnt2d P1,P2,P,PSol;
  gp_Vec2d Tan1,Tan2;
  Standard_Boolean IsConvexA = Bisector::IsConvex(CA,SignA);
  Standard_Boolean IsConvexB = Bisector::IsConvex(CB,SignB);

  CA->D1(UOnA,P1,Tan1);
  gp_Vec2d N1(Tan1.Y(), - Tan1.X());
 
  //--------------------------------------------------------------------------
  // test de confusion de P1 avec extremite de curve2.
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
  // Construction de la bisectrice point courbe et de la droite passant
  // par P1 et portee par la normale.
  //--------------------------------------------------------------------
  Handle(Bisector_BisecPC) BisPC 
    = new Bisector_BisecPC(CB,P1,SignB );
  //-------------------------------
  // Test si la bissectrice existe.
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
    // Point trouve => Test distance courbure + Test angulaire
    //---------------------------------------------------------------
    P2 = CB->Value(UOnB);
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
  // Calcul du parametre  sur curve1 associees a l une ou lautre des extremites
  // de curve2 suivant les valeurs de sign1 et sign2.
  // la bissectrice est restreinte par les parametres obtenus.
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
  else             Standard_OutOfRange::Raise();  
  return curve1;
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
  else {
    UOnCurve = ProjOnCurve(P,curve1);
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
  else             Standard_OutOfRange::Raise();  
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
  else             Standard_OutOfRange::Raise();  
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
  else             Standard_OutOfRange::Raise();  
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
static Standard_Real ProjOnCurve (const gp_Pnt2d&             P,
				  const Handle(Geom2d_Curve)& C)
{ 
#ifndef DEB
  Standard_Real UOnCurve =0.;
#else
  Standard_Real UOnCurve;
#endif
  gp_Pnt2d      PF,PL;
  gp_Vec2d      TF,TL;

  C->D1(C->FirstParameter(),PF,TF);
  C->D1(C->LastParameter() ,PL,TL);

  if (P.IsEqual(PF ,Precision::Confusion())) {
    return C->FirstParameter();
  }
  if (P.IsEqual(PL ,Precision::Confusion())) {
    return C->LastParameter();
  }
  gp_Vec2d PPF(PF.X() - P.X(), PF.Y() - P.Y());
  TF.Normalize();
  if ( Abs (PPF.Dot(TF)) < Precision::Confusion()) {
    return C->FirstParameter();
  }
  gp_Vec2d PPL (PL.X() - P.X(), PL.Y() - P.Y());
  TL.Normalize();
  if ( Abs (PPL.Dot(TL)) < Precision::Confusion()) {
    return C->LastParameter();
  }
  Geom2dAPI_ProjectPointOnCurve Proj(P,C,
				     C->FirstParameter(),
				     C->LastParameter());
  if (Proj.NbPoints() > 0) {
    UOnCurve = Proj.LowerDistanceParameter();
  }
  else {
    Standard_OutOfRange::Raise();
  }
  return UOnCurve;
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


