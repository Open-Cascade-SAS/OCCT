// Created on: 1993-05-14
// Created by: Joelle CHAUVET
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

// Modified:	Thu Nov 26 16:37:18 1998
//		correction in NbUIntervals for SurfaceOfLinearExtrusion 
//		(PRO16346)

#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define PosTol (Precision::PConfusion()*0.5)


#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <BSplCLib.hxx>
#include <BSplSLib_Cache.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax1.hxx>
#include <gp_Circ.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>

//#include <GeomConvert_BSplineSurfaceKnotSplitting.hxx>
#define myBspl Handle(Geom_BSplineSurface)::DownCast (mySurface)
#define myExtSurf Handle(Geom_SurfaceOfLinearExtrusion)::DownCast (mySurface)
#define myRevSurf Handle(Geom_SurfaceOfRevolution)::DownCast (mySurface)
#define myOffSurf Handle(Geom_OffsetSurface)::DownCast (mySurface)

//=======================================================================
//function : LocalContinuity
//purpose  : 
//=======================================================================

GeomAbs_Shape LocalContinuity(Standard_Integer         Degree,
                              Standard_Integer         Nb,
                              TColStd_Array1OfReal&    TK,
                              TColStd_Array1OfInteger& TM,
                              Standard_Real            PFirst,
                              Standard_Real            PLast,
                              Standard_Boolean         IsPeriodic) 
{
  Standard_DomainError_Raise_if( (TK.Length()!=Nb || TM.Length()!=Nb )," ");
  Standard_Integer Index1 = 0;
  Standard_Integer Index2 = 0;
  Standard_Real newFirst, newLast;
  BSplCLib::LocateParameter(Degree,TK,TM,PFirst,IsPeriodic,1,Nb,Index1,newFirst);
  BSplCLib::LocateParameter(Degree,TK,TM,PLast, IsPeriodic,1,Nb,Index2,newLast );
  const Standard_Real EpsKnot = Precision::PConfusion();
  if (Abs(newFirst-TK(Index1+1))< EpsKnot) Index1++;
  if (Abs(newLast -TK(Index2  ))< EpsKnot) Index2--;
  // attention aux courbes peridiques.
  if ( (IsPeriodic) && (Index1 == Nb) )
    Index1 = 1;

  if (Index2!=Index1)
  {
    Standard_Integer i, Multmax = TM(Index1+1);
	for (i = Index1+1; i<=Index2; i++) {
      if (TM(i)>Multmax) Multmax=TM(i);
    }
    Multmax = Degree - Multmax;
    if (Multmax <= 0) return GeomAbs_C0;
    switch (Multmax) {
    case 1: return GeomAbs_C1;
    case 2: return GeomAbs_C2;
    case 3: return GeomAbs_C3;
    }
  }
  return GeomAbs_CN;
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void GeomAdaptor_Surface::load(const Handle(Geom_Surface)& S,
                               const Standard_Real UFirst,
                               const Standard_Real ULast,
                               const Standard_Real VFirst,
                               const Standard_Real VLast,
                               const Standard_Real TolU,
                               const Standard_Real TolV)
{
  myTolU =  TolU;
  myTolV =  TolV;  
  myUFirst = UFirst;
  myULast  = ULast;
  myVFirst = VFirst;
  myVLast  = VLast;

  if ( mySurface != S) {
    mySurface = S;
    
    const Handle(Standard_Type)& TheType = S->DynamicType();
    if ( TheType == STANDARD_TYPE(Geom_BezierSurface))
      mySurfaceType = GeomAbs_BezierSurface;
    else if (TheType == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
      Load(Handle(Geom_RectangularTrimmedSurface)::DownCast (S)->BasisSurface(),
           UFirst,ULast,VFirst,VLast);
    }
    else if ( TheType == STANDARD_TYPE(Geom_Plane))
      mySurfaceType = GeomAbs_Plane;
    else if ( TheType == STANDARD_TYPE(Geom_CylindricalSurface))
      mySurfaceType = GeomAbs_Cylinder;
    else if ( TheType == STANDARD_TYPE(Geom_ConicalSurface))
      mySurfaceType = GeomAbs_Cone;
    else if ( TheType == STANDARD_TYPE(Geom_SphericalSurface))
      mySurfaceType = GeomAbs_Sphere;
    else if ( TheType == STANDARD_TYPE(Geom_ToroidalSurface))
      mySurfaceType = GeomAbs_Torus;
    else if ( TheType == STANDARD_TYPE(Geom_SurfaceOfRevolution))
      mySurfaceType = GeomAbs_SurfaceOfRevolution;
    else if ( TheType == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))
      mySurfaceType = GeomAbs_SurfaceOfExtrusion;
    else if ( TheType == STANDARD_TYPE(Geom_BSplineSurface)) {
      mySurfaceType = GeomAbs_BSplineSurface;
      myBspl        = Handle(Geom_BSplineSurface)::DownCast (mySurface);
      // Create cache for B-spline
      mySurfaceCache = new BSplSLib_Cache( 
        myBspl->UDegree(), myBspl->IsUPeriodic(), myBspl->UKnotSequence(), 
        myBspl->VDegree(), myBspl->IsVPeriodic(), myBspl->VKnotSequence(), 
        myBspl->Poles(), myBspl->Weights());
    }
    else if ( TheType == STANDARD_TYPE(Geom_OffsetSurface))
      mySurfaceType = GeomAbs_OffsetSurface;
    else
      mySurfaceType = GeomAbs_OtherSurface;
  }
}

//    --
//    --     Global methods - Apply to the whole Surface.
//    --     


//=======================================================================
//function : UContinuity
//purpose  : 
//=======================================================================

GeomAbs_Shape GeomAdaptor_Surface::UContinuity() const 
{
  switch (mySurfaceType)
  {
    case GeomAbs_BSplineSurface:
    {
      const Standard_Integer N = myBspl->NbUKnots();
      TColStd_Array1OfReal TK(1,N);
      TColStd_Array1OfInteger TM(1,N);
      myBspl->UKnots(TK);
      myBspl->UMultiplicities(TM);
      return LocalContinuity(myBspl->UDegree(), myBspl->NbUKnots(), TK, TM,
                             myUFirst, myULast, IsUPeriodic());
    }
    case GeomAbs_OffsetSurface:
    {
      switch(BasisSurface()->UContinuity())
      {
      case GeomAbs_CN :
      case GeomAbs_C3 : return GeomAbs_CN;
      case GeomAbs_G2 :
      case GeomAbs_C2 : return GeomAbs_C1;
      case GeomAbs_G1 :
      case GeomAbs_C1 : return GeomAbs_C0;
      case GeomAbs_C0 : break;
      }
      Standard_NoSuchObject::Raise("GeomAdaptor_Surface::UContinuity");
      break;
    }
    case GeomAbs_SurfaceOfExtrusion:
    {
      GeomAdaptor_Curve GC
        (Handle(Geom_SurfaceOfLinearExtrusion)::DownCast (mySurface)->BasisCurve(),myUFirst,myULast);
      return GC.Continuity();
    }
    case GeomAbs_OtherSurface: 
      Standard_NoSuchObject::Raise("GeomAdaptor_Surface::UContinuity");
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
    case GeomAbs_BezierSurface:
    case GeomAbs_SurfaceOfRevolution: break;
  }
  return GeomAbs_CN;
}

//=======================================================================
//function : VContinuity
//purpose  : 
//=======================================================================

GeomAbs_Shape GeomAdaptor_Surface::VContinuity() const 
{
  switch (mySurfaceType)
  {
    case GeomAbs_BSplineSurface:
    {
      const Standard_Integer N = myBspl->NbVKnots();
      TColStd_Array1OfReal TK(1,N);
      TColStd_Array1OfInteger TM(1,N);
      myBspl->VKnots(TK);
      myBspl->VMultiplicities(TM);
      return LocalContinuity(myBspl->VDegree(), myBspl->NbVKnots(), TK, TM,
                             myVFirst, myVLast, IsVPeriodic());
    }
    case GeomAbs_OffsetSurface:
    {
      switch(BasisSurface()->VContinuity())
      {
      case GeomAbs_CN : 
      case GeomAbs_C3 : return GeomAbs_CN; 
      case GeomAbs_G2 :
      case GeomAbs_C2 : return GeomAbs_C1;
      case GeomAbs_G1 :
      case GeomAbs_C1 : return GeomAbs_C0;
      case GeomAbs_C0 : break;
      }
      Standard_NoSuchObject::Raise("GeomAdaptor_Surface::VContinuity");
      break;
    }
    case GeomAbs_SurfaceOfRevolution:
    {
      GeomAdaptor_Curve GC
        (Handle(Geom_SurfaceOfRevolution)::DownCast (mySurface)->BasisCurve(),myVFirst,myVLast);
      return GC.Continuity();
    }
    case GeomAbs_OtherSurface:
      Standard_NoSuchObject::Raise("GeomAdaptor_Surface::VContinuity");
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
    case GeomAbs_BezierSurface:
    case GeomAbs_SurfaceOfExtrusion: break;
  }
  return GeomAbs_CN;
}

//=======================================================================
//function : NbUIntervals
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_Surface::NbUIntervals(const GeomAbs_Shape S) const
{
  switch (mySurfaceType)
  {
    case GeomAbs_BSplineSurface:
    {
      GeomAdaptor_Curve myBasisCurve
        (myBspl->VIso(myBspl->VKnot(myBspl->FirstVKnotIndex())),myUFirst,myULast);
      return myBasisCurve.NbIntervals(S);
    }
	  case GeomAbs_SurfaceOfExtrusion:
    {
      GeomAdaptor_Curve myBasisCurve
        (Handle(Geom_SurfaceOfLinearExtrusion)::DownCast (mySurface)->BasisCurve(),myUFirst,myULast);
	  if (myBasisCurve.GetType() == GeomAbs_BSplineCurve)
        return myBasisCurve.NbIntervals(S);
      break;
    }
	  case GeomAbs_OffsetSurface:
    {
      GeomAbs_Shape BaseS = GeomAbs_CN;
      switch(S)
      {
        case GeomAbs_G1:
        case GeomAbs_G2: Standard_DomainError::Raise("GeomAdaptor_Curve::NbUIntervals");
        case GeomAbs_C0: BaseS = GeomAbs_C1; break;
        case GeomAbs_C1: BaseS = GeomAbs_C2; break;
        case GeomAbs_C2: BaseS = GeomAbs_C3; break;
        case GeomAbs_C3:
        case GeomAbs_CN: break;
      }
      GeomAdaptor_Surface Sur(Handle(Geom_OffsetSurface)::DownCast (mySurface)->BasisSurface());
      return Sur.NbUIntervals(BaseS);
    }
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
    case GeomAbs_BezierSurface:
    case GeomAbs_OtherSurface:
    case GeomAbs_SurfaceOfRevolution: break;
  }
  return 1;
}

//=======================================================================
//function : NbVIntervals
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_Surface::NbVIntervals(const GeomAbs_Shape S) const
{
  switch (mySurfaceType)
  {
    case GeomAbs_BSplineSurface:
    {
      GeomAdaptor_Curve myBasisCurve
        (myBspl->UIso(myBspl->UKnot(myBspl->FirstUKnotIndex())),myVFirst,myVLast);
      return myBasisCurve.NbIntervals(S);
    }
	  case GeomAbs_SurfaceOfRevolution:
    {
      GeomAdaptor_Curve myBasisCurve
        (Handle(Geom_SurfaceOfRevolution)::DownCast (mySurface)->BasisCurve(),myVFirst,myVLast);
	  if (myBasisCurve.GetType() == GeomAbs_BSplineCurve)
        return myBasisCurve.NbIntervals(S);
      break;
    }
	  case GeomAbs_OffsetSurface:
    {
      GeomAbs_Shape BaseS = GeomAbs_CN;
      switch(S)
      {
        case GeomAbs_G1:
        case GeomAbs_G2: Standard_DomainError::Raise("GeomAdaptor_Curve::NbVIntervals");
        case GeomAbs_C0: BaseS = GeomAbs_C1; break;
        case GeomAbs_C1: BaseS = GeomAbs_C2; break;
        case GeomAbs_C2: BaseS = GeomAbs_C3; break;
        case GeomAbs_C3:
        case GeomAbs_CN: break;
      }
      GeomAdaptor_Surface Sur(Handle(Geom_OffsetSurface)::DownCast (mySurface)->BasisSurface());
      return Sur.NbVIntervals(BaseS);
	  }
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
    case GeomAbs_BezierSurface:
    case GeomAbs_OtherSurface:
    case GeomAbs_SurfaceOfExtrusion: break;
  }
  return 1;
}

//=======================================================================
//function : UIntervals
//purpose  : 
//=======================================================================

void GeomAdaptor_Surface::UIntervals(TColStd_Array1OfReal& T, const GeomAbs_Shape S) const
{
  Standard_Integer myNbUIntervals = 1;

  switch (mySurfaceType)
  {
    case GeomAbs_BSplineSurface:
    {
      GeomAdaptor_Curve myBasisCurve
        (myBspl->VIso(myBspl->VKnot(myBspl->FirstVKnotIndex())),myUFirst,myULast);
      myNbUIntervals = myBasisCurve.NbIntervals(S);
      myBasisCurve.Intervals(T,S);
      break;
    }
	  case GeomAbs_SurfaceOfExtrusion:
    {
      GeomAdaptor_Curve myBasisCurve
        (Handle(Geom_SurfaceOfLinearExtrusion)::DownCast (mySurface)->BasisCurve(),myUFirst,myULast);
      if (myBasisCurve.GetType() == GeomAbs_BSplineCurve)
      {
        myNbUIntervals = myBasisCurve.NbIntervals(S);
        myBasisCurve.Intervals(T,S);
      }
      break;
    }
	  case GeomAbs_OffsetSurface:
    {
      GeomAbs_Shape BaseS = GeomAbs_CN;
      switch(S)
      {
        case GeomAbs_G1:
        case GeomAbs_G2: Standard_DomainError::Raise("GeomAdaptor_Curve::UIntervals");
        case GeomAbs_C0: BaseS = GeomAbs_C1; break;
        case GeomAbs_C1: BaseS = GeomAbs_C2; break;
        case GeomAbs_C2: BaseS = GeomAbs_C3; break;
        case GeomAbs_C3:
        case GeomAbs_CN: break;
      }
      GeomAdaptor_Surface Sur(Handle(Geom_OffsetSurface)::DownCast (mySurface)->BasisSurface());
      myNbUIntervals = Sur.NbUIntervals(BaseS);
      Sur.UIntervals(T, BaseS);
    }
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
    case GeomAbs_BezierSurface:
    case GeomAbs_OtherSurface:
    case GeomAbs_SurfaceOfRevolution: break;
  }

  T(T.Lower()) = myUFirst;
  T(T.Lower() + myNbUIntervals) = myULast;
}

//=======================================================================
//function : VIntervals
//purpose  : 
//=======================================================================

void GeomAdaptor_Surface::VIntervals(TColStd_Array1OfReal& T, const GeomAbs_Shape S) const
{
  Standard_Integer myNbVIntervals = 1;

  switch (mySurfaceType)
  {
    case GeomAbs_BSplineSurface:
    {
      GeomAdaptor_Curve myBasisCurve
        (myBspl->UIso(myBspl->UKnot(myBspl->FirstUKnotIndex())),myVFirst,myVLast);
      myNbVIntervals = myBasisCurve.NbIntervals(S);
      myBasisCurve.Intervals(T,S);
      break;
    }
	  case GeomAbs_SurfaceOfRevolution:
    {
      GeomAdaptor_Curve myBasisCurve
        (Handle(Geom_SurfaceOfRevolution)::DownCast (mySurface)->BasisCurve(),myVFirst,myVLast);
      if (myBasisCurve.GetType() == GeomAbs_BSplineCurve)
      {
        myNbVIntervals = myBasisCurve.NbIntervals(S);
        myBasisCurve.Intervals(T,S);
      }
      break;
    }
	  case GeomAbs_OffsetSurface:
    {
      GeomAbs_Shape BaseS = GeomAbs_CN;
      switch(S)
      {
        case GeomAbs_G1:
        case GeomAbs_G2: Standard_DomainError::Raise("GeomAdaptor_Curve::VIntervals");
        case GeomAbs_C0: BaseS = GeomAbs_C1; break;
        case GeomAbs_C1: BaseS = GeomAbs_C2; break;
        case GeomAbs_C2: BaseS = GeomAbs_C3; break;
        case GeomAbs_C3:
        case GeomAbs_CN: break;
      }
      GeomAdaptor_Surface Sur(Handle(Geom_OffsetSurface)::DownCast (mySurface)->BasisSurface());
      myNbVIntervals = Sur.NbVIntervals(BaseS);
      Sur.VIntervals(T, BaseS);
    }
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
    case GeomAbs_BezierSurface:
    case GeomAbs_OtherSurface:
    case GeomAbs_SurfaceOfExtrusion: break;
  }
  
  T(T.Lower()) = myVFirst;
  T(T.Lower() + myNbVIntervals) = myVLast;
}

//=======================================================================
//function : UTrim
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HSurface) GeomAdaptor_Surface::UTrim(const Standard_Real First,
                                                      const Standard_Real Last ,
                                                      const Standard_Real Tol   ) const
{
  return Handle(GeomAdaptor_HSurface)
    (new GeomAdaptor_HSurface(mySurface,First,Last,myVFirst,myVLast,Tol,myTolV));
}

//=======================================================================
//function : VTrim
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HSurface) GeomAdaptor_Surface::VTrim(const Standard_Real First,
                                                      const Standard_Real Last ,
                                                      const Standard_Real Tol   ) const
{
  return Handle(GeomAdaptor_HSurface)
    (new GeomAdaptor_HSurface(mySurface,myUFirst,myULast,First,Last,myTolU,Tol));
}

//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================

Standard_Boolean GeomAdaptor_Surface::IsUClosed() const 
{
  if (!mySurface->IsUClosed())
    return Standard_False;

  Standard_Real U1,U2,V1,V2;
  mySurface->Bounds(U1,U2,V1,V2);
  if (mySurface->IsUPeriodic())
    return (Abs(Abs(U1-U2)-Abs(myUFirst-myULast))<Precision::PConfusion());

  return (   Abs(U1-myUFirst)<Precision::PConfusion() 
          && Abs(U2-myULast )<Precision::PConfusion() );
}

//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================

Standard_Boolean GeomAdaptor_Surface::IsVClosed() const 
{
  if (!mySurface->IsVClosed())
    return Standard_False;

  Standard_Real U1,U2,V1,V2;
  mySurface->Bounds(U1,U2,V1,V2);
  if (mySurface->IsVPeriodic())
    return (Abs(Abs(V1-V2)-Abs(myVFirst-myVLast))<Precision::PConfusion());

  return (   Abs(V1-myVFirst)<Precision::PConfusion() 
          && Abs(V2-myVLast )<Precision::PConfusion() );
}

//=======================================================================
//function : IsUPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean GeomAdaptor_Surface::IsUPeriodic() const 
{
  return (mySurface->IsUPeriodic());
}

//=======================================================================
//function : UPeriod
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_Surface::UPeriod() const 
{
  Standard_NoSuchObject_Raise_if(!IsUPeriodic()," ");
  return mySurface->UPeriod();
}

//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean GeomAdaptor_Surface::IsVPeriodic() const 
{
  return (mySurface->IsVPeriodic());
}

//=======================================================================
//function : VPeriod
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_Surface::VPeriod() const 
{
  Standard_NoSuchObject_Raise_if(!IsVPeriodic()," ");
  return mySurface->VPeriod();
}

//=======================================================================
//function : RebuildCache
//purpose  : 
//=======================================================================
void GeomAdaptor_Surface::RebuildCache(const Standard_Real theU,
                                       const Standard_Real theV) const
{
  mySurfaceCache->BuildCache(theU, theV, 
    myBspl->UDegree(), myBspl->IsUPeriodic(), myBspl->UKnotSequence(), 
    myBspl->VDegree(), myBspl->IsVPeriodic(), myBspl->VKnotSequence(), 
    myBspl->Poles(), myBspl->Weights());
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt GeomAdaptor_Surface::Value(const Standard_Real U, 
                                  const Standard_Real V) const 
{
  if (mySurfaceType == GeomAbs_BSplineSurface && !mySurfaceCache.IsNull())
  {
    if (!mySurfaceCache->IsCacheValid(U, V))
      RebuildCache(U, V);
    gp_Pnt P;
    mySurfaceCache->D0(U, V, P);
    return P;
  }

  return mySurface->Value(U,V);
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void GeomAdaptor_Surface::D0(const Standard_Real U, 
                             const Standard_Real V, gp_Pnt& P) const 
{
  if (mySurfaceType == GeomAbs_BSplineSurface && !mySurfaceCache.IsNull())
  {
    if (!mySurfaceCache->IsCacheValid(U, V))
      RebuildCache(U, V);
    mySurfaceCache->D0(U, V, P);
    return;
  }

  mySurface->D0(U,V,P);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void GeomAdaptor_Surface::D1(const Standard_Real U, 
                             const Standard_Real V, 
                                   gp_Pnt&       P,
                                   gp_Vec&       D1U, 
                                   gp_Vec&       D1V ) const 
{
  Standard_Integer Ideb, Ifin, IVdeb, IVfin, USide=0, VSide=0;
  Standard_Real u = U, v = V;
  if (Abs(U-myUFirst) <= myTolU) {USide= 1; u = myUFirst;}
  else if (Abs(U-myULast) <= myTolU) {USide= -1; u = myULast;}
  if (Abs(V-myVFirst) <= myTolV) {VSide= 1; v = myVFirst;}
  else if (Abs(V-myVLast) <= myTolV) {VSide= -1; v = myVLast;}

  switch(mySurfaceType) {
  case GeomAbs_BSplineSurface:
    if ((USide != 0 || VSide != 0) && 
        IfUVBound(u, v, Ideb, Ifin, IVdeb, IVfin, USide, VSide))
      myBspl->LocalD1(u, v, Ideb, Ifin, IVdeb, IVfin, P, D1U, D1V);
    else if (!mySurfaceCache.IsNull())
    {
      if (!mySurfaceCache->IsCacheValid(U, V))
        RebuildCache(U, V);
      mySurfaceCache->D1(U, V, P, D1U, D1V);
    }
    else
      myBspl->D1(u, v, P, D1U, D1V);
    break;

  case GeomAbs_SurfaceOfExtrusion:
    if (USide==0) 
      myExtSurf->D1(u, v, P, D1U, D1V);
    else 
      myExtSurf->LocalD1(u, v, USide, P, D1U, D1V);
    break;

  case GeomAbs_SurfaceOfRevolution:
    if (VSide==0) 
      myRevSurf->D1(u, v, P, D1U, D1V);
    else 
      myRevSurf->LocalD1(u, v, VSide, P, D1U, D1V);
    break;

  case GeomAbs_OffsetSurface:
    if (USide==0 && VSide==0) 
      myOffSurf->D1(u, v, P, D1U, D1V);
    else 
      myOffSurf->LocalD1(u, v, USide, VSide, P, D1U, D1V);
    break;
  default:
    mySurface->D1(u, v, P, D1U, D1V);
  }
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void GeomAdaptor_Surface::D2(const Standard_Real U,
                             const Standard_Real V, 
                                   gp_Pnt&       P,
                                   gp_Vec&       D1U, 
                                   gp_Vec&       D1V, 
                                   gp_Vec&       D2U,
                                   gp_Vec&       D2V, 
                                   gp_Vec&       D2UV) const 
{ 
  Standard_Integer Ideb, Ifin, IVdeb, IVfin, USide=0, VSide=0;
  Standard_Real u = U, v = V;
  if (Abs(U-myUFirst) <= myTolU) {USide= 1; u = myUFirst;}
  else if (Abs(U-myULast) <= myTolU) {USide= -1; u = myULast;}
  if (Abs(V-myVFirst) <= myTolV) {VSide= 1; v = myVFirst;}
  else if (Abs(V-myVLast) <= myTolV) {VSide= -1; v = myVLast;}

  switch(mySurfaceType) {
  case  GeomAbs_BSplineSurface:
    if((USide != 0 || VSide != 0) && 
       IfUVBound(u, v, Ideb, Ifin, IVdeb, IVfin, USide, VSide))
      myBspl->LocalD2(u, v, Ideb, Ifin, IVdeb, IVfin, P, D1U, D1V, D2U, D2V, D2UV);
    else if (!mySurfaceCache.IsNull())
    {
      if (!mySurfaceCache->IsCacheValid(U, V))
        RebuildCache(U, V);
      mySurfaceCache->D2(U, V, P, D1U, D1V, D2U, D2V, D2UV);
    }
    else myBspl->D2(u,v,P,D1U,D1V,D2U,D2V,D2UV);
    break;

  case GeomAbs_SurfaceOfExtrusion  :

    if(USide==0)  myExtSurf->D2(u,v,P,D1U,D1V,D2U,D2V,D2UV);
    else myExtSurf->LocalD2(u,v,USide,P,D1U,D1V,D2U,D2V,D2UV);
    break;

  case GeomAbs_SurfaceOfRevolution :

    if(VSide==0) myRevSurf->D2 (u, v, P,D1U,D1V,D2U,D2V,D2UV );
    else myRevSurf->LocalD2 (u, v, VSide, P,D1U,D1V,D2U,D2V,D2UV );
    break;

  case  GeomAbs_OffsetSurface :
    {
      if((USide==0)&&(VSide==0)) myOffSurf->D2 (u, v,P,D1U,D1V,D2U,D2V,D2UV ); 
      else myOffSurf->LocalD2 (u, v, USide, VSide ,P,D1U,D1V,D2U,D2V,D2UV ); 
      break;
    }
  default :  { mySurface->D2(u,v,P,D1U,D1V,D2U,D2V,D2UV);
    break;}
  }
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void GeomAdaptor_Surface::D3(const Standard_Real U, const Standard_Real V,
                             gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V,
                             gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV,
                             gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV,
                             gp_Vec& D3UVV) const
{  
  Standard_Integer Ideb,Ifin,IVdeb,IVfin,USide=0,VSide=0;
  Standard_Real u = U, v = V;
  if (Abs(U-myUFirst) <= myTolU) {USide= 1; u = myUFirst;}
  else if (Abs(U-myULast) <= myTolU) {USide= -1; u = myULast;}
  if (Abs(V-myVFirst) <= myTolV) {VSide= 1; v = myVFirst;}
  else if (Abs(V-myVLast) <= myTolV) {VSide= -1; v = myVLast;}

  switch(mySurfaceType) {
  case  GeomAbs_BSplineSurface:
    
      if((USide==0)&&(VSide==0))
	myBspl->D3(u,v,P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV);
      else {
	if(IfUVBound(u,v,Ideb,Ifin,IVdeb,IVfin,USide,VSide))
	  myBspl-> LocalD3 (u, v, Ideb, Ifin,IVdeb ,IVfin ,
			    P ,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV); 
	else
	  myBspl->D3(u,v,P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV);
      }
      break;
    
  case GeomAbs_SurfaceOfExtrusion  :
    
      if(USide==0)  myExtSurf->D3(u,v,P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV);
      else myExtSurf->LocalD3(u,v,USide,P,D1U,D1V,D2U,D2V,D2UV,
			      D3U,D3V,D3UUV,D3UVV);
      break;
    
  case GeomAbs_SurfaceOfRevolution :
    
      if(VSide==0) myRevSurf->D3 (u, v, P ,D1U,D1V,D2U,D2V,D2UV,
				  D3U,D3V,D3UUV,D3UVV);
      else myRevSurf->LocalD3 (u, v, VSide, P,D1U,D1V,D2U,D2V,D2UV,
			       D3U,D3V,D3UUV,D3UVV );
      break;
    
  case  GeomAbs_OffsetSurface : 
    {
      if((USide==0)&&(VSide==0)) myOffSurf->D3 (u, v,P ,D1U,D1V,D2U,D2V,D2UV,
						D3U,D3V,D3UUV,D3UVV); 
      else   myOffSurf->LocalD3 (u, v, USide, VSide ,P ,D1U,D1V,D2U,D2V,D2UV,
				 D3U,D3V,D3UUV,D3UVV); 
      break; 
    }
    default : {  mySurface->D3(u,v,P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV); 
		 break;}
  }
}

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec GeomAdaptor_Surface::DN(const Standard_Real    U,
                               const Standard_Real    V, 
                               const Standard_Integer Nu,
                               const Standard_Integer Nv) const 
{
  Standard_Integer Ideb,Ifin,IVdeb,IVfin,USide=0,VSide=0;
  Standard_Real u = U, v = V;
  if (Abs(U-myUFirst) <= myTolU) {USide= 1; u = myUFirst;}
  else if (Abs(U-myULast) <= myTolU) {USide= -1; u = myULast;}
  if (Abs(V-myVFirst) <= myTolV) {VSide= 1; v = myVFirst;}
  else if (Abs(V-myVLast) <= myTolV) {VSide= -1; v = myVLast;}

  switch(mySurfaceType) 
  {
  case GeomAbs_BSplineSurface:
    
    if((USide==0)&&(VSide==0)) return  myBspl->DN(u,v,Nu,Nv);
    else {
      if(IfUVBound(u,v,Ideb,Ifin,IVdeb,IVfin,USide,VSide))
        return myBspl->LocalDN (u, v, Ideb, Ifin,IVdeb ,IVfin ,Nu,Nv ); 
      else
        return  myBspl->DN(u,v,Nu,Nv); 
    }
    
  case GeomAbs_SurfaceOfExtrusion:
    
    if(USide==0)  return myExtSurf-> DN (u, v,Nu,Nv );
    else return myExtSurf->LocalDN (u, v, USide,Nu,Nv );
    
  case GeomAbs_SurfaceOfRevolution:

    if(VSide==0)  return myRevSurf->DN (u, v, Nu, Nv );
    else  return myRevSurf->LocalDN (u, v,VSide, Nu, Nv );
     
  case GeomAbs_OffsetSurface:
    
    if((USide==0)&&(VSide==0)) return myOffSurf->DN (u, v, Nu, Nv );
    else return myOffSurf->LocalDN (u, v, USide, VSide, Nu, Nv );

  case GeomAbs_Plane:
  case GeomAbs_Cylinder:
  case GeomAbs_Cone:
  case GeomAbs_Sphere:
  case GeomAbs_Torus:
  case GeomAbs_BezierSurface:
  case GeomAbs_OtherSurface:
  default:
    break;
  }
  
  return mySurface->DN(u,v, Nu, Nv);
}


//=======================================================================
//function : UResolution
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_Surface::UResolution(const Standard_Real R3d) const
{
  Standard_Real Res = 0.;

  switch (mySurfaceType)
  {
    case GeomAbs_SurfaceOfExtrusion:
    {
      GeomAdaptor_Curve myBasisCurve
        (Handle(Geom_SurfaceOfLinearExtrusion)::DownCast (mySurface)->BasisCurve(),myUFirst,myULast);
      return myBasisCurve.Resolution(R3d);
    }
	case GeomAbs_Torus:
    {
      Handle(Geom_ToroidalSurface) S (Handle(Geom_ToroidalSurface)::DownCast (mySurface));
      const Standard_Real R = S->MajorRadius() + S->MinorRadius();
      if(R>Precision::Confusion())
        Res = R3d/(2.*R);
	  break;
    }
	case GeomAbs_Sphere:
    {
      Handle(Geom_SphericalSurface) S (Handle(Geom_SphericalSurface)::DownCast (mySurface));
      const Standard_Real R = S->Radius();
      if(R>Precision::Confusion())
        Res = R3d/(2.*R);
	  break;
    }
	case GeomAbs_Cylinder:
    {
      Handle(Geom_CylindricalSurface) S (Handle(Geom_CylindricalSurface)::DownCast (mySurface));
      const Standard_Real R = S->Radius();
      if(R>Precision::Confusion())
        Res = R3d/(2.*R);
	  break;
    }
	case GeomAbs_Cone:
    {
      if (myVLast - myVFirst > 1.e10) {
        // Pas vraiment borne => resolution inconnue
        return Precision::Parametric(R3d);
      }
      Handle(Geom_ConicalSurface) S (Handle(Geom_ConicalSurface)::DownCast (mySurface));
      Handle(Geom_Curve) C = S->VIso(myVLast);
      const Standard_Real Rayon1 = Handle(Geom_Circle)::DownCast (C)->Radius();
      C = S->VIso(myVFirst);
      const Standard_Real Rayon2 = Handle(Geom_Circle)::DownCast (C)->Radius();
	  const Standard_Real R = (Rayon1 > Rayon2)? Rayon1 : Rayon2;
	  return (R>Precision::Confusion()? (R3d / R) : 0.);
    }
	case GeomAbs_Plane:
    {
      return R3d;
    }
	case GeomAbs_BezierSurface:
    {
      Standard_Real Ures,Vres;
      Handle(Geom_BezierSurface)::DownCast (mySurface)->Resolution(R3d,Ures,Vres);
      return Ures;
    }
	case GeomAbs_BSplineSurface:
    {
      Standard_Real Ures,Vres;
      Handle(Geom_BSplineSurface)::DownCast (mySurface)->Resolution(R3d,Ures,Vres);
      return Ures;
    }
	case GeomAbs_OffsetSurface:
    {
      Handle(Geom_Surface) base = Handle(Geom_OffsetSurface)::DownCast (mySurface)->BasisSurface();
      GeomAdaptor_Surface gabase(base,myUFirst,myULast,myVFirst,myVLast);
      return gabase.UResolution(R3d);
    }
	default: return Precision::Parametric(R3d);
  }

  if ( Res <= 1.)  
    return 2.*ASin(Res);
  
  return 2.*M_PI;
}

//=======================================================================
//function : VResolution
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_Surface::VResolution(const Standard_Real R3d) const
{
  Standard_Real Res = 0.;

  switch (mySurfaceType)
  {
    case GeomAbs_SurfaceOfRevolution:
    {
      GeomAdaptor_Curve myBasisCurve
        (Handle(Geom_SurfaceOfRevolution)::DownCast (mySurface)->BasisCurve(),myUFirst,myULast);
      return myBasisCurve.Resolution(R3d);
    }
	case GeomAbs_Torus:
    {
      Handle(Geom_ToroidalSurface) S (Handle(Geom_ToroidalSurface)::DownCast (mySurface));
      const Standard_Real R = S->MinorRadius();
      if(R>Precision::Confusion())
        Res = R3d/(2.*R);
      break;
    }
	case GeomAbs_Sphere:
    {
      Handle(Geom_SphericalSurface) S (Handle(Geom_SphericalSurface)::DownCast (mySurface));
      const Standard_Real R = S->Radius();
      if(R>Precision::Confusion())
        Res = R3d/(2.*R);
      break;
    }
	case GeomAbs_SurfaceOfExtrusion:
	case GeomAbs_Cylinder:
	case GeomAbs_Cone:
	case GeomAbs_Plane:
    {
      return R3d;
    }
	case GeomAbs_BezierSurface:
    {
      Standard_Real Ures,Vres;
      Handle(Geom_BezierSurface)::DownCast (mySurface)->Resolution(R3d,Ures,Vres);
      return Vres;
    }
	case GeomAbs_BSplineSurface:
    {
      Standard_Real Ures,Vres;
      Handle(Geom_BSplineSurface)::DownCast (mySurface)->Resolution(R3d,Ures,Vres);
      return Vres;
    }
	case GeomAbs_OffsetSurface:
    {
      Handle(Geom_Surface) base = Handle(Geom_OffsetSurface)::DownCast (mySurface)->BasisSurface();
      GeomAdaptor_Surface gabase(base,myUFirst,myULast,myVFirst,myVLast);
      return gabase.VResolution(R3d);
    }
	default: return Precision::Parametric(R3d);
  }

  if ( Res <= 1.) 
    return 2.*ASin(Res);

  return 2.*M_PI;
}

//=======================================================================
//function : Plane
//purpose  : 
//=======================================================================

gp_Pln GeomAdaptor_Surface::Plane() const 
{
  if (mySurfaceType != GeomAbs_Plane)
    Standard_NoSuchObject::Raise("GeomAdaptor_Surface::Plane");
  return Handle(Geom_Plane)::DownCast (mySurface)->Pln();
}

//=======================================================================
//function : Cylinder
//purpose  : 
//=======================================================================

gp_Cylinder GeomAdaptor_Surface::Cylinder() const 
{
  if (mySurfaceType != GeomAbs_Cylinder)
    Standard_NoSuchObject::Raise("GeomAdaptor_Surface::Cylinder");
  return Handle(Geom_CylindricalSurface)::DownCast (mySurface)->Cylinder();
}

//=======================================================================
//function : Cone
//purpose  : 
//=======================================================================

gp_Cone GeomAdaptor_Surface::Cone() const 
{
  if (mySurfaceType != GeomAbs_Cone)
    Standard_NoSuchObject::Raise("GeomAdaptor_Surface::Cone");
  return Handle(Geom_ConicalSurface)::DownCast (mySurface)->Cone();
}

//=======================================================================
//function : Sphere
//purpose  : 
//=======================================================================

gp_Sphere GeomAdaptor_Surface::Sphere() const 
{
  if (mySurfaceType != GeomAbs_Sphere)
    Standard_NoSuchObject::Raise("GeomAdaptor_Surface::Sphere");
  return Handle(Geom_SphericalSurface)::DownCast (mySurface)->Sphere();
}

//=======================================================================
//function : Torus
//purpose  : 
//=======================================================================

gp_Torus GeomAdaptor_Surface::Torus() const 
{
  if (mySurfaceType != GeomAbs_Torus)
    Standard_NoSuchObject::Raise("GeomAdaptor_Surface::Torus");
  return Handle(Geom_ToroidalSurface)::DownCast (mySurface)->Torus(); 
}

//=======================================================================
//function : UDegree
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_Surface::UDegree() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return Handle(Geom_BSplineSurface)::DownCast (mySurface)->UDegree(); 
  if ( mySurfaceType == GeomAbs_BezierSurface)
    return Handle(Geom_BezierSurface)::DownCast (mySurface)->UDegree(); 
  if ( mySurfaceType == GeomAbs_SurfaceOfExtrusion)
  {
    GeomAdaptor_Curve myBasisCurve
      (Handle(Geom_SurfaceOfLinearExtrusion)::DownCast (mySurface)->BasisCurve(),myUFirst,myULast);
    return myBasisCurve.Degree();
  }
  Standard_NoSuchObject::Raise("GeomAdaptor_Surface::UDegree");
  return 0;
}

//=======================================================================
//function : NbUPoles
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_Surface::NbUPoles() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return Handle(Geom_BSplineSurface)::DownCast (mySurface)->NbUPoles();
  if ( mySurfaceType == GeomAbs_BezierSurface)
    return Handle(Geom_BezierSurface)::DownCast (mySurface)->NbUPoles(); 
  if ( mySurfaceType == GeomAbs_SurfaceOfExtrusion)
  {
    GeomAdaptor_Curve myBasisCurve
      (Handle(Geom_SurfaceOfLinearExtrusion)::DownCast (mySurface)->BasisCurve(),myUFirst,myULast);
    return myBasisCurve.NbPoles();
  }
  Standard_NoSuchObject::Raise("GeomAdaptor_Surface::NbUPoles");
  return 0;
}

//=======================================================================
//function : VDegree
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_Surface::VDegree() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return Handle(Geom_BSplineSurface)::DownCast (mySurface)->VDegree(); 
  if ( mySurfaceType == GeomAbs_BezierSurface)
    return Handle(Geom_BezierSurface)::DownCast (mySurface)->VDegree(); 
  if ( mySurfaceType == GeomAbs_SurfaceOfRevolution)
  {
    GeomAdaptor_Curve myBasisCurve
      (Handle(Geom_SurfaceOfRevolution)::DownCast (mySurface)->BasisCurve(),myUFirst,myULast);
    return myBasisCurve.Degree();
  }
  Standard_NoSuchObject::Raise("GeomAdaptor_Surface::VDegree");
  return 0;
}

//=======================================================================
//function : NbVPoles
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_Surface::NbVPoles() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return Handle(Geom_BSplineSurface)::DownCast (mySurface)->NbVPoles(); 
  if ( mySurfaceType == GeomAbs_BezierSurface)
    return Handle(Geom_BezierSurface)::DownCast (mySurface)->NbVPoles(); 
  if ( mySurfaceType == GeomAbs_SurfaceOfRevolution)
  {
    GeomAdaptor_Curve myBasisCurve
      (Handle(Geom_SurfaceOfRevolution)::DownCast (mySurface)->BasisCurve(),myUFirst,myULast);
    return myBasisCurve.NbPoles();
  }
  Standard_NoSuchObject::Raise("GeomAdaptor_Surface::NbVPoles");
  return 0;
}

//=======================================================================
//function : NbUKnots
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_Surface::NbUKnots() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return Handle(Geom_BSplineSurface)::DownCast (mySurface)->NbUKnots(); 
  if ( mySurfaceType == GeomAbs_SurfaceOfExtrusion)
  {
    GeomAdaptor_Curve myBasisCurve
      (Handle(Geom_SurfaceOfLinearExtrusion)::DownCast (mySurface)->BasisCurve(),myUFirst,myULast);
    return myBasisCurve.NbKnots();
  }
  Standard_NoSuchObject::Raise("GeomAdaptor_Surface::NbUKnots");
  return 0;
}

//=======================================================================
//function : NbVKnots
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_Surface::NbVKnots() const 
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return Handle(Geom_BSplineSurface)::DownCast (mySurface)->NbVKnots(); 
  Standard_NoSuchObject::Raise("GeomAdaptor_Surface::NbVKnots");
  return 0;
}
//=======================================================================
//function : IsURational
//purpose  : 
//=======================================================================

Standard_Boolean GeomAdaptor_Surface::IsURational() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return Handle(Geom_BSplineSurface)::DownCast (mySurface)->IsURational(); 
  if (mySurfaceType == GeomAbs_BezierSurface)
    return Handle(Geom_BezierSurface)::DownCast (mySurface)->IsURational(); 
  return Standard_False;
}

//=======================================================================
//function : IsVRational
//purpose  : 
//=======================================================================

Standard_Boolean GeomAdaptor_Surface::IsVRational() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return Handle(Geom_BSplineSurface)::DownCast (mySurface)->IsVRational(); 
  if (mySurfaceType == GeomAbs_BezierSurface)
    return Handle(Geom_BezierSurface)::DownCast (mySurface)->IsVRational(); 
  return Standard_False;
}

//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================

Handle(Geom_BezierSurface) GeomAdaptor_Surface::Bezier() const 
{
  if (mySurfaceType != GeomAbs_BezierSurface)
    Standard_NoSuchObject::Raise("GeomAdaptor_Surface::Bezier");
  return Handle(Geom_BezierSurface)::DownCast (mySurface);
}

//=======================================================================
//function : BSpline
//purpose  : 
//=======================================================================

Handle(Geom_BSplineSurface) GeomAdaptor_Surface::BSpline() const 
{
  if (mySurfaceType != GeomAbs_BSplineSurface)  
    Standard_NoSuchObject::Raise("GeomAdaptor_Surface::BSpline");
  return Handle(Geom_BSplineSurface)::DownCast (mySurface);
}

//=======================================================================
//function : AxeOfRevolution
//purpose  : 
//=======================================================================

gp_Ax1 GeomAdaptor_Surface::AxeOfRevolution() const 
{
  if (mySurfaceType != GeomAbs_SurfaceOfRevolution)
    Standard_NoSuchObject::Raise("GeomAdaptor_Surface::AxeOfRevolution");
  return Handle(Geom_SurfaceOfRevolution)::DownCast (mySurface)->Axis();
}

//=======================================================================
//function : Direction
//purpose  : 
//=======================================================================

gp_Dir GeomAdaptor_Surface::Direction() const 
{
  if (mySurfaceType != GeomAbs_SurfaceOfExtrusion)
    Standard_NoSuchObject::Raise("GeomAdaptor_Surface::Direction");
  return Handle(Geom_SurfaceOfLinearExtrusion)::DownCast (mySurface)->Direction();
}

//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HCurve) GeomAdaptor_Surface::BasisCurve() const 
{
  Handle(Geom_Curve) C;
  if (mySurfaceType == GeomAbs_SurfaceOfExtrusion)
    C = Handle(Geom_SurfaceOfLinearExtrusion)::DownCast (mySurface)->BasisCurve();
  else if (mySurfaceType == GeomAbs_SurfaceOfRevolution)
    C = Handle(Geom_SurfaceOfRevolution)::DownCast (mySurface)->BasisCurve();
  else
    Standard_NoSuchObject::Raise("GeomAdaptor_Surface::BasisCurve");
  return Handle(GeomAdaptor_HCurve)(new GeomAdaptor_HCurve(C));
}

//=======================================================================
//function : BasisSurface
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HSurface) GeomAdaptor_Surface::BasisSurface() const 
{
  if (mySurfaceType != GeomAbs_OffsetSurface) 
    Standard_NoSuchObject::Raise("GeomAdaptor_Surface::BasisSurface");
  return new GeomAdaptor_HSurface
    (Handle(Geom_OffsetSurface)::DownCast (mySurface)->BasisSurface(),
     myUFirst,myULast,myVFirst,myVLast);
}

//=======================================================================
//function : OffsetValue
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_Surface::OffsetValue() const 
{
  if (mySurfaceType != GeomAbs_OffsetSurface) 
    Standard_NoSuchObject::Raise("GeomAdaptor_Surface::BasisSurface");
  return Handle(Geom_OffsetSurface)::DownCast (mySurface)->Offset();
}

//=======================================================================
//function : IfUVBound <private>
//purpose  :  locates U,V parameters if U,V =First, Last, 
//	      processes the finding span and returns the 
//	      parameters for LocalDi     
//=======================================================================

Standard_Boolean GeomAdaptor_Surface::IfUVBound(const Standard_Real U,
                                                const Standard_Real V,
                                                Standard_Integer& IOutDeb,
                                                Standard_Integer& IOutFin,
                                                Standard_Integer& IOutVDeb,
                                                Standard_Integer& IOutVFin,
                                                const Standard_Integer USide,
                                                const Standard_Integer VSide) const
{
  Standard_Integer Ideb,Ifin;
  Standard_Integer anUFKIndx = myBspl->FirstUKnotIndex(), 
    anULKIndx = myBspl->LastUKnotIndex(), 
    aVFKIndx = myBspl->FirstVKnotIndex(), aVLKIndx = myBspl->LastVKnotIndex();
  myBspl->LocateU(U, PosTol, Ideb, Ifin, Standard_False);
  Standard_Boolean Local = (Ideb == Ifin);
  Span(USide,Ideb,Ifin,Ideb,Ifin,anUFKIndx,anULKIndx);
  Standard_Integer IVdeb,IVfin;
  myBspl->LocateV(V, PosTol, IVdeb, IVfin, Standard_False); 
  if(IVdeb == IVfin) Local = Standard_True;
  Span(VSide,IVdeb,IVfin,IVdeb,IVfin,aVFKIndx,aVLKIndx);

  IOutDeb=Ideb;   IOutFin=Ifin; 
  IOutVDeb=IVdeb; IOutVFin=IVfin;

  return Local;
}     
//=======================================================================
//function : Span <private>
//purpose  : locates U,V parameters if U=UFirst or U=ULast, 
//	     processes the finding span and returns the 
//	     parameters for LocalDi   
//=======================================================================

void GeomAdaptor_Surface::Span(const Standard_Integer Side,
                               const Standard_Integer Ideb,
                               const Standard_Integer Ifin,
                               Standard_Integer& OutIdeb,
                               Standard_Integer& OutIfin,
                               const Standard_Integer theFKIndx,
                               const Standard_Integer theLKIndx) const
{
  if(Ideb!=Ifin)//not a knot
  { 
    if(Ideb<theFKIndx)                 { OutIdeb=theFKIndx; OutIfin=theFKIndx+1; }
	else if(Ifin>theLKIndx)      { OutIdeb=theLKIndx-1; OutIfin=theLKIndx; }
	else if(Ideb>=(theLKIndx-1)) { OutIdeb=theLKIndx-1; OutIfin=theLKIndx; }
	else if(Ifin<=theFKIndx+1)           { OutIdeb=theFKIndx; OutIfin=theFKIndx+1; }
	else if(Ideb>Ifin)         { OutIdeb=Ifin-1;   OutIfin=Ifin; }
	else                       { OutIdeb=Ideb;   OutIfin=Ifin; }
  }
  else
  {
    if(Ideb<=theFKIndx){ OutIdeb=theFKIndx;   OutIfin=theFKIndx+1;}//first knot
    else if(Ifin>=theLKIndx) { OutIdeb=theLKIndx-1;OutIfin=theLKIndx;}//last knot
    else
    {
	  if(Side==-1){OutIdeb=Ideb-1;   OutIfin=Ifin;}
	  else {OutIdeb=Ideb;   OutIfin=Ifin+1;}
    } 
  }
}
