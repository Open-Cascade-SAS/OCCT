// File:	PGeom_RectangularTrimmedSurface.cxx
// Created:	Thu Mar  4 10:12:26 1993
// Author:	Philippe DAUTRY
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993


#include <PGeom_RectangularTrimmedSurface.ixx>

//=======================================================================
//function : PGeom_RectangularTrimmedSurface
//purpose  : 
//=======================================================================

PGeom_RectangularTrimmedSurface::PGeom_RectangularTrimmedSurface()
{}


//=======================================================================
//function : PGeom_RectangularTrimmedSurface
//purpose  : 
//=======================================================================

PGeom_RectangularTrimmedSurface::PGeom_RectangularTrimmedSurface
  (const Handle(PGeom_Surface)& aBasisSurface,
   const Standard_Real aFirstU,
   const Standard_Real aLastU,
   const Standard_Real aFirstV,
   const Standard_Real aLastV) :
  basisSurface(aBasisSurface),
  firstU(aFirstU),
  lastU(aLastU),
  firstV(aFirstV),
  lastV(aLastV)
{}


//=======================================================================
//function : BasisSurface
//purpose  : 
//=======================================================================

void  PGeom_RectangularTrimmedSurface::BasisSurface
  (const Handle(PGeom_Surface)& aBasisSurface)
{ basisSurface = aBasisSurface;}


//=======================================================================
//function : BasisSurface
//purpose  : 
//=======================================================================

Handle(PGeom_Surface)  PGeom_RectangularTrimmedSurface::BasisSurface() const 
{ return basisSurface; }


//=======================================================================
//function : FirstU
//purpose  : 
//=======================================================================

void  PGeom_RectangularTrimmedSurface::FirstU(const Standard_Real aFirstU)
{ firstU = aFirstU; }


//=======================================================================
//function : FirstU
//purpose  : 
//=======================================================================

Standard_Real  PGeom_RectangularTrimmedSurface::FirstU() const 
{ return firstU; }


//=======================================================================
//function : LastU
//purpose  : 
//=======================================================================

void  PGeom_RectangularTrimmedSurface::LastU(const Standard_Real aLastU)
{ lastU = aLastU; }


//=======================================================================
//function : LastU
//purpose  : 
//=======================================================================

Standard_Real  PGeom_RectangularTrimmedSurface::LastU() const 
{ return lastU; }


//=======================================================================
//function : FirstV
//purpose  : 
//=======================================================================

void  PGeom_RectangularTrimmedSurface::FirstV(const Standard_Real aFirstV)
{ firstV = aFirstV; }


//=======================================================================
//function : FirstV
//purpose  : 
//=======================================================================

Standard_Real  PGeom_RectangularTrimmedSurface::FirstV() const 
{ return firstV; }


//=======================================================================
//function : LastV
//purpose  : 
//=======================================================================

void  PGeom_RectangularTrimmedSurface::LastV(const Standard_Real aLastV)
{ lastV = aLastV; }


//=======================================================================
//function : LastV
//purpose  : 
//=======================================================================

Standard_Real  PGeom_RectangularTrimmedSurface::LastV() const 
{ return lastV; }
