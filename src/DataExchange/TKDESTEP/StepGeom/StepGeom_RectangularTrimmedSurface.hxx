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

#ifndef _StepGeom_RectangularTrimmedSurface_HeaderFile
#define _StepGeom_RectangularTrimmedSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <StepGeom_BoundedSurface.hxx>
class StepGeom_Surface;
class TCollection_HAsciiString;

class StepGeom_RectangularTrimmedSurface : public StepGeom_BoundedSurface
{

public:
  //! Returns a RectangularTrimmedSurface
  Standard_EXPORT StepGeom_RectangularTrimmedSurface();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aName,
                            const occ::handle<StepGeom_Surface>&         aBasisSurface,
                            const double                     aU1,
                            const double                     aU2,
                            const double                     aV1,
                            const double                     aV2,
                            const bool                  aUsense,
                            const bool                  aVsense);

  Standard_EXPORT void SetBasisSurface(const occ::handle<StepGeom_Surface>& aBasisSurface);

  Standard_EXPORT occ::handle<StepGeom_Surface> BasisSurface() const;

  Standard_EXPORT void SetU1(const double aU1);

  Standard_EXPORT double U1() const;

  Standard_EXPORT void SetU2(const double aU2);

  Standard_EXPORT double U2() const;

  Standard_EXPORT void SetV1(const double aV1);

  Standard_EXPORT double V1() const;

  Standard_EXPORT void SetV2(const double aV2);

  Standard_EXPORT double V2() const;

  Standard_EXPORT void SetUsense(const bool aUsense);

  Standard_EXPORT bool Usense() const;

  Standard_EXPORT void SetVsense(const bool aVsense);

  Standard_EXPORT bool Vsense() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_RectangularTrimmedSurface, StepGeom_BoundedSurface)

private:
  occ::handle<StepGeom_Surface> basisSurface;
  double            u1;
  double            u2;
  double            v1;
  double            v2;
  bool         usense;
  bool         vsense;
};

#endif // _StepGeom_RectangularTrimmedSurface_HeaderFile
