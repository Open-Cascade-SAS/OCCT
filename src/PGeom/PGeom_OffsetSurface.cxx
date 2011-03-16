// File:	PGeom_OffsetSurface.cxx
// Created:	Thu Mar  4 10:05:18 1993
// Author:	Philippe DAUTRY
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993


#include <PGeom_OffsetSurface.ixx>

//=======================================================================
//function : PGeom_OffsetSurface
//purpose  : 
//=======================================================================

PGeom_OffsetSurface::PGeom_OffsetSurface()
{}


//=======================================================================
//function : PGeom_OffsetSurface
//purpose  : 
//=======================================================================

PGeom_OffsetSurface::PGeom_OffsetSurface
  (const Handle(PGeom_Surface)& aBasisSurface,
   const Standard_Real aOffsetValue) :
  basisSurface(aBasisSurface),
  offsetValue(aOffsetValue)
{}


//=======================================================================
//function : BasisSurface
//purpose  : 
//=======================================================================

void  PGeom_OffsetSurface::BasisSurface
  (const Handle(PGeom_Surface)& aBasisSurface)
{ basisSurface = aBasisSurface; }


//=======================================================================
//function : BasisSurface
//purpose  : 
//=======================================================================

Handle(PGeom_Surface)  PGeom_OffsetSurface::BasisSurface() const 
{ return basisSurface; }


//=======================================================================
//function : OffsetValue
//purpose  : 
//=======================================================================

void  PGeom_OffsetSurface::OffsetValue(const Standard_Real aOffsetValue)
{ offsetValue = aOffsetValue; }


//=======================================================================
//function : OffsetValue
//purpose  : 
//=======================================================================

Standard_Real  PGeom_OffsetSurface::OffsetValue() const 
{ return offsetValue; }
