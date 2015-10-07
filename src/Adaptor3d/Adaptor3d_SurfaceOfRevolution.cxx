// Created on: 1993-04-21
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


#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_HSurfaceOfRevolution.hxx>
#include <Adaptor3d_SurfaceOfRevolution.hxx>
#include <ElCLib.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>

//=======================================================================
//function : Adaptor3d_SurfaceOfRevolution
//purpose  : 
//=======================================================================
Adaptor3d_SurfaceOfRevolution::Adaptor3d_SurfaceOfRevolution()
:myHaveAxis(Standard_False)
{}

//=======================================================================
//function : Adaptor3d_SurfaceOfRevolution
//purpose  : 
//=======================================================================

Adaptor3d_SurfaceOfRevolution::Adaptor3d_SurfaceOfRevolution
(const Handle(Adaptor3d_HCurve)& C)
:myHaveAxis(Standard_False)
{
  Load( C);
}

//=======================================================================
//function : Adaptor3d_SurfaceOfRevolution
//purpose  : 
//=======================================================================

Adaptor3d_SurfaceOfRevolution::Adaptor3d_SurfaceOfRevolution
(const Handle(Adaptor3d_HCurve)& C,
 const gp_Ax1&        V)
:myHaveAxis(Standard_False)
{
  Load( C);
  Load( V);
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void Adaptor3d_SurfaceOfRevolution::Load( const Handle(Adaptor3d_HCurve)& C)
{
  myBasisCurve = C;
  if ( myHaveAxis)  Load(myAxis); // to evaluate the new myAxeRev.
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void Adaptor3d_SurfaceOfRevolution::Load( const gp_Ax1& V)
{
  myHaveAxis = Standard_True;
  myAxis = V;
  
  // Eval myAxeRev : axe of revolution ( Determination de Ox).
  gp_Pnt P,Q;
  gp_Pnt O = myAxis.Location();
  gp_Dir Ox;
  gp_Dir Oz = myAxis.Direction();
  Standard_Boolean yrev = Standard_False;
  if (myBasisCurve->GetType() == GeomAbs_Line) {
    if((myBasisCurve->Line().Direction()).Dot(Oz) < 0.){
      yrev = Standard_True;
      Oz.Reverse();
    }
  }

  if (myBasisCurve->GetType() == GeomAbs_Circle) {
    Q = P = (myBasisCurve->Circle()).Location();
  }
  else {
    Standard_Real First = myBasisCurve->FirstParameter();
    P = Value( 0., 0.);// ce qui ne veut pas dire grand chose
    if ( GetType() == GeomAbs_Cone) {
      if ( gp_Lin(myAxis).Distance(P) <= Precision::Confusion())
	Q = ElCLib::Value(1.,myBasisCurve->Line());
      else 
	Q = P;
    }
    else if (Precision::IsInfinite(First))
      Q = P; 
    else 
      Q = Value( 0., First);
  }
  
  gp_Dir DZ = myAxis.Direction();
  O.SetXYZ( O.XYZ() + ( gp_Vec(O,P) * DZ) * DZ.XYZ());
  if ( gp_Lin(myAxis).Distance(Q) > Precision::Confusion()) {
    Ox = gp_Dir(Q.XYZ() - O.XYZ());
  }
  else {
    Standard_Real First = myBasisCurve->FirstParameter();
    Standard_Real Last  = myBasisCurve->LastParameter();
    Standard_Integer Ratio = 1;
    Standard_Real Dist; 
    gp_Pnt PP;
    do {
      PP = myBasisCurve->Value(First+(Last-First)/Ratio);
      Dist = gp_Lin(myAxis).Distance(PP);
      Ratio++;
    }
    while ( Dist < Precision::Confusion() && Ratio < 100);

    if ( Ratio >= 100 ) {
      Standard_ConstructionError::Raise
	("Adaptor3d_SurfaceOfRevolution : Axe and meridian are confused");
    }
    Ox = ( (Oz^gp_Vec(PP.XYZ()-O.XYZ()))^Oz); 
  }

  myAxeRev = gp_Ax3(O,Oz,Ox);

  if (yrev) {
    myAxeRev.YReverse();
  }
  else if (myBasisCurve->GetType() == GeomAbs_Circle) {
    gp_Dir DC = (myBasisCurve->Circle()).Axis().Direction();
    if ((Ox.Crossed(Oz)).Dot(DC) < 0.)  myAxeRev.ZReverse(); 
  }
}

//=======================================================================
//function : AxeOfRevolution
//purpose  : 
//=======================================================================

gp_Ax1 Adaptor3d_SurfaceOfRevolution::AxeOfRevolution() const
{
  return myAxis;
}

//=======================================================================
//function : FirstUParameter
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_SurfaceOfRevolution::FirstUParameter() const 
{
  return 0.;
}

//=======================================================================
//function : LastUParameter
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_SurfaceOfRevolution::LastUParameter() const 
{
  return 2*M_PI;
}

//=======================================================================
//function : FirstVParameter
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_SurfaceOfRevolution::FirstVParameter() const 
{
  return myBasisCurve->FirstParameter();
}

//=======================================================================
//function : LastVParameter
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_SurfaceOfRevolution::LastVParameter() const 
{
  return myBasisCurve->LastParameter();
}

//=======================================================================
//function : UContinuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Adaptor3d_SurfaceOfRevolution::UContinuity() const 
{
  return GeomAbs_CN;
}

//=======================================================================
//function : VContinuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Adaptor3d_SurfaceOfRevolution::VContinuity() const 
{
  return myBasisCurve->Continuity();
}

//=======================================================================
//function : NbUIntervals
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_SurfaceOfRevolution::NbUIntervals
//(const GeomAbs_Shape S) const
(const GeomAbs_Shape ) const
{
  return 1;
}

//=======================================================================
//function : NbVIntervals
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_SurfaceOfRevolution::NbVIntervals
( const GeomAbs_Shape S) const 
{
  return myBasisCurve->NbIntervals(S);
}

//=======================================================================
//function : UIntervals
//purpose  : 
//=======================================================================

void Adaptor3d_SurfaceOfRevolution::UIntervals (TColStd_Array1OfReal& T,
//					      const GeomAbs_Shape S) const 
					      const GeomAbs_Shape ) const 
{
  T(T.Lower()  ) = 0.;
  T(T.Lower()+1) = 2*M_PI;
}


//=======================================================================
//function : VIntervals
//purpose  : 
//=======================================================================

void Adaptor3d_SurfaceOfRevolution::VIntervals(TColStd_Array1OfReal& T,
					     const GeomAbs_Shape S) const 
{
  myBasisCurve->Intervals(T,S);
}


//=======================================================================
//function : UTrim
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HSurface) Adaptor3d_SurfaceOfRevolution::UTrim
(const Standard_Real 
#ifndef No_Exception
                     First
#endif
 ,const Standard_Real 
#ifndef No_Exception
                     Last
#endif
 ,const Standard_Real 
                         ) const 
{
#ifndef No_Exception
  Standard_Real Eps = Precision::PConfusion();
#endif
  Standard_OutOfRange_Raise_if
    (  Abs(First) > Eps || Abs(Last - 2.*M_PI) > Eps,
     "Adaptor3d_SurfaceOfRevolution : UTrim : Parameters out of range");

  Handle(Adaptor3d_HSurfaceOfRevolution) HR =
    new Adaptor3d_HSurfaceOfRevolution(*this);
  return HR;
}


//=======================================================================
//function : VTrim
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HSurface) Adaptor3d_SurfaceOfRevolution::VTrim
(const Standard_Real First,
 const Standard_Real Last,
 const Standard_Real Tol) const 
{
  Handle(Adaptor3d_HSurfaceOfRevolution) HR =
    new Adaptor3d_HSurfaceOfRevolution(*this);
  Handle(Adaptor3d_HCurve) HC = BasisCurve()->Trim(First,Last,Tol);
  HR->ChangeSurface().
Load(HC);
  return HR;
}


//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_SurfaceOfRevolution::IsUClosed() const 
{
  return Standard_True;
}

//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_SurfaceOfRevolution::IsVClosed() const 
{
  return myBasisCurve->IsClosed();
}

//=======================================================================
//function : IsUPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_SurfaceOfRevolution::IsUPeriodic() const
{
  return Standard_True;
}

//=======================================================================
//function : UPeriod
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_SurfaceOfRevolution::UPeriod() const
{
  return 2*M_PI;
}

//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_SurfaceOfRevolution::IsVPeriodic() const
{
  return myBasisCurve->IsPeriodic();
}

//=======================================================================
//function : VPeriod
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_SurfaceOfRevolution::VPeriod() const 
{
  return myBasisCurve->Period();
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt Adaptor3d_SurfaceOfRevolution::Value(const Standard_Real U, 
					  const Standard_Real V) const
{
  gp_Pnt P;
  myBasisCurve->D0(V,P);
  P.Rotate( myAxis, U);
  return P;
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Adaptor3d_SurfaceOfRevolution::D0(const Standard_Real U,
				     const Standard_Real V,
				           gp_Pnt&       P) const
{
  myBasisCurve->D0(V,P);
  P.Rotate( myAxis, U);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Adaptor3d_SurfaceOfRevolution::D1(const Standard_Real U, 
				     const Standard_Real V, 
				     gp_Pnt& P, gp_Vec& D1U,
				     gp_Vec& D1V) const 
{
  myBasisCurve->D1(V,P,D1V);
  Standard_Real R = gp_Vec(myAxeRev.Location(), P) * myAxeRev.XDirection();
  
  D0( U,V,P);
  D1V.Rotate( myAxis, U);
  D1U = R*(myAxeRev.YDirection());
  D1U.Rotate( myAxis, U);
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Adaptor3d_SurfaceOfRevolution::D2(const Standard_Real U, 
				     const Standard_Real V,
				     gp_Pnt& P, gp_Vec& D1U, 
				     gp_Vec& D1V,
				     gp_Vec& D2U, gp_Vec& D2V,
				     gp_Vec& D2UV) const
{
  myBasisCurve->D2(V,P,D1V,D2V);

  gp_Vec D1 = (myAxeRev.YDirection()).Rotated( myAxis, U);
  gp_Vec D2 = (myAxeRev.XDirection()).Rotated( myAxis, U);

  Standard_Real R   = gp_Vec(myAxeRev.Location(), P) * myAxeRev.XDirection();
  Standard_Real D1R = D1V * myAxeRev.XDirection(); // D1R = dR/dV 
                                                   // et R=AP*XDirection

  D0( U,V,P);
  D1V.Rotate( myAxis, U);
  D2V.Rotate( myAxis, U);
  D1U  =   R * D1;
  D2U  =  -R * D2;
  D2UV = D1R * D1;
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Adaptor3d_SurfaceOfRevolution::D3(const Standard_Real U, 
				     const Standard_Real V,
				     gp_Pnt& P,gp_Vec& D1U, gp_Vec& D1V,
				     gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV,
				     gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV,
				     gp_Vec& D3UVV) const
{
  myBasisCurve->D3(V,P,D1V,D2V,D3V);
  
  gp_Vec D1 = (myAxeRev.YDirection()).Rotated( myAxis, U);
  gp_Vec D2 = (myAxeRev.XDirection()).Rotated( myAxis, U);

  Standard_Real R   = gp_Vec(myAxeRev.Location(), P) * myAxeRev.XDirection();
  Standard_Real D1R = D1V * myAxeRev.XDirection();  // D1R = dR/dV et
                                                    // R=AP*XDirection
  Standard_Real D2R = D2V * myAxeRev.XDirection();

  D0( U,V,P);
  D1V.Rotate( myAxis, U);
  D2V.Rotate( myAxis, U);
  D3V.Rotate( myAxis, U);
  D1U   =    R * D1;
  D2U   =   -R * D2;
  D3U   =   -R * D1;
  D2UV  =  D1R * D1;
  D3UUV = -D1R * D2;
  D3UVV =  D2R * D1;
}

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec Adaptor3d_SurfaceOfRevolution::DN(const Standard_Real    U,
				       const Standard_Real    V,
                                       const Standard_Integer NU,
				       const Standard_Integer NV) const 
{
  if ( (NU+NV)<1 || NU<0 || NV<0) {
    Standard_DomainError::Raise("Adaptor3d_SurfaceOfRevolution::DN");
  }
  else {
    gp_Vec DNv = myBasisCurve->DN( V, NV);
    if ( NU == 0) {
      return DNv.Rotated( myAxis, U);
    }
    else {
      Standard_Real DNR = DNv * myAxeRev.XDirection();
      gp_Vec DNu = ( myAxeRev.XDirection()).Rotated( myAxis, U + NU*M_PI/2);
      return ( DNR * DNu);
    }
  }   
  // portage WNT
  return gp_Vec();
}

//=======================================================================
//function : UResolution
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_SurfaceOfRevolution::UResolution
(const Standard_Real R3d) const 
{
  return Precision::Parametric(R3d);
}

//=======================================================================
//function : VResolution
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_SurfaceOfRevolution::VResolution
(const Standard_Real R3d) const 
{
  return myBasisCurve->Resolution(R3d);
}

//=======================================================================
//function : GetType
//purpose  : 
//=======================================================================

GeomAbs_SurfaceType Adaptor3d_SurfaceOfRevolution::GetType() const 
{
  Standard_Real TolConf, TolAng;
  GeomAbs_SurfaceType bRet;
  //
  bRet=GeomAbs_SurfaceOfRevolution;
  TolConf = Precision::Confusion();
  TolAng  = Precision::Angular();
  //
  switch ( myBasisCurve->GetType()) {
  case GeomAbs_Line:    {
    gp_Ax1 Axe = myBasisCurve->Line().Position();
    
    if (myAxis.IsParallel(Axe, TolAng)) {
      bRet=GeomAbs_Cylinder;
      return bRet;
    }
    else if (myAxis.IsNormal( Axe, TolAng)) {
      bRet=GeomAbs_Plane;
      return bRet;
    }
    else {
      Standard_Real uf = myBasisCurve->FirstParameter();
      Standard_Real ul = myBasisCurve->LastParameter();
      Standard_Boolean istrim = (!Precision::IsInfinite(uf) && 
				 !Precision::IsInfinite(ul));
      if(istrim){
	gp_Pnt pf = myBasisCurve->Value(uf);
	gp_Pnt pl = myBasisCurve->Value(ul);
	Standard_Real len = pf.Distance(pl);
	//on calcule la distance projetee sur l axe.
	gp_Vec vlin(pf,pl);
	gp_Vec vaxe(myAxis.Direction());
	Standard_Real projlen = Abs(vaxe.Dot(vlin));
	Standard_Real aTolConf = len*TolAng;
	if ((len - projlen) <= aTolConf) {
	  bRet=GeomAbs_Cylinder;
	  return bRet;
	}
	else if (projlen <= aTolConf) {
	  bRet=GeomAbs_Plane;
	  return bRet;
	}
      }
      gp_Vec V(myAxis.Location(),
	       myBasisCurve->Line().Location());
      gp_Vec W(Axe.Direction());
      if (Abs(V.DotCross(myAxis.Direction(),W)) <= TolConf){
	bRet=GeomAbs_Cone;
	return bRet;
      }
      else {
	return bRet;
      }
    }
  }//case GeomAbs_Line: 
  //  
  case GeomAbs_Circle:   {
    
    Standard_Real MajorRadius, aR;
    gp_Lin aLin(myAxis);
    //
    gp_Circ C = myBasisCurve->Circle();
    const gp_Pnt& aLC = C.Location();
    aR=C.Radius();
    //
   
    if (!C.Position().IsCoplanar(myAxis, TolConf, TolAng)) {
      return bRet;
    }
    else if(aLin.Distance(aLC) <= TolConf) {
      bRet=GeomAbs_Sphere;
      return bRet;
    }
    else {
      MajorRadius = aLin.Distance(aLC);
      if(MajorRadius>aR) {
	//modified by NIZNHY-PKV Thu Feb 24 09:46:29 2011f
	Standard_Real aT, aDx, dX;
	gp_Pnt aPx;
	//
	aT=0.;
	aPx=ElCLib::Value(aT, C);
	aDx=aLin.Distance(aPx);
	dX=aDx-MajorRadius-aR;
	if (dX<0.) {
	  dX=-dX;
	}
	if (dX<TolConf) {
	  bRet=GeomAbs_Torus;
	}
	//bRet=GeomAbs_Torus;
	//return bRet;
	//modified by NIZNHY-PKV Thu Feb 24 09:52:29 2011t
      }
      return bRet;
    }
  }
  //  
  default:
    break;
  }
  
  return bRet;
}

//=======================================================================
//function : Plane
//purpose  : 
//=======================================================================

gp_Pln Adaptor3d_SurfaceOfRevolution::Plane() const 
{ 
  Standard_NoSuchObject_Raise_if
    (GetType() != GeomAbs_Plane, "Adaptor3d_SurfaceOfRevolution:Plane");

  gp_Ax3 Axe = myAxeRev;
  gp_Pnt P;
  
  // P = Projection du Point Debut de la generatrice sur l axe de rotation.
  P.SetXYZ((myAxis.Location()).XYZ() +
	   (Value(0.,0.).XYZ()-(myAxis.Location()).XYZ()).
	   Dot((myAxis.Direction()).XYZ())
	   *(myAxis.Direction()).XYZ());
  Axe.SetLocation( P);
  //// modified by jgv, 8.01.03 for OCC1226 ////
  if (Axe.XDirection().
      Dot(myBasisCurve->Line().Direction()) >= -Precision::Confusion()) // > 0.
    Axe.XReverse();
  //////////////////////////////////////////////

  return gp_Pln( Axe);
}

//=======================================================================
//function : Cylinder
//purpose  : 
//=======================================================================

gp_Cylinder Adaptor3d_SurfaceOfRevolution::Cylinder() const
{
  Standard_NoSuchObject_Raise_if
    (GetType() != GeomAbs_Cylinder, "Adaptor3d_SurfaceOfRevolution::Cylinder");

  gp_Pnt P = Value( 0., 0.);
  Standard_Real R   = gp_Vec(myAxeRev.Location(), P) * myAxeRev.XDirection();
  return gp_Cylinder( myAxeRev, R);
}

//=======================================================================
//function : Cone
//purpose  : 
//=======================================================================

gp_Cone Adaptor3d_SurfaceOfRevolution::Cone() const
{
  Standard_NoSuchObject_Raise_if
    ( GetType() != GeomAbs_Cone, "Adaptor3d_SurfaceOfRevolution:Cone");

  gp_Ax3 Axe = myAxeRev;
  gp_Dir ldir = (myBasisCurve->Line()).Direction();
  Standard_Real Angle = (Axe.Direction()).Angle(ldir);
  gp_Pnt P0 = Value(0., 0.);
  Standard_Real R = (Axe.Location()).Distance(P0);
  if ( R >= Precision::Confusion()) {
    gp_Pnt O = Axe.Location();
    gp_Vec OP0(O,P0);
    Standard_Real t = OP0.Dot(Axe.XDirection());
    t /= ldir.Dot(Axe.XDirection());
    OP0.Add(-t * gp_Vec(ldir));
    if ( OP0.Dot(Axe.Direction()) > 0.) Angle = -Angle;
  }
  return gp_Cone( Axe, Angle, R);
}


//=======================================================================
//function : Sphere
//purpose  : 
//=======================================================================

gp_Sphere Adaptor3d_SurfaceOfRevolution::Sphere() const 
{
  Standard_NoSuchObject_Raise_if
    ( GetType() != GeomAbs_Sphere, "Adaptor3d_SurfaceOfRevolution:Sphere");

  gp_Circ C = myBasisCurve->Circle();
  gp_Ax3 Axe = myAxeRev;
  Axe.SetLocation( C.Location());
  return gp_Sphere( Axe, C.Radius());
}


//=======================================================================
//function : Torus
//purpose  : 
//=======================================================================

gp_Torus Adaptor3d_SurfaceOfRevolution::Torus() const 
{
  Standard_NoSuchObject_Raise_if
    (GetType() != GeomAbs_Torus, "Adaptor3d_SurfaceOfRevolution:Torus");

  gp_Circ C = myBasisCurve->Circle();
  Standard_Real MajorRadius = gp_Lin(myAxis).Distance(C.Location());
  return gp_Torus( myAxeRev, MajorRadius, C.Radius());
}

//=======================================================================
//function : UDegree
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_SurfaceOfRevolution::UDegree() const 
{
  Standard_NoSuchObject::Raise("Adaptor3d_SurfaceOfRevolution::UDegree");
  return 0;
}

//=======================================================================
//function : NbUPoles
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_SurfaceOfRevolution::NbUPoles() const 
{
  Standard_NoSuchObject::Raise("Adaptor3d_SurfaceOfRevolution::NbUPoles");
  return 0;
}

//=======================================================================
//function : VDegree
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_SurfaceOfRevolution::VDegree() const 
{
  return myBasisCurve->Degree();
}

//=======================================================================
//function : NbVPoles
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_SurfaceOfRevolution::NbVPoles() const
{
  return myBasisCurve -> NbPoles();
}

//=======================================================================
//function : NbUKnots
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_SurfaceOfRevolution::NbUKnots() const 
{
  Standard_NoSuchObject::Raise("Adaptor3d_SurfaceOfRevolution::NbUKnots");
  return 0;
}


//=======================================================================
//function : NbVKnots
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_SurfaceOfRevolution::NbVKnots() const 
{
  Standard_NoSuchObject::Raise("Adaptor3d_SurfaceOfRevolution::NbVKnots");
  return 0;
}



//=======================================================================
//function : IsURational
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_SurfaceOfRevolution::IsURational() const 
{
  Standard_NoSuchObject::Raise("Adaptor3d_SurfaceOfRevolution::IsURational");
  return Standard_False;
}

//=======================================================================
//function : IsVRational
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_SurfaceOfRevolution::IsVRational() const 
{
  Standard_NoSuchObject::Raise("Adaptor3d_SurfaceOfRevolution::IsVRational");
  return Standard_False;
}


//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================

Handle(Geom_BezierSurface) Adaptor3d_SurfaceOfRevolution::Bezier() const 
{
  Standard_NoSuchObject::Raise("");
  Handle(Geom_BezierSurface) Dummy;
  return Dummy;
}


//=======================================================================
//function : BSpline
//purpose  : 
//=======================================================================

Handle(Geom_BSplineSurface) Adaptor3d_SurfaceOfRevolution::BSpline() const 
{
  Standard_NoSuchObject::Raise("");
  Handle(Geom_BSplineSurface) Dummy;
  return Dummy;
}


//=======================================================================
//function : Axis
//purpose  : 
//=======================================================================

gp_Ax3 Adaptor3d_SurfaceOfRevolution::Axis() const 
{
  return myAxeRev;
}

//=======================================================================
//function : Direction
//purpose  : 
//=======================================================================

gp_Dir Adaptor3d_SurfaceOfRevolution::Direction() const 
{
  Standard_NoSuchObject::Raise("");
  return gp_Dir();
}


//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HCurve) Adaptor3d_SurfaceOfRevolution::BasisCurve() const 
{
  return myBasisCurve;
}
