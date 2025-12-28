// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepElement_SurfaceSectionFieldVarying_HeaderFile
#define _StepElement_SurfaceSectionFieldVarying_HeaderFile

#include <Standard.hxx>

#include <StepElement_SurfaceSection.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Boolean.hxx>
#include <StepElement_SurfaceSectionField.hxx>

//! Representation of STEP entity SurfaceSectionFieldVarying
class StepElement_SurfaceSectionFieldVarying : public StepElement_SurfaceSectionField
{

public:
  //! Empty constructor
  Standard_EXPORT StepElement_SurfaceSectionFieldVarying();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<NCollection_HArray1<occ::handle<StepElement_SurfaceSection>>>& aDefinitions,
    const bool aAdditionalNodeValues);

  //! Returns field Definitions
  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<StepElement_SurfaceSection>>>
                  Definitions() const;

  //! Set field Definitions
  Standard_EXPORT void SetDefinitions(
    const occ::handle<NCollection_HArray1<occ::handle<StepElement_SurfaceSection>>>& Definitions);

  //! Returns field AdditionalNodeValues
  Standard_EXPORT bool AdditionalNodeValues() const;

  //! Set field AdditionalNodeValues
  Standard_EXPORT void SetAdditionalNodeValues(const bool AdditionalNodeValues);

  DEFINE_STANDARD_RTTIEXT(StepElement_SurfaceSectionFieldVarying, StepElement_SurfaceSectionField)

private:
  occ::handle<NCollection_HArray1<occ::handle<StepElement_SurfaceSection>>> theDefinitions;
  bool                                                                      theAdditionalNodeValues;
};

#endif // _StepElement_SurfaceSectionFieldVarying_HeaderFile
