// Created on: 2000-06-07
// Created by: Galina KULIKOVA
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

#ifndef _ShapeUpgrade_FixSmallBezierCurves_HeaderFile
#define _ShapeUpgrade_FixSmallBezierCurves_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <ShapeUpgrade_FixSmallCurves.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class Geom_Curve;
class Geom2d_Curve;


class ShapeUpgrade_FixSmallBezierCurves;
DEFINE_STANDARD_HANDLE(ShapeUpgrade_FixSmallBezierCurves, ShapeUpgrade_FixSmallCurves)


class ShapeUpgrade_FixSmallBezierCurves : public ShapeUpgrade_FixSmallCurves
{

public:

  
  Standard_EXPORT ShapeUpgrade_FixSmallBezierCurves();
  
  Standard_EXPORT virtual Standard_Boolean Approx (Handle(Geom_Curve)& Curve3d, Handle(Geom2d_Curve)& Curve2d, Handle(Geom2d_Curve)& Curve2dR, Standard_Real& First, Standard_Real& Last) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_FixSmallBezierCurves,ShapeUpgrade_FixSmallCurves)

protected:




private:




};







#endif // _ShapeUpgrade_FixSmallBezierCurves_HeaderFile
