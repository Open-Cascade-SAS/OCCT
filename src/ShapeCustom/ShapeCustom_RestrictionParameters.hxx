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

#ifndef _ShapeCustom_RestrictionParameters_HeaderFile
#define _ShapeCustom_RestrictionParameters_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>


class ShapeCustom_RestrictionParameters;
DEFINE_STANDARD_HANDLE(ShapeCustom_RestrictionParameters, Standard_Transient)

//! This class is axuluary tool which contains parameters for
//! BSplineRestriction class.
class ShapeCustom_RestrictionParameters : public Standard_Transient
{

public:

  
  //! Sets default parameters.
  Standard_EXPORT ShapeCustom_RestrictionParameters();
  
  //! Returns (modifiable) maximal degree of approximation.
    Standard_Integer& GMaxDegree();
  
  //! Returns (modifiable) maximal number of spans of
  //! approximation.
    Standard_Integer& GMaxSeg();
  
  //! Sets flag for define if Plane converted to BSpline surface.
    Standard_Boolean& ConvertPlane();
  
  //! Sets flag for define if Bezier surface converted to BSpline
  //! surface.
    Standard_Boolean& ConvertBezierSurf();
  
  //! Sets flag for define if surface of Revolution converted to
  //! BSpline surface.
    Standard_Boolean& ConvertRevolutionSurf();
  
  //! Sets flag for define if surface of LinearExtrusion converted
  //! to BSpline surface.
    Standard_Boolean& ConvertExtrusionSurf();
  
  //! Sets flag for define if Offset surface converted to BSpline
  //! surface.
    Standard_Boolean& ConvertOffsetSurf();
  
  //! Sets flag for define if cylindrical surface converted to BSpline
  //! surface.
    Standard_Boolean& ConvertCylindricalSurf();
  
  //! Sets flag for define if conical surface converted to BSpline
  //! surface.
    Standard_Boolean& ConvertConicalSurf();
  
  //! Sets flag for define if toroidal surface converted to BSpline
  //! surface.
    Standard_Boolean& ConvertToroidalSurf();
  
  //! Sets flag for define if spherical surface converted to BSpline
  //! surface.
    Standard_Boolean& ConvertSphericalSurf();
  
  //! Sets Segment mode for surface. If Segment is True surface is
  //! approximated in the bondaries of face lying on this surface.
    Standard_Boolean& SegmentSurfaceMode();
  
  //! Sets flag for define if 3d curve converted to BSpline curve.
    Standard_Boolean& ConvertCurve3d();
  
  //! Sets flag for define if Offset curve3d converted to BSpline
  //! surface.
    Standard_Boolean& ConvertOffsetCurv3d();
  
  //! Returns (modifiable) flag for define if 2d curve converted
  //! to BSpline curve.
    Standard_Boolean& ConvertCurve2d();
  
  //! Returns (modifiable) flag for define if Offset curve2d
  //! converted to BSpline surface.
    Standard_Boolean& ConvertOffsetCurv2d();




  DEFINE_STANDARD_RTTIEXT(ShapeCustom_RestrictionParameters,Standard_Transient)

protected:




private:


  Standard_Integer myGMaxDegree;
  Standard_Integer myGMaxSeg;
  Standard_Boolean myConvPlane;
  Standard_Boolean myConvConicalSurf;
  Standard_Boolean myConvSphericalSurf;
  Standard_Boolean myConvCylindricalSurf;
  Standard_Boolean myConvToroidalSurf;
  Standard_Boolean myConvBezierSurf;
  Standard_Boolean myConvRevolSurf;
  Standard_Boolean myConvExtrSurf;
  Standard_Boolean myConvOffsetSurf;
  Standard_Boolean mySegmentSurfaceMode;
  Standard_Boolean myConvCurve3d;
  Standard_Boolean myConvOffsetCurv3d;
  Standard_Boolean myConvCurve2d;
  Standard_Boolean myConvOffsetCurv2d;


};


#include <ShapeCustom_RestrictionParameters.lxx>





#endif // _ShapeCustom_RestrictionParameters_HeaderFile
