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

#ifndef _StepGeom_RectangularCompositeSurface_HeaderFile
#define _StepGeom_RectangularCompositeSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_HArray2OfSurfacePatch.hxx>
#include <StepGeom_BoundedSurface.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepGeom_SurfacePatch;

class StepGeom_RectangularCompositeSurface;
DEFINE_STANDARD_HANDLE(StepGeom_RectangularCompositeSurface, StepGeom_BoundedSurface)

class StepGeom_RectangularCompositeSurface : public StepGeom_BoundedSurface
{

public:
  //! Returns a RectangularCompositeSurface
  Standard_EXPORT StepGeom_RectangularCompositeSurface();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&       aName,
                            const Handle(StepGeom_HArray2OfSurfacePatch)& aSegments);

  Standard_EXPORT void SetSegments(const Handle(StepGeom_HArray2OfSurfacePatch)& aSegments);

  Standard_EXPORT Handle(StepGeom_HArray2OfSurfacePatch) Segments() const;

  Standard_EXPORT Handle(StepGeom_SurfacePatch) SegmentsValue(const Standard_Integer num1,
                                                              const Standard_Integer num2) const;

  Standard_EXPORT Standard_Integer NbSegmentsI() const;

  Standard_EXPORT Standard_Integer NbSegmentsJ() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_RectangularCompositeSurface, StepGeom_BoundedSurface)

protected:
private:
  Handle(StepGeom_HArray2OfSurfacePatch) segments;
};

#endif // _StepGeom_RectangularCompositeSurface_HeaderFile
