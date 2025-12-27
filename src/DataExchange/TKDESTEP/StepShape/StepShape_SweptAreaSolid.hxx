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

#ifndef _StepShape_SweptAreaSolid_HeaderFile
#define _StepShape_SweptAreaSolid_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_SolidModel.hxx>
class StepGeom_CurveBoundedSurface;
class TCollection_HAsciiString;

class StepShape_SweptAreaSolid : public StepShape_SolidModel
{

public:
  //! Returns a SweptAreaSolid
  Standard_EXPORT StepShape_SweptAreaSolid();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&     aName,
                            const occ::handle<StepGeom_CurveBoundedSurface>& aSweptArea);

  Standard_EXPORT void SetSweptArea(const occ::handle<StepGeom_CurveBoundedSurface>& aSweptArea);

  Standard_EXPORT occ::handle<StepGeom_CurveBoundedSurface> SweptArea() const;

  DEFINE_STANDARD_RTTIEXT(StepShape_SweptAreaSolid, StepShape_SolidModel)

private:
  occ::handle<StepGeom_CurveBoundedSurface> sweptArea;
};

#endif // _StepShape_SweptAreaSolid_HeaderFile
