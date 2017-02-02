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
#include <Adaptor3d_HIsoCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_IsoCurve.hxx>
#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>

//=======================================================================
//function : Adaptor3d_IsoCurve
//purpose  : 
//=======================================================================
Adaptor3d_IsoCurve::Adaptor3d_IsoCurve()
: myIso      (GeomAbs_NoneIso),
  myFirst    (0.0),
  myLast     (0.0),
  myParameter(0.0)
{
}

//=======================================================================
//function : Adaptor3d_IsoCurve
//purpose  : 
//=======================================================================

Adaptor3d_IsoCurve::Adaptor3d_IsoCurve(const Handle(Adaptor3d_HSurface)& S)
: mySurface  (S),
  myIso      (GeomAbs_NoneIso),
  myFirst    (0.0),
  myLast     (0.0),
  myParameter(0.0)
{
}

//=======================================================================
//function : Adaptor3d_IsoCurve
//purpose  : 
//=======================================================================

Adaptor3d_IsoCurve::Adaptor3d_IsoCurve(const Handle(Adaptor3d_HSurface)& S,
                                       const GeomAbs_IsoType theIso,
                                       const Standard_Real theParam)
: mySurface  (S),
  myIso      (GeomAbs_NoneIso),
  myFirst    (0.0),
  myLast     (0.0),
  myParameter(0.0)
{
  Load(theIso, theParam);
}

//=======================================================================
//function : Adaptor3d_IsoCurve
//purpose  : 
//=======================================================================

Adaptor3d_IsoCurve::Adaptor3d_IsoCurve(const Handle(Adaptor3d_HSurface)& theS,
                                       const GeomAbs_IsoType theIso,
                                       const Standard_Real theParam,
                                       const Standard_Real theWFirst,
                                       const Standard_Real theWLast)
: mySurface  (theS),
  myIso      (theIso),
  myFirst    (theWFirst),
  myLast     (theWLast),
  myParameter(theParam)
{
  Load(theIso, theParam, theWFirst, theWLast);
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void Adaptor3d_IsoCurve::Load(const Handle(Adaptor3d_HSurface)& S ) 
{
  mySurface = S;
  myIso = GeomAbs_NoneIso;
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void Adaptor3d_IsoCurve::Load(const GeomAbs_IsoType Iso,
			    const Standard_Real Param) 
{
  switch (Iso) {

  case GeomAbs_IsoU:
    Load(Iso,Param,
	 mySurface->FirstVParameter(),
	 mySurface->LastVParameter());
    break;
      
  case GeomAbs_IsoV:
    Load(Iso,Param,
	 mySurface->FirstUParameter(),
	 mySurface->LastUParameter());
    break;

  case GeomAbs_NoneIso:
    throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
    break;
  }
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void Adaptor3d_IsoCurve::Load(const GeomAbs_IsoType Iso,
			    const Standard_Real Param,
			    const Standard_Real WFirst,
			    const Standard_Real WLast) 
{
  myIso =Iso;
  myParameter = Param;
  myFirst = WFirst;
  myLast = WLast;


  if (myIso == GeomAbs_IsoU) {
    myFirst = Max(myFirst, mySurface->FirstVParameter());
    myLast  = Min(myLast,  mySurface->LastVParameter());
  }
  else {
    myFirst = Max(myFirst, mySurface->FirstUParameter());
    myLast  = Min(myLast,  mySurface->LastUParameter());
  }

// Adjust the parameters on periodic surfaces

  Standard_Real dummy = myParameter;

  if (mySurface->IsUPeriodic()) {
    
    if (myIso == GeomAbs_IsoU) {
      ElCLib::AdjustPeriodic
	(mySurface->FirstUParameter(),
	 mySurface->FirstUParameter()+
	 mySurface->UPeriod(),
	 mySurface->UResolution(Precision::Confusion()),
	 myParameter,dummy);
    }
    else {
      ElCLib::AdjustPeriodic
	(mySurface->FirstUParameter(),
	 mySurface->FirstUParameter()+
	 mySurface->UPeriod(),
	 mySurface->UResolution(Precision::Confusion()),
	 myFirst,myLast);
    }
  }
  
  if (mySurface->IsVPeriodic()) {
    
    if (myIso == GeomAbs_IsoV) {
      ElCLib::AdjustPeriodic
	(mySurface->FirstVParameter(),
	 mySurface->FirstVParameter() +
	 mySurface->VPeriod(),
	 mySurface->VResolution(Precision::Confusion()),
	 myParameter,dummy);
    }
    else {
      ElCLib::AdjustPeriodic
	(mySurface->FirstVParameter(),
	 mySurface->FirstVParameter() +
	 mySurface->VPeriod(),
	 mySurface->VResolution(Precision::Confusion()),
	 myFirst,myLast);
    }
  }
  
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Adaptor3d_IsoCurve::Continuity() const
{
  switch (myIso) {
  case GeomAbs_IsoU: 
    return mySurface->VContinuity();
  case GeomAbs_IsoV:
    return mySurface->UContinuity();
  case GeomAbs_NoneIso:
  default:
    break;
  }
  
  throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
}

//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_IsoCurve::NbIntervals(const GeomAbs_Shape S) const
{
  if (myIso == GeomAbs_NoneIso) throw Standard_NoSuchObject();
  Standard_Boolean UIso =  (myIso == GeomAbs_IsoU);

  Standard_Integer nbInter = UIso ? 
      mySurface->NbVIntervals(S) :  
      mySurface->NbUIntervals(S);

  TColStd_Array1OfReal T(1,nbInter+1);

  if (UIso) 
    mySurface->VIntervals(T,S);
  else
    mySurface->UIntervals(T,S);

  if(nbInter == 1) return nbInter;

  Standard_Integer first = 1;
  while (T(first) <= myFirst) first++;
  Standard_Integer last = nbInter+1;
  while (T(last) >= myLast) last--;
  return (last - first + 2);
}

//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================

void Adaptor3d_IsoCurve::Intervals(TColStd_Array1OfReal& TI,
                                   const GeomAbs_Shape S) const
{
  if (myIso == GeomAbs_NoneIso) throw Standard_NoSuchObject();
  Standard_Boolean UIso =  (myIso == GeomAbs_IsoU);

  Standard_Integer nbInter = UIso ? 
      mySurface->NbVIntervals(S) :  
      mySurface->NbUIntervals(S);

  TColStd_Array1OfReal T(1,nbInter+1);

  if (UIso) 
    mySurface->VIntervals(T,S);
  else
    mySurface->UIntervals(T,S);

  if(nbInter == 1) {
    TI(TI.Lower()) = myFirst ;
    TI(TI.Lower() + 1) = myLast ;
    return;
  }

  Standard_Integer first = 1;
  while (T(first) <= myFirst) first++;
  Standard_Integer last = nbInter+1;
  while (T(last) >= myLast) last--;

  Standard_Integer i = TI.Lower(), j;
  for (j = first-1; j <= last+1; j++) {
    TI(i) = T(j);
    i++;
  }
  TI(TI.Lower()) = myFirst ;
  TI(TI.Lower() + last-first + 2) = myLast ; 
}

//=======================================================================
//function : Trim
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HCurve) Adaptor3d_IsoCurve::Trim
 (const Standard_Real First,
  const Standard_Real Last,
  const Standard_Real) const 
{
  Handle(Adaptor3d_HIsoCurve) HI = new Adaptor3d_HIsoCurve(*this);
  ((Adaptor3d_IsoCurve *)&(HI->Curve()))->Load(myIso,myParameter,First,Last);
  return HI;
}

//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_IsoCurve::IsClosed() const
{
  switch (myIso) {
  case GeomAbs_IsoU:
    return mySurface->IsVClosed();
  case GeomAbs_IsoV:
    return mySurface->IsUClosed();
  case GeomAbs_NoneIso:
  default:
    break;
  }

  throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
}

//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_IsoCurve::IsPeriodic() const
{
  switch (myIso) {
  case GeomAbs_IsoU:
    return mySurface->IsVPeriodic();
  case GeomAbs_IsoV:
    return mySurface->IsUPeriodic();
  case GeomAbs_NoneIso:
  default:
    break;
  }

  throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
}

//=======================================================================
//function : Period
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_IsoCurve::Period() const
{
  switch (myIso) {
  case GeomAbs_IsoU:
    return mySurface->VPeriod();
  case GeomAbs_IsoV:
    return mySurface->UPeriod();
  case GeomAbs_NoneIso:
  default:
    break;
  }

  throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt Adaptor3d_IsoCurve::Value(const Standard_Real T) const
{
  switch (myIso) {
	
  case GeomAbs_IsoU:
    return mySurface->Value(myParameter,T);
    
  case GeomAbs_IsoV:
    return mySurface->Value(T,myParameter);
    
  case GeomAbs_NoneIso: 
    {
      throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
      break;
    }
  }
  // portage WNT
  return gp_Pnt();
}


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Adaptor3d_IsoCurve::D0(const Standard_Real T, gp_Pnt& P) const
{
  switch (myIso) {
    
  case GeomAbs_IsoU:
    mySurface->D0(myParameter,T,P);
    break;
    
  case GeomAbs_IsoV:
    mySurface->D0(T,myParameter,P);
    break;
    
  case GeomAbs_NoneIso:
    throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
    break;
  }
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Adaptor3d_IsoCurve::D1(const Standard_Real T, gp_Pnt& P, gp_Vec& V) const
{
  gp_Vec dummy;
  switch (myIso) {
    
  case GeomAbs_IsoU:
    mySurface->D1(myParameter,T,P,dummy,V);
    break;
    
  case GeomAbs_IsoV:
    mySurface->D1(T,myParameter,P,V,dummy);
    break;
    
  case GeomAbs_NoneIso:
    throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
    break;
  }
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Adaptor3d_IsoCurve::D2(const Standard_Real T, gp_Pnt& P, 
			  gp_Vec& V1, gp_Vec& V2) const
{
  gp_Vec dummy1,dummy2,dummy3;
  switch (myIso) {
    
  case GeomAbs_IsoU:
    mySurface->D2(myParameter,T,P,
		  dummy1,V1,dummy2,V2,dummy3);
    break;      
  case GeomAbs_IsoV:
    mySurface->D2(T,myParameter,
		  P,V1,dummy1,V2,dummy2,dummy3);
    break;
  case GeomAbs_NoneIso:
    throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
    break;
  }
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Adaptor3d_IsoCurve::D3(const Standard_Real T, gp_Pnt& P,
			  gp_Vec& V1, gp_Vec& V2, gp_Vec& V3) const
{
  gp_Vec dummy[6];
  switch (myIso) {
    
  case GeomAbs_IsoU:
    mySurface->D3(myParameter,T,P,dummy[0],V1,dummy[1],
		  V2,dummy[2],dummy[3],V3,dummy[4],dummy[5]);
    break;
    
  case GeomAbs_IsoV:
    mySurface->D3(T,myParameter,P,V1,dummy[0],V2,dummy[1],
		  dummy[2],V3,dummy[3],dummy[4],dummy[5]);
    break;
    
  case GeomAbs_NoneIso:
    throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
    break;
  }
}

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec Adaptor3d_IsoCurve::DN(const Standard_Real T, 
			    const Standard_Integer N) const
{
  switch (myIso) {
    
  case GeomAbs_IsoU:
    return mySurface->DN(myParameter,T,0,N);
  case GeomAbs_IsoV:
    return mySurface->DN(T,myParameter,N,0);
  case GeomAbs_NoneIso:
    {
      throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
      break;
    }
  }

  // portage WNT
  return gp_Vec();
}


//=======================================================================
//function : Resolution
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_IsoCurve::Resolution(const Standard_Real R3D) const
{
  // Peut-on faire mieux ??
  return Precision::Parametric(R3D);
}



//=======================================================================
//function : GetType
//purpose  : 
//=======================================================================

GeomAbs_CurveType Adaptor3d_IsoCurve::GetType() const {
  
  switch (mySurface->GetType()) {

  case GeomAbs_Plane:
    return GeomAbs_Line;
    
  case GeomAbs_Cylinder:
  case GeomAbs_Cone:
    {
      switch (myIso) {
      case GeomAbs_IsoU:
	return GeomAbs_Line;
	
      case GeomAbs_IsoV:
	return GeomAbs_Circle;
	
      case GeomAbs_NoneIso:
	{
	  throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
	}
      }
      break;
    }
    
  case GeomAbs_Sphere:
  case GeomAbs_Torus:
    return GeomAbs_Circle;
    
  case GeomAbs_BezierSurface:
    return GeomAbs_BezierCurve;
    
  case GeomAbs_BSplineSurface:
    return GeomAbs_BSplineCurve;
    
  case GeomAbs_SurfaceOfRevolution:
    {
      switch (myIso) {
      case GeomAbs_IsoU:
	return mySurface->BasisCurve()->GetType();
	
      case GeomAbs_IsoV:
	return GeomAbs_Circle;
	
      case GeomAbs_NoneIso:
	throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
	break;
      }
      break;
    }
    
  case GeomAbs_SurfaceOfExtrusion:
    {
      switch (myIso) {
      case GeomAbs_IsoU:
	return GeomAbs_Line;
	
      case GeomAbs_IsoV:
	return mySurface->BasisCurve()->GetType();
	
      case GeomAbs_NoneIso:
	throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
	break;
      }
      break;
    }
  default:
    return GeomAbs_OtherCurve;
  }

  // portage WNT
  return GeomAbs_OtherCurve;
}

//=======================================================================
//function : Line
//purpose  : 
//=======================================================================

gp_Lin Adaptor3d_IsoCurve::Line() const
{
  gp_Pnt P;
  gp_Vec V;
  D1(0,P,V);
  return gp_Lin(P,V);
}

//=======================================================================
//function : computeHR
//purpose  : 
//=======================================================================

static void computeHR(const gp_Ax3&        axes, 
		      const gp_Pnt&        P, 
		            Standard_Real& h,
		            Standard_Real& radius)
{
  gp_Vec V(axes.Location(),P);
  h = V * axes.Direction();
  radius = V * axes.XDirection();
}

//=======================================================================
//function : Circle
//purpose  : 
//=======================================================================

gp_Circ Adaptor3d_IsoCurve::Circle() const
{
  gp_Ax3 axes;
  Standard_Real radius,h = 0.;

  switch (mySurface->GetType()) {
    
  case GeomAbs_Cylinder: 
    {
      gp_Cylinder cyl = mySurface->Cylinder();
      
      switch (myIso) {
	
      case GeomAbs_IsoU: 
	{
	  throw Standard_NoSuchObject("Adaptor3d_IsoCurve:UIso");
	}
      case GeomAbs_IsoV: 
	{
	  return ElSLib::CylinderVIso(cyl.Position(),cyl.Radius(),myParameter);
	}
      case GeomAbs_NoneIso: 
	{
	  throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
	}
      }
      break;
    }

  case GeomAbs_Cone: 
    {
      gp_Cone cone = mySurface->Cone();
      
      switch (myIso) {
	
      case GeomAbs_IsoU: 
	{
	  throw Standard_NoSuchObject("Adaptor3d_IsoCurve:UIso");
	}
      case GeomAbs_IsoV: 
	{
	  return ElSLib::ConeVIso(cone.Position(),cone.RefRadius(),
				  cone.SemiAngle(),myParameter);
	}
      case GeomAbs_NoneIso: 
	{
	  throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
	}
      }
      break;
    }
    
  case GeomAbs_Sphere: 
    {
      gp_Sphere sph = mySurface->Sphere();
      
      switch (myIso) {
	
      case GeomAbs_IsoU: 
	{
	  return ElSLib::SphereUIso(sph.Position(),sph.Radius(),myParameter);
	}
	
      case GeomAbs_IsoV: 
	{
	  return ElSLib::SphereVIso(sph.Position(),sph.Radius(),myParameter);
	}
	
      case GeomAbs_NoneIso: 
	{
	  throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
	}
      }
      break;
    }
    
  case GeomAbs_Torus: {
    gp_Torus tor = mySurface->Torus();

    switch (myIso) {

    case GeomAbs_IsoU: 
      {
	return ElSLib::TorusUIso(tor.Position(),tor.MajorRadius(),
				 tor.MinorRadius(),myParameter);
      }
      
    case GeomAbs_IsoV: 
      {
	return ElSLib::TorusVIso(tor.Position(),tor.MajorRadius(),
				 tor.MinorRadius(),myParameter);
      }
      
    case GeomAbs_NoneIso: 
      {
	throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
      }
    }
    break;
  }
    
  case GeomAbs_SurfaceOfRevolution: 
    {
      if (myIso == GeomAbs_IsoV) {
        const gp_Pnt aVal0 = Value (0.0);
        gp_Ax1 Ax1 = mySurface->AxeOfRevolution();
        if (gp_Lin (Ax1).Contains (aVal0, Precision::Confusion())) {
          return gp_Circ(gp_Ax2(aVal0, Ax1.Direction()),0);
        }
        else {
          gp_Vec DX(Ax1.Location(), aVal0);
          axes = gp_Ax3(Ax1.Location(), Ax1.Direction(), DX);
          computeHR(axes,aVal0,h,radius);
          gp_Vec VT = axes.Direction();
          axes.Translate(VT * h);
          return gp_Circ(axes.Ax2(),radius);
        }
      }
      else {
	return mySurface->BasisCurve()->Circle().Rotated
	  (mySurface->AxeOfRevolution(),myParameter);
      }
    }
    
  case GeomAbs_SurfaceOfExtrusion: {
    return  mySurface->BasisCurve()->Circle().Translated
      (myParameter * gp_Vec(mySurface->Direction()));
  }
  default:  
    {
      throw Standard_NoSuchObject("Adaptor3d_IsoCurve:Circle");
    }
    
  }
  // portage WNT
  return gp_Circ();
}

//=======================================================================
//function : Ellipse
//purpose  : 
//=======================================================================

gp_Elips Adaptor3d_IsoCurve::Ellipse() const
{
  switch (mySurface->GetType()) {
    
  case GeomAbs_SurfaceOfExtrusion: {
    return  mySurface->BasisCurve()->Ellipse().Translated
      (myParameter * gp_Vec(mySurface->Direction()));
  }
  default:  
    {
      throw Standard_NoSuchObject("Adaptor3d_IsoCurve:Ellipse");
    } 
  }
}

//=======================================================================
//function : Hyperbola
//purpose  : 
//=======================================================================

gp_Hypr Adaptor3d_IsoCurve::Hyperbola() const
{
  throw Standard_NoSuchObject("Adaptor3d_IsoCurve:Hyperbola");
}

//=======================================================================
//function : Parabola
//purpose  : 
//=======================================================================

gp_Parab Adaptor3d_IsoCurve::Parabola() const
{
  throw Standard_NoSuchObject("Adaptor3d_IsoCurve:Parabola");
}

//=======================================================================
//function : Degree
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_IsoCurve::Degree() const 
{
  Standard_Integer degree = 0 ;
  GeomAbs_SurfaceType type = mySurface->GetType() ;
  switch(type) {
  case GeomAbs_BezierSurface:
  case GeomAbs_BSplineSurface:
    {
      switch (myIso) {
      case GeomAbs_IsoU:
	degree = mySurface->VDegree() ;
	break ;
      case GeomAbs_IsoV:
	degree = mySurface->UDegree() ;
	break ;
	
      case GeomAbs_NoneIso:
      default:
	throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
      }	
    }
    break ;
  case GeomAbs_SurfaceOfRevolution:
    {
      switch (myIso) {
      case GeomAbs_IsoU:
	degree = mySurface->BasisCurve()->Degree();
	break;
      default:
	throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
      }
    }
    break;
  case GeomAbs_SurfaceOfExtrusion:
    {
      switch (myIso) {
      case GeomAbs_IsoV:
	degree = mySurface->BasisCurve()->Degree();
	break;
      default:
	throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
      }
    }
    break;
  default:
    throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
    break ;
  }
  return degree ;
}

//=======================================================================
//function : IsRational
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_IsoCurve::IsRational() const 
{
  Standard_Boolean is_rational = Standard_False;
  GeomAbs_SurfaceType type = mySurface->GetType() ;
  switch(type) {
  case GeomAbs_BezierSurface:
  case GeomAbs_BSplineSurface:
    {
      switch (myIso) {
      case GeomAbs_IsoU:
	is_rational = mySurface->IsVRational() ;
	break ;
      case GeomAbs_IsoV:
	is_rational = mySurface->IsURational() ;
	break ;
	
      case GeomAbs_NoneIso:
      default:
	throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
      }	
    }
    break ;
  case GeomAbs_SurfaceOfRevolution:
    {
      switch (myIso) {
      case GeomAbs_IsoU:
	is_rational = mySurface->BasisCurve()->IsRational();
	break;
      default:
	throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
      }
    }
    break;
  case GeomAbs_SurfaceOfExtrusion:
    {
      switch (myIso) {
      case GeomAbs_IsoV:
	is_rational = mySurface->BasisCurve()->IsRational();
	break;
      default:
	throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
      }
    }
    break;
  default:
    throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
  }
  return is_rational;
}	


//=======================================================================
//function : NbPoles
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_IsoCurve::NbPoles() const 
{
  Standard_Integer nb_poles = 0 ;
  GeomAbs_SurfaceType type = mySurface->GetType() ;
  switch(type) {
  case GeomAbs_BezierSurface:
  case GeomAbs_BSplineSurface:
    switch (myIso) {
    case GeomAbs_IsoU:
      nb_poles = mySurface->NbVPoles() ;
      break ;
    case GeomAbs_IsoV:
      nb_poles = mySurface->NbUPoles() ;
      break ;
      
    case GeomAbs_NoneIso:
    default:
      throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
    }	
    break ;
  case GeomAbs_SurfaceOfRevolution: 
    {
      switch( myIso) {
      case GeomAbs_IsoU: 
	{
	  nb_poles = mySurface->BasisCurve()->NbPoles();
	}
	break;
      default:
	throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
      }
    }
    break;
  case GeomAbs_SurfaceOfExtrusion:
    {
      switch( myIso) {
      case GeomAbs_IsoV: 
	{
	  nb_poles = mySurface->BasisCurve()->NbPoles();
	}
	break;
      default:
	throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
      }
    }
    break;

  default:
    throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
    break ;
  }
 return nb_poles ;
 }

//=======================================================================
//function : NbKnots
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_IsoCurve::NbKnots() const 
{
  Standard_Integer nb_knots = 0 ;
  GeomAbs_SurfaceType type = mySurface->GetType() ;
  switch(type) {
  case GeomAbs_BSplineSurface:
    {
      switch (myIso) {
      case GeomAbs_IsoU:
	nb_knots = mySurface->NbVKnots() ;
	break ;
      case GeomAbs_IsoV:
	nb_knots = mySurface->NbUKnots() ;
	break ;
	
      case GeomAbs_NoneIso:
      default:
	throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
      }	
    }
    break ;
  case GeomAbs_SurfaceOfRevolution:
    {
      switch (myIso) {
      case GeomAbs_IsoU:
	{
	  nb_knots = mySurface->BasisCurve()->NbKnots() ;
	  break ;
	}
      default:
	throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
      }	
    }
    break ;
  case GeomAbs_SurfaceOfExtrusion:
    {
      switch (myIso) {
      case GeomAbs_IsoV:
	{
	  nb_knots = mySurface->BasisCurve()->NbKnots() ;
	  break ;
	}
      default:
	throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
      }	
    }
    break ;
  default:
    throw Standard_NoSuchObject("Adaptor3d_IsoCurve:NoneIso");
    break ;
  }
  return nb_knots ;
 }

//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================

Handle(Geom_BezierCurve) Adaptor3d_IsoCurve::Bezier() const 
{
  Handle(Geom_BezierCurve) C;
  if (mySurface->GetType() == GeomAbs_SurfaceOfRevolution) {
    C = mySurface->BasisCurve()->Bezier();
    C = Handle(Geom_BezierCurve)::DownCast(C->Copy());
    C->Rotate(mySurface->AxeOfRevolution(),myParameter);
  }
  else if (mySurface->GetType() == GeomAbs_SurfaceOfExtrusion) {
    C = mySurface->BasisCurve()->Bezier();
    C = Handle(Geom_BezierCurve)::DownCast(C->Copy());
    C->Translate(myParameter * gp_Vec(mySurface->Direction()));
  }
  else if (myIso == GeomAbs_IsoU) {
    C = Handle(Geom_BezierCurve)::DownCast
      (mySurface->Bezier()->UIso(myParameter)) ;
  }
  else {
    C = Handle(Geom_BezierCurve)::DownCast
      (mySurface->Bezier()->VIso(myParameter));
  }
//  C->Segment(myFirst,myLast);
  return C;
}

//=======================================================================
//function : BSpline
//purpose  : 
//=======================================================================

Handle(Geom_BSplineCurve) Adaptor3d_IsoCurve::BSpline() const 
{
  Handle(Geom_BSplineCurve) C;
  if (mySurface->GetType() == GeomAbs_SurfaceOfRevolution) {
    C = mySurface->BasisCurve()->BSpline();
    C = Handle(Geom_BSplineCurve)::DownCast(C->Copy());
    C->Rotate(mySurface->AxeOfRevolution(),myParameter);
  }
  else if (mySurface->GetType() == GeomAbs_SurfaceOfExtrusion) {
    C = mySurface->BasisCurve()->BSpline();
    C = Handle(Geom_BSplineCurve)::DownCast(C->Copy());
    C->Translate(myParameter * gp_Vec(mySurface->Direction()));
  }
  else if (myIso == GeomAbs_IsoU) {
    C = Handle(Geom_BSplineCurve)::DownCast
      (mySurface->BSpline()->UIso(myParameter)) ;
  }
  else {
    C = Handle(Geom_BSplineCurve)::DownCast
      (mySurface->BSpline()->VIso(myParameter));
  }
//  C->Segment(myFirst,myLast);
  return C;
}

