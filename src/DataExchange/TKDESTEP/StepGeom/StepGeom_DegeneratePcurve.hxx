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

#ifndef _StepGeom_DegeneratePcurve_HeaderFile
#define _StepGeom_DegeneratePcurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_Point.hxx>
class StepGeom_Surface;
class StepRepr_DefinitionalRepresentation;
class TCollection_HAsciiString;

class StepGeom_DegeneratePcurve;
DEFINE_STANDARD_HANDLE(StepGeom_DegeneratePcurve, StepGeom_Point)

class StepGeom_DegeneratePcurve : public StepGeom_Point
{

public:
  //! Returns a DegeneratePcurve
  Standard_EXPORT StepGeom_DegeneratePcurve();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&            aName,
                            const Handle(StepGeom_Surface)&                    aBasisSurface,
                            const Handle(StepRepr_DefinitionalRepresentation)& aReferenceToCurve);

  Standard_EXPORT void SetBasisSurface(const Handle(StepGeom_Surface)& aBasisSurface);

  Standard_EXPORT Handle(StepGeom_Surface) BasisSurface() const;

  Standard_EXPORT void SetReferenceToCurve(
    const Handle(StepRepr_DefinitionalRepresentation)& aReferenceToCurve);

  Standard_EXPORT Handle(StepRepr_DefinitionalRepresentation) ReferenceToCurve() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_DegeneratePcurve, StepGeom_Point)

protected:
private:
  Handle(StepGeom_Surface)                    basisSurface;
  Handle(StepRepr_DefinitionalRepresentation) referenceToCurve;
};

#endif // _StepGeom_DegeneratePcurve_HeaderFile
