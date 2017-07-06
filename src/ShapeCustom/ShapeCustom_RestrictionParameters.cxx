// Created on: 2000-05-22
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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


#include <ShapeCustom_RestrictionParameters.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeCustom_RestrictionParameters,Standard_Transient)

//=======================================================================
//function : ShapeCustom_RestrictionParameters
//purpose  : 
//=======================================================================
ShapeCustom_RestrictionParameters::ShapeCustom_RestrictionParameters()
{
  myGMaxSeg = 10000;
  myGMaxDegree = 15;
  
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

