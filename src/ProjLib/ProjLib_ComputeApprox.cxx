// Created on: 1993-09-07
// Created by: Bruno DUMORTIER
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

// modified by NIZHNY-OFV  Thu Jan 20 11:04:19 2005

#include <ProjLib_ComputeApprox.hxx>

#include <GeomAbs_SurfaceType.hxx>
#include <GeomAbs_CurveType.hxx>
#include <AppCont_Function.hxx>
#include <Convert_CompBezierCurves2dToBSplineCurve2d.hxx>
#include <ElSLib.hxx>
#include <ElCLib.hxx>
#include <BSplCLib.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <gp.hxx>
#include <gp_Trsf.hxx>
#include <Precision.hxx>
#include <Approx_FitAndDivide2d.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <GCPnts_AbscissaPoint.hxx>

//#define DRAW
#ifdef DRAW
#include <DrawTrSurf.hxx>
#endif
#ifdef OCCT_DEBUG
//static Standard_Boolean AffichValue = Standard_False;
#endif    

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================
// OFV:
static inline Standard_Boolean IsEqual(Standard_Real Check,Standard_Real With,Standard_Real Toler)
{
  return ((Abs(Check - With) < Toler) ? Standard_True : Standard_False);
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

static gp_Pnt2d Function_Value(const Standard_Real U,
			       const Handle(Adaptor3d_HCurve)&   myCurve,
			       const Handle(Adaptor3d_HSurface)& mySurface,
			       const Standard_Real U1,
			       const Standard_Real U2, 
			       const Standard_Real V1,
			       const Standard_Real V2,
			       const Standard_Boolean UCouture,
			       const Standard_Boolean VCouture ) 
{
  Standard_Real S = 0., T = 0.;

  gp_Pnt P3d = myCurve->Value(U);
  GeomAbs_SurfaceType SType = mySurface->GetType();

  switch ( SType ) {
    
  case GeomAbs_Plane:
    {
      gp_Pln Plane = mySurface->Plane();
      ElSLib::Parameters( Plane, P3d, S, T);
      break;
    }
  case GeomAbs_Cylinder:
    {
      gp_Cylinder Cylinder = mySurface->Cylinder();
      ElSLib::Parameters( Cylinder, P3d, S, T);
      break;
    }
  case GeomAbs_Cone:
    {
      gp_Cone Cone = mySurface->Cone();
      ElSLib::Parameters( Cone, P3d, S, T);
      break;
    }
  case GeomAbs_Sphere:
    {
      gp_Sphere Sphere = mySurface->Sphere();
      ElSLib::Parameters(Sphere, P3d, S, T);
      break;
    }
  case GeomAbs_Torus:
    {
      gp_Torus Torus = mySurface->Torus();
      ElSLib::Parameters( Torus, P3d, S, T);
      break;
    }
  default:
    throw Standard_NoSuchObject("ProjLib_ComputeApprox::Value");
  }

  if ( UCouture) {
    if(S < U1 || S > U2)
    {
        S = ElCLib::InPeriod(S, U1, U2);
    }
  }
 
  if ( VCouture) {
    if(SType == GeomAbs_Sphere) {
      if ( Abs( S - U1 ) > M_PI ) {
	      T = M_PI - T;
	      S = M_PI + S;
      }
      if(S > U1 || S < U2)
        S = ElCLib::InPeriod(S, U1, U2);
    }
    if(T < V1 || T > V2)
      T = ElCLib::InPeriod(T, V1, V2);
  }

  return gp_Pnt2d(S, T);
}
//=======================================================================
//function : D1
//purpose  : 
//=======================================================================
static Standard_Boolean Function_D1( const Standard_Real U, 
				    gp_Pnt2d&            P,
				    gp_Vec2d&            D,
				    const Handle(Adaptor3d_HCurve)&   myCurve,
				    const Handle(Adaptor3d_HSurface)& mySurface,
				    const Standard_Real U1,
				    const Standard_Real U2, 
				    const Standard_Real V1,
				    const Standard_Real V2,
				    const Standard_Boolean UCouture,
				    const Standard_Boolean VCouture )
{
  gp_Pnt P3d;
  Standard_Real dU, dV;
  
  P = Function_Value(U,myCurve,mySurface,U1,U2,V1,V2,UCouture,VCouture);

  GeomAbs_SurfaceType Type = mySurface->GetType();
  
  switch ( Type) {
  case GeomAbs_Plane:
  case GeomAbs_Cone:
  case GeomAbs_Cylinder:
  case GeomAbs_Sphere:
  case GeomAbs_Torus: 
    {
      gp_Vec D1U, D1V;
      gp_Vec T;
      myCurve->D1(U,P3d,T);
      mySurface->D1(P.X(),P.Y(),P3d,D1U,D1V);
      
      dU = T.Dot(D1U);
      dV = T.Dot(D1V);
      Standard_Real Nu = D1U.SquareMagnitude();
      Standard_Real Nv = D1V.SquareMagnitude(); 
      
      if ( Nu < Epsilon(1.) || Nv < Epsilon(1.))
	      return Standard_False;
      
      dU /= Nu;
      dV /= Nv;
      D = gp_Vec2d( dU, dV);   
    }
    break;
    
  default:
    return Standard_False;
  }
  
  return Standard_True;
}
//=======================================================================
//function : Function_ComputeStep
//purpose  : 
//=======================================================================
static Standard_Real Function_ComputeStep(
  const Handle(Adaptor3d_HCurve)&   myCurve,
  const Standard_Real R)
{
  Standard_Real Step0 = .1;
  Standard_Real W1, W2;
  W1 = myCurve->FirstParameter();
  W2 = myCurve->LastParameter();
  Standard_Real L = GCPnts_AbscissaPoint::Length(myCurve->Curve());
  Standard_Integer nbp = RealToInt(L / (R*M_PI_4)) + 1;
  nbp = Max(nbp, 3);
  Standard_Real Step = (W2 - W1) / (nbp - 1);
  if (Step > Step0)
  {
    Step = Step0;
    nbp = RealToInt((W2 - W1) / Step) + 1;
    nbp = Max(nbp, 3);
    Step = (W2 - W1) / (nbp - 1);
  }

  return Step;

}
//=======================================================================
//function : Function_SetUVBounds
//purpose  : 
//=======================================================================
static void Function_SetUVBounds(Standard_Real& myU1, 
				 Standard_Real& myU2,
				 Standard_Real& myV1,
				 Standard_Real& myV2,
				 Standard_Boolean& UCouture,
				 Standard_Boolean& VCouture,
				 const Handle(Adaptor3d_HCurve)&   myCurve,
				 const Handle(Adaptor3d_HSurface)& mySurface) 
{
  Standard_Real W1, W2, W;
  gp_Pnt P1, P2, P;
  //
  W1 = myCurve->FirstParameter();
  W2 = myCurve->LastParameter ();
  W  = 0.5*(W1+W2);
  // on ouvre l`intervalle
  // W1 += 1.0e-9;
  // W2 -= 1.0e-9;
  P1 = myCurve->Value(W1);
  P2 = myCurve->Value(W2);
  P  = myCurve->Value(W);

  switch ( mySurface->GetType()) {

    case GeomAbs_Cone:    {
      Standard_Real tol = Epsilon(1.);
      Standard_Real ptol = Precision::PConfusion();
      gp_Cone Cone = mySurface->Cone();
      VCouture = Standard_False;
      //Calculation of cone parameters for P == ConeApex often produces wrong
      //values of U
      gp_Pnt ConeApex = Cone.Apex();
      if(ConeApex.XYZ().IsEqual(P1.XYZ(), tol))
      {
        W1 += ptol;
        P1 = myCurve->Value(W1);
      }
      if(ConeApex.XYZ().IsEqual(P2.XYZ(), tol))
      {
        W2 -= ptol;
        P2 = myCurve->Value(W2);
      }
      if(ConeApex.XYZ().IsEqual(P.XYZ(), tol))
      {
        W += ptol;
        P = myCurve->Value(W);
      }

      switch( myCurve->GetType() ){
      case GeomAbs_Parabola:
      case GeomAbs_Hyperbola:
      case GeomAbs_Ellipse:{
        Standard_Real U1, U2, V1, V2, U , V;
        ElSLib::Parameters( Cone, P1, U1, V1);
        ElSLib::Parameters( Cone, P2, U2, V2);
        ElSLib::Parameters( Cone, P , U , V );
        myU1 = Min(U1,U2);
        myU2 = Max(U1,U2);
        if (  ( U1 < U  &&  U < U2 ) && !myCurve->IsClosed() ) {
          UCouture = Standard_False;
        }      
        else {
          UCouture = Standard_True;
          myU2 = myU1 + 2*M_PI;
        }

      }
      break;
      default:	  { 
        Standard_Real U1, V1, U , V, Delta = 0., d = 0., pmin = W1, pmax = W1, dmax = 0., Uf, Ul;
        ElSLib::Parameters( Cone, P1, U1, V1);
        ElSLib::Parameters( Cone, P2, Ul, V1);
        const gp_Ax1& anAx1 = Cone.Axis();
        gp_Lin aLin(anAx1);
        Standard_Real R = (aLin.Distance(P1) + aLin.Distance(P2) + aLin.Distance(P)) / 3.;
        Standard_Real Step;
        myU1 = U1; myU2 = U1; Uf = U1;
        if (myCurve->GetType() == GeomAbs_Line)
        {
          Standard_Integer nbp = 3;
          Step = (W2 - W1) / (nbp - 1);
        }
        else
        {
          Step = Function_ComputeStep(myCurve, R);
        }
        //
        Standard_Boolean isclandper = (!(myCurve->IsClosed()) && !(myCurve->IsPeriodic()));
        Standard_Boolean isFirst = Standard_True;
        for(Standard_Real par = W1 + Step; par <= W2; par += Step)
        {
          if(!isclandper) par += Step;
          P = myCurve->Value(par);
          ElSLib::Parameters( Cone, P, U, V);
          U += Delta;
          d = U - U1;
          if(d > M_PI)
          {
            if( ( (IsEqual(U,(2*M_PI),1.e-10) && (U1 >= 0. && U1 <= M_PI)) && 
              (IsEqual(U,Ul,1.e-10) && !IsEqual(Uf,0.,1.e-10)) ) && isclandper )
              U = 0.0;
            else
            {
              // Protection against first-last point on seam.
              if (isFirst)
                U1 = 2*M_PI;
              else if (par + Step >= W2)
                U = 0.0;
              else
                Delta -= 2*M_PI;
            }
            U += Delta;
            d = U - U1;
          }
          else if(d < -M_PI)
          {
            if( ( (IsEqual(U,0.,1.e-10) && (U1 >= M_PI && U1 <= (2*M_PI))) &&
              (IsEqual(U,Ul,1.e-10) && !IsEqual(Uf,(2*M_PI),1.e-10)) ) && isclandper )
              U = 2*M_PI;
            else
            {
              // Protection against first-last point on seam.
              if (isFirst)
                U1 = 0.0;
              else if (par + Step >= W2)
                U = 2*M_PI;
              else
                Delta += 2*M_PI;
            }
            U += Delta;
            d = U - U1;
          }
          dmax = Max(dmax, Abs(d));
          if(U < myU1) {myU1 = U; pmin = par;}
          if(U > myU2) {myU2 = U; pmax = par;}
          U1 = U;
          isFirst = Standard_False;
        } // for(Standard_Real par = W1 + Step; par <= W2; par += Step)

        if(!(Abs(pmin - W1) <= Precision::PConfusion() || Abs(pmin - W2) <= Precision::PConfusion()) ) myU1 -= dmax*.5;
        if(!(Abs(pmax - W1) <= Precision::PConfusion() || Abs(pmax - W2) <= Precision::PConfusion()) ) myU2 += dmax*.5;

        if((myU1 >=0. && myU1 <= 2*M_PI) && (myU2 >=0. && myU2 <= 2*M_PI) ) UCouture = Standard_False;
        else{
          U = ( myU1 + myU2 ) /2.;
          myU1 = U - M_PI;
          myU2 = U + M_PI;
          UCouture = Standard_True;
        }
      }
      break;
    }// switch curve type
  }// case Cone
  break;

  case GeomAbs_Cylinder:    {
    gp_Cylinder Cylinder = mySurface->Cylinder();
    VCouture = Standard_False;

    if (myCurve->GetType() == GeomAbs_Ellipse) {

      Standard_Real U1, U2, V1, V2, U , V;
      ElSLib::Parameters( Cylinder, P1, U1, V1);
      ElSLib::Parameters( Cylinder, P2, U2, V2);
      ElSLib::Parameters( Cylinder, P , U , V );
      myU1 = Min(U1,U2);
      myU2 = Max(U1,U2);

      if ( !myCurve->IsClosed()) {
        if ( myU1 < U && U < myU2) {
          U = ( myU1 + myU2 ) /2.;
          myU1 = U - M_PI;
          myU2 = U + M_PI;
        }
        else {
          U = ( myU1 + myU2 ) /2.;
          if ( myU1 < U) {
            myU1 = U - 2*M_PI;
            myU2 = U;
          }
          else {
            myU1 = U;
            myU2 = U + 2*M_PI;
          }
        }
        UCouture = Standard_True;
      }
      else {
        gp_Vec D1U, D1V;
        gp_Vec T;
        gp_Pnt P3d;
        myCurve->D1(W1,P3d,T);
        mySurface->D1(U1,U2,P3d,D1U,D1V);
        Standard_Real dU = T.Dot(D1U);

        UCouture = Standard_True;
        if ( dU > 0.) {
          myU2 = myU1 + 2*M_PI;
        }
        else {
          myU2 = myU1;
          myU1 -= 2*M_PI;
        }
      }
    }
    else {
      Standard_Real U1, V1, U , V;
      ElSLib::Parameters( Cylinder, P1, U1, V1);
      Standard_Real R = Cylinder.Radius();
      Standard_Real Delta = 0., Step;
      Standard_Real eps = M_PI, dmax = 0., d = 0.;
      Step = Function_ComputeStep(myCurve, R);
      myU1 = U1; myU2 = U1;
      Standard_Real pmin = W1, pmax = W1, plim = W2+.1*Step;
      for(Standard_Real par = W1 + Step; par <= plim; par += Step) {
        P = myCurve->Value(par);
        ElSLib::Parameters( Cylinder, P, U, V);
        U += Delta;
        d = U - U1;
        if(d > eps) {
          U -= Delta;
          Delta -= 2*M_PI;
          U += Delta;
          d = U - U1;
        }
        else if(d < -eps) {
          U -= Delta;
          Delta += 2*M_PI;
          U += Delta;
          d = U - U1;
        }
        dmax = Max(dmax, Abs(d));
        if(U < myU1) {myU1 = U; pmin = par;}
        if(U > myU2) {myU2 = U; pmax = par;}
        U1 = U;
      }

      if(!(Abs(pmin - W1) <= Precision::PConfusion() ||
        Abs(pmin - W2) <= Precision::PConfusion())  ) myU1 -= dmax*.5;
      if(!(Abs(pmax - W1) <= Precision::PConfusion() ||
        Abs(pmax - W2) <= Precision::PConfusion())  ) myU2 += dmax*.5;

      if((myU1 >=0. && myU1 <= 2*M_PI) &&
        (myU2 >=0. && myU2 <= 2*M_PI)    ) {
          UCouture = Standard_False;
      }
      else {
        U = ( myU1 + myU2 ) /2.;
        myU1 = U - M_PI;
        myU2 = U + M_PI;
        UCouture = Standard_True;
      }
    }
  }
  break;
  //    
  case GeomAbs_Sphere:{
    VCouture = Standard_False;
    gp_Sphere SP = mySurface->Sphere();
    if ( myCurve->GetType() == GeomAbs_Circle) {
      UCouture = Standard_True;

      // on cherche a savoir le nombre de fois que la couture est
      // traversee.
      // si 0 ou 2 fois : la PCurve est fermee et dans l`intervalle 
      //                  [Uc-PI, Uc+PI] (Uc: U du centre du cercle)
      // si 1 fois      : la PCurve est ouverte et dans l`intervalle
      //                  [U1, U1 +/- 2*PI]

      // pour determiner le nombre de solution, on resoud le systeme
      // x^2 + y^2 + z^2     = R^2  (1)
      // A x + B y + C z + D = 0    (2)
      // x > 0                      (3)
      // y = 0                      (4)
      // REM : (1) (2)     : equation du cercle
      //       (1) (3) (4) : equation de la couture.
      Standard_Integer NbSolutions = 0;
      Standard_Real A, B, C, D, R, Tol = 1.e-10;
      Standard_Real U1, U2, V1, V2;
      gp_Trsf Trsf;
      //
      gp_Circ Circle = myCurve->Circle();
      Trsf.SetTransformation(SP.Position());
      Circle.Transform(Trsf);
      //
      R = SP.Radius();
      gp_Pln Plane( gp_Ax3(Circle.Position()));
      Plane.Coefficients(A,B,C,D);
      //
      if ( Abs(C) < Tol) {
        if ( Abs(A) > Tol) {
          if ( (D/A) < 0.) {
            if      ( ( R - Abs(D/A))  > Tol)  NbSolutions = 2;
            else if ( Abs(R - Abs(D/A))< Tol)  NbSolutions = 1;
            else                               NbSolutions = 0;
          }
        }
      }
      else {
        Standard_Real delta = R*R*(A*A+C*C) - D*D;
        delta *= C*C;
        if ( Abs(delta) < Tol*Tol) {
          if ( A*D > 0.) NbSolutions = 1;
        }
        else if ( delta > 0) {
          Standard_Real xx;
          delta = Sqrt(delta);
          xx = -A*D+delta;
          //	  
          if ( xx > Tol) NbSolutions++;
          xx = -A*D-delta;
          //	
          if ( xx > Tol) NbSolutions++;
        }
      }
      //

      // box+sphere >>
      Standard_Real UU = 0.;
      ElSLib::Parameters(SP, P1, U1, V1);
      Standard_Real eps = 10.*Epsilon(1.);
      Standard_Real dt = Max(Precision::PConfusion(), 0.01*(W2-W1)); 
      if(Abs(U1) < eps)
      {
        //May be U1 must be equal 2*PI?
        gp_Pnt Pd  = myCurve->Value(W1+dt);
        Standard_Real ud, vd;
        ElSLib::Parameters(SP, Pd, ud, vd);
        if(Abs(U1 - ud) > M_PI)
        {
          U1 = 2.*M_PI;
        }
      }
      else if(Abs(2.*M_PI - U1) < eps)
      {
        //maybe U1 = 0.?
        gp_Pnt Pd  = myCurve->Value(W1+dt);
        Standard_Real ud, vd;
        ElSLib::Parameters(SP, Pd, ud, vd);
        if(Abs(U1 - ud) > M_PI)
        {
          U1 = 0.;
        }
      }
      //
      ElSLib::Parameters(SP, P2, U2, V1);
      if(Abs(U2) < eps)
      {
        //May be U2 must be equal 2*PI?
        gp_Pnt Pd  = myCurve->Value(W2-dt);
        Standard_Real ud, vd;
        ElSLib::Parameters(SP, Pd, ud, vd);
        if(Abs(U2 - ud) > M_PI)
        {
          U2 = 2.*M_PI;
        }
      }
      else if(Abs(2.*M_PI - U2) < eps)
      {
        //maybe U2 = 0.?
        gp_Pnt Pd  = myCurve->Value(W2-dt);
        Standard_Real ud, vd;
        ElSLib::Parameters(SP, Pd, ud, vd);
        if(Abs(U2 - ud) > M_PI)
        {
          U2 = 0.;
        }
      }
      //
      ElSLib::Parameters(SP, P, UU, V1);
      //+This fragment was the reason of bug # 26008.
      //+It has been deleted on April, 03 2015.
      //Standard_Real UUmi = Min(Min(U1,UU),Min(UU,U2));
      //Standard_Real UUma = Max(Max(U1,UU),Max(UU,U2));
      //Standard_Boolean reCalc = ((UUmi >= 0. && UUmi <= M_PI) && (UUma >= 0. && UUma <= M_PI));
      // box+sphere <<
      P2 = myCurve->Value(W1+M_PI/8);
      ElSLib::Parameters(SP,P2,U2,V2);
      //
      if ( NbSolutions == 1) {
        if ( Abs(U1-U2) > M_PI) { // on traverse la couture
          if ( U1 > M_PI) {
            myU1 = U1;
            myU2 = U1+2*M_PI;
          }
          else {
            myU2 = U1;
            myU1 = U1-2*M_PI;
          }
        }
        else { // on ne traverse pas la couture
          if ( U1 > U2) {
            myU2 = U1;
            myU1 = U1-2*M_PI;
          }
          else {
            myU1 = U1;
            myU2 = U1+2*M_PI;
          }
        }
      }
      else { // 0 ou 2 solutions
        gp_Pnt Center = Circle.Location();
        Standard_Real U,V;
        ElSLib::SphereParameters(gp_Ax3(gp::XOY()),1,Center, U, V);
        myU1 = U-M_PI;
        myU2 = U+M_PI;
      }
      //
      // eval the VCouture.
      if ( (C==0) || Abs(Abs(D/C)-R) > 1.e-10) {
        VCouture = Standard_False;
      }
      else {
        VCouture = Standard_True;
        UCouture = Standard_True;

        if ( D/C < 0.) {
          myV1 =   - M_PI / 2.;
          myV2 = 3 * M_PI / 2.;
        }
        else {
          myV1 = -3 * M_PI / 2.;
          myV2 =      M_PI / 2.;
        }

        // si P1.Z() vaut +/- R on est sur le sommet : pas significatif.
        gp_Pnt pp = P1.Transformed(Trsf);

        if ( Abs( Abs(pp.Z()) - R) < Tol) {
          gp_Pnt Center = Circle.Location();
          Standard_Real U,V;
          ElSLib::SphereParameters(gp_Ax3(gp::XOY()),1,Center, U, V);
          myU1 = U-M_PI;
          myU2 = U+M_PI;
          VCouture = Standard_False;
        }
      }

      // box+sphere >>
      myV1 = -1.e+100; myV2 = 1.e+100;
      
      //+This fragment was the reason of bug # 26008.
      //+It has been deleted on April, 03 2015.
      //Standard_Real UU1 = myU1, UU2 = myU2;
      //if((Abs(UU1) <= (2.*M_PI) && Abs(UU2) <= (2.*M_PI)) && NbSolutions == 1 && reCalc) {
      //  gp_Pnt Center = Circle.Location();
      //  Standard_Real U,V;
      //  ElSLib::SphereParameters(gp_Ax3(gp::XOY()),1,Center, U, V);
      //  myU1 = U-M_PI;
      //  myU1 = Min(UU1,myU1);
      //  myU2 = myU1 + 2.*M_PI;
      //}
      // box+sphere <<

    }//if ( myCurve->GetType() == GeomAbs_Circle)

    else {
      Standard_Real U1, V1, U , V;
      ElSLib::Parameters( SP, P1, U1, V1);
      Standard_Real R = SP.Radius();
      Standard_Real Delta = 0., Step;
      Standard_Real eps = M_PI, dmax = 0., d = 0.;
      Step = Function_ComputeStep(myCurve, R);
      myU1 = U1; myU2 = U1;
      Standard_Real pmin = W1, pmax = W1, plim = W2+.1*Step;
      for(Standard_Real par = W1 + Step; par <= plim; par += Step) {
        P = myCurve->Value(par);
        ElSLib::Parameters( SP, P, U, V);
        U += Delta;
        d = U - U1;
        if(d > eps) {
          U -= Delta;
          Delta -= 2*M_PI;
          U += Delta;
          d = U - U1;
        }
        else if(d < -eps) {
          U -= Delta;
          Delta += 2*M_PI;
          U += Delta;
          d = U - U1;
        }
        dmax = Max(dmax, Abs(d));
        if(U < myU1) {myU1 = U; pmin = par;}
        if(U > myU2) {myU2 = U; pmax = par;}
        U1 = U;
      }

      if(!(Abs(pmin - W1) <= Precision::PConfusion() ||
        Abs(pmin - W2) <= Precision::PConfusion())  ) myU1 -= dmax*.5;
      if(!(Abs(pmax - W1) <= Precision::PConfusion() ||
        Abs(pmax - W2) <= Precision::PConfusion())  ) myU2 += dmax*.5;

      if((myU1 >=0. && myU1 <= 2*M_PI) &&
        (myU2 >=0. && myU2 <= 2*M_PI)    ) {
          myU1 = 0.;
          myU2 = 2.*M_PI;
          UCouture = Standard_False;
      }
      else {
        U = ( myU1 + myU2 ) /2.;
        myU1 = U - M_PI;
        myU2 = U + M_PI;
        UCouture = Standard_True;
      }

      VCouture = Standard_False;
    }
  }
  break;
  //     
  case GeomAbs_Torus:{
    gp_Torus TR = mySurface->Torus();
    Standard_Real U1, V1, U , V, dU, dV;
    ElSLib::Parameters( TR, P1, U1, V1);
    Standard_Real R = TR.MinorRadius();
    Standard_Real DeltaU = 0., DeltaV = 0., Step;
    Standard_Real eps = M_PI, dmaxU = 0., dmaxV = 0.;
    Step = Function_ComputeStep(myCurve, R);
    myU1 = U1; myU2 = U1;
    myV1 = V1; myV2 = V1;
    Standard_Real pminU = W1, pmaxU = W1, pminV = W1, pmaxV = W1,
      plim = W2+.1*Step;
    for(Standard_Real par = W1 + Step; par <= plim; par += Step) {
      P = myCurve->Value(par);
      ElSLib::Parameters( TR, P, U, V);
      U += DeltaU;
      V += DeltaV;
      dU = U - U1;
      dV = V - V1;
      if(dU > eps) {
        U -= DeltaU;
        DeltaU -= 2*M_PI;
        U += DeltaU;
        dU = U - U1;
      }
      else if(dU < -eps) {
        U -= DeltaU;
        DeltaU += 2*M_PI;
        U += DeltaU;
        dU = U - U1;
      }
      if(dV > eps) {
        V -= DeltaV;
        DeltaV -= 2*M_PI;
        V += DeltaV;
        dV = V - V1;
      }
      else if(dV < -eps) {
        V -= DeltaV;
        DeltaV += 2*M_PI;
        V += DeltaV;
        dV = V - V1;
      }
      dmaxU = Max(dmaxU, Abs(dU));
      dmaxV = Max(dmaxV, Abs(dV));
      if(U < myU1) {myU1 = U; pminU = par;}
      if(U > myU2) {myU2 = U; pmaxU = par;}
      if(V < myV1) {myV1 = V; pminV = par;}
      if(V > myV2) {myV2 = V; pmaxV = par;}
      U1 = U;
      V1 = V;
    }

    if(!(Abs(pminU - W1) <= Precision::PConfusion() ||
      Abs(pminU - W2) <= Precision::PConfusion())  ) myU1 -= dmaxU*.5;
    if(!(Abs(pmaxU - W1) <= Precision::PConfusion() ||
      Abs(pmaxU - W2) <= Precision::PConfusion())  ) myU2 += dmaxU*.5;
    if(!(Abs(pminV - W1) <= Precision::PConfusion() ||
      Abs(pminV - W2) <= Precision::PConfusion())  ) myV1 -= dmaxV*.5;
    if(!(Abs(pmaxV - W1) <= Precision::PConfusion() ||
      Abs(pmaxV - W2) <= Precision::PConfusion())  ) myV2 += dmaxV*.5;

    if((myU1 >=0. && myU1 <= 2*M_PI) &&
      (myU2 >=0. && myU2 <= 2*M_PI)    ) {
        myU1 = 0.;
        myU2 = 2.*M_PI;
        UCouture = Standard_False;
    }
    else {
      U = ( myU1 + myU2 ) /2.;
      myU1 = U - M_PI;
      myU2 = U + M_PI;
      UCouture = Standard_True;
    }
    if((myV1 >=0. && myV1 <= 2*M_PI) &&
      (myV2 >=0. && myV2 <= 2*M_PI)    ) {
        VCouture = Standard_False;
    }
    else {
      V = ( myV1 + myV2 ) /2.;
      myV1 = V - M_PI;
      myV2 = V + M_PI;
      VCouture = Standard_True;
    }

  }
  break;

  default:
    {
      UCouture = Standard_False;
      VCouture = Standard_False;
    }
    break;
  }
}
//
//
//=======================================================================
//classn : ProjLib_Function
//purpose  : 
//=======================================================================
class ProjLib_Function : public AppCont_Function
{
  Handle(Adaptor3d_HCurve)   myCurve;
  Handle(Adaptor3d_HSurface) mySurface;
  Standard_Boolean myIsPeriodic[2];
  Standard_Real myPeriod[2];
  public :

  Standard_Real    myU1,myU2,myV1,myV2;
  Standard_Boolean UCouture,VCouture;

  ProjLib_Function(const Handle(Adaptor3d_HCurve)&   C, 
                   const Handle(Adaptor3d_HSurface)& S)
: myCurve(C),
  mySurface(S),
  myU1(0.0),
  myU2(0.0),
  myV1(0.0),
  myV2(0.0),
  UCouture(Standard_False),
  VCouture(Standard_False)
  {
    myNbPnt = 0;
    myNbPnt2d = 1;
    Function_SetUVBounds(myU1,myU2,myV1,myV2,UCouture,VCouture,myCurve,mySurface);
    myIsPeriodic[0] = mySurface->IsUPeriodic();
    myIsPeriodic[1] = mySurface->IsVPeriodic();

    if (myIsPeriodic[0])
      myPeriod[0] = mySurface->UPeriod();
    else
      myPeriod[0] = 0.0;

    if (myIsPeriodic[1])
      myPeriod[1] = mySurface->VPeriod();
    else
      myPeriod[1] = 0.0;
  }

  void PeriodInformation(const Standard_Integer theDimIdx,
                         Standard_Boolean& IsPeriodic,
                         Standard_Real& thePeriod) const
  {
    IsPeriodic = myIsPeriodic[theDimIdx - 1];
    thePeriod = myPeriod[theDimIdx - 1];
  }

  Standard_Real FirstParameter() const
  {
    return (myCurve->FirstParameter());
  }

  Standard_Real LastParameter() const
  {
    return (myCurve->LastParameter());
  }

  Standard_Boolean Value(const Standard_Real   theT,
                         NCollection_Array1<gp_Pnt2d>& thePnt2d,
                         NCollection_Array1<gp_Pnt>&   /*thePnt*/) const
  {
    thePnt2d(1) = Function_Value(theT, myCurve, mySurface, myU1, myU2, myV1, myV2, UCouture, VCouture);
    return Standard_True;
  }

  gp_Pnt2d Value(const Standard_Real   theT) const
  {
    return Function_Value(theT, myCurve, mySurface, myU1, myU2, myV1, myV2, UCouture, VCouture);
  }

  Standard_Boolean D1(const Standard_Real   theT,
                      NCollection_Array1<gp_Vec2d>& theVec2d,
                      NCollection_Array1<gp_Vec>&   /*theVec*/) const
  {
    gp_Pnt2d aPnt2d;
    gp_Vec2d aVec2d;
    Standard_Boolean isOk = Function_D1(theT, aPnt2d,aVec2d, myCurve, mySurface, myU1, myU2, myV1, myV2, UCouture, VCouture);
    theVec2d(1) = aVec2d;
    return isOk;
  }
};

//=======================================================================
//function : ComputeTolU
//purpose  : 
//=======================================================================

static Standard_Real ComputeTolU(const Handle(Adaptor3d_HSurface)& theSurf,
                                 const Standard_Real theTolerance)
{
  Standard_Real aTolU = theSurf->UResolution(theTolerance);
  if (theSurf->IsUPeriodic())
  {
    aTolU = Min(aTolU, 0.01*theSurf->UPeriod());
  }

  return aTolU;
}

//=======================================================================
//function : ComputeTolV
//purpose  : 
//=======================================================================

static Standard_Real ComputeTolV(const Handle(Adaptor3d_HSurface)& theSurf,
                                 const Standard_Real theTolerance)
{
  Standard_Real aTolV = theSurf->VResolution(theTolerance);
  if (theSurf->IsVPeriodic())
  {
    aTolV = Min(aTolV, 0.01*theSurf->VPeriod());
  }

  return aTolV;
}

//=======================================================================
//function : ProjLib_ComputeApprox
//purpose  : 
//=======================================================================

ProjLib_ComputeApprox::ProjLib_ComputeApprox
  (const Handle(Adaptor3d_HCurve)   & C,
   const Handle(Adaptor3d_HSurface) & S,
   const Standard_Real              Tol )
{
  // if the surface is a plane and the curve a BSpline or a BezierCurve,
  // don`t make an Approx but only the projection of the poles.

  myTolerance = Max(Precision::PApproximation(),Tol);
  Standard_Integer NbKnots, NbPoles ;
  GeomAbs_CurveType   CType = C->GetType();
  GeomAbs_SurfaceType SType = S->GetType();

  Standard_Boolean SurfIsAnal = (SType != GeomAbs_BSplineSurface) &&
                                (SType != GeomAbs_BezierSurface)  &&
                                (SType != GeomAbs_OtherSurface)     ;

  Standard_Boolean CurvIsAnal = (CType != GeomAbs_BSplineCurve) &&
                                (CType != GeomAbs_BezierCurve)  &&
                                (CType != GeomAbs_OffsetCurve)  &&
                                (CType != GeomAbs_OtherCurve)     ;

  Standard_Boolean simplecase = SurfIsAnal && CurvIsAnal;

  if (CType == GeomAbs_BSplineCurve &&
      SType == GeomAbs_Plane ) {
    
    // get the poles and eventually the weights
    Handle(Geom_BSplineCurve) BS = C->BSpline();
    NbPoles = BS->NbPoles();
    TColgp_Array1OfPnt P3d( 1, NbPoles);
    TColgp_Array1OfPnt2d Poles( 1, NbPoles);
    TColStd_Array1OfReal Weights( 1, NbPoles);
    if ( BS->IsRational()) BS->Weights(Weights);
    BS->Poles( P3d);
    gp_Pln Plane = S->Plane();
    Standard_Real U,V;
    for ( Standard_Integer i = 1; i <= NbPoles; i++) {
      ElSLib::Parameters( Plane, P3d(i), U, V);
      Poles.SetValue(i,gp_Pnt2d(U,V));
    }
    NbKnots = BS->NbKnots();
    TColStd_Array1OfReal     Knots(1,NbKnots);
    TColStd_Array1OfInteger  Mults(1,NbKnots);
    BS->Knots(Knots) ;
    BS->Multiplicities(Mults) ; 
    // get the knots and mults if BSplineCurve
    if ( BS->IsRational()) {
      myBSpline = new Geom2d_BSplineCurve(Poles,
					  Weights,
					  Knots,
					  Mults,
					  BS->Degree(),
					  BS->IsPeriodic());
    }
    else {
      myBSpline = new Geom2d_BSplineCurve(Poles,
					  Knots,
					  Mults,
					  BS->Degree(),
					  BS->IsPeriodic());
    }
  }
  else if (CType == GeomAbs_BezierCurve &&
	   SType == GeomAbs_Plane ) {
    
    // get the poles and eventually the weights
    Handle(Geom_BezierCurve) BezierCurvePtr = C->Bezier() ;
    NbPoles = BezierCurvePtr->NbPoles();
    TColgp_Array1OfPnt P3d( 1, NbPoles);
    TColgp_Array1OfPnt2d  Poles( 1, NbPoles);
    TColStd_Array1OfReal   Weights( 1, NbPoles);
    if ( BezierCurvePtr->IsRational()) {
      BezierCurvePtr->Weights(Weights);
    }
    BezierCurvePtr->Poles( P3d);  
    
    // project the 3D-Poles on the plane
    
    gp_Pln Plane = S->Plane();
    Standard_Real U,V;
    for ( Standard_Integer i = 1; i <= NbPoles; i++) {
      ElSLib::Parameters( Plane, P3d(i), U, V);
      Poles.SetValue(i,gp_Pnt2d(U,V));
    }
    if (  BezierCurvePtr->IsRational()) {
      myBezier =  new Geom2d_BezierCurve(Poles, Weights);
    }
    else {
      myBezier =  new Geom2d_BezierCurve(Poles);
    }
  }
  else {
    ProjLib_Function F( C, S);

#ifdef OCCT_DEBUG
    //if ( AffichValue) {
    //  Standard_Integer Nb = 20;
    //  Standard_Real U1, U2, dU, U;
    //  U1 = F.FirstParameter();
    //  U2 = F.LastParameter();
    //  dU = ( U2 - U1) / Nb;
    //  TColStd_Array1OfInteger Mults(1,Nb+1);
    //  TColStd_Array1OfReal    Knots(1,Nb+1);
    //  TColgp_Array1OfPnt2d    Poles(1,Nb+1);
    //  for ( Standard_Integer i = 1; i <= Nb+1; i++) {
    //   U = U1 + (i-1)*dU;
    //   Poles(i) = F.Value(U);
    //   cout << "i = " << i << ": U = " << U << 
    //      ", p(" << Poles(i).X() << ", " << Poles(i).Y() << ");" << endl;
    //   Knots(i) = i;
    //   Mults(i) = 1;
    //  }
    //  Mults(1)    = 2;
    //  Mults(Nb+1) = 2;

    //2D-curve for showing in DRAW
    //  Handle(Geom2d_Curve) aCC = new Geom2d_BSplineCurve(Poles,Knots,Mults,1);
    //  AffichValue = Standard_False;
    //}
#endif    

//-----------
    Standard_Integer Deg1, Deg2;
    if(simplecase) {
      Deg1 = 8; 
      Deg2 = 10; 
    }
    else {
      Deg1 = 8; 
      Deg2 = 12; 
    }
//-------------
    const Standard_Real aTolU = ComputeTolU(S, myTolerance);
    const Standard_Real aTolV = ComputeTolV(S, myTolerance);
    const Standard_Real aTol2d = Max(Sqrt(aTolU*aTolU + aTolV*aTolV), Precision::PConfusion());

    Approx_FitAndDivide2d Fit(F, Deg1, Deg2, myTolerance, aTol2d, Standard_True);

    Standard_Real aNewTol2d = 0;
    if(Fit.IsAllApproximated()) {
      Standard_Integer i;
      Standard_Integer NbCurves = Fit.NbMultiCurves();
    
    // on essaie de rendre la courbe au moins C1
      Convert_CompBezierCurves2dToBSplineCurve2d Conv;

      Standard_Real Tol3d,Tol2d;
      for (i = 1; i <= NbCurves; i++) {
	      Fit.Error(i,Tol3d, Tol2d);
              aNewTol2d = Max(aNewTol2d, Tol2d);
	      AppParCurves_MultiCurve MC = Fit.Value( i);       //Charge la Ieme Curve
	      TColgp_Array1OfPnt2d Poles2d( 1, MC.Degree() + 1);//Recupere les poles
	      MC.Curve(1, Poles2d);
            
	      Conv.AddCurve(Poles2d);
      }

      //mise a jour des fields de ProjLib_Approx
      Conv.Perform();
      NbPoles    = Conv.NbPoles();
      NbKnots    = Conv.NbKnots();

      if(NbPoles <= 0 || NbPoles > 100000)
        return;
      if(NbKnots <= 0 || NbKnots > 100000)
        return;

      TColgp_Array1OfPnt2d    NewPoles(1,NbPoles);
      TColStd_Array1OfReal    NewKnots(1,NbKnots);
      TColStd_Array1OfInteger NewMults(1,NbKnots);

      Conv.KnotsAndMults(NewKnots,NewMults);
      Conv.Poles(NewPoles);

      BSplCLib::Reparametrize(C->FirstParameter(),
                              C->LastParameter(),
                              NewKnots);

      /*cout << endl;
      for (int i = 1; i <= NbPoles; i++)
      {
        cout << NewPoles.Value(i).X() << " " << NewPoles.Value(i).Y() << endl;
      }
      cout << endl; */

      // il faut recadrer les poles de debut et de fin:
      // ( Car pour les problemes de couture, on a du ouvrir l`intervalle
      // de definition de la courbe.)
      // On choisit de calculer ces poles par prolongement de la courbe
      // approximee.
      myBSpline = new Geom2d_BSplineCurve (NewPoles,
                                           NewKnots,
                                           NewMults,
                                           Conv.Degree());
    }
    else {
      Standard_Integer NbCurves = Fit.NbMultiCurves();
      if(NbCurves != 0) {
	      Standard_Real Tol3d,Tol2d;
	      Fit.Error(NbCurves,Tol3d, Tol2d);
              aNewTol2d = Tol2d;
      }
    }

    // restore tolerance 3d from 2d

    //Here we consider that 
    //   aTolU(new)/aTolV(new) = aTolU(old)/aTolV(old)
    //(it is assumption indeed).
    //Then,
    //  Tol3D(new)/Tol3D(old) = Tol2D(new)/Tol2D(old).
    myTolerance *= (aNewTol2d / aTol2d);

    //Return curve home
    Standard_Real UFirst = F.FirstParameter();
    gp_Pnt P3d = C->Value( UFirst );
    Standard_Real u = 0., v = 0.;
    switch (SType)
    {
    case GeomAbs_Plane:
      {
        gp_Pln Plane = S->Plane();
        ElSLib::Parameters( Plane, P3d, u, v );
        break;
      }
    case GeomAbs_Cylinder:
      {
        gp_Cylinder Cylinder = S->Cylinder();
        ElSLib::Parameters( Cylinder, P3d, u, v );
        break;
      }
    case GeomAbs_Cone:
      {
        gp_Cone Cone = S->Cone();
        ElSLib::Parameters( Cone, P3d, u, v );
        break;
      }
    case GeomAbs_Sphere:
      {
        gp_Sphere Sphere = S->Sphere();
        ElSLib::Parameters( Sphere, P3d, u, v );
        break;
      }
    case GeomAbs_Torus:
      {
        gp_Torus Torus = S->Torus();
        ElSLib::Parameters( Torus, P3d, u, v );
        break;
      }
    default:
      throw Standard_NoSuchObject("ProjLib_ComputeApprox::Value");
    }
    Standard_Boolean ToMirror = Standard_False;
    Standard_Real du = 0., dv = 0.;
    Standard_Integer number;
    if (F.VCouture)
    { 
      if (SType == GeomAbs_Sphere && Abs(u-F.myU1) > M_PI)
      {
        ToMirror = Standard_True;
        dv = -M_PI;
        v = M_PI - v;
      }
      Standard_Real newV = ElCLib::InPeriod( v, F.myV1, F.myV2 );
      number = (Standard_Integer) (Floor((newV-v)/(F.myV2-F.myV1)));
      dv -= number*(F.myV2-F.myV1);
    }
    if (F.UCouture || (F.VCouture && SType == GeomAbs_Sphere))
    {
      Standard_Real aNbPer;
      gp_Pnt2d P2d = F.Value(UFirst);
      du = u - P2d.X();
      du = (du < 0) ? (du - Precision::PConfusion()) : 
        (du + Precision::PConfusion());
      modf(du/M_PI, &aNbPer);
      number = (Standard_Integer)aNbPer;
      du = number*M_PI;
    }

    if (!myBSpline.IsNull())
    {
      if (du != 0. || dv != 0.)
        myBSpline->Translate( gp_Vec2d(du,dv) );
      if (ToMirror)
      {
        gp_Ax2d Axe( gp_Pnt2d(0.,0.), gp_Dir2d(1.,0.) );
        myBSpline->Mirror( Axe );
      }
    }
  }
}

//=======================================================================
//function : BSpline
//purpose  : 
//=======================================================================

Handle(Geom2d_BSplineCurve)  ProjLib_ComputeApprox::BSpline() const 

{
  return myBSpline ;
}

//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================

Handle(Geom2d_BezierCurve)  ProjLib_ComputeApprox::Bezier()  const 

{
  return myBezier ;
}


//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

Standard_Real ProjLib_ComputeApprox::Tolerance() const 
{
  return myTolerance;
}


