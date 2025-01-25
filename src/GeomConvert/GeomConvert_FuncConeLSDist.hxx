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

#ifndef _GeomConvert_FuncConeLSDist_HeaderFile
#define _GeomConvert_FuncConeLSDist_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <math_MultipleVarFunction.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <math_Vector.hxx>
#include <gp_Dir.hxx>

//! Function for search of Cone canonic parameters: coordinates of center local coordinate system,
//! direction of axis, radius and semi-angle from set of points
//! by least square method.
//!
//!
class GeomConvert_FuncConeLSDist : public math_MultipleVarFunction
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor.
  Standard_EXPORT GeomConvert_FuncConeLSDist() {};

  Standard_EXPORT GeomConvert_FuncConeLSDist(const Handle(TColgp_HArray1OfXYZ)& thePoints,
                                             const gp_Dir&                      theDir);

  void SetPoints(const Handle(TColgp_HArray1OfXYZ)& thePoints) { myPoints = thePoints; }

  void SetDir(const gp_Dir& theDir) { myDir = theDir; }

  //! Number of variables.
  Standard_EXPORT Standard_Integer NbVariables() const Standard_OVERRIDE;

  //! Value.
  Standard_EXPORT Standard_Boolean Value(const math_Vector& X, Standard_Real& F) Standard_OVERRIDE;

private:
  Handle(TColgp_HArray1OfXYZ) myPoints;
  gp_Dir                      myDir;
};
#endif // _GeomConvert_FuncConeLSDist_HeaderFile
