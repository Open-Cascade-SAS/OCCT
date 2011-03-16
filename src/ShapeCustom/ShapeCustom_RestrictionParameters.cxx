// File:	ShapeCustom_RestrictionParameters.cxx
// Created:	Mon May 22 17:01:50 2000
// Author:	data exchange team
//		<det@friendox>


#include <ShapeCustom_RestrictionParameters.ixx>

//=======================================================================
//function : ShapeCustom_RestrictionParameters
//purpose  : 
//=======================================================================

ShapeCustom_RestrictionParameters::ShapeCustom_RestrictionParameters()
{
  myGMaxSeg = 15;
  myGMaxDegree = 10000;
  
  myConvPlane         = Standard_False;
  //myConvElementarySurf = Standard_False;
  //conversion of elementary surfaces are off by default
  myConvConicalSurf = Standard_False;
  myConvSphericalSurf = Standard_False;
  myConvCylindricalSurf = Standard_False;
  myConvToroidalSurf = Standard_False;
  
  myConvBezierSurf    = Standard_False;
  myConvRevolSurf     = Standard_True;
  myConvExtrSurf      = Standard_True;
  myConvOffsetSurf    = Standard_True;
  mySegmentSurfaceMode= Standard_True;
  myConvCurve3d       = Standard_True;
  myConvOffsetCurv3d  = Standard_True;
  myConvCurve2d       = Standard_True;
  myConvOffsetCurv2d  = Standard_True;
}

