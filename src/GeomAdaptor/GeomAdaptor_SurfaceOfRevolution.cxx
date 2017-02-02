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

#include <GeomAdaptor_SurfaceOfRevolution.hxx>

#include <Adaptor3d_HCurve.hxx>
#include <ElCLib.hxx>
#include <GeomAdaptor_HSurfaceOfRevolution.hxx>
#include <GeomEvaluator_SurfaceOfRevolution.hxx>
#include <Standard_NoSuchObject.hxx>

//=======================================================================
//function : GeomAdaptor_SurfaceOfRevolution
//purpose  : 
//=======================================================================
GeomAdaptor_SurfaceOfRevolution::GeomAdaptor_SurfaceOfRevolution()
  : myHaveAxis(Standard_False)
{}

//=======================================================================
//function : GeomAdaptor_SurfaceOfRevolution
//purpose  : 
//=======================================================================

GeomAdaptor_SurfaceOfRevolution::GeomAdaptor_SurfaceOfRevolution(
    const Handle(Adaptor3d_HCurve)& C)
  : myHaveAxis(Standard_False)
{
  Load(C);
}

//=======================================================================
//function : GeomAdaptor_SurfaceOfRevolution
//purpose  : 
//=======================================================================

GeomAdaptor_SurfaceOfRevolution::GeomAdaptor_SurfaceOfRevolution(
    const Handle(Adaptor3d_HCurve)& C,
    const gp_Ax1& V)
  : myHaveAxis(Standard_False)
{
  Load(C);
  Load(V);
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void GeomAdaptor_SurfaceOfRevolution::Load(const Handle(Adaptor3d_HCurve)& C)
{
  myBasisCurve = C;
  if (myHaveAxis)
    Load(myAxis); // to evaluate the new myAxeRev.
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void GeomAdaptor_SurfaceOfRevolution::Load(const gp_Ax1& V)
{
  myHaveAxis = Standard_True;
  myAxis = V;

  mySurfaceType = GeomAbs_SurfaceOfRevolution;
  myNestedEvaluator = new GeomEvaluator_SurfaceOfRevolution(myBasisCurve,
      myAxis.Direction(), myAxis.Location());

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
      throw Standard_ConstructionError("Adaptor3d_SurfaceOfRevolution : Axe and meridian are confused");
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

gp_Ax1 GeomAdaptor_SurfaceOfRevolution::AxeOfRevolution() const
{
  return myAxis;
}

//=======================================================================
//function : FirstUParameter
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_SurfaceOfRevolution::FirstUParameter() const 
{
  return 0.;
}

//=======================================================================
//function : LastUParameter
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_SurfaceOfRevolution::LastUParameter() const 
{
  return 2*M_PI;
}

//=======================================================================
//function : FirstVParameter
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_SurfaceOfRevolution::FirstVParameter() const 
{
  return myBasisCurve->FirstParameter();
}

//=======================================================================
//function : LastVParameter
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_SurfaceOfRevolution::LastVParameter() const 
{
  return myBasisCurve->LastParameter();
}

//=======================================================================
//function : UContinuity
//purpose  : 
//=======================================================================

GeomAbs_Shape GeomAdaptor_SurfaceOfRevolution::UContinuity() const 
{
  return GeomAbs_CN;
}

//=======================================================================
//function : VContinuity
//purpose  : 
//=======================================================================

GeomAbs_Shape GeomAdaptor_SurfaceOfRevolution::VContinuity() const 
{
  return myBasisCurve->Continuity();
}

//=======================================================================
//function : NbUIntervals
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_SurfaceOfRevolution::NbUIntervals(const GeomAbs_Shape ) const
{
  return 1;
}

//=======================================================================
//function : NbVIntervals
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_SurfaceOfRevolution::NbVIntervals(const GeomAbs_Shape S) const
{
  return myBasisCurve->NbIntervals(S);
}

//=======================================================================
//function : UIntervals
//purpose  : 
//=======================================================================

void GeomAdaptor_SurfaceOfRevolution::UIntervals(TColStd_Array1OfReal& T,
                                                 const GeomAbs_Shape ) const
{
  T(T.Lower()  ) = 0.;
  T(T.Lower()+1) = 2*M_PI;
}


//=======================================================================
//function : VIntervals
//purpose  : 
//=======================================================================

void GeomAdaptor_SurfaceOfRevolution::VIntervals(TColStd_Array1OfReal& T,
                                                 const GeomAbs_Shape S) const 
{
  myBasisCurve->Intervals(T,S);
}


//=======================================================================
//function : UTrim
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HSurface) GeomAdaptor_SurfaceOfRevolution::UTrim
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
     "GeomAdaptor_SurfaceOfRevolution : UTrim : Parameters out of range");

  Handle(GeomAdaptor_HSurfaceOfRevolution) HR = new GeomAdaptor_HSurfaceOfRevolution(
      GeomAdaptor_SurfaceOfRevolution(myBasisCurve, myAxis));
  return HR;
}


//=======================================================================
//function : VTrim
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HSurface) GeomAdaptor_SurfaceOfRevolution::VTrim
(const Standard_Real First,
 const Standard_Real Last,
 const Standard_Real Tol) const 
{
  Handle(Adaptor3d_HCurve) HC = BasisCurve()->Trim(First,Last,Tol);
  Handle(GeomAdaptor_HSurfaceOfRevolution) HR = new GeomAdaptor_HSurfaceOfRevolution(
      GeomAdaptor_SurfaceOfRevolution(HC, myAxis));
  return HR;
}


//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================

Standard_Boolean GeomAdaptor_SurfaceOfRevolution::IsUClosed() const 
{
  return Standard_True;
}

//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================

Standard_Boolean GeomAdaptor_SurfaceOfRevolution::IsVClosed() const 
{
  return myBasisCurve->IsClosed();
}

//=======================================================================
//function : IsUPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean GeomAdaptor_SurfaceOfRevolution::IsUPeriodic() const
{
  return Standard_True;
}

//=======================================================================
//function : UPeriod
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_SurfaceOfRevolution::UPeriod() const
{
  return 2*M_PI;
}

//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean GeomAdaptor_SurfaceOfRevolution::IsVPeriodic() const
{
  return myBasisCurve->IsPeriodic();
}

//=======================================================================
//function : VPeriod
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_SurfaceOfRevolution::VPeriod() const 
{
  return myBasisCurve->Period();
}

//=======================================================================
//function : UResolution
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_SurfaceOfRevolution::UResolution
(const Standard_Real R3d) const 
{
  return Precision::Parametric(R3d);
}

//=======================================================================
//function : VResolution
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_SurfaceOfRevolution::VResolution
(const Standard_Real R3d) const 
{
  return myBasisCurve->Resolution(R3d);
}

//=======================================================================
//function : GetType
//purpose  : 
//=======================================================================

GeomAbs_SurfaceType GeomAdaptor_SurfaceOfRevolution::GetType() const 
{
  Standard_Real TolConf = Precision::Confusion();
  Standard_Real TolAng  = Precision::Angular();
  Standard_Real TolConeSemiAng = Precision::Confusion();

  switch (myBasisCurve->GetType()) {
  case GeomAbs_Line:    {
    gp_Ax1 Axe = myBasisCurve->Line().Position();
    
    if (myAxis.IsParallel(Axe, TolAng))
    {
      gp_Pnt P = Value(0., 0.);
      Standard_Real R = gp_Vec(myAxeRev.Location(), P) * myAxeRev.XDirection();
      if (R > TolConf)
      {
        return GeomAbs_Cylinder;
      }
    }
    else if (myAxis.IsNormal(Axe, TolAng))
      return GeomAbs_Plane;
    else
    {
      Standard_Real uf = myBasisCurve->FirstParameter();
      Standard_Real ul = myBasisCurve->LastParameter();
      Standard_Boolean istrim = (!Precision::IsInfinite(uf) && 
                                 !Precision::IsInfinite(ul));
      if(istrim)
      {
        gp_Pnt pf = myBasisCurve->Value(uf);
        gp_Pnt pl = myBasisCurve->Value(ul);
        Standard_Real len = pf.Distance(pl);
        //on calcule la distance projetee sur l axe.
        gp_Vec vlin(pf,pl);
        gp_Vec vaxe(myAxis.Direction());
        Standard_Real projlen = Abs(vaxe.Dot(vlin));
        if ((len - projlen) <= TolConf)
        {
          gp_Pnt P = Value(0., 0.);
          Standard_Real R = gp_Vec(myAxeRev.Location(), P) * myAxeRev.XDirection();
          if (R > TolConf)
          {
            return GeomAbs_Cylinder;
          }
        }
        else if (projlen <= TolConf)
          return GeomAbs_Plane;
      }
      gp_Vec V(myAxis.Location(), myBasisCurve->Line().Location());
      gp_Vec W(Axe.Direction());
      gp_Vec AxisDir(myAxis.Direction());
      Standard_Real proj = Abs(W.Dot(AxisDir));
      if (Abs(V.DotCross(AxisDir, W)) <= TolConf &&
        (proj >= TolConeSemiAng && proj <= 1. - TolConeSemiAng))
      {
        return GeomAbs_Cone;
      }
    }
    break;
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
   
    if (!C.Position().IsCoplanar(myAxis, TolConf, TolAng))
      return GeomAbs_SurfaceOfRevolution;
    else if(aLin.Distance(aLC) <= TolConf)
      return GeomAbs_Sphere;
    else
    {
      MajorRadius = aLin.Distance(aLC);
      if(MajorRadius > aR)
      {
        Standard_Real aT = 0., aDx, dX;
        gp_Pnt aPx;

        aPx = ElCLib::Value(aT, C);
        aDx = aLin.Distance(aPx);
        dX = aDx - MajorRadius - aR;
        if (dX < 0.)
          dX = -dX;
        if (dX < TolConf)
          return GeomAbs_Torus;
      }
    }
    break;
  }
  //  
  default:
    break;
  }
  
  return GeomAbs_SurfaceOfRevolution;
}

//=======================================================================
//function : Plane
//purpose  : 
//=======================================================================

gp_Pln GeomAdaptor_SurfaceOfRevolution::Plane() const 
{ 
  Standard_NoSuchObject_Raise_if
    (GetType() != GeomAbs_Plane, "GeomAdaptor_SurfaceOfRevolution:Plane");

  gp_Ax3 Axe = myAxeRev;
  gp_Pnt aPonCurve = Value(0., 0.);
  Standard_Real aDot = (aPonCurve.XYZ() - myAxis.Location().XYZ()).Dot(myAxis.Direction().XYZ());

  gp_Pnt P(myAxis.Location().XYZ() + aDot * myAxis.Direction().XYZ());
  Axe.SetLocation(P);
  if (Axe.XDirection().Dot(myBasisCurve->Line().Direction()) >= -Precision::Confusion())
    Axe.XReverse();

  return gp_Pln( Axe);
}

//=======================================================================
//function : Cylinder
//purpose  : 
//=======================================================================

gp_Cylinder GeomAdaptor_SurfaceOfRevolution::Cylinder() const
{
  Standard_NoSuchObject_Raise_if
    (GetType() != GeomAbs_Cylinder, "GeomAdaptor_SurfaceOfRevolution::Cylinder");

  gp_Pnt P = Value(0., 0.);
  Standard_Real R = gp_Vec(myAxeRev.Location(), P) * myAxeRev.XDirection();
  return gp_Cylinder(myAxeRev, R);
}

//=======================================================================
//function : Cone
//purpose  : 
//=======================================================================

gp_Cone GeomAdaptor_SurfaceOfRevolution::Cone() const
{
  Standard_NoSuchObject_Raise_if
    ( GetType() != GeomAbs_Cone, "GeomAdaptor_SurfaceOfRevolution:Cone");

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

gp_Sphere GeomAdaptor_SurfaceOfRevolution::Sphere() const 
{
  Standard_NoSuchObject_Raise_if
    ( GetType() != GeomAbs_Sphere, "GeomAdaptor_SurfaceOfRevolution:Sphere");

  gp_Circ C = myBasisCurve->Circle();
  gp_Ax3 Axe = myAxeRev;
  Axe.SetLocation(C.Location());
  return gp_Sphere(Axe, C.Radius());
}


//=======================================================================
//function : Torus
//purpose  : 
//=======================================================================

gp_Torus GeomAdaptor_SurfaceOfRevolution::Torus() const 
{
  Standard_NoSuchObject_Raise_if
    (GetType() != GeomAbs_Torus, "GeomAdaptor_SurfaceOfRevolution:Torus");

  gp_Circ C = myBasisCurve->Circle();
  Standard_Real MajorRadius = gp_Lin(myAxis).Distance(C.Location());
  return gp_Torus(myAxeRev, MajorRadius, C.Radius());
}

//=======================================================================
//function : VDegree
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_SurfaceOfRevolution::VDegree() const 
{
  return myBasisCurve->Degree();
}

//=======================================================================
//function : NbVPoles
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_SurfaceOfRevolution::NbVPoles() const
{
  return myBasisCurve->NbPoles();
}

//=======================================================================
//function : NbVKnots
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_SurfaceOfRevolution::NbVKnots() const 
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfRevolution::NbVKnots");
}



//=======================================================================
//function : IsURational
//purpose  : 
//=======================================================================

Standard_Boolean GeomAdaptor_SurfaceOfRevolution::IsURational() const 
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfRevolution::IsURational");
}

//=======================================================================
//function : IsVRational
//purpose  : 
//=======================================================================

Standard_Boolean GeomAdaptor_SurfaceOfRevolution::IsVRational() const 
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfRevolution::IsVRational");
}


//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================

Handle(Geom_BezierSurface) GeomAdaptor_SurfaceOfRevolution::Bezier() const 
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfRevolution::Bezier");
}


//=======================================================================
//function : BSpline
//purpose  : 
//=======================================================================

Handle(Geom_BSplineSurface) GeomAdaptor_SurfaceOfRevolution::BSpline() const 
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfRevolution::BSpline");
}

//=======================================================================
//function : Axis
//purpose  : 
//=======================================================================

const gp_Ax3& GeomAdaptor_SurfaceOfRevolution::Axis() const 
{
  return myAxeRev;
}

//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HCurve) GeomAdaptor_SurfaceOfRevolution::BasisCurve() const 
{
  return myBasisCurve;
}
