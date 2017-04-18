// Created on: 1994-09-05
// Created by: Bruno DUMORTIER
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

// 09-Aug-95 : xab : changed the ProjLib_ProjectOnPlane in the case
//                   of the line and the parameteriation is kept
#include <ProjLib_ProjectOnPlane.hxx>
#include <AppCont_Function.hxx>
#include <Approx_FitAndDivide.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Precision.hxx>
#include <BSplCLib.hxx>
#include <PLib.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Precision.hxx>
#include <ElCLib.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <Geom_Line.hxx>
#include <GeomConvert.hxx>
#include <BSplCLib.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Ellipse.hxx>



//=======================================================================
//function : OnPlane_Value
//purpose  : Evaluate current point of the projected curve
//=======================================================================

static gp_Pnt OnPlane_Value(const Standard_Real U,
			    const Handle(Adaptor3d_HCurve)& aCurvePtr,
			    const gp_Ax3& Pl,
			    const gp_Dir& D)
{
  //                   PO . Z                /  Z = Pl.Direction()
  // Proj(u) = P(u) + -------  * D     avec  \  O = Pl.Location()
  //                   D  . Z

  gp_Pnt Point = aCurvePtr->Value(U);
  
  gp_Vec PO(Point,Pl.Location());
  
  Standard_Real Alpha = PO * gp_Vec(Pl.Direction());
  Alpha /= D * Pl.Direction();
  Point.SetXYZ(Point.XYZ() + Alpha * D.XYZ());

  return Point;
}

//=======================================================================
//function : OnPlane_DN
//purpose  : Evaluate current point of the projected curve
//=======================================================================

static gp_Vec OnPlane_DN(const Standard_Real U,
			 const Standard_Integer DerivativeRequest,
			 const Handle(Adaptor3d_HCurve)& aCurvePtr,
			 const gp_Ax3& Pl,
			 const gp_Dir& D)
{
  //                   PO . Z                /  Z = Pl.Direction()
  // Proj(u) = P(u) + -------  * D     avec  \  O = Pl.Location()
  //                   D  . Z

  gp_Vec Vector = aCurvePtr->DN(U,DerivativeRequest);

  gp_Dir Z = Pl.Direction();
  
  Standard_Real
  Alpha  = Vector * gp_Vec(Z);
  Alpha /=  D * Z;

  Vector.SetXYZ( Vector.XYZ() - Alpha * D.XYZ());
  return Vector ;
}

//=======================================================================
//function : OnPlane_D1
//purpose  : 
//=======================================================================

static Standard_Boolean OnPlane_D1(const Standard_Real U,
				         gp_Pnt& P,
				         gp_Vec& V,
				   const Handle(Adaptor3d_HCurve)& aCurvePtr,
				   const gp_Ax3& Pl,
				   const gp_Dir& D)
{
  Standard_Real Alpha;
  gp_Pnt Point;
  gp_Vec Vector;
  
  gp_Dir Z = Pl.Direction();

  aCurvePtr->D1(U, Point, Vector);

  // evaluate the point as in `OnPlane_Value`
  gp_Vec PO(Point,Pl.Location());
  Alpha  = PO * gp_Vec(Z);
  Alpha /= D * Z;
  P.SetXYZ(Point.XYZ() + Alpha * D.XYZ());

  
  // evaluate the derivative.
  // 
  //   d(Proj)  d(P)       1        d(P)
  //   ------ = ---  - -------- * ( --- . Z ) * D
  //     dU     dU     ( D . Z)     dU  
  //

  Alpha  = Vector * gp_Vec(Z);
  Alpha /=  D * Z;

  V.SetXYZ( Vector.XYZ() - Alpha * D.XYZ());

  return Standard_True;
}
//=======================================================================
//function : OnPlane_D2
//purpose  : 
//=======================================================================

static Standard_Boolean OnPlane_D2(const Standard_Real U,
				         gp_Pnt& P,
				         gp_Vec& V1,
				         gp_Vec& V2,
				   const Handle(Adaptor3d_HCurve) & aCurvePtr,
				   const gp_Ax3& Pl,
				   const gp_Dir& D)
{
  Standard_Real Alpha;
  gp_Pnt Point;
  gp_Vec Vector1,
  Vector2;
  
  gp_Dir Z = Pl.Direction();

  aCurvePtr->D2(U, Point, Vector1, Vector2);

  // evaluate the point as in `OnPlane_Value`
  gp_Vec PO(Point,Pl.Location());
  Alpha  = PO * gp_Vec(Z);
  Alpha /= D * Z;
  P.SetXYZ(Point.XYZ() + Alpha * D.XYZ());

  // evaluate the derivative.
  // 
  //   d(Proj)  d(P)       1        d(P)
  //   ------ = ---  - -------- * ( --- . Z ) * D
  //     dU     dU     ( D . Z)     dU  
  //

  Alpha  = Vector1 * gp_Vec(Z);
  Alpha /=  D * Z;

  V1.SetXYZ( Vector1.XYZ() - Alpha * D.XYZ());

  Alpha  = Vector2 * gp_Vec(Z);
  Alpha /=  D * Z;

  V2.SetXYZ( Vector2.XYZ() - Alpha * D.XYZ());
  return Standard_True;
}

//=======================================================================
//function : OnPlane_D3
//purpose  : 
//=======================================================================

static Standard_Boolean OnPlane_D3(const Standard_Real U,
				         gp_Pnt& P,
				         gp_Vec& V1,
				         gp_Vec& V2,
				         gp_Vec& V3,
				   const Handle(Adaptor3d_HCurve)& aCurvePtr,
				   const gp_Ax3& Pl,
				   const gp_Dir& D)
{
  Standard_Real Alpha;
  gp_Pnt Point;
  gp_Vec Vector1,
  Vector2,
  Vector3;
  
  gp_Dir Z = Pl.Direction();

  aCurvePtr->D3(U, Point, Vector1, Vector2, Vector3);

  // evaluate the point as in `OnPlane_Value`
  gp_Vec PO(Point,Pl.Location());
  Alpha  = PO * gp_Vec(Z);
  Alpha /= D * Z;
  P.SetXYZ(Point.XYZ() + Alpha * D.XYZ());
  
  // evaluate the derivative.
  // 
  //   d(Proj)  d(P)       1        d(P)
  //   ------ = ---  - -------- * ( --- . Z ) * D
  //     dU     dU     ( D . Z)     dU  
  //

  Alpha  = Vector1 * gp_Vec(Z);
  Alpha /=  D * Z;

  V1.SetXYZ( Vector1.XYZ() - Alpha * D.XYZ());

  Alpha  = Vector2 * gp_Vec(Z);
  Alpha /=  D * Z;

  V2.SetXYZ( Vector2.XYZ() - Alpha * D.XYZ());
  Alpha  = Vector3 * gp_Vec(Z);
  Alpha /=  D * Z;

  V3.SetXYZ( Vector3.XYZ() - Alpha * D.XYZ());
  return Standard_True;
}

//=======================================================================
//  class  : ProjLib_OnPlane
//purpose  : Use to approximate the projection on a plane
//=======================================================================

class ProjLib_OnPlane : public AppCont_Function

{
  Handle(Adaptor3d_HCurve) myCurve;
  gp_Ax3 myPlane;
  gp_Dir myDirection;

public :

  ProjLib_OnPlane(const Handle(Adaptor3d_HCurve)& C, 
                  const gp_Ax3& Pl, 
                  const gp_Dir& D) 
: myCurve(C),
  myPlane(Pl),
  myDirection(D)
  {
    myNbPnt = 1;
    myNbPnt2d = 0;
  }

  Standard_Real FirstParameter() const
    {return myCurve->FirstParameter();}
  
  Standard_Real LastParameter() const
    {return myCurve->LastParameter();}

    Standard_Boolean Value(const Standard_Real   theT,
                           NCollection_Array1<gp_Pnt2d>& /*thePnt2d*/,
                           NCollection_Array1<gp_Pnt>&   thePnt) const
    {
      thePnt(1) = OnPlane_Value(theT, myCurve, myPlane, myDirection);
      return Standard_True;
    }
  
  Standard_Boolean D1(const Standard_Real   theT,
                      NCollection_Array1<gp_Vec2d>& /*theVec2d*/,
                      NCollection_Array1<gp_Vec>&   theVec) const
  {
    gp_Pnt aDummyPnt;
    return OnPlane_D1(theT, aDummyPnt, theVec(1),myCurve,myPlane,myDirection);
  }
};




//=====================================================================//
//                                                                     //
//  D E S C R I P T I O N   O F   T H E   C L A S S  :                 // 
//                                                                     //
//         P r o j L i b _ A p p r o x P r o j e c t O n P l a n e     //
//                                                                     //
//=====================================================================//



//=======================================================================
//function : PerformApprox
//purpose  : 
//=======================================================================

static void  PerformApprox (const Handle(Adaptor3d_HCurve)& C,
			    const gp_Ax3& Pl,
			    const gp_Dir& D,
			    Handle(Geom_BSplineCurve) &BSplineCurvePtr) 

{
  ProjLib_OnPlane F(C,Pl,D);

  Standard_Integer Deg1, Deg2;
  Deg1 = 8; Deg2 = 8;
  
  Approx_FitAndDivide Fit(F,Deg1,Deg2,Precision::Approximation(),
			  Precision::PApproximation(),Standard_True);
  Standard_Integer i;
  Standard_Integer NbCurves = Fit.NbMultiCurves();
  Standard_Integer MaxDeg = 0;
  
  // Pour transformer la MultiCurve en BSpline, il faut que toutes 
  // les Bezier la constituant aient le meme degre -> Calcul de MaxDeg
  Standard_Integer NbPoles  = 1;
  for (i = 1; i <= NbCurves; i++) {
    Standard_Integer Deg = Fit.Value(i).Degree();
    MaxDeg = Max ( MaxDeg, Deg);
  }
  NbPoles = MaxDeg * NbCurves + 1;               //Poles sur la BSpline

  TColgp_Array1OfPnt    Poles( 1, NbPoles);
  
  TColgp_Array1OfPnt TempPoles( 1, MaxDeg + 1);  //pour augmentation du degre
  
  TColStd_Array1OfReal Knots( 1, NbCurves + 1);  //Noeuds de la BSpline
  
  Standard_Integer Compt = 1;
  for (i = 1; i <= Fit.NbMultiCurves(); i++) {
    Fit.Parameters(i, Knots(i), Knots(i+1)); 
    
    AppParCurves_MultiCurve MC = Fit.Value( i);   //Charge la Ieme Curve
    TColgp_Array1OfPnt LocalPoles( 1, MC.Degree() + 1);//Recupere les poles
    MC.Curve(1, LocalPoles);
    
    //Augmentation eventuelle du degre
    Standard_Integer Inc = MaxDeg - MC.Degree();
    if ( Inc > 0) {
      BSplCLib::IncreaseDegree(Inc, Poles, BSplCLib::NoWeights(),
			       TempPoles, BSplCLib::NoWeights());
      //mise a jour des poles de la PCurve
      for (Standard_Integer j = 1 ; j <= MaxDeg + 1; j++) {
	Poles.SetValue( Compt, TempPoles( j));
	Compt++;
      }
    }
    else {
      //mise a jour des poles de la PCurve
      for (Standard_Integer j = 1 ; j <= MaxDeg + 1; j++) {
	Poles.SetValue( Compt, LocalPoles( j));
	Compt++;
      }
    } 
    
    Compt--;
  }
  
  //mise a jour des fields de ProjLib_Approx

  Standard_Integer 
  NbKnots = NbCurves + 1;

  TColStd_Array1OfInteger    Mults(1, NbKnots);
  Mults.SetValue( 1, MaxDeg + 1);
  for ( i = 2; i <= NbCurves; i++) {
    Mults.SetValue( i, MaxDeg);
  }
  Mults.SetValue( NbKnots, MaxDeg + 1);
  BSplineCurvePtr = 
    new Geom_BSplineCurve(Poles,Knots,Mults,MaxDeg,Standard_False);
}


//=======================================================================
//function : ProjectOnPlane
//purpose  : 
//=======================================================================

ProjLib_ProjectOnPlane::ProjLib_ProjectOnPlane() :
myKeepParam(Standard_False),
myFirstPar(0.),
myLastPar(0.),
myTolerance(0.),
myType     (GeomAbs_OtherCurve),
myIsApprox (Standard_False)
{
}

//=======================================================================
//function : ProjLib_ProjectOnPlane
//purpose  : 
//=======================================================================

ProjLib_ProjectOnPlane::ProjLib_ProjectOnPlane(const gp_Ax3& Pl) : 
myPlane     (Pl)                ,
myDirection (Pl.Direction())    ,
myKeepParam(Standard_False),
myFirstPar(0.),
myLastPar(0.),
myTolerance(0.),
myType      (GeomAbs_OtherCurve),
myIsApprox  (Standard_False)
{
}

//=======================================================================
//function : ProjLib_ProjectOnPlane
//purpose  : 
//=======================================================================

ProjLib_ProjectOnPlane::ProjLib_ProjectOnPlane(const gp_Ax3& Pl,
					       const gp_Dir& D  ) :
myPlane     (Pl)                ,
myDirection (D)                 ,
myKeepParam(Standard_False),
myFirstPar(0.),
myLastPar(0.),
myTolerance(0.),
myType      (GeomAbs_OtherCurve),
myIsApprox  (Standard_False)
{
//  if ( Abs(D * Pl.Direction()) < Precision::Confusion()) {
//    throw Standard_ConstructionError
//      ("ProjLib_ProjectOnPlane:  The Direction and the Plane are parallel");
//  }
}

//=======================================================================
//function : Project
//purpose  : Returns the projection of a point <Point> on a plane 
//           <ThePlane>  along a direction <TheDir>.
//=======================================================================

static gp_Pnt ProjectPnt(const gp_Ax3& ThePlane,
			 const gp_Dir& TheDir,
			 const gp_Pnt& Point) 
{
  gp_Vec PO(Point,ThePlane.Location());

  Standard_Real Alpha = PO * gp_Vec(ThePlane.Direction());
  Alpha /= TheDir * ThePlane.Direction();
  
  gp_Pnt P;
  P.SetXYZ(Point.XYZ() + Alpha * TheDir.XYZ());

  return P;
}


//=======================================================================
//function : Project
//purpose  : Returns the projection of a Vector <Vec> on a plane 
//           <ThePlane> along a direction <TheDir>.
//=======================================================================

static gp_Vec ProjectVec(const gp_Ax3& ThePlane,
			 const gp_Dir& TheDir,
			 const gp_Vec& Vec) 
{
  gp_Vec D = Vec;
  gp_Vec Z = ThePlane.Direction();

  D -= ( (Vec * Z) / (TheDir * Z)) * TheDir;

  return D;
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void ProjLib_ProjectOnPlane::Load(const Handle(Adaptor3d_HCurve)&    C,
				  const Standard_Real Tolerance,
				  const Standard_Boolean KeepParametrization) 
				  
{
  myCurve      = C;
  myType       = GeomAbs_OtherCurve;
  myIsApprox   = Standard_False;
  myTolerance  = Tolerance ;

  Handle(Geom_BSplineCurve)  ApproxCurve;
  Handle(GeomAdaptor_HCurve) aGAHCurve;

  Handle(Geom_Line)      GeomLinePtr;
  Handle(Geom_Circle)    GeomCirclePtr ;
  Handle(Geom_Ellipse)   GeomEllipsePtr ;
  Handle(Geom_Hyperbola) GeomHyperbolaPtr ;

  gp_Lin   aLine; 
  gp_Elips Elips;
//  gp_Hypr  Hypr ;

  Standard_Integer num_knots ;
  GeomAbs_CurveType Type = C->GetType();

  gp_Ax2 Axis;
  Standard_Real R1 =0., R2 =0.;

  if ( Type != GeomAbs_Line)  // on garde le parametrage
    myKeepParam = Standard_True;
  else                        // on prend le choix utilisateur.
    myKeepParam = KeepParametrization;

  switch ( Type) {
  case GeomAbs_Line: 
    {
      //     P(u) = O + u * Xc
      // ==> Q(u) = f(P(u)) 
      //          = f(O) + u * f(Xc)

      gp_Lin L  = myCurve->Line();
      gp_Vec Xc = ProjectVec(myPlane,myDirection,gp_Vec(L.Direction()));

      if ( Xc.Magnitude() < Precision::Confusion()) { // line orthog au plan
	myType = GeomAbs_BSplineCurve;
	gp_Pnt P = ProjectPnt(myPlane,myDirection,L.Location());
	TColStd_Array1OfInteger Mults(1,2); Mults.Init(2);
	TColgp_Array1OfPnt      Poles(1,2); Poles.Init(P);
	TColStd_Array1OfReal    Knots(1,2); 
	Knots(1) = myCurve->FirstParameter();
	Knots(2) = myCurve->LastParameter();
	Handle(Geom_BSplineCurve) BSP = 
	  new Geom_BSplineCurve(Poles,Knots,Mults,1);

//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
	GeomAdaptor_Curve aGACurve(BSP);
	myResult = new GeomAdaptor_HCurve(aGACurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
      }
      else if ( Abs( Xc.Magnitude() - 1.) < Precision::Confusion()) {
	myType      = GeomAbs_Line;
	gp_Pnt P    = ProjectPnt(myPlane,myDirection,L.Location());
        myFirstPar  = myCurve->FirstParameter();
        myLastPar   = myCurve->LastParameter();
        aLine       = gp_Lin(P,gp_Dir(Xc));
        GeomLinePtr = new Geom_Line(aLine);
	
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
	GeomAdaptor_Curve aGACurve(GeomLinePtr,
				   myCurve->FirstParameter(),
				   myCurve->LastParameter() );
	myResult = new GeomAdaptor_HCurve(aGACurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
      }
      else {
	myType     = GeomAbs_Line;
	gp_Pnt P   = ProjectPnt(myPlane,myDirection,L.Location());
        aLine      = gp_Lin(P,gp_Dir(Xc));
	Standard_Real Udeb, Ufin;
	
	// eval the first and last parameters of the projected curve
	Udeb = myCurve->FirstParameter();
	Ufin = myCurve->LastParameter();
	gp_Pnt P1  = ProjectPnt(myPlane,myDirection,
				myCurve->Value(Udeb)); 
	gp_Pnt P2  = ProjectPnt(myPlane,myDirection,
				myCurve->Value(Ufin)); 
	myFirstPar = gp_Vec(aLine.Direction()).Dot(gp_Vec(P,P1));
	myLastPar  = gp_Vec(aLine.Direction()).Dot(gp_Vec(P,P2));
        GeomLinePtr = new Geom_Line(aLine);
	if (!myKeepParam) {
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
	  GeomAdaptor_Curve aGACurve(GeomLinePtr,
				     myFirstPar,
				     myLastPar) ;
	  myResult = new GeomAdaptor_HCurve(aGACurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
	}
	else {
	  myType     = GeomAbs_BSplineCurve;
	  //
	  // make a linear BSpline of degree 1 between the end points of
	  // the projected line 
	  //
	  Handle(Geom_TrimmedCurve) NewTrimCurvePtr =
	    new Geom_TrimmedCurve(GeomLinePtr,
				  myFirstPar,
				  myLastPar) ;
	  
	  Handle(Geom_BSplineCurve) NewCurvePtr =
	    GeomConvert::CurveToBSplineCurve(NewTrimCurvePtr) ;
	  num_knots = NewCurvePtr->NbKnots() ;
	  TColStd_Array1OfReal    BsplineKnots(1,num_knots)  ;
	  NewCurvePtr->Knots(BsplineKnots) ;
	  
	  BSplCLib::Reparametrize(myCurve->FirstParameter(), 
				  myCurve->LastParameter(),
				  BsplineKnots) ;
	  
	  NewCurvePtr->SetKnots(BsplineKnots) ;
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
	  GeomAdaptor_Curve aGACurve(NewCurvePtr);
	  myResult = new GeomAdaptor_HCurve(aGACurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
	}
      }
      break;
    }
  case GeomAbs_Circle:
    {
      // Pour le cercle et l ellipse on a les relations suivantes:
      // ( Rem : pour le cercle R1 = R2 = R)
      //     P(u) = O + R1 * Cos(u) * Xc + R2 * Sin(u) * Yc
      // ==> Q(u) = f(P(u)) 
      //          = f(O) + R1 * Cos(u) * f(Xc) + R2 * Sin(u) * f(Yc)
      
      gp_Circ Circ = myCurve->Circle();
      Axis = Circ.Position();
      R1 = R2 = Circ.Radius();

    }
  case GeomAbs_Ellipse:
    {
      if ( Type == GeomAbs_Ellipse) {
	gp_Elips E = myCurve->Ellipse();
	Axis = E.Position();
	R1 = E.MajorRadius();
	R2 = E.MinorRadius();
      }

      // Common Code  for CIRCLE & ELLIPSE begin here
      gp_Dir X  = Axis.XDirection();
      gp_Dir Y  = Axis.YDirection();
      gp_Vec VDx = ProjectVec(myPlane,myDirection,X);
      gp_Vec VDy = ProjectVec(myPlane,myDirection,Y);
      gp_Dir Dx,Dy;

      Standard_Real Tol2 = myTolerance*myTolerance;
      if (VDx.SquareMagnitude() < Tol2 ||
	  VDy.SquareMagnitude() < Tol2    ) {
	myIsApprox = Standard_True;
      }

      if (!myIsApprox && 
	  gp_Dir(VDx).IsNormal(gp_Dir(VDy),Precision::Angular())) {
	Dx = gp_Dir(VDx);
	Dy = gp_Dir(VDy);
	gp_Pnt O    = Axis.Location();
	gp_Pnt P    = ProjectPnt(myPlane,myDirection,O);
	gp_Pnt Px   = ProjectPnt(myPlane,myDirection,O.Translated(R1*gp_Vec(X)));
	gp_Pnt Py   = ProjectPnt(myPlane,myDirection,O.Translated(R2*gp_Vec(Y)));
	Standard_Real Major = P.Distance(Px);
	Standard_Real Minor = P.Distance(Py);
	gp_Ax2 Axe( P, Dx^Dy,Dx);

	if ( Abs( Major - Minor) < Precision::Confusion()) {
	  myType = GeomAbs_Circle;
 	  gp_Circ Circ(Axe, Major);
	  GeomCirclePtr  = new Geom_Circle(Circ);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
	  GeomAdaptor_Curve aGACurve(GeomCirclePtr);
	  myResult = new GeomAdaptor_HCurve(aGACurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
	}
	else if ( Major > Minor) {
	  myType = GeomAbs_Ellipse;
	  Elips  = gp_Elips( Axe, Major, Minor);
	  
          GeomEllipsePtr = new Geom_Ellipse(Elips) ;
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
	  GeomAdaptor_Curve aGACurve(GeomEllipsePtr);
	  myResult = new GeomAdaptor_HCurve(aGACurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
	}
	else {
	  myIsApprox = Standard_True;
	  myType = GeomAbs_BSplineCurve;
          PerformApprox(myCurve,myPlane,myDirection,ApproxCurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
	  GeomAdaptor_Curve aGACurve(ApproxCurve);
	  myResult = new GeomAdaptor_HCurve(aGACurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
	}
      }
      else {
	myIsApprox = Standard_True;
	myType     = GeomAbs_BSplineCurve;
        PerformApprox(myCurve,myPlane,myDirection,ApproxCurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
	GeomAdaptor_Curve aGACurve(ApproxCurve);
	myResult = new GeomAdaptor_HCurve(aGACurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
      }
    }
    break;
  case GeomAbs_Parabola:
    {
      //     P(u) = O + (u*u)/(4*f) * Xc + u * Yc
      // ==> Q(u) = f(P(u)) 
      //          = f(O) + (u*u)/(4*f) * f(Xc) + u * f(Yc)

      gp_Parab Parab  = myCurve->Parabola();
      gp_Ax2   AxeRef = Parab.Position();
      gp_Vec Xc = ProjectVec(myPlane,myDirection,gp_Vec(AxeRef.XDirection()));
      gp_Vec Yc = ProjectVec(myPlane,myDirection,gp_Vec(AxeRef.YDirection()));

      // fix for case when no one action is done. 28.03.2002
      Standard_Boolean alocalIsDone = Standard_False;

      if ( Abs( Yc.Magnitude() - 1.) < Precision::Confusion()) {
	gp_Pnt P = ProjectPnt(myPlane,myDirection,AxeRef.Location());
	if ( Xc.Magnitude() < Precision::Confusion()) {
	  myType = GeomAbs_Line;
	  aLine  = gp_Lin( P, gp_Dir(Yc));
          
          GeomLinePtr    = new Geom_Line(aLine) ;
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
	  GeomAdaptor_Curve aGACurve(GeomLinePtr);
	  myResult = new GeomAdaptor_HCurve(aGACurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
          alocalIsDone = Standard_True;
	}
	else if ( Xc.IsNormal(Yc,Precision::Angular())) {
	  myType = GeomAbs_Parabola;
	  Standard_Real F = Parab.Focal() / Xc.Magnitude();
	  gp_Parab aParab = gp_Parab( gp_Ax2(P,Xc^Yc,Xc), F);
          Handle(Geom_Parabola) GeomParabolaPtr =
	    new Geom_Parabola(aParab) ;
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
	  GeomAdaptor_Curve aGACurve(GeomParabolaPtr);
	  myResult = new GeomAdaptor_HCurve(aGACurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
          alocalIsDone = Standard_True;
	}
      }
      if (!alocalIsDone)/*else*/ {
	myIsApprox = Standard_True;
	myType     = GeomAbs_BSplineCurve;
        PerformApprox(myCurve,myPlane,myDirection,ApproxCurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
	GeomAdaptor_Curve aGACurve(ApproxCurve);
	myResult = new GeomAdaptor_HCurve(aGACurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
      }
    }
    break;
  case GeomAbs_Hyperbola:
    {
      //     P(u) = O + R1 * Cosh(u) * Xc + R2 * Sinh(u) * Yc
      // ==> Q(u) = f(P(u)) 
      //          = f(O) + R1 * Cosh(u) * f(Xc) + R2 * Sinh(u) * f(Yc)

      gp_Hypr Hypr  = myCurve->Hyperbola();
      gp_Ax2 AxeRef = Hypr.Position();
      gp_Vec Xc = ProjectVec(myPlane,myDirection,gp_Vec(AxeRef.XDirection()));
      gp_Vec Yc = ProjectVec(myPlane,myDirection,gp_Vec(AxeRef.YDirection()));
      gp_Pnt P  = ProjectPnt(myPlane,myDirection,AxeRef.Location());
      Standard_Real aR1 = Hypr.MajorRadius();
      Standard_Real aR2 = Hypr.MinorRadius();
      gp_Dir Z = myPlane.Direction();

      if ( Xc.Magnitude() < Precision::Confusion()) {
	myType   = GeomAbs_Hyperbola;
	gp_Dir X = gp_Dir(Yc) ^ Z;
	Hypr   = gp_Hypr(gp_Ax2( P, Z, X), 0., aR2 * Yc.Magnitude());
        GeomHyperbolaPtr =
	  new Geom_Hyperbola(Hypr) ;
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
	GeomAdaptor_Curve aGACurve(GeomHyperbolaPtr);
	myResult = new GeomAdaptor_HCurve(aGACurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
      }
      else if ( Yc.Magnitude() < Precision::Confusion()) {
	myType = GeomAbs_Hyperbola;
	Hypr = 
	   gp_Hypr(gp_Ax2(P, Z, gp_Dir(Xc)), aR1 * Xc.Magnitude(), 0.);
        GeomHyperbolaPtr =
	  new Geom_Hyperbola(Hypr) ;
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
	GeomAdaptor_Curve aGACurve(GeomHyperbolaPtr);
	myResult = new GeomAdaptor_HCurve(aGACurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
      }
      else if ( Xc.IsNormal(Yc,Precision::Angular())) {
	myType = GeomAbs_Hyperbola;
	Hypr = gp_Hypr( gp_Ax2( P, gp_Dir( Xc ^ Yc), gp_Dir( Xc)),
		       aR1 * Xc.Magnitude(), aR2 * Yc.Magnitude() );
	GeomHyperbolaPtr =
	  new Geom_Hyperbola(Hypr) ;
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
	GeomAdaptor_Curve aGACurve(GeomHyperbolaPtr);
	myResult = new GeomAdaptor_HCurve(aGACurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
      }
      else {
	myIsApprox = Standard_True;
	myType     = GeomAbs_BSplineCurve;
        PerformApprox(myCurve,myPlane,myDirection,ApproxCurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
	GeomAdaptor_Curve aGACurve(ApproxCurve);
	myResult = new GeomAdaptor_HCurve(aGACurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
      }
    }
    break;
  case GeomAbs_BezierCurve:
    {
      Handle(Geom_BezierCurve) BezierCurvePtr =
	myCurve->Bezier() ;
      Standard_Integer NbPoles = 
	BezierCurvePtr->NbPoles() ;
      
      Handle(Geom_BezierCurve) ProjCu = 
	Handle(Geom_BezierCurve)::DownCast(BezierCurvePtr->Copy());

      myIsApprox = Standard_False;
      myType = Type;
      for ( Standard_Integer i = 1; i <= NbPoles; i++) {
	ProjCu->SetPole
	  (i,ProjectPnt(myPlane,myDirection,BezierCurvePtr->Pole(i)));
      }
      
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
      GeomAdaptor_Curve aGACurve(ProjCu);
      myResult = new GeomAdaptor_HCurve(aGACurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
    }
    break ;
  case GeomAbs_BSplineCurve:
    {
      Handle(Geom_BSplineCurve) BSplineCurvePtr =
	myCurve->BSpline() ;
      //
      //    make a copy of the curve and projects its poles 
      //
      Handle(Geom_BSplineCurve) ProjectedBSplinePtr =
	Handle(Geom_BSplineCurve)::DownCast(BSplineCurvePtr->Copy()) ;
      
      myIsApprox = Standard_False;
      myType = Type;
      for ( Standard_Integer i = 1; i <= BSplineCurvePtr->NbPoles(); i++) {
	ProjectedBSplinePtr->SetPole
	  (i,ProjectPnt(myPlane,myDirection,BSplineCurvePtr->Pole(i)));
      }
      
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
      GeomAdaptor_Curve aGACurve(ProjectedBSplinePtr);
      myResult = new GeomAdaptor_HCurve(aGACurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
    }
    break;
  default:
    {
      myIsApprox = Standard_True;
      myType     = GeomAbs_BSplineCurve;
      PerformApprox(myCurve,myPlane,myDirection,ApproxCurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
      GeomAdaptor_Curve aGACurve(ApproxCurve);
      myResult = new GeomAdaptor_HCurve(aGACurve);
//  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
    }
    break;
  }
}

//=======================================================================
//function : GetPlane
//purpose  : 
//=======================================================================

const gp_Ax3& ProjLib_ProjectOnPlane::GetPlane() const 
{
  return myPlane;
}

//=======================================================================
//function : GetDirection
//purpose  : 
//=======================================================================

const gp_Dir& ProjLib_ProjectOnPlane::GetDirection() const 
{
  return myDirection;
}

//=======================================================================
//function : GetCurve
//purpose  : 
//=======================================================================

const Handle(Adaptor3d_HCurve)& ProjLib_ProjectOnPlane::GetCurve() const
{
  return myCurve;
}

//=======================================================================
//function : GetResult
//purpose  : 
//=======================================================================

const Handle(GeomAdaptor_HCurve)& ProjLib_ProjectOnPlane::GetResult() const
{
  return myResult;
}


//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real ProjLib_ProjectOnPlane::FirstParameter() const 
{
  if ( myKeepParam || myIsApprox) 
    return myCurve->FirstParameter();
  else
    return myFirstPar;
}


//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real ProjLib_ProjectOnPlane::LastParameter() const 
{
  if ( myKeepParam || myIsApprox) 
    return myCurve->LastParameter();
  else
    return myLastPar;
}


//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape ProjLib_ProjectOnPlane::Continuity() const
{
  return myCurve->Continuity() ;
}


//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================

Standard_Integer ProjLib_ProjectOnPlane::NbIntervals(const GeomAbs_Shape S) const
{
  return myCurve->NbIntervals(S) ;
}


//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================

void ProjLib_ProjectOnPlane::Intervals(TColStd_Array1OfReal& T, 
                                       const GeomAbs_Shape S) const
{
  myCurve->Intervals(T,S) ;
}

//=======================================================================
//function : Trim
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HCurve)  
ProjLib_ProjectOnPlane::Trim(const Standard_Real First,
			     const Standard_Real Last,
			     const Standard_Real Tolerance) const 
{
  if (myType != GeomAbs_OtherCurve){
    return myResult->Trim(First,Last,Tolerance) ;
  }
  else {
    throw Standard_NotImplemented ("ProjLib_ProjectOnPlane::Trim() - curve of unsupported type");
  }
}

  
//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean ProjLib_ProjectOnPlane::IsClosed() const
{
 return myCurve->IsClosed() ;
}


//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean ProjLib_ProjectOnPlane::IsPeriodic() const
{
  if ( myIsApprox)
    return Standard_False;
  else 
    return myCurve->IsPeriodic();
}


//=======================================================================
//function : Period
//purpose  : 
//=======================================================================

Standard_Real ProjLib_ProjectOnPlane::Period() const
{
  if ( !IsPeriodic()) {
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane::Period");
  }
				
  if ( myIsApprox)
    return Standard_False;
  else 
    return myCurve->Period();
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt ProjLib_ProjectOnPlane::Value(const Standard_Real U) const 
{
  if (myType != GeomAbs_OtherCurve) { 
    return myResult->Value(U);
  }
  else {
    return OnPlane_Value(U,
			 myCurve,
			 myPlane,
			 myDirection);
    
  }
} 


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void ProjLib_ProjectOnPlane::D0(const Standard_Real U , gp_Pnt& P) const
{
  if (myType != GeomAbs_OtherCurve) {
    myResult->D0(U,P) ;
  }
  else {
    P = OnPlane_Value(U,
		      myCurve,
		      myPlane,
		      myDirection);
  }
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void ProjLib_ProjectOnPlane::D1(const Standard_Real U,
			              gp_Pnt&    P , 
                                      gp_Vec&    V ) const 
{
  if (myType != GeomAbs_OtherCurve) {
    myResult->D1(U,P,V) ;
  }
  else {
    OnPlane_D1(U,
	       P,
	       V,
	       myCurve,
	       myPlane,
	       myDirection);
  }
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void ProjLib_ProjectOnPlane::D2(const Standard_Real U, 
			              gp_Pnt&     P, 
                                      gp_Vec&     V1, 
                                      gp_Vec&     V2) const 
{
  if (myType != GeomAbs_OtherCurve)  {
    myResult->D2(U,P,V1,V2) ;
  }
  else {
    OnPlane_D2(U,
	       P,
	       V1,
	       V2,
	       myCurve,
	       myPlane,
	       myDirection);
  }
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void ProjLib_ProjectOnPlane::D3(const Standard_Real U, 
				gp_Pnt& P, 
				gp_Vec& V1, 
				gp_Vec& V2, 
				gp_Vec& V3) const 
{
  if (myType != GeomAbs_OtherCurve)   {
    myResult->D3(U,P,V1,V2,V3) ;
  }
  else   {
    OnPlane_D3(U,
	       P,
	       V1,
	       V2,
	       V3,
	       myCurve,
	       myPlane,
	       myDirection); 
  }
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec ProjLib_ProjectOnPlane::DN(const Standard_Real U, 
				  const Standard_Integer DerivativeRequest)
     const 
{
  if (myType != GeomAbs_OtherCurve) {
    return myResult->DN(U,DerivativeRequest) ;
  }
  else {
    return OnPlane_DN(U,
		      DerivativeRequest,
		      myCurve,
		      myPlane,
		      myDirection); 
  }   
}


//=======================================================================
//function : Resolution
//purpose  : 
//=======================================================================

Standard_Real ProjLib_ProjectOnPlane::Resolution
(const Standard_Real Tolerance) const 
{
  if (myType != GeomAbs_OtherCurve) {
    return myResult->Resolution(Tolerance) ;
  }
  else {
    return 0;
  }
}
    

//=======================================================================
//function : GetType
//purpose  : 
//=======================================================================

GeomAbs_CurveType ProjLib_ProjectOnPlane::GetType() const
{
  return myType;
}


//=======================================================================
//function : Line
//purpose  : 
//=======================================================================

gp_Lin ProjLib_ProjectOnPlane::Line() const
{
  if (myType != GeomAbs_Line)
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:Line");

  return myResult->Line();
}


//=======================================================================
//function : Circle
//purpose  : 
//=======================================================================

gp_Circ ProjLib_ProjectOnPlane::Circle() const
{
  if (myType != GeomAbs_Circle)
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:Circle");

  return myResult->Circle();
}


//=======================================================================
//function : Ellipse
//purpose  : 
//=======================================================================

gp_Elips ProjLib_ProjectOnPlane::Ellipse() const
{
  if (myType != GeomAbs_Ellipse)
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:Ellipse");

  return myResult->Ellipse();
}


//=======================================================================
//function : Hyperbola
//purpose  : 
//=======================================================================

gp_Hypr ProjLib_ProjectOnPlane::Hyperbola() const
{
  if (myType != GeomAbs_Hyperbola)
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:Hyperbola");

  return myResult->Hyperbola() ;
}


//=======================================================================
//function : Parabola
//purpose  : 
//=======================================================================

gp_Parab ProjLib_ProjectOnPlane::Parabola() const
{
  if (myType != GeomAbs_Parabola)
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:Parabola");
  
  return myResult->Parabola() ;
}

//=======================================================================
//function : Degree
//purpose  : 
//=======================================================================

Standard_Integer ProjLib_ProjectOnPlane::Degree() const
{
  if ((GetType() != GeomAbs_BSplineCurve) &&
      (GetType() != GeomAbs_BezierCurve))
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:Degree");

  if ( myIsApprox)
    return myResult->Degree();
  else
    return myCurve->Degree();
}

//=======================================================================
//function : IsRational
//purpose  : 
//=======================================================================

Standard_Boolean ProjLib_ProjectOnPlane::IsRational() const 
{
  if ((GetType() != GeomAbs_BSplineCurve) &&
      (GetType() != GeomAbs_BezierCurve))
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:IsRational");
  
  if ( myIsApprox) 
    return myResult->IsRational();
  else
    return myCurve->IsRational();
}

//=======================================================================
//function : NbPoles
//purpose  : 
//=======================================================================

Standard_Integer ProjLib_ProjectOnPlane::NbPoles() const
{
  if ((GetType() != GeomAbs_BSplineCurve) &&
      (GetType() != GeomAbs_BezierCurve))
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:NbPoles");
  
  if ( myIsApprox)
    return myResult->NbPoles();
  else
    return myCurve->NbPoles();
}

//=======================================================================
//function : NbKnots
//purpose  : 
//=======================================================================

Standard_Integer ProjLib_ProjectOnPlane::NbKnots() const 
{
  if ( GetType() != GeomAbs_BSplineCurve) 
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:NbKnots");
  
  if ( myIsApprox) 
    return myResult->NbKnots();
  else
    return myCurve->NbKnots();
}


//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================

Handle(Geom_BezierCurve)  ProjLib_ProjectOnPlane::Bezier() const
{
  if (myType != GeomAbs_BezierCurve)
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:Bezier");

  return myResult->Bezier() ;
}

//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================

Handle(Geom_BSplineCurve)  ProjLib_ProjectOnPlane::BSpline() const
{
  if (myType != GeomAbs_BSplineCurve)
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:BSpline");

  return myResult->BSpline() ;
}

