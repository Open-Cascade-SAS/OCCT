// Created on: 1993-03-03
// Created by: Philippe DAUTRY
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <PGeom_BSplineSurface.ixx>

//=======================================================================
//function : PGeom_BSplineSurface
//purpose  : 
//=======================================================================

PGeom_BSplineSurface::PGeom_BSplineSurface()
{}


//=======================================================================
//function : PGeom_BSplineSurface
//purpose  : 
//=======================================================================

PGeom_BSplineSurface::PGeom_BSplineSurface
  (const Standard_Boolean aURational,
   const Standard_Boolean aVRational,
   const Standard_Boolean aUPeriodic,
   const Standard_Boolean aVPeriodic,
   const Standard_Integer aUSpineDegree,
   const Standard_Integer aVSpineDegree,
   const Handle(PColgp_HArray2OfPnt)& aPoles,
   const Handle(PColStd_HArray2OfReal)& aWeights,
   const Handle(PColStd_HArray1OfReal)& aUKnots,
   const Handle(PColStd_HArray1OfReal)& aVKnots,
   const Handle(PColStd_HArray1OfInteger)& aUMultiplicities,
   const Handle(PColStd_HArray1OfInteger)& aVMultiplicities) :
   uRational(aURational),
   vRational(aVRational),
   uPeriodic(aUPeriodic),
   vPeriodic(aVPeriodic),
   uSpineDegree(aUSpineDegree),
   vSpineDegree(aVSpineDegree),
   poles(aPoles),
   weights(aWeights),
   uKnots(aUKnots),
   vKnots(aVKnots),
   uMultiplicities(aUMultiplicities),
   vMultiplicities(aVMultiplicities)
{}


//=======================================================================
//function : URational
//purpose  : 
//=======================================================================

void  PGeom_BSplineSurface::URational(const Standard_Boolean aURational)
{ uRational = aURational; }


//=======================================================================
//function : URational
//purpose  : 
//=======================================================================

Standard_Boolean  PGeom_BSplineSurface::URational() const 
{ return uRational; }


//=======================================================================
//function : VRational
//purpose  : 
//=======================================================================

void  PGeom_BSplineSurface::VRational(const Standard_Boolean aVRational)
{ vRational = aVRational; }


//=======================================================================
//function : VRational
//purpose  : 
//=======================================================================

Standard_Boolean  PGeom_BSplineSurface::VRational() const 
{ return vRational; }


//=======================================================================
//function : UPeriodic
//purpose  : 
//=======================================================================

void  PGeom_BSplineSurface::UPeriodic(const Standard_Boolean aUPeriodic)
{ uPeriodic = aUPeriodic; }


//=======================================================================
//function : UPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean  PGeom_BSplineSurface::UPeriodic() const 
{ return uPeriodic; }


//=======================================================================
//function : VPeriodic
//purpose  : 
//=======================================================================

void  PGeom_BSplineSurface::VPeriodic(const Standard_Boolean aVPeriodic)
{ vPeriodic = aVPeriodic; }


//=======================================================================
//function : VPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean  PGeom_BSplineSurface::VPeriodic() const 
{ return vPeriodic; }


//=======================================================================
//function : USpineDegree
//purpose  : 
//=======================================================================

void  PGeom_BSplineSurface::USpineDegree(const Standard_Integer aUSpineDegree)
{ uSpineDegree = aUSpineDegree; }


//=======================================================================
//function : USpineDegree
//purpose  : 
//=======================================================================

Standard_Integer  PGeom_BSplineSurface::USpineDegree() const 
{ return uSpineDegree; }


//=======================================================================
//function : VSpineDegree
//purpose  : 
//=======================================================================

void  PGeom_BSplineSurface::VSpineDegree(const Standard_Integer aVSpineDegree)
{ vSpineDegree = aVSpineDegree; }


//=======================================================================
//function : VSpineDegree
//purpose  : 
//=======================================================================

Standard_Integer  PGeom_BSplineSurface::VSpineDegree() const 
{ return vSpineDegree; }


//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void  PGeom_BSplineSurface::Poles(const Handle(PColgp_HArray2OfPnt)& aPoles)
{ poles = aPoles; }


//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

Handle(PColgp_HArray2OfPnt)  PGeom_BSplineSurface::Poles() const 
{ return poles; }


//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

void  PGeom_BSplineSurface::
  Weights(const Handle(PColStd_HArray2OfReal)& aWeights)
{ weights = aWeights; }


//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

Handle(PColStd_HArray2OfReal)  PGeom_BSplineSurface::Weights() const 
{ return weights; }


//=======================================================================
//function : UKnots
//purpose  : 
//=======================================================================

void  PGeom_BSplineSurface::
  UKnots(const Handle(PColStd_HArray1OfReal)& aUKnots)
{ uKnots = aUKnots; }


//=======================================================================
//function : UKnots
//purpose  : 
//=======================================================================

Handle(PColStd_HArray1OfReal)  PGeom_BSplineSurface::UKnots() const 
{ return uKnots; }


//=======================================================================
//function : VKnots
//purpose  : 
//=======================================================================

void  PGeom_BSplineSurface::
  VKnots(const Handle(PColStd_HArray1OfReal)& aVKnots)
{ vKnots = aVKnots; }


//=======================================================================
//function : VKnots
//purpose  : 
//=======================================================================

Handle(PColStd_HArray1OfReal)  PGeom_BSplineSurface::VKnots() const 
{ return vKnots; }


//=======================================================================
//function : UMultiplicities
//purpose  : 
//=======================================================================

void  PGeom_BSplineSurface::UMultiplicities
  (const Handle(PColStd_HArray1OfInteger)& aUMultiplicities)
{ uMultiplicities = aUMultiplicities; }


//=======================================================================
//function : UMultiplicities
//purpose  : 
//=======================================================================

Handle(PColStd_HArray1OfInteger)  PGeom_BSplineSurface::
       UMultiplicities() const 
{ return uMultiplicities; }


//=======================================================================
//function : VMultiplicities
//purpose  : 
//=======================================================================

void  PGeom_BSplineSurface::
  VMultiplicities(const Handle(PColStd_HArray1OfInteger)& aVMultiplicities)
{ vMultiplicities = aVMultiplicities; }


//=======================================================================
//function : VMultiplicities
//purpose  : 
//=======================================================================

Handle(PColStd_HArray1OfInteger)  PGeom_BSplineSurface::
       VMultiplicities() const 
{ return vMultiplicities; }
