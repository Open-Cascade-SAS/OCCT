// File:	PGeom_SurfaceOfLinearExtrusion.cxx
// Created:	Thu Mar  4 10:40:52 1993
// Author:	Philippe DAUTRY
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993


#include <PGeom_SurfaceOfLinearExtrusion.ixx>

//=======================================================================
//function : PGeom_SurfaceOfLinearExtrusion
//purpose  : 
//=======================================================================

PGeom_SurfaceOfLinearExtrusion::PGeom_SurfaceOfLinearExtrusion()
{}


//=======================================================================
//function : PGeom_SurfaceOfLinearExtrusion
//purpose  : 
//=======================================================================

PGeom_SurfaceOfLinearExtrusion::PGeom_SurfaceOfLinearExtrusion
  (const Handle(PGeom_Curve)& aBasisCurve,
   const gp_Dir& aDirection) :
  PGeom_SweptSurface(aBasisCurve, aDirection)
{}
