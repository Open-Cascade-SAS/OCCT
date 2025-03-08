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

#include <StepElement_HArray1OfSurfaceSection.hxx>
#include <Standard_Boolean.hxx>
#include <StepElement_SurfaceSectionField.hxx>

class StepElement_SurfaceSectionFieldVarying;
DEFINE_STANDARD_HANDLE(StepElement_SurfaceSectionFieldVarying, StepElement_SurfaceSectionField)

//! Representation of STEP entity SurfaceSectionFieldVarying
class StepElement_SurfaceSectionFieldVarying : public StepElement_SurfaceSectionField
{

public:
  //! Empty constructor
  Standard_EXPORT StepElement_SurfaceSectionFieldVarying();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(StepElement_HArray1OfSurfaceSection)& aDefinitions,
                            const Standard_Boolean aAdditionalNodeValues);

  //! Returns field Definitions
  Standard_EXPORT Handle(StepElement_HArray1OfSurfaceSection) Definitions() const;

  //! Set field Definitions
  Standard_EXPORT void SetDefinitions(
    const Handle(StepElement_HArray1OfSurfaceSection)& Definitions);

  //! Returns field AdditionalNodeValues
  Standard_EXPORT Standard_Boolean AdditionalNodeValues() const;

  //! Set field AdditionalNodeValues
  Standard_EXPORT void SetAdditionalNodeValues(const Standard_Boolean AdditionalNodeValues);

  DEFINE_STANDARD_RTTIEXT(StepElement_SurfaceSectionFieldVarying, StepElement_SurfaceSectionField)

protected:
private:
  Handle(StepElement_HArray1OfSurfaceSection) theDefinitions;
  Standard_Boolean                            theAdditionalNodeValues;
};

#endif // _StepElement_SurfaceSectionFieldVarying_HeaderFile
