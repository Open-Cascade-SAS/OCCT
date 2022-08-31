// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2022 OPEN CASCADE SAS
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

#ifndef _GeomConvert_FuncCylinderLSDist_HeaderFile
#define _GeomConvert_FuncCylinderLSDist_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <math_MultipleVarFunctionWithGradient.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <math_Vector.hxx>
#include <gp_Dir.hxx>

//! Function for search of cylinder canonic parameters: coordinates of center local coordinate system, 
//! direction of axis and radius from set of points
//! by least square method.
//! 
//! The class inherits math_MultipleVarFunctionWithGradient and thus is intended
//! for use in math_BFGS algorithm.
//!
//! Parametrisation:
//! Cylinder is defined by its axis and radius. Axis is defined by 3 cartesian coordinats it location x0, y0, z0 
//! and direction, which is constant and set by user: 
//! dir.x, dir.y, dir.z
//! The criteria is:
//! F(x0, y0, z0, theta, phi, R) = Sum[|(P(i) - Loc)^dir|^2 - R^2]^2 => min
//! P(i) is i-th sample point, Loc, dir - axis location and direction, R - radius
//!
//! The square vector product |(P(i) - Loc)^dir|^2 is:
//!
//! [(y - y0)*dir.z - (z - z0)*dir.y]^2 +
//! [(z - z0)*dir.x - (x - x0)*dir.z]^2 +
//! [(x - x0)*dir.y - (y - y0)*dir.x]^2
//!
//! First derivative of square vector product are:
//! Dx0 =  2*[(z - z0)*dir.x - (x - x0)*dir.z]*dir.z 
//!       -2*[(x - x0)*dir.y - (y - y0)*dir.x]*dir.y
//! Dy0 = -2*[(y - y0)*dir.z - (z - z0)*dir.y]*dir.z
//!       +2*[(x - x0)*dir.y - (y - y0)*dir.x]*dir.x
//! Dz0 =  2*[(y - y0)*dir.z - (z - z0)*dir.y]*dir.y
//!       -2*[(z - z0)*dir.x - (x - x0)*dir.z]*dir.x
//!
//! dF/dx0 : G1(...) = 2*Sum{[...]*Dx0}
//! dF/dy0 : G2(...) = 2*Sum{[...]*Dy0}
//! dF/dz0 : G3(...) = 2*Sum{[...]*Dz0}
//! dF/dR : G4(...) = -4*R*Sum[...]
//! [...] = [|(P(i) - Loc)^dir|^2 - R^2]
class GeomConvert_FuncCylinderLSDist : public math_MultipleVarFunctionWithGradient
{
public:

  DEFINE_STANDARD_ALLOC

  //! Constructor.
  Standard_EXPORT GeomConvert_FuncCylinderLSDist() {};
  
  Standard_EXPORT GeomConvert_FuncCylinderLSDist(const Handle(TColgp_HArray1OfXYZ)& thePoints,
                                                   const gp_Dir& theDir);

  void SetPoints(const Handle(TColgp_HArray1OfXYZ)& thePoints)
  {
    myPoints = thePoints;
  }

  void SetDir(const gp_Dir& theDir)
  {
    myDir = theDir;
  }

  //! Number of variables.
  Standard_EXPORT Standard_Integer NbVariables() const Standard_OVERRIDE;

  //! Value.
  Standard_EXPORT Standard_Boolean Value(const math_Vector& X,Standard_Real& F) Standard_OVERRIDE;

  //! Gradient.
  Standard_EXPORT Standard_Boolean Gradient(const math_Vector& X,math_Vector& G) Standard_OVERRIDE;

  //! Value and gradient.
  Standard_EXPORT Standard_Boolean Values(const math_Vector& X,Standard_Real& F,math_Vector& G) Standard_OVERRIDE;

private:

  Handle(TColgp_HArray1OfXYZ) myPoints;
  gp_Dir myDir;
  
};
#endif // _GeomConvert_FuncCylinderLSDist_HeaderFile
