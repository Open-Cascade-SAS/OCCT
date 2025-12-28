// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepGeom_OffsetSurface_HeaderFile
#define _StepGeom_OffsetSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <StepData_Logical.hxx>
#include <StepGeom_Surface.hxx>
class TCollection_HAsciiString;

class StepGeom_OffsetSurface : public StepGeom_Surface
{

public:
  //! Returns a OffsetSurface
  Standard_EXPORT StepGeom_OffsetSurface();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aName,
                            const occ::handle<StepGeom_Surface>&         aBasisSurface,
                            const double                                 aDistance,
                            const StepData_Logical                       aSelfIntersect);

  Standard_EXPORT void SetBasisSurface(const occ::handle<StepGeom_Surface>& aBasisSurface);

  Standard_EXPORT occ::handle<StepGeom_Surface> BasisSurface() const;

  Standard_EXPORT void SetDistance(const double aDistance);

  Standard_EXPORT double Distance() const;

  Standard_EXPORT void SetSelfIntersect(const StepData_Logical aSelfIntersect);

  Standard_EXPORT StepData_Logical SelfIntersect() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_OffsetSurface, StepGeom_Surface)

private:
  occ::handle<StepGeom_Surface> basisSurface;
  double                        distance;
  StepData_Logical              selfIntersect;
};

#endif // _StepGeom_OffsetSurface_HeaderFile
