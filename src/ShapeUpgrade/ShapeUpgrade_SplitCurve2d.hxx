// Created on: 1998-03-12
// Created by: Pierre BARRAS
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _ShapeUpgrade_SplitCurve2d_HeaderFile
#define _ShapeUpgrade_SplitCurve2d_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColGeom2d_HArray1OfCurve.hxx>
#include <ShapeUpgrade_SplitCurve.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class Geom2d_Curve;


class ShapeUpgrade_SplitCurve2d;
DEFINE_STANDARD_HANDLE(ShapeUpgrade_SplitCurve2d, ShapeUpgrade_SplitCurve)

//! Splits a 2d curve with a criterion.
class ShapeUpgrade_SplitCurve2d : public ShapeUpgrade_SplitCurve
{

public:

  
  //! Empty constructor.
  Standard_EXPORT ShapeUpgrade_SplitCurve2d();
  
  //! Initializes with pcurve with its first and last parameters.
  Standard_EXPORT void Init (const Handle(Geom2d_Curve)& C);
  
  //! Initializes with pcurve with its parameters.
  Standard_EXPORT void Init (const Handle(Geom2d_Curve)& C, const Standard_Real First, const Standard_Real Last);
  
  //! If Segment is True, the result is composed with
  //! segments of the curve bounded by the SplitValues.  If
  //! Segment is False, the result is composed with trimmed
  //! Curves all based on the same complete curve.
  Standard_EXPORT virtual void Build (const Standard_Boolean Segment) Standard_OVERRIDE;
  
  Standard_EXPORT const Handle(TColGeom2d_HArray1OfCurve)& GetCurves() const;




  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_SplitCurve2d,ShapeUpgrade_SplitCurve)

protected:


  Handle(Geom2d_Curve) myCurve;
  Handle(TColGeom2d_HArray1OfCurve) myResultingCurves;


private:




};







#endif // _ShapeUpgrade_SplitCurve2d_HeaderFile
