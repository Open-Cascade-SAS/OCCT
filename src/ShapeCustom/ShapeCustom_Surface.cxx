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

//abv 06.01.99 fix of misprint
//:p6 abv 26.02.99: make ConvertToPeriodic() return Null if nothing done

#include <ElSLib.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <ShapeAnalysis_Geom.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeCustom_Surface.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>

//=======================================================================
//function : ShapeCustom_Surface
//purpose  : 
//=======================================================================
ShapeCustom_Surface::ShapeCustom_Surface() : myGap (0)
{
}

//=======================================================================
//function : ShapeCustom_Surface
//purpose  : 
//=======================================================================

ShapeCustom_Surface::ShapeCustom_Surface (const Handle(Geom_Surface)& S)
     : myGap (0)
{
  Init ( S );
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void ShapeCustom_Surface::Init (const Handle(Geom_Surface)& S) 
{
  mySurf = S;
}

//=======================================================================
//function : ConvertToAnalytical
//purpose  : 
//=======================================================================

Handle(Geom_Surface) ShapeCustom_Surface::ConvertToAnalytical (const Standard_Real tol,
							       const Standard_Boolean substitute) 
{
  Handle(Geom_Surface) newSurf;

  Standard_Integer nUP, nVP, nCP, i, j , UDeg, VDeg;
  Standard_Real U1, U2, V1, V2, C1, C2, DU, DV, U=0, V=0;
  Handle(Geom_Curve) iso;
  Standard_Boolean uClosed = Standard_True;

  // seuls cas traites : BSpline et Bezier
  Handle(Geom_BSplineSurface) theBSplneS = 
    Handle(Geom_BSplineSurface)::DownCast(mySurf);
  if (theBSplneS.IsNull()) {
    Handle(Geom_BezierSurface) theBezierS = 
      Handle(Geom_BezierSurface)::DownCast(mySurf);
    if (!theBezierS.IsNull()) {    // Bezier :
      nUP  = theBezierS->NbUPoles();   
      nVP  = theBezierS->NbVPoles();   
      UDeg = theBezierS->UDegree();
      VDeg = theBezierS->VDegree();
    }
    else return newSurf;           // non reconnu : terminus
  }
  else {                           // BSpline :
    nUP  = theBSplneS->NbUPoles();
    nVP  = theBSplneS->NbVPoles();
    UDeg = theBSplneS->UDegree();
    VDeg = theBSplneS->VDegree();
  }


  mySurf->Bounds(U1, U2, V1, V2);
//  mySurf->Bounds(U1, U2, V1, V2);
  TColgp_Array1OfPnt p1(1, 3), p2(1, 3), p3(1, 3);
  TColStd_Array1OfReal R(1,3);
  gp_Pnt origPnt, resPnt;  
  gp_Vec origD1U, resD1U, resD1V;
    
  Standard_Boolean aCySpCo = Standard_False;
  Standard_Boolean aToroid = Standard_False;
  Standard_Boolean aPlanar = Standard_False;

  if (nUP == 2 && nVP == 2) {
    if (UDeg == 1 && VDeg == 1) aPlanar = Standard_True;
  } else if (mySurf->IsUClosed()) {    // VRAI IsUClosed
    if (mySurf->IsVClosed())   aToroid = Standard_True;
    else                        aCySpCo = Standard_True;
  } else {
    if(mySurf->IsVClosed()) {    // VRAI IsVClosed
      aCySpCo = Standard_True;
      uClosed = Standard_False;
    }
  }

  if (aPlanar) {
//    NearestPlane ...
    TColgp_Array1OfPnt Pnts(1,4);
    Pnts.SetValue(1,mySurf->Value(U1,V1));
    Pnts.SetValue(2,mySurf->Value(U2,V1));
    Pnts.SetValue(3,mySurf->Value(U1,V2));
    Pnts.SetValue(4,mySurf->Value(U2,V2));
    gp_Pln aPln;// Standard_Real Dmax;
    Standard_Integer It = ShapeAnalysis_Geom::NearestPlane (Pnts,aPln,myGap/*Dmax*/);

//  ICI, on fabrique le plan, et zou
    if (It == 0 || myGap/*Dmax*/ > tol) return newSurf;    //  pas un plan

//    IL RESTE a verifier l orientation ...
//    On regarde sur chaque surface les vecteurs P(U0->U1),P(V0->V1)
//    On prend la normale : les deux normales doivent etre dans le meme sens
//    Sinon, inverser la normale (pas le Pln entier !) et refaire la Plane
    newSurf = new Geom_Plane (aPln);
    gp_Vec uold (Pnts(1),Pnts(2));
    gp_Vec vold (Pnts(1),Pnts(3));
    gp_Vec nold = uold.Crossed (vold);
    gp_Vec unew (newSurf->Value(U1,V1), newSurf->Value(U2,V1));
    gp_Vec vnew (newSurf->Value(U1,V1), newSurf->Value(U1,V2));
    gp_Vec nnew = unew.Crossed (vnew);
    if (nold.Dot (nnew) < 0.0) {
      gp_Ax3 ax3 = aPln.Position();
      ax3.ZReverse();
      ax3.XReverse();
      aPln = gp_Pln (ax3);
      newSurf = new Geom_Plane (aPln);
    }

    if (substitute) {
      Init (newSurf);
    }
    return newSurf;


  } else if (aCySpCo) {
    if (!uClosed) {
      C1 = U1; C2 = U2;
      U1 = V1; U2 = V2;
      V1 = C1; V2 = C2;
      nCP = nUP; nUP = nVP; nVP = nCP;
    }
  
    for (i=1; i<=3; i++) {
      if      (i==1) V = V1;
      else if (i==2) V = V2;
      else if (i==3) V = 0.5*(V1+V2);

      if(uClosed) iso = mySurf->VIso(V);
      else        iso = mySurf->UIso(V);

      iso->D0(U1, p1(i)); 
      iso->D0(0.5*(U1+U2), p2(i));
      p3(i).SetCoord(0.5*(p1(i).X()+p2(i).X()), 
		     0.5*(p1(i).Y()+p2(i).Y()), 
		     0.5*(p1(i).Z()+p2(i).Z()));
      R(i) = p3(i).Distance(p1(i));
//	cout<<"sphere, i="<<i<<" V="<<V<<" R="<<R(i)<<" p1="<<p1(i).X()<<","<<p1(i).Y()<<","<<p1(i).Z()<<" p2="<<p2(i).X()<<","<<p2(i).Y()<<","<<p2(i).Z()<<" p3="<<p3(i).X()<<","<<p3(i).Y()<<","<<p3(i).Z()<<endl;
    }
      
    iso->D1 (0.,origPnt,origD1U);
    gp_Vec xVec(p3(3), p1(3));
    gp_Vec aVec(p3(1), p3(2));
//      gp_Dir xDir(xVec);  ne sert pas. Null si R3 = 0
    gp_Dir aDir(aVec);
    gp_Ax3 aAx3 (p3(1),aDir,xVec);
    //  CKY  3-FEV-1997 : verification du sens de description
    //gp_Dir AXY = aAx3.YDirection(); // AXY not used (skl)
    if (aAx3.YDirection().Dot (origD1U) < 0) {
#ifdef OCCT_DEBUG
      cout<<" Surface Analytique : sens a inverser"<<endl;
#endif
      aAx3.YReverse();    //  mais X reste !
    }
      
    if (nVP > 2) {
      if ((Abs(R(1)) < tol) && 
	  (Abs(R(2)) < tol) && 
	  (Abs(R(3)) > tol)) {
// deja fait	  gp_Ax3 aAx3(p3(1), aDir, xVec);
          //gp_Ax3 aAx3(p3(3), aDir);
	Handle(Geom_SphericalSurface) anObject = 
	  new Geom_SphericalSurface(aAx3, R(3));
	if (!uClosed) anObject->UReverse();
	newSurf = anObject;         
      }
    }
    else if (nVP == 2) {

// deja fait	gp_Ax3 aAx3(p3(1), aDir, xVec); 
        //gp_Ax3 aAx3(p3(1), aDir);

      if (Abs(R(2)-R(1)) < tol) {
	Handle(Geom_CylindricalSurface) anObject = 
	  new Geom_CylindricalSurface(aAx3, R(1));
	if (!uClosed) anObject->UReverse();
	newSurf = anObject;
      }
      else {
	gp_Vec aVec2(p1(1), p1(2));
	Standard_Real angle = aVec.Angle(aVec2);
	if (R(1) < R(2)) {
	  Handle(Geom_ConicalSurface) anObject = 
	    new Geom_ConicalSurface(aAx3, angle, R(1));
	  //if (!uClosed) anObject->UReverse();
	  anObject->UReverse();
	  newSurf = anObject;
	}
	else {
	  aDir.Reverse();
	  gp_Vec anotherXVec(p3(2), p1(2));
	  gp_Dir anotherXDir(anotherXVec);
	  gp_Ax3 anotherAx3(p3(2), aDir, anotherXDir);
	  Handle(Geom_ConicalSurface) anObject = 
	    new Geom_ConicalSurface(anotherAx3, angle, R(2));
	  //if (!uClosed) anObject->UReverse();
	  anObject->UReverse();
	  newSurf = anObject;
	}
      }
    }
  }
  else if (aToroid) {
    // test by iso U and isoV
    Standard_Boolean  isFound = Standard_False;
    for (j=1; (j<=2) && !isFound; j++) {
      if (j==2) {
	C1 = U1; C2 = U2;
	U1 = V1; U2 = V2;
	V1 = C1; V2 = C2;
      }
      for (i=1; i<=3; i++) {
	if      (i==1) U = U1;
	else if (i==2) U = 0.5*(U1+U2);
	else if (i==3) U = 0.25*(U1+U2);

	iso = mySurf->UIso(U);

	iso->D0(V1, p1(i)); 
	iso->D0(0.5*(V1+V2), p2(i));
	p3(i).SetCoord(0.5*(p1(i).X()+p2(i).X()), 
		       0.5*(p1(i).Y()+p2(i).Y()), 
		       0.5*(p1(i).Z()+p2(i).Z()));
	R(i) = p3(i).Distance(p1(i));
      }
      if ((Abs(R(1)-R(2))< tol) && 
	  (Abs(R(1)-R(3))< tol)) {
	gp_Pnt p10(0.5*(p3(1).X()+p3(2).X()), 
		   0.5*(p3(1).Y()+p3(2).Y()), 
		   0.5*(p3(1).Z()+p3(2).Z()));
	gp_Vec aVec(p10, p3(1));
	gp_Vec aVec2(p10, p3(3));
	Standard_Real RR1 = R(1), RR2 = R(2), RR3;
	aVec ^= aVec2;

	if (aVec.Magnitude() <= gp::Resolution()) aVec.SetCoord(0., 0., 1.);

	gp_Dir aDir(aVec);

	gp_Ax3 aAx3(p10, aDir);
	RR1 = p10.Distance(p3(1));
//          modif empirique (pourtant NON DEMONTREE) : inverser roles RR1,RR2
//          CKY, 24-JAN-1997
	if (RR1 < RR2)  {  RR3 = RR1; RR1 = RR2; RR2 = RR3;  }
	Handle(Geom_ToroidalSurface) anObject = 
	  new Geom_ToroidalSurface(aAx3, RR1, RR2);
	if (j==2) anObject->UReverse();
	anObject->D1 (0.,0.,resPnt,resD1U,resD1V);
#ifdef OCCT_DEBUG
	if (resD1U.Dot(origD1U) < 0 && j != 2)
	  cout<<" Tore a inverser !"<<endl;
#endif
	newSurf = anObject;
	isFound = Standard_True;
      }
    }
  }
  if (newSurf.IsNull()) return newSurf;
  
  //---------------------------------------------------------------------
  //                 verification
  //---------------------------------------------------------------------
  
  Handle(GeomAdaptor_HSurface) NHS = new GeomAdaptor_HSurface (newSurf);
  GeomAdaptor_Surface& SurfAdapt = NHS->ChangeSurface();

  const Standard_Integer NP = 21;
  Standard_Real S = 0., T = 0.;  // U,V deja fait
  gp_Pnt P3d, P3d2;
  Standard_Boolean onSurface = Standard_True;

  Standard_Real dis;  myGap = 0.;

  DU = (U2-U1)/(NP-1);
  DV = (V2-V1)/(NP-1);
  for (j=1; (j<=NP) && onSurface; j++) {
    V = V1 + DV*(j-1);

    if(uClosed) iso = mySurf->VIso(V);
    else        iso = mySurf->UIso(V);

    for (i=1; i<=NP; i++) {
      U = U1 + DU*(i-1);
      iso->D0(U, P3d);
      switch (SurfAdapt.GetType()){    
	  
	case GeomAbs_Cylinder :
	  {
	    gp_Cylinder Cylinder = SurfAdapt.Cylinder();
	    ElSLib::Parameters( Cylinder, P3d, S, T);
	    break;
	  }
	case GeomAbs_Cone :
	  {
	    gp_Cone Cone = SurfAdapt.Cone();
	    ElSLib::Parameters( Cone, P3d, S, T);
	    break;
	  }
	case GeomAbs_Sphere :
	  {
	    gp_Sphere Sphere = SurfAdapt.Sphere();
	    ElSLib::Parameters( Sphere, P3d, S, T);
	    break;
	  }
	case GeomAbs_Torus :
	  {
	    gp_Torus Torus = SurfAdapt.Torus();
	    ElSLib::Parameters( Torus, P3d, S, T);
	    break;
	  }
	default:
	  break;
      }

      newSurf->D0(S, T, P3d2);

      dis = P3d.Distance(P3d2);
      if (dis > myGap) myGap = dis;

      if (dis > tol) {
	onSurface = Standard_False;
	newSurf.Nullify();
          // The presumption is rejected
	break;
      }
    }
  }
  if (substitute && !NHS.IsNull()) {
    Init (newSurf);
  }
  return newSurf;
}
 
//%pdn 30 Nov 98: converting bspline surfaces with degree+1 at ends to periodic
// UKI60591, entity 48720
Handle(Geom_Surface) ShapeCustom_Surface::ConvertToPeriodic (const Standard_Boolean substitute,
                                                             const Standard_Real preci)
{
  Handle(Geom_Surface) newSurf;
  Handle(Geom_BSplineSurface) BSpl = Handle(Geom_BSplineSurface)::DownCast(mySurf);
  if (BSpl.IsNull()) return newSurf;
  
  ShapeAnalysis_Surface sas(mySurf);
  Standard_Boolean uclosed = sas.IsUClosed(preci);
  Standard_Boolean vclosed = sas.IsVClosed(preci);
  
  if ( ! uclosed && ! vclosed ) return newSurf;
  
  Standard_Boolean converted = Standard_False; //:p6

  if ( uclosed && ! BSpl->IsUPeriodic() && BSpl->NbUPoles() >3 ) {
    Standard_Boolean set = Standard_True;
    // if degree+1 at ends, first change it to 1 by rearranging knots
    if ( BSpl->UMultiplicity(1) == BSpl->UDegree() + 1 &&
         BSpl->UMultiplicity(BSpl->NbUKnots()) == BSpl->UDegree() + 1 ) {
      Standard_Integer nbUPoles = BSpl->NbUPoles();
      Standard_Integer nbVPoles = BSpl->NbVPoles();
      TColgp_Array2OfPnt oldPoles(1,nbUPoles,1,nbVPoles);
      TColStd_Array2OfReal oldWeights(1,nbUPoles,1,nbVPoles);
      Standard_Integer nbUKnots = BSpl->NbUKnots();
      Standard_Integer nbVKnots = BSpl->NbVKnots();
      TColStd_Array1OfReal oldUKnots(1,nbUKnots);
      TColStd_Array1OfReal oldVKnots(1,nbVKnots);
      TColStd_Array1OfInteger oldUMults(1,nbUKnots);
      TColStd_Array1OfInteger oldVMults(1,nbVKnots);
      
      BSpl->Poles(oldPoles);
      BSpl->Weights(oldWeights);
      BSpl->UKnots(oldUKnots);
      BSpl->VKnots(oldVKnots);
      BSpl->UMultiplicities(oldUMults);
      BSpl->VMultiplicities(oldVMults);
      
      TColStd_Array1OfReal newUKnots (1,nbUKnots+2);
      TColStd_Array1OfInteger newUMults(1,nbUKnots+2);
      Standard_Real a = 0.5 * ( BSpl->UKnot(2) - BSpl->UKnot(1) + 
			        BSpl->UKnot(nbUKnots) - BSpl->UKnot(nbUKnots-1) );
      
      newUKnots(1) = oldUKnots(1) - a;
      newUKnots(nbUKnots+2) = oldUKnots(nbUKnots) + a;
      newUMults(1) = newUMults(nbUKnots+2) = 1;
      for (Standard_Integer i = 2; i<=nbUKnots+1; i++) {
	newUKnots(i) = oldUKnots(i-1);
	newUMults(i) = oldUMults(i-1);
      }
      newUMults(2) = newUMults(nbUKnots+1) = BSpl->UDegree();
      Handle(Geom_BSplineSurface) res = new Geom_BSplineSurface(oldPoles,
								oldWeights,
								newUKnots,oldVKnots,
								newUMults,oldVMults,
								BSpl->UDegree(),BSpl->VDegree(),
								BSpl->IsUPeriodic(),BSpl->IsVPeriodic());
      BSpl = res;
    }
    else if ( BSpl->UMultiplicity(1) > BSpl->UDegree() ||
	      BSpl->UMultiplicity(BSpl->NbUKnots()) > BSpl->UDegree() + 1 ) set = Standard_False;
    if ( set ) {
      BSpl->SetUPeriodic(); // make periodic
      converted = Standard_True;
    }
  }
  
  if ( vclosed && ! BSpl->IsVPeriodic() && BSpl->NbVPoles() >3 ) {	
    Standard_Boolean set = Standard_True;
    // if degree+1 at ends, first change it to 1 by rearranging knots
    if ( BSpl->VMultiplicity(1) == BSpl->VDegree() + 1 &&
	 BSpl->VMultiplicity(BSpl->NbVKnots()) == BSpl->VDegree() + 1 ) {
      Standard_Integer nbUPoles = BSpl->NbUPoles();
      Standard_Integer nbVPoles = BSpl->NbVPoles();
      TColgp_Array2OfPnt oldPoles(1,nbUPoles,1,nbVPoles);
      TColStd_Array2OfReal oldWeights(1,nbUPoles,1,nbVPoles);
      Standard_Integer nbUKnots = BSpl->NbUKnots();
      Standard_Integer nbVKnots = BSpl->NbVKnots();
      TColStd_Array1OfReal oldUKnots(1,nbUKnots);
      TColStd_Array1OfReal oldVKnots(1,nbVKnots);
      TColStd_Array1OfInteger oldUMults(1,nbUKnots);
      TColStd_Array1OfInteger oldVMults(1,nbVKnots);
      
      BSpl->Poles(oldPoles);
      BSpl->Weights(oldWeights);
      BSpl->UKnots(oldUKnots);
      BSpl->VKnots(oldVKnots);
      BSpl->UMultiplicities(oldUMults);
      BSpl->VMultiplicities(oldVMults);
      
      TColStd_Array1OfReal newVKnots (1,nbVKnots+2);
      TColStd_Array1OfInteger newVMults(1,nbVKnots+2);
      Standard_Real a = 0.5 * ( BSpl->VKnot(2) - BSpl->VKnot(1) + 
			        BSpl->VKnot(nbVKnots) - BSpl->VKnot(nbVKnots-1) );
      
      newVKnots(1) = oldVKnots(1) - a;
      newVKnots(nbVKnots+2) = oldVKnots(nbVKnots) + a;
      newVMults(1) = newVMults(nbVKnots+2) = 1;
      for (Standard_Integer i = 2; i<=nbVKnots+1; i++) {
	newVKnots(i) = oldVKnots(i-1);
	newVMults(i) = oldVMults(i-1);
      }
      newVMults(2) = newVMults(nbVKnots+1) = BSpl->VDegree();
      Handle(Geom_BSplineSurface) res = new Geom_BSplineSurface(oldPoles,
								oldWeights,
								oldUKnots,newVKnots,
								oldUMults,newVMults,
								BSpl->UDegree(),BSpl->VDegree(),
								BSpl->IsUPeriodic(),BSpl->IsVPeriodic());
      BSpl = res;
    }
    else if ( BSpl->VMultiplicity(1) > BSpl->VDegree() ||
	      BSpl->VMultiplicity(BSpl->NbVKnots()) > BSpl->VDegree() + 1 ) set = Standard_False;
    if ( set ) {
      BSpl->SetVPeriodic(); // make periodic
      converted = Standard_True;
    }
  }
  
#ifdef OCCT_DEBUG
  cout << "Warning: ShapeCustom_Surface: Closed BSplineSurface is caused to be periodic" << endl;
#endif
  if ( ! converted ) return newSurf;
  newSurf = BSpl;
  if ( substitute ) mySurf = newSurf;
  return newSurf;
}
