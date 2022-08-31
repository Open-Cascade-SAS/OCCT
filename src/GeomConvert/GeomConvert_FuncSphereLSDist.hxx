
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

#ifndef _GeomConvert_FuncSphereLSDist_HeaderFile
#define _GeomConvert_FuncSphereLSDist_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <math_MultipleVarFunctionWithGradient.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <math_Vector.hxx>

//! Function for search of sphere canonic parameters: coordinates of center and radius from set of moints
//! by least square method.
//! //!
//! The class inherits math_MultipleVarFunctionWithGradient and thus is intended
//! for use in math_BFGS algorithm.
//!
//! The criteria is:
//! F(x0, y0, z0, R) = Sum[(x(i) - x0)^2 + (y(i) - y0)^2 + (z(i) - z0)^2 - R^2]^2 => min,
//! x(i), y(i), z(i) - coordinates of sample points, x0, y0, z0, R - coordinates of center and radius of sphere,
//! which must be defined
//!
//! The first derivative are:
//! dF/dx0 : G1(x0, y0, z0, R) = -4*Sum{[...]*(x(i) - x0)} 
//! dF/dy0 : G2(x0, y0, z0, R) = -4*Sum{[...]*(y(i) - y0)}
//! dF/dz0 : G3(x0, y0, z0, R) = -4*Sum{[...]*(z(i) - z0)}
//! dF/dR : G4(x0, y0, z0, R) = -4*R*Sum[...]
//! [...] = [(x(i) - x0)^2 + (y(i) - y0)^2 + (z(i) - z0)^2 - R^2]
//!
class GeomConvert_FuncSphereLSDist : public math_MultipleVarFunctionWithGradient
{
public:

  DEFINE_STANDARD_ALLOC

  //! Constructor.
  Standard_EXPORT GeomConvert_FuncSphereLSDist() {};
  
  Standard_EXPORT GeomConvert_FuncSphereLSDist(const Handle(TColgp_HArray1OfXYZ)& thePoints);

  void SetPoints(const Handle(TColgp_HArray1OfXYZ)& thePoints)
  {
    myPoints = thePoints;
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
  
};
#endif // _GeomConvert_FuncSphereLSDist_HeaderFile
