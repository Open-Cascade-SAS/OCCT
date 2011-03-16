// File:	PGeom_BezierSurface.cxx
// Created:	Wed Mar  3 16:26:50 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom_BezierSurface.ixx>

//=======================================================================
//function : PGeom_BezierSurface
//purpose  : 
//=======================================================================

PGeom_BezierSurface::PGeom_BezierSurface()
{}


//=======================================================================
//function : PGeom_BezierSurface
//purpose  : 
//=======================================================================

PGeom_BezierSurface::PGeom_BezierSurface
  (const Standard_Boolean aURational,
   const Standard_Boolean aVRational,
   const Handle(PColgp_HArray2OfPnt)& aPoles,
   const Handle(PColStd_HArray2OfReal)& aWeights) :
  uRational(aURational),
  vRational(aVRational),
  poles(aPoles),
  weights(aWeights)
{}


//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void  PGeom_BezierSurface::Poles(const Handle(PColgp_HArray2OfPnt)& aPoles)
{ poles = aPoles; }


//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

Handle(PColgp_HArray2OfPnt)  PGeom_BezierSurface::Poles() const 
{ return poles; }


//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

void  PGeom_BezierSurface::Weights(const Handle(PColStd_HArray2OfReal)& aWeights)
{ weights = aWeights; }


//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

Handle(PColStd_HArray2OfReal)  PGeom_BezierSurface::Weights() const 
{ return weights; }


//=======================================================================
//function : URational
//purpose  : 
//=======================================================================

void  PGeom_BezierSurface::URational(const Standard_Boolean aURational)
{ uRational = aURational; }


//=======================================================================
//function : URational
//purpose  : 
//=======================================================================

Standard_Boolean  PGeom_BezierSurface::URational() const 
{ return uRational; }


//=======================================================================
//function : VRational
//purpose  : 
//=======================================================================

void  PGeom_BezierSurface::VRational(const Standard_Boolean aVRational)
{ vRational = aVRational; }


//=======================================================================
//function : VRational
//purpose  : 
//=======================================================================

Standard_Boolean  PGeom_BezierSurface::VRational() const 
{ return vRational; }
